#include "executor.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cctype>

struct CommandStage {
    std::vector<std::string> args;
    std::string inputFile;
    std::string outputFile;
    bool appendOutput = false;
};

// Internal helper to tokenize command string respecting quotes and operators
static std::vector<std::string> tokenize(const std::string& command) {
    std::vector<std::string> tokens;
    std::string current;
    bool inDoubleQuotes = false;
    bool inSingleQuotes = false;
    
    for (size_t i = 0; i < command.length(); ++i) {
        char c = command[i];
        if (c == '"' && !inSingleQuotes) {
            inDoubleQuotes = !inDoubleQuotes;
            continue;
        } else if (c == '\'' && !inDoubleQuotes) {
            inSingleQuotes = !inSingleQuotes;
            continue;
        }
        
        if (inDoubleQuotes || inSingleQuotes) {
            current += c;
        } else {
            if (std::isspace(static_cast<unsigned char>(c))) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            } else if (c == '|' || c == '<') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                tokens.push_back(std::string(1, c));
            } else if (c == '>') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                if (i + 1 < command.length() && command[i + 1] == '>') {
                    tokens.push_back(">>");
                    ++i;
                } else {
                    tokens.push_back(">");
                }
            } else {
                current += c;
            }
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

// Internal helper to parse redirection from token stream of a stage
static bool parseStage(const std::vector<std::string>& stageTokens, CommandStage& stage) {
    for (size_t i = 0; i < stageTokens.size(); ++i) {
        if (stageTokens[i] == "<") {
            if (i + 1 >= stageTokens.size()) {
                std::cerr << "Syntax error: missing file for input redirection\n";
                return false;
            }
            stage.inputFile = stageTokens[i + 1];
            i++;
        } else if (stageTokens[i] == ">") {
            if (i + 1 >= stageTokens.size()) {
                std::cerr << "Syntax error: missing file for output redirection\n";
                return false;
            }
            stage.outputFile = stageTokens[i + 1];
            stage.appendOutput = false;
            i++;
        } else if (stageTokens[i] == ">>") {
            if (i + 1 >= stageTokens.size()) {
                std::cerr << "Syntax error: missing file for output redirection (append)\n";
                return false;
            }
            stage.outputFile = stageTokens[i + 1];
            stage.appendOutput = true;
            i++;
        } else {
            stage.args.push_back(stageTokens[i]);
        }
    }
    return true;
}

// Internal helper to resolve home path shorthand '~'
static std::string resolvePath(const std::string& path) {
    if (path == "~") {
        const char* home = getenv("HOME");
        return home ? home : "";
    }
    if (path.rfind("~/", 0) == 0) {
        const char* home = getenv("HOME");
        return home ? std::string(home) + path.substr(1) : path;
    }
    return path;
}

CommandExecutor::CommandExecutor() : lastExitCode(0) {}

int CommandExecutor::execute(const std::string& command) {
    std::vector<std::string> tokens = tokenize(command);
    if (tokens.empty()) {
        return 0;
    }
    
    // Split tokens by pipe operators into stages
    std::vector<std::vector<std::string>> stagesTokens;
    std::vector<std::string> currentStageTokens;
    for (const auto& token : tokens) {
        if (token == "|") {
            if (currentStageTokens.empty()) {
                std::cerr << "Syntax error: empty pipe stage\n";
                lastExitCode = 1;
                return lastExitCode;
            }
            stagesTokens.push_back(currentStageTokens);
            currentStageTokens.clear();
        } else {
            currentStageTokens.push_back(token);
        }
    }
    if (!currentStageTokens.empty()) {
        stagesTokens.push_back(currentStageTokens);
    } else {
        std::cerr << "Syntax error: empty pipe stage\n";
        lastExitCode = 1;
        return lastExitCode;
    }
    
    // Parse redirection for each stage
    std::vector<CommandStage> stages(stagesTokens.size());
    for (size_t i = 0; i < stagesTokens.size(); ++i) {
        if (!parseStage(stagesTokens[i], stages[i])) {
            lastExitCode = 1;
            return lastExitCode;
        }
    }
    
    if (stages.empty()) {
        return 0;
    }
    
    // Intercept built-ins in the parent process if running a single command stage
    if (stages.size() == 1) {
        const auto& args = stages[0].args;
        if (!args.empty()) {
            if (args[0] == "cd") {
                std::string path;
                if (args.size() < 2) {
                    const char* home = getenv("HOME");
                    if (home) {
                        path = home;
                    } else {
                        std::cerr << "cd: HOME not set\n";
                        lastExitCode = 1;
                        return lastExitCode;
                    }
                } else {
                    path = resolvePath(args[1]);
                }
                if (chdir(path.c_str()) != 0) {
                    perror("cd failed");
                    lastExitCode = 1;
                } else {
                    lastExitCode = 0;
                }
                return lastExitCode;
            } else if (args[0] == "exit" || args[0] == "quit") {
                int code = 0;
                if (args.size() >= 2) {
                    try {
                        code = std::stoi(args[1]);
                    } catch (...) {
                        code = 0;
                    }
                }
                exit(code);
            }
        }
    }
    
    // Execute multiple stages / single external command
    size_t numStages = stages.size();
    std::vector<pid_t> pids(numStages);
    std::vector<int> pipeFds(2 * (numStages - 1));
    
    for (size_t i = 0; i < numStages - 1; ++i) {
        if (pipe(&pipeFds[2 * i]) < 0) {
            perror("pipe failed");
            lastExitCode = 1;
            return lastExitCode;
        }
    }
    
    for (size_t i = 0; i < numStages; ++i) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork failed");
            for (int fd : pipeFds) {
                close(fd);
            }
            lastExitCode = 1;
            return lastExitCode;
        } else if (pids[i] == 0) {
            // Child process stage i
            
            // Redirect stdin from previous pipe if not first stage
            if (i > 0) {
                if (dup2(pipeFds[2 * (i - 1)], STDIN_FILENO) < 0) {
                    perror("dup2 stdin pipe failed");
                    exit(1);
                }
            }
            
            // Redirect stdout to current pipe if not last stage
            if (i < numStages - 1) {
                if (dup2(pipeFds[2 * i + 1], STDOUT_FILENO) < 0) {
                    perror("dup2 stdout pipe failed");
                    exit(1);
                }
            }
            
            // Close all pipe FDs in child
            for (int fd : pipeFds) {
                close(fd);
            }
            
            // Apply redirection: input
            if (!stages[i].inputFile.empty()) {
                int inFd = open(stages[i].inputFile.c_str(), O_RDONLY);
                if (inFd < 0) {
                    perror(("open " + stages[i].inputFile + " failed").c_str());
                    exit(1);
                }
                if (dup2(inFd, STDIN_FILENO) < 0) {
                    perror("dup2 input file failed");
                    exit(1);
                }
                close(inFd);
            }
            
            // Apply redirection: output
            if (!stages[i].outputFile.empty()) {
                int outFlags = O_WRONLY | O_CREAT;
                if (stages[i].appendOutput) {
                    outFlags |= O_APPEND;
                } else {
                    outFlags |= O_TRUNC;
                }
                int outFd = open(stages[i].outputFile.c_str(), outFlags, 0644);
                if (outFd < 0) {
                    perror(("open " + stages[i].outputFile + " failed").c_str());
                    exit(1);
                }
                if (dup2(outFd, STDOUT_FILENO) < 0) {
                    perror("dup2 output file failed");
                    exit(1);
                }
                close(outFd);
            }
            
            if (stages[i].args.empty()) {
                exit(0);
            }
            
            // Intercept built-ins inside child if run in pipeline
            if (stages[i].args[0] == "cd") {
                std::string path;
                if (stages[i].args.size() < 2) {
                    const char* home = getenv("HOME");
                    if (home) path = home;
                    else exit(1);
                } else {
                    path = resolvePath(stages[i].args[1]);
                }
                if (chdir(path.c_str()) != 0) {
                    perror("cd failed");
                    exit(1);
                }
                exit(0);
            } else if (stages[i].args[0] == "exit" || stages[i].args[0] == "quit") {
                int code = 0;
                if (stages[i].args.size() >= 2) {
                    try {
                        code = std::stoi(stages[i].args[1]);
                    } catch (...) {
                        code = 0;
                    }
                }
                exit(code);
            }
            
            // Build the argv pointer array required by execvp
            std::vector<std::string> argsCopy = stages[i].args;
            std::vector<char*> childArgv;
            for (auto& s : argsCopy) {
                childArgv.push_back(&s[0]);
            }
            childArgv.push_back(nullptr);
            
            execvp(childArgv[0], childArgv.data());
            
            perror(("execvp " + stages[i].args[0] + " failed").c_str());
            exit(127);
        }
    }
    
    // Parent closes all pipe FDs
    for (int fd : pipeFds) {
        close(fd);
    }
    
    // Wait for all child processes and capture exit code of the final stage
    int lastStatus = 0;
    for (size_t i = 0; i < numStages; ++i) {
        int childStatus = 0;
        if (waitpid(pids[i], &childStatus, 0) == -1) {
            perror("waitpid failed");
            lastExitCode = 1;
        } else {
            if (i == numStages - 1) {
                lastStatus = childStatus;
            }
        }
    }
    
    if (WIFEXITED(lastStatus)) {
        lastExitCode = WEXITSTATUS(lastStatus);
    } else if (WIFSIGNALED(lastStatus)) {
        lastExitCode = 128 + WTERMSIG(lastStatus);
    } else {
        lastExitCode = 1;
    }
    
    return lastExitCode;
}

int CommandExecutor::getLastExitCode() const {
    return lastExitCode;
}

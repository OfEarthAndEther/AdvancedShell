#include "executor.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>

CommandExecutor::CommandExecutor() : lastExitCode(0) {}

int CommandExecutor::execute(const std::string& command) {
    // 1. Tokenize command by whitespace
    std::vector<std::string> args;
    std::string arg;
    std::istringstream iss(command);
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    if (args.empty()) {
        return 0;
    }
    
    // 2. Build the argv pointer array required by execvp
    std::vector<char*> argv;
    for (auto& s : args) {
        argv.push_back(&s[0]);
    }
    argv.push_back(nullptr);
    
    // 3. Spawning lifecycle
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        lastExitCode = 1;
        return lastExitCode;
    } else if (pid == 0) {
        // Child Process
        execvp(argv[0], argv.data());
        // execvp only returns if an error occurred
        perror("execvp failed");
        exit(127);
    } else {
        // Parent Process
        int status = 0;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            lastExitCode = 1;
        } else {
            if (WIFEXITED(status)) {
                lastExitCode = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                lastExitCode = 128 + WTERMSIG(status);
            } else {
                lastExitCode = 1;
            }
        }
    }
    
    return lastExitCode;
}

int CommandExecutor::getLastExitCode() const {
    return lastExitCode;
}

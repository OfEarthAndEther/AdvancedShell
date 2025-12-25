#include "git_info.hpp"
#include <cstdio>
#include <cstdlib>

GitInfo::GitInfo() {}

std::string GitInfo::executeCommand(const std::string& cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    
    char buffer = {0};
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

bool GitInfo::isInRepository() {
    int result = system("git rev-parse --git-dir > /dev/null 2>&1");
    return result == 0;
}

std::string GitInfo::getBranch() {
    return executeCommand("git rev-parse --abbrev-ref HEAD 2>/dev/null");
}

GitInfo::GitStatus GitInfo::getStatus() {
    GitStatus status = {false, false, false};
    
    std::string output = executeCommand("git status --porcelain 2>/dev/null");
    if (output.empty()) return status;
    
    for (size_t i = 0; i < output.length(); i += 3) {
        if (i + 1 < output.length()) {
            char first = output[i];
            char second = output[i + 1];
            
            if (first != ' ' && first != '?') {
                if (second == 'M' || second == 'D') status.modified = true;
            }
            if (first == 'A') status.staged = true;
            if (first == '?' && second == '?') status.untracked = true;
        }
    }
    
    return status;
}

std::string GitInfo::getPromptString() {
    if (!isInRepository()) return "";
    
    std::string prompt = " [🌱 ";
    prompt += getBranch();
    
    GitStatus status = getStatus();
    if (status.modified) prompt += " ✏️";
    if (status.staged) prompt += " ✅";
    if (status.untracked) prompt += " ❓";
    
    prompt += "]";
    return prompt;
}

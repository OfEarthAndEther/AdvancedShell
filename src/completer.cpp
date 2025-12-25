#include "completer.hpp"
#include <dirent.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>

FileCompleter::FileCompleter() {}

std::vector<std::string> FileCompleter::completeCommands(const std::string& partial) {
    std::vector<std::string> results;
    
    const char* pathEnv = getenv("PATH");
    if (!pathEnv) return results;
    
    std::string paths(pathEnv);
    size_t start = 0;
    
    while (start < paths.length()) {
        size_t end = paths.find(':', start);
        std::string dir = paths.substr(start, end - start);
        
        DIR* dirp = opendir(dir.c_str());
        if (dirp) {
            struct dirent* entry;
            while ((entry = readdir(dirp)) != nullptr) {
                std::string name = entry->d_name;
                
                if (name.length() >= partial.length() && 
                    name.substr(0, partial.length()) == partial &&
                    results.size() < MAX_COMPLETIONS) {
                    
                    if (std::find(results.begin(), results.end(), name) == results.end()) {
                        results.push_back(name);
                    }
                }
            }
            closedir(dirp);
        }
        
        start = (end == std::string::npos) ? paths.length() : end + 1;
    }
    
    std::sort(results.begin(), results.end());
    return results;
}

std::vector<std::string> FileCompleter::completeFiles(const std::string& partial) {
    std::vector<std::string> results;
    
    size_t lastSlash = partial.rfind('/');
    std::string directory = ".";
    std::string prefix = partial;
    
    if (lastSlash != std::string::npos) {
        directory = partial.substr(0, lastSlash);
        if (directory.empty()) directory = "/";
        prefix = partial.substr(lastSlash + 1);
    }
    
    DIR* dirp = opendir(directory.c_str());
    if (dirp) {
        struct dirent* entry;
        while ((entry = readdir(dirp)) != nullptr && results.size() < MAX_COMPLETIONS) {
            std::string name = entry->d_name;
            
            if (name == "." || name == "..") continue;
            
            if (name.length() >= prefix.length() && 
                name.substr(0, prefix.length()) == prefix) {
                
                results.push_back(name);
            }
        }
        closedir(dirp);
    }
    
    std::sort(results.begin(), results.end());
    return results;
}

std::vector<std::string> FileCompleter::completeVariables(const std::string& partial) {
    std::vector<std::string> results;
    
    extern char** environ;
    for (int i = 0; environ[i] != nullptr; i++) {
        std::string envVar = environ[i];
        std::string varName = envVar.substr(0, envVar.find('='));
        
        std::string searchPrefix = partial.substr(1);
        
        if (varName.length() >= searchPrefix.length() && 
            varName.substr(0, searchPrefix.length()) == searchPrefix &&
            results.size() < MAX_COMPLETIONS) {
            
            results.push_back("$" + varName);
        }
    }
    
    std::sort(results.begin(), results.end());
    return results;
}

std::vector<std::string> FileCompleter::complete(const std::string& input, int cursorPos) {
    if (input.empty() || cursorPos == 0) {
        return {};
    }
    
    std::string partial = input.substr(0, cursorPos);
    
    if (partial.find('$') != std::string::npos && partial.back() != ' ') {
        size_t dollarPos = partial.rfind('$');
        return completeVariables(partial.substr(dollarPos));
    }
    
    if (partial.find('/') != std::string::npos) {
        return completeFiles(partial);
    }
    
    size_t spacePos = partial.find(' ');
    if (spacePos == std::string::npos) {
        return completeCommands(partial);
    }
    
    std::string lastWord = partial.substr(spacePos + 1);
    if (lastWord.empty()) return {};
    return completeFiles(lastWord);
}

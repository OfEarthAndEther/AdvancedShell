#include "highlighter.hpp"
#include <sstream>
#include <cstdlib>
#include <algorithm>

constexpr const char* COLOR_RESET = "\033[0m";
constexpr const char* COLOR_CMD_OK = "\033[1;32m";
constexpr const char* COLOR_CMD_FAIL = "\033[1;31m";
constexpr const char* COLOR_FLAG = "\033[1;36m";
constexpr const char* COLOR_REDIRECT = "\033[1;33m";
constexpr const char* COLOR_PIPE = "\033[1;33m";
constexpr const char* COLOR_VARIABLE = "\033[1;35m";

SyntaxHighlighter::SyntaxHighlighter() {
    builtins = {"cd", "pwd", "exit", "echo", "export", "alias", 
                 "unalias", "history", "jobs", "fg", "bg", "kill", "clear"};
}

std::string SyntaxHighlighter::highlight(const std::string& input) {
    std::istringstream iss(input);
    std::string token;
    std::string result;
    int tokenNum = 0;
    
    while (iss >> token) {
        if (tokenNum > 0) result += " ";
        
        if (tokenNum == 0) {
            result += highlightCommand(token);
        } else if (token == "-") {
            result += COLOR_FLAG;
            result += token;
            result += COLOR_RESET;
        } else if (token == "|" || token == "||" || token == "&&") {
            result += COLOR_PIPE;
            result += token;
            result += COLOR_RESET;
        } else if (token == ">" || token == "<" || token == ">>" || token == "2>") {
            result += COLOR_REDIRECT;
            result += token;
            result += COLOR_RESET;
        } else if (token == "$") {
            result += COLOR_VARIABLE;
            result += token;
            result += COLOR_RESET;
        } else {
            result += token;
        }
        
        tokenNum++;
    }
    
    return result;
}

std::string SyntaxHighlighter::highlightCommand(const std::string& cmd) {
    if (isBuiltin(cmd)) {
        return std::string(COLOR_CMD_OK) + cmd + COLOR_RESET;
    }
    
    if (commandExists(cmd)) {
        return std::string(COLOR_CMD_OK) + cmd + COLOR_RESET;
    }
    
    return std::string(COLOR_CMD_FAIL) + cmd + COLOR_RESET;
}

bool SyntaxHighlighter::isBuiltin(const std::string& cmd) {
    return builtins.find(cmd) != builtins.end();
}

bool SyntaxHighlighter::commandExists(const std::string& cmd) {
    std::string checkCmd = "which " + cmd + " > /dev/null 2>&1";
    return system(checkCmd.c_str()) == 0;
}

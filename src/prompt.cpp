#include "prompt.hpp"
#include <unistd.h>
#include <pwd.h>
#include <cstring>
#include <sys/types.h>

constexpr const char* ANSI_RESET = "\033[0m";
constexpr const char* ANSI_BOLD = "\033[1m";
constexpr const char* ANSI_GREEN = "\033[32m";
constexpr const char* ANSI_RED = "\033[31m";
constexpr const char* ANSI_BLUE = "\033[34m";
constexpr const char* ANSI_YELLOW = "\033[33m";

ShellPrompt::ShellPrompt() : lastExitCode(0) {}

std::string ShellPrompt::generate(int exitCode, const std::string& currentDir) {
    lastExitCode = exitCode;
    
    std::string prompt;
    
    // Status indicator
    if (exitCode == 0) {
        prompt += ANSI_GREEN;
        prompt += "✓";
    } else {
        prompt += ANSI_RED;
        prompt += "✗";
    }
    prompt += ANSI_RESET;
    prompt += " ";
    
    // Username
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        prompt += ANSI_BOLD;
        prompt += pw->pw_name;
        prompt += ANSI_RESET;
    }
    
    prompt += "@";
    
    // Hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strncpy(hostname, "unknown", sizeof(hostname) - 1);
        hostname[sizeof(hostname) - 1] = '\0';
    }
    prompt += ANSI_BLUE;
    prompt += hostname;
    prompt += ANSI_RESET;
    
    prompt += " ";
    
    // Current directory
    std::string displayDir = expandHome(currentDir);
    prompt += ANSI_YELLOW;
    prompt += displayDir;
    prompt += ANSI_RESET;
    
    // Prompt symbol
    prompt += (getuid() == 0) ? "# " : "$ ";
    
    return prompt;
}

std::string ShellPrompt::expandHome(const std::string& path) {
    if (path.empty()) return path;
    
    const char* home = getenv("HOME");
    if (home && path.find(home) == 0) {
        return "~" + path.substr(strlen(home));
    }
    
    return path;
}

std::string ShellPrompt::getLastStatus() {
    return (lastExitCode == 0) ? "success" : "error";
}

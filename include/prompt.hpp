#ifndef SHELL_PROMPT_HPP
#define SHELL_PROMPT_HPP

#include <string>

class ShellPrompt {
public:
    ShellPrompt();
    
    std::string generate(int exitCode, const std::string& currentDir);
    std::string getLastStatus();
    
private:
    int lastExitCode;
    std::string expandHome(const std::string& path);
};

#endif // SHELL_PROMPT_HPP

#ifndef SHELL_GIT_INFO_HPP
#define SHELL_GIT_INFO_HPP

#include <string>

class GitInfo {
public:
    struct GitStatus {
        bool modified;
        bool staged;
        bool untracked;
    };
    
    GitInfo();
    
    bool isInRepository();
    std::string getBranch();
    GitStatus getStatus();
    std::string getPromptString();
    
private:
    std::string executeCommand(const std::string& cmd);
};

#endif // SHELL_GIT_INFO_HPP

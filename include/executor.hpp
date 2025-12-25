#ifndef SHELL_EXECUTOR_HPP
#define SHELL_EXECUTOR_HPP

#include <string>

class CommandExecutor {
public:
    CommandExecutor();
    
    int execute(const std::string& command);
    int getLastExitCode() const;
    
private:
    int lastExitCode;
};

#endif // SHELL_EXECUTOR_HPP

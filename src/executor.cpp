#include "executor.hpp"
#include <cstdlib>
#include <sys/wait.h>

CommandExecutor::CommandExecutor() : lastExitCode(0) {}

int CommandExecutor::execute(const std::string& command) {
    if (command.empty()) {
        return 0;
    }
    
    lastExitCode = system(command.c_str());
    
    if (WIFEXITED(lastExitCode)) {
        lastExitCode = WEXITSTATUS(lastExitCode);
    } else {
        lastExitCode = 1;
    }
    
    return lastExitCode;
}

int CommandExecutor::getLastExitCode() const {
    return lastExitCode;
}

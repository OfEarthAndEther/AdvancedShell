#ifndef SHELL_COMPLETER_HPP
#define SHELL_COMPLETER_HPP

#include <string>
#include <vector>

class FileCompleter {
public:
    FileCompleter();
    
    std::vector<std::string> complete(const std::string& input, int cursorPos);
    
private:
    static constexpr int MAX_COMPLETIONS = 50;
    
    std::vector<std::string> completeCommands(const std::string& partial);
    std::vector<std::string> completeFiles(const std::string& partial);
    std::vector<std::string> completeVariables(const std::string& partial);
};

#endif // SHELL_COMPLETER_HPP

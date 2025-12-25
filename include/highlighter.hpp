#ifndef SHELL_HIGHLIGHTER_HPP
#define SHELL_HIGHLIGHTER_HPP

#include <string>
#include <set>

class SyntaxHighlighter {
public:
    SyntaxHighlighter();
    
    std::string highlight(const std::string& input);
    
private:
    std::set<std::string> builtins;
    
    std::string highlightCommand(const std::string& cmd);
    bool isBuiltin(const std::string& cmd);
    bool commandExists(const std::string& cmd);
};

#endif // SHELL_HIGHLIGHTER_HPP

#ifndef SHELL_HPP
#define SHELL_HPP

#include "prompt.hpp"
#include "history.hpp"
#include "completer.hpp"
#include "highlighter.hpp"
#include "git_info.hpp"
#include "executor.hpp"
#include <string>

class Shell {
public:
    Shell();
    void run();
    
private:
    ShellPrompt prompter;
    CommandHistory history;
    FileCompleter completer;
    SyntaxHighlighter highlighter;
    GitInfo gitInfo;
    CommandExecutor executor;
    bool isRunning;
    
    void processBuiltins(const std::string& input);
    bool isBuiltinCommand(const std::string& cmd);
};

#endif // SHELL_HPP

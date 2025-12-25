#ifndef SHELL_HISTORY_HPP
#define SHELL_HISTORY_HPP

#include <string>
#include <vector>
#include <deque>

class CommandHistory {
public:
    CommandHistory();
    
    void addCommand(const std::string& cmd);
    std::vector<std::string> search(const std::string& prefix);
    std::string getPrevious(int index);
    void print();
    void clear();
    int size() const;
    
private:
    static constexpr int MAX_HISTORY = 1000;
    static constexpr int MAX_RESULTS = 50;
    
    std::deque<std::string> history;
    std::string historyFile;
    
    void loadHistory();
    void saveHistory();
};

#endif // SHELL_HISTORY_HPP

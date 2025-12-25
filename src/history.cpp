#include "history.hpp"
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <cstdio>

CommandHistory::CommandHistory() {
    const char* home = getenv("HOME");
    if (home) {
        historyFile = std::string(home) + "/.myshell_history";
        loadHistory();
    }
}

void CommandHistory::addCommand(const std::string& cmd) {
    if (cmd.empty()) return;
    if (!history.empty() && history.back() == cmd) return;
    
    history.push_back(cmd);
    
    while (history.size() > MAX_HISTORY) {
        history.pop_front();
    }
    
    saveHistory();
}

std::vector<std::string> CommandHistory::search(const std::string& prefix) {
    std::vector<std::string> results;
    
    if (prefix.empty()) return results;
    
    for (auto it = history.rbegin(); it != history.rend() && results.size() < MAX_RESULTS; ++it) {
        if (it->find(prefix) == 0) {
            results.push_back(*it);
        }
    }
    
    return results;
}

std::string CommandHistory::getPrevious(int index) {
    if (index < 0 || index >= static_cast<int>(history.size())) {
        return "";
    }
    
    return history[history.size() - 1 - index];
}

void CommandHistory::print() {
    int num = 1;
    for (const auto& cmd : history) {
        printf("%5d  %s\n", num++, cmd.c_str());
    }
}

void CommandHistory::loadHistory() {
    std::ifstream file(historyFile);
    if (!file.is_open()) return;
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            history.push_back(line);
        }
    }
    
    while (history.size() > MAX_HISTORY) {
        history.pop_front();
    }
}

void CommandHistory::saveHistory() {
    std::ofstream file(historyFile, std::ios::app);
    if (!file.is_open()) return;
    
    if (!history.empty()) {
        file << history.back() << "\n";
    }
}

int CommandHistory::size() const {
    return history.size();
}

void CommandHistory::clear() {
    history.clear();
    std::ofstream file(historyFile, std::ios::trunc);
    file.close();
}

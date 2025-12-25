#include "shell.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>

Shell::Shell() : isRunning(true) {}

void Shell::run() {
    while (isRunning) {
        try {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) == nullptr) {
                strcpy(cwd, "~");
            }
            
            std::string prompt = prompter.generate(executor.getLastExitCode(), std::string(cwd));
            if (gitInfo.isInRepository()) {
                prompt += gitInfo.getPromptString();
            }
            prompt += " ";
            
            std::cout << prompt;
            std::cout.flush();
            
            std::string input;
            if (!std::getline(std::cin, input)) {
                isRunning = false;
                break;
            }
            
            if (input.empty()) continue;
            
            if (input == "exit" || input == "quit") {
                isRunning = false;
                break;
            }
            
            if (input == "clear") {
                system("clear");
                continue;
            }
            
            if (input == "history") {
                history.print();
                continue;
            }
            
            history.addCommand(input);
            executor.execute(input);
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}

#include "shell.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        Shell shell;
        shell.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

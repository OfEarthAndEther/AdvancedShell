# Advanced Unix Shell (C++17)

A modular, POSIX-compliant Unix shell implemented from scratch in C++17. Designed as a high-performance portfolio codebase showcasing systems programming, process lifecycle management, and lightweight shell enhancements.

---

```
  [User Input] ──> [Syntax Highlighter] ──> [Builtin/Command Executor]
                                                       │
                                            ┌──────────┴──────────┐
                                            ▼                     ▼
                                     [POSIX fork()]       [Builtin Commands]
                                            │                     │
                                     [execvp() child]     [Local chdir()/env]
```

---

## 🚀 Core Features

*   **POSIX Subprocess Spawning:** Clones processes natively via `fork()`, executing system binaries through `execvp()` and waiting synchronously via `waitpid()`.
*   **Git-Aware Prompt:** Visualizes current working directory, status codes (✓/✗), and real-time Git status (current branch, modified `✏️`, staged `✅`, or untracked `❓` changes).
*   **Static Syntax Highlighting:** Lexes command lines to color-code flags, pipes, redirects, variables, and commands in real time.
*   **Deduplicated History:** Manages an on-disk log file (`~/.myshell_history`) via `std::deque` with prefix search capabilities.
*   **Auto-Completion Engine:** Scans `PATH` and directories for matching executables and system variables.

---

## 🛠️ Key Technical Achievements
*   **Modern C++ Standardization:** Warnings-free compilation using C++17 features on strict compilers (GCC 13+ / Ubuntu 24.04).
*   **Direct OS Subprocess Spawning:** Replaced unsafe, high-overhead `std::system()` wrapper calls with standard POSIX child control, propagating return codes and signal terminations correctly.

---

## 📦 Quick Start

```bash
# Build the project
mkdir -p build && cd build
cmake ..
make

# Run the shell
./myshell
```

---

## 🔮 Future Scope & Milestones

*   **Phase 2: True Parent-Shell Builtins**
    *   Intercept `cd` and `pwd` builtins using POSIX `chdir()` and `getcwd()` so that directory changes alter the parent process state.
    *   Implement `export` and `unset` using `setenv()` and `unsetenv()` to support shell environment state persistence.
*   **Phase 3: Signal Handling & Job Control**
    *   Forward `SIGINT` (`Ctrl+C`) and `SIGTSTP` (`Ctrl+Z`) to active child subprocesses instead of killing the main shell process.
    *   Manage foreground/background tasks using job arrays.
*   **Phase 4: Raw Terminal Input & Tab Completion**
    *   Transition terminal control from canonical (cooked) mode to raw mode using `<termios.h>` (`tcsetattr`).
    *   Read keyboard input character-by-character to intercept the `Tab` key and dynamically render completion suggestions inline.
*   **Phase 5: Pipelines & Redirects**
    *   Parse and construct execution chains using multi-stage piping (`|`) and file redirection (`<`, `>`, `>>`, `2>`) implemented via `pipe()` and `dup2()`.
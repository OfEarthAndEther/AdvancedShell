# Advanced Unix Shell in Modern C++

A Unix-style command shell implemented in modern C++17, designed to showcase practical systems programming, clean software architecture, and a considered command-line user experience. The project combines core operating-systems concepts with higher-level features such as history, completion, and Git-awareness to form a cohesive and extensible tool.

---

## 1. Project Overview

This shell was developed with two primary goals:

1. To translate operating-systems theory (processes, environments, file descriptors, terminal I/O) into a concrete, working implementation.
2. To produce a portfolio-quality codebase that can be discussed in depth in technical interviews and used as a reference for future systems work.

Rather than attempting to replicate every feature of a mature shell, the implementation focuses on a carefully selected subset that illustrates depth of understanding and thoughtful design:

- External command execution via a controlled interface.
- A context-aware, colored prompt with exit-status feedback.
- Persistent command history stored on disk.
- Basic yet performant tab completion for commands, files, and environment variables.
- Lightweight syntax highlighting of common shell constructs.
- Optional Git integration for repository and branch visibility in the prompt.

The code is organized into modular components so that each concern (prompt, history, completion, execution) can be understood and evolved independently.

---

## 2. Features

### 2.1 Core Shell Functionality

- Executes external programs using the classic shell model (spawn a process, run the command, propagate the exit status).
- Handles command lines consisting of a command name and whitespace-separated arguments.
- Provides a set of common built-in commands:
  - `cd` – change the current working directory.
  - `pwd` – print the current working directory.
  - `exit` / `quit` – terminate the shell.
  - `clear` – clear the terminal screen.
  - `history` – list previously executed commands.
- Robust handling of empty input lines and end-of-file conditions.
- Clear separation between parsing, execution, and user-interface concerns.

### 2.2 Prompt and User Experience

- Context-aware prompt that includes:
  - The exit status of the previous command, surfaced as a status indicator.
  - Username, obtained from the system account database.
  - Hostname, obtained from the operating system.
  - Current working directory, with `~` expansion for the user’s home directory.
  - Different prompt marker for privileged vs non-privileged users.
- Prompt coloring implemented with ANSI escape sequences in a dedicated component, isolating terminal-specific details from the rest of the logic.
- Designed for low overhead so that prompt rendering does not become a performance bottleneck during interactive use.

### 2.3 Command History

- Persistent history stored in a user-specific file (for example, `~/.myshell_history`).
- In-memory representation based on `std::deque` with a configurable maximum length (default 1000 entries) to balance usefulness and memory usage.
- Avoidance of consecutive duplicate entries to keep the history semantically meaningful.
- Simple prefix-based search interface that can be reused by any feature requiring access to prior commands.
- Conservative file I/O strategy that appends new commands without rewriting the entire history file on each execution.

### 2.4 Tab Completion

- Command completion that searches all directories listed in the `PATH` environment variable and proposes matching executables.
- File and directory completion using POSIX directory traversal, with support for nested and relative paths.
- Environment-variable completion for tokens starting with `$`, using the current process environment.
- Deduplicated result lists, truncated to a configurable maximum, to ensure predictable and responsive behaviour in large file systems or environments.
- Abstracted into a `FileCompleter` component to simplify later extensions (e.g. branch completion, flag completion, or programmable completion).

### 2.5 Syntax Highlighting

- Token-based syntax highlighter that categorizes parts of the command line and assigns color schemes:
  - First token treated as a command; highlighted differently when known vs unknown.
  - Flags and options (tokens beginning with `-`) highlighted as a separate category.
  - Pipes and redirection operators (e.g. `|`, `>`, `>>`, `<`) highlighted to visually reveal pipeline structure.
  - Environment variables (tokens beginning with `$`) highlighted to distinguish them from literal arguments.
- Implemented without external libraries, using only standard C++ string and stream facilities together with ANSI color codes.
- Designed as a pure transformation from input string to highlighted output, which simplifies future testing and refactoring.

### 2.6 Git Integration

- Optional Git-aware prompt capable of:
  - Detecting whether the current directory is within a Git repository.
  - Reading the current branch name via a lightweight Git command.
  - Inferring the presence of modified, staged, or untracked files using the porcelain status output.
- Implemented so that:
  - Failure to find Git or to read repository data results in a graceful fallback to a non-Git prompt.
  - Overhead is minimized by limiting Git invocations and avoiding expensive operations on each command.

---

## 3. Architecture

The codebase is structured around a set of well-defined components, each with a narrow responsibility.

### 3.1 High-Level Components

- `Shell`  
  Manages the main read–evaluate–print loop. It owns instances of the prompt, history, completer, highlighter, Git integration, and executor, and coordinates their interaction.

- `ShellPrompt`  
  Generates the textual prompt string. It knows how to format and color status indicators, usernames, hostnames, and directories, but is unaware of how commands are executed.

- `CommandHistory`  
  Encapsulates all logic related to storing and retrieving historical commands, including file persistence and prefix-based search.

- `FileCompleter`  
  Resolves partial tokens to candidate completions, based on PATH scanning, file-system inspection, and environment variables.

- `SyntaxHighlighter`  
  Transforms raw command lines into color-annotated strings while preserving semantic structure, using a minimal lexical analysis.

- `GitInfo`  
  Provides a small interface for obtaining Git-related information. All use of Git commands and parsing of their output is confined here.

- `CommandExecutor`  
  Exposes a simple `execute(command)` function, responsible for running the command line and returning an exit status. The current implementation uses the C library’s command execution interface, but the class is intentionally small so it can be replaced with a more advanced pipeline-based implementation.

### 3.2 Design Principles

- Separation of concerns: each source file serves a focused purpose, improving readability and maintainability.
- Modern C++ usage: the project makes extensive use of standard containers and RAII; manual memory management is avoided entirely.
- Error containment: non-critical failures (for example, inability to access the history file or absence of Git) do not terminate the shell; they result in degraded functionality with predictable behaviour.
- Extensibility: component boundaries were chosen to make it straightforward to add advanced features such as job control, more sophisticated parsing, or richer completion strategies.

---

## 4. Building the Project

### Quick Start

The project uses CMake for configuration and generation of build files. A typical build flow on a Unix-like system is:
    
```    
git clone https://github.com/yourusername/AdvancedShell.git
cd AdvancedShell

mkdir -p build
cd build
cmake ..
make

./myshell    
```
    

Requirements:

- C++17-capable compiler (for example, a recent version of GCC or Clang).
- CMake (version 3.10 or newer is a reasonable baseline).
- A POSIX-like environment providing standard headers and system calls.

---

## 5. Usage

Once `myshell` is built and executed, it can be used interactively:
    
### Basic navigation and inspection
    
```
$ pwd
$ cd /tmp
$ ls -la
```    

### History
    
```
$ history
$ history | grep ls
```    

### File and command completion
    
```
$ ls /et<TAB> # completes to /etc/
$ cd ~/Doc<TAB> # completes to an appropriate directory
$ echo $HO<TAB> # completes to $HOME
```    

### Working inside a Git repository
    
```
$ cd /path/to/repo
```    

prompt now includes current branch name and, when applicable, indicators for modified or untracked files.


The prompt continually reflects the success or failure of the most recent command, which is especially helpful when chaining commands or running longer workflows.

---

## 6. Implementation Notes

- The implementation uses C++17 standard facilities and avoids non-portable extensions where possible.
- ANSI escape codes are used for coloring, which is the de facto standard for terminal control in Unix-like environments.
- History persistence is implemented via straightforward text file appends to keep the design simple and transparent.
- Completion and Git integration interact with the underlying system using a minimal set of POSIX calls and subprocess invocations, keeping the shell largely self-contained.

---

## 7. Learning Outcomes and Rationale

This project was intentionally scoped to be more than a minimal academic exercise:

- It exercises a range of systems concepts, including user identities, process creation, environment handling, and terminal behaviour.
- It demonstrates modern C++ practices through the use of RAII, standard containers, and attention to exception safety.
- It emphasizes modular design, enabling discussion of system boundaries, interface design, and testing strategies.
- By including user-oriented features such as the prompt, history, completion, and Git-awareness, it shows consideration for developer workflows and product thinking, not just low-level mechanics.

The codebase is therefore suitable both as a learning vehicle and as a substantive demonstration piece for internships or early-career systems and backend roles.

---

## 8. Possible Extensions

Although the current implementation is intentionally focused, it has been designed with growth in mind. Natural next steps include:

- Explicit pipeline and redirection support using `fork`, `dup2`, and `exec` for multi-stage command chains.
- Foreground/background job control with job listing and signal management.
- Script and configuration file support (for example, startup files that define aliases, environment variables, and prompt themes).
- Integration with an advanced line-editing library for richer editing capabilities, key bindings, and incremental search.
- Automated unit tests for core modules such as the history manager, completer, and prompt generator.

These features can be added incrementally without disrupting the existing structure, illustrating the value of the chosen architecture.

---

## 9. Repository Structure

A typical layout of the repository is as follows:
    
```
AdvancedShell/
├── CMakeLists.txt # Build configuration
├── README.md # Project documentation
├── .gitignore # Version-control hygiene
├── include/ # Public headers
│ ├── shell.hpp
│ ├── prompt.hpp
│ ├── history.hpp
│ ├── completer.hpp
│ ├── highlighter.hpp
│ ├── git_info.hpp
│ └── executor.hpp
└── src/ # Implementation files
├── main.cpp
├── shell.cpp
├── prompt.cpp
├── history.cpp
├── completer.cpp
├── highlighter.cpp
├── git_info.cpp
└── executor.cpp
```    


This structure keeps interfaces and implementations clearly separated and is compatible with standard CMake practices.

---

## 10. License

This project is intended primarily as an educational and portfolio resource. A permissive license such as MIT is suitable for allowing others to study, adapt, and extend the implementation while providing appropriate attribution.
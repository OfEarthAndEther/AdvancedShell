cmake_minimum_required(VERSION 3.10)
project(MyShell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/shell.cpp
    src/prompt.cpp
    src/history.cpp
    src/completer.cpp
    src/highlighter.cpp
    src/git_info.cpp
    src/executor.cpp
)

add_executable(myshell ${SOURCES})
target_compile_features(myshell PRIVATE cxx_std_17)

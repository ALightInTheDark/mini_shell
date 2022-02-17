cmake_minimum_required(VERSION 3.16)

project(mini_shell)

set(CMAKE_CXX_STANDARD 17)

add_executable(mini_shell main.cpp executer.cpp global_variable.cpp parser.cpp)

cmake_minimum_required(VERSION 3.8)

#set the project name
project(minihell)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_COMPILER /usr/bin/g++)
#set(CMAKE_C++_COMPILER_INIT g++)

#add the executable
add_executable(cl CobleParseCommandLine.cpp)

add_executable(exec_ex exec_ex.cpp)

add_executable(hs minishell.cpp)

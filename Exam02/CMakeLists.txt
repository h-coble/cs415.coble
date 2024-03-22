cmake_minimum_required(VERSION 3.8)

#set the project name
project(minihell)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(CMAKE_CXX_COMPILER /usr/bin/g++)
#set(CMAKE_C++_COMPILER_INIT g++)

#add the executable
add_executable(Read-Write Q2.cpp)
add_executable(Read-Write3 Q3.cpp)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(Read-Write PRIVATE Threads::Threads)


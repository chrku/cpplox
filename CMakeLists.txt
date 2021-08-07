cmake_minimum_required(VERSION 3.16)
project(lox)

set(CMAKE_CXX_STANDARD 20)

add_executable(lox
               src/main.cpp
               src/lox.cpp
               src/string_utils.cpp)
cmake_minimum_required(VERSION 3.16)
project(lox)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(lox
               src/main.cpp
               src/lox.cpp
               src/token.cpp
               src/scanner.cpp
               src/token_type.cpp
               src/parser.cpp
               src/interpreter.cpp
               src/types.cpp
               src/environment.cpp src/native_functions/clock.cpp src/native_functions/clock.h)
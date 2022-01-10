cmake_minimum_required(VERSION 3.16)
project(lox)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        9ca071b6e55568dff6960bebe1a5cfaa180fb3ce
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(src)

add_library(lox_common STATIC
            src/lox.cpp
            src/token.cpp
            src/scanner.cpp
            src/token_type.cpp
            src/parser.cpp
            src/interpreter.cpp
            src/types.cpp
            src/environment.cpp
            src/native_functions/clock.cpp
            src/loxfunction.cpp
            src/resolver.cpp
            src/loxclass.cpp
            src/loxinstance.cpp)

add_executable(lox
               src/main.cpp)

target_link_libraries(lox
                      lox_common)

enable_testing()

add_executable(lox_test
               test/test.cpp)

target_link_libraries(lox_test
                      lox_common
                      gtest_main)

include(GoogleTest)
gtest_discover_tests(lox_test)

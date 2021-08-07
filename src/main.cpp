#include <iostream>

#include "lox.h"

int main(int argc, const char *argv[]) {
    // Remember: First arg is program name
    LoxInterpreter interpreter;
    if (argc > 2) {
        std::cout << "Usage: cpplox [script]";
    } else if (argc == 2) {
        interpreter.runFile(argv[1]);
    } else {
        interpreter.runPrompt();
    }
    return 0;
}

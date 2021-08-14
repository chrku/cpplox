#include <iostream>
#include <memory>

#include "lox.h"
#include "types.h"

int main(int argc, const char *argv[]) {
    std::shared_ptr<LoxInterpreter> interpreter = std::make_shared<LoxInterpreter>();

    // Remember: First arg is program name
    if (argc > 2) {
        std::cout << "Usage: cpplox [script]";
    } else if (argc == 2) {
        interpreter->runFile(argv[1]);
    } else {
        interpreter->runPrompt();
    }
    return 0;
}

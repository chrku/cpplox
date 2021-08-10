//
// Created by chrku on 04.08.2021.
//

#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "interpreter.h"
#include "astprinter.h"
#include "utils.h"

#include <fstream>
#include <string>
#include <iostream>

void LoxInterpreter::runFile(const char *filename) {
    std::ifstream ifs{filename};
    if (!ifs.good()) { // We have to check if the stream could be opened
        throw std::invalid_argument{"File could not be opened"};
    }

    // Read file into string
    auto source = std::make_unique<std::string>(std::istreambuf_iterator<char>(ifs),
                                                std::istreambuf_iterator<char>());
    run(std::move(source));

    if (hadError_) {
        std::exit(65);
    }
    if (hadRuntimeError_) {
        std::exit(70);
    }
}

void LoxInterpreter::runPrompt() {
    std::string input_line;

    // Read in each line until EOF
    while (std::cin) {
        std::cout << "> "; // Prompt character
        std::getline(std::cin, input_line);
        run(std::make_unique<std::string>(input_line));
        hadError_ = false;
    }
}

void LoxInterpreter::run(std::unique_ptr<std::string> source) {
    Scanner scanner{std::move(source), shared_from_this()};
    scanner.scanTokens();
    auto tokens = scanner.getTokens();
    Parser parser{tokens, shared_from_this()};
    auto expr = parser.parse();

    if (hadError_) return;
    try {
        LoxType result = interpreter_.evaluate(*expr);
        std::cout << stringify(result) << std::endl;
    } catch (const RuntimeError& error) {
        runtimeError(error);
    }
}

void LoxInterpreter::error(int line, std::string_view message) {
    reportError(line, "", message);
}

void LoxInterpreter::reportError(int line, std::string_view where, std::string_view message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError_ = true;
}

void LoxInterpreter::error(const Token& token, std::string_view message) {
    if (token.getType() == TokenType::EOF_TYPE) {
        reportError(token.getLine(), " at end", message);
    } else {
        reportError(token.getLine(), "at '" + token.getLexeme() + "'", message);
    }
}

std::string LoxInterpreter::stringify(const LoxType &l) {
    std::string return_value;

    std::visit(overload{
            [&](double d) {
                return_value = std::to_string(d);
            },
            [&](const std::string& s) {
                return_value = s;
            },
            [&](const NullType& n) {
                return_value = "nil";
            },
            [&](bool b) {
                return_value = std::to_string(b);
            }
    }, l);

    return return_value;
}

void LoxInterpreter::runtimeError(const RuntimeError& e) {
    std::cout << e.what() << "\nline " << e.getToken().getLine() << "]\n";
    hadRuntimeError_ = true;
}

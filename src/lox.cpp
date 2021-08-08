//
// Created by chrku on 04.08.2021.
//

#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "astprinter.h"

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
        throw std::runtime_error{"Error during scanning of program"};
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
    AstPrinter printer;
    std::cout << printer.print(*expr) << std::endl;
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

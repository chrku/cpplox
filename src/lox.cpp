//
// Created by chrku on 04.08.2021.
//

#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "interpreter.h"
#include "resolver.h"

#include <fstream>
#include <string>
#include <iostream>

LoxInterpreter::LoxInterpreter()
: interpreter_{std::make_shared<Interpreter>()}, outputStream_{&std::cout}, errorStream_{&std::cerr}
{

}

LoxInterpreter::LoxInterpreter(std::ostream *output_stream, std::ostream *error_stream, bool test_mode)
: interpreter_{std::make_shared<Interpreter>(output_stream, test_mode)},
  outputStream_{output_stream}, errorStream_{error_stream}, testMode_{test_mode}
{

}


void LoxInterpreter::runFile(const char* filename) {
    std::ifstream ifs{filename};
    if (!ifs.good()) { // We have to check if the stream could be opened
        *errorStream_ << "File " << filename << " could not be opened" << std::endl;
        std::exit(64);
    }

    // Read file into string
    auto source = std::make_unique<std::string>(std::istreambuf_iterator<char>(ifs),
                                                std::istreambuf_iterator<char>());
    run(std::move(source), false);

    if (hadError_) {
        if (!testMode_) {
            std::exit(65);
        }
    }
    if (hadRuntimeError_) {
        if (!testMode_) {
            std::exit(70);
        }
    }
}

void LoxInterpreter::runPrompt() {
    std::string input_line;

    // Read in each line until EOF
    while (std::cin) {
        std::cout << "> "; // Prompt character
        std::getline(std::cin, input_line);
        run(std::make_unique<std::string>(input_line), true);
        hadError_ = false;
    }
}

void LoxInterpreter::run(std::unique_ptr<std::string> source, bool repl_mode) {
    Scanner scanner{std::move(source), shared_from_this()};
    scanner.scanTokens();
    auto tokens = scanner.getTokens();
    Parser parser{tokens, shared_from_this()};

    if (repl_mode) {

        disableParseErrorReporting();
        auto expression = parser.parseExpression();
        enableParseErrorReporting();

        if (!hadError_) {
            try {
                auto result = interpreter_->evaluate(*expression);
                std::cout << stringify(result) << std::endl;
            } catch (const RuntimeError &error) {
                runtimeError(error);
            }

            return;
        }
        parser.reset();
        hadError_ = false;
    }

    auto program = parser.parse();
    if (hadError_) { return; }

    Resolver resolver{interpreter_, shared_from_this()};
    resolver.resolve(program);

    if (hadError_) { return; }

    try {
        interpreter_->interpret(program, shared_from_this());
    } catch (const RuntimeError& error) {
        runtimeError(error);
    }
}

void LoxInterpreter::error(int line, std::string_view message) {
    reportError(line, "", message);
}

void LoxInterpreter::reportError(int line, std::string_view where, std::string_view message) {
    if (!silentParseErrors_) {
        *errorStream_ << "[line " << line << "] Error " << where << ": " << message << std::endl;
    }
    hadError_ = true;
}

void LoxInterpreter::error(const Token& token, std::string_view message) {
    if (token.getType() == TokenType::EOF_TYPE) {
        reportError(token.getLine(), " at end", message);
    } else {
        reportError(token.getLine(), "at '" + token.getLexeme() + "'", message);
    }
}

void LoxInterpreter::runtimeError(const RuntimeError& e) {
    *errorStream_ << "[" << e.what() << " line " << e.getToken().getLine() << "]\n";
    hadRuntimeError_ = true;
}

void LoxInterpreter::enableParseErrorReporting() {
    silentParseErrors_ = false;
}

void LoxInterpreter::disableParseErrorReporting() {
    silentParseErrors_ = true;
}



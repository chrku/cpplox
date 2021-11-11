//
// Created by chrku on 04.08.2021.
//

/*!
 * This file contains definitions for running the lox interpreter
 * from the command line
 */

#ifndef LOX_LOX_H
#define LOX_LOX_H

#include <memory>
#include <ostream>

#include "token.h"
#include "types.h"
#include "interpreter.h"

/*!
 * Class representing the context of the lox interpreter
 */
class LoxInterpreter : public std::enable_shared_from_this<LoxInterpreter> {
public:
    LoxInterpreter();

    LoxInterpreter(std::ostream* output_stream, std::ostream* error_stream, bool test_mode = true);

    /*!
     * Run file containing lox commands
     * @param filename C-Style string containing the file name of the script to be run
     */
    void runFile(const char* filename); // We use const char* because we pass in a command line argument

    /*!
     * Run commands in a REPL
     */
    void runPrompt();

    /*!
     * This is the core functions that takes in a fragment of source code,
     * does the requisite steps and then runs it
     * @param source fragment of source code to run
     */
    void run(std::unique_ptr<std::string> source, bool repl_mode);

    /*!
     * Report an error in a certain line
     * @param line line number where it happened
     * @param message error message
     */
    void error(int line, std::string_view message);

    /*!
     * Report error
     * @param token Token where error occured
     * @param message error message
     */
    void error(const Token& token, std::string_view message);

    /*!
     * Report runtime error
     * @param e runtime error exception object
     */
    void runtimeError(const RuntimeError& e);
private:
    bool hadError_ = false;
    bool hadRuntimeError_ = false;
    bool silentParseErrors_ = false;
    std::shared_ptr<Interpreter> interpreter_;

    std::ostream* outputStream_;
    std::ostream* errorStream_;
    bool testMode_ = false;

    void reportError(int line, std::string_view where, std::string_view message);

    // For REPL functionality, we sometimes need to disable parse error reporting
    void enableParseErrorReporting();
    void disableParseErrorReporting();
};

#endif //LOX_LOX_H

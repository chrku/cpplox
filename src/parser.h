//
// Created by chrku on 08.08.2021.
//

#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "lox.h"
#include "token.h"
#include "types.h"
#include "expressions.h"
#include "statements.h"

#include <memory>
#include <vector>
#include <string_view>
#include <optional>

/*!
 * Represents errors that arise during parsing, i.e. syntax errors
 */
class ParseError : public std::exception {
};

/*!
 * Class that implements a recursive descent parser for the lox language
 */
class Parser {
public:
    /*!
     * Construct parser object
     * @param tokens Token list, from scanner
     * @param interpreter interpreter context for error reporting
     */
    explicit Parser(std::shared_ptr<std::vector<Token>> tokens, std::shared_ptr<LoxInterpreter> interpreter);

    /*!
     * Parse a lox program
     * @return List of lox statements constituting program
     */
    std::vector<std::shared_ptr<Statement>> parse();

    /*!
     * Parse an expression
     * @return Expression AST tree root
     */
    std::unique_ptr<Expression> parseExpression();

    /*!
     * Reset parser state and errors
     */
    void reset();
private:
    std::shared_ptr<std::vector<Token>> tokens_;
    std::shared_ptr<LoxInterpreter> interpreter_;

    // Position in token stream
    int current_ = 0;

    // Number of loops currently enclosing position
    int numLoops_ = 0;

    // Parsing routines for different grammar rules
    // Statements
    std::shared_ptr<Statement> declaration();
    std::unique_ptr<Statement> varDeclaration();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> expressionStatement();
    std::shared_ptr<Statement> functionDeclaration();
    std::shared_ptr<Statement> classDeclaration();
    std::vector<std::shared_ptr<Statement>> block();
    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> forStatement();
    std::unique_ptr<Statement> breakStatement();

    // Expressions
    std::unique_ptr<Expression> expression(bool disable_comma=false);
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> comma(bool disable_comma);
    std::unique_ptr<Expression> logicOr();
    std::unique_ptr<Expression> logicAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> ternary();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();

    // Helper to handle both functions and methods
    std::shared_ptr<Function> function(const std::string& kind);

    // Matching and handling of tokens
    bool match(std::initializer_list<TokenType> types);
    [[nodiscard]] bool check(TokenType type) const;
    [[nodiscard]] bool checkNext(TokenType type) const;
    const Token& advance();
    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token& peek() const;
    [[nodiscard]] const Token& previous() const;
    [[nodiscard]] const Token& next() const;
    [[nodiscard]] bool isInLoop() const;
    void openLoop();
    void closeLoop();

    // Handling of function calls
    std::unique_ptr<Expression> finishCall(std::unique_ptr<Expression> callee);

    // Handling of function expressions
    std::unique_ptr<Expression> handleFunctionExpression();

    // Error reporting and recovery
    const Token& consume(TokenType type, std::string_view message);
    ParseError error(const Token& t, std::string_view message);
    void synchronize();
};


#endif //LOX_PARSER_H

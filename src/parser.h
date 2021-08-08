//
// Created by chrku on 08.08.2021.
//

#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "lox.h"
#include "token.h"
#include "ast.h"

#include <memory>
#include <vector>
#include <string_view>
#include <optional>

class ParseError : public std::exception {
};

class Parser {
public:
    explicit Parser(std::shared_ptr<std::vector<Token>> tokens, std::shared_ptr<LoxInterpreter> interpreter);
    std::unique_ptr<Expression> parse();
private:
    std::shared_ptr<std::vector<Token>> tokens_;
    std::shared_ptr<LoxInterpreter> interpreter_;

    int current_ = 0;

    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();

    bool match(std::initializer_list<TokenType> types);
    [[nodiscard]] bool check(TokenType type) const;
    const Token& advance();
    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token& peek() const;
    [[nodiscard]] const Token& previous() const;
    const Token& consume(TokenType type, std::string_view message);
    ParseError error(const Token& t, std::string_view message);
    void synchronize();
};


#endif //LOX_PARSER_H

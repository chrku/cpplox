//
// Created by chrku on 07.08.2021.
//

#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include "token_type.h"

#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <iostream>

/*!
 * Class representing tokens
 */
class Token {
public:
    Token(TokenType type, std::string_view lexeme, int line);
    Token(TokenType type, std::string_view lexeme, int line, double value);
    Token(TokenType type, std::string_view lexeme, int line, std::string_view value);
private:
    const TokenType type_;
    const std::string lexeme_;
    const int line_;
    std::variant<std::monostate, double, std::string> literal_;

    friend std::ostream& operator<<(std::ostream& os, const Token& t);
};

std::ostream& operator<<(std::ostream& os, const Token& t);

#endif //LOX_TOKEN_H

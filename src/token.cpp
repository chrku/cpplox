//
// Created by chrku on 07.08.2021.
//

#include "token.h"

#include "utils.h"

Token::Token(const TokenType type, std::string_view lexeme, int line)
    : type_{type}, lexeme_{lexeme}, line_{line},  literal_{std::monostate{}} {}

Token::Token(TokenType type, std::string_view lexeme, int line, std::string_view value)
    : type_{type}, lexeme_{lexeme}, line_{line}, literal_{std::string{value}} {}

Token::Token(TokenType type, std::string_view lexeme, int line, double value)
    : type_{type}, lexeme_{lexeme}, line_{line}, literal_{value} {}

std::ostream &operator<<(std::ostream &os, const Token &t) {
    os << "[" << t.type_ << ", " << "Line " << t.line_ << ", Lexeme " << t.lexeme_;

    // Print depending on which type of literal we have
    // The syntax for pattern matching is kind of weird, but it works
    std::visit(overload{
        [&](double d) { os << ", Literal " << d; },
        [&](const std::string& s) { os << ", Literal " << s; },
        [](std::monostate) { },
    }, t.literal_);

    os << "]";

    return os;
}

TokenType Token::getType() const {
    return type_;
}

const std::variant<std::monostate, double, std::string> &Token::getLiteral() const {
    return literal_;
}

const std::string& Token::getLexeme() const {
    return lexeme_;
}

int Token::getLine() const {
    return line_;
}

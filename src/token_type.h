//
// Created by chrku on 07.08.2021.
//

#ifndef LOX_TOKEN_TYPE_H
#define LOX_TOKEN_TYPE_H

#include <iostream>

/*!
 * Enumeration representing different types of tokens in lox
 */
enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    QUESTION_MARK, COLON,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    EOF_TYPE
};

/*!
 * Print token type to output stream
 * @param os output stream to use
 * @param t token enumeration object to print
 * @return output stream handle
 */
std::ostream& operator<<(std::ostream& os, TokenType t);

#endif //LOX_TOKEN_TYPE_H

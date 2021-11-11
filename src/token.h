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
    /*!
     * Create token without literal
     * @param type token type
     * @param lexeme lexeme
     * @param line line number
     */
    Token(TokenType type, std::string_view lexeme, int line);

    /*!
     * Create token with numeric literal
     * @param type token type
     * @param lexeme lexeme
     * @param line line number
     * @param value numeric value of token
     */
    Token(TokenType type, std::string_view lexeme, int line, double value);

    /*!
     * Create token with string literal
     * @param type token type
     * @param lexeme lexeme
     * @param line line number
     * @param value string value of token
     */
    Token(TokenType type, std::string_view lexeme, int line, std::string_view value);

    // Getters for attributes
    [[nodiscard]] TokenType getType() const;
    [[nodiscard]] const std::variant<std::monostate, double, std::string>& getLiteral() const;
    [[nodiscard]] const std::string& getLexeme() const;
    [[nodiscard]] int getLine() const;

    bool operator==(const Token& rhs) const;

    bool operator!=(const Token& rhs) const;

private:
    const TokenType type_;
    const std::string lexeme_;
    const int line_;
    const std::variant<std::monostate, double, std::string> literal_;

    friend std::ostream& operator<<(std::ostream& os, const Token& t);
};

/*!
 * Print token to output stream
 * @param os output stream to use
 * @param t token to print
 * @return output stream handle
 */
std::ostream& operator<<(std::ostream& os, const Token& t);

namespace std {
    template <>
    struct hash<Token>
    {
        std::size_t operator()(const Token& k) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return ((hash<string>()(k.getLexeme())));
        }
    };
}

#endif //LOX_TOKEN_H

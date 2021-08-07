//
// Created by chrku on 07.08.2021.
//

#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "token.h"
#include "lox.h"

#include <string_view>
#include <string>
#include <vector>
#include <memory>
#include <map>

/*!
 * Lexer, produces tokens from raw input
 */
class Scanner {
public:
    /*!
     * Constructor
     * @param source source string, class takes ownership
     * @param loxInterpreter reference to interpreter context for error reporting
     */
    Scanner(std::unique_ptr<std::string> source, std::shared_ptr<LoxInterpreter> loxInterpreter);

    /*!
     * Return any tokens after scanning
     * @return tokens
     */
    [[nodiscard]] const std::vector<Token>& getTokens() const;

    /*!
     * Scan tokens in source
     */
    void scanTokens();
private:
    const static std::map<std::string, TokenType> keywords_;
    std::shared_ptr<LoxInterpreter> interpreter_;
    std::unique_ptr<std::string> source_;

    std::vector<Token> tokens_;

    // Information about where we are in the code
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;

    // Information about position in source and extraction of chars from the source
    [[nodiscard]] bool isAtEnd() const;
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);

    // Adding various types of tokens
    void addToken(TokenType type);
    void addToken(std::string_view literal);
    void addToken(double literal);

    // Dealing with literals and identifiers
    void string();
    void number();
    void identifier();

    // Needed for literals and identifiers
    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isAlphaNumeric(char c);

    // Main function for scanning tokens
    void scanToken();
};


#endif //LOX_SCANNER_H

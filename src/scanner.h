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

class Scanner {
public:
    Scanner(std::unique_ptr<std::string> source, std::shared_ptr<LoxInterpreter> loxInterpreter);
    [[nodiscard]] const std::vector<Token>& getTokens() const;
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

    [[nodiscard]] bool isAtEnd() const;
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);

    void addToken(TokenType type);
    void addToken(std::string_view literal);
    void addToken(double literal);

    void string();
    void number();
    void identifier();

    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isAlphaNumeric(char c);

    void scanToken();
};


#endif //LOX_SCANNER_H

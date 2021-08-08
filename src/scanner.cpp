//
// Created by chrku on 07.08.2021.
//

#include "scanner.h"

const std::map<std::string, TokenType> Scanner::keywords_ = {
        {"and", TokenType::AND},
        {"class", TokenType::CLASS},
        {"else", TokenType::ELSE},
        {"false", TokenType::FALSE},
        {"for", TokenType::FOR},
        {"fun", TokenType::FUN},
        {"if", TokenType::IF},
        {"nil", TokenType::NIL},
        {"or", TokenType::OR},
        {"print", TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super", TokenType::SUPER},
        {"this", TokenType::THIS},
        {"true", TokenType::TRUE},
        {"var", TokenType::VAR},
        {"while", TokenType::WHILE}
};

Scanner::Scanner(std::unique_ptr<std::string> source, std::shared_ptr<LoxInterpreter> loxInterpreter)
    : source_{std::move(source)}, interpreter_{std::move(loxInterpreter)},
      tokens_(std::make_shared<std::vector<Token>>()) {}

std::shared_ptr<std::vector<Token>> Scanner::getTokens() const {
    return tokens_;
}

void Scanner::scanTokens() {
    while(!isAtEnd()) {
        start_ = current_;
        scanToken();
    }

    addToken(TokenType::EOF_TYPE);
}

bool Scanner::isAtEnd() const {
    return current_ >= source_->length();
}

char Scanner::advance() {
    return source_->at(current_++);
}

bool Scanner::match(char expected) {
    if (isAtEnd()) { return false; }
    if (source_->at(current_) != expected) { return false; }

    ++current_;
    return true;
}

char Scanner::peek() {
    if (isAtEnd()) return '\0';
    return source_->at(current_);
}

char Scanner::peekNext() {
    if (current_ + 1 >= source_->length()) return '\0';
    return source_->at(current_ + 1);
}

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line_++;
        advance();
    }

    if (isAtEnd()) {
        interpreter_->error(line_, "Unterminated string.");
        return;
    }

    // The closing "
    advance();

    // Trim the surrounding quotes
    auto value = source_->substr(start_ + 1, current_ - start_ - 2);
    addToken(value);
}

void Scanner::number() {
    while (isDigit(peek())) { advance(); }

    // Handle the decimal part
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume decimal part and advance
        advance();
        while (isDigit(peek())) { advance(); }
    }

    auto value = std::stod(source_->substr(start_, current_ - start_));
    addToken(value);
}

void Scanner::identifier() {
    while (isAlphaNumeric(peek())) advance();

    auto text = source_->substr(start_, current_ - start_);
    if (keywords_.contains(text)) {
        addToken(keywords_.at(text));
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Scanner::addToken(TokenType type) {
    auto lexeme = source_->substr(start_, current_ - start_);
    tokens_->emplace_back(type, lexeme, line_);
}

void Scanner::addToken(std::string_view literal) {
    auto lexeme = source_->substr(start_, current_ - start_);
    tokens_->emplace_back(TokenType::STRING, lexeme, line_, literal);
}

void Scanner::addToken(double literal) {
    auto lexeme = source_->substr(start_, current_ - start_);
    tokens_->emplace_back(TokenType::NUMBER, lexeme, line_, literal);
}

void Scanner::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;

        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;

        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) { // Multi-line comments
                int num_comments_opened = 1;
                bool comment_done = false;
                while (!comment_done && !isAtEnd()) {
                    if (match('*') && match('/')) {
                        num_comments_opened -= 1;
                    } else if (match('/') && match('*')) {
                        num_comments_opened += 1;
                    }

                    if (!isAtEnd()) { advance(); }

                    if (num_comments_opened <= 0) { comment_done = true; }
                }

                if (isAtEnd() && num_comments_opened > 0) {
                    interpreter_->error(line_, "Multi-line comment not closed");
                }
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line_++;
            break;

        case '"': string(); break;

        default:
            if (std::isdigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else { interpreter_->error(line_, "Unexpected character encountered"); }
            break;
    }
}

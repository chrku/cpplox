//
// Created by chrku on 08.08.2021.
//

#include "parser.h"

#include <algorithm>

Parser::Parser(std::shared_ptr<std::vector<Token>> tokens, std::shared_ptr<LoxInterpreter> interpreter)
    : tokens_(std::move(tokens)), interpreter_(std::move(interpreter)) {}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements{};

    while (!isAtEnd()) {
        statements.push_back(declaration());
    }

    return statements;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    try {
        return expression();
    } catch (const ParseError& e) {
        return {};
    }
}

void Parser::reset() {
    current_ = 0;
}

std::unique_ptr<Statement> Parser::declaration() {
    try {
        if (match({TokenType::VAR})) { return varDeclaration(); }
        return statement();
    } catch (const ParseError& e) {
        synchronize();
        return {};
    }
}

std::unique_ptr<Statement> Parser::varDeclaration() {
    const Token& name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expression> initializer{};
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");
    return std::make_unique<VariableDeclaration>(std::move(initializer), name);
}

std::unique_ptr<Statement> Parser::statement() {
    if (match({TokenType::PRINT})) {
        return printStatement();
    }
    if (match({TokenType::LEFT_BRACE})) {
        return std::make_unique<Block>(block());
    }

    return expressionStatement();
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    std::unique_ptr<Expression> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::vector<std::unique_ptr<Statement>> Parser::block() {
    std::vector<std::unique_ptr<Statement>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
    return statements;
}

std::unique_ptr<Statement> Parser::printStatement() {
    std::unique_ptr<Expression> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintStatement>(std::move(expr));
}

std::unique_ptr<Expression> Parser::expression() {
    return comma();
}

std::unique_ptr<Expression> Parser::comma() {
    auto left = assignment();
    while (match({TokenType::COMMA})) {
        auto op = previous();
        auto right = assignment();
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::assignment() {
    auto left = equality();

    if (match({TokenType::EQUAL})) {
        const Token& equals = previous();
        auto right = assignment();

        if (left->lvalue()) {
            auto* var_access = dynamic_cast<VariableAccess*>(left.get());
            if (var_access) {
                const Token& name = var_access->getToken();
                return std::make_unique<Assignment>(name, std::move(right));
            }
        }

        error(equals, "Invalid assignment target, has to be lvalue.");
    }

    return left;
}

std::unique_ptr<Expression> Parser::equality() {
    auto left = ternary();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        auto op = previous();
        auto right = ternary();
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::ternary() {
    auto left = comparison();
    while (match({TokenType::QUESTION_MARK})) {
        auto middle = expression();
        consume(TokenType::COLON, "Colon expected");
        auto right = comparison();
        left = std::make_unique<Ternary>(std::move(left), std::move(middle), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto left = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        auto op = previous();
        auto right = term();
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::term() {
    auto left = factor();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        auto op = previous();
        auto right = factor();
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::factor() {
    auto left = unary();

    while (match({TokenType::STAR, TokenType::SLASH})) {
        auto op = previous();
        auto right = unary();
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto op = previous();
        auto right = primary();
        return std::make_unique<Unary>(op, std::move(right));
    }

    return primary();
}

std::unique_ptr<Expression> Parser::primary() {
    if (match({TokenType::FALSE})) { return std::make_unique<Literal>(false); }
    if (match({TokenType::TRUE})) { return std::make_unique<Literal>(true); }
    if (match({TokenType::NIL})) { return std::make_unique<Literal>(); }

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        std::unique_ptr<Literal> literal;
        auto op = previous();

        std::visit(overload{
                [&](const double& d) { literal = std::make_unique<Literal>(d); },
                [&](const std::string& s) { literal = std::make_unique<Literal>(s); },
                [](const std::monostate&) { throw std::runtime_error("This should never happen"); },
        }, previous().getLiteral());

        return literal;
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableAccess>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expression> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}


bool Parser::match(std::initializer_list<TokenType> types) {
    if (std::any_of(types.begin(), types.end(), [&](TokenType t) { return check(t); })) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) { return false; }
    return peek().getType() == type;
}

const Token& Parser::advance() {
    if (!isAtEnd()) { current_++; }
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().getType() == TokenType::EOF_TYPE;
}

const Token& Parser::peek() const {
    return tokens_->at(current_);
}

const Token& Parser::previous() const {
    return tokens_->at(current_ - 1);
}

const Token& Parser::consume(TokenType type, std::string_view message) {
    if (check(type)) { return advance(); }
    throw error(peek(), message);
}

ParseError Parser::error(const Token& t, std::string_view message) {
    interpreter_->error(t, message);
    return ParseError{};
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().getType() == TokenType::SEMICOLON) { return; }

        switch (peek().getType()) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}



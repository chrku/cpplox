//
// Created by chrku on 08.08.2021.
//

#include "parser.h"

#include <algorithm>

Parser::Parser(std::shared_ptr<std::vector<Token>> tokens, std::shared_ptr<LoxInterpreter> interpreter)
    : tokens_(std::move(tokens)), interpreter_(std::move(interpreter)) {}

std::vector<std::shared_ptr<Statement>> Parser::parse() {
    std::vector<std::shared_ptr<Statement>> statements{};

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

std::shared_ptr<Statement> Parser::declaration() {
    try {
        if (match({TokenType::CLASS})) { return classDeclaration(); }
        if (check(TokenType::FUN) && checkNext(TokenType::IDENTIFIER)) {
            advance();
            return functionDeclaration();
        }
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
    if (match({TokenType::IF})) {
        return ifStatement();
    }
    if (match({TokenType::WHILE})) {
        return whileStatement();
    }
    if (match({TokenType::FOR})) {
        return forStatement();
    }
    if (match({TokenType::BREAK})) {
        return breakStatement();
    }
    if (match({TokenType::PRINT})) {
        return printStatement();
    }
    if (match({TokenType::RETURN})) {
        return returnStatement();
    }
    if (match({TokenType::LEFT_BRACE})) {
        auto statements = block();
        return std::make_unique<Block>(statements);
    }

    return expressionStatement();
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    std::unique_ptr<Expression> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::shared_ptr<Function> Parser::function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    std::vector<Token> params;

    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters");
            }

            params.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name"));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    auto body = block();

    return std::make_unique<Function>(name, params, body);
}

std::shared_ptr<Statement> Parser::functionDeclaration() {
    return function("function");
}

std::shared_ptr<Statement> Parser::classDeclaration() {
    auto name = consume(TokenType::IDENTIFIER, "Expect class name");
    consume(TokenType::LEFT_BRACE, "Expect '{' before class body");

    std::vector<std::shared_ptr<Function>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        methods.push_back(function("method"));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");

    return std::make_shared<ClassDeclaration>(name, std::move(methods));
}

std::vector<std::shared_ptr<Statement>> Parser::block() {
    std::vector<std::shared_ptr<Statement>> statements;

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

std::unique_ptr<Statement> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expression> value{};
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<Return>(keyword, std::move(value));
}

std::unique_ptr<Statement> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after if.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");

    auto thenBranch = statement();
    std::unique_ptr<Statement> elseBranch{};
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}


std::unique_ptr<Statement> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after while.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");

    openLoop();
    auto thenBranch = statement();
    closeLoop();

    return std::make_unique<WhileStatement>(std::move(condition), std::move(thenBranch));
}

std::unique_ptr<Statement> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after for.");

    // Parse initializer
    std::unique_ptr<Statement> initializer{};
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    // Parse condition
    std::unique_ptr<Expression> condition{};
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    // Parse increment
    std::unique_ptr<Expression> increment{};
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    // Parse body
    openLoop();
    std::unique_ptr<Statement> body = statement();
    closeLoop();

    // Desugaring
    std::vector<std::shared_ptr<Statement>> statements;
    statements.push_back(std::move(body));

    // Add increment if available
    if (increment) {
        statements.push_back(std::make_unique<ExpressionStatement>(std::move(increment)));
    }
    body = std::make_unique<Block>(statements);

    // Add condition
    if (condition) {
        body = std::make_unique<WhileStatement>(std::move(condition), std::move(body));
    }

    // Add initializer
    if (initializer) {
        statements.clear();
        statements.push_back(std::move(initializer));
        statements.push_back(std::move(body));
        body = std::make_unique<Block>(statements);
    }

    return body;
}

std::unique_ptr<Statement> Parser::breakStatement() {
    if (!isInLoop()) {
        throw error(previous(), "Can only use break within loop");
    }
    consume(TokenType::SEMICOLON, "Expect ';' after break.");
    return std::make_unique<BreakStatement>();
}

std::unique_ptr<Expression> Parser::expression(bool disable_comma) {
    return comma(disable_comma);
}

std::unique_ptr<Expression> Parser::comma(bool disable_comma) {
    auto left = assignment();

    if (!disable_comma) {
        while (match({TokenType::COMMA})) {
            auto op = previous();
            auto right = assignment();
            left = std::make_unique<Binary>(std::move(left), op, std::move(right));
        }
    }

    return left;
}

std::unique_ptr<Expression> Parser::assignment() {
    auto left = logicOr();

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

std::unique_ptr<Expression> Parser::logicOr() {
    auto left = logicAnd();

    while (match({TokenType::OR})) {
        auto op = previous();
        auto right = logicAnd();
        left = std::make_unique<Logical>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::logicAnd() {
    auto left = equality();

    while (match({TokenType::AND})) {
        auto op = previous();
        auto right = equality();
        left = std::make_unique<Logical>(std::move(left), op, std::move(right));
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

    return call();
}

std::unique_ptr<Expression> Parser::call() {
    auto expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else if (match({TokenType::DOT})) {
            auto name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<GetExpression>(std::move(expr), name);
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    if (match({TokenType::FALSE})) { return std::make_unique<Literal>(false); }
    if (match({TokenType::TRUE})) { return std::make_unique<Literal>(true); }
    if (match({TokenType::NIL})) { return std::make_unique<Literal>(); }
    if (match({TokenType::FUN})) { return handleFunctionExpression(); }

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

bool Parser::checkNext(TokenType type) const {
    if (isAtEnd()) { return false; }
    if (next().getType() == type) { return true;}
    return false;
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

const Token& Parser::next() const {
    return tokens_->at(current_ + 1);
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

bool Parser::isInLoop() const {
    return numLoops_ > 0;
}

void Parser::openLoop() {
    numLoops_++;
}

void Parser::closeLoop() {
    numLoops_--;
}

std::unique_ptr<Expression> Parser::finishCall(std::unique_ptr<Expression> callee) {
    std::vector<std::unique_ptr<Expression>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression(true));
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_unique<Call>(std::move(callee), paren, std::move(arguments));
}

std::unique_ptr<Expression> Parser::handleFunctionExpression() {
    std::vector<Token> params;

    consume(TokenType::LEFT_PAREN, "Expect '(' after anonymous function declaration.");
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters");
            }

            params.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name"));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters");
    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");

    auto body = block();

    return std::make_unique<FunctionExpression>(params, body);
}





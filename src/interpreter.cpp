//
// Created by chrku on 10.08.2021.
//

#include "interpreter.h"

#include "utils.h"
#include "lox.h"

#include <iostream>

RuntimeError::RuntimeError(Token t, const std::string &message) : std::runtime_error(message), token_(std::move(t)) {
}

const Token& RuntimeError::getToken() const {
    return token_;
}

void Interpreter::interpret(std::vector<std::unique_ptr<Statement>>& program,
                            const std::shared_ptr<LoxInterpreter>& context) {
    try {
        for (std::unique_ptr<Statement>& stmt : program) {
            execute(*stmt);
        }
    } catch (const RuntimeError& error) {
        context->runtimeError(error);
    }
}

LoxType Interpreter::evaluate(Expression& expr) {
    expr.accept(*this);
    return valueStack_.back();
}

void Interpreter::execute(Statement& statement) {
    statement.accept(*this);
}

void Interpreter::visitBinary(Binary& b) {
    evaluate(*b.getLeft());
    evaluate(*b.getRight());

    // Get result of left and right
    LoxType right_val = valueStack_.back();
    valueStack_.pop_back();
    LoxType left_val = valueStack_.back();
    valueStack_.pop_back();

    switch (b.getOperator().getType()) {
        case TokenType::MINUS:
            checkNumberOperands(b.getOperator(), left_val, right_val);
            valueStack_.emplace_back(toDouble(left_val) - toDouble(right_val));
            break;
        case TokenType::SLASH:
            checkNumberOperands(b.getOperator(), left_val, right_val);
            valueStack_.emplace_back(toDouble(left_val) / toDouble(right_val));
            break;
        case TokenType::STAR:
            checkNumberOperands(b.getOperator(), left_val, right_val);
            valueStack_.emplace_back(toDouble(left_val) * toDouble(right_val));
            break;

        case TokenType::PLUS:
            if (std::holds_alternative<double>(left_val) && std::holds_alternative<double>(right_val)) {
                double left = std::get<double>(left_val);
                double right = std::get<double>(right_val);
                valueStack_.emplace_back(left + right);
            } else if (std::holds_alternative<std::string>(left_val) && std::holds_alternative<std::string>(right_val)) {
                const std::string& left = std::get<std::string>(left_val);
                const std::string& right = std::get<std::string>(right_val);
                valueStack_.emplace_back(left + right);
            } else if ((std::holds_alternative<std::string>(left_val) && std::holds_alternative<double>(right_val)) ||
                    (std::holds_alternative<double>(left_val) && std::holds_alternative<std::string>(right_val))) {
                if (std::holds_alternative<double>(left_val)) {
                    const std::string& right = std::get<std::string>(right_val);
                    const std::string left = std::to_string(std::get<double>(left_val));
                    valueStack_.emplace_back(left + right);
                } else {
                    const std::string& left = std::get<std::string>(left_val);
                    const std::string right = std::to_string(std::get<double>(right_val));
                    valueStack_.emplace_back(left + right);
                }
            }
            else {
                throw RuntimeError(b.getOperator(), "Operands must be numbers or strings");
            }
            break;

        case TokenType::GREATER:
            checkNumberOperands(b.getOperator(), left_val, right_val);
            valueStack_.emplace_back(toDouble(left_val) > toDouble(right_val));
            break;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(b.getOperator(), left_val, right_val);
            valueStack_.emplace_back(toDouble(left_val) >= toDouble(right_val));
            break;
        case TokenType::LESS:
            checkNumberOperands(b.getOperator(), left_val, right_val);
            valueStack_.emplace_back(toDouble(left_val) < toDouble(right_val));
            break;
        case TokenType::LESS_EQUAL:
            checkNumberOperands(b.getOperator(), left_val, right_val);
            valueStack_.emplace_back(toDouble(left_val) <= toDouble(right_val));
            break;

        case TokenType::BANG_EQUAL:
            valueStack_.emplace_back(!isEqual(left_val, right_val));
            break;
        case TokenType::EQUAL_EQUAL:
            valueStack_.emplace_back(isEqual(left_val, right_val));
            break;
        case TokenType::COMMA:
            valueStack_.emplace_back(left_val);
            break;
        default:
            throw std::runtime_error("This should never happen.");
    }
}

void Interpreter::visitTernary(Ternary &t) {
    evaluate(*t.getLeft());

    LoxType result_val = valueStack_.back();
    valueStack_.pop_back();

    if (isTruthy(result_val)) {
        evaluate(*t.getMiddle());
    } else {
        evaluate(*t.getRight());
    }
}

void Interpreter::visitGrouping(Grouping& g) {
    evaluate(*g.getExpression());
}

void Interpreter::visitLiteral(Literal& l) {
    valueStack_.push_back(l.getValue());
}

void Interpreter::visitUnary(Unary& u) {
    evaluate(*u.getRight());

    // Get result of expression
    LoxType result_val = valueStack_.back();
    valueStack_.pop_back();

    switch (u.getOperator().getType()) {
        case TokenType::BANG:
            valueStack_.emplace_back(!isTruthy(result_val));
            break;
        case TokenType::MINUS:
            valueStack_.emplace_back(negate(u.getOperator(), result_val));
            break;
        default:
            throw std::runtime_error("This should never happen.");
    }
}

void Interpreter::visitVariableAccess(VariableAccess& v) {
    valueStack_.emplace_back(environment_.get(v.getToken()));
}

void Interpreter::visitExpressionStatement(ExpressionStatement& s) {
    // Evaluate and discard
    evaluate(*s.getExpression());
    valueStack_.pop_back();
}

void Interpreter::visitPrintStatement(PrintStatement &p) {
    evaluate(*p.getExpression());
    LoxType result_val = valueStack_.back();
    valueStack_.pop_back();

    std::cout << stringify(result_val) << '\n';
}

void Interpreter::visitVariableDeclaration(VariableDeclaration& v) {

    LoxType value = NullType{};

    if (v.getExpression()) {
        evaluate(*v.getExpression());
        value = valueStack_.back();
        valueStack_.pop_back();
    }

    environment_.define(v.getToken().getLexeme(), value);
}

bool Interpreter::isTruthy(const LoxType& t) {
    bool return_value;

    std::visit(overload{
            [&](double d) {
                return_value = true;
            },
            [&](const std::string& s) {
                return_value = true;
            },
            [&](const NullType& n) {
                return_value = false;
            },
            [&](bool b) {
                return_value = b;
            }
    }, t);

    return return_value;
}

double Interpreter::negate(const Token& op, const LoxType &t) {
    double return_value = 0;

    std::visit(overload{
            [&](double d) {
                return_value = -d;
            },
            [&](const std::string& s) {
                throw RuntimeError(op, "Cannot negate string.");
            },
            [&](const NullType& n) {
                throw RuntimeError(op, "Cannot negate null.");
            },
            [&](bool b) {
                return_value = -static_cast<double>(b);
            }
    }, t);

    return return_value;
}

double Interpreter::toDouble(const LoxType &t) {
    double return_value = 0;

    std::visit(overload{
            [&](double d) {
                return_value = d;
            },
            [&](const std::string& s) {
                return_value = std::numeric_limits<double>::quiet_NaN();
            },
            [&](const NullType& n) {
                return_value = std::numeric_limits<double>::quiet_NaN();
            },
            [&](bool b) {
                return_value = static_cast<double>(b);
            }
    }, t);

    return return_value;
}

bool Interpreter::isEqual(const LoxType& t1, const LoxType& t2) {
    bool return_value = false;

    std::visit(overload{
            [&](double d) {
                return_value = toDouble(t2) == d;
            },
            [&](const std::string& s) {
                if (std::holds_alternative<std::string>(t2)) {
                    const auto& s1 = std::get<std::string>(t2);
                    return_value = s1 == s;
                }
            },
            [&](const NullType& n) {
                if (std::holds_alternative<NullType>(t2)){
                    return_value = true;
                }
            },
            [&](bool b) {
                return_value = b == isTruthy(t2);
            }
    }, t1);

    return return_value;
}

void Interpreter::checkNumberOperands(const Token& op, const LoxType& t1, const LoxType& t2) {
    if (std::holds_alternative<double>(t1) && std::holds_alternative<double>(t2)) {
        return;
    }

    throw RuntimeError(op, "Operands must be numbers");
}



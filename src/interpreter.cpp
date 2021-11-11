//
// Created by chrku on 10.08.2021.
//

#include "interpreter.h"

#include "lox.h"
#include "loxfunction.h"
#include "native_functions/clock.h"

#include <iostream>

RuntimeError::RuntimeError(Token t, const std::string &message) : std::runtime_error(message), token_(std::move(t)) {
}

const Token& RuntimeError::getToken() const {
    return token_;
}

Interpreter::Interpreter()
: valueStack_{}, globals_{std::make_shared<Environment>()}, environment_{globals_}, outputStream_{&std::cout}
{
    globals_->define("clock",
                     std::make_shared<Clock>());
}


Interpreter::Interpreter(std::ostream *ostream)
: valueStack_{}, globals_{std::make_shared<Environment>()}, environment_{globals_}, outputStream_{ostream}
{
    globals_->define("clock",
                     std::make_shared<Clock>());
}

void Interpreter::interpret(std::vector<std::shared_ptr<Statement>>& program,
                            const std::shared_ptr<LoxInterpreter>& context) {
    try {
        for (std::shared_ptr<Statement>& stmt : program) {
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
    valueStack_.emplace_back(lookUpVariable(v.getToken(), &v));
}

void Interpreter::visitAssignment(Assignment& a) {
    evaluate(*a.getValue());
    // Get result of expression
    LoxType result_val = valueStack_.back();

    if (locals_.count(&a)) {
        environment_->assignAt(a.getName(), result_val, locals_[&a]);
    } else {
        globals_->assign(a.getName(), result_val);
    }
}

void Interpreter::visitLogical(Logical& l) {
    // Evaluate left first
    evaluate(*l.getLeft());
    LoxType left_val = valueStack_.back();
    valueStack_.pop_back();

    if (l.getOperator().getType() == TokenType::OR) {
        if (isTruthy(left_val)) {
            valueStack_.emplace_back(left_val);
            return;
        }
    } else {
        if (!isTruthy(left_val)) {
            valueStack_.emplace_back(left_val);
            return;
        }
    }

    evaluate(*l.getRight());
}

void Interpreter::visitCall(Call &c) {
    evaluate(*c.getCallee());
    LoxType left_val = valueStack_.back();
    valueStack_.pop_back();

    if (std::holds_alternative<std::shared_ptr<Callable>>(left_val)) {
        auto& callable = std::get<std::shared_ptr<Callable>>(left_val);

        std::vector<LoxType> arguments;
        for (auto& argument : c.getArguments()) {
            evaluate(*argument);
            LoxType result = valueStack_.back();
            valueStack_.pop_back();
            arguments.push_back(result);
        }

        if (arguments.size() == callable->arity()) {
            LoxType ret_val = callable->call(*this, arguments);
            valueStack_.push_back(ret_val);
        } else {
            throw RuntimeError(c.getParen(), "Expected " +
                                             std::to_string(callable->arity()) + " arguments but got " +
                                             std::to_string(arguments.size()) + ".");
        }
    } else {
        throw RuntimeError(c.getParen(), "Can only call functions and classes.");
    }
}

void Interpreter::visitFunctionExpression(FunctionExpression& f) {
    std::shared_ptr<LoxFunction> l = std::make_shared<LoxFunction>(f, getEnvironment());
    valueStack_.emplace_back(l);
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

    *outputStream_ << stringify(result_val) << '\n';
}

void Interpreter::visitVariableDeclaration(VariableDeclaration& v) {

    LoxType value = NullType{};

    if (v.getExpression()) {
        evaluate(*v.getExpression());
        value = valueStack_.back();
        valueStack_.pop_back();
    }

    environment_->define(v.getToken().getLexeme(), value);
}

void Interpreter::visitBlock(Block& b) {
    std::shared_ptr<Environment> new_environment = std::make_shared<Environment>();
    new_environment->setEnclosing(environment_);
    executeBlock(b.getStatements(), new_environment);
}

void Interpreter::visitIfStatement(IfStatement& i) {
    // Evaluate condition
    evaluate(*i.getCondition());
    LoxType condition = valueStack_.back();
    valueStack_.pop_back();

    if (isTruthy(condition)) {
        execute(*i.getThenBranch());
    } else if (i.getElseBranch()) {
        execute(*i.getElseBranch());
    }
}

void Interpreter::visitWhileStatement(WhileStatement &w) {
    // Evaluate condition
    evaluate(*w.getCondition());
    LoxType condition = valueStack_.back();
    valueStack_.pop_back();

    try {
        while (isTruthy(condition)) {
            execute(*w.getThenBranch());
            evaluate(*w.getCondition());
            condition = valueStack_.back();
            valueStack_.pop_back();
        }
    } catch (const BreakException& b) {
    }
}

void Interpreter::visitBreakStatement(BreakStatement &b) {
    throw BreakException{};
}

void Interpreter::visitFunction(Function& f) {
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(f, environment_);
    environment_->define(f.getName().getLexeme(), function);
}

void Interpreter::visitReturn(Return& r) {
    LoxType value = NullType{};
    if (r.getValue()) {
        value = evaluate(*r.getValue());
        valueStack_.pop_back();
    }

    throw ReturnException{value};
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
            },
            [&](const std::shared_ptr<Callable>& c) {
                return_value = true;
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
            },
            [&](const std::shared_ptr<Callable>& c) {
                throw RuntimeError(op, "Cannot negate callable.");
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
            },
            [&](const std::shared_ptr<Callable>& c) {
                return_value = std::numeric_limits<double>::quiet_NaN();
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
                if (std::holds_alternative<NullType>(t2)) {
                    return_value = true;
                }
            },
            [&](bool b) {
                return_value = b == isTruthy(t2);
            },
            [&](const std::shared_ptr<Callable>& c) {
                if (std::holds_alternative<std::shared_ptr<Callable>>(t2)) {
                    const auto& c1 = std::get<std::shared_ptr<Callable>>(t2);
                    return_value = c == c1;
                }
                return_value = false;
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

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Statement>>& statements,
                               std::shared_ptr<Environment> new_environment) {
    // Create a pointer to restore environment in case of error
    std::shared_ptr<Environment> prior_environment = environment_;
    try {
        environment_ = std::move(new_environment);

        for (const auto& statement : statements) {
            execute(*statement);
        }
    } catch(...) {
        environment_ = prior_environment;
        throw;
    }

    environment_ = prior_environment;
}

const std::shared_ptr<Environment>& Interpreter::getEnvironment() const {
    return environment_;
}

void Interpreter::resolve(Expression* expr, int depth) {
    locals_[expr] = depth;
}

LoxType Interpreter::lookUpVariable(const Token& name, Expression* expr) {
    if (locals_.count(expr)) {
        return environment_->getAt(name, locals_[expr]);
    } else {
        return globals_->get(name);
    }
}


ReturnException::ReturnException(const LoxType& value)
    : value_(value)
{

}

const LoxType& ReturnException::getValue() const {
    return value_;
}


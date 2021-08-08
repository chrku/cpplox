//
// Created by chrku on 08.08.2021.
//

#include "astprinter.h"

#include "utils.h"

#include <sstream>

void AstPrinter::visitBinary(Binary& b) {
    stream_ << "(";
    stream_ << b.getOperator().getLexeme();
    b.getLeft()->accept(*this);
    stream_ << " ";
    b.getRight()->accept(*this);
    stream_ << ")";
}

void AstPrinter::visitGrouping(Grouping &g) {
    stream_ << "(";
    g.getExpression()->accept(*this);
    stream_ << ")";
}

void AstPrinter::visitLiteral(Literal &l) {
    const auto& value = l.getValue();
    std::visit(overload{
            [&](double d) { stream_ << d; },
            [&](const std::string& s) { stream_ << s; },
            [&](const NullType& n) { stream_ << "nil"; },
            [&](bool b) {stream_ << b ? "true" : "false";}
    }, l.getValue());
}

void AstPrinter::visitUnary(Unary &u) {
    stream_ << "(";
    stream_ << u.getOperator().getLexeme();
    u.getRight()->accept(*this);
    stream_ << ")";
}

std::string AstPrinter::print(Expression& expr) {
    expr.accept(*this);
    return stream_.str();
}


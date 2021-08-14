//
// Created by chrku on 08.08.2021.
//

#ifndef LOX_ASTPRINTER_H
#define LOX_ASTPRINTER_H

#include "types.h"
#include "expressions.h"

#include <string>
#include <sstream>

class AstPrinter : public ExpressionVisitor {
public:
    std::string print(Expression& expr);
private:
    void visitBinary(Binary& b) override;
    void visitTernary(Ternary& t) override;
    void visitGrouping(Grouping& g) override;
    void visitLiteral(Literal& l) override;
    void visitUnary(Unary& u) override;

    ~AstPrinter() override = default;

    std::stringstream stream_{};
};


#endif //LOX_ASTPRINTER_H

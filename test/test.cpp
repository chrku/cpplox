//
// Created by Christoph Kummer on 11.11.21.
//

#include "lox.h"

#include <string_view>
#include <gtest/gtest.h>

void expectProgram(const char* filename,
                   std::string_view expected_stdout,
                   std::string_view expected_stderr) {
    std::stringstream out;
    std::stringstream err;
    std::shared_ptr<LoxInterpreter> interpreter = std::make_shared<LoxInterpreter>(&out, &err);
    interpreter->runFile(filename);
    EXPECT_EQ(out.str(), expected_stdout);
    EXPECT_EQ(err.str(), expected_stderr);
}

TEST(LoxTests, BreakTest) {
    expectProgram("examples/break.lox", "8281.000000\n", "");
}

TEST(LoxTests, BreakErrorTest1) {
    expectProgram("examples/break_error_1.lox", "",
                  "[line 1] Error at 'break': Can only use break within loop\n");
}

TEST(LoxTests, BreakErrorTest2) {
    expectProgram("examples/break_error_2.lox", "",
                  "[line 10] Error at 'break': Can only use break within loop\n");
}

TEST(LoxTests, ClassCall) {
    expectProgram("examples/class_call.lox", "", "");
}

TEST(LoxTests, ClassTest1) {
    expectProgram("examples/class_1.lox", "DevonshireCream\n",
                  "");
}

TEST(LoxTests, ClassTest2) {
    expectProgram("examples/class_2.lox", "",
                  "[Undefined property 'property'. line 7]\n");
}

TEST(LoxTests, ClassTest3) {
    expectProgram("examples/class_3.lox", "property\n",
                  "");
}

TEST(LoxTests, ClassTest4) {
    expectProgram("examples/class_4.lox", "Crunch crunch crunch!\n",
                  "");
}

TEST(LoxTests, ClassTest5) {
    expectProgram("examples/class_5.lox", "The German chocolate cake is delicious!\n",
                  "");
}

TEST(LoxTests, ClassTest6) {
    expectProgram("examples/class_6.lox", "Thing instance\n",
                  "");
}

TEST(LoxTests, ClockTest) {
    expectProgram("examples/clock.lox", "0.000000\n",
                  "");
}

TEST(LoxTests, CtorTest1) {
    expectProgram("examples/ctor_1.lox", "The south German chocolate cake is delicious!\n",
                  "");
}

TEST(LoxTests, ClosureTest) {
    expectProgram("examples/closure_test.lox", "global\nglobal\nblock\n",
                  "");
}

TEST(LoxTests, CounterTest) {
    expectProgram("examples/counter.lox", "1.000000\n2.000000\n",
                  "");
}

TEST(LoxTests, FibTest1) {
    expectProgram("examples/fib.lox", "0.000000\n1.000000\n1.000000\n2.000000\n3.000000\n"
                                      "5.000000\n8.000000\n13.000000\n21.000000\n34.000000\n55.000000\n89.000000"
                                      "\n144.000000\n233.000000\n377.000000\n610.000000\n987.000000\n1597.000000"
                                      "\n2584.000000\n4181.000000\n",
                  "");
}

TEST(LoxTests, FibTest2) {
    expectProgram("examples/fibo.lox", "0.000000\n1.000000\n1.000000\n2.000000\n3.000000\n"
                                      "5.000000\n8.000000\n13.000000\n21.000000\n34.000000\n55.000000\n89.000000"
                                      "\n144.000000\n233.000000\n377.000000\n610.000000\n987.000000\n1597.000000"
                                      "\n2584.000000\n4181.000000\n6765.000000\n",
                  "");
}

TEST(LoxTests, HiTest) {
    expectProgram("examples/hi.lox", "Hi, Dear Reader!\n",
                  "");
}

TEST(LoxTests, InheritanceTest1) {
    expectProgram("examples/inheritance_1.lox", "Fry until golden brown.\n",
                  "");
}

TEST(LoxTests, InheritanceTest2) {
    expectProgram("examples/inheritance_2.lox",
                  "Fry until golden brown.\nPipe full of custard and coat with chocolate.\n",
                  "");
}

TEST(LoxTests, IfTest1) {
    expectProgram("examples/if_1.lox", "2.000000\n4.000000\n",
                  "");
}

TEST(LoxTests, InvalidThis) {
    expectProgram("examples/invalidthis.lox", "",
                  "[line 1] Error at 'this': Can't use 'this' outside of a class.\n");
}

TEST(LoxTests, LogicalOps) {
    expectProgram("examples/logicalops.lox", "hi\nyes\n",
                  "");
}

TEST(LoxTests, RecursionTest) {
    expectProgram("examples/recursion.lox", "1.000000\n2.000000\n3.000000\n4.000000\n5.000000"
                                            "\n6.000000\n7.000000\n8.000000\n9.000000\n10.000000\n",
                  "");
}

TEST(LoxTests, ObjTest1) {
    expectProgram("examples/obj_1.lox", "Bagel instance\n",
                  "");
}

TEST(LoxTests, ReturnTest) {
    expectProgram("examples/return_error.lox", "",
                  "[line 1] Error at 'return': Can't return from top-level code.\n");
}

TEST(LoxTests, ScopeTest) {
    expectProgram("examples/scopes.lox", "inner a\nouter b\nglobal c\nouter a\nouter b\n"
                                         "global c\nglobal a\nglobal b\nglobal c\n",
                  "");
}

TEST(LoxTests, ScopeTest2) {
    expectProgram("examples/scopes_2.lox", "",
                  "[line 3] Error at 'a': Can't read local variable in its own initializer\n");
}

TEST(LoxTests, Thrice) {
    expectProgram("examples/thrice.lox", "1.000000\n2.000000\n3.000000\n",
                  "");
}

TEST(LoxTests, UndefinedVar1) {
    expectProgram("examples/undefined_var.lox", "",
                  "[line 1] Error at 'a': Undefined variable.\n");
}

TEST(LoxTests, Unused) {
    expectProgram("examples/unused.lox", "",
                  "[line 2] Error at 'a': Local variable not used.\n");
}

TEST(LoxTests, VarTest) {
    expectProgram("examples/var_test.lox", "",
                  "[line 3] Error at 'a': Can't read local variable in its own initializer\n");
}

TEST(LoxTests, WhileTest1) {
    expectProgram("examples/while_1.lox", "3.000000\n4.000000\n5.000000\n6.000000\n7.000000\n8.000000\n9.000000\n10.000000\n",
                  "");
}

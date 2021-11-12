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

TEST(LoxTests, ClockTest) {
    expectProgram("examples/clock.lox", "0.000000\n",
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


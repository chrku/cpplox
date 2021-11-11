//
// Created by Christoph Kummer on 11.11.21.
//

#include "lox.h"

#include <string_view>
#include <sstream>
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


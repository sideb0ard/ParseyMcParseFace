#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

#include "gtest/gtest.h"

#include "../evaluator.hpp"
#include "../lexer.hpp"
#include "../object.hpp"
#include "../parser.hpp"
#include "../token.hpp"

using namespace ast;
using namespace lexer;
using namespace token;
using namespace parser;
using namespace object;
using namespace evaluator;

namespace
{

struct EvaluatorTest : public ::testing::Test
{
};

bool TestIntegerObject(std::shared_ptr<Object> obj, int64_t expected)
{
    std::shared_ptr<Integer> io = std::dynamic_pointer_cast<Integer>(obj);
    if (!io)
        return false;

    if (io->value_ != expected)
    {
        std::cerr << "TEST INTEGER OBJECT - val not correct - actual:"
                  << io->value_ << " // expected:" << expected << std::endl;
        return false;
    }
    return true;
}

bool TestBooleanObject(std::shared_ptr<Object> obj, bool expected)
{
    std::shared_ptr<Boolean> bo = std::dynamic_pointer_cast<Boolean>(obj);
    if (!bo)
        return false;

    std::cout << "Testing Boolean! - object val:"
              << (bo->value_ ? "true" : "false")
              << " // Expected:" << (expected ? "true" : "false") << std::endl;
    if (bo->value_ != expected)
    {
        std::cerr << "TEST BOOLEAN OBJECT - val not correct - actual:"
                  << (bo->value_ ? "true" : "false")
                  << " // expected:" << (expected ? "true" : "false")
                  << std::endl;
        return false;
    }
    return true;
}

std::shared_ptr<Object> TestEval(std::string input)
{
    std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(input);
    std::unique_ptr<Parser> parsley = std::make_unique<Parser>(std::move(lex));
    EXPECT_FALSE(parsley->CheckErrors());

    std::shared_ptr<Program> program = parsley->ParseProgram();
    std::cout << "Program has " << program->statements_.size() << " statements"
              << std::endl;
    EXPECT_EQ(1, program->statements_.size());
    return Eval(std::move(program));
}

TEST_F(EvaluatorTest, TestIntegerExpression)
{
    struct TestCase
    {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests{{"5", 5},
                                {"10", 10},
                                {"-5", -5},
                                {"-10", -10},
                                {"5 + 5 + 5 + 5 - 10", 10},
                                {"2 * 2 * 2 * 2 * 2", 32},
                                {"-50 + 100 + -50", 0},
                                {"5 * 2 + 10", 20},
                                {"5 + 2 * 10", 25},
                                {"20 + 2 * -10 ", 0},
                                {"50 / 2 * 2 + 10", 60},
                                {"2 * (5 + 10)", 30},
                                {"3 * 3  * 3 + 10", 37},
                                {"3 * (3 * 3) + 10", 37},
                                {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}};
    for (auto tt : tests)
    {
        std::cout << "\nTesting! input: " << tt.input << std::endl;
        auto evaluated = TestEval(tt.input);
        EXPECT_TRUE(TestIntegerObject(evaluated, tt.expected));
    }
}

TEST_F(EvaluatorTest, TestBooleanExpression)
{
    struct TestCase
    {
        std::string input;
        bool expected;
    };
    std::vector<TestCase> tests{{"true", true},
                                {"false", false},
                                {"1 < 2", true},
                                {"1 > 2", false},
                                {"1 < 1", false},
                                {"1 > 1", false},
                                {"1 == 1", true},
                                {"1 != 1", false},
                                {"1 == 2", false},
                                {"1 != 2", true},
                                {"true == true", true},
                                {"false == false", true},
                                {"true == false", false},
                                {"true != false", true},
                                {"false != true", true},
                                {"(1<2) == true", true},
                                {"(1<2) == false", false},
                                {"(1 > 2) == true", false},
                                {"(1 > 2) == false", true}};
    for (auto tt : tests)
    {
        std::cout << "\nTesting! input: " << tt.input << std::endl;
        auto evaluated = TestEval(tt.input);
        EXPECT_TRUE(TestBooleanObject(evaluated, tt.expected));
    }
} // namespace

TEST_F(EvaluatorTest, TestBangOperator)
{
    struct TestCase
    {
        std::string input;
        bool expected;
    };
    std::vector<TestCase> tests{{"!true", false},   {"!false", true},
                                {"!5", false},      {"!!true", true},
                                {"!!false", false}, {"!!5", true}};
    for (auto tt : tests)
    {
        std::cout << "\nTesting! input: " << tt.input << std::endl;
        auto evaluated = TestEval(tt.input);
        EXPECT_TRUE(TestBooleanObject(evaluated, tt.expected));
    }
}

} // namespace

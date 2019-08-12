#include <variant>

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "../evaluator.hpp"
#include "../lexer.hpp"
#include "../object.hpp"
#include "../parser.hpp"
#include "../token.hpp"

namespace
{

struct EvaluatorTest : public ::testing::Test
{
};

bool TestIntegerObject(std::shared_ptr<object::Object> obj, int64_t expected)
{
    std::shared_ptr<object::Integer> io =
        std::dynamic_pointer_cast<object::Integer>(obj);
    if (!io)
    {
        std::cerr << "NOT AN INTEGER OBJECT\n";
        return false;
    }

    if (io->value_ != expected)
    {
        std::cerr << "TEST INTEGER OBJECT - val not correct - actual:"
                  << io->value_ << " // expected:" << expected << std::endl;
        return false;
    }
    return true;
}

bool TestBooleanObject(std::shared_ptr<object::Object> obj, bool expected)
{
    std::shared_ptr<object::Boolean> bo =
        std::dynamic_pointer_cast<object::Boolean>(obj);
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

std::shared_ptr<object::Object> TestEval(std::string input)
{
    auto lex = std::make_unique<lexer::Lexer>(input);
    auto parsley = std::make_unique<parser::Parser>(std::move(lex));
    EXPECT_FALSE(parsley->CheckErrors());

    auto program = parsley->ParseProgram();
    std::cout << "Program has " << program->statements_.size() << " statements"
              << std::endl;
    auto env = std::make_shared<object::Environment>();
    return evaluator::Eval(program, env);
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

TEST_F(EvaluatorTest, TestIfElseExpression)
{
    struct TestCaseInt
    {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCaseInt> tests{{"if (true) { 10 }", 10},
                                   {"if (1) { 10 }", 10},
                                   {"if (1 < 2) { 10 }", 10},
                                   {"if (1 > 2) { 10 } else { 20 }", 20},
                                   {"if (1 < 2) { 10 } else { 20 }", 10}};
    for (auto tt : tests)
    {
        std::cout << "\nTesting! input: " << tt.input << std::endl;
        auto evaluated = TestEval(tt.input);

        EXPECT_TRUE(TestIntegerObject(evaluated, tt.expected));
    }

    ////////////////////

    std::vector<std::string> nullTests{"if (false) { 10 }",
                                       "if (1 > 2) { 10 }"};
    for (auto tt : nullTests)
    {
        std::cout << "\nTesting! input: " << tt << std::endl;
        std::shared_ptr<object::Object> evaluated = TestEval(tt);
        EXPECT_EQ(evaluated, evaluator::NULLL);
    }
}

TEST_F(EvaluatorTest, TestReturnStatements)
{
    struct TestCase
    {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests{
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9", 10},
        {"if (10 > 1) { if (10 > 1) { return 10;} } return 1;}", 10}};
    for (auto tt : tests)
    {
        std::cout << "\nTesting! input: " << tt.input << std::endl;
        auto evaluated = TestEval(tt.input);
        EXPECT_TRUE(TestIntegerObject(evaluated, tt.expected));
    }
}

TEST_F(EvaluatorTest, TestErrorHandling)
{
    struct TestCase
    {
        std::string input;
        std::string expected;
    };
    std::vector<TestCase> tests{
        {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
        {"5 + true; 5", "type mismatch: INTEGER + BOOLEAN"},
        {"-true", "unknown operator: -BOOLEAN"},
        {"true + false;", "unknown operator: BOOLEAN + BOOLEAN"},
        {"5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"},
        {"if (10 > 1) { true + false; }",
         "unknown operator: BOOLEAN + BOOLEAN"},
        {"if (10 > 1) { if (10 > 1) { true + false; } return 1;}",
         "unknown operator: BOOLEAN + BOOLEAN"},
        {"foobar", "identifier not found: foobar"}};

    for (auto tt : tests)
    {
        std::cout << "Testing input:" << tt.input << std::endl;
        auto evaluated = TestEval(tt.input);
        std::shared_ptr<object::Error> err_obj =
            std::dynamic_pointer_cast<object::Error>(evaluated);
        EXPECT_TRUE(err_obj);
        if (!err_obj)
        {
            std::cerr << "No Error object returned. Got "
                      << typeid(evaluated).name() << "\n\n";
            continue;
        }
        EXPECT_EQ(err_obj->message_, tt.expected);
    }
}

TEST_F(EvaluatorTest, TestLetStatements)
{
    struct TestCase
    {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests{
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15}};

    for (auto tt : tests)
    {
        EXPECT_TRUE(TestIntegerObject(TestEval(tt.input), tt.expected));
    }
}

TEST_F(EvaluatorTest, TestFunctionObject)
{
    auto input = "fn(x) { x + 2; };";
    auto evaluated = TestEval(input);
    std::shared_ptr<object::Function> fn =
        std::dynamic_pointer_cast<object::Function>(evaluated);
    EXPECT_TRUE(fn);
    if (!fn)
    {
        std::cerr << "Object is not a function. Got "
                  << typeid(evaluated).name() << "\n\n";
    }
    EXPECT_EQ(fn->parameters_.size(), 1);
    EXPECT_EQ(fn->parameters_[0]->String(), "x");
    EXPECT_EQ(fn->body_->String(), "(x+2)");
}

TEST_F(EvaluatorTest, TestFunctionApplication)
{
    struct TestCase
    {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests{
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let add = fn(x,y) { x + y; }; add(5,5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5,5));", 20},
        {"fn(x) {x;}(5)", 5}};

    for (auto tt : tests)
    {
        EXPECT_TRUE(TestIntegerObject(TestEval(tt.input), tt.expected));
    }
}

TEST_F(EvaluatorTest, TestClosures)
{
    auto input = R"(let newAdder = fn(x) {
  fn(y) { x + y; };
};

let addTwo = newAdder(2);
addTwo(2);)";

    TestIntegerObject(TestEval(input), 4);
}

} // namespace

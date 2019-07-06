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
    std::cout << "CAST WAS All good!\n";

    if (io->value_ != expected)
    {
        std::cerr << "TEST INTEGER OBJECT - val not correct - actual:"
                  << io->value_ << " // expected:" << expected << std::endl;
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

TEST_F(EvaluatorTest, TestEvalIntegerExpression)
{
    struct TestCase
    {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests{{"5", 5}, {"10", 10}};
    for (auto tt : tests)
    {
        std::cout << "\nTesting! input: " << tt.input << std::endl;
        auto evaluated = TestEval(tt.input);
        EXPECT_TRUE(TestIntegerObject(evaluated, tt.expected));
    }
}

} // namespace

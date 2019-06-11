#include <iostream>
#include <memory>

#include "gtest/gtest.h"

#include "lexer.hpp"
#include "parser.hpp"
#include "token.hpp"

using namespace ast;
using namespace lexer;
using namespace token;
using namespace parser;

namespace
{

struct ParserTest : public ::testing::Test
{
    std::unique_ptr<Parser> parsley_;
    std::unique_ptr<Program> program_;

    std::string input = R"(
let x = 5;
let y = 10;
let foobar = 838383;
)";

    std::vector<std::string> tests = {
        {"x"},
        {"y"},
        {"foobar"},
    };

    ParserTest()
    {
        std::cout << "Parsey Test setup!\n";
        std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(input);
        parsley_ = std::make_unique<Parser>(std::move(lex));
        program_ = parsley_->ParseProgram();
    }
};

bool TestLetStatement(std::shared_ptr<Statement> s, std::string name)
{
    std::cout << "Literal is " << s->TokenLiteral() << std::endl;

    if (s->TokenLiteral().compare("let") != 0)
        return false;

    std::cout << "All good!\n";

    std::cout << "s is of type " << typeid(s).name() << std::endl;

    std::shared_ptr<LetStatement> ls =
        std::dynamic_pointer_cast<LetStatement>(s);
    if (!ls)
        return false;
    std::cout << "CAST WAS All good!\n";

    if (ls->name_.value_.compare(name) != 0)
        return false;
    std::cout << "NAME WAS All good!\n";

    if (ls->name_.TokenLiteral().compare(name) != 0)
        return false;
    std::cout << "NAME LITERAL WAS All good!\n";

    return true;
}

TEST_F(ParserTest, TestLetStatements)
{
    const int tests_len = tests.size();
    ASSERT_EQ(tests_len, program_->statements_.size());
    for (int i = 0; i < tests_len; i++)
    {
        std::shared_ptr<Statement> stmt = program_->statements_[i];
        std::string tt = tests[i];
        EXPECT_TRUE(TestLetStatement(stmt, tt));
    }
}

} // namespace

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

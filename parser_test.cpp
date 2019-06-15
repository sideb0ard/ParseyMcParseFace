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

    std::cout << "Parsey Test setup!\n";
    std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(input);
    std::unique_ptr<Parser> parsley_ = std::make_unique<Parser>(std::move(lex));
    std::unique_ptr<Program> program_ = parsley_->ParseProgram();
    EXPECT_FALSE(parsley_->CheckErrors());

    const int tests_len = tests.size();
    ASSERT_EQ(tests_len, program_->statements_.size());
    for (int i = 0; i < tests_len; i++)
    {
        std::shared_ptr<Statement> stmt = program_->statements_[i];
        std::string tt = tests[i];
        EXPECT_TRUE(TestLetStatement(stmt, tt));
    }
}

bool TestReturnStatement(std::shared_ptr<Statement> s)
{
    std::cout << "Literal is " << s->TokenLiteral() << std::endl;

    if (s->TokenLiteral().compare("return") != 0)
        return false;

    return true;
}

TEST_F(ParserTest, TestReturnStatements)
{
    std::string input = R"(
return 5;
return 10;
return 993322;
)";

    std::cout << "Return Statements Test setup!\n";
    std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(input);
    std::unique_ptr<Parser> parsley_ = std::make_unique<Parser>(std::move(lex));
    std::unique_ptr<Program> program_ = parsley_->ParseProgram();
    EXPECT_FALSE(parsley_->CheckErrors());

    ASSERT_EQ(3, program_->statements_.size());
    int len = program_->statements_.size();
    for (int i = 0; i < len; i++)
    {
        std::shared_ptr<Statement> stmt = program_->statements_[i];
        EXPECT_TRUE(TestReturnStatement(stmt));
    }
}

TEST_F(ParserTest, TestString)
{
    std::string input = R"(let myVar = anotherVar;)";

    Token toke{LET, "let"};
    std::shared_ptr<LetStatement> stmt = std::make_shared<LetStatement>(toke);
    Token name{IDENT, "myVar"};
    stmt->name_ = Identifier(name, "myVar");
    Token val{IDENT, "anotherVar"};
    stmt->value_ = Identifier(val, "anotherVar");

    std::unique_ptr<Program> program = std::make_unique<Program>();
    program->statements_.push_back(stmt);

    ASSERT_EQ(input, program->String());
}

} // namespace

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

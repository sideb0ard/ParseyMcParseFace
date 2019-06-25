#include <iostream>
#include <memory>
#include <variant>

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

    if (ls->name_->value_.compare(name) != 0)
        return false;
    std::cout << "NAME WAS All good!\n";

    if (ls->name_->TokenLiteral().compare(name) != 0)
        return false;
    std::cout << "NAME LITERAL WAS All good!\n";

    return true;
}

bool TestIdentifier(std::shared_ptr<Expression> expr, std::string val)
{
    std::shared_ptr<Identifier> ident =
        std::dynamic_pointer_cast<Identifier>(expr);
    if (!ident)
        return false;
    if (ident->value_ != val)
        return false;
    if (ident->TokenLiteral() != val)
        return false;
    return true;
}

bool TestIntegerLiteral(std::shared_ptr<Expression> expr, int64_t value)
{
    std::shared_ptr<IntegerLiteral> integ =
        std::dynamic_pointer_cast<IntegerLiteral>(expr);
    if (!integ)
        return false;
    if (integ->value_ != value)
        return false;
    if (integ->TokenLiteral() != std::to_string(value))
        return false;
    return true;
}

bool TestLiteralExpression(std::shared_ptr<Expression> expr,
                           std::variant<int64_t, std::string, bool> val)
{

    if (const auto int_ptr(std::get_if<int64_t>(&val)); int_ptr)
    {
        std::cout << "int!" << *int_ptr << "\n";
        return TestIntegerLiteral(expr, *int_ptr);
    }
    else if (const auto string_ptr(std::get_if<std::string>(&val)); string_ptr)
    {
        std::cout << "string!" << *string_ptr << "\n";
        return TestIdentifier(expr, *string_ptr);
    }
    else if (const auto bool_ptr(std::get_if<bool>(&val)); bool_ptr)
    {
        std::cout << "bool!" << *bool_ptr << "\n";
    }
    return false;
}

bool TestBooleanLiteral(std::shared_ptr<Expression> expr, bool val)
{
    // std::shared_ptr<Boolean> boool;
    return true;
}

bool TestInfixExpression(std::shared_ptr<Expression> expr,
                         std::variant<int64_t, std::string, bool> left,
                         std::string op,
                         std::variant<int64_t, std::string, bool> right)
{
    std::shared_ptr<InfixExpression> op_expr =
        std::dynamic_pointer_cast<InfixExpression>(expr);
    if (!op_expr)
    {
        std::cerr << "Not an InfixExpression - got " << typeid(&expr).name();
        return false;
    }

    if (!TestLiteralExpression(op_expr->left_, left))
        return false;

    if (op_expr->operator_.compare(op) != 0)
        return false;

    if (!TestLiteralExpression(op_expr->right_, right))
        return false;

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
    std::unique_ptr<Parser> parsley = std::make_unique<Parser>(std::move(lex));
    std::unique_ptr<Program> program = parsley->ParseProgram();
    EXPECT_FALSE(parsley->CheckErrors());

    const int tests_len = tests.size();
    ASSERT_EQ(tests_len, program->statements_.size());
    for (int i = 0; i < tests_len; i++)
    {
        std::shared_ptr<Statement> stmt = program->statements_[i];
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
    std::unique_ptr<Parser> parsley = std::make_unique<Parser>(std::move(lex));
    std::unique_ptr<Program> program = parsley->ParseProgram();
    EXPECT_FALSE(parsley->CheckErrors());

    ASSERT_EQ(3, program->statements_.size());
    int len = program->statements_.size();
    for (int i = 0; i < len; i++)
    {
        std::shared_ptr<Statement> stmt = program->statements_[i];
        EXPECT_TRUE(TestReturnStatement(stmt));
    }
}

TEST_F(ParserTest, TestString)
{
    std::string input = R"(let myVar = anotherVar;)";

    Token toke{LET, "let"};
    std::shared_ptr<LetStatement> stmt = std::make_shared<LetStatement>(toke);
    Token name{IDENT, "myVar"};
    stmt->name_ = std::make_shared<Identifier>(name, "myVar");
    Token val{IDENT, "anotherVar"};
    stmt->value_ = std::make_shared<Identifier>(val, "anotherVar");

    std::unique_ptr<Program> program = std::make_unique<Program>();
    program->statements_.push_back(stmt);

    ASSERT_EQ(input, program->String());
}

TEST_F(ParserTest, TestIdentifierExpression)
{
    std::string input = "foobar";
    std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(input);
    std::unique_ptr<Parser> parsley = std::make_unique<Parser>(std::move(lex));
    std::unique_ptr<Program> program = parsley->ParseProgram();
    EXPECT_FALSE(parsley->CheckErrors());
    ASSERT_EQ(1, program->statements_.size());

    std::shared_ptr<ExpressionStatement> stmt =
        std::dynamic_pointer_cast<ExpressionStatement>(program->statements_[0]);
    if (!stmt)
        FAIL() << "program->statements_[0] is not an ExpressionStatement";

    std::shared_ptr<Identifier> ident =
        std::dynamic_pointer_cast<Identifier>(stmt->expression_);
    if (!ident)
        FAIL() << "Not an Identifier - got "
               << typeid(&stmt->expression_).name();
    ASSERT_EQ(ident->value_, "foobar");
    ASSERT_EQ(ident->TokenLiteral(), "foobar");
}

TEST_F(ParserTest, TestIntegerLiteralExpression)
{
    std::string input = "5";
    std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(input);
    std::unique_ptr<Parser> parsley = std::make_unique<Parser>(std::move(lex));
    std::unique_ptr<Program> program = parsley->ParseProgram();
    EXPECT_FALSE(parsley->CheckErrors());
    ASSERT_EQ(1, program->statements_.size());

    std::shared_ptr<ExpressionStatement> stmt =
        std::dynamic_pointer_cast<ExpressionStatement>(program->statements_[0]);
    if (!stmt)
        FAIL() << "program_->statements_[0] is not an ExpressionStatement";

    std::shared_ptr<IntegerLiteral> literal =
        std::dynamic_pointer_cast<IntegerLiteral>(stmt->expression_);
    if (!literal)
        FAIL() << "Not an IntegerLiteral - got "
               << typeid(&stmt->expression_).name();
    ASSERT_EQ(literal->value_, 5);
    ASSERT_EQ(literal->TokenLiteral(), "5");
}

TEST_F(ParserTest, TestPrefixExpressions)
{

    struct TestCase
    {
        std::string input;
        std::string op;
        int64_t integer_value;
    };
    std::vector<TestCase> prefix_tests{{"!5", "!", 5}, {"-15", "-", 15}};
    for (auto tt : prefix_tests)
    {
        std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(tt.input);
        std::unique_ptr<Parser> parsley =
            std::make_unique<Parser>(std::move(lex));
        std::unique_ptr<Program> program = parsley->ParseProgram();
        EXPECT_FALSE(parsley->CheckErrors());
        ASSERT_EQ(1, program->statements_.size());

        std::shared_ptr<ExpressionStatement> stmt =
            std::dynamic_pointer_cast<ExpressionStatement>(
                program->statements_[0]);
        if (!stmt)
            FAIL() << "program_->statements_[0] is not an ExpressionStatement";

        std::shared_ptr<PrefixExpression> expr =
            std::dynamic_pointer_cast<PrefixExpression>(stmt->expression_);
        if (!expr)
            FAIL() << "Not a Prefix Expression - got "
                   << typeid(&stmt->expression_).name();
        ASSERT_EQ(expr->operator_, tt.op);
        ASSERT_TRUE(TestIntegerLiteral(expr->right_, tt.integer_value));

        std::cout << "JHER!\n";
        std::cout << program->String() << std::endl;
    }
}

TEST_F(ParserTest, TestInfixExpressions)
{
    struct TestCase
    {
        std::string input;
        int64_t left_value;
        std::string op;
        int64_t right_value;
    };
    std::vector<TestCase> infix_tests{
        {"5 + 5", 5, "+", 5},   {"5 - 5", 5, "-", 5},  {"5 * 5", 5, "*", 5},
        {"5 / 5", 5, "/", 5},   {"5 > 5", 5, ">", 5},  {"5 < 5", 5, "<", 5},
        {"5 == 5", 5, "==", 5}, {"5 != 5", 5, "!=", 5}};

    for (auto tt : infix_tests)
    {
        std::cout << "Testing with input = " << tt.input << std::endl;
        std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(tt.input);
        std::unique_ptr<Parser> parsley =
            std::make_unique<Parser>(std::move(lex));
        std::unique_ptr<Program> program = parsley->ParseProgram();
        EXPECT_FALSE(parsley->CheckErrors());

        std::shared_ptr<ExpressionStatement> stmt =
            std::dynamic_pointer_cast<ExpressionStatement>(
                program->statements_[0]);
        if (!stmt)
            FAIL() << "program_->statements_[0] is not an ExpressionStatement";

        std::variant<int64_t, std::string, bool> left = tt.left_value;
        std::variant<int64_t, std::string, bool> right = tt.right_value;
        ASSERT_TRUE(TestInfixExpression(stmt->expression_, left, tt.op, right));
        // std::cout << program->String() << std::endl;
    }
}
TEST_F(ParserTest, TestOperatorPrecedence)
{
    struct TestCase
    {
        std::string input;
        std::string expected;
    };
    std::vector<TestCase> tests{
        {"-a * b", "((-a)*b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a+b)+c)"},
        {"a + b - c", "((a+b)-c)"},
        {"a * b * c", "((a*b)*c)"},
        {"a * b / c", "((a*b)/c)"},
        {"a + b / c", "(a+(b/c))"},
        {"a + b * c + d / e - f", "(((a+(b*c))+(d/e))-f)"},
        {"3 + 4; -5 * 5", "(3+4)((-5)*5)"},
        {"5 > 5 == 3 < 4", "((5>5)==(3<4))"},
        {"5 < 4 != 3 > 4", "((5<4)!=(3>4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3+(4*5))==((3*1)+(4*5)))"}};

    for (auto tt : tests)
    {
        std::cout << "Testing with input = " << tt.input << std::endl;
        std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(tt.input);
        std::unique_ptr<Parser> parsley =
            std::make_unique<Parser>(std::move(lex));
        std::unique_ptr<Program> program = parsley->ParseProgram();
        EXPECT_FALSE(parsley->CheckErrors());

        auto actual = program->String();
        std::cout << program->String() << std::endl;
        ASSERT_EQ(actual, tt.expected);
    }
}

} // namespace

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

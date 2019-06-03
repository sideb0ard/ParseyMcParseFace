#include <iostream>

#include "gtest/gtest.h"

#include "lexer.hpp"
#include "lexical_token.hpp"

using namespace lexer;

namespace
{

struct LexerTest : public ::testing::Test
{
    std::unique_ptr<Lexer> lex;

    std::string input = R"(let five = 5;
let ten = 10;

let add = fn(x, y) {
    x + y;
};

let result = add(five, ten);
!-/*5;
5 < 10 > 5;

if (5 < 10) {
    return true;
} else {
    return false;
}

10 == 10;
10 != 9;
)";

    std::vector<std::pair<TokenType, std::string>> testTokens = {
        {LET, "let"},      {IDENT, "five"},    {ASSIGN, "="},
        {INT, "5"},        {SEMICOLON, ";"},   {LET, "let"},
        {IDENT, "ten"},    {ASSIGN, "="},      {INT, "10"},
        {SEMICOLON, ";"},  {LET, "let"},       {IDENT, "add"},
        {ASSIGN, "="},     {FUNCTION, "fn"},   {LPAREN, "("},
        {IDENT, "x"},      {COMMA, ","},       {IDENT, "y"},
        {RPAREN, ")"},     {LBRACE, "{"},      {IDENT, "x"},
        {PLUS, "+"},       {IDENT, "y"},       {SEMICOLON, ";"},
        {RBRACE, "}"},     {SEMICOLON, ";"},   {LET, "let"},
        {IDENT, "result"}, {ASSIGN, "="},      {IDENT, "add"},
        {LPAREN, "("},     {IDENT, "five"},    {COMMA, ","},
        {IDENT, "ten"},    {RPAREN, ")"},      {SEMICOLON, ";"},
        {BANG, "!"},       {MINUS, "-"},       {SLASH, "/"},
        {ASTERISK, "*"},   {INT, "5"},         {SEMICOLON, ";"},
        {INT, "5"},        {LT, "<"},          {INT, "10"},
        {GT, ">"},         {INT, "5"},         {SEMICOLON, ";"},
        {IF, "if"},        {LPAREN, "("},      {INT, "5"},
        {LT, "<"},         {INT, "10"},        {RPAREN, ")"},
        {LBRACE, "{"},     {RETURN, "return"}, {TRUE, "true"},
        {SEMICOLON, ";"},  {RBRACE, "}"},      {ELSE, "else"},
        {LBRACE, "{"},     {RETURN, "return"}, {FALSE, "false"},
        {SEMICOLON, ";"},  {RBRACE, "}"},      {INT, "10"},
        {EQ, "=="},        {INT, "10"},        {SEMICOLON, ";"},
        {INT, "10"},       {NE, "!="},         {INT, "9"},
        {SEMICOLON, ";"}};

    LexerTest()
    {
        std::cout << "Parsey setup!\n";
        lex = std::make_unique<Lexer>(input);
    }
};

TEST_F(LexerTest, TestNextToken)
{
    for (auto tt : testTokens)
    {
        Token tok = lex->NextToken();
        EXPECT_EQ(tok.type, tt.first);
        EXPECT_EQ(tok.literal, tt.second);
    }
}

} // namespace

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

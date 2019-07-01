#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

using namespace ast;
using namespace lexer;
using namespace token;

namespace parser
{

enum class Precedence
{
    _,
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL
};

const std::unordered_map<TokenType, Precedence> precedences{
    {EQ, Precedence::EQUALS},      {NOT_EQ, Precedence::EQUALS},
    {LT, Precedence::LESSGREATER}, {GT, Precedence::LESSGREATER},
    {PLUS, Precedence::SUM},       {MINUS, Precedence::SUM},
    {SLASH, Precedence::PRODUCT},  {ASTERISK, Precedence::PRODUCT},
    {LPAREN, Precedence::CALL}};

class Parser
{
  public:
    Parser(std::unique_ptr<Lexer> lexer);

    std::unique_ptr<Program> ParseProgram();
    bool CheckErrors();

  private:
    std::shared_ptr<Statement> ParseStatement();
    std::shared_ptr<LetStatement> ParseLetStatement();
    std::shared_ptr<ReturnStatement> ParseReturnStatement();
    std::shared_ptr<ExpressionStatement> ParseExpressionStatement();
    std::shared_ptr<Expression> ParseExpression(Precedence p);
    std::shared_ptr<Expression> ParseIdentifier();
    std::shared_ptr<Expression> ParseIntegerLiteral();
    std::shared_ptr<Expression> ParseBoolean();
    std::shared_ptr<Expression> ParseForPrefixExpression();
    std::shared_ptr<Expression> ParsePrefixExpression();
    std::shared_ptr<Expression>
    ParseInfixExpression(std::shared_ptr<Expression> left);
    std::shared_ptr<Expression> ParseGroupedExpression();
    std::shared_ptr<Expression> ParseIfExpression();

    std::shared_ptr<Expression> ParseFunctionLiteral();
    std::vector<std::shared_ptr<Identifier>> ParseFunctionParameters();

    std::shared_ptr<Expression>
    ParseCallExpression(std::shared_ptr<Expression> funct);
    std::vector<std::shared_ptr<Expression>> ParseCallArguments();

    std::shared_ptr<BlockStatement> ParseBlockStatement();

    bool ExpectPeek(TokenType t);
    bool CurTokenIs(TokenType t) const;
    bool PeekTokenIs(TokenType t) const;

    void PeekError(TokenType t);

    Precedence PeekPrecedence() const;
    Precedence CurPrecedence() const;
    void NextToken();

  private:
    std::unique_ptr<Lexer> lexer_;
    Token cur_token_;
    Token peek_token_;

    std::vector<std::string> errors_;
};

} // namespace parser

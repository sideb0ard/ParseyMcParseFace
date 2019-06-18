#pragma once

#include <optional>
#include <sstream>
#include <string>
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
    std::shared_ptr<Expression> ParsePrefixExpression();

    bool ExpectPeek(TokenType t);
    bool CurTokenIs(TokenType t);
    bool PeekTokenIs(TokenType t);

    void PeekError(TokenType t);
    void NextToken();

  private:
    std::unique_ptr<Lexer> lexer_;
    Token cur_token_;
    Token peek_token_;

    std::vector<std::string> errors_;
};

} // namespace parser

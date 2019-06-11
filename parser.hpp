#pragma once

#include <optional>

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

using namespace ast;
using namespace lexer;
using namespace token;

namespace parser
{

class Parser
{
  public:
    Parser(std::unique_ptr<Lexer> lexer) : lexer_{std::move(lexer)}
    {
        NextToken();
        NextToken();
    }

    std::unique_ptr<Program> ParseProgram();

  private:
    std::shared_ptr<Statement> ParseStatement();
    std::shared_ptr<LetStatement> ParseLetStatement();

    bool ExpectPeek(TokenType t)
    {
        if (PeekTokenIs(t))
        {
            NextToken();
            return true;
        }
        else
        {
            return false;
        }
    }

    void NextToken()
    {
        cur_token_ = peek_token_;
        peek_token_ = lexer_->NextToken();
    }

    bool CurTokenIs(TokenType t)
    {
        if (cur_token_.type.compare(t) == 0)
            return true;
        return false;
    }

    bool PeekTokenIs(TokenType t)
    {
        if (peek_token_.type.compare(t) == 0)
            return true;
        return false;
    }

  private:
    std::unique_ptr<Lexer> lexer_;
    Token cur_token_;
    Token peek_token_;
};

} // namespace parser

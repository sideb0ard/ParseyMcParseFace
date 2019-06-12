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

class Parser
{
  public:
    Parser(std::unique_ptr<Lexer> lexer) : lexer_{std::move(lexer)}
    {
        NextToken();
        NextToken();
    }

    std::unique_ptr<Program> ParseProgram();

    bool CheckErrors();

  private:
    std::shared_ptr<Statement> ParseStatement();
    std::shared_ptr<LetStatement> ParseLetStatement();
    std::shared_ptr<ReturnStatement> ParseReturnStatement();

    bool ExpectPeek(TokenType t)
    {
        std::cout << "  ExpectPeek - looking at " << peek_token_.type_
                  << " and expect it to be " << t << std::endl;
        if (PeekTokenIs(t))
        {
            NextToken();
            return true;
        }
        else
        {
            PeekError(t);
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
        if (cur_token_.type_.compare(t) == 0)
            return true;
        return false;
    }

    bool PeekTokenIs(TokenType t)
    {
        if (peek_token_.type_.compare(t) == 0)
            return true;
        return false;
    }

    void PeekError(TokenType t)
    {
        std::stringstream msg;
        msg << "Expected next token to be " << t << ", got "
            << peek_token_.type_ << " instead";
        errors_.push_back(msg.str());
    }

  private:
    std::unique_ptr<Lexer> lexer_;
    Token cur_token_;
    Token peek_token_;

    std::vector<std::string> errors_;
};

} // namespace parser

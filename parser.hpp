#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

using namespace ast;
using namespace lexer;

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

    Program ParseProgram() { return Program(); }

    void NextToken()
    {
        cur_token_ = peek_token_;
        peek_token_ = lexer_->NextToken();
    }

  private:
    std::unique_ptr<Lexer> lexer_;
    Token cur_token_;
    Token peek_token_;
};

} // namespace parser

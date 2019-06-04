#pragma once

#include <string>

#include "token.hpp"

using namespace token;

namespace lexer
{

class Lexer
{
  public:
    Lexer(std::string input);
    Token NextToken();

  private:
    void ReadChar();
    char PeekChar();
    std::string ReadIdentifier();
    std::string ReadNumber();
    void SkipWhiteSpace();

  private:
    std::string input_;
    char current_char_{0};
    int current_position_{0};
    int next_position_{0};
};

} // namespace lexer

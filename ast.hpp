#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "token.hpp"

using namespace token;

namespace ast
{

class Node
{
  public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
};

///////////////////

class Expression : public Node
{
  public:
    Expression() {}
    Expression(Token token) : token_{token} {}
    std::string TokenLiteral() const override { return token_.literal_; }

  private:
    Token token_;
};

class Identifier : public Expression
{
  public:
    Identifier() {}
    Identifier(Token token, std::string val) : Expression{token}, value_{val} {}
    std::string value_;
};

////////////////////////

class Statement : public Node
{
  public:
    Statement(Token toke) : token_{toke} {};
    std::string TokenLiteral() const override { return token_.literal_; }

  protected:
    Token token_;
};

class LetStatement : public Statement
{
  public:
    LetStatement(Token toke) : Statement(toke){};
    std::string TokenLiteral() const override { return token_.literal_; }

  public:
    Identifier name_;
    Expression value_;
};

class ReturnStatement : public Statement
{
  public:
    ReturnStatement(Token toke) : Statement(toke){};
    std::string TokenLiteral() const override { return token_.literal_; }

  public:
    Expression ReturnValue;
};

// ROOT //////////////////////

class Program : public Node
{
  public:
    Program() { std::cout << " Constructing Program\n"; }
    std::string TokenLiteral() const override;

  public:
    std::vector<std::shared_ptr<Statement>> statements_;
};

} // namespace ast

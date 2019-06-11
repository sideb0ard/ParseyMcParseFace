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
    std::string TokenLiteral() const override { return token_.literal; }
    virtual void ExpressionNode() {}

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
    virtual void StatementNode(){};
    std::string TokenLiteral() const override
    {
        std::cout << "\nBASE stmt\n";
        return token_.literal;
    }

  protected:
    Token token_;
};

class LetStatement : public Statement
{
  public:
    LetStatement(Token toke);
    std::string TokenLiteral() const override
    {
        std::cout << "\nBEEP\n";
        return token_.literal;
    }
    Identifier name_;
    Expression value_;
};

// ROOT //////////////////////

class Program : public Node
{
  public:
    std::string TokenLiteral() const override;

  public:
    std::vector<std::shared_ptr<Statement>> statements_;
};

} // namespace ast

#pragma once

#include <string>
#include <vector>

#include "token.hpp"

using namespace token;

namespace ast
{

class Node
{
  public:
    virtual std::string TokenLiteral();

  private:
    TokenType literal_;
};

///////////////////

class Expression : public Node
{
  public:
    virtual void ExpressionNode(){};
};

class Identifier : public Expression
{
  public:
    void ExpressionNode() override{};
    std::string TokenLiteral() override { return token_.literal; };

  private:
    Token token_;
    std::string value_;
};

////////////////////////

class Statement : public Node
{
  public:
    virtual void StatementNode(){};
};

class LetStatement : public Statement
{
  public:
    void StatementNode() override{};
    std::string TokenLiteral() override { return token_.literal; };

  private:
    Token token_;
    Identifier name_;
    Expression value_;
};

// ROOT //////////////////////

class Program : public Node
{
  public:
    std::string TokenLiteral() override;

  private:
    std::vector<Statement> statements_;
};

} // namespace ast

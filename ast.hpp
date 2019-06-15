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
    Node() = default;
    Node(Token toke) : token_{toke} {}
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const { return token_.literal_; }
    virtual std::string String() const = 0;

  public:
    Token token_;
};

///////////////////

class Expression : public Node
{
  public:
    Expression() {}
    Expression(Token token, std::string val) : Node{token}, value_{val} {}
    virtual std::string String() const override { return value_; }
    std::string value_;
};

class Identifier : public Expression
{
  public:
    Identifier() {}
    Identifier(Token token, std::string val) : Expression{token, val} {}
};

////////////////////////

class Statement : public Node
{
  public:
    Statement(Token toke) : Node{toke} {};
};

class LetStatement : public Statement
{
  public:
    LetStatement(Token toke) : Statement(toke){};
    std::string String() const override;

  public:
    Identifier name_;
    Expression value_;
};

class ReturnStatement : public Statement
{
  public:
    ReturnStatement(Token toke) : Statement(toke){};
    std::string String() const override;

  public:
    Expression return_value_;
};

class ExpressionStatement : public Statement
{
  public:
    ExpressionStatement(Token toke) : Statement(toke){};
    std::string String() const override;

  public:
    Expression expression_;
};

// ROOT //////////////////////

class Program : public Node
{
  public:
    Program() { std::cout << " Constructing Program\n"; }
    std::string TokenLiteral() const override;
    std::string String() const override;

  public:
    std::vector<std::shared_ptr<Statement>> statements_;
};

} // namespace ast

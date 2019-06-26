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
    Expression(Token token) : Node{token} {}
};

class Identifier : public Expression
{
  public:
    Identifier() {}
    Identifier(Token token, std::string val) : Expression{token}, value_{val} {}
    std::string String() const override;
    std::string value_;
};

class IntegerLiteral : public Expression
{
  public:
    IntegerLiteral() {}
    IntegerLiteral(Token token) : Expression{token} {}
    IntegerLiteral(Token token, int64_t val) : Expression{token}, value_{val} {}
    std::string String() const override;

  public:
    int64_t value_;
};

class BooleanExpression : public Expression
{
  public:
    BooleanExpression() {}
    BooleanExpression(Token token) : Expression{token} {}
    BooleanExpression(Token token, bool val) : Expression{token}, value_{val}
    {
        std::cout << "New BOOL! " << val << std::endl;
    }
    std::string String() const override;

  public:
    bool value_;
};

class PrefixExpression : public Expression
{
  public:
    PrefixExpression() {}
    PrefixExpression(Token token) : Expression{token} {}
    PrefixExpression(Token token, std::string op)
        : Expression{token}, operator_{op}
    {
    }
    std::string String() const override;

  public:
    std::string operator_;
    std::shared_ptr<Expression> right_;
};

class InfixExpression : public Expression
{
  public:
    InfixExpression() {}
    InfixExpression(Token token) : Expression{token} {}
    InfixExpression(Token token, std::string op,
                    std::shared_ptr<Expression> left)
        : Expression{token}, operator_{op}, left_{left}
    {
    }
    std::string String() const override;

  public:
    std::string operator_;
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
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
    std::shared_ptr<Identifier> name_{nullptr};
    std::shared_ptr<Expression> value_{nullptr};
};

class ReturnStatement : public Statement
{
  public:
    ReturnStatement(Token toke) : Statement(toke){};
    std::string String() const override;

  public:
    std::shared_ptr<Expression> return_value_;
};

class ExpressionStatement : public Statement
{
  public:
    ExpressionStatement(Token toke) : Statement(toke){};
    std::string String() const override;

  public:
    std::shared_ptr<Expression> expression_;
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

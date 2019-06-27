#include <sstream>

#include "ast.hpp"

namespace ast
{

std::string Program::TokenLiteral() const
{
    if (!statements_.empty())
        return statements_[0]->TokenLiteral();
    else
        return "";
}

std::string Program::String() const
{
    std::stringstream ss;

    for (auto s : statements_)
        ss << s->String();

    return ss.str();
}

std::string LetStatement::String() const
{
    std::stringstream ss;
    if (name_)
        ss << TokenLiteral() << " " << name_->String() << " = ";
    if (value_)
        ss << value_->String();
    ss << ";";

    return ss.str();
}

std::string ReturnStatement::String() const
{
    std::stringstream ss;
    ss << TokenLiteral();
    if (return_value_)
        ss << " " << return_value_->String();
    ss << ";";

    return ss.str();
}

std::string ExpressionStatement::String() const
{
    std::stringstream ss;
    if (expression_)
        ss << expression_->String();

    return ss.str();
}

std::string PrefixExpression::String() const
{
    std::stringstream ss;
    ss << "(";
    if (!operator_.empty())
        ss << operator_;
    if (right_)
        ss << right_->String();
    ss << ")";

    return ss.str();
}

std::string BlockStatement::String() const
{
    std::stringstream ss;
    for (auto &s : statements_)
        ss << s->String();

    return ss.str();
}

std::string IfExpression::String() const
{
    std::stringstream ss;
    ss << "if ";
    if (!condition_)
        ss << condition_->String();
    ss << " ";
    if (consequence_)
        ss << consequence_->String();
    if (alternative_)
    {
        ss << " else ";
        ss << alternative_->String();
    }

    return ss.str();
}

std::string InfixExpression::String() const
{
    std::stringstream ss;
    ss << "(";
    if (left_)
        ss << left_->String();
    if (!operator_.empty())
        ss << operator_;
    if (right_)
        ss << right_->String();
    ss << ")";

    return ss.str();
}

std::string Identifier::String() const { return value_; }
std::string IntegerLiteral::String() const { return token_.literal_; }
std::string BooleanExpression::String() const { return token_.literal_; }

} // namespace ast

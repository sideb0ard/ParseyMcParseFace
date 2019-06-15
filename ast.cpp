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
    ss << TokenLiteral() << " " << name_.String() << " = " << value_.String()
       << ";";

    return ss.str();
}

std::string ReturnStatement::String() const
{
    std::stringstream ss;
    ss << TokenLiteral() << " " << return_value_.String() << ";";

    return ss.str();
}

std::string ExpressionStatement::String() const
{
    std::stringstream ss;
    ss << expression_.String();

    return ss.str();
}

} // namespace ast

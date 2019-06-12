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

} // namespace ast

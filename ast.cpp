#include "ast.hpp"

namespace ast
{

std::string ProgramRoot::TokenLiteral()
{
    if (!statements_.empty())
        return statements_[0].TokenLiteral();
    else
        return "";
}

} // namespace ast

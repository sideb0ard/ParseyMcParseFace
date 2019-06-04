#include <string>

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
}

class Statement : public Node
{
  public:
    void StatementNode();
}

class Expression : public Node
{
  public:
    void ExpressionNode();
}

class ProgramRoot : public Node
{
    std::vector<Statement> statements_;

  public:
    std::string TokenLiteral() override;
}

} // namespace ast

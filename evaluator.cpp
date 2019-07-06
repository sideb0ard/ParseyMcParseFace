#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"

namespace evaluator
{

using namespace ast;
using namespace object;

std::shared_ptr<Object> Eval(std::shared_ptr<Node> node)
{
    std::shared_ptr<Program> prog_node =
        std::dynamic_pointer_cast<Program>(node);
    if (prog_node)
        return EvalStatements(prog_node->statements_);

    std::shared_ptr<ExpressionStatement> expr_statement_node =
        std::dynamic_pointer_cast<ExpressionStatement>(node);
    if (expr_statement_node)
        return Eval(expr_statement_node->expression_);

    // Expressions
    std::shared_ptr<IntegerLiteral> il =
        std::dynamic_pointer_cast<IntegerLiteral>(node);
    if (il)
        return std::make_shared<Integer>(il->value_);

    std::shared_ptr<BooleanExpression> be =
        std::dynamic_pointer_cast<BooleanExpression>(node);
    if (be)
        return std::make_shared<Boolean>(be->value_);

    std::shared_ptr<PrefixExpression> pe =
        std::dynamic_pointer_cast<PrefixExpression>(node);
    if (pe)
    {
        auto right = Eval(pe->right_);
        return EvalPrefixExpression(pe->operator_, right);
    }

    return nullptr;
}

std::shared_ptr<Object> EvalPrefixExpression(std::string op,
                                             std::shared_ptr<Object> right)
{
    if (op.compare("!") == 0)
        return EvalBangOperatorExpression(right);

    return nullptr;
}

std::shared_ptr<Object>
EvalBangOperatorExpression(std::shared_ptr<Object> right)
{
    std::shared_ptr<Boolean> b = std::dynamic_pointer_cast<Boolean>(right);
    if (b)
        return std::make_shared<Boolean>(!b->value_);

    std::shared_ptr<Integer> i = std::dynamic_pointer_cast<Integer>(right);
    if (i)
        return std::make_shared<Boolean>(!i->value_);

    std::shared_ptr<Null> n = std::dynamic_pointer_cast<Null>(right);
    if (n)
        return std::make_shared<Boolean>(true);

    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object>
EvalStatements(std::vector<std::shared_ptr<Statement>> &stmts)
{
    std::shared_ptr<Object> result;
    for (auto &s : stmts)
        result = Eval(s);

    return result;
}

} // namespace evaluator

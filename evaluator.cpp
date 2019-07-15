#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"

namespace evaluator
{

using namespace ast;
using namespace object;

const auto TRUE = std::make_shared<object::Boolean>(true);
const auto FALSE = std::make_shared<object::Boolean>(false);

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
        return NativeBoolToBooleanObject(be->value_);

    std::shared_ptr<PrefixExpression> pe =
        std::dynamic_pointer_cast<PrefixExpression>(node);
    if (pe)
    {
        auto right = Eval(pe->right_);
        return EvalPrefixExpression(pe->operator_, right);
    }

    std::shared_ptr<InfixExpression> ie =
        std::dynamic_pointer_cast<InfixExpression>(node);
    if (ie)
    {
        auto left = Eval(ie->left_);
        auto right = Eval(ie->right_);
        return EvalInfixExpression(ie->operator_, left, right);
    }

    return nullptr;
}

std::shared_ptr<Object> EvalPrefixExpression(std::string op,
                                             std::shared_ptr<Object> right)
{
    if (op.compare("!") == 0)
        return EvalBangOperatorExpression(right);
    else if (op.compare("-") == 0)
        return EvalMinusPrefixOperatorExpression(right);

    return nullptr;
}

std::shared_ptr<Object> EvalInfixExpression(std::string op,
                                            std::shared_ptr<Object> left,
                                            std::shared_ptr<Object> right)
{
    std::shared_ptr<Integer> left_val =
        std::dynamic_pointer_cast<Integer>(left);

    std::shared_ptr<Integer> right_val =
        std::dynamic_pointer_cast<Integer>(right);

    if (left_val && right_val)
    {
        return EvalIntegerInfixExpression(op, left_val, right_val);
    }
    else if (op.compare("==") == 0)
        return NativeBoolToBooleanObject(left == right);
    else if (op.compare("!=") == 0)
        return NativeBoolToBooleanObject(left != right);

    return nullptr;
}

std::shared_ptr<Object>
EvalIntegerInfixExpression(std::string op, std::shared_ptr<Integer> left,
                           std::shared_ptr<Integer> right)
{

    if (op.compare("+") == 0)
        return std::make_shared<Integer>(left->value_ + right->value_);
    else if (op.compare("-") == 0)
        return std::make_shared<Integer>(left->value_ - right->value_);
    else if (op.compare("*") == 0)
        return std::make_shared<Integer>(left->value_ * right->value_);
    else if (op.compare("/") == 0)
        return std::make_shared<Integer>(left->value_ / right->value_);
    else if (op.compare("<") == 0)
        return NativeBoolToBooleanObject(left->value_ < right->value_);
    else if (op.compare(">") == 0)
        return NativeBoolToBooleanObject(left->value_ > right->value_);
    else if (op.compare("==") == 0)
        return NativeBoolToBooleanObject(left->value_ == right->value_);
    else if (op.compare("!=") == 0)
        return NativeBoolToBooleanObject(left->value_ != right->value_);

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
EvalMinusPrefixOperatorExpression(std::shared_ptr<Object> right)
{
    std::shared_ptr<Integer> i = std::dynamic_pointer_cast<Integer>(right);
    if (!i)
        return nullptr;

    return std::make_shared<Integer>(-i->value_);
}

std::shared_ptr<Object>
EvalStatements(std::vector<std::shared_ptr<Statement>> &stmts)
{
    std::shared_ptr<Object> result;
    for (auto &s : stmts)
        result = Eval(s);

    return result;
}

std::shared_ptr<object::Boolean> NativeBoolToBooleanObject(bool input)
{
    if (input)
        return TRUE;
    return FALSE;
}

} // namespace evaluator

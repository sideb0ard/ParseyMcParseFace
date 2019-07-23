#include <iostream>

#include "ast.hpp"
#include "evaluator.hpp"
#include "object.hpp"

namespace
{
bool IsTruthy(std::shared_ptr<object::Object> obj)
{
    if (obj->Type() == object::NULL_OBJ)
        return false;

    std::shared_ptr<object::Boolean> b =
        std::dynamic_pointer_cast<object::Boolean>(obj);
    if (b && b->value_ == true)
        return true;
    else if (b && b->value_ == false)
        return false;

    return true;
}
} // namespace

namespace evaluator
{

using namespace ast;
using namespace object;

static const auto TRUE = std::make_shared<object::Boolean>(true);
static const auto FALSE = std::make_shared<object::Boolean>(false);
static const auto NULLL = std::make_shared<object::Null>();

std::shared_ptr<Object> Eval(std::shared_ptr<Node> node)
{
    std::shared_ptr<Program> prog_node =
        std::dynamic_pointer_cast<Program>(node);
    if (prog_node)
    {
        return EvalProgram(prog_node->statements_);
    }

    std::shared_ptr<BlockStatement> block_statement_node =
        std::dynamic_pointer_cast<BlockStatement>(node);
    if (block_statement_node)
    {
        return EvalBlockStatement(block_statement_node);
    }

    std::shared_ptr<ExpressionStatement> expr_statement_node =
        std::dynamic_pointer_cast<ExpressionStatement>(node);
    if (expr_statement_node)
    {
        return Eval(expr_statement_node->expression_);
    }

    std::shared_ptr<ReturnStatement> return_statement_node =
        std::dynamic_pointer_cast<ReturnStatement>(node);
    if (return_statement_node)
    {
        auto val = Eval(return_statement_node->return_value_);
        return std::make_shared<object::ReturnValue>(val);
    }

    // Expressions
    std::shared_ptr<IntegerLiteral> il =
        std::dynamic_pointer_cast<IntegerLiteral>(node);
    if (il)
    {
        return std::make_shared<Integer>(il->value_);
    }

    std::shared_ptr<BooleanExpression> be =
        std::dynamic_pointer_cast<BooleanExpression>(node);
    if (be)
    {
        return NativeBoolToBooleanObject(be->value_);
    }

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

    std::shared_ptr<IfExpression> if_expr =
        std::dynamic_pointer_cast<IfExpression>(node);
    if (if_expr)
    {
        std::cout << "AN IF EXPRESSION!\n";
        return EvalIfExpression(if_expr);
    }

    return NULLL;
}

std::shared_ptr<Object> EvalPrefixExpression(std::string op,
                                             std::shared_ptr<Object> right)
{
    if (op.compare("!") == 0)
        return EvalBangOperatorExpression(right);
    else if (op.compare("-") == 0)
        return EvalMinusPrefixOperatorExpression(right);

    return NULLL;
}

std::shared_ptr<object::Object>
EvalIfExpression(std::shared_ptr<IfExpression> if_expr)
{
    auto condition = Eval(if_expr->condition_);
    if (IsTruthy(condition))
    {
        std::cout << "TRUTHY TRUE!\n";
        return Eval(if_expr->consequence_);
    }
    else if (if_expr->alternative_)
    {
        std::cout << "TRUTHY FALSE!\n";
        return Eval(if_expr->alternative_);
    }

    return evaluator::NULLL;
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

    return NULLL;
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

    return NULLL;
}

std::shared_ptr<Object>
EvalBangOperatorExpression(std::shared_ptr<Object> right)
{
    if (right == TRUE)
        return FALSE;
    else if (right == FALSE)
        return TRUE;
    else if (right == NULLL)
        return TRUE;
    else
        return FALSE;
}

std::shared_ptr<Object>
EvalMinusPrefixOperatorExpression(std::shared_ptr<Object> right)
{
    std::shared_ptr<Integer> i = std::dynamic_pointer_cast<Integer>(right);
    if (!i)
        return NULLL;

    return std::make_shared<Integer>(-i->value_);
}

std::shared_ptr<Object>
EvalProgram(std::vector<std::shared_ptr<Statement>> &stmts)
{
    std::shared_ptr<Object> result;
    for (auto &s : stmts)
    {
        result = Eval(s);

        std::shared_ptr<ReturnValue> r =
            std::dynamic_pointer_cast<ReturnValue>(result);
        if (r)
            return r->value_;
    }

    return result;
}

std::shared_ptr<Object>
EvalBlockStatement(std::shared_ptr<BlockStatement> block)
{
    std::shared_ptr<Object> result;
    for (auto &s : block->statements_)
    {
        result = Eval(s);
        if (result->Type() != object::NULL_OBJ &&
            result->Type() == object::RETURN_VALUE_OBJ)
            return result;
    }
    return result;
}

std::shared_ptr<object::Boolean> NativeBoolToBooleanObject(bool input)
{
    std::cout << "NATIVE! " << (input ? "true" : "false") << std::endl;
    if (input)
        return TRUE;
    return FALSE;
}

} // namespace evaluator

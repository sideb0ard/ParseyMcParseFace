#include <iostream>
#include <sstream>

#include "ast.hpp"
#include "evaluator.hpp"
#include "object.hpp"

namespace
{
bool IsTruthy(std::shared_ptr<object::Object> obj)
{
    if (obj == evaluator::NULLL)
        return false;
    else if (obj == evaluator::TRUE)
        return true;
    else if (obj == evaluator::FALSE)
        return false;

    return true;
}

bool IsError(std::shared_ptr<object::Object> obj)
{
    if (obj)
    {
        return obj->Type() == object::ERROR_OBJ;
    }
    return false;
}

} // namespace

namespace evaluator
{

using namespace ast;
using namespace object;

std::shared_ptr<Object> Eval(std::shared_ptr<Node> node,
                             std::shared_ptr<Environment> env)
{
    std::shared_ptr<Program> prog_node =
        std::dynamic_pointer_cast<Program>(node);
    if (prog_node)
    {
        return EvalProgram(prog_node->statements_, env);
    }

    std::shared_ptr<BlockStatement> block_statement_node =
        std::dynamic_pointer_cast<BlockStatement>(node);
    if (block_statement_node)
    {
        return EvalBlockStatement(block_statement_node, env);
    }

    std::shared_ptr<ExpressionStatement> expr_statement_node =
        std::dynamic_pointer_cast<ExpressionStatement>(node);
    if (expr_statement_node)
    {
        return Eval(expr_statement_node->expression_, env);
    }

    std::shared_ptr<ReturnStatement> return_statement_node =
        std::dynamic_pointer_cast<ReturnStatement>(node);
    if (return_statement_node)
    {
        auto val = Eval(return_statement_node->return_value_, env);
        if (IsError(val))
            return val;
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
        auto right = Eval(pe->right_, env);
        if (IsError(right))
            return right;
        return EvalPrefixExpression(pe->operator_, right);
    }

    std::shared_ptr<InfixExpression> ie =
        std::dynamic_pointer_cast<InfixExpression>(node);
    if (ie)
    {
        auto left = Eval(ie->left_, env);
        if (IsError(left))
            return left;

        auto right = Eval(ie->right_, env);
        if (IsError(right))
            return right;

        return EvalInfixExpression(ie->operator_, left, right);
    }

    std::shared_ptr<IfExpression> if_expr =
        std::dynamic_pointer_cast<IfExpression>(node);
    if (if_expr)
    {
        return EvalIfExpression(if_expr, env);
    }

    std::shared_ptr<LetStatement> let_expr =
        std::dynamic_pointer_cast<LetStatement>(node);
    if (let_expr)
    {
        auto val = Eval(let_expr->value_, env);
        if (IsError(val))
        {
            return val;
        }
        env->Set(let_expr->name_->value_, val);
    }

    std::shared_ptr<Identifier> ident =
        std::dynamic_pointer_cast<Identifier>(node);
    if (ident)
    {
        return EvalIdentifier(ident, env);
    }

    std::shared_ptr<FunctionLiteral> fn =
        std::dynamic_pointer_cast<FunctionLiteral>(node);
    if (fn)
    {
        auto params = fn->parameters_;
        auto body = fn->body_;
        return std::make_shared<object::Function>(params, env, body);
    }

    std::shared_ptr<CallExpression> call_expr =
        std::dynamic_pointer_cast<CallExpression>(node);
    if (call_expr)
    {
        auto fun = Eval(call_expr->function_, env);
        if (IsError(fun))
            return fun;

        std::vector<std::shared_ptr<Object>> args =
            EvalExpressions(call_expr->arguments_, env);
        if (args.size() == 1 && IsError(args[0]))
            return args[0];

        auto func_obj = std::dynamic_pointer_cast<object::Function>(fun);
        if (func_obj)
            return ApplyFunction(func_obj, args);
        else
            return NewError("Not a function object, mate:%s!", fun->Type());
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
    else
        return NewError("unknown operator: %s %s ", op, right->Type());
}

std::shared_ptr<object::Object>
EvalIfExpression(std::shared_ptr<IfExpression> if_expr,
                 std::shared_ptr<Environment> env)
{
    auto condition = Eval(if_expr->condition_, env);
    if (IsError(condition))
        return condition;

    if (IsTruthy(condition))
    {
        return Eval(if_expr->consequence_, env);
    }
    else if (if_expr->alternative_)
    {
        return Eval(if_expr->alternative_, env);
    }

    return evaluator::NULLL;
}
std::shared_ptr<Object> EvalInfixExpression(std::string op,
                                            std::shared_ptr<Object> left,
                                            std::shared_ptr<Object> right)
{
    if (left->Type() == object::INTEGER_OBJ &&
        right->Type() == object::INTEGER_OBJ)
    {
        auto leftie = std::dynamic_pointer_cast<object::Integer>(left);
        auto rightie = std::dynamic_pointer_cast<object::Integer>(right);
        return EvalIntegerInfixExpression(op, leftie, rightie);
    }
    else if (op.compare("==") == 0)
        return NativeBoolToBooleanObject(left == right);
    else if (op.compare("!=") == 0)
        return NativeBoolToBooleanObject(left != right);
    else if (left->Type() != right->Type())
    {
        std::cerr << "LEFT AND  RIGHT AIN'T CORRECT!\n";
        return NewError("type mismatch: %s %s %s", left->Type(), op,
                        right->Type());
    }

    return NewError("unknown operator: %s %s %s", left->Type(), op,
                    right->Type());
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

    return NewError("unknown operator: %s %s %s", left->Type(), op,
                    right->Type());
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
    {
        return NewError("unknown operator: -%s", right->Type());
    }

    return std::make_shared<Integer>(-i->value_);
}

std::shared_ptr<Object>
EvalProgram(std::vector<std::shared_ptr<Statement>> &stmts,
            std::shared_ptr<Environment> env)
{
    std::shared_ptr<Object> result;
    for (auto &s : stmts)
    {
        result = Eval(s, env);

        std::shared_ptr<ReturnValue> r =
            std::dynamic_pointer_cast<ReturnValue>(result);
        if (r)
            return r->value_;

        std::shared_ptr<object::Error> e =
            std::dynamic_pointer_cast<Error>(result);
        if (e)
            return e;
    }

    return result;
}

std::shared_ptr<Object>
EvalBlockStatement(std::shared_ptr<BlockStatement> block,
                   std::shared_ptr<Environment> env)
{
    std::shared_ptr<Object> result;
    for (auto &s : block->statements_)
    {
        result = Eval(s, env);
        if (result != evaluator::NULLL)
        {
            if (result->Type() == object::RETURN_VALUE_OBJ ||
                result->Type() == object::ERROR_OBJ)
                return result;
        }
    }
    return result;
}

std::shared_ptr<object::Boolean> NativeBoolToBooleanObject(bool input)
{
    if (input)
        return TRUE;
    return FALSE;
}

std::shared_ptr<object::Object>
EvalIdentifier(std::shared_ptr<ast::Identifier> ident,
               std::shared_ptr<object::Environment> env)
{
    auto val = env->Get(ident->value_);
    if (!val)
        return NewError("identifier not found: %s", ident->value_);
    return val;
}

std::vector<std::shared_ptr<object::Object>>
EvalExpressions(std::vector<std::shared_ptr<ast::Expression>> exps,
                std::shared_ptr<object::Environment> env)
{
    std::vector<std::shared_ptr<object::Object>> result;

    for (auto const &e : exps)
    {
        auto evaluated = Eval(e, env);
        if (IsError(evaluated))
            return std::vector<std::shared_ptr<object::Object>>{evaluated};

        result.push_back(evaluated);
    }

    return result;
}

std::shared_ptr<object::Object>
ApplyFunction(std::shared_ptr<object::Function> fun,
              std::vector<std::shared_ptr<object::Object>> args)
{
    auto extended_env = ExtendFunctionEnv(fun, args);
    auto evaluated = Eval(fun->body_, extended_env);
    return UnwrapReturnValue(evaluated);
}

std::shared_ptr<object::Environment>
ExtendFunctionEnv(std::shared_ptr<object::Function> fun,
                  std::vector<std::shared_ptr<object::Object>> &args)
{
    std::shared_ptr<object::Environment> new_env =
        std::make_shared<object::Environment>(fun->env_);
    if (fun->parameters_.size() != args.size())
    {
        std::cerr
            << "Function Eval - args and params not same size, ya numpty!\n";
        return new_env;
    }

    int args_len = args.size();
    for (int i = 0; i < args_len; i++)
    {
        auto param = fun->parameters_[i];
        new_env->Set(param->value_, args[i]);
    }
    return new_env;
}

std::shared_ptr<object::Object>
UnwrapReturnValue(std::shared_ptr<object::Object> obj)
{
    std::shared_ptr<object::ReturnValue> ret =
        std::dynamic_pointer_cast<object::ReturnValue>(obj);
    if (ret)
        return ret->value_;
    return obj;
}

//////////// Error shizzle below

void SSprintF(std::ostringstream &msg, const char *s)
{
    while (*s)
    {
        if (*s == '%')
        {
            if (*(s + 1) == '%')
                ++s;
            else
                msg << "ooft, really fucked up mate. disappointing.";
        }
        msg << *s++;
    }
}

template <typename T, typename... Args>
void SSprintF(std::ostringstream &msg, const char *format, T value,
              Args... args)
{
    while (*format)
    {
        if (*format == '%')
        {
            if (*(format + 1) != '%')
            {
                msg << value;

                format += 2; // only work on 2 char format strings
                SSprintF(msg, format, args...);
                return;
            }
            ++format;
        }
        msg << *format++;
    }
}

template <typename... Args>
std::shared_ptr<object::Error> NewError(std::string format, Args... args)
{

    std::ostringstream error_msg;

    SSprintF(error_msg, format.c_str(), std::forward<Args>(args)...);

    return std::make_shared<object::Error>(error_msg.str());
}

} // namespace evaluator

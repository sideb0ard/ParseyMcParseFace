#pragma once

#include <memory>

#include "ast.hpp"
#include "object.hpp"

namespace evaluator
{

inline auto const TRUE = std::make_shared<object::Boolean>(true);
inline auto const FALSE = std::make_shared<object::Boolean>(false);
inline auto const NULLL = std::make_shared<object::Null>();

std::shared_ptr<object::Object> Eval(std::shared_ptr<ast::Node> node,
                                     std::shared_ptr<object::Environment> env);

std::shared_ptr<object::Object>
EvalProgram(std::vector<std::shared_ptr<ast::Statement>> &stmts,
            std::shared_ptr<object::Environment> env);

std::shared_ptr<object::Object>
EvalBlockStatement(std::shared_ptr<ast::BlockStatement> block,
                   std::shared_ptr<object::Environment> env);

std::shared_ptr<object::Object>
EvalPrefixExpression(std::string op, std::shared_ptr<object::Object> obj);

std::shared_ptr<object::Object>
EvalInfixExpression(std::string op, std::shared_ptr<object::Object> left,
                    std::shared_ptr<object::Object> right);

std::shared_ptr<object::Object>
EvalIntegerInfixExpression(std::string op,
                           std::shared_ptr<object::Integer> left,
                           std::shared_ptr<object::Integer> right);

std::shared_ptr<object::Object>
EvalBangOperatorExpression(std::shared_ptr<object::Object> right);

std::shared_ptr<object::Object>
EvalMinusPrefixOperatorExpression(std::shared_ptr<object::Object> right);

std::shared_ptr<object::Boolean> NativeBoolToBooleanObject(bool input);

std::shared_ptr<object::Object>
EvalIfExpression(std::shared_ptr<ast::IfExpression> if_expr,
                 std::shared_ptr<object::Environment> env);

std::shared_ptr<object::Object>
EvalIdentifier(std::shared_ptr<ast::Identifier> ident,
               std::shared_ptr<object::Environment> env);

std::vector<std::shared_ptr<object::Object>>
EvalExpressions(std::vector<std::shared_ptr<ast::Expression>> exps,
                std::shared_ptr<object::Environment> env);

std::shared_ptr<object::Object>
ApplyFunction(std::shared_ptr<object::Function> fun,
              std::vector<std::shared_ptr<object::Object>> args);

std::shared_ptr<object::Environment>
ExtendFunctionEnv(std::shared_ptr<object::Function> fun,
                  std::vector<std::shared_ptr<object::Object>> &args);

std::shared_ptr<object::Object>
UnwrapReturnValue(std::shared_ptr<object::Object> obj);

template <typename... Args>
std::shared_ptr<object::Error> NewError(std::string format, Args... args);

} // namespace evaluator

#pragma once

#include <memory>

#include "ast.hpp"
#include "object.hpp"

namespace evaluator
{

std::shared_ptr<object::Object> Eval(std::shared_ptr<ast::Node> node);
std::shared_ptr<object::Object>
EvalStatements(std::vector<std::shared_ptr<ast::Statement>> &stmts);
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
} // namespace evaluator

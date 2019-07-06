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
EvalBangOperatorExpression(std::shared_ptr<object::Object> right);
} // namespace evaluator

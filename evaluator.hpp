#pragma once

#include <memory>

#include "ast.hpp"
#include "object.hpp"

namespace evaluator
{

std::shared_ptr<object::Object> Eval(std::shared_ptr<ast::Node> node);
std::shared_ptr<object::Object>
EvalStatements(std::vector<std::shared_ptr<ast::Statement>> &stmts);
} // namespace evaluator

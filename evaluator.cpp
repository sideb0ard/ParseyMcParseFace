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

    return nullptr;
}

std::shared_ptr<object::Object>
EvalStatements(std::vector<std::shared_ptr<Statement>> &stmts)
{
    std::shared_ptr<Object> result;
    for (auto &s : stmts)
    {
        std::cout << "Evaluating: " << s->String() << std::endl;
        result = Eval(s);
    }

    return result;
}

} // namespace evaluator

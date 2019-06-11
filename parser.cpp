#include <iostream>
#include <optional>

#include "parser.hpp"

namespace parser
{

std::unique_ptr<Program> Parser::ParseProgram()
{
    std::unique_ptr<Program> program = std::make_unique<Program>();

    while (cur_token_.type != EOFF)
    {
        std::shared_ptr<Statement> stmt = ParseStatement();
        if (stmt)
        {
            std::cout << " Gots a " << typeid(stmt).name() << " "
                      << stmt->TokenLiteral() << std::endl;
            program->statements_.push_back(std::move(stmt));
        }
        NextToken();
    }
    return program;
}

std::shared_ptr<Statement> Parser::ParseStatement()
{
    if (cur_token_.type.compare(LET) == 0)
    {
        std::cout << "Got a LET statement\n";
        return ParseLetStatement();
    }
    else
        return nullptr;
}

std::shared_ptr<LetStatement> Parser::ParseLetStatement()
{

    std::cout << "Parsing LET Statement\n";

    if (!ExpectPeek(IDENT))
    {
        std::cout << "NO IDENT! - returning nullopt \n";
        return nullptr;
    }

    auto name = Identifier(cur_token_, cur_token_.literal);

    if (!ExpectPeek(ASSIGN))
    {
        std::cout << "NO ASSIGN! - returning nullopt \n";
        return nullptr;
    }

    while (!CurTokenIs(SEMICOLON))
        NextToken();

    std::shared_ptr<LetStatement> stmt =
        std::make_shared<LetStatement>(cur_token_);

    stmt->name_ = name;

    std::cout << "Returning a " << typeid(stmt).name() << std::endl;
    std::cout << stmt->TokenLiteral() << std::endl;
    return stmt;
}

} // namespace parser

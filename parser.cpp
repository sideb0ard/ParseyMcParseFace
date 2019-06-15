#include <iostream>
#include <optional>

#include "parser.hpp"

namespace parser
{

std::unique_ptr<Program> Parser::ParseProgram()
{
    std::cout << "ParseProgram begins..\n";

    std::unique_ptr<Program> program = std::make_unique<Program>();

    while (cur_token_.type_ != EOFF)
    {
        std::shared_ptr<Statement> stmt = ParseStatement();
        if (stmt)
        {
            std::cout << " Gots a " << typeid(stmt).name() << " "
                      << stmt->TokenLiteral() << std::endl;
            // program->statements_.push_back(std::move(stmt));
            program->statements_.push_back(stmt);
        }
        NextToken();
    }
    return program;
}

std::shared_ptr<Statement> Parser::ParseStatement()
{
    if (cur_token_.type_.compare(LET) == 0)
    {
        std::cout << "Got a LET statement\n";
        return ParseLetStatement();
    }
    else if (cur_token_.type_.compare(RETURN) == 0)
    {
        std::cout << "Got a RETURN statement\n";
        return ParseReturnStatement();
    }
    else
        return nullptr;
}

std::shared_ptr<LetStatement> Parser::ParseLetStatement()
{

    std::cout << "Parsing LET Statement\n";

    std::shared_ptr<LetStatement> stmt =
        std::make_shared<LetStatement>(cur_token_);

    if (!ExpectPeek(IDENT))
    {
        std::cout << "NO IDENT! - returning nullopt \n";
        return nullptr;
    }

    stmt->name_ = Identifier(cur_token_, cur_token_.literal_);

    if (!ExpectPeek(ASSIGN))
    {
        std::cout << "NO ASSIGN! - returning nullopt \n";
        return nullptr;
    }

    while (!CurTokenIs(SEMICOLON))
        NextToken();

    std::cout << "Returning a " << typeid(stmt).name() << std::endl;
    std::cout << stmt->TokenLiteral() << std::endl;
    return stmt;
}

std::shared_ptr<ReturnStatement> Parser::ParseReturnStatement()
{

    std::cout << "Parsing RETURN Statement\n";

    std::shared_ptr<ReturnStatement> stmt =
        std::make_shared<ReturnStatement>(cur_token_);

    NextToken();

    while (!CurTokenIs(SEMICOLON))
        NextToken();

    std::cout << "Returning a " << typeid(stmt).name() << std::endl;
    std::cout << stmt->TokenLiteral() << std::endl;
    return stmt;
}

bool Parser::CheckErrors()
{
    if (errors_.empty())
        return false;
    std::cout << "Parser had " << errors_.size() << " errors\n";
    for (auto e : errors_)
        std::cout << e << std::endl;
    return true;
}

} // namespace parser

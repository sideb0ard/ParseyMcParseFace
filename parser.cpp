#include <iostream>
#include <optional>
#include <string>

#include "parser.hpp"

namespace parser
{

Parser::Parser(std::unique_ptr<Lexer> lexer) : lexer_{std::move(lexer)}
{
    NextToken();
    NextToken();
}

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
        return ParseExpressionStatement();
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

    stmt->name_ = std::make_unique<Identifier>(cur_token_, cur_token_.literal_);

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

std::shared_ptr<ExpressionStatement> Parser::ParseExpressionStatement()
{
    std::cout << "Parsing EXPRESSION Statement\n";
    std::shared_ptr<ExpressionStatement> stmt =
        std::make_shared<ExpressionStatement>(cur_token_);
    stmt->expression_ = ParseExpression(Precedence::LOWEST);

    if (PeekTokenIs(SEMICOLON))
        NextToken();

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

bool Parser::ExpectPeek(TokenType t)
{
    std::cout << "  ExpectPeek - looking at " << peek_token_.type_
              << " and expect it to be " << t << std::endl;
    if (PeekTokenIs(t))
    {
        NextToken();
        return true;
    }
    else
    {
        PeekError(t);
        return false;
    }
}

void Parser::NextToken()
{
    cur_token_ = peek_token_;
    peek_token_ = lexer_->NextToken();
}

bool Parser::CurTokenIs(TokenType t)
{
    if (cur_token_.type_.compare(t) == 0)
        return true;
    return false;
}

bool Parser::PeekTokenIs(TokenType t)
{
    if (peek_token_.type_.compare(t) == 0)
        return true;
    return false;
}

void Parser::PeekError(TokenType t)
{
    std::stringstream msg;
    msg << "Expected next token to be " << t << ", got " << peek_token_.type_
        << " instead";
    errors_.push_back(msg.str());
}

std::shared_ptr<Expression> Parser::ParseExpression(Precedence p)
{
    if (cur_token_.type_ == IDENT)
        return ParseIdentifier();
    else if (cur_token_.type_ == INT)
        return ParseIntegerLiteral();
    return nullptr;
}

std::shared_ptr<Expression> Parser::ParseIdentifier()
{
    return std::make_shared<Identifier>(cur_token_, cur_token_.literal_);
}

std::shared_ptr<Expression> Parser::ParseIntegerLiteral()
{
    auto literal = std::make_shared<IntegerLiteral>(cur_token_);
    int64_t val = std::stoll(cur_token_.literal_, nullptr, 10);
    literal->value_ = val;
    return literal;
}

} // namespace parser

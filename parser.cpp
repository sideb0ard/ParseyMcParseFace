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

    NextToken();

    stmt->value_ = ParseExpression(Precedence::LOWEST);

    if (PeekTokenIs(SEMICOLON))
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

    stmt->return_value_ = ParseExpression(Precedence::LOWEST);

    if (PeekTokenIs(SEMICOLON))
        NextToken();

    std::cout << "Returning a " << typeid(stmt).name() << std::endl;
    std::cout << stmt->TokenLiteral() << std::endl;
    return stmt;
}

std::shared_ptr<ExpressionStatement> Parser::ParseExpressionStatement()
{
    std::cout << "  Parsing EXPRESSION Statement\n";
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
    std::cout << "\n======================\nParser had " << errors_.size()
              << " errors\n";
    for (auto e : errors_)
        std::cout << e << std::endl;
    std::cout << "=======================\n";
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

bool Parser::CurTokenIs(TokenType t) const
{
    if (cur_token_.type_.compare(t) == 0)
        return true;
    return false;
}

bool Parser::PeekTokenIs(TokenType t) const
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

static bool IsInfixOperator(TokenType type)
{
    if (type == PLUS || type == MINUS || type == SLASH || type == ASTERISK ||
        type == EQ || type == NOT_EQ || type == LT || type == GT ||
        type == LPAREN)
    {
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////

std::shared_ptr<Expression> Parser::ParseExpression(Precedence p)
{
    std::cout << "      ParseExpression - cur token:" << cur_token_.type_
              << " val: " << cur_token_.literal_ << "!\n";
    std::shared_ptr<Expression> left_expr = ParseForPrefixExpression();

    if (!left_expr)
        return nullptr;

    std::cout << "PeekTOken is " << peek_token_.type_
              << " Precedecne is:" << static_cast<int>(p)
              << " and PeepPrecedence is " << static_cast<int>(PeekPrecedence())
              << std::endl;
    while (!PeekTokenIs(SEMICOLON) && p < PeekPrecedence())
    {
        std::cout << "WHILEin...' - cur_token.type:" << cur_token_.type_
                  << " \n";
        if (IsInfixOperator(peek_token_.type_))
        {
            std::cout << "     " << peek_token_.literal_
                      << " ParseExpression is INFIX!\n";
            NextToken();
            if (cur_token_.type_ == LPAREN)
                left_expr = ParseCallExpression(left_expr);
            else
                left_expr = ParseInfixExpression(left_expr);
        }
        else
        {
            return left_expr;
        }
    }
    return left_expr;
}

std::shared_ptr<Expression> Parser::ParseForPrefixExpression()
{
    if (cur_token_.type_ == IDENT)
        return ParseIdentifier();
    else if (cur_token_.type_ == INT)
        return ParseIntegerLiteral();
    else if (cur_token_.type_ == BANG)
        return ParsePrefixExpression();
    else if (cur_token_.type_ == MINUS)
        return ParsePrefixExpression();
    else if (cur_token_.type_ == TRUE)
        return ParseBoolean();
    else if (cur_token_.type_ == FALSE)
        return ParseBoolean();
    else if (cur_token_.type_ == LPAREN)
        return ParseGroupedExpression();
    else if (cur_token_.type_ == IF)
        return ParseIfExpression();
    else if (cur_token_.type_ == FUNCTION)
        return ParseFunctionLiteral();

    std::cout << "No Prefix parser for " << cur_token_.type_ << std::endl;
    return nullptr;
}

std::shared_ptr<Expression> Parser::ParseIdentifier()
{
    std::cout << "          ParseIdentifier!\n";
    return std::make_shared<Identifier>(cur_token_, cur_token_.literal_);
}

std::shared_ptr<Expression> Parser::ParseBoolean()
{
    std::cout << "          ParseIdentifier!\n";
    return std::make_shared<BooleanExpression>(cur_token_, CurTokenIs(TRUE));
}

std::shared_ptr<Expression> Parser::ParseIntegerLiteral()
{
    std::cout << "          ParseIntegerLiteral! " << cur_token_.literal_
              << "\n";
    auto literal = std::make_shared<IntegerLiteral>(cur_token_);
    int64_t val = std::stoll(cur_token_.literal_, nullptr, 10);
    literal->value_ = val;
    return literal;
}

std::shared_ptr<Expression> Parser::ParseIfExpression()
{
    std::cout << "          ParseIfExpression!\n";
    auto expression = std::make_shared<IfExpression>(cur_token_);

    if (!ExpectPeek(LPAREN))
        return nullptr;

    NextToken();
    expression->condition_ = ParseExpression(Precedence::LOWEST);

    if (!ExpectPeek(RPAREN))
        return nullptr;

    if (!ExpectPeek(LBRACE))
        return nullptr;

    expression->consequence_ = ParseBlockStatement();

    if (PeekTokenIs(ELSE))
    {
        std::cout << "          ELSE!\n";
        NextToken();

        if (!ExpectPeek(LBRACE))
            return nullptr;

        expression->alternative_ = ParseBlockStatement();
    }

    return expression;
}

std::shared_ptr<Expression> Parser::ParseFunctionLiteral()
{
    std::cout << "          ParsePrefixExpression!\n";
    auto lit = std::make_shared<FunctionLiteral>(cur_token_);

    if (!ExpectPeek(LPAREN))
        return nullptr;

    lit->parameters_ = ParseFunctionParameters();

    if (!ExpectPeek(LBRACE))
        return nullptr;

    lit->body_ = ParseBlockStatement();

    return lit;
}

std::shared_ptr<Expression> Parser::ParsePrefixExpression()
{
    std::cout << "          ParsePrefixExpression!\n";
    auto expression =
        std::make_shared<PrefixExpression>(cur_token_, cur_token_.literal_);

    NextToken();

    expression->right_ = ParseExpression(Precedence::PREFIX);

    return expression;
}

std::shared_ptr<Expression>
Parser::ParseInfixExpression(std::shared_ptr<Expression> left)
{
    std::cout << "          ParseInfixExpression!\n";
    auto expression = std::make_shared<InfixExpression>(
        cur_token_, cur_token_.literal_, left);

    auto precedence = CurPrecedence();
    NextToken();
    expression->right_ = ParseExpression(precedence);

    return expression;
}

std::shared_ptr<Expression> Parser::ParseGroupedExpression()
{
    NextToken();
    std::shared_ptr<Expression> expr = ParseExpression(Precedence::LOWEST);
    if (!ExpectPeek(RPAREN))
        return nullptr;
    return expr;
}

Precedence Parser::PeekPrecedence() const
{
    auto it = precedences.find(peek_token_.type_);
    if (it != precedences.end())
    {
        std::cout << "found precedence! for " << peek_token_.type_ << " ("
                  << static_cast<int>(it->second) << ")\n";
        return it->second;
    }
    std::cout << "DIDNAE find precedence! for " << peek_token_.type_
              << std::endl;
    return Precedence::LOWEST;
}

Precedence Parser::CurPrecedence() const
{
    auto it = precedences.find(cur_token_.type_);
    if (it != precedences.end())
    {
        return it->second;
    }
    return Precedence::LOWEST;
}

std::shared_ptr<BlockStatement> Parser::ParseBlockStatement()
{
    auto block_stmt = std::make_shared<BlockStatement>(cur_token_);

    NextToken();
    while (!CurTokenIs(RBRACE) && !CurTokenIs(EOFF))
    {
        auto stmt = ParseStatement();
        if (stmt)
            block_stmt->statements_.push_back(stmt);
        NextToken();
    }
    return block_stmt;
}

std::vector<std::shared_ptr<Identifier>> Parser::ParseFunctionParameters()
{
    std::vector<std::shared_ptr<Identifier>> identifiers;

    if (PeekTokenIs(RPAREN))
    {
        NextToken();
        return identifiers;
    }

    NextToken();

    auto ident = std::make_shared<Identifier>(cur_token_, cur_token_.literal_);
    identifiers.push_back(ident);
    while (PeekTokenIs(COMMA))
    {
        NextToken();
        NextToken();
        auto ident =
            std::make_shared<Identifier>(cur_token_, cur_token_.literal_);
        identifiers.push_back(ident);
    }

    if (!ExpectPeek(RPAREN))
    {
        return std::vector<std::shared_ptr<Identifier>>();
    }

    return identifiers;
}

std::shared_ptr<Expression>
Parser::ParseCallExpression(std::shared_ptr<Expression> funct)
{
    std::cout << "PARSE CALL EXPRESSSSSION\n";
    std::shared_ptr<CallExpression> expr =
        std::make_shared<CallExpression>(cur_token_, funct);
    expr->arguments_ = ParseCallArguments();
    return expr;
}

std::vector<std::shared_ptr<Expression>> Parser::ParseCallArguments()
{
    std::vector<std::shared_ptr<Expression>> arguments;
    if (PeekTokenIs(RPAREN))
    {
        NextToken();
        return arguments;
    }

    NextToken();
    arguments.push_back(ParseExpression(Precedence::LOWEST));

    while (PeekTokenIs(COMMA))
    {
        NextToken();
        NextToken();
        arguments.push_back(ParseExpression(Precedence::LOWEST));
    }

    if (!ExpectPeek(RPAREN))
    {
        return std::vector<std::shared_ptr<Expression>>();
    }

    return arguments;
}
} // namespace parser

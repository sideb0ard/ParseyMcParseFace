#include <iostream>
#include <string>
#include <unordered_map>

#include "lexical_token.hpp"

namespace lexer
{
const std::unordered_map<std::string, TokenType> keywords{
    {"fn", FUNCTION}, {"let", LET},   {"true", TRUE},    {"false", FALSE},
    {"if", IF},       {"else", ELSE}, {"return", RETURN}};

TokenType LookupIdent(std::string ident)
{
    std::unordered_map<std::string, TokenType>::const_iterator got =
        keywords.find(ident);
    if (got != keywords.end())
        return got->second;
    return IDENT;
}

std::ostream &operator<<(std::ostream &out, const Token &tok)
{
    out << "{type:" << tok.type << " literal:" << tok.literal << "}";
    return out;
}

} // namespace lexer

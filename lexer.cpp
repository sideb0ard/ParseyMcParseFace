#include "lexer.hpp"
#include <string>

namespace
{
bool IsValidIdentifier(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}
bool IsDigit(char c) { return '0' <= c && c <= '9'; }
} // namespace

namespace lexer
{

Lexer::Lexer(std::string input) : input_{input} { ReadChar(); }

void Lexer::ReadChar()
{
    if (next_position_ >= static_cast<int>(input_.length()))
        current_char_ = 0;
    else
        current_char_ = input_[next_position_];

    current_position_ = next_position_;
    next_position_ += 1;
}

char Lexer::PeekChar()
{
    if (next_position_ >= static_cast<int>(input_.length()))
        return 0;
    else
        return input_[next_position_];
}

Token Lexer::NextToken()
{

    Token tok;

    SkipWhiteSpace();

    switch (current_char_)
    {
    case ('='):
        if (PeekChar() == '=')
        {
            ReadChar();
            tok.type = EQ;
            tok.literal = "==";
        }
        else
        {
            tok.type = ASSIGN;
            tok.literal = current_char_;
        }
        break;
    case ('+'):
        tok.type = PLUS;
        tok.literal = current_char_;
        break;
    case ('-'):
        tok.type = MINUS;
        tok.literal = current_char_;
        break;
    case ('!'):
        if (PeekChar() == '=')
        {
            ReadChar();
            tok.type = NE;
            tok.literal = "!=";
        }
        else
        {
            tok.type = BANG;
            tok.literal = current_char_;
        }
        break;
    case ('*'):
        tok.type = ASTERISK;
        tok.literal = current_char_;
        break;
    case ('/'):
        tok.type = SLASH;
        tok.literal = current_char_;
        break;
    case ('<'):
        tok.type = LT;
        tok.literal = current_char_;
        break;
    case ('>'):
        tok.type = GT;
        tok.literal = current_char_;
        break;
    case (','):
        tok.type = COMMA;
        tok.literal = current_char_;
        break;
    case (';'):
        tok.type = SEMICOLON;
        tok.literal = current_char_;
        break;
    case ('('):
        tok.type = LPAREN;
        tok.literal = current_char_;
        break;
    case (')'):
        tok.type = RPAREN;
        tok.literal = current_char_;
        break;
    case ('{'):
        tok.type = LBRACE;
        tok.literal = current_char_;
        break;
    case ('}'):
        tok.type = RBRACE;
        tok.literal = current_char_;
        break;
    case (0):
        tok.type = EOFF;
        break;
    default:
        if (IsValidIdentifier(current_char_))
        {
            tok.literal = ReadIdentifier();
            tok.type = LookupIdent(tok.literal);
            return tok;
        }
        else if (IsDigit(current_char_))
        {
            tok.type = INT;
            tok.literal = ReadNumber();
            return tok;
        }
        else
        {
            tok.type = ILLEGAL;
            tok.literal = current_char_;
        }
    }

    ReadChar();
    return tok;
}

std::string Lexer::ReadIdentifier()
{
    int position = current_position_;
    while (IsValidIdentifier(current_char_))
        ReadChar();
    return input_.substr(position, current_position_ - position);
}

std::string Lexer::ReadNumber()
{
    int position = current_position_;
    while (IsDigit(current_char_))
        ReadChar();
    return input_.substr(position, current_position_ - position);
}

void Lexer::SkipWhiteSpace()
{
    while (current_char_ == ' ' || current_char_ == '\t' ||
           current_char_ == '\n' || current_char_ == '\r')
        ReadChar();
}

} // namespace lexer

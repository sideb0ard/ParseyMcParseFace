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
    const int len = input_.length();
    if (next_position_ >= len)
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
            tok.type_ = EQ;
            tok.literal_ = "==";
        }
        else
        {
            tok.type_ = ASSIGN;
            tok.literal_ = current_char_;
        }
        break;
    case ('+'):
        tok.type_ = PLUS;
        tok.literal_ = current_char_;
        break;
    case ('-'):
        tok.type_ = MINUS;
        tok.literal_ = current_char_;
        break;
    case ('!'):
        if (PeekChar() == '=')
        {
            ReadChar();
            tok.type_ = NE;
            tok.literal_ = "!=";
        }
        else
        {
            tok.type_ = BANG;
            tok.literal_ = current_char_;
        }
        break;
    case ('*'):
        tok.type_ = ASTERISK;
        tok.literal_ = current_char_;
        break;
    case ('/'):
        tok.type_ = SLASH;
        tok.literal_ = current_char_;
        break;
    case ('<'):
        tok.type_ = LT;
        tok.literal_ = current_char_;
        break;
    case ('>'):
        tok.type_ = GT;
        tok.literal_ = current_char_;
        break;
    case (','):
        tok.type_ = COMMA;
        tok.literal_ = current_char_;
        break;
    case (';'):
        tok.type_ = SEMICOLON;
        tok.literal_ = current_char_;
        break;
    case ('('):
        tok.type_ = LPAREN;
        tok.literal_ = current_char_;
        break;
    case (')'):
        tok.type_ = RPAREN;
        tok.literal_ = current_char_;
        break;
    case ('{'):
        tok.type_ = LBRACE;
        tok.literal_ = current_char_;
        break;
    case ('}'):
        tok.type_ = RBRACE;
        tok.literal_ = current_char_;
        break;
    case (0):
        tok.type_ = EOFF;
        break;
    default:
        if (IsValidIdentifier(current_char_))
        {
            tok.literal_ = ReadIdentifier();
            tok.type_ = LookupIdent(tok.literal_);
            return tok;
        }
        else if (IsDigit(current_char_))
        {
            tok.type_ = INT;
            tok.literal_ = ReadNumber();
            return tok;
        }
        else
        {
            tok.type_ = ILLEGAL;
            tok.literal_ = current_char_;
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

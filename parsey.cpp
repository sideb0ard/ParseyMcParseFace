#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.hpp"
#include "lexical_token.hpp"

using namespace lexer;

int main()
{
    const std::string prompt = ">> ";

    std::string input;

    std::cout << prompt;
    while (!std::getline(std::cin, input).fail())
    {
        if (input.empty())
            break;

        Lexer lex{input};

        for (Token tok = lex.NextToken(); tok.type != EOFF;
             tok = lex.NextToken())
            std::cout << tok << std::endl;

        std::cout << prompt;
    }
}

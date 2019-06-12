#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.hpp"
#include "token.hpp"

using namespace lexer;
using namespace token;

int main()
{
    const std::string prompt = ">> ";

    std::cout << prompt;
    for (std::string input; std::getline(std::cin, input);)
    {
        if (input.empty())
        {
            std::cout << prompt;
            continue;
        }

        Lexer lex{input};

        for (Token tok = lex.NextToken(); tok.type_ != EOFF;
             tok = lex.NextToken())
            std::cout << tok << std::endl;

        std::cout << prompt;
    }
}

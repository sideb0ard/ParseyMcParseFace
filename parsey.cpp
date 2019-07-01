#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.hpp"
#include "parser.hpp"
#include "token.hpp"

using namespace lexer;
using namespace token;
using namespace parser;

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

        std::unique_ptr<Lexer> lex = std::make_unique<Lexer>(input);
        std::unique_ptr<Parser> parsley =
            std::make_unique<Parser>(std::move(lex));
        std::unique_ptr<Program> program = parsley->ParseProgram();

        std::cout << program->String() << std::endl;

        std::cout << prompt;
    }
}

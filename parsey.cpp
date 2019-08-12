#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "evaluator.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "token.hpp"

int main()
{
    const std::string prompt = ">> ";

    auto env = std::make_shared<object::Environment>();

    std::cout << prompt;
    for (std::string input; std::getline(std::cin, input);)
    {
        if (input.empty())
        {
            std::cout << prompt;
            continue;
        }

        auto lex = std::make_unique<lexer::Lexer>(input);
        auto parsley = std::make_unique<parser::Parser>(std::move(lex));
        auto program = parsley->ParseProgram();

        auto evaluated = evaluator::Eval(program, env);
        if (evaluated)
            std::cout << evaluated->Inspect() << std::endl;

        std::cout << prompt;
    }
}

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

constexpr char prompt[] = ">> ";

int main()
{
    auto env = std::make_shared<object::Environment>();

    std::cout << prompt;
    auto lex = std::make_shared<lexer::Lexer>();

    for (std::string input; std::getline(std::cin, input);)
    {
        if (input.empty())
        {
            std::cout << prompt;
            continue;
        }

        bool done_slurping = lex->ReadInput(input);
        if (!done_slurping)
        {
            std::cout << ">>>> ";
            continue;
        }

        std::unique_ptr<parser::Parser> parsley =
            std::make_unique<parser::Parser>(lex);

        std::shared_ptr<ast::Program> program = parsley->ParseProgram();

        auto evaluated = evaluator::Eval(program, env);
        if (evaluated)
        {
            auto result = evaluated->Inspect();
            if (result.compare("null") != 0)
                std::cout << result << std::endl;
        }

        lex->Reset();

        std::cout << prompt;
    }
}

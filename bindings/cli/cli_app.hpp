#pragma once
#include "bindings/basic_logger/standard_logger.hpp"
#include "cli.hpp"
#include <stdio.h>
#include <cstdlib>
#include <memory>

struct Config
{
    using basic_logger_type = sygaldry::bindings::basic_logger::StandardLogger;
};

template<typename ... Components>
struct CliApp
{
    int main()
    {
        auto components = std::make_shared<std::tuple<Components...>>();
        auto cli = sygaldry::bindings::cli::make_default_cli<Config>(components, "Type Ctrl-D to exit.");
        for (;;)
        {
            char c = getchar();
            if (c == EOF)
                std::exit(EXIT_SUCCESS);
            else
                cli.process(c);
        }
    }
};

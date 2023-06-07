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

template<typename Components>
struct CliApp
{
    int main()
    {
        auto components = Components{};
        auto cli = sygaldry::bindings::cli::Cli<Config, Components>{};
        cli.init();
        for (;;)
        {
            char c = getchar();
            if (c == EOF)
                return EXIT_SUCCESS;
            else
                cli.process(c, components);
        }
    }
};

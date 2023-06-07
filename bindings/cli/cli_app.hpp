#pragma once
#include "bindings/basic_logger/standard_logger.hpp"
#include "cli.hpp"
#include <stdio.h>
#include <cstdlib>
#include <memory>

template<typename Components>
struct CliApp
{
    int main()
    {
        auto components = Components{};
        auto log = sygaldry::bindings::basic_logger::StandardLogger{};
        auto cli = sygaldry::bindings::cli::Cli<decltype(log), Components>{};
        cli.init(log);
        for (;;)
        {
            char c = getchar();
            if (c == EOF)
                return EXIT_SUCCESS;
            else
                cli(c, log, components);
        }
    }
};

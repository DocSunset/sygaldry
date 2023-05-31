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

template<typename ... Devices>
struct CliApp
{
    int main()
    {
        auto devices = std::make_shared<std::tuple<Devices...>>();
        auto cli = sygaldry::bindings::cli::make_default_cli<Config>(devices, "Type Ctrl-D to exit.");
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

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
    using devs_tuple_t = std::tuple<Devices...>;
    using devs_t = std::shared_ptr<devs_tuple_t>;
    devs_t devices = std::make_shared<devs_tuple_t>();
    sygaldry::bindings::cli::Cli<Config, devs_t> cli{devices, "Type Ctrl-D to exit."};

    int main()
    {
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

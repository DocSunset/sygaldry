#pragma once
#include "basic_logger.hpp"
#include <cstdio>
#include <cstdlib>

namespace sygaldry::bindings::basic_logger
{

struct CstdioPutter
{
    void operator()(char c)
    {
        auto retcode = std::putchar(c);
        if (retcode == EOF)
        {
            // I guess we're unexpectedly done now?
            std::exit(EXIT_FAILURE);
        }
    }
};

using CstdioLogger = BasicLogger<CstdioPutter>;

}

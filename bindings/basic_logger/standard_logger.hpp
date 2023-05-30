#pragma once
#include "basic_logger.hpp"
#include <stdio.h>
#include <cstdlib>

namespace sygaldry::bindings::basic_logger
{

struct StandardPutter
{
    void operator()(char c)
    {
        auto retcode = putchar(c);
        if (retcode == EOF)
        {
            // I guess we're unexpectedly done now?
            std::exit(EXIT_FAILURE);
        }
    }
};

using StandardLogger = BasicLogger<StandardPutter>;

}

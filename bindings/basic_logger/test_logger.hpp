#pragma once
#include "basic_logger.hpp"

namespace sygaldry::bindings::basic_logger
{

struct TestPutter
{
    static std::stringstream ss;
    void operator()(char c)
    {
        ss << c;
    }
};

using TestLogger = BasicLogger<TestPutter>;

std::stringstream TestPutter::ss;

}

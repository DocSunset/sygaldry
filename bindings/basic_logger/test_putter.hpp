#pragma once

namespace bindings::basic_logger
{

struct TestPutter
{
    std::stringstream ss;
    void operator()(char c)
    {
        ss << c;
    }
};

}

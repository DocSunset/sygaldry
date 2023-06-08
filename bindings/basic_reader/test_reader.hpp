#pragma once

#include <string>
#include <sstream>

namespace sygaldry { namespace bindings { namespace basic_reader {

struct TestReader
{
    std::stringstream ss;
    bool ready() {return std::stringstream::traits_type::not_eof(ss.peek());}
    char getchar() {return static_cast<char>(ss.get());}
};

} } }

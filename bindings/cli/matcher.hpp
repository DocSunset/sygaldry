#pragma once

#include <string_view>
#include "bindings/spelling.hpp"

namespace sygaldry { namespace bindings {

struct CommandMatcher
{
    template<typename stringish, typename Command>
    bool operator()(stringish arg0, const Command& command)
    {
        return std::string_view(arg0) == std::string_view(lower_kebab_case(command));
    }
};

} }

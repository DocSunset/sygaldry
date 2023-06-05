#pragma once

#include <string_view>
#include "utilities/spelling.hpp"

namespace sygaldry { namespace bindings { namespace cli {

struct CommandMatcher
{
    template<typename stringish, typename Command>
    bool operator()(stringish arg0, const Command& command)
    {
        using spelling::lower_kebab_case;
        return std::string_view(arg0) == std::string_view(lower_kebab_case(command));
    }
};

} } }

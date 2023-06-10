#pragma once

#include <type_traits>
#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"
#include "bindings/spelling.hpp"

namespace sygaldry { namespace bindings { namespace clicommands {

struct List
{
    static _consteval auto name() { return "list"; }
    static _consteval auto usage() { return ""; }
    static _consteval auto description() { return "List the components available to interact with through the CLI"; }

    int main(int argc, char** argv, auto& log, auto& components)
    {
        boost::pfr::for_each_field(components, [&](const auto& component)
        {
            log.println(lower_kebab_case_v<std::decay_t<decltype(component)>>);
        });
        return 0;
    }
};

} } }

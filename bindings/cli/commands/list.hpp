#pragma once

#include <type_traits>
#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"

namespace sygaldry { namespace bindings { namespace cli { namespace commands {

template<typename Logger>
struct List
{
    static _consteval auto name() { return "list"; }
    static _consteval auto usage() { return ""; }
    static _consteval auto description() { return "List the components available to interact with through the CLI"; }

    template<typename Components>
    int main(int argc, char** argv, Logger& log, Components& components)
    {
        boost::pfr::for_each_field(components, [&](const auto& component)
        {
            log.println(spelling::lower_kebab_case_v<std::decay_t<decltype(component)>>);
        });
        return 0;
    }
};

} } } }

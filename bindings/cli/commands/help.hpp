#pragma once
#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"

namespace sygaldry { namespace bindings { namespace cli { namespace commands {
template<typename Logger>
struct Help
{
    static _consteval auto name() { return "help"; }
    static _consteval auto usage() { return ""; }
    static _consteval auto description() { return "Describe the available commands and their usage"; }

    void _print(auto& log, auto&& command)
    {
        if constexpr (requires {command.usage();})
            log.println(command.name(), " ", command.usage());
        else
            log.println(command.name());
        log.println("    ", command.description());
    }

    int main(Logger& log, auto& commands)
    {
        boost::pfr::for_each_field(commands, [&](auto&& command)
        {
            _print(log, command);
        });
        log.println(name());
        log.println("    ", description());
        return 0;
    }
};

} } } }

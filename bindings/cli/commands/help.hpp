#pragma once
#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"

namespace sygaldry { namespace bindings { namespace cli { namespace commands {
template<typename Config>
struct Help
{
    static _consteval auto name() { return "help"; }
    static _consteval auto usage() { return ""; }
    static _consteval auto description() { return "Describe the available commands and their usage"; }

    [[no_unique_address]] typename Config::basic_logger_type log;

    void _print(auto&& command)
    {
        if constexpr (requires {command.usage();})
            log.println(command.name(), " ", command.usage());
        else
            log.println(command.name());
        log.println("    ", command.description());
    }

    int main(auto&& commands)
    {
        boost::pfr::for_each_field(commands, [&](auto&& command)
        {
            _print(command);
        });
        log.println(name());
        log.println("    ", description());
        return 0;
    }
};

} } } }

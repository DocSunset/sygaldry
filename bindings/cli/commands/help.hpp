#pragma once
#include "utilities/consteval/consteval.hpp"

namespace sygaldry
{
namespace bindings::cli::commands
{
template<typename Config>
struct Help
{
    static _consteval auto name() { return "help"; }
    static _consteval auto usage() { return ""; }
    static _consteval auto description() { return "Describe the available commands and their usage"; }

    [[no_unique_address]] typename Config::basic_logger_type log;

    template<typename Cmd>
    void _print(Cmd&& command)
    {
        if constexpr (requires {command.usage();})
            log.println(command.name(), " ", command.usage());
        else
            log.println(command.name());
        log.println("    ", command.description());
    }

    template<typename ... Commands>
    int main(Commands&& ... commands)
    {
        ( _print(commands), ... );
        log.println(name());
        log.println("    ", description());
        return 0;
    }
};

}
}

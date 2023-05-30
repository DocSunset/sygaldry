#pragma once

namespace sygaldry::bindings::cli::commands
{
template<typename Config>
struct Help
{
    static consteval auto name() { return "help"; }
    static consteval auto usage() { return ""; }
    static consteval auto description() { return "Describe the available commands and their usage"; }

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

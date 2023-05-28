#pragma once

namespace bindings::cli::commands
{
template<typename Config, typename ... Commands>
struct Help
{
    static consteval auto name() { return "help"; }
    static consteval auto usage() { return ""; }
    static consteval auto description() { return "Describe the available commands and their usage"; }

    [[no_unique_address]] typename Config::basic_logger_type log;

    template<typename Cmd>
    void _print()
    {
        log.print(Cmd::name());
        if constexpr (requires {Cmd::usage();})
        {
            log.print(" ");
            log.println(Cmd::usage());
        }
        else
        {
            log.println();
        }
        log.print("    ");
        log.println(Cmd::description());
    }

    int main()
    {
        ( _print<Commands>(), ... );
        return 0;
    }
};
}

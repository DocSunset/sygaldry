#pragma once
#include <memory>
#include <string_view>
#include "utilities/consteval.hpp"
#include "bindings/name_dispatch.hpp"
#include "matcher.hpp"

#include "commands/list.hpp"
#include "commands/help.hpp"
#include "commands/describe.hpp"
#include "commands/set.hpp"
#include "commands/trigger.hpp"

namespace sygaldry { namespace bindings::cli
{

template<typename Config, typename Components, template<typename>typename Commands>
struct CustomCli
{
    [[no_unique_address]] typename Config::basic_logger_type log{};
    Commands<Config> commands{};

    void init()
    {
        log.println("CLI enabled. Write `help` for a list of available commands.");
        _prompt();
    }

    // TODO: automatically size the buffers depending on the commands
    static constexpr size_t MAX_ARGS = 5;
    static constexpr size_t BUFFER_SIZE = 128;
    int argc = 0;
    char * argv[MAX_ARGS];
    unsigned char write_pos = 0;
    char buffer[BUFFER_SIZE];

    int _try_to_match_and_execute(Components components)
    {
        return dispatch<CommandMatcher>(argv[0], commands, 127, [&](auto& command)
            {
                if constexpr (std::is_same_v<decltype(command), commands::Help<Config>&>)
                {
                    return command.main(commands);
                }
                else return command.main(argc, argv, components);
            });
    }
    bool _is_whitespace(char c)
    {
        if (c == ' ' || c == '\t' || c == '\n') return true;
        else return false;
    }

    bool _new_arg() const
    {
        return write_pos == 0 || buffer[write_pos-1] == 0;
    }

    bool _overflow() const
    {
        return argc == MAX_ARGS || write_pos == BUFFER_SIZE;
    }

    void _prompt()
    {
        log.print("> ");
    }

    void _reset()
    {
        argc = 0;
        write_pos = 0;
        _prompt();
    }

    void _complain_about_command_failure(int retcode)
    {} // TODO

    void process(char c, Components& components)
    {
        if (_is_whitespace(c))
            buffer[write_pos++] = 0;
        else
        {
            buffer[write_pos] = c;
            if (_new_arg())
                argv[argc++] = &buffer[write_pos];
            write_pos++;
        }

        if (c == '\n')
        {
            auto retcode = _try_to_match_and_execute(components);
            if (retcode != 0) _complain_about_command_failure(retcode);
            _reset();
        }

        if (_overflow())
        {
            log.println("CLI line buffer overflow!");
            _reset();
        }
    }
};

template<typename Config>
struct DefaultCommands
{
    #define default_command(TYPENAME) commands::TYPENAME<Config> _##TYPENAME
    default_command(Help);
    default_command(List);
    default_command(Describe);
    default_command(Set);
    default_command(Trigger);
    #undef default_command
};

template<typename Config, typename Components>
using Cli = CustomCli<Config, Components, DefaultCommands>;

} }

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

namespace sygaldry { namespace bindings { namespace cli {

template<typename Logger, typename Components, template<typename>typename Commands>
struct CustomCli
{
    struct parts_t
    {
        [[no_unique_address]] Commands<Logger> commands;
    } parts;

    void init(Logger& log)
    {
        log.println("CLI enabled. Write `help` for a list of available commands.");
        _prompt(log);
    }

    // TODO: automatically size the buffers depending on the commands
    static constexpr size_t MAX_ARGS = 5;
    static constexpr size_t BUFFER_SIZE = 128;
    int argc = 0;
    char * argv[MAX_ARGS];
    unsigned char write_pos = 0;
    char buffer[BUFFER_SIZE];

    int _try_to_match_and_execute(Logger& log, Components& components)
    {
        return dispatch<CommandMatcher>(argv[0], parts.commands, 127, [&](auto& command)
            {
                if constexpr (std::is_same_v<decltype(command), commands::Help<Logger>&>)
                {
                    return command.main(log, parts.commands);
                }
                else return command.main(argc, argv, log, components);
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

    void _prompt(auto& log)
    {
        log.print("> ");
    }

    void _reset(auto& log)
    {
        argc = 0;
        write_pos = 0;
        _prompt(log);
    }

    void _complain_about_command_failure(auto& log, int retcode)
    {} // TODO

    void operator()(char c, Logger& log, Components& components)
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
            auto retcode = _try_to_match_and_execute(log, components);
            if (retcode != 0) _complain_about_command_failure(log, retcode);
            _reset(log);
        }

        if (_overflow())
        {
            log.println("CLI line buffer overflow!");
            _reset(log);
        }
    }
};

template<typename Logger>
struct DefaultCommands
{
    #define default_command(TYPENAME) commands::TYPENAME<Logger> _##TYPENAME
    default_command(Help);
    default_command(List);
    default_command(Describe);
    default_command(Set);
    default_command(Trigger);
    #undef default_command
};

template<typename Logger, typename Components>
using Cli = CustomCli<Logger, Components, DefaultCommands>;

} } }

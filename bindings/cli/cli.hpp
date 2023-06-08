#pragma once
#include <memory>
#include <string_view>
#include "utilities/consteval.hpp"
#include "components/endpoints.hpp"
#include "bindings/name_dispatch.hpp"
#include "matcher.hpp"

#include "commands/help.hpp"
#include "commands/list.hpp"
#include "commands/describe.hpp"
#include "commands/set.hpp"
#include "commands/trigger.hpp"

namespace sygaldry { namespace bindings { namespace cli {

using namespace sygaldry::endpoints;

template<typename Reader, typename Logger, typename Components, typename Commands>
struct CustomCli : name_<"CLI">
                 , author_<"Travis J. West">
                 , description_<"Generate a simple command line interface for inspecting and sending data to the bound components.">
                 , version_<"0.0.0">
                 , copyright_<"Travis J. West <C) 2023">
{
    [[no_unique_address]] struct parts_t {
    [[no_unique_address]] Logger log;
    [[no_unique_address]] Reader reader;
    [[no_unique_address]] Commands commands;
    } parts;

    void init()
    {
        parts.log.println("CLI enabled. Write `help` for a list of available commands.");
        _prompt();
    }

    // TODO: automatically size the buffers depending on the commands
    static constexpr size_t MAX_ARGS = 5;
    static constexpr size_t BUFFER_SIZE = 128;
    int argc = 0;
    char * argv[MAX_ARGS];
    unsigned char write_pos = 0;
    char buffer[BUFFER_SIZE];

    int _try_to_match_and_execute(Components& components)
    {
        return dispatch<CommandMatcher>(argv[0], parts.commands, 127, [&](auto& command)
            {
                if constexpr (std::is_same_v<decltype(command), commands::Help&>)
                {
                    return command.main(parts.log, parts.commands);
                }
                else return command.main(argc, argv, parts.log, components);
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
        parts.log.print("> ");
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
            parts.log.println("CLI line buffer overflow!");
            _reset();
        }
    }

    void operator()(Components& components)
    {
        while(parts.reader.ready()) process(parts.reader.getchar(), components);
    }
};

struct DefaultCommands
{
    commands::Help help;
    commands::List list;
    commands::Describe describe;
    commands::Set set;
    commands::Trigger trigger;
};

template<typename Reader, typename Logger, typename Components>
using Cli = CustomCli<Reader, Logger, Components, DefaultCommands>;

} } }

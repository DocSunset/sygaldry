#pragma once
#include <memory>
#include <string_view>
#include <concepts>
#include <cstdlib>
#include "utilities/consteval.hpp"
#include "helpers/metadata.hpp"
#include "bindings/name_dispatch.hpp"
#include "bindings/basic_logger/cstdio_logger.hpp"
#include "bindings/basic_reader/cstdio_reader.hpp"
#include "matcher.hpp"

#include "commands/help.hpp"
#include "commands/list.hpp"
#include "commands/describe.hpp"
#include "commands/set.hpp"
#include "commands/trigger.hpp"

namespace sygaldry { namespace bindings {

template<typename Reader, typename Logger, typename Components, typename Commands>
struct CustomCli : name_<"CLI">
                 , author_<"Travis J. West">
                 , description_<"Generate a simple command line interface for inspecting and sending data to the bound components.">
                 , version_<"0.0.0">
                 , copyright_<"Travis J. West <C) 2023">
{
    struct inputs_t {} inputs; // so that component concepts will recognize component
    [[no_unique_address]] Logger log;
    [[no_unique_address]] Reader reader;
    [[no_unique_address]] Commands commands;

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

    int _try_to_match_and_execute(Components& components)
    {
        return dispatch<CommandMatcher>(argv[0], commands, 127, [&](auto& command)
            {
                if constexpr (std::is_same_v<decltype(command), clicommands::Help&>)
                {
                    return command.main(log, commands);
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

    void process(const char c, Components& components)
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

        #ifdef ESP_PLATFORM
        char s[2] = {0,0};
        s[0] = c;
        log.print(s);
        #endif

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

    void operator()(Components& components)
    {
        while(reader.ready()) process(reader.getchar(), components);
    }
};

struct DefaultCommands
{
    clicommands::Help help;
    clicommands::List list;
    clicommands::Describe describe;
    clicommands::Set set;
    clicommands::Trigger trigger;
};

template<typename Reader, typename Logger, typename Components>
using Cli = CustomCli<Reader, Logger, Components, DefaultCommands>;

template<typename Components>
using CstdioCli = Cli<CstdioReader, CstdioLogger, Components>;

} }

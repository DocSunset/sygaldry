#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <memory>
#include <string_view>
#include <concepts>
#include <cstdlib>
#include "sygah-consteval.hpp"
#include "sygah-metadata.hpp"
#include "sygbp-osc_match_pattern.hpp"
#include "sygup-cstdio_logger.hpp"
#include "sygbp-cstdio_reader.hpp"

#include "commands/help.hpp"
#include "commands/list.hpp"
#include "commands/describe.hpp"
#include "commands/set.hpp"

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp sygbp: Portable Bindings
///\{
///\defgroup sygbp-cli sygbp-cli: CLI Binding
///\{

template<typename Reader, typename Logger, typename Components, typename Commands>
struct CustomCli : name_<"CLI">
                 , author_<"Travis J. West">
                 , description_<"Generate a simple command line interface for inspecting and sending data to the bound components.">
                 , version_<"0.0.0">
                 , copyright_<"Copyright 2023 Sygaldry contributors">
                 , license_<"SPDX-License-Identifier: MIT">
{
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

    void _try_to_match_and_execute(Components& components)
    {
        boost::pfr::for_each_field(commands, [&](auto& command)
        {
            if (not osc_match_pattern(argv[0], command.name())) return;
            int retcode;
            if constexpr (std::is_same_v<decltype(command), Help&>)
            {
                retcode = command.main(log, commands);
            }
            else retcode = command.main(argc, argv, log, components);
            if (retcode != 0) _complain_about_command_failure(retcode);
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
            _try_to_match_and_execute(components);
            _reset();
        }

        if (_overflow())
        {
            log.println("CLI line buffer overflow!");
            _reset();
        }
    }

    void external_sources(Components& components)
    {
        while(reader.ready()) process(reader.getchar(), components);
    }
};

struct DefaultCommands
{
    Help help;
    List list;
    Describe describe;
    Set set;
};

template<typename Reader, typename Logger, typename Components>
using Cli = CustomCli<Reader, Logger, Components, DefaultCommands>;

template<typename Components>
using CstdioCli = Cli<CstdioReader, sygup::CstdioLogger, Components>;

///\}
///\}
} }

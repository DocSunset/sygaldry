#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <boost/pfr.hpp>
#include "sygaldry-utilities-consteval.hpp"

namespace sygaldry { namespace bindings { namespace clicommands {
struct Help
{
    static _consteval auto name() { return "/help"; }
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

    int main(auto& log, auto& commands)
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

} } }

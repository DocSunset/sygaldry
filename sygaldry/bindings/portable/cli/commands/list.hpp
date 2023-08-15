#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <type_traits>
#include "sygaldry-utilities-consteval.hpp"
#include "sygaldry-concepts-components.hpp"
#include "sygaldry-bindings-portable-osc_string_constants.hpp"

namespace sygaldry { namespace bindings { namespace clicommands {

struct List
{
    static _consteval auto name() { return "/list"; }
    static _consteval auto usage() { return ""; }
    static _consteval auto description() { return "List the components available to interact with through the CLI"; }

    int main(int argc, char** argv, auto& log, auto& components)
    {
        for_each_component(components, [&](const auto& component)
        {
            log.println(osc_path_v< std::remove_cvref_t<decltype(component)>
                                  , std::remove_cvref_t<decltype(components)>
                                  >);
        });
        return 0;
    }
};

} } }

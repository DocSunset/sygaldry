/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include "sygaldry-bindings-portable-spelling.hpp"
#include "sygaldry-concepts-metadata.hpp"
#include "sygaldry-concepts-components.hpp"
#include "sygaldry-helpers-metadata.hpp"
#include "sygaldry-bindings-portable-osc_string_constants.hpp"
#include "sygaldry-utility_components-portable-cstdio_logger.hpp"

namespace sygaldry { namespace bindings {

template<typename Logger, typename Components>
struct OutputLogger : name_<"Output Logger">
{
    struct inputs_t {} inputs;

    [[no_unique_address]] Logger log;

    output_endpoints_t<Components> last_out_list{};

    void external_destinations(Components& components)
    {
        tuple_for_each(last_out_list, [&]<typename T>(T& last_out)
        {
            const T& current_out = find<T>(components);
            if (value_of(current_out) != value_of(last_out))
            {
                if constexpr (Bang<T>)
                {
                    if (value_of(current_out))
                        log.println(osc_path_v<T, Components>);
                    return;
                }
                else
                {
                    last_out = current_out;
                    log.print(osc_path_v<T, Components>);
                    if constexpr (has_value<T>)
                        log.print(" ", value_of(current_out));
                    log.println();
                }
            }
        });
    }
};

template<typename Components> using CstdioOutputLogger = OutputLogger<utility_components::CstdioLogger, Components>;

} }

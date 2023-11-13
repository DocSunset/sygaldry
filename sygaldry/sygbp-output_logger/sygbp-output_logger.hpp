/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include "sygbp-spelling.hpp"
#include "sygac-metadata.hpp"
#include "sygac-components.hpp"
#include "sygah-metadata.hpp"
#include "sygbp-osc_string_constants.hpp"
#include "sygup-cstdio_logger.hpp"

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp
///\{
///\defgroup sygbp-output_logger sygbp-output_logger: Output Logger
///\{

template<typename Logger, typename Components>
struct OutputLogger : name_<"Output Logger">
{
    struct inputs_t {} inputs;

    [[no_unique_address]] Logger log;

    void external_destinations(Components& components)
    {
        for_each_output(components, [&]<typename T>(T& current_out)
        {
            if constexpr (Bang<T> || OccasionalValue<T>)
            {
                if (not flag_state_of(current_out)) return;
                log.println(osc_path_v<T, Components>);
            }
            else if constexpr (requires (T t) {current_out == t;})
            {
                static T last_out{};
                if (value_of(current_out) == value_of(last_out)) return;
                last_out = current_out;
                log.print(osc_path_v<T, Components>);
                if constexpr (has_value<T>)
                    log.print(" ", value_of(current_out));
                log.println();
            }
            else return;
        });
    }
};

template<typename Components> using CstdioOutputLogger = OutputLogger<sygup::CstdioLogger, Components>;

///\}
///\}
} }

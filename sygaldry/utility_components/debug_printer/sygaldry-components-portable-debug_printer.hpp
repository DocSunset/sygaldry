#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygaldry-helpers-metadata.hpp"

namespace sygaldry { namespace components { namespace portable

template<typename Logger, string_literal message>
struct DebugPrinter
: name_<"Debug Printer">
, description_<"prints the runtime stage it is currently running">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    [[no_unique_address]] Logger log;
    void init() { log.println(message.value, " init"); }
    void external_sources() { log.println(message.value, " external_sources"); }
    void main() { log.println(message.value, " main"); }
    void external_destinations() { log.println(message.value, " external_destinations"); }
};

} } }

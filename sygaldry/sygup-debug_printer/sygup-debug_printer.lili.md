\page page-sygup-debug_printer sygup-debug_printer Debug Printer

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

When debugging issues on embedded systems, it is often helpful to insert
textual printouts at various locations in the running program in order to
attempt to isolate the moment at which issues occur. The following simple
component allows inserting such printouts in the runtime without having
to modify other components. It's subroutines simply print a message
customized by a template parameter.

```cpp
// @#'sygup-debug_printer.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"

namespace sygaldry { namespace sygup {
/// \addtogroup sygup
/// \{
/// \defgroup sygsp-debug_printer sygsp-debug_printer: Runtime Stage Printer for Debugging
/// \{

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

/// \}
/// \}
} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygup-debug_printer)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-metadata
        )
# @/
```

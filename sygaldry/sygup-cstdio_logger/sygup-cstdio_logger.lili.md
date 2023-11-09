\page page-sygup-cstdio_logger sygup-cstdio_logger: C Standard Input/Output Logger

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Another putter for use with the [basic logger](\ref page-sygup-basic_logger),
useful for components in environments where the C library `putc` is available,
puts messages onto the standard output.

```cpp
// @#'sygup-cstdio_logger.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include <cstdio>
#include <cstdlib>
#include "sygup-basic_logger.hpp"

namespace sygaldry { namespace sygup {
/// \addtogroup sygup
/// \{
/// \defgroup sygup-cstdio_logger sygup-cstdio_logger: C Standard Input/Output Logger
/// \{

struct CstdioPutter
{
    void operator()(char c)
    {
        auto retcode = std::putchar(c);
        if (retcode == EOF)
        {
            // I guess we're unexpectedly done now?
            std::exit(EXIT_FAILURE);
        }
    }
};

using CstdioLogger = BasicLogger<CstdioPutter>;

/// \}
/// \}
} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygup-cstdio_logger)

add_library(${lib} INTERFACE)
target_include_directories(${lib}
        INTERFACE .
        )
target_link_libraries(${lib}
        INTERFACE sygup-basic_logger
        )
# @/
```


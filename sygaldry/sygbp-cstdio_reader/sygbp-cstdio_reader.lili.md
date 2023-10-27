\page page-sygbp-cstdio_reader sygbp-cstdio_reader: C Standard Input/Output Reader

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

# Cstdio Reader

On platforms where C standard input/output is available, the following
[basic reader](\ref page-sygbp-basic_reader) implementation can be used. Notice
that this implementation requires the user to check `ready()` before *every*
call to `getchar()`, which doesn't actually get a character...

```cpp
// @#'sygbp-cstdio_reader.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/


#include <stdio.h>
#include <stdlib.h>

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp
///\{
///\defgroup sygbp-cstdio_reader sygbp-cstdio_reader: C Standard Input/Output Reader
///\{

/*! \brief Command line input reader using standard IO calls

This reader calls `getc` to get one character. There are some limitations to
this implementation:

- `ready()` needs to be called once before every call to `getchar()`, which doesn't actually
  get any characters
- on some platforms, `getc` is blocking, and this implementation cannot be used

It remains as future work to resolve the latter issue, but the former is
considered part of the API. Beware!
*/
struct CstdioReader
{
    int last_read;
    bool ready()
    {
        last_read = getc(stdin);
        return last_read != EOF;
    }

    char getchar()
    {
        return last_read;
    }
};

///\}
///\}
} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygbp-cstdio_reader)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
    #target_sources(${lib}
            #PRIVATE ${lib}.cpp
            #)
# @/
```

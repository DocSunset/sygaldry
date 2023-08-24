\page sygin-test Sygaldry Instrument Test

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

```cpp
// @#'sygin-test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygac-runtime.hpp"
#include "sygsp-button.hpp"
#include "sygbp-cli.hpp"
#include "sygbp-output_logger.hpp"

using namespace sygaldry;

struct Button1 : name_<"button1">, components::portable::ButtonGestureModel
{
    void main()
    {
        components::portable::ButtonGestureModel();
    }
};

struct Button2 : name_<"button2">, components::portable::ButtonGestureModel
{
    void main()
    {
        components::portable::ButtonGestureModel();
    }
};

struct Test
{
    struct Instrument {
        Button1 button1;
        Button2 button2;
    } instrument;
    bindings::CstdioCli<Instrument> cli;
} test;

constexpr auto runtime = Runtime{test};

int main()
{
    runtime.app_main();
}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(exe sygin-test)
add_executable(${exe} sygin-test.cpp)
target_link_libraries(${exe} PRIVATE sygaldry)
# @/
```

\page bind_demo Bindings Demo

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

For quick tests and experimentation, we would like to be able to run our
bindings as an interactive command line application in the OS of our choice,
allowing us to run our components in a simulated environment, feed them test
inputs, and examine their outputs. The same implementation should also be
usable on any environment that provides `getchar` and `putchar` from the C
standard library.

```cpp
// @#'demo.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "utilities/consteval.hpp"
#include "concepts/components.hpp"
#include "concepts/runtime.hpp"
#include "helpers/metadata.hpp"
#include "components/sensors/button.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/basic_reader/cstdio_reader.hpp"
#include "bindings/basic_logger/cstdio_logger.hpp"
#include "bindings/cli/cli.hpp"
#include "bindings/liblo.hpp"
#include "bindings/output_logger.hpp"

using namespace sygaldry;
using namespace sygaldry::components;
using namespace sygaldry::bindings;


struct AppComponents {
    struct api_t
    {
        TestComponent tc;
        struct Button : name_<"Button">, ButtonGestureModel {} bgm;
    } api;

    //Cli<CstdioReader, CstdioLogger, decltype(api)> cli;
    LibloOsc<decltype(api)> osc;
    OutputLogger<CstdioLogger, decltype(api)> out;
} constinit assemblage{};

static_assert(Component<decltype(assemblage.osc)>);

constexpr auto runtime = Runtime{assemblage};

int main()
{
    runtime.init();
    for (;;)
    {
        runtime.tick();
        usleep(30*1000);
    }
}
// @/
```

```cmake
# @#'CMakeLists.txt'
# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(bindings-demo demo.cpp)
target_link_libraries(bindings-demo PRIVATE ${LIBLO_LIBRARIES})
target_include_directories(bindings-demo PRIVATE ${LIBLO_INCLUDE_DIRS})
# @/
````

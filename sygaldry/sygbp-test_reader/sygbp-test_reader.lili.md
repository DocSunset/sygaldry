\page page-sygbp-test_reader Test Reader

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

# Test Reader

The test reader implements a [basic reader](\ref page-sygbp-basic_reader).

We need a reader that we can use in test cases to inject input into our
components. We define one with a string stream from which we pull the
inputs.

```cpp
// @#'sygbp-test_reader.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/


#include <string>
#include <sstream>

namespace sygaldry { namespace bindings {

struct TestReader
{
    std::stringstream ss;
    bool ready() {return std::stringstream::traits_type::not_eof(ss.peek());}
    char getchar() {return static_cast<char>(ss.get());}
};

} }
// @/

// @#'sygbp-test_reader.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>

#include "sygbp-test_reader.hpp"

using namespace sygaldry::bindings;

TEST_CASE("sygaldry TestReader")
{
    TestReader reader{};
    reader.ss.str("Hello world!\n");

    REQUIRE(reader.ready());
    REQUIRE(reader.getchar() == 'H');

    char last_char = 'x';
    while (reader.ready())
    {
        last_char = reader.getchar();
    }

    REQUIRE(last_char == '\n');
}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygbp-test_reader)

if (SYGALDRY_BUILD_TESTS)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)

add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
catch_discover_tests(${lib}-test)
endif()
# @/
```

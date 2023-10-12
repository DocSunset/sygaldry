\page page-sygup-test_logger Test Logger

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

The test putter is used for tests with the
[basic logger](\ref page-sygup-basic_logger), putting messages into a string.
We define the putter and declare an instantiation of the basic logger template,
which we define in a separate `.cpp` file so that the instantiation will only
be compiled once.

```cpp
// @#'sygup-test_logger.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include <sstream>
#include "sygup-basic_logger.hpp"

namespace sygaldry { namespace sygup {
/// \addtogroup sygup
/// \{
/// \defgroup sygup-test_logger sygup-test_logger: Test Logger
/// \{

struct TestPutter
{
    std::stringstream ss;
    void operator()(char c)
    {
        ss << c;
    }
};

using TestLogger = BasicLogger<TestPutter>;

/// \}
/// \}
} }
// @/
```

```cpp
// @#'sygup-test_logger.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>

#include "sygup-test_logger.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <array>

using namespace sygaldry::sygup;

@{tests}

TEST_CASE("sygaldry BasicLogger print") {
    TestLogger logger;
    logger.put.ss.str("");

    SECTION("Printing integers")
    {
        logger.print(42);
        REQUIRE(logger.put.ss.str() == "42");
    }

    SECTION("Printing floating-point numbers")
    {
        logger.print(-2.71828);
        REQUIRE(logger.put.ss.str() == "-2.71828");
    }

    SECTION("Printing strings")
    {
        logger.print("Hello world!");
        REQUIRE(logger.put.ss.str() == "Hello world!");
    }

    SECTION("Printing arrays")
    {
        logger.print(std::array<int, 3>{1,2,3});
        REQUIRE(logger.put.ss.str() == "[1 2 3]");
    }

    SECTION("Variadic print")
    {
        logger.print("1", 2, " ", 3.14159);
        REQUIRE(logger.put.ss.str() == "12 3.14159");
    }

    SECTION("Empty println")
    {
        logger.println();
        REQUIRE(logger.put.ss.str() == "\n");
    }
}
// @/
```

```cmake
# @#'CMakeLists.txt'
if (SYGALDRY_BUILD_TESTS)

set(lib sygup-test_logger)

add_library(${lib} INTERFACE)
target_include_directories(${lib}
        INTERFACE .
        )
target_link_libraries(${lib}
        INTERFACE sygup-basic_logger
        )

add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
catch_discover_tests(${lib}-test)

endif()
# @/
```

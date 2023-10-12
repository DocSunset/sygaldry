\page page-sygah-string_literal sygah-string_literal: String Literal Template Parameter Wrapper

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

All components and endpoints are required to have a name. To facilitate setting
this conveniently, one approach is to define a thin wrapper around a string
literal that allows us to pass string literals as template parameters. Then we
define a base class that takes a string template parameter and synthesizes a
compile-time evaluated name method.

```cpp
// @#'sygah-string_literal.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <cstddef>
#include "sygah-consteval.hpp"

namespace sygaldry {

/// \ingroup sygah Sygaldry Helpers
/// \{

/// \defgroup sygah-string_literal sygah-string_literal: String Literal Template Parameter Wrapper
/// \{

/// A wrapper around a string lteral that enables using strings as template parameters.
template<std::size_t N>
struct string_literal
{
    /// The string buffer
    char value[N];
    /// Constructor from string literal
    _consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

/// \}
/// \}
}
// @/
```

```cpp
// @#'sygah-string_literal.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include "sygah-string_literal.hpp"

using namespace sygaldry;

using std::string_view;

template<string_literal str>
_consteval auto name() {return string_view{str.value};}

TEST_CASE("sygaldry String literal", "[endpoints][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygah-string_literal)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygah-consteval)
target_include_directories(${lib} INTERFACE .)

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain
        PRIVATE ${lib}
        )
catch_discover_tests(${lib}-test)
endif()
# @/
```

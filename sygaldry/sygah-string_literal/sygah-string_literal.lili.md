\page page-sygah-string_literal sygah-string_literal: String Literal Template Parameter Wrapper

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

It's common for components and endpoints to have textual metadata. Endpoints
generally have a name and description, and may have a unit. Components generally
have a name and description, and may have additional metadata such as an author,
version, copyright notice, and other information that may be useful to have at
runtime for support and informational purposes. To facilitate exposing this
metadata will minimum runtime cost and with a minimum of code to maintain, it
may be convenient to supply such textual metadata as template parameters to
a helper class.

As of C++20 it's not possible to pass a string literal as a non-type template
parameter, but it is possible to use certain user defined types. The class
below provides a compile-time constructor that takes a string literal as
argument and copies it into an array of characters that can be treated as a
string literal and accepted anywhere a string literal is expected. When a
non-type template parameter expects a `string_literal`, an actual string
literal can be supplied to the template in that position and the compiler
accepts it because of the availability of the compile-time constructor.

This helper is mainly used in \ref page-sygah-metadata, but is kept seperate so
that it can be drawn on any time string literals are needed as template
parameters.

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

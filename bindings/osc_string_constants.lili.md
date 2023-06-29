# OSC String Constants

OSC paths and type tag strings can be determined at compile time from the
metadata of an endpoint in an assemblage of components.

[TOC]

# OSC Paths

```cpp
// @+'tests'
TEST_CASE("osc path")
{
    struct root_t { static _consteval const char * name() {return "Root";} };
    struct leaf_t { static _consteval const char * name() {return "leaf";} };
    using Path = std::tuple<root_t, leaf_t>;
    CHECK(string_view(osc_path<Path>::value.data()) == string_view("/Root/leaf"));
}
// @/
```

Using the `path_t` metafunction defined in
[`concepts/components.lili`](concepts/components.lili.md), generating an OSC
address for an endpoint amounts to simply concatenating
[the `snake_case_v` spelling](bindings/spelling.lili.md) of each node in the
path. The implementation is fairly similar to that of the respeller. We
allocate a `std::array` with the appropriate size and copy the snake case names
of each node.

```cpp
// @+'string length functions'
template<typename> struct osc_path_length : std::integral_constant<std::size_t, 0> {};
template<template<typename...>typename L, typename ... Path>
struct osc_path_length<L<Path...>>
: std::integral_constant<std::size_t, (name_length<Path>() + ...) + sizeof...(Path)> {};
// @/

// @='osc paths'
template<typename> struct osc_path;
template<template<typename...>typename L, typename ... Path>
struct osc_path<L<Path...>>
{
    static constexpr size_t N = osc_path_length<L<Path...>>() + 1; // + 1 for null terminator
    static constexpr std::array<char, N> value = []()
    {
        L<Path...> path;
        std::array<char, N> ret;
        std::size_t write_pos = 0;
        auto copy_one = [&]<typename T>(T)
        {
            ret[write_pos++] = '/';
            for (std::size_t i = 0; i < name_length<T>(); ++i)
            {
                ret[write_pos++] = snake_case_v<T>[i];
            }
        };
        std::apply([&]<typename ... Ts>(Ts... ts)
        {
            (copy_one(ts), ...);
        }, path);
        ret[write_pos] = 0;
        return ret;
    }();
};

template<typename T, typename Components>
constexpr const char * osc_path_v = osc_path<path_t<T,Components>>::value.data();
// @/

```

# OSC Types

```cpp
// @='osc types'
// TODO: define a conversion from endpoint to typespec
template<typename T> constexpr const char * osc_types_v = "f";
// @/
```

# Summary

```cpp
// @#'osc_string_constants.hpp'
#pragma once
#include <array>
#include <tuple>
#include "concepts/components.hpp"
#include "concepts/endpoints.hpp"
#include "bindings/spelling.hpp"

namespace sygaldry { namespace bindings {

@{string length functions}

@{osc paths}

@{osc types}

} }
// @/

// @#'tests/osc_string_constants/tests.cpp'
#include <string_view>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "helpers/endpoints.hpp"
#include "bindings/osc_string_constants.hpp"

using std::string_view;
using namespace sygaldry;
using namespace sygaldry::bindings;

@{tests}
// @/
```

```cmake
# @#'tests/osc_string_constants/CMakeLists.txt'
add_executable(osc-string-constants-tests tests.cpp)
target_link_libraries(osc-string-constants-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(osc-string-constants-tests)
# @/
```

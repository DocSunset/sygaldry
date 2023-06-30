# OSC String Constants

An OSC message consists of its address pattern (called its path in some
implementations), a type tag string, and zero or more arguments. This document
concerns the compile-time generation of OSC paths and type tag strings by
reflecting over an endpoint and its position in a component tree.

[TOC]

# OSC Paths

## Paths Background

An OSC address pattern, here termed an OSC path, is a typically URL-like
represetation of an OSC endpoint. In
[the original OSC specification](https://opensoundcontrol.stanford.edu/spec-1_0.html)
the only formal requirement for an OSC address pattern is that it should be
an OSC string, meaning a 32-bit padded null-terminated ASCII string, that begins
with the character `/`:

> "An OSC Address Pattern is an OSC-string beginning with the character `/` (forward slash)."

The semantics of OSC further stipulates that an OSC server should be structured
as an OSC address space, i.e. a tree of OSC container nodes and OSC method
leaves, where each node and method is identified by a name that doesn't contain
any of the character `#*,/?[]{}` and `' '` (space). An OSC address is then the
fully qualified `/` seperated path in the address space that identifies an OSC
method, beginning with a `/` character that represents the root of the tree,
proceeding through the name of each node leading to the method seperated by
slashes, and ending with the name of the method. "The syntax of OSC Addresses
was chosen to match the syntax of URLs."

Notably, this semantics implies that OSC servers should be the authority on the
interpretation of the OSC messages that they receive, and that clients of a
given server are expected to send messages known to be compatible with the
server in order to invoke the execution of the server's a-priori-understood
methods. In common practice, this is often not the case. Instead, particularly
when making input devices such as a gestural controller, it is just as common
for an OSC "client" to send messages to a "server" that reflect the state of
the "client's" endpoints, such as the state of a button or slider. In such
cases, the client has authority on the interpretation of the messages, which
formally don't adhere to the OSC semantics described in the specification.

In either case, whether receiving messages from an authoritative client or
sending them to an authoritative server, it is one of the central limitations
of OSC as a protocol that no provisions are available to exchange information
about the interpretation of an OSC message between participants in the
OSC network. Later protocols such as libmapper and OSC-query were developed,
among other reasons, to address this shortcoming. A similar deficiency in
the ubiquitous MIDI 1.0 protocol is meant to be addressed by some of the recent
MIDI 2.0 protocol updates.

## Paths Implementation

For now, we presume that component and endpoint names will naturally tend not
to include any of the prohibited characters, except spaces. We will use the
[`snake_case`](bindings/spelling.lili.md) respeller to remove spaces, and we
will ignore the issue of other prohibited characters for now. Future work
should endeavour to verify their absence, or escape prohibted characters
e.g. using the '%XX' encoding commonly employed in URLs, where 'XX' are
hexadecimal digits for the encoding of the escaped character.

We will leverage [the `path_t` metafunction](concepts/components.lili.md) to
derive a type list representing the path to an endpoint in the component tree
and use this representation to derive the path string.

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

Generating the OSC path string from the path type list should be a reasonably
simple matter of copying the `snake_case` spelt name of each node in this path
to a static buffer. The implementation is fairly similar to that of
[the respeller class](bindings/spelling.lili.md). We allocate a `std::array`
with the appropriate size and copy the snake case names of each node to the
buffer.

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

After the address pattern, an OSC message contains a type tag string. This is a
32-bit-aligned trailing-zero-padded null-terminated ASCII string beginning with
a comma `,` and containing a sequence of ASCII character type tags that
represent the type of the OSC arguments, in order. We wish to provide type-tag
string representations for all our supported endpoint helpers.

Notably, unlike the OSC path which depends on an endpoint *and its place in
the node tree*, an OSC type tag string depends only on the endpoint.

Currently, we conservatively stick to OSC 1.0 type tags, avoiding 1.1 extended
types, for simplicity and in hopes of improving compatibility.

```cpp
// @+'tests'
TEST_CASE("osc type tag string")
{
    CHECK(string_view(osc_type_string_v<button<"test button">>)             == string_view(",i"));
    CHECK(string_view(osc_type_string_v<toggle<"test toggle">>)             == string_view(",i"));
    CHECK(string_view(osc_type_string_v<text<"test text">>)                 == string_view(",s"));
    CHECK(string_view(osc_type_string_v<text_message<"test text message">>) == string_view(",s"));
    CHECK(string_view(osc_type_string_v<slider<"test slider">>)             == string_view(",f"));
    CHECK(string_view(osc_type_string_v<bng<"test bang">>)                  == string_view(","));
}
// @/
```

Following the same strategy as above, we statically allocate a `std::array` of appropriate
size and `constexpr` initialize it with an appropriate string. As all of our supported
endpoints at the time of writing have only one argument, the size is constant (four), and
the type of the endpoint is easily determined using
[the endpoint concepts](concepts/endpoints.lili.md).

```cpp
// @='osc types'
template<typename T>
struct osc_type_string
{
    static constexpr size_t N = 4;
    static constexpr std::array<char, 4> value = []()
    {
        std::array<char, N> ret{0};
        ret[0] = ',';
        if constexpr (Bang<T>) return ret;
        else if constexpr (std::integral<value_t<T>>) ret[1] = 'i';
        else if constexpr (std::floating_point<value_t<T>>) ret[1] = 'f';
        else if constexpr (string_like<value_t<T>>) ret[1] = 's';
        return ret;
    }();
};

template<typename T> constexpr const char * osc_type_string_v = osc_type_string<T>::value.data();
// @/
```

# Summary

```cpp
// @#'osc_string_constants.hpp'
#pragma once
#include <array>
#include <tuple>
#include <concepts>
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

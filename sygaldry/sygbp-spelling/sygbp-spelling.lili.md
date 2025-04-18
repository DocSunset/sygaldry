\page page-sygbp-spelling sygbp-spelling: Spelling

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

The base helpers used by components and endpoints to declare their names
conveniently and concepts for bindings to access names generically are
defined in \ref sygah-metadata and \ref sygac-metadata
respectively. This document deals with transformations over names required for
bindings.

Names are spelled differently when generating bindings. For example, a processor
that applies a one pole exponential moving average low pass filter to its input
might be called "Simple Lowpass", and an endpoints called "cutoff frequency".
In its Open Sound Control binding, these might have to be spelled differently,
such as "SimpleLowpass" and "SimpleLowpass/cutoff_frequency". In a command-line
style binding or as a Pure Data externail, it might be more convenient and
idiomatic to spell them "simple-lowpass" and "cutoff-frequency". In other
contexts, different spellings might be required.

To support all these use cases, one approach is for the author of the processor
to specify different spellings for alternative applications. For example, the
name of the processor might be specified in this way:

```cpp
struct SimpleLowpass
{
    static _consteval const char * name() {return "Simple Lowpass";}
    static _consteval const char * camel_name() {return "SimpleLowpass";}
    static _consteval const char * snake_name() {return "simple_lowpass";}
    static _consteval const char * kebab_name() {return "simple-lowpass";}

    struct inputs_t
    {
        struct cutoff_t
        {
            static _consteval const char * name() {return "cutoff frequency";}
            static _consteval const char * camel_name() {return "cutoffFrequency";}
            static _consteval const char * snake_name() {return "cutoff_frequency";}
            static _consteval const char * kebab_name() {return "cutoff-frequency";}
            static _consteval const char * osc_path() {return "SimpleLowpass/cutoff_frequency";}
            float value;
        } cutoff;
    } inputs;

    // low pass implementation here ...
};
```

This obviously has some shortcomings, not least of all that the author has to
manually repeat the name with however many spellings are required. Generating
an address space, such as for Open Sound Control bindings, might be especially
cumbersome. The various spelling conventions are reasonably regular, so it
should be possible to do the conversion from one canonical natural language
name to various technical spellings with an algorithm. Since the name is
presumably known at compile time, these conversion can be performed at compile
time, imposing no runtime cost.

# Basic transformations

Snake case is achieved by replacing spaces with underscores. Kebab case is achieved
by replacing spaces with dashes. Lowercase is achieved by replacing uppercase
letters with their lowercase equivalent. Uppercase, also called all caps, also
called yelling, does the inverse of lowercase. These are all simple one-to-one
transformations that are easily defined on a character by character basis.

```cpp
// @='mappings'
constexpr char snake(char c) {return c == ' ' ? '_' : c;}
constexpr char kebab(char c) {return c == ' ' ? '-' : c;}
constexpr char lower(char c)
{
    if ('A' <= c && c <= 'Z') return c+('a'-'A');
    return c;
}
constexpr char upper(char c)
{
    if ('a' <= c && c <= 'z') return c-('a'-'A');
    return c;
}
// @/
```

Taking one or more of these mappings as template arguments, we can
generate a mapping that composes them sequentially:

```cpp
// @='compose'
typedef char (*char_mapping)(char);
template<char_mapping...mappings> struct compose;

template<char_mapping mapping> struct compose<mapping>
{
    constexpr char operator()(char c) { return mapping(c); }
};

template<char_mapping mapping, char_mapping... mappings> struct compose<mapping, mappings...>
{
    constexpr char operator()(char c) { return compose<mappings...>{}(mapping(c)); }
};
// @/
```

# Compile-time strings

The main challenge then is where to put the transformed string at compile time.
We're not allowed to allocate memory and then return a pointer to that memory,
so we can't return `const char *`. We could use `std::array` instead, but then
the user would have to extract the character pointer anytime they want to use
the data as a string, e.g. `snake_case("example name").data()`, which we would
like to avoid. It's also not possible to use a template variable e.g.
`snake_case<"example name">`, since string literals cannot be used as non-type
template parameters easily, and even if we use the `string_literal` type
defined in `helpers/metadata.lili`, we will eventually run into problems when
someone accidentally tries to pass a decayed string literal to our
transformation. The problem is that string literals decay to points at the
slightest provocation, losing essential information about their length in the
process.

One solution comes from the fact that the strings we want to process are
assumed to be `static _consteval` member functions of the named components and
endpoints we are reflecting over. From this, we can use the type of the
processor as a template parameter to a `struct` with its own `static constexpr`
string data, called `value` below. We can initialize this string with a lambda
that applies the mapping to the name of the component, which gives us the
following implementation:

```cpp
// @='respeller'
@{composition functor}

template<typename NamedType, char_mapping... Mappings>
struct respeller
{
    static constexpr size_t N = name_length<NamedType>() + 1; // + 1 for null terminator
    static constexpr std::array<char, N> value = [](const char * s)
    {
        auto mapping = compose<Mappings...>{}; // construct the composition of mappings
        std::array<char, N> ret{};
        for (size_t i = 0; i < N; ++i)
        {
            char c = s[i];
            ret[i] = mapping(c); // apply the mapping
        }
        return ret;
    }(NamedType::name());

    respeller(NamedType) {}
    constexpr operator const char *() noexcept { return value.data(); }
};
// @/
```

In case there are no mappings given, which may be useful for certain templates
e.g. as a default parameter, we can save some space by providing a specialization:

```cpp
// @+'respeller'
template<typename NamedType>
struct respeller<NamedType>
{
    respeller(NamedType) {}
    constexpr operator const char *() noexcept { return NamedType::name(); }
};
// @/
```

In order to statically allocate enough memory for the transformed string, we
need to know its size. We can write a simple compile-time evaluated function to
count how many characters there are in the string. We trust that the compiler
will catch if a non-null-terminated string is passed in, e.g. by recognizing
out of bounds access on the compile-time constant input.

```cpp
// @='string length function'
template<typename Device>
    requires requires {Device::name();}
_consteval auto name_length()
{
    size_t ret = 0;
    while (Device::name()[ret] != 0) ret++;
    return ret;
}
// @/
```

# Syntax sugar

To save the user having to write out `respeller` with all of its template
arguments, we provide some "aliases" for expected use cases, so that
the user can write `snake_case(x)` or `snake_case_v<T>` as seen above.

My initial thought here was to literally use a template type alias for the
function-like syntax--

```cpp
template<typename NamedType> using snake_case = respeller<NamedType, snake>;
```

--and this works fine with `gcc`. Unfortunately, `clang` doesn't allow
argument deduction for template aliases:

```sh
...thetest.cpp:18:24: error: alias template
'snake_case' requires template arguments; argument deduction only allowed for
class templates
        REQUIRE(string(snake_case(x)) == string("snake_case_example"));
                       ^
```

Instead, we can use derived classes. We have to re-define the constructor from `NamedType`,
but it'll do.

```cpp
// @='template aliases'
template<typename NamedType>
struct passthrough_spelling : respeller<NamedType>
{
    passthrough_spelling([[maybe_unused]] NamedType x) : respeller<NamedType>{x} {}
};

template<typename NamedType>
struct snake_case : respeller<NamedType, snake>
{
    snake_case([[maybe_unused]] NamedType x) : respeller<NamedType, snake>{x} {}
};

template<typename NamedType>
struct upper_snake_case : respeller<NamedType, snake, upper>
{
    upper_snake_case([[maybe_unused]] NamedType x) : respeller<NamedType, snake, upper>{x} {}
};

template<typename NamedType>
struct lower_snake_case : respeller<NamedType, snake, lower>
{
    lower_snake_case([[maybe_unused]] NamedType x) : respeller<NamedType, snake, lower>{x} {}
};

template<typename NamedType>
struct kebab_case : respeller<NamedType, kebab>
{
    kebab_case([[maybe_unused]] NamedType x) : respeller<NamedType, kebab>{x} {}
};

template<typename NamedType>
struct upper_kebab_case : respeller<NamedType, kebab, upper>
{
    upper_kebab_case([[maybe_unused]] NamedType x) : respeller<NamedType, kebab, upper>{x} {}
};

template<typename NamedType>
struct lower_kebab_case : respeller<NamedType, kebab, lower>
{
    lower_kebab_case([[maybe_unused]] NamedType x) : respeller<NamedType, kebab, lower>{x} {}
};
// @/

// @='template variables'
template<typename NamedType> constexpr const char * snake_case_v       =       snake_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * upper_snake_case_v = upper_snake_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * lower_snake_case_v = lower_snake_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * kebab_case_v       =       kebab_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * upper_kebab_case_v = upper_kebab_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * lower_kebab_case_v = lower_kebab_case<NamedType>::value.data();
// @/
```

# Tests

```cpp
// @#'sygbp-spelling.test.cpp'
#include <string_view>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "sygah-consteval.hpp"
#include "sygbp-spelling.hpp"
#include "sygah-metadata.hpp"

using std::string_view;
using namespace sygaldry;
using namespace sygaldry::sygbp;

TEST_CASE("sygaldry Snake case", "[utilities][metadata][names][snake]")
{
    SECTION("Snake Case Function-like Access")
    {
        struct example_t
        {
            static _consteval const char * name() {return "snake case example";}
        } x;

        REQUIRE(string_view(snake_case(x)) == string_view("snake_case_example"));
    }

    SECTION("Snake Case Template Variable Access")
    {
        struct example_t
        {
            static _consteval const char * name() {return "snake case example";}
        };

        REQUIRE(string_view(snake_case_v<example_t>) == string_view("snake_case_example"));
    }
}

TEST_CASE("sygaldry Kebab Case")
{
    struct example_t
    {
        static _consteval const char * name() {return "kebab case example";}
    } x;

    CHECK(string_view(kebab_case(x)) == string_view("kebab-case-example"));
    CHECK(string_view(kebab_case_v<example_t>) == string_view("kebab-case-example"));
}

TEST_CASE("sygaldry Upper/lower")
{
    struct example_t
    {
        static _consteval const char * name() {return "A Basic Example";}
    } x;
    CHECK(string_view(upper_snake_case(x)) == string_view("A_BASIC_EXAMPLE"));
    CHECK(string_view(lower_kebab_case(x)) == string_view("a-basic-example"));
}

TEST_CASE("sygaldry Spelling with helpers")
{
    struct example_t : name_<"Helper Example"> {} x;
    CHECK(string_view(upper_snake_case(x)) == string_view("HELPER_EXAMPLE"));
    CHECK(string_view(lower_kebab_case(x)) == string_view("helper-example"));
    CHECK(string_view(upper_kebab_case(x)) == string_view("HELPER-EXAMPLE"));
    CHECK(string_view(lower_snake_case(x)) == string_view("helper_example"));
}

TEST_CASE("sygaldry Passthrough spelling")
{
    struct example_t
    {
        static _consteval const char * name() {return "A Basic Example";}
    } x;
    REQUIRE(string_view(passthrough_spelling(x)) == string_view("A Basic Example"));
}
// @/
```

# Future Work

The `respeller` defined above assumes that the textual mapping functions are
one-to-one, e.g. spaces become underscores, lower case becomes uppercase, etc.
However, some common spelling conventions, e.g. `CamelCase` and `dromedaryCase`
can reduce the length of the output string. This will require some modification
of our approach, to allocate the correct string length, and to iterate through
the input string. In the most general framework, it might be useful to replace
arbitrary regular expression matches with given replacements, or to add strings
where matches are located. Such extensions should be added if these features
become necessary.

# Summary

```cpp
// @#'sygbp-spelling.hpp'
#pragma once
#include <array>
#include "sygah-consteval.hpp"

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp
///\{
///\defgroup sygbp-spelling sygbp-spelling: Spelling
///\{

@{string length function}
@{compose}
@{mappings}
@{respeller}
@{template aliases}
@{template variables}

///\}
///\}
} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygbp-spelling)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
    INTERFACE sygah-consteval
    )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ${lib}
        PRIVATE sygah-metadata
        )
catch_discover_tests(${lib}-test)
endif()
# @/
```

# OSC Address Pattern Matching

An OSC address (of an OSC Methd) is a null terminated string beginning with a
`/` and consisting of `/` seperated ASCII-encoded parts, similar to a URL, that
doesn't include any of the special characters `#*,?[]{}`. An address pattern
matches an address if both are the same, or if each `/` seperated part of the
address matches the corresponding `/` seperated part of the pattern. The parts
match according to the following rules given in the OSC 1.0 spec (Matt Wright).

## OSC Matching Rules

Wildcards: A in the pattern `?` matches any single character in the address.

```cpp
// @+'tests'
TEST_CASE("OSC match wildcards")
{
    CHECK(osc_match_pattern("/???", "/123"));
    CHECK(osc_match_pattern("/foo.?", "/foo.8"));
    CHECK(not osc_match_pattern("/foo.?", "/foo.42"));
}
// @/
```

Globs: A `*` in the pattern matches any sequence of zero or more characters in
the address.

```cpp
// @+'tests'
TEST_CASE("OSC match globs")
{
    CHECK(osc_match_pattern("/*", "/123"));
    CHECK(osc_match_pattern("/foo.*", "/foo.42"));
    CHECK(osc_match_pattern("/*/123", "/banana/123"));
    CHECK(not osc_match_pattern("/*/*", "/banana"));
    CHECK(not osc_match_pattern("/banana/*", "/apple/pie"));
}
// @/
```

Sets: A string of characters enclosed in `[]` brackets in the pattern matches
one character in the address if it is found inside the brackets.

```cpp
// @+'tests'
TEST_CASE("OSC match sets")
{
    CHECK(osc_match_pattern("/[123]23", "/123"));
    CHECK(osc_match_pattern("/[123]23", "/223"));
    CHECK(osc_match_pattern("/[123]23", "/323"));
    CHECK(not osc_match_pattern("/[123]23", "/423"));
}
// @/
```

Ranges: Two characters seperated by a `-` within brackets indicate the ASCII
collating sequence of characters starting with the first character and ending
with the last character, inclusive. A `-` at the beinning or end of a set has
no special interpretation.

The spec is not clear how to handle a range such as `[z-a]` where the beginning
and end are not in ASCII order. Liblo treats this as matching `z` and `a` and
nothing else. The CNMAT OSC library for Arduino requires the first character in
the range to be `<=` the address and address to be `<=` the second, so nothing
would match this range. In any case, such a range is obviously invalid, so we
adopt the latter approach since it's easier to implement, and let the user
figure it out.

```cpp
// @+'tests'
TEST_CASE("OSC match ranges")
{
    CHECK(osc_match_pattern("/[1-9]", "/1"));
    CHECK(osc_match_pattern("/[1-9]", "/5"));
    CHECK(osc_match_pattern("/[1-9]", "/9"));
    CHECK(not osc_match_pattern("/[1-9]", "/a"));
    CHECK(osc_match_pattern("/[-1]", "/-"));
    CHECK(osc_match_pattern("/[-1]", "/1"));
    CHECK(osc_match_pattern("/[1-]", "/-"));
    CHECK(osc_match_pattern("/[1-]", "/1"));
    CHECK(not osc_match_pattern("/[-1]", "/2"));
    CHECK(not osc_match_pattern("/[1-]", "/2"));
}
// @/
```

Inverted sets: If the first character within brackets is `!`, this inverts the
meaning of the brackets so that they match any character or range not given by
the expression inside the brackets.

```cpp
// @+'tests'
TEST_CASE("OSC match inverted sets")
{
    CHECK(not osc_match_pattern("/[!123]23", "/123"));
    CHECK(not osc_match_pattern("/[!123]23", "/223"));
    CHECK(not osc_match_pattern("/[!123]23", "/323"));
    CHECK(osc_match_pattern("/[!123]23", "/423"));
    CHECK(not osc_match_pattern("/[!1-9]", "/1"));
    CHECK(not osc_match_pattern("/[!1-9]", "/5"));
    CHECK(not osc_match_pattern("/[!1-9]", "/9"));
    CHECK(osc_match_pattern("/[!1-9]", "/a"));
    CHECK(not osc_match_pattern("/[!-1]", "/-"));
    CHECK(not osc_match_pattern("/[!-1]", "/1"));
    CHECK(not osc_match_pattern("/[!1-]", "/-"));
    CHECK(not osc_match_pattern("/[!1-]", "/1"));
    CHECK(osc_match_pattern("/[!-1]", "/2"));
    CHECK(osc_match_pattern("/[!1-]", "/2"));
    CHECK(osc_match_pattern("/[whatever!]", "/!"));
}
// @/
```

Substrings: A comma-separated list of strings enclosed in curly braces (e.g.,
“{foo,bar}”) in the pattern matches any one of the strings in the list
found in the address.

```cpp
// @+'tests'
TEST_CASE("OSC match substrings")
{
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/apple/pie"));
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/blueberry/pie"));
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/banana/pie"));
    CHECK(not osc_match_pattern("/{apple,banana,blueberry}/pie", "/sugar/pie"));
}
// @/
```

Normal characters: Any other character in the pattern matches the same
character in the address.

```cpp
// @+'tests'
TEST_CASE("OSC match regular")
{
    CHECK(osc_match_pattern("/sugar/pie", "/sugar/pie"));
    CHECK(not osc_match_pattern("/sugar/pie", "/apple/pie"));
}
// @/
```

# Summary

```cpp
// @#'tests/osc_match_pattern/tests.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "bindings/osc_match_pattern.hpp"

@{tests}
// @/

// @#'osc_match_pattern.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

// TODO: this should probably not be a header only implementation
/// Match an OSC address pattern against the given address
bool osc_match_pattern(const char * pattern, const char * address)
{
    switch (*pattern)
    {
    case '*':
        break;
    case '?':
        break;
    case '[':
        break;
    case '{':
        break;
    case '\0':
        if (*address == '\0') return true;
        break;
    default:
        if (*pattern == *address) return osc_match_pattern(++pattern, ++address);
        break;
    }
    return false;
}
// @/
```

```cmake
# @#'tests/osc_match_pattern/CMakeLists.txt'
add_executable(osc-match-tests tests.cpp)
target_link_libraries(osc-match-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(osc-match-tests)
# @/
```

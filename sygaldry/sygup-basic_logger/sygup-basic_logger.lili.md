\page page-sygup-basic_logger sygup-basic_logger: Basic Logger

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

# Motivation

As outlined in [the CLI component](\ref page-sygbp-cli), there may be a need
for bindings and devices to have some means of writing textual messages, e.g.
to a serial interface such as a UART, to a text-based UI such as a console, or
simply to debugging logs. Our preferred method of achieving this is to inject a
logger class into devices which need one through a template parameter, i.e. as
an internal plugin in the sense described in \ref page-docs-design_concepts.
This unfortunately imposes an implicit dependency on the API of the logger, but
provides the best runtime performance, opportunity for compile-time programming
optimizations, and type safety.

Ideally, we would use an excellent available logging facility, such as `fmtlib`
used by Avendish, or eventually `std::format`. However, these facilities may not
be available on all platforms we wish to target, for one reason or another,
so it may be worthwhile to have a backup facility that can be easily ported to
a different platform as needed.

# Design Thinking

We would like for an author porting this basic logging class to a different
platform to require minimum effort to do so. The simplest thing that seems
readily obvious to me is to provide a single template parameter for a callback
function that is used to write one character into, presumably, a buffer for
the driver that actually writes to the text output, e.g. UART. This is a
push-based strategy. The next simplest thing that occurse to me would be for
the logging class to handle buffering, and allow the platform driver author
to call a method of the logger to get one character from the buffer, or to
get a span of characters that are ready to be printed together. This is a
pull-based strategy.

In the push-based strategy, the basic logger doesn't need to manage memory
buffering. It can dynamically allocate memory needed to format incoming
messages, push the formatted string to the platform driver, and immediately
free the allocated memory. However, if the platform is not able to accomodate
incoming data when the logger tries to push, the decision about what to do
falls to the platform. The logger, and its client, may become blocked while
waiting for the platform, for example.

In the pull-based strategy, the basic logger is less dependent on the
platform's readiness. In exchange, the logger has to allocate and manage memory
in a way that is appropriate for all platforms. In case allocated memory runs
out, the logger may stall it client while allocating more memory, and if
allocation fails then there is no apparent way to recover except perhaps to
drop messages.

In either case, issues only arise if the platform is unable to consume data
faster than the logger and its clients can generate it. To guarantee this
condition requires information about both how quickly the platform can consume
data, and how quickly the logger's clients can produce it, and it is
consequently impossible to ensure safety from logger hangups without
coordination of both clients and the platform, which is arguably outside the
scope of the logger's responsibilities.

Consequently, these complexities are deferred to the platform driver author.
A push-based strategy is chosen, and it is assumed that the platform drivers
will be written to accomodate the requirements of the clients via the logger
so that resource contention or exhaustion issues are avoided. This gives us
the following overall plan:

```cpp
// @='basic logger plan'
template<typename putter>
struct BasicLogger
{
    [[no_unique_address]] putter put;

    template<typename cvrT> void print_(cvrT& x)
    {
        using T = std::remove_cvref_t<cvrT>;

        @{stack allocated buffer}

        @{convert message to a string}

        for (char c : string_message)
            put(c);
    };

    template<typename ... Ts> void print(Ts... x)
    {
        (print_(x), ...);
    }

    template<typename ... Ts> void println(Ts... x)
    {
        print(x...);
        print("\n");
    }
};
// @/
```

# Convert message to a string

At this stage, for ease of rapid prototyping, we'll simply make use of C++
standard library functionality to implement the `print` template. For now, we
will support only numeric conversions, string views, and string literals. As
seen below, the conversion to string is reasonably trivial for boolean and
string-like values:

```cpp
// @+'basic logger includes'
#include <type_traits>
#include <string_view>
#include <string>
// @/

// @='convert message to a string'
std::string_view string_message;
if constexpr (std::is_same_v<T, bool>)
{
    string_message = x ? "true" : "false";
}
else if constexpr (std::is_arithmetic_v<T>)
{
    @{convert a number}
}
else if constexpr (requires {string_message = x;})
    string_message = x;
else if constexpr (requires {x[0]; x.size();})
{
    print("[", x[0]);
    for (std::size_t i = 1; i < x.size(); ++i) print(" ", x[i]);
    print("]");
    return;
}
else string_message = "unknown type for basic logger";
// @/
```

The conversion functions defined in `<charconv>` since C++17 purport to provide
the best performance for numeric conversions, so we'll use those, specifically
`std::to_chars`. It requires a buffer in which to write the characters. We'll
place a scratch buffer that should heuristically be long enough on the stack,
and in case it's ever too small, we'll write an error message instead of the
number and hopefully someone will fix it... This will likely never be the case,
as `<numeric_limits>` provides information about the maximum number of base 10
digits representable by a given numeric type, and these are conservatively
augmented with additional padding space to be safe.

```cpp
// @+'basic logger includes'
#include <limits>
#include <charconv>
// @/

// @='stack allocated buffer'
constexpr int max_num_digits =
(
    // floats: mantissa digits + exponent digits + sign + e + exponent sign
    std::is_floating_point_v<T> ? std::numeric_limits<T>::max_digits10
                                  + std::numeric_limits<T>::max_exponent10 + 3

    // ints: digits + sign
    : (std::numeric_limits<T>::digits10 + std::is_signed_v<T>) ? 1 : 0
);

constexpr int buffer_size = max_num_digits + 16; // Extra padding for safety

char buffer[buffer_size];
// @/

// @='convert a number'
auto [ptr, ec] = std::to_chars(buffer, buffer + buffer_size, x);

if (ec == std::errc()) {
    string_message = std::string_view(buffer, ptr - buffer);
} else {
    string_message = "error:\n\
        bindings/basic_logger/basic_logger.lili:'convert a number'\n\
        std::to_chars scratch buffer unexpectedly too small!";
}
// @/
```
# Summary

```cpp
// @#'sygup-basic_logger.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

@{basic logger includes}

namespace sygaldry { namespace sygup {
/// \addtogroup sygup sygup: Portable Utilities
/// \{
/// \defgroup sygup-basic_logger sygup-basic_logger: Basic Logger
/// Literate source code: page-sygup-basic_logger
/// \{

@{basic logger plan}

/// \}
/// \}
} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygup-basic_logger)

add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
# @/
```

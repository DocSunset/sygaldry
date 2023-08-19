\page page-sygsp-array_order_mapping Array Order Mapping

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This component implements a simple mapping that changes the order of the
elements of an array based on a given specification. It's a patch bay, or a
combinator. It was originally implemented for use by the [Trill Craft
component](\ref sygsa-trill_craft) in the T-Stick to
facilitate changing the order of the raw sensor data from the order in which it
is measured by the sensor to the order in which the touch sensors appear in a
T-Stick.

# Core Implementation

```cpp
// @='core implementation'
/*! \brief Change the order of the elements in an array or array-like endpoint

\param[in] in The input array to reorder.
\param[out] out An array in which the reordered output can be written.
\param[in] size The expected number of elements in the in, out, and order arrays.
\param[in] order

A specification of the output order, such that the nth element `order[n]`
states the index into the input array from which the nth element of the output
should be given. E.g. to reverse a 3-element input, give `order = {2,1,0};`.

\warning

It is assumed without checking to that all elements of `order` are valid
indices into the input array, e.g. non-negative integers less than `size`.
If this is not true, an out of bounds memory access will occur resulting in
undefined behaviour (likely a crash).

*/
constexpr void array_order_mapping(const auto& in, auto& out, std::size_t size, const auto& order) noexcept
{
    for (std::size_t i = 0; i < size; ++i)
        out[i] = in[order[i]];
}
// @/
```

The only tricky thing about this is the interpretation of the `order` array,
which is hopefully sufficiently well explained by the reference documentation.

We choose to provide this core functionality as a generic function so that
clients who wish to use it directly (i.e. not wrapped in a component) may do
so. Arguably the argument types should be more carefully constrained, but for
now we assume that the client can be trusted not to abuse the function with
types that will obviously not work.

For convenience, we provide specialised overloads for some common expected
cases. The overload for `std::array`-like input (with `size()` member function)
should probably be constained more carefully.

```cpp
// @+'core implementation'

/// Overload for array-like types that have a size member function
template<typename T> requires requires (T t) { t.size(); }
constexpr void array_order_mapping(const T& in, auto& out, const auto& order) noexcept
{
    array_order_mapping(in, out, in.size(), order);
}

/// Overload for raw C-style arrays
template<typename T, std::size_t N>
constexpr void array_order_mapping(const T(& in)[N], auto& out, const auto& order) noexcept
{
    array_order_mapping(in, out, N, order);
}
// @/
```

```cpp
// @='core tests'
TEST_CASE("sygsp-array_order_mapping_function")
{
    SECTION("c array")
    {
        int in[5] = {0, 1, 2, 3, 4};
        int out[5] = {0};
        int order[5] = {1, 2, 4, 0, 3};
        array_order_mapping(in, out, order);
        CHECK(std::memcmp((const void *)out, (const void *)order, 5 * sizeof(int)) == 0);
    }

    SECTION("std::array")
    {
        std::array<int, 5> in = {0, 1, 2, 3, 4};
        std::array<int, 5> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping(in, out, order);
        CHECK(out == order);
    }

    SECTION("sygaldry::array")
    {
        array<"array1", 5, "for testing", int, 0, 4> in; in.value = {0, 1, 2, 3, 4};
        array<"array2", 5, "for testing", int, 0, 4> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping(in, out, order);
        CHECK(out.value == order);
    }
}
// @/
```

## Core Boilerplate

```cpp
// @#'function/sygsp-array_order_mapping_function.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

namespace sygaldry { namespace sensors { namespace portable {

/*! \addtogroup sygsp-array_order_mapping
*/
/// \{

/*! \defgroup sygsp-array_order_mapping_function Array Order Mapping Function
*/
/// \{

@{core implementation}

} } }

/// \}
/// \}
// @/
```

```cpp
// @#'function/sygsp-array_order_mapping_function.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include <array>
#include <cstring>
#include "sygah-endpoints.hpp"
#include "sygsp-array_order_mapping_function.hpp"

using namespace sygaldry;
using namespace sygaldry::sensors;
using namespace sygaldry::sensors::portable;
@{core tests}
// @/
```

# Component Wrapper

In addition to the above core function, we also wrap the mapping in a
component with additional boundary safety checks.

```cmake
# @#'function/CMakeLists.txt'
set(lib sygsp-array_order_mapping_function)
add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
target_link_libraries(sygsp-array_order_mapping INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
target_link_libraries(${lib}-test PRIVATE sygah-endpoints)
catch_discover_tests(${lib}-test)
endif()
# @//
```

```cpp
// @#'sygsp-array_order_mapping.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

namespace sygaldry { namespace sensors { namespace portable {

/*! \addtogroup sygsp
*/
/// \{

/*! \defgroup sygsp-array_order_mapping Array Order Mapping
*/
/// \{

/*! Map the template-parameterised input array to a re-ordered output.

\tparam array_in The type of the input array to be passed to `main()`.
\tparam order

A `std::array` of indices with which to re-order `array_in`.
See \ref sygsp-array_order_mapping_function for the
expected format of this array.

*/
template<typename array_in, std::array<std::size_t, array_in::size()> order, typename ... Tags>
struct ArrayOrderMapping
: name_<"Array Order Mapping">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct outputs_t {
        array< "reordered"
             , array_in::size()
             , array_in::description()
             , array_in::min()
             , array_in::max()
             , array_in::init()
             , Tags...
             > reordered;
    } outputs;

    /*! Perform the remapping with output to `reordered`.

    \param[in] in The array with type `array_in` that will be reordered by the mapping.
    
    \sa \ref sygsp-array_order_mapping_function.
    */
    void main()
    {
        array_order_mapping(
    }
}

/// \}
/// \}

} } }
// @/
```

```cpp
// @#'sygsp-array_order_mapping.cpp'
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
namespace sygaldry { namespace sensors { namespace portable {
} } }
// @/
```

```cpp
// @#'sygsp-array_order_mapping.test.cpp'
#include "sygsp-array_order_mapping.hpp"

// test suite setup

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'

set(lib sygsp-array_order_mapping)
add_library(${lib} STATIC)

add_subdirectory(function)

target_sources(${lib}
        PRIVATE ${lib}.cpp
        )
target_include_directories(${lib}
        PUBLIC .
        )
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        )

# build automation for the test suite goes here

# @/
```



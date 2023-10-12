\page page-sygac-metadata sygac-metadata: Text Metadata Reflection

[TOC]

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

An entity is considered to have a name if its type has a static method `name()`
that returns something that is convertible to some string representation. The
use of `std::decay` removes cv-ref qualifications on the type and seems to be
necessary so that its name can be accessed and recognized when the type is a
reference, although I'm not entirely sure why.

```cpp
template<typename T>
concept has_name = requires
{
    {std::decay_t<T>::name()} -> std::convertible_to<std::string>;
    {std::decay_t<T>::name()} -> std::convertible_to<std::string_view>;
    {std::decay_t<T>::name()} -> std::convertible_to<const char *>;
};
template<has_name T>
constexpr auto name_of(T&) { return T::name(); }

template<has_name T>
_consteval auto name_of() { return std::decay_t<T>::name(); }
```

In the same way that all textual metadata can be defined using the same
pattern, so too can it be detected and accessed. So just as we use a
macro in \ref page-sygah-metadata to generate metadata helpers, here we
use one to generate their generic accessors.

```cpp
// @#'sygac-metadata.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <concepts>
#include "sygah-consteval.hpp"

namespace sygaldry {

/*! \addtogroup sygac
 */
/// \{

/*! \defgroup sygac-metadata sygac-metadata: Text Metadata Reflection
 */
/// \{

#define text_concept(CONCEPT_NAME) template<typename T> \
concept has_##CONCEPT_NAME = requires \
{ \
    {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<std::string>; \
    {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<std::string_view>; \
    {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<const char *>; \
}; \
template<has_##CONCEPT_NAME T> \
constexpr auto CONCEPT_NAME##_of(const T&) { return T::CONCEPT_NAME(); } \
 \
template<has_##CONCEPT_NAME T> \
_consteval auto CONCEPT_NAME##_of() { return std::decay_t<T>::CONCEPT_NAME(); }

text_concept(name);
text_concept(author);
text_concept(email);
text_concept(license);
text_concept(description);
text_concept(uuid);
text_concept(unit);
text_concept(version);
text_concept(date);

#undef text_concept

/// \}
/// \}
}

// @/
```

```cpp
// @#'sygac-metadata.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include "sygac-metadata.hpp"

using namespace sygaldry;
using std::string_view;

struct base_struct_with_name {static constexpr auto name() {return "yup";}};
TEST_CASE("sygaldry name_of", "[components][concepts][name_of]")
{
    static_assert(has_name<base_struct_with_name>);
    base_struct_with_name yup{};
    SECTION("T")
    {
        REQUIRE(string_view(name_of(yup)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name>()) == string_view("yup"));
    }
    SECTION("T&")
    {
        auto& baz = yup;
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name&>()) == string_view("yup"));
    }
    SECTION("const T&")
    {
        const auto& baz = yup;
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<const base_struct_with_name&>()) == string_view("yup"));
    }
}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygac-metadata)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygah-consteval)
target_link_libraries(sygac INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
catch_discover_tests(${lib}-test)
endif()
set(lib sygac-metadata)
# @/
```

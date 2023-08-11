\page concepts_metadata Metadata Concepts

[TOC]

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

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
macro in `components/metadata.lili` to generate metadata helpers, here we
use one to generate their generic accessors.

```cpp
// @#'concepts/metadata.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <concepts>
#include "utilities/consteval.hpp"

namespace sygaldry {

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

}
// @/
```

```cpp
// @#'tests/metadata/tests.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <type_traits>
#include <boost/pfr.hpp>
#include "components/metadata.hpp"
#include "concepts/metadata.hpp"

using namespace sygaldry;
using std::string_view;

struct struct_with_name : name_<"foo"> {};
struct base_struct_with_name {static _consteval auto name() {return "yup";}};
TEST_CASE("name_of", "[components][concepts][name_of]")
{
    static_assert(has_name<base_struct_with_name>);
    static_assert(has_name<struct_with_name>);
    struct_with_name foo{};
    base_struct_with_name yup{};
    SECTION("T")
    {
        REQUIRE(string_view(name_of(foo)) == string_view("foo"));
        REQUIRE(string_view(name_of<struct_with_name>()) == string_view("foo"));
        REQUIRE(string_view(name_of(yup)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name>()) == string_view("yup"));
    }
    SECTION("T&")
    {
        auto& bar = foo;
        auto& baz = yup;
        REQUIRE(string_view(name_of(bar)) == string_view("foo"));
        REQUIRE(string_view(name_of<struct_with_name&>()) == string_view("foo"));
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name&>()) == string_view("yup"));
    }
    SECTION("const T&")
    {
        const auto& bar = foo;
        const auto& baz = yup;
        REQUIRE(string_view(name_of(bar)) == string_view("foo"));
        REQUIRE(string_view(name_of<const struct_with_name&>()) == string_view("foo"));
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<const base_struct_with_name&>()) == string_view("yup"));
    }
}
// @/
```

```cmake
# @#'tests/metadata/CMakeLists.txt'
add_compile_options(-fconcepts-diagnostics-depth=2)
add_executable(concepts-metadata-tests tests.cpp)
target_link_libraries(concepts-metadata-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(concepts-metadata-tests PRIVATE Sygaldry::Concepts)
catch_discover_tests(concepts-metadata-tests)
# @/
```

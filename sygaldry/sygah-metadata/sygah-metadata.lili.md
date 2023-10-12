\page page-sygah-metadata sygah-metadata: Metadata Helpers

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

The `string_literal` defined in \ref page-sygah-string_literal is combined with
simple classes useful for associating various kinds of expected textual
metadata with an endpoint or component, such as a unit, author, or description.
We employ a macro to define such classes, allowing us to easily add other
textual metadata in this way.

```cpp
// @#'sygah-metadata.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-string_literal.hpp"
#include "sygah-consteval.hpp"

namespace sygaldry {

/// \ingroup sygah
/// \{

/// \defgroup sygah-metadata sygah-metadata: Metadata Helpers
/// \{

/// Helper macro used in the definition of metadata structs; This get's undefined immediately so don't try to use it!
#define metadata_struct(NAME) template<string_literal str>\
struct NAME##_\
{\
    /*! Returns the NAME metadata string */\
    static _consteval auto NAME() {return str.value;}\
}

// TODO: refactor these so the base classes are capitalised instead of post-underscored?

/// Document the name of an entity, e.g. an endpoint, component, or binding
metadata_struct(name);
/// Document a textual description of an entity, e.g. an endpoint, component or binding
metadata_struct(description);
/// Document a textual description of the unit of measurement used by an entity, especially an endpoint
metadata_struct(unit);
/// Document the author of an entity, e.g. a component or binding
metadata_struct(author);
/// Document the designer of an entity, if different from the author, e.g. designer of an instrument vs author of the firmware
metadata_struct(designer);
/// Document the author's contact email of an entity, e.g. a component or binding
metadata_struct(email);
/// Document the copyright license of an entity, e.g. a component or binding
metadata_struct(license);
/// Document the copyright statement of an entity, e.g. a component or binding
metadata_struct(copyright);
/// Document an unambiguous unique identifier for the type of an entity e.g. a component or binding
metadata_struct(uuid);
/// Document a textual description of the version number of an entity, e.g. a component or binding
metadata_struct(version);
/// Document a textual description of the date when an entity was released, e.g. a component or binding
metadata_struct(date);

#undef metadata_struct

/// \}
/// \}
}
// @/
```

The main limitation with this approach is that the name of the base class
cannot be the same as the name of the method that it provides. It turns out
this limitation is not particularly more severe than those of any other
approach. For example, the strategy taken by Avendish is to provide a macro,
e.g. `halp_meta(author, "Jane Doe")` that expands to something like `static
_constexpr auto author() {return "Jane Doe";}`. In this case the user is
obliged to write out `halp_meta` for every point of metadata, which is no worse
than having to add an underscore after the the name of the metadata. The main
disadvantage of this approach is that it uses a macro, with the attendant
(though small) risks involved, whereas the approach shown above protects the
metadata within a namespace. Another possible disadvantage is that the name of
the metadata, e.g. "author" is not checked by the compiler, meaning one could
easily write "authr" by mistake and run into frustrating issues as a
consequence. This is also the macro method's main advantage: there's no need
for the helper library author to specify in advance all of the possible
metadata types. It seems likely both methods have similar if not identical
runtime performance. For now, we opt for the base-class approach in keeping
with our overall strategy.

```cpp
// @#'sygah-metadata.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include "sygah-metadata.hpp"

using namespace sygaldry;

using std::string_view;

struct struct_with_name : name_<"foo"> {};
TEST_CASE("sygaldry name_", "[endpoints][bases][name_]")
{
    REQUIRE(string_view(struct_with_name::name()) == string_view("foo"));
}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygah-metadata)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygah-string_literal)
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

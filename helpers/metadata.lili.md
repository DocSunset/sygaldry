# Metadata Helpers

[TOC]

All components and endpoints are required to have a name. To facilitate setting
this conveniently, one approach is to define a thin wrapper around a string
literal that allows us to pass string literals as template parameters. Then we
define a base class that takes a string template parameter and synthesizes a
compile-time evaluated name method.

The above `string_literal` combined with a simple class may also be useful
for associating other kinds of textual metadata with an endpoint or component,
such as a unit, author, or description. We employ a macro to define
such classes, allowing us to easily add other textual metadata in this way.


```cpp
// @#'metadata.hpp'
#pragma once

#include <cstddef>
#include "utilities/consteval.hpp"

namespace sygaldry {

template<std::size_t N>
struct string_literal
{
    char value[N];
    _consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

#define metadata_struct(NAME) template<string_literal str>\
struct NAME##_\
{\
    static _consteval auto NAME() {return str.value;}\
}

metadata_struct(name);
metadata_struct(author);
metadata_struct(email);
metadata_struct(license);
metadata_struct(copyright);
metadata_struct(description);
metadata_struct(uuid);
metadata_struct(unit);
metadata_struct(version);
metadata_struct(date);

#undef metadata_struct

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
// @#'tests/metadata/tests.cpp'
#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <type_traits>
#include <optional>
#include <boost/pfr.hpp>
#include "helpers/metadata.hpp"

using namespace sygaldry;

using std::string_view;

template<string_literal str>
_consteval auto name() {return string_view{str.value};}

TEST_CASE("String literal", "[endpoints][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}

struct struct_with_name : name_<"foo"> {};
TEST_CASE("name_", "[endpoints][bases][name_]")
{
    REQUIRE(string_view(struct_with_name::name()) == string_view("foo"));
}
// @/
```

```cmake
# @#'tests/metadata/CMakeLists.txt'
add_executable(metadata-tests tests.cpp)
target_link_libraries(metadata-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(metadata-tests)
# @/
```
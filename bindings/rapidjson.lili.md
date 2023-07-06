# RapidJSON Binding

This binding stores the value of session data endpoints in JSON format using
the RapidJSON library. A platform-specific plugin parameter is used to store
and retrieve this data across sessions, allowing session data to be restored
when loading a new session, such as after rebooting an embedded device.

[TOC]

TODO: license text here

# Overview

The functionality of this session storage component is provided by its
initialization and main subroutines. The main subroutine tracks changes to
session data endpoints and saves updates by serializing JSON data into
persistent storage using the platform-specific plugin parameter part. The
initialization subroutine retrieves and deserializes the JSON data and restores
the last recorded state of the session data. This component should be
initialized before any components with session data, so that the restored state
can be used by those components during their initialization subroutines.

# Implementation

## Test Component

We define a component with some session data for testing.

```cpp
// @+'tests'
struct test_component_t
: name_<"Test">
{
    struct inputs_t {
        text_message<"text", "description goes here", tag_session_data> some_text;
        slider<"slider", "description goes here", float, 0.0, tag_session_data> a_slider;
    } inputs;

    void main() {}
} test_component;
// @/
```

We will use the input and output string streams provided by RapidJSON for
testing, so we define a type alias to avoid having to repeat this in each
test case.

```cpp
// @+'tests'
using TestStorage = RapidJsonSessionStorage<rapidjson::StringStream, rapidjson::StringBuffer, decltype(test_component)>
// @/
```

## Accessing JSON Member Values

When initializing, persistent data endpoints must be set to the state contained
in the JSON object's member fields. When updating in the main subroutine, the
value of persistent endpoints must be compared with the value of the JSON
object's member fields. The following subroutine abstracts access to the JSON
object's members, accepting a lambda that is used to perform the necessary
specific functionality.

```cpp
// @='json member value'
template<typename T>
static void apply_with_json_member_value(auto& json, auto&& f)
{
    if (not json.HasMember(osc_path_v<T, Components>)) return;
    auto& m = json[osc_path_v<T, Components>];
    if constexpr (has_value<T>)
    {
        if constexpr (std::integral<value_t<T>>)
        {
            if constexpr (std::is_signed_v<value_t<T>>)
            {
                     if (m.isInt())   f(m.getInt());
                else if (m.isInt64()) f(m.getInt64());
            } else
            {
                     if (m.isUint())   f(m.getUint());
                else if (m.isUint64()) f(m.getUint64());
            }
        } else if constexpr (std::floating_point<value_t<T>>)
        {
            if (m.isDouble()) f(static_cast<value_t<T>>(m.getDouble()));
        } else if constexpr (string_like<value_t<T>>)
        {
            if (m.isString()) f(m.getString());
        }
    }
}
// @/
```

## Init

A template-parameter input stream is instantiated at the beginning of the
initialization subroutine and used to load JSON data from storage.

```cpp
// @+'init'
json.ParseStream(istream);
// @/
```

In case there was no data stored, e.g. if this is the first time ever
booting the device or its flash memory was recently erased, then we
set the json document to an empty object and return; there's nothing
else to do.

```cpp
// @+'init'
if (not json.IsObject())
{
    json.SetObject();
    return;
}
// @/

// @+'tests'
TEST_CASE("RapidJSON creates object given empty input stream")
{
    string ibuffer{""};
    rapidjson::StringStream istream{ibuffer.c_str()};
    TestStorage storage{};
    storage.init(istream, test_component);
    CHECK(storage.json.IsObject());
    REQUIRE(storage.json.ObjectEmpty());
}
// @/
```

Otherwise, we iterate over each session data endpoint and attempt to
set the value of the endpoint based on that stored in the JSON document.
As seen above, this will silently fail if the stored value does not
exist (e.g. if the session data endpoint was just added), or is the
wrong type (e.g. if the session data endpoint was just edited). It
remains as future work to attempt to coerce a stored value to the
correct type in the latter case.

```cpp
// @+'init'
for_each_session_datum(components, [&]<typename T>(T& endpoint)
{
    apply_with_json_member_value<T>(json, [&](auto value)
    {
        set_value(endpoint, value);
    });

});
// @/
```

## Main

The structure of the main subroutine actually mirrors that of the
initialization subroutine, with one branch for the case where the
JSON document doesn't have any existing data for an endpoint, and
one where it does.

```cpp
// @+'main'
bool updated = false;
for_each_session_datum(components, [&]<typename T>(T& endpoint)
{
    if constexpr (has_value<T>)
    {
        if (not json.HasMember(osc_path_v<T, Components>))
        {
            @{main not HasMember branch}
        }
        else
        {
            @{main HasMember branch}
        }
    }
});
// @/
```

In case the document doesn't already have a member for a given endpoint, one is
added appropriately depending on the value type of that endpoint. String-like
data in particular requires special handling, since RapidJSON needs to copy this
data on the heap. This branch always results in a change to the JSON document,
which is signaled via the `updated` boolean flag.

```cpp
// @='main not HasMember branch'
if constexpr (string_like<value_t<T>>)
{
    rapidjson::Value v{value_of(endpoint), json.GetAllocator()};
    json.AddMember(osc_path_v<T, Components>, v, json.GetAllocator());
}
else
{
    json.AddMember(osc_path_v<T, Components>, value_of(endpoint), json.GetAllocator());
}
updated = true;
// @/
```

If a member already exists, then we check if its value has changed. For
`OccasionalValue` types, this is a simple matter of checking the boolean
interpretation of the endpoint. Otherwise, the current value of the JSON
document member is compared with the value of the endpoint.

```cpp
// @='main HasMember branch'
bool endpoint_updated = false;
if constexpr (OccasionalValue<T>)
    endpoint_updated = bool(endpoint);
else apply_with_json_member_value<T>(json, [&](auto value)
{
    endpoint_updated = value != value_of(endpoint);
});
// @/
```

In case the value has changed, we update it appropriately depending on its type,
once again handling strings seperately from other (numerical) data.

Special handling for array-type data is left as future work.

```cpp
// @+'main HasMember branch'
if (endpoint_updated)
{
    if constexpr (string_like<value_t<T>>)
        json[osc_path_v<T, Components>].setString(value_of(endpoint), json.GetAllocator());
    else json[osc_path_v<T, Components>] = value_of(endpoint);
    updated = true;
}
// @/
```

If either of the above branches results in a change to the document on any
endpoint, then the document is sent to the template-parameter output stream for
long-term storage.

```cpp
// @+'main'
if (updated)
{
    OStream ostream{};
    json.Accept(ostream);
}
// @/
```

# Summary

```cpp
// @#'rapidjson.hpp'
#pragma once

#include <rapidjson/document.h>
#include "concepts/endpoints.hpp"
#include "concepts/components.hpp"
#include "helpers/metadata.hpp"
#include "bindings/osc_string_constants.hpp"

namespace sygaldry { namespace bindings {

template<typename IStream, typename OStream, typename Components>
struct RapidJsonSessionStorage
: name_<"RapidJSON Session Storage">
// TODO: other metadata
{
    @{ostream}
    rapidjson::Document json{};

    @{json member value}

    void init(IStream& istream, Components& components)
    {
        @{init}
    }

    void main(OStream& ostream, Components& components)
    {
        @{main}
    }
};

} }
// @/

// @#'tests/rapidjson/tests.cpp'
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <rapidjson/stream.h>
#include <rapidjson/stringbuffer.h>
#include "helpers/endpoints.hpp"
#include "bindings/rapidjson.hpp"

using std::string;
using namespace sygaldry;
using namespace sygaldry::bindings;

@{tests}
// @/
```

```cmake
# @#'tests/rapidjson/CMakeLists.txt'
add_executable(rapidjson-tests tests.cpp)
target_link_libraries(rapidjson-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(rapidjson-tests)
# @/
```

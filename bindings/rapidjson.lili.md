\page bind_rapidjson RapidJSON Binding

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This binding stores the value of session data endpoints in JSON format using
the RapidJSON library. A platform-specific plugin parameter is used to store
and retrieve this data across sessions, allowing session data to be restored
when loading a new session, such as after rebooting an embedded device.

[TOC]

# Overview

The functionality of this session storage component is provided by its
initialization and main subroutines. The main subroutine tracks changes to
session data endpoints and saves updates by serializing JSON data into
persistent storage using the platform-specific plugin parameter part. The
initialization subroutine retrieves and deserializes the JSON data and restores
the last recorded state of the session data. This component should be
initialized before any components with session data, so that the restored state
can be used by those components during their initialization subroutines.

This component is intended to be used as a part in a platform-specific storage
component that provides RapidJSON compatible `IStream` and `OStream`
parameters, and manages their initialization and lifetimes. In particular, the
`IStream` should be initialized before calling this component's `init`
subroutine, and can be cleaned up immediately afterwards. The `OStream` may be
instantiated before each call to main, or kept around persistently, whichever
is better suited for the platform.

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
        slider<"slider", "description goes here", float, 0.0f, 1.0f, 0.0f, tag_session_data> my_slider;
        array<"array", 3, "description goes here", float, 0.0f, 1.0f, 0.0f, tag_session_data> my_array;
    } inputs;

    void main() {}
} test_component;
// @/
```

We define a test output stream:

```cpp
// @+'tests'
struct OStream{
    inline static rapidjson::StringBuffer obuffer{};
    rapidjson::Writer<rapidjson::StringBuffer> writer;
    OStream() : writer{obuffer} {}
};
// @/
```

This kind of wrapper is required so that side-effects and resources required to
instantiate an output stream can be only invoked when necessary.

We will use the input and output string streams provided by RapidJSON for
testing, so we define a type alias to avoid having to repeat this in each
test case.

```cpp
// @+'tests'
using TestStorage = RapidJsonSessionStorage<rapidjson::StringStream, OStream, decltype(test_component)>;
// @/
```

## Accessing JSON Member Values

When initializing, persistent data endpoints must be set to the state contained
in the JSON object's member fields. When updating in the external destinations
subroutine, the value of persistent endpoints must be compared with the value
of the JSON object's member fields. The following subroutine abstracts access
to the JSON object's members, accepting a lambda that is used to perform the
necessary specific functionality, so that the type safety checks needn't be
repeated every time the JSON member data is accessed.

```cpp
// @='json member value'
/// apply the functor `f` to the value of each member of the JSON object `json` extracted depending on the type of endpoint `T`.
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
                     if (m.IsInt())   f(m.GetInt());
                else if (m.IsInt64()) f(m.GetInt64());
            } else
            {
                     if (m.IsUint())   f(m.GetUint());
                else if (m.IsUint64()) f(m.GetUint64());
            }
        } else if constexpr (std::floating_point<value_t<T>>)
        {
            if (m.IsDouble()) f(static_cast<value_t<T>>(m.GetDouble()));
        } else if constexpr (string_like<value_t<T>>)
        {
            if (m.IsString()) f(m.GetString());
        } else if constexpr (array_like<value_t<T>>)
        {
            if (!m.IsArray() || m.Empty() || m.Size() != size<value_t<T>>()) return;
            if constexpr (std::integral<element_t<T>>)
            {
                if (m[0].IsInt())
                    f(m, [](auto& arr, auto idx) { return arr[idx].GetInt(); });
                else if (m[0].IsInt64())
                    f(m, [](auto& arr, auto idx) { return arr[idx].GetInt64(); });
            } else if constexpr (std::floating_point<element_t<T>>)
            {
                if (m[0].IsDouble())
                    f(m, [](auto& arr, auto idx) { return arr[idx].GetDouble(); });
            } else if constexpr (string_like<element_t<T>>)
            {
                if (m[0].IsString())
                    f(m, [](auto& arr, auto idx) { return arr[idx].GetString(); });
            }
        }
    }
}
// @/
```

## Init

A template-parameter input stream is used at the beginning of the
initialization subroutine to load JSON data from storage.

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
    if constexpr (array_like<value_t<T>>)
        apply_with_json_member_value<T>(json, [&](auto& arr, auto&& get)
    {
        for (std::size_t i = 0; i < size<value_t<T>>(); ++i)
            value_of(endpoint)[i] = get(arr, i);
    });
    else apply_with_json_member_value<T>(json, [&](auto value)
    {
        set_value(endpoint, value);
    });

});
// @/

// @+'tests'
TEST_CASE("RapidJSON sets endpoints based on input stream")
{
    string ibuffer{
R"JSON(
{ "/Test/text" : "hello world"
, "/Test/slider" : 42.0
, "/Test/array" : [1.0,2.0,3.0]
})JSON"};
    rapidjson::StringStream istream{ibuffer.c_str()};
    TestStorage storage{};
    test_component_t tc{};
    storage.init(istream, tc);
    CHECK(tc.inputs.some_text.value() == string("hello world"));
    CHECK(tc.inputs.my_slider.value == 42.0f);
    CHECK(tc.inputs.my_array.value == std::array{1.0f,2.0f,3.0f});
}
// @/
```

## External Destinations

The structure of the external destinations subroutine actually mirrors that of
the initialization subroutine, with one branch for the case where the JSON
document doesn't have any existing data for an endpoint, and one where it does.

```cpp
// @+'external_destinations'
bool updated = false;
for_each_session_datum(components, [&]<typename T>(T& endpoint)
{
    if constexpr (has_value<T>)
    {
        if (not json.HasMember(osc_path_v<T, Components>))
        {
            @{external_destinations not HasMember branch}
        }
        else
        {
            @{external_destinations HasMember branch}
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
// @='external_destinations not HasMember branch'
if constexpr (string_like<value_t<T>>)
{
    rapidjson::Value v{value_of(endpoint).c_str(), json.GetAllocator()};
    json.AddMember(rapidjson::GenericStringRef{osc_path_v<T, Components>}, v, json.GetAllocator());
}
else if constexpr (array_like<value_t<T>>)
{
    rapidjson::Value v{rapidjson::kArrayType};
    v.Reserve(3, json.GetAllocator());
    for (auto& element : value_of(endpoint)) v.PushBack(rapidjson::Value{element}, json.GetAllocator());
    json.AddMember(rapidjson::GenericStringRef{osc_path_v<T, Components>}, v, json.GetAllocator());
}
else
{
    json.AddMember(rapidjson::GenericStringRef{osc_path_v<T, Components>}, value_of(endpoint), json.GetAllocator());
}
updated = true;
// @/

// @='external_destinations not HasMember test'
tc.inputs.some_text = string("foo");
tc.inputs.my_slider.value = 888;
tc.inputs.my_array.value = std::array{1.0f,2.0f,3.0f};
storage.external_destinations(tc);

CHECK(storage.json.HasMember("/Test/text"));
CHECK(storage.json["/Test/text"].IsString());
CHECK(string("foo") == string(storage.json["/Test/text"].GetString()));

CHECK(storage.json.HasMember("/Test/slider"));
CHECK(storage.json["/Test/slider"].IsDouble());
CHECK(888.0 == storage.json["/Test/slider"].GetDouble());

CHECK(storage.json["/Test/array"].IsArray());
CHECK(storage.json["/Test/array"].Size() == 3);
CHECK(storage.json["/Test/array"][0].GetDouble() == 1.0f);
CHECK(storage.json["/Test/array"][1].GetDouble() == 2.0f);
CHECK(storage.json["/Test/array"][2].GetDouble() == 3.0f);

CHECK(string(R"JSON({"/Test/text":"foo","/Test/slider":888.0,"/Test/array":[1.0,2.0,3.0]})JSON") == string(OStream::obuffer.GetString()));
// @/
```

If a member already exists, then we check if its value has changed. For
`OccasionalValue` types, this is a simple matter of checking the boolean
interpretation of the endpoint. Otherwise, the current value of the JSON
document member is compared with the value of the endpoint.

```cpp
// @='external_destinations HasMember branch'
bool endpoint_updated = false;
if constexpr (OccasionalValue<T>)
    endpoint_updated = bool(endpoint);
else if constexpr (array_like<value_t<T>>)
    apply_with_json_member_value<T>(json, [&](auto& arr, auto&& get)
{
    for (std::size_t i = 0; i < size<value_t<T>>(); ++i)
        endpoint_updated = endpoint_updated || (value_of(endpoint)[i] != get(arr, i));
});
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
// @+'external_destinations HasMember branch'
if (endpoint_updated)
{
    if constexpr (string_like<value_t<T>>)
        json[osc_path_v<T, Components>].SetString(value_of(endpoint).c_str(), json.GetAllocator());
    else if constexpr (array_like<value_t<T>>)
    {
        auto& arr = json[osc_path_v<T, Components>];
        for (std::size_t i = 0; i < size<value_t<T>>(); ++i)
        {
            arr[i] = value_of(endpoint)[i];
        }
    }
    else json[osc_path_v<T, Components>] = value_of(endpoint);
    updated = true;
}
// @/

// @+'external_destinations HasMember test'
// following setting the previous values...
tc.inputs.some_text = string("bar");
tc.inputs.my_slider.value = 777;
tc.inputs.my_array.value = std::array{11.0f,22.0f,33.0f};
storage.external_destinations(tc);
CHECK(string("bar") == string(storage.json["/Test/text"].GetString()));
CHECK(777.0 == storage.json["/Test/slider"].GetDouble());
CHECK(string(R"JSON({"/Test/text":"bar","/Test/slider":777.0,"/Test/array":[11.0,22.0,33.0]})JSON") == string(OStream::obuffer.GetString()));
// @/
```

If either of the above branches results in a change to the document on any
endpoint, then the document is sent to the template-parameter output stream for
long-term storage.

```cpp
// @+'external_destinations'
if (updated)
{
    OStream ostream{};
    json.Accept(ostream.writer);
}
// @/

// @+'tests'
TEST_CASE("RapidJSON external_destinations")
{
    string ibuffer{""};
    rapidjson::StringStream istream{ibuffer.c_str()};
    OStream::obuffer.Clear();
    TestStorage storage{};
    test_component_t tc{};
    storage.init(istream, tc);

    @{external_destinations not HasMember test}

    OStream::obuffer.Clear();

    @{external_destinations HasMember test}
}
// @/
```

# Summary

```cpp
// @#'bindings/rapidjson.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/


#include <rapidjson/document.h>
#include "sygaldry-concepts-endpoints.hpp"
#include "sygaldry-concepts-components.hpp"
#include "helpers/metadata.hpp"
#include "bindings/osc_string_constants.hpp"
#include "bindings/session_data.hpp"

namespace sygaldry { namespace bindings {

template<typename IStream, typename OStream, typename Components>
struct RapidJsonSessionStorage
{
    rapidjson::Document json{};

    @{json member value}

    void init(IStream& istream, Components& components)
    {
        @{init}
    }

    void external_destinations(Components& components)
    {
        @{external_destinations}
    }
};

} }
// @/

// @#'tests/rapidjson/tests.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <string>
#include <catch2/catch_test_macros.hpp>
#include <rapidjson/stream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
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
target_link_libraries(rapidjson-tests PRIVATE Sygaldry::Bindings)
target_include_directories(rapidjson-tests PRIVATE ${PROJECT_SOURCE_DIR}/dependencies/rapidjson/include)
catch_discover_tests(rapidjson-tests)
# @/
```

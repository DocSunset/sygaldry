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
    auto& m = json[osc_path_v<T>];
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

First we parse the JSON string loaded from platform-specific storage.

```cpp
// @+'init'
json.ParseStream(istream);
if (not json.IsObject())
{
    json.setObject();
    return;
}
for_each_session_datum(components, [&]<typename T>(T& endpoint)
{
    if (not json.HasMember(osc_path_v<T>)) return;
    apply_with_json_member_value<T>(json, [&](auto value)
    {
        set_value(endpoint, value);
    });

});
// @/
```

## Main

```cpp
// @+'main'
bool updated = false;
for_each_session_datum(components, [&]<typename T>(T& endpoint)
{
    if constexpr (has_value<T>)
    {
        if (json.HasMember(osc_path_v<T>))
        {
            bool endpoint_updated = false;
            if constexpr (Occasional<T>)
                endpoint_updated = bool(endpoint);
            else apply_with_json_member_value<T>(json, [&](auto value)
            {
                endpoint_updated = value != value_of(endpoint);
            });

            if (endpoint_updated)
            {
                if constexpr (string_like<value_t<T>>)
                    json[osc_path_v<T>].setString(value_of(endpoint), json.GetAllocator());
                else json[osc_path_v<T>] = value_of(endpoint);
                updated = true;
            }
        } else
        {
            if constexpr (string_like<value_t<T>>)
            {
                rapidjson::Value v{value_of(endpoint), json.GetAllocator()};
                json.AddMember(osc_path_v<T>, v, json.GetAllocator());
            } else
            {
                json.AddMember(osc_path_v<T>, value_of(endpoint), json.GetAllocator());
            }
            updated = true;
        }
    }
});

if (updated) json.Accept(ostream);

// @/
```

# Summary

```cpp
// @#'rapid_json.hpp'
#pragma once

#include <rapidjson/document.h>
#include "concepts/components.hpp"

namespace sygaldry { namespace bindings {

template<typename IStream, typename OStream, typename Components>
struct RapidJsonSessionStorage
: name_<"RapidJSON Session Storage">
// TODO: other metadata
{
    [[no_unique_address]] OStream ostream{};
    rapidjson::Document json{};

    @{json member value}

    void init(Components& components)
    {
        IStream istream{};

        @{init}
    }

    void main(Components& components)
    {
        @{init}
    }
};

} }
// @/
```

# Session Data

It's common that some configurable parameters of a DMI system should be
persistent in the long term, e.g. across boot cycles of the processor. We term
these parameters *session data*. A component that manages storing and reloading
session data is termed a *session manager*. Currently, a
[JSON session manager](bindings/rapidjson.lili.md) is provided. Other formats
and storage mechanisms are easy to imagine, and may be provided in future work.

TODO: license text

[TOC]

This document describes the implementation of fundamental functionality shared
by all session managers.

# Representation

Session data is represented by endpoints marked with the tag `session_data`.
Using [the endpoint helpers library](helpers/endpoints.lili.md), an endpoint
can be marked as session data by passing the `tag_session_data` class as part
of the endpoint's variadic template type parameter pack (if using one of the
built in endpoint templates), or by inheriting from `tag_session_data` (if
defining a custom endpoint by assembling the helper base classes), or by
defining an unscoped enumeration with a member called `session_data`. The
presence of the session data endpoint can be queried at compile time using the
concept `tagged_session_data` provided by
[the endpoint concepts library](concepts/endpoints.lili.md).

# For Each Session Datum

All session managers require a means to iterate over all session data endpoint.
This is accomplished using the `for_each_endpoint` function provided by
[the component concepts library](concepts/components.lili.md).

```cpp
// @#'session_data.hpp'
#pragma once
#include "concepts/components.hpp"
#include "concepts/endpoints.hpp"

namespace sygaldry { namespace bindings {

template<typename C>
constexpr void for_each_session_datum(C& components, auto fn)
{
    for_each_endpoint(components, [&]<typename T>(T& endpoint)
    {
        if constexpr (tagged_session_data<T>) fn(endpoint);
    });
}

} }
// @/
```

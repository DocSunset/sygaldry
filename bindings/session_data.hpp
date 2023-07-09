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

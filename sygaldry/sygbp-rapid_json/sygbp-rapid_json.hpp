#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/


#include <rapidjson/document.h>
#include "sygac-endpoints.hpp"
#include "sygac-components.hpp"
#include "sygah-metadata.hpp"
#include "sygbp-osc_string_constants.hpp"
#include "sygbp-session_data.hpp"

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp
///\{
///\defgroup sygbp-rapid_json sygbp-rapid_json: RapidJSON Binding
///\{

template<typename IStream, typename OStream, typename Components>
struct RapidJsonSessionStorage
{
    rapidjson::Document json{};

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

    void init(IStream& istream, Components& components)
    {
        json.ParseStream(istream);
        if (not json.IsObject())
        {
            json.SetObject();
            return;
        }
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
    }

    void external_destinations(Components& components)
    {
        bool updated = false;
        for_each_session_datum(components, [&]<typename T>(T& endpoint)
        {
            if constexpr (has_value<T>)
            {
                if (not json.HasMember(osc_path_v<T, Components>))
                {
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
                }
                else
                {
                    bool endpoint_updated = false;
                    if constexpr (OccasionalValue<T>)
                        endpoint_updated = flag_state_of(endpoint);
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
                }
            }
        });
        if (updated)
        {
            OStream ostream{};
            json.Accept(ostream.writer);
        }
    }
};

///\}
///\}
} }

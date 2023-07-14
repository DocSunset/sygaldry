#pragma once

#include <rapidjson/document.h>
#include "concepts/endpoints.hpp"
#include "concepts/components.hpp"
#include "helpers/metadata.hpp"
#include "bindings/osc_string_constants.hpp"
#include "bindings/session_data.hpp"

namespace sygaldry { namespace bindings {

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
            apply_with_json_member_value<T>(json, [&](auto value)
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
                        endpoint_updated = bool(endpoint);
                    else apply_with_json_member_value<T>(json, [&](auto value)
                    {
                        endpoint_updated = value != value_of(endpoint);
                    });
                    if (endpoint_updated)
                    {
                        if constexpr (string_like<value_t<T>>)
                            json[osc_path_v<T, Components>].SetString(value_of(endpoint).c_str(), json.GetAllocator());
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

} }

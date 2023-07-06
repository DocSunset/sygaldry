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
    rapidjson::Document json{};

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

    void main(OStream& ostream, Components& components)
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
                        rapidjson::Value v{value_of(endpoint), json.GetAllocator()};
                        json.AddMember(osc_path_v<T, Components>, v, json.GetAllocator());
                    }
                    else
                    {
                        json.AddMember(osc_path_v<T, Components>, value_of(endpoint), json.GetAllocator());
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
                            json[osc_path_v<T, Components>].setString(value_of(endpoint), json.GetAllocator());
                        else json[osc_path_v<T, Components>] = value_of(endpoint);
                        updated = true;
                    }
                }
            }
        });
        if (updated)
        {
            OStream ostream{};
            json.Accept(ostream);
        }
    }
};

} }

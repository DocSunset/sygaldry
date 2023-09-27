#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <boost/pfr.hpp>
#include "sygah-consteval.hpp"
#include "sygac-components.hpp"
#include "sygac-metadata.hpp"
#include "sygac-endpoints.hpp"
#include "sygbp-osc_string_constants.hpp"
#include "sygbp-osc_match_pattern.hpp"

namespace sygaldry { namespace sygbp {

struct Describe
{
    static _consteval auto name() { return "/describe"; }
    static _consteval auto usage() { return "osc-address-pattern"; }
    static _consteval auto description() { return "Convey metadata about entities that match the given address pattern"; }

    template<typename T>
    void describe_entity_type(auto& log, T& entity)
    {
        if constexpr (Bang<T>) log.println("bang");
        else if constexpr (has_value<T>)
        {
            if constexpr (OccasionalValue<T>)
            {
                if constexpr (array_like<value_t<T>>)
                    log.print("array of ");
                else log.print("occasional ");
            }
            else if constexpr (PersistentValue<T>)
            {
                if constexpr (array_like<value_t<T>>)
                    log.print("array of ");
                else log.print("persistent ");
            }
            if constexpr (std::integral<element_t<T>>)
                log.println("int");
            else if constexpr (std::floating_point<element_t<T>>)
                log.println("float");
            else if constexpr (string_like<element_t<T>>)
                log.println("text");
            else log.println("unknown value type");
        }
        else if constexpr (Component<T>) log.println("component");
        else log.println("unknown");
    }

    template<typename T>
    void describe_entity_value(auto& log, T& entity)
    {
        if constexpr (Bang<T>)
        {
            if (flag_state_of(entity)) log.println("(! bang !)");
            else log.println("()");
        }
        else if constexpr (OccasionalValue<T>)
        {
            if (flag_state_of(entity)) log.println("(! ", value_of(entity), " !)");
            else log.println("(", value_of(entity), ")");
        }
        else if constexpr (PersistentValue<T>)
        {
            if constexpr (tagged_write_only<T>) log.println("WRITE ONLY");
            else if constexpr (string_like<value_t<T>>)
                log.println("\"", value_of(entity), "\"");
            else log.println(value_of(entity));
        }
    }

    template<typename T, typename Components>
    void describe_entity(auto& log, auto preface, T& entity, auto ... indents)
    {
        static_assert(has_name<T>);
        log.println(indents..., preface, (const char *)osc_path_v<T, Components>);
        log.println(indents..., "  name: \"", entity.name(), "\"");
        log.print(indents...,   "  type:  ");
        describe_entity_type(log, entity);
        if constexpr (has_range<T>)
        {
            log.print(indents..., "  range: ");
            auto range = get_range<T>();
            log.println(range.min, " to ", range.max, " (init: ", range.init, ")");
        }
        if constexpr (has_value<T>)
        {
            log.print(indents...,   "  value: ");
            describe_entity_value(log, entity);
        }
        if constexpr (Component<T>)
        {
            auto describe_group = [&](auto& group, auto groupname)
            {
                boost::pfr::for_each_field(group, [&]<typename Y>(Y& endpoint)
                {
                    describe_entity<Y, Components>(log, groupname, endpoint, "  ", indents...);
                });
            };
            if constexpr (has_inputs<T>) describe_group(inputs_of(entity),  "input:   ");
            if constexpr (has_outputs<T>) describe_group(outputs_of(entity), "output:  ");
        }
    }

    template<typename Components>
    int main(int argc, char** argv, auto& log, Components& components)
    {
        if (argc < 2) return 2;
        for_each_node(components, [&]<typename T>(T& node, auto)
        {
            if constexpr (has_name<T>)
                if (osc_match_pattern(argv[1], osc_path_v<T, Components>))
                    describe_entity<T, Components>(log, "entity: ", node);
        });
        return 0;
    };

};

} } // namespaces

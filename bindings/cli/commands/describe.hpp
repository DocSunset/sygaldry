#pragma once

#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"
#include "bindings/spelling.hpp"
#include "concepts/metadata.hpp"
#include "concepts/endpoints.hpp"
#include "bindings/name_dispatch.hpp"

namespace sygaldry { namespace bindings { namespace clicommands {

struct Describe
{
    static _consteval auto name() { return "describe"; }
    static _consteval auto usage() { return "component-name [endpoint-name]"; }
    static _consteval auto description() { return "Convey metadata about a component or its endpoint. Pass * to describe all"; }

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
            if (entity) log.println("(! bang !)");
            else log.println("()");
        }
        else if constexpr (OccasionalValue<T>)
        {
            if (entity) log.println("(! ", value_of(entity), " !)");
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

    template<typename T>
    void describe_entity(auto& log, auto preface, T& entity, auto ... indents)
    {
        static_assert(has_name<T>);
        log.println(indents..., preface, (const char *)lower_kebab_case(entity));
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
                boost::pfr::for_each_field(group, [&](auto& entity)
                {
                    describe_entity(log, groupname, entity, "  ", indents...);
                });
            };
            if constexpr (has_inputs<T>) describe_group(inputs_of(entity),  "input:   ");
            if constexpr (has_outputs<T>) describe_group(outputs_of(entity), "output:  ");
        }
    }

    int main(int argc, char** argv, auto& log, auto& components)
    {
        if (argc < 2) return 2;
        bool describe_component = argc == 2;
        bool describe_endpoint = argc > 2;
        return dispatch<CommandMatcher>(argv[1], components, 2, [&](auto& component)
        {
            if (describe_component)
            {
                describe_entity(log, "component: ", component);
                return 0;
            }
            else if (describe_endpoint) return dispatch<CommandMatcher>(argv[2], component, 2, [&](auto& endpoint)
            {
                describe_entity(log, "endpoint: ", endpoint);
                return 0;
            });
            else return 0;
        });
    };

};

} } } // namespaces

#pragma once

#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"
#include "components/concepts.hpp"
#include "bindings/name_dispatch.hpp"

namespace sygaldry { namespace bindings { namespace cli { namespace commands {

using namespace sygaldry::concepts;

template<typename Config>
struct Describe
{
    static _consteval auto name() { return "describe"; }
    static _consteval auto usage() { return "component-name [endpoint-name]"; }
    static _consteval auto description() { return "Convey metadata about a component or its endpoint. Pass * to describe all"; }

    [[no_unique_address]] typename Config::basic_logger_type log;


    template<typename T>
    void describe_entity_type(T& entity)
    {
        if constexpr (Component<T>) log.println("component");
        else if constexpr (Bang<T>) log.println("bang");
        else if constexpr (PersistentValue<T>) log.println("persistent value");
        else if constexpr (OccasionalValue<T>) log.println("occasional value");
        else log.println("unknown");
    }

    template<typename T>
    void describe_entity_value(T& entity)
    {
        if constexpr (Bang<T>)
        {
            if (entity) log.println("(bang!)");
            else log.println("()");
        }
        else if constexpr (PersistentValue<decltype(entity)>) log.println(value_of(entity));
        else if (entity) log.println("(", value_of(entity), ")");
        else log.println("()");
    }

    template<typename T>
    void describe_entity(auto preface, T& entity, auto ... indents)
    {
        using spelling::lower_kebab_case;
        static_assert(has_name<T>);
        log.println(indents..., preface, (const char *)lower_kebab_case(entity));
        log.println(indents..., "  name: \"", entity.name(), "\"");
        log.print(indents...,   "  type:  ");
        describe_entity_type(entity);
        if constexpr (has_range<T>)
        {
            log.print(indents..., "  range: ");
            auto range = get_range<T>();
            log.println(range.min, " to ", range.max, " (init: ", range.init, ")");
        }
        if constexpr (has_value<T>)
        {
            log.print(indents...,   "  value: ");
            describe_entity_value(entity);
        }
        if constexpr (Component<T>)
        {
            auto describe_group = [&](auto& group, auto groupname)
            {
                boost::pfr::for_each_field(group, [&](auto& entity)
                {
                    describe_entity(groupname, entity, "  ", indents...);
                });
            };
            describe_group(entity.inputs,  "input:   ");
            describe_group(entity.outputs, "output:  ");
        }
    }

    template<typename Components>
    int main(int argc, char** argv, Components& components)
    {
        if (argc < 2) return 2;
        bool describe_component = argc == 2;
        bool describe_endpoint = argc > 2;
        return dispatch<CommandMatcher>(argv[1], components, 2, [&](auto& component)
        {
            if (describe_component)
            {
                describe_entity("component: ", component);
                return 0;
            }
            else if (describe_endpoint) return dispatch<CommandMatcher>(argv[2], component, 2, [&](auto& endpoint)
            {
                describe_entity("endpoint: ", endpoint);
                return 0;
            });
            else return 0;
        });
    }
};

} } } } // namespaces

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

    struct DescribeMatcher
    {
        template<typename stringish, typename Entity>
        bool operator()(stringish name, const Entity& entity)
        {
            using spelling::lower_kebab_case;
            return std::string_view(name) == std::string_view(lower_kebab_case(entity));
        }
    };

    void describe_entity_type(auto& entity)
    {
        using T = decltype(entity);
        if constexpr (Component<T>) log.println("component");
        else if constexpr (PersistentValue<T>) log.println("persistent value");
        else if constexpr (OccasionalValue<T>) log.println("occasional value");
        else if constexpr (Bang<T>) log.println("bang");
        else if constexpr (ClearableFlag<T>) log.println("clearable flag");
        else if constexpr (value_like<T>) log.println("value-like");
    }

    void describe_entity(auto preface, auto& entity, auto ... indents)
    {
        using spelling::lower_kebab_case;
        using T = decltype(entity);
        static_assert(Named<T>);
        log.println(indents..., preface, (const char *)lower_kebab_case(entity));
        log.println(indents..., "  name: \"", entity.name(), "\"");
        log.print(indents...,   "  type:  ");
        describe_entity_type(entity);
        if constexpr (Ranged<T>)
        {
            log.print(indents..., "  range: ");
            auto range = get_range<T>();
            log.println(range.min, " to ", range.max, " (init: ", range.init, ")");
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

    template<typename... Components>
    int main(int argc, char** argv, std::tuple<Components...>& components)
    {
        if (argc < 2) return 2;
        bool describe_component = argc == 2;
        bool describe_endpoint = argc > 2;
        return dispatch<DescribeMatcher>(argv[1], components, 2, [&](auto& component)
        {
            if (describe_component)
            {
                describe_entity("component: ", component);
                return 0;
            }
            else if (describe_endpoint) return dispatch<DescribeMatcher>(argv[2], component, 2, [&](auto& endpoint)
            {
                describe_entity("endpoint: ", endpoint);
                return 0;
            });
            else return 0;
        });
    }
};

} } } } // namespaces

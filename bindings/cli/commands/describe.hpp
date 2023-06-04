#pragma once

#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"
#include "components/endpoints/concepts.hpp"
#include "bindings/name_dispatch.hpp"

namespace sygaldry
{
namespace bindings::cli
{
namespace commands
{

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

    void describe_component(auto& component, bool and_endpoints, auto ... indents)
    {
        using spelling::lower_kebab_case;
        log.println(indents..., "component: ", (const char *)lower_kebab_case(component));
        log.println(indents..., "  name: \"", component.name(), "\"");
        if (and_endpoints) list_endpoints(component, "  ", indents...);
    }

    void list_endpoints(auto& component, auto ... indents)
    {
        // TODO: we should get the inputs and outputs generically
        using spelling::lower_kebab_case;
        auto list_it = [&](auto& endpoint){ describe_endpoint(endpoint, "  ", indents...); };
        auto list_group = [&](auto& group, auto groupname)
        {
            log.println(indents..., groupname, ":");
            boost::pfr::for_each_field(group,  list_it);
        };
        list_group(component.inputs, "inputs");
        list_group(component.outputs, "outputs");
    }

    void describe_endpoint_type(auto& endpoint)
    {
        log.println("todo");
    }

    void describe_endpoint_range(auto& endpoint)
    {
        log.println("todo");
    }

    void describe_endpoint(auto& endpoint, auto ... indents)
    {
        using spelling::lower_kebab_case;
        log.println(indents..., (const char *)lower_kebab_case(endpoint));
        log.println(indents..., "  name: \"", endpoint.name(), "\"");
        log.print(indents..., "  type: ");
        describe_endpoint_type(endpoint);
        if constexpr (concepts::Ranged<decltype(endpoint)>)
        {
            log.print(indents..., "  range: ");
            describe_endpoint_range(endpoint);
        }
    }

    template<typename... Components>
    int main(int argc, char** argv, std::tuple<Components...>& components)
    {
        if (argc < 2) return 2;
        bool list_eps = argc == 2;
        bool describe_eps = argc > 2;
        return dispatch<DescribeMatcher>(argv[1], components, 2, [&](auto& component)
        {
            describe_component(component, list_eps);
            if (describe_eps) return dispatch<DescribeMatcher>(argv[2], component, 2, [&](auto& endpoint)
            {
                describe_endpoint(endpoint, "  ");
                return 0;
            });
            else return 0;
        });
    }
};

}
}
}

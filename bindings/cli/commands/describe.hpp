#pragma once

#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"
#include "components/endpoints/concepts.hpp"
#include "../matcher.hpp"

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

    template<typename NamedT, typename ... Args>
    void log_name(NamedT& t, Args ... args)
    {
        using sygaldry::spelling::lower_kebab_case_v;
        log.println(lower_kebab_case_v<NamedT>, args...);
    }

    template<typename Component>
    void describe_component(Component& component)
    {
        log_name(component, ":");
    }

    template<typename Component>
    void list_endpoints(Component& component)
    {
        //auto& inputs  = get_inputs(component);
        //auto& outputs = get_outputs(component);
        //boost::pfr::for_each_field(inputs,  [&](auto& endpoint){ log_name(endpoint); });
        //boost::pfr::for_each_field(outputs, [&](auto& endpoint){ log_name(endpoint); });
    }

    template<typename... Components>
    int main(int argc, char** argv, std::tuple<Components...>& components)
    {
        if (argc < 2) return 2;
        else return try_to_match_and_execute_with_wildcard(argv[1], components, 2, [&](auto component)
        {

            describe_component(component);

            if (argc > 2)
            {
                auto cb = [&](auto endpoint) { describe_endpoint(endpoint); return true; };
                bool match = false;
                if (match) return 0;
                else 
                {
                    log.println(name(), ": cannot access '", argv[2], "': No such endpoint");
                    return 2;
                }
            }
            else /* argc == 2, no endpoint argument */
            {
                list_endpoints(component);
                return 0;
            }
            return 0;
        });
    }
};

}
}
}

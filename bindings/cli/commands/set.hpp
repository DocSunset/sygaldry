#pragma once

#include <charconv>
#include "concepts/endpoints.hpp"
#include "bindings/name_dispatch.hpp"

namespace sygaldry { namespace bindings { namespace cli { namespace commands {

struct Set
{
    static _consteval auto name() { return "set"; }
    static _consteval auto usage() { return "component-name endpoint-name [value] [value] [...]"; }
    static _consteval auto description() { return "Change the current value of the given endoint"; }

    template<typename T>
    int parse_and_set(auto& log, auto& endpoint, char * argstart)
    {
        T num{};
        auto argend = argstart;

        // search for the end of the token, with an arbitrary maximum upper bound
        for (int i = 0; *argend != 0 && i < 256; ++i, ++argend) {}
        if (*argend != 0) 
        {
            log.println("Unable to parse number, couldn't find end of token");
            return 2;
        }
        auto [ptr, ec] = std::from_chars(argstart, argend, num);
        if (ec == std::errc{})
        {
            set_value(endpoint, num);
            if (ptr != argend)
                log.println("Warning: couldn't parse the entire token '", argstart, "'");
            return 0;
        }
        else
        {
            log.println("Unable to parse token '", argstart, "'");
            return 2;
        }
    }

    template<typename T>
    int set_endpoint_value(auto& log, T& endpoint, int argc, char ** argv)
    {
        if constexpr (Bang<T>)
        {
            if (argc != 0) log.println("Note: no arguments are required to set a bang.");
            set_value(endpoint, true);
            return 0;
        }
        else if constexpr (has_value<T>)
        {
            // TODO: support values with more than one argument
            if (argc < 1)
            {
                log.println("Not enough arguments to set this endpoint.");
                return 2;
            }
            // TODO: we should determine the type of the value and use it here
            else return parse_and_set<float>(log, endpoint, argv[0]);
        }
        else return 2;
    }

    int main(int argc, char** argv, auto& log, auto& components)
    {
        if (argc < 3)
        {
            log.println("usage: ", usage());
            return 2;
        }
        auto component_name = argv[1];
        auto endpoint_name = argv[2];
        return dispatch<CommandMatcher>(component_name, components, 2, [&](auto& component) {
            return dispatch<CommandMatcher>(endpoint_name, component, 2, [&](auto& endpoint) {
                return set_endpoint_value(log, endpoint, argc-3, argv+3);
            });
        });
    }

};

} } } }

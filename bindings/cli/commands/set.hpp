#pragma once

#include <charconv>
#include "concepts/endpoints.hpp"
#include "bindings/name_dispatch.hpp"

namespace sygaldry { namespace bindings { namespace clicommands {

struct Set
{
    static _consteval auto name() { return "set"; }
    static _consteval auto usage() { return "component-name endpoint-name [value] [value] [...]"; }
    static _consteval auto description() { return "Change the current value of the given endoint"; }

    template<typename T>
        requires std::integral<T>
    T from_chars(const char * start, const char * end, bool& success)
    {
        T ret{};
        auto [ptr, ec] = std::from_chars(start, end, ret);
        if (ec == std::errc{}) success = true;
        else success = false;
        return ret;
    }

    template<typename T>
        requires std::floating_point<T>
    T from_chars(const char * start, const char * end, bool& success)
    {
    #ifdef ESP_PLATFORM
        char * e;
        T ret;
        if constexpr (std::is_same_v<T, float>)
            ret = std::strtof(start, &e);
        else if constexpr (std::is_same_v<T, double>)
            ret = std::strtod(start, &e);
        else if constexpr (std::is_same_v<T, long double>)
            ret = std::strtold(start, &e);
        if (start == e) success = false;
        else success = true;
        return ret;
    #else
        T ret;
        auto [ptr, ec] = std::from_chars(start, end, ret);
        if (ec == std::errc{}) success = true;
        else success = false;
        return ret;
    #endif
    }
    template<typename T>
        requires requires (T t, const char * s) {t = s;}
    T from_chars(const char * start, const char *, bool& success)
    {
        success = true;
        return start;
    }

    template<typename T>
    int parse_and_set(auto& log, auto& endpoint, const char * argstart)
    {
        auto argend = argstart;

        // search for the end of the token, with an arbitrary maximum upper bound
        for (int i = 0; *argend != 0 && i < 256; ++i, ++argend) {}
        if (*argend != 0) 
        {
            log.println("Unable to parse number, couldn't find end of token");
            return 2;
        }
        bool success = false;
        T val = from_chars<T>(argstart, argend, success);
        if (success)//ec == std::errc{})
        {
            endpoint = val;
            return 0;
        }
        else
        {
            log.println("Unable to parse token '", argstart, "'");
            return 2;
        }
    }
    int parse_and_set(auto& log, auto& endpoint, int argc, char ** argv)
    {
        for (int i = 0; i < argc; ++i)
        {
            using T = decltype(value_of(endpoint)[0]);
            auto ret = parse_and_set<std::remove_cvref_t<T>>(log, value_of(endpoint)[i], argv[i]);
            if (ret != 0) return ret;
        }
        return 0;
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
        else if constexpr (array_like<T> && not string_like<T>) // strings are handled by the has_value case
        {
            if (argc < size_of(endpoint))
            {
                log.println("Not enough arguments to set this endpoint.");
                return 2;
            }
            else return parse_and_set(log, endpoint, argc, argv);
        }
        else if constexpr (has_value<T>)
        {
            if (argc < 1)
            {
                log.println("Not enough arguments to set this endpoint.");
                return 2;
            }
            else return parse_and_set<value_t<T>>(log, endpoint, argv[0]);
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

} } }

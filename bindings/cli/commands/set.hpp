#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <charconv>
#include "concepts/endpoints.hpp"
#include "bindings/osc_string_constants.hpp"
#include "bindings/osc_match_pattern.hpp"

namespace sygaldry { namespace bindings { namespace clicommands {

struct Set
{
    static _consteval auto name() { return "/set"; }
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
        else if constexpr (array_like<value_t<T>>)
        {
            if (argc < size<value_t<T>>())
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

    template<typename Components>
    int main(int argc, char** argv, auto& log, Components& components)
    {
        if (argc < 2)
        {
            log.println("usage: ", usage());
            return 2;
        }
        for_each_endpoint(components, [&]<typename T>(T& endpoint) {
            if (osc_match_pattern(argv[1], osc_path_v<T, Components>))
                set_endpoint_value(log, endpoint, argc-2, argv+2);
        });
        return 0;
    }

};

} } }

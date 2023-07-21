#pragma once
#include <array>
#include <tuple>
#include <concepts>
#include "concepts/components.hpp"
#include "concepts/endpoints.hpp"
#include "bindings/spelling.hpp"

namespace sygaldry { namespace bindings {

template<typename> struct osc_path_length : std::integral_constant<std::size_t, 0> {};
template<template<typename...>typename L, typename ... Path>
struct osc_path_length<L<Path...>>
: std::integral_constant<std::size_t, (name_length<Path>() + ...) + sizeof...(Path)> {};
template<typename T>
constexpr std::size_t osc_type_string_length()
{
    if constexpr (array_like<value_t<T>>)
    {
        constexpr auto length = size<value_t<T>>() + 1;
        constexpr auto remainder = length % 4;
        constexpr auto padding = 4 - remainder;
        return length + padding;
    }
    else return 4;
}

template<typename> struct osc_path;
template<template<typename...>typename L, typename ... Path>
struct osc_path<L<Path...>>
{
    static constexpr size_t N = osc_path_length<L<Path...>>() + 1; // + 1 for null terminator
    static constexpr std::array<char, N> value = []()
    {
        L<Path...> path;
        std::array<char, N> ret;
        std::size_t write_pos = 0;
        auto copy_one = [&]<typename T>(T)
        {
           ret[write_pos++] = '/';
            for (std::size_t i = 0; i < name_length<T>(); ++i)
            {
                ret[write_pos++] = snake_case_v<T>[i];
            }
        };
        std::apply([&]<typename ... Ts>(Ts... ts)
        {
            (copy_one(ts), ...);
        }, path);
        ret[write_pos] = 0;
        return ret;
    }();
};

template<typename T, typename Components>
constexpr const char * osc_path_v = osc_path<path_t<T,Components>>::value.data();

template<typename T>
struct osc_type_string
{
    static constexpr size_t N = osc_type_string_length<T>();
    static constexpr std::array<char, N> value = []()
    {
        std::array<char, N> ret{0};
        ret[0] = ',';

        if constexpr (Bang<T>) return ret;

        char tag;
        if constexpr (std::integral<element_t<T>>) tag = 'i';
        else if constexpr (std::floating_point<element_t<T>>) tag = 'f';
        else if constexpr (string_like<element_t<T>>) tag = 's';
        else return ret; // this should never happen

        if constexpr (array_like<value_t<T>>)
            for (std::size_t i = 0; i < size<value_t<T>>(); ++i)
                ret[1 + i] = tag;
        else ret[1] = tag;
        return ret;
    }();
};

template<typename T> constexpr const char * osc_type_string_v = osc_type_string<T>::value.data();

} }

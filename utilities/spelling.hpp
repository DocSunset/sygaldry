#pragma once
#include <array>
#include "utilities/consteval.hpp"
#include "components/endpoints/inspectors.hpp"

namespace sygaldry::utilities::spelling
{

constexpr char snake(char c) {return c == ' ' ? '_' : c;}
constexpr char kebab(char c) {return c == ' ' ? '-' : c;}
constexpr char lower(char c)
{
    if ('A' <= c && c <= 'Z') return c+('a'-'A');
    return c;
}
constexpr char upper(char c)
{
    if ('a' <= c && c <= 'z') return c-('a'-'A');
    return c;
}
typedef char (*char_mapping)(char);
template<char_mapping...mappings> struct compose;

template<char_mapping mapping> struct compose<mapping>
{
    constexpr char operator()(char c) { return mapping(c); }
};

template<char_mapping mapping, char_mapping... mappings> struct compose<mapping, mappings...>
{
    constexpr char operator()(char c) { return compose<mappings...>{}(mapping(c)); }
};
template<string_literal in, char_mapping...mappings>
constexpr auto respell()
{
    string_literal out{in.data};
    compose<mappings...> m;
    for (std::size_t i = 0; i < in.size(); ++i)
        out.data[i] = m(in.data[i]);
    return out;
}
#define define_respeller(RESPELLER_NAME, ...) template<string_literal in>\
constexpr auto RESPELLER_NAME()\
{\
    return respell<in, __VA_ARGS__>();\
}

define_respeller(      snake_case, snake)
define_respeller(upper_snake_case, snake, upper)
define_respeller(lower_snake_case, snake, lower)
define_respeller(      kebab_case, kebab)
define_respeller(upper_kebab_case, kebab, upper)
define_respeller(lower_kebab_case, kebab, lower)

#undef define_respeller

}

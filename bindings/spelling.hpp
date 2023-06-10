#pragma once
#include <array>
#include "utilities/consteval.hpp"

namespace sygaldry { namespace bindings {

template<typename Device>
_consteval auto name_length()
{
    size_t ret = 0;
    while (Device::name()[ret] != 0) ret++;
    return ret;
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

template<typename NamedType, char_mapping... Mappings>
struct respeller
{
    static constexpr size_t N = name_length<NamedType>() + 1; // + 1 for null terminator
    static constexpr std::array<char, N> value = [](const char * s)
    {
        auto mapping = compose<Mappings...>{}; // construct the composition of mappings
        std::array<char, N> ret{};
        for (size_t i = 0; i < N; ++i)
        {
            char c = s[i];
            ret[i] = mapping(c); // apply the mapping
        }
        return ret;
    }(NamedType::name());

    respeller(NamedType) {}
    constexpr operator const char *() noexcept { return value.data(); }
};
template<typename NamedType>
struct respeller<NamedType>
{
    respeller(NamedType) {}
    constexpr operator const char *() noexcept { return NamedType::name(); }
};
template<typename NamedType>
struct passthrough_spelling : respeller<NamedType>
{
    passthrough_spelling([[maybe_unused]] NamedType x) : respeller<NamedType>{x} {}
};

template<typename NamedType>
struct snake_case : respeller<NamedType, snake>
{
    snake_case([[maybe_unused]] NamedType x) : respeller<NamedType, snake>{x} {}
};

template<typename NamedType>
struct upper_snake_case : respeller<NamedType, snake, upper>
{
    upper_snake_case([[maybe_unused]] NamedType x) : respeller<NamedType, snake, upper>{x} {}
};

template<typename NamedType>
struct lower_snake_case : respeller<NamedType, snake, lower>
{
    lower_snake_case([[maybe_unused]] NamedType x) : respeller<NamedType, snake, lower>{x} {}
};

template<typename NamedType>
struct kebab_case : respeller<NamedType, kebab>
{
    kebab_case([[maybe_unused]] NamedType x) : respeller<NamedType, kebab>{x} {}
};

template<typename NamedType>
struct upper_kebab_case : respeller<NamedType, kebab, upper>
{
    upper_kebab_case([[maybe_unused]] NamedType x) : respeller<NamedType, kebab, upper>{x} {}
};

template<typename NamedType>
struct lower_kebab_case : respeller<NamedType, kebab, lower>
{
    lower_kebab_case([[maybe_unused]] NamedType x) : respeller<NamedType, kebab, lower>{x} {}
};
template<typename NamedType> constexpr const char * snake_case_v       =       snake_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * upper_snake_case_v = upper_snake_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * lower_snake_case_v = lower_snake_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * kebab_case_v       =       kebab_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * upper_kebab_case_v = upper_kebab_case<NamedType>::value.data();
template<typename NamedType> constexpr const char * lower_kebab_case_v = lower_kebab_case<NamedType>::value.data();

} }

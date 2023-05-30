#pragma once
#include <array>

namespace sygaldry::utilities::metadata::names
{

template<typename Device>
consteval auto name_length()
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
    constexpr operator const char *() { return value.data(); }
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
template<typename NamedType> using snake_case = respeller<NamedType, snake>;
template<typename NamedType> using upper_snake_case = respeller<NamedType, snake, upper>;
template<typename NamedType> using lower_snake_case = respeller<NamedType, snake, lower>;
template<typename NamedType> using kebab_case = respeller<NamedType, kebab>;
template<typename NamedType> using upper_kebab_case = respeller<NamedType, kebab, upper>;
template<typename NamedType> using lower_kebab_case = respeller<NamedType, kebab, lower>;
template<typename NamedType>
constexpr const char * snake_case_v = snake_case<NamedType>::value.data();
template<typename NamedType>
constexpr const char * upper_snake_case_v = upper_snake_case<NamedType>::value.data();
template<typename NamedType>
constexpr const char * lower_snake_case_v = lower_snake_case<NamedType>::value.data();
template<typename NamedType>
constexpr const char * kebab_case_v = kebab_case<NamedType>::value.data();
template<typename NamedType>
constexpr const char * upper_kebab_case_v = upper_kebab_case<NamedType>::value.data();
template<typename NamedType>
constexpr const char * lower_kebab_case_v = lower_kebab_case<NamedType>::value.data();

}

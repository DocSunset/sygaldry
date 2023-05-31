#pragma once

#include <string_view>

namespace sygaldry::ports
{

using namespace sygaldry;

template<std::size_t N>
struct string_literal
{
    char value[N];
    consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

template<string_literal str>
struct named
{
    static consteval auto name() {return std::string_view{str.value};}
};
template<typename T>
concept number = std::is_integral_v<T> || std::is_floating_point_v<T>;

template<number auto _min, number auto _max>
struct ranged
{
    static consteval auto range() {
        static_assert(std::is_same_v<decltype(_min), decltype(_max)>,
                      "ranged min and max must be of same type");
        struct {
            decltype(_min) min = _min;
            decltype(_max) max = _max;
        } r;
        return r;
    }
};
template<number auto _init>
struct initialized
{
    static consteval auto init() {return _init;}
};

//template <string_literal str, typename T>
//    requires std::is_trivial_v<T>
//struct value_port : named<str>
//{
//    using type = T;
//    T value;
//    operator T&() noexcept {return value;}
//    operator const T&() const noexcept {return value;}
//    auto& operator=(T& t) noexcept {return value = t;}
//    auto& operator=(const T& t) noexcept {return value = t;}
//};
//template<string_literal str, bool init = false>
//struct toggle : public value_port<str, bool>
//{
//    struct range
//    {
//        bool min = false;
//        bool max = true;
//        bool init = init;
//    };
//};

//template<string_literal str, float min, float max, 

}

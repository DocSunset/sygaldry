#pragma once

#include <string_view>
#include <optional>

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
template <typename T>
struct persistent
{
    using type = T;
    T value;
    operator T&() noexcept {return value;}
    operator const T&() const noexcept {return value;}
    auto& operator=(T&& t) noexcept {value = std::move(t); return *this;}
    auto& operator=(const T& t) noexcept {value = t; return *this;}
};

template <typename T>
using occasional = std::optional<T>;
template<string_literal str, bool init = false>
struct _btn : named<str>, initialized<init>, ranged<false, true> { };

template<string_literal str, bool init = false>
struct button : occasional<bool>, _btn<str, init>
{
    using occasional<bool>::operator=;
};

template<string_literal str, bool init = false>
struct toggle : persistent<bool>, _btn<str, init>
{
    using persistent<bool>::operator=;
};

template<string_literal str, float init = 0.0f>
struct slider : persistent<float>, named<str>, initialized<init>, ranged<0.0f, 1.0f>
{
    using persistent<float>::operator=;
};
template<string_literal str>
struct bng : persistent<bool>, named<str>
{
    using persistent<bool>::operator=;
    enum {impulse};
    void operator()() {value = true;}
};

}

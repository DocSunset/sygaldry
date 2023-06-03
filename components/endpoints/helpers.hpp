#pragma once

#include <string_view>
#include <optional>
#include "utilities/consteval/consteval.hpp"

namespace sygaldry::endpoints
{

template<std::size_t N>
struct string_literal
{
    char value[N];
    _consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

template<string_literal str>
struct named
{
    static _consteval auto name() {return std::string_view{str.value};}
};
template<typename T>
struct range
{
    const T min;
    const T max;
    const T init;

    _consteval range(const T _min, const T _max) noexcept
    : min{_min < _max ? _min : _max}
    , max{_min > _max ? _min : _max}
    , init{min}
    { }

    _consteval range(const T _min, const T _max, const T _init) noexcept
    : min{_min < _max ? _min : _max}
    , max{_min > _max ? _min : _max}
    , init{_init}
    { }
};

template<auto arg>
struct with {};

template<range r>
struct with<r>
{
    static _consteval auto range() {
        return r;
    }
};
template <typename T>
struct persistent
{
    using type = T;
    T value;
    constexpr operator T&() noexcept {return value;}
    constexpr operator const T&() const noexcept {return value;}
    constexpr auto& operator=(T&& t) noexcept {value = std::move(t); return *this;}
    constexpr auto& operator=(const T& t) noexcept {value = t; return *this;}
};
template <typename T>
using occasional = std::optional<T>;

template<string_literal str, bool init = false>
struct _btn : named<str>, with<range{false, true, init}> { };

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

template<string_literal str, range<float> range_arg = {0.0f, 1.0f, 0.0f}>
struct slider : persistent<float>, named<str>, with<range_arg>
{
    using persistent<float>::operator=;
};
template<string_literal str>
struct bng : persistent<bool>, named<str>
{
    using persistent<bool>::operator=;
    enum {bang, impulse};
    void operator()() {value = true;}
    void reset() {value = false;}
};

}

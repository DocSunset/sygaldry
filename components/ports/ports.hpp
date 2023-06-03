#pragma once

#include <string_view>
#include <optional>
#include "utilities/consteval/consteval.hpp"

namespace sygaldry::ports
{

using namespace sygaldry;

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
    T min;
    T max;
    _consteval range(T _min, T _max) noexcept
    {
        min = _min < _max ? _min : _max;
        max = _min > _max ? _min : _max;
    }
};

template<typename T>
struct init
{
    T value;
    _consteval init(T x) noexcept : value{x} { }
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

template<init i>
struct with<i>
{
    static _consteval auto init() {
        return i.value;
    }
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
template<string_literal str, bool initial_value = false>
struct _btn : named<str>, with<init{initial_value}>, with<range{false, true}> { };

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

template<string_literal str, init<float> initial_value = 0.0f>
struct slider : persistent<float>, named<str>, with<initial_value>, with<range{0.0f, 1.0f}>
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

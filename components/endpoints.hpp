#pragma once

#include <string_view>
#include <optional>
#include "utilities/consteval.hpp"

namespace sygaldry { namespace endpoints {

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
    static _consteval auto name() {return str.value;}
};
template<typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;
template<arithmetic T>
struct num_literal
{
    using type = T;
    T value;
    _consteval num_literal(T f) : value{f} {}
    operator T() {return value;}
};
template<num_literal _min, num_literal _max, num_literal _init = _min>
struct ranged
{
    static _consteval auto range()
    {
        struct {
            decltype(_min.value) min = _min.value;
            decltype(_max.value) max = _max.value;
            decltype(_init.value) init = _init.value;
        } r;
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
struct button : occasional<bool>, named<str>, ranged<false, true, init>
{
    using occasional<bool>::operator=;
};

template<string_literal str, bool init = false>
struct toggle : persistent<bool>, named<str>, ranged<false, true, init>
{
    using persistent<bool>::operator=;
};

template<string_literal str
        , arithmetic T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        >
struct slider : persistent<float>, named<str>, ranged<min, max, init>
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

} } // namespaces

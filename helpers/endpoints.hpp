#pragma once

#include <string_view>
#include <optional>
#include "utilities/consteval.hpp"
#include "helpers/metadata.hpp"

namespace sygaldry {

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
struct range_
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
    constexpr persistent() noexcept : value{} {}
    constexpr persistent(T&& t) noexcept {value = std::move(t);}
    constexpr persistent(const T& t) noexcept : value{t} {}
    constexpr operator T&() noexcept {return value;}
    constexpr operator const T&() const noexcept {return value;}
    constexpr auto& operator=(T&& t) noexcept {value = std::move(t); return *this;}
    constexpr auto& operator=(const T& t) noexcept {value = t; return *this;}
};
template <typename T>
using occasional = std::optional<T>;

template<string_literal str, char init = 0>
struct button : occasional<char>, name_<str>, range_<0, 1, init>
{
    using occasional<char>::operator=;
};

template<string_literal str, char init = 0>
struct toggle : persistent<char>, name_<str>, range_<0, 1, init>
{
    using persistent<char>::operator=;
};

template<string_literal str
        , arithmetic T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        >
struct slider : persistent<float>, name_<str>, range_<min, max, init>
{
    using persistent<float>::operator=;
};
template<string_literal str>
struct bng : persistent<bool>, name_<str>
{
    using persistent<bool>::operator=;
    enum {bang, impulse};
    void operator()() {value = true;}
    void reset() {value = false;}
};

} // namespaces

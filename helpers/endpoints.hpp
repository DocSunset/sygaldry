#pragma once

#include <string_view>
#include <optional>
#include <string>
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

    // convert from T
    constexpr persistent(T&& t) noexcept {value = std::move(t);}
    constexpr persistent(const T& t) noexcept : value{t} {}
    constexpr auto& operator=(T&& t) noexcept {value = std::move(t); return *this;}
    constexpr auto& operator=(const T& t) noexcept {value = t; return *this;}

    // convert to T
    constexpr operator T&() noexcept {return value;}
    constexpr operator const T&() const noexcept {return value;}
};
template <typename T>
struct occasional
{
    using type = T;
    T state;
    bool updated;

    constexpr occasional() noexcept : state{}, updated{false} {}
    constexpr auto& operator=(const occasional<T>& other)
    {
        if (other.updated)
        {
            state = other.state;
            updated = true;
        } else updated = false; // keep current value
        return *this;
    }
    constexpr operator T&() noexcept {return state;}
    constexpr operator const  T&() const noexcept {return state;}

    // optional-like semantics
    constexpr occasional(T&& t) noexcept : state{std::move(t)}, updated{true} {}
    constexpr occasional(const T& t) noexcept : state{t}, updated{true} {}
    constexpr auto& operator=(T&& t) noexcept {state = std::move(t); updated = true; return *this;}
    constexpr auto& operator=(const T& t) noexcept {state = t; updated = true; return *this;}
    constexpr operator bool() noexcept {return updated;}
    constexpr T& operator *() noexcept {return state;}
    constexpr const T& operator *() const noexcept {return state;}
    constexpr T* operator ->() noexcept {return &state;}
    constexpr const T* operator ->() const noexcept {return &state;}
    constexpr T& value() noexcept {return state;}
    constexpr const T& value() const noexcept {return state;}
    constexpr void reset() noexcept {updated = false;} // maintains current state
};


#define tag(TAG) struct tag_##TAG {enum {TAG}; }
tag(write_only);
tag(session_data);
#undef tag
template<typename ... Tags>
struct tagged_ : Tags... {};
template<string_literal name_str, string_literal desc = "", char init = 0, typename ... Tags>
struct button
: occasional<char>
, name_<name_str>
, description_<desc>
, range_<0, 1, init>
, tagged_<Tags...>
{
    using occasional<char>::operator=;
};

template<string_literal name_str, string_literal desc = "", char init = 0, typename ... Tags>
struct toggle
: persistent<char>
, name_<name_str>
, description_<desc>
, range_<0, 1, init>
, tagged_<Tags...>
{
    using persistent<char>::operator=;
};

template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct text
: persistent<std::string>
, name_<name_str>
, description_<desc>
, tagged_<Tags...>
{
    using persistent<std::string>::operator=;
};

template< string_literal name_str
        , string_literal desc = ""
        , arithmetic T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        , typename ... Tags
        >
struct slider
: persistent<float>
, name_<name_str>
, description_<desc>
, range_<min, max, init>
, tagged_<Tags...>
{
    using persistent<float>::operator=;
};
template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct bng
: persistent<bool>
, name_<name_str>
, description_<"">
, tagged_<Tags...>
{
    using persistent<bool>::operator=;
    enum {bang, impulse};
    void operator()() {value = true;}
    void reset() {value = false;}
};

} // namespaces

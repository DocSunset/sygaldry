#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <concepts>
#include "utilities/consteval.hpp"

namespace sygaldry {

template<typename T>
concept has_range = requires
{
    std::decay_t<T>::range().min;
    std::decay_t<T>::range().max;
    std::decay_t<T>::range().init;
};

template<has_range T>
constexpr auto get_range(const T&) { return T::range(); }

template<has_range T>
_consteval auto get_range() { return std::decay_t<T>::range(); }
template <typename T, typename Y>
concept strict_similar_to 
    =  requires (T t) { static_cast<Y>(t); }
    && requires (Y y) { T{y}; }
    && requires (T t, Y y) { t = y; }
    && requires (T a, T b, Y c) { a = b = c; }
    ;

template<typename T, typename Y>
concept similar_to = strict_similar_to<std::remove_cvref_t<T>, std::remove_cvref_t<Y>>;
template <typename T>
concept has_value_variable = requires (T t)
{
    t.value;
    requires similar_to<T, decltype(t.value)>;
};

template <typename T>
concept has_value_method = requires (T t)
{
    t.value();
    requires similar_to<T, decltype(t.value())>;
};

template <typename T>
concept PersistentValue
    =  has_value_variable<T> || has_value_method<T>
    && std::default_initializable<std::remove_cvref_t<T>>;
template<typename T>
concept Flag = requires (T t)
{
    bool(t);
    requires std::is_same_v<std::integral_constant<bool, bool(T{})>, std::false_type>;
    requires std::is_same_v<std::integral_constant<bool, bool(T{T{}})>, std::false_type>;
    t = T{};
};

template<typename T>
concept _occasional_value = requires (T t)
{
    *t;
    T{*t};
    *t = *t;
} && Flag<T>;

template<typename T>
concept OccasionalValue = _occasional_value<T> || _occasional_value<std::decay_t<T>>;
template<typename T>
concept Bang = requires (T t)
{
    requires std::is_enum_v<decltype(T::bang)>;
} && Flag<T>;
template<typename T>
concept ClearableFlag = Flag<T> && (OccasionalValue<T> || Bang<T>);

template<ClearableFlag T>
constexpr void clear_flag(T& t)
{
    t = T{};
}
template <typename T>
concept has_value = OccasionalValue<T> || PersistentValue<T>;

template <has_value T>
auto& value_of(T& v)
{
    static_assert(PersistentValue<T> || OccasionalValue<T>, "value_of: Neither PersistentValue nor OccasionalValue. Did we add a new kind?");
    if constexpr (PersistentValue<T>)
    {
        static_assert(has_value_variable<T> || has_value_method<T>, "value_of: PersistentValue with no method or variable. Did we add a new kind?");
        if constexpr (has_value_variable<T>) return v.value;
        else if constexpr (has_value_method<T>) return v.value();
        else return 0;
    }
    else if constexpr (OccasionalValue<T>)
    {
        return *v;
    }
    else return 0;
}

template<has_value T>
const auto& value_of(const T& v)
{
    return value_of(const_cast<T&>(v));
}

template <has_value T>
auto& set_value(T& v, const auto& arg)
{
    v = arg;
    return v;
}
template<has_value T>
using value_t = std::remove_cvref_t<decltype(value_of(std::declval<T&>()))>;

template<typename T> concept string_like = requires (T t, const char * s)
{
    t = s;
};

template<typename T> concept array_like = not string_like<T> && requires (T t)
{
    t[0];
    t.size();
} && std::same_as<std::remove_cvref_t<decltype(std::tuple_size_v<T>)>, std::size_t>;

template<array_like T>
_consteval auto size()
{
    return std::tuple_size_v<T>;
}

template<typename T> struct _element_type {};
template<typename T> requires (array_like<T> && has_value<T>)
struct _element_type<T>
{
    using type = std::remove_cvref_t<decltype(std::declval<T>()[0])>;
};
template<typename T> requires (has_value<T>)
struct _element_type<T> { using type = value_t<T>; };
template<typename T> requires (array_like<T> || has_value<T>) using element_t = typename _element_type<T>::type;
#define tagged(TAG) template<typename T> concept tagged_##TAG\
    =  std::is_enum_v<decltype(T::TAG)>\
    || std::is_enum_v<typename T::TAG>
tagged(write_only); // don't display or output this endpoint's value
tagged(session_data); // store this endpoint's value across sessions, e.g. across power cycles on an embedded system
#undef tagged

} // namespaces

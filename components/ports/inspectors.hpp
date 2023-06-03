#pragma once

#include <concepts>
#include "utilities/consteval/consteval.hpp"

namespace sygaldry::ports::inspectors
{
    template<typename T>
    concept Named
        =  requires { {T::name()} -> std::convertible_to<std::string>; }
        || requires { {T::name()} -> std::convertible_to<std::string_view>; }
        || requires { {T::name()} -> std::convertible_to<const char *>; }
        ;

    template<Named T>
    _consteval auto get_name(const T&) { return T::name(); }

    template<Named T>
    _consteval auto get_name() { return T::name(); }
    template<typename T>
    concept Ranged = requires
    {
        T::range().min;
        T::range().max;
        T::range().init;
    };

    template<Ranged T>
    _consteval auto get_range(const T&) { return T::range(); }

    template<Ranged T>
    _consteval auto get_range() { return T::range(); }
    template <typename T>
    concept _value_like = requires (T t)
    {
        t.value;
        T{t.value};
        t = t.value;
        t.value = t;
        t.value = t.value;
        t = t;
    };

    template <typename T>
    concept PersistentValue
        =  _value_like<T>
        && std::default_initializable<T>;
    template<typename T>
    concept ClearableFlag = requires (T t)
    {
        bool(t);
        requires std::is_same_v<std::integral_constant<bool, bool(T{})>, std::false_type>;
        requires std::is_same_v<std::integral_constant<bool, bool(T{T{}})>, std::false_type>;
        t = T{};
    };

    template<ClearableFlag T>
    constexpr void clear_flag(T& t)
    {
        t = T{};
    }

    template<typename T>
    concept OccasionalValue = requires (T t)
    {
        *t;
        T{*t};
        *t = *t;
    } && ClearableFlag<T>;
    template<typename T>
    concept Bang = requires (T t)
    {
        requires std::is_enum_v<decltype(T::bang)>;
    } && ClearableFlag<T>;
}

#pragma once

#include <concepts>
#include "utilities/consteval.hpp"

namespace sygaldry { namespace concepts {

    template<typename T>
    concept Named = requires
    {
        {T::name()} -> std::convertible_to<std::string>;
        {T::name()} -> std::convertible_to<std::string_view>;
        {T::name()} -> std::convertible_to<const char *>;
    } || requires
    {
        {std::decay_t<T>::name()} -> std::convertible_to<std::string>;
        {std::decay_t<T>::name()} -> std::convertible_to<std::string_view>;
        {std::decay_t<T>::name()} -> std::convertible_to<const char *>;
    };
    // todo: this should probably be constexpr
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
    } || requires
    {
        std::decay_t<T>::range().min;
        std::decay_t<T>::range().max;
        std::decay_t<T>::range().init;
    };

    template<Ranged T>
    constexpr auto get_range(const T&) { return T::range(); }

    template<Ranged T>
    _consteval auto get_range() { return std::decay_t<T>::range(); }
    template <typename T>
    concept value_like = requires (T t) // TODO: T a, T b
    {
        t.value;
        // TODO: t{t}
        t = t;
        T{t.value};
        t = t.value;
        // TODO: t.value{t}
        t.value = t;
        // TODO: t.value{t.value}
        t.value = t.value;
    };

    template <typename T>
    concept _persistent_value
        =  value_like<T>
        && std::default_initializable<T>;

    template <typename T>
    concept PersistentValue = _persistent_value<T> || _persistent_value<std::decay_t<T>>;
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
    concept _occasional_value = requires (T t)
    {
        *t;
        T{*t};
        *t = *t;
    } && ClearableFlag<T>;

    template<typename T>
    concept OccasionalValue = _occasional_value<T> || _occasional_value<std::decay_t<T>>;
    template<typename T>
    concept Bang = requires (T t)
    {
        requires std::is_enum_v<decltype(T::bang)>;
    } && ClearableFlag<T>;

} } // namespaces

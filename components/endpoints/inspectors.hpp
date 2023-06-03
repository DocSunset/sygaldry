#pragma once

#include <concepts>
#include "utilities/consteval/consteval.hpp"

namespace sygaldry::endpoints
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
    #define type_or_value_qualification(Name)                                         \
      template <typename T>                                                           \
      concept Name##_is_value = requires(T t) { decltype(std::decay_t<T>::Name){}; }; \
                                                                                      \
      template <typename T>                                                           \
      concept Name##_is_type                                                          \
          = requires(T t) { !std::is_void_v<typename std::decay_t<T>::Name>; };       \
                                                                                      \
      template <typename T>                                                           \
      concept has_##Name = Name##_is_type<T> || Name##_is_value<T>;

    #define type_or_value_reflection(Name)                                             \
      template <typename T>                                                            \
      struct Name##_type                                                               \
      {                                                                                \
        using type = void;                                                             \
      };                                                                               \
                                                                                       \
      template <Name##_is_type T>                                                      \
      struct Name##_type<T>                                                            \
      {                                                                                \
        using type = typename std::decay_t<T>::Name;                                   \
      };                                                                               \
                                                                                       \
      template <Name##_is_value T>                                                     \
      struct Name##_type<T>                                                            \
      {                                                                                \
        using type                                                                     \
            = std::remove_reference_t<decltype(std::declval<std::decay_t<T>>().Name)>; \
      };                                                                               \
      template <typename T>                                                            \
      using Name##_type_t = Name##_type<T>;                                            

    #define type_or_value_accessors(Name)                 \
      template <typename T>                               \
        requires(Name##_is_type<T> || Name##_is_value<T>) \
      auto&& get_##Name(T&& t)                            \
      {                                                   \
        if constexpr(Name##_is_type<T>)                   \
          return typename std::decay_t<T>::Name{};        \
        else if constexpr(Name##_is_value<T>)             \
          return std::forward<T>(t).Name;                 \
      }

    type_or_value_qualification(inputs)
    type_or_value_reflection(inputs)
    type_or_value_accessors(inputs)

    type_or_value_qualification(outputs)
    type_or_value_reflection(outputs)
    type_or_value_accessors(outputs)

    #undef type_or_value_qualification
    #undef type_or_value_reflection
    #undef type_or_value_accessors
}

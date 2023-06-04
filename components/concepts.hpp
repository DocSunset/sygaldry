#pragma once

#include <concepts>
#include "utilities/consteval.hpp"

namespace sygaldry { namespace concepts {

    #define text_concept(CONCEPT_NAME) template<typename T> \
    concept has_##CONCEPT_NAME = requires \
    { \
        {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<std::string>; \
        {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<std::string_view>; \
        {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<const char *>; \
    }; \
    template<has_##CONCEPT_NAME T> \
    constexpr auto get_##CONCEPT_NAME(const T&) { return T::CONCEPT_NAME(); } \
     \
    template<has_##CONCEPT_NAME T> \
    _consteval auto get_##CONCEPT_NAME() { return std::decay_t<T>::CONCEPT_NAME(); }

    text_concept(name);
    text_concept(author);
    text_concept(email);
    text_concept(license);
    text_concept(description);
    text_concept(uuid);
    text_concept(unit);
    text_concept(version);
    text_concept(date);

    #undef text_concept
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

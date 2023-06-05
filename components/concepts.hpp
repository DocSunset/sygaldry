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
        if constexpr (PersistentValue<T>)
        {
            if constexpr (has_value_variable<T>) return v.value;
            else if constexpr (has_value_method<T>) return v.value;
            else static_assert(false, "value_of: PersistentValue with no method or variable. Did we add a new kind?");
        }
        else if constexpr (OccasionalValue<T>)
        {
            return *v;
        }
        else static_assert(false, "value_of: Neither PersistentValue nor OccasionalValue. Did we add a new kind?");
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

} } // namespaces

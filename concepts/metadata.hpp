#pragma once

#include <concepts>
#include "utilities/consteval.hpp"

namespace sygaldry {

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

}

#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <concepts>
#include "sygaldry-utilities-consteval.hpp"

namespace sygaldry {

#define text_concept(CONCEPT_NAME) template<typename T> \
concept has_##CONCEPT_NAME = requires \
{ \
    {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<std::string>; \
    {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<std::string_view>; \
    {std::decay_t<T>::CONCEPT_NAME()} -> std::convertible_to<const char *>; \
}; \
template<has_##CONCEPT_NAME T> \
constexpr auto CONCEPT_NAME##_of(const T&) { return T::CONCEPT_NAME(); } \
 \
template<has_##CONCEPT_NAME T> \
_consteval auto CONCEPT_NAME##_of() { return std::decay_t<T>::CONCEPT_NAME(); }

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

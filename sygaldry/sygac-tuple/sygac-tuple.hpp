#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <utility>
#include <tuplet/tuple.hpp>
#include <boost/pfr.hpp>

namespace tpl = tuplet;

namespace sygaldry {

/// \addtogroup sygac
/// \{

/// \defgroup sygac-tuple sygac-tuple: Sygaldry Tuple Functions
/// Literate source code: page-$COMPONENT
/// \{

template<typename T> struct is_tuple : std::false_type {};
template<typename ... Ts> struct is_tuple<tpl::tuple<Ts...>> : std::true_type {};
template<typename T> constexpr const bool is_tuple_v = is_tuple<T>::value;
template<typename T> concept Tuple = is_tuple_v<std::remove_cvref_t<T>>;

template<Tuple T>
constexpr auto tuple_head(T tup)
{
    if constexpr (std::tuple_size_v<T> == 0) return tup;
    else return tpl::get<0>(tup);
}

template<Tuple T, size_t ... Ns>
constexpr auto tuple_tail_impl(T tup, std::index_sequence<Ns...>)
{
    return tpl::make_tuple(tpl::get<Ns + 1>(tup)...);
}

template<Tuple T>
constexpr auto tuple_tail(T tup)
{
    if constexpr (std::tuple_size_v<T> <= 1) return tpl::tuple<>{};
    else return tuple_tail_impl(tup, std::make_index_sequence<std::tuple_size_v<T> - 1>{});
}

void tuple_for_each(Tuple auto t, auto&& f)
{
    t.for_each(f);
}

void tuple_transform(Tuple auto t, auto&& f)
{
    t.map(f);
}

namespace pfr { namespace detail { // sygaldry::pfr::detail
template <class T, std::size_t... I>
constexpr auto make_tpltuple_from_tietuple(const T& t, std::index_sequence<I...>) noexcept {
    return tpl::make_tuple(
        boost::pfr::detail::sequence_tuple::get<I>(t)...
    );
}

template <class T, std::size_t... I>
constexpr auto make_tpltiedtuple_from_tietuple(const T& t, std::index_sequence<I...>) noexcept {
    return tpl::tie(
        boost::pfr::detail::sequence_tuple::get<I>(t)...
    );
}

template <class T, std::size_t... I>
constexpr auto make_consttpltiedtuple_from_tietuple(const T& t, std::index_sequence<I...>) noexcept {
    return tpl::tuple<
        std::add_lvalue_reference_t<std::add_const_t<
            std::remove_reference_t<decltype(boost::pfr::detail::sequence_tuple::get<I>(t))>
        >>...
    >(
        boost::pfr::detail::sequence_tuple::get<I>(t)...
    );
}
} }

namespace pfr {
template <class T>
constexpr auto structure_tie(const T& val) noexcept {
    return ::sygaldry::pfr::detail::make_consttpltiedtuple_from_tietuple(
        boost::pfr::detail::tie_as_tuple(const_cast<T&>(val)),
        boost::pfr::detail::make_index_sequence< boost::pfr::tuple_size_v<T> >()
    );
}

/// \overload structure_tie
template <class T>
constexpr auto structure_tie(T& val) noexcept {
    return ::sygaldry::pfr::detail::make_tpltiedtuple_from_tietuple(
        boost::pfr::detail::tie_as_tuple(val),
        boost::pfr::detail::make_index_sequence< boost::pfr::tuple_size_v<T> >()
    );
}

/// \overload structure_tie
template <class T>
constexpr auto structure_tie(T&&, std::enable_if_t< std::is_rvalue_reference<T&&>::value>* = nullptr) noexcept {
    static_assert(sizeof(T) && false, "====================> Boost.PFR: Calling boost::pfr::structure_tie on rvalue references is forbidden");
    return 0;
}
}

/// \}
/// \}

}

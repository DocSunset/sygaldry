#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

namespace sygaldry { namespace sensors { namespace portable {

/*! \addtogroup sygsp-array_order_mapping
*/
/// \{

/*! \defgroup sygsp-array_order_mapping_function Array Order Mapping Function
*/
/// \{

/*! \brief Change the order of the elements in an array or array-like endpoint

\param[in] in The input array to reorder.
\param[out] out An array in which the reordered output can be written.
\param[in] size The expected number of elements in the in, out, and order arrays.
\param[in] order

A specification of the output order, such that the nth element `order[n]`
states the index into the input array from which the nth element of the output
should be given. E.g. to reverse a 3-element input, give `order = {2,1,0};`.

\warning

It is assumed without checking to that all elements of `order` are valid
indices into the input array, e.g. non-negative integers less than `size`.
If this is not true, an out of bounds memory access will occur resulting in
undefined behaviour (likely a crash).

*/
constexpr void array_order_mapping(const auto& in, auto& out, std::size_t size, const auto& order) noexcept
{
    for (std::size_t i = 0; i < size; ++i)
        out[i] = in[order[i]];
}

/// Overload for array-like types that have a size member function
template<typename T> requires requires (T t) { t.size(); }
constexpr void array_order_mapping(const T& in, auto& out, const auto& order) noexcept
{
    array_order_mapping(in, out, in.size(), order);
}

/// Overload for raw C-style arrays
template<typename T, std::size_t N>
constexpr void array_order_mapping(const T(& in)[N], auto& out, const auto& order) noexcept
{
    array_order_mapping(in, out, N, order);
}

} } }

/// \}
/// \}

#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include <array>

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
indices into the input array, e.g. non-negative integers less than `size`, and
that all arrays have at least `size` elements. If either assumption is invalid,
an out of bounds memory access will occur resulting in undefined behaviour
(likely a crash). Use one of the overloads below for greater static safety
guarantees.

*/
constexpr void array_order_mapping( const auto& in
                                  , auto& out
                                  , std::size_t size
                                  , const auto& order
                                  ) noexcept
{
    for (std::size_t i = 0; i < size; ++i) out[i] = in[order[i]];
}

/*! Overload for array-like types that have a size member function

This overload ensures that the output and order arrays have at least as many
elements as the input array (they are allowed to have more that will not be
accessed by this function), but it does not validate the order indices. See
array_order_mapping_is_valid.

Note that it is allowed for all three arrays to have different value types.
*/
template<typename T, typename Y, typename Z>
    requires requires (T t) { t.size(); }
    && requires (Y y) { y.size(); }
    && requires (Z z) { z.size(); }
constexpr void array_order_mapping(const T& in, Y& out, const Z& order) noexcept
{
    static_assert( T{}.size() <= Y{}.size() && T{}.size() <= Z{}.size()
                 , "out and order arrays must be at least as long or longer than input array"
                 );
    array_order_mapping(in, out, in.size(), order);
}

/*! Overload for raw C-style arrays

This overload ensures that the output and order arrays have at least as many
elements as the input array (they are allowed to have more that will not be
accessed by this function), but it does not validate the order indices. See
array_order_mapping_is_valid.

Note that it is allowed for all three arrays to have different value types.
*/
template<typename T, std::size_t N, typename Y, std::size_t M, typename Z, std::size_t L>
constexpr void array_order_mapping(const T(& in)[N], Y(& out)[M], const Z(& order)[L]) noexcept
{
    static_assert( N <= M && N <= L
                 , "out and order arrays must be at least as long or longer than input array"
                 );
    array_order_mapping(in, out, N, order);
}

/*! \brief Verify that the given order array's elements are valid indices.

\param[in] size Expected size of the array to reorder.
\param[in] order The re-ordering indices to validate.
\return True if the order is valid, false otherwise.

\warning

This function assumes that the `order` array is at least `size` elements long.
If that is not the case, an out of bounds memory access will occur resulting in
undefined behavior. It also doesn't check whether the `in` and `out` arrays are
the right size.
*/
constexpr bool array_order_mapping_is_valid(std::size_t size, const auto& order) noexcept
{
    for (std::size_t i = 0; i < size; ++i) if (order[i] < 0 || size <= order[i]) return false;
    return true;
}

/*! Overload where the order indices are known at compile time, but not the size of the input and output arrays

This overload ensures that the indices in the `order` array, given as template
parameters, are valid with respect to the size `N` given as a template
parameter. It's not possible to ensure whether the number of indices
corresponds correctly to the size of the `input` or `output` arrays in this
case, where the type of these arrays is unknown.
*/
template<std::size_t N, std::size_t ... indices>
constexpr void array_order_mapping(const auto& in, auto& out) noexcept
{
    constexpr const std::size_t order[sizeof...(indices)]{indices...};
    static_assert(N <= sizeof...(indices));
    static_assert(array_order_mapping_is_valid(N, order));
    array_order_mapping(in, out, N, order);
}

/*! Overload where the order indices are known at compile time for types with constexpr size operators and constructors

This overload is statically safety-checked and guaranteed not to result in
runtime errors due to out-of-bounds memory access.
*/
template<std::size_t ... indices, typename T, typename Y>
    requires requires (T t) { t.size(); }
    && requires (Y y) { y.size(); }
constexpr void array_order_mapping(const T& in, Y& out) noexcept
{
    constexpr const std::size_t order[sizeof...(indices)]{indices...};
    static_assert(T{}.size() <= Y{}.size() && T{}.size() <= sizeof...(indices));
    static_assert(array_order_mapping_is_valid(T{}.size(), order));
    array_order_mapping(in, out, T{}.size(), order);
}

/*! Overload where the order indices are known at compile time for c arrays

This overload is statically safety-checked and guaranteed not to result in
runtime errors due to out-of-bounds memory access.
*/
template<std::size_t ... indices, typename T, std::size_t N, typename Y, std::size_t M>
constexpr void array_order_mapping(const T(& in)[N], Y(& out)[M]) noexcept
{
    constexpr std::size_t order[sizeof...(indices)]{indices...};
    static_assert(N <= M && N <= sizeof...(indices));
    static_assert(array_order_mapping_is_valid(N, order));
    array_order_mapping(in, out, N, order);
}

/// \}
/// \}

} } }


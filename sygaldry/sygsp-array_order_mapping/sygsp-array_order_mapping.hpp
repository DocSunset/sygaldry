#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"
#include "sygsp-array_order_mapping_function.hpp"

namespace sygaldry { namespace sensors { namespace portable {

/*! \addtogroup sygsp
*/
/// \{

/*! \defgroup sygsp-array_order_mapping Array Order Mapping
*/
/// \{

/*! Map the template-parameterised input array to a re-ordered output.

Unlike the `array_order_mapping` function, this component is safe at
runtime, and guarantees statically that no out of bounds access will
occur, as long as `array_in` is a `sygaldry::array`, or something
equivalent.

\tparam array_in The type of the input array to be passed to `main()`.
\tparam order

A `std::array` of indices with which to re-order `array_in`.
See \ref sygsp-array_order_mapping_function for the
expected format of this array.

*/
template<typename array_in, std::size_t ... indices>
struct ArrayOrderMapping
: name_<"Array Order Mapping">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct outputs_t {
        array< "reordered"
             , array_in::size()
             , "reordered array"
             , typename array_in::type
             , array_in::min()
             , array_in::max()
             , array_in::init()
             > out;
    } outputs;

    /*! Perform the remapping with output to `reordered`.

    \param[in] in The array with type `array_in` that will be reordered by the mapping.
    
    \sa \ref sygsp-array_order_mapping_function.
    */
    void main(array_in& in) noexcept
    {
        array_order_mapping<indices...>(in, outputs.out);
        if constexpr (requires (array_in a) {a.set_updated;}) outputs.out.set_updated();
    }

    /// Alias for `main()`.
    void operator()(array_in& in) noexcept {main(in);}
};

/// \}
/// \}

} } }

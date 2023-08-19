#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

namespace sygaldry { namespace sensors { namespace portable {

/*! \addtogroup sygsp
*/
/// \{

/*! \defgroup sygsp-array_order_mapping Array Order Mapping
*/
/// \{

/*! Map the template-parameterised input array to a re-ordered output.

\tparam array_in The type of the input array to be passed to `main()`.
\tparam order

A `std::array` of indices with which to re-order `array_in`.
See \ref sygsp-array_order_mapping_function for the
expected format of this array.

*/
template<typename array_in, std::array<std::size_t, array_in::size()> order, typename ... Tags>
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
             , array_in::description()
             , array_in::min()
             , array_in::max()
             , array_in::init()
             , Tags...
             > reordered;
    } outputs;

    /*! Perform the remapping with output to `reordered`.

    \param[in] in The array with type `array_in` that will be reordered by the mapping.
    
    \sa \ref sygsp-array_order_mapping_function.
    */
    void main()
    {
        array_order_mapping(
    }
}

/// \}
/// \}

} } }

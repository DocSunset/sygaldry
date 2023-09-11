#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <array>
#include "sygac-mimu.hpp"
#include "sygsp-complementary_mimu_fusion_endpoints.hpp"

namespace sygaldry { namespace sygsp {

/*! \addtogroup sygsp
*/
/// \{

/*! \defgroup sygsp-mimu_fusion MIMU Sensor Fusion
*/
/// \{

/*! Core complementary MIMU fusion filter initialization

\param[in,out] in Filter input endpoints to initialize
\param[in,out] out Filter output endpoints to initialize
*/
void complementary_mimu_fusion_init( ComplementaryMimuFusionInputs& in
                                   , ComplementaryMimuFusionOutputs& out
                                   );


/*! Core complementary MIMU fusion filter routine

\param[in] gyro Angular rate measurement from gyroscope. A gyroscope measurement must always be given.
\param[in] accl Acceleration measurement from accelerometer. Set to all 0 if no measurement is available.
\param[in] magn Magnetic field measurement from magnetometer. Set to all 0 if no measurement is available.
\param[in] elapsed Time elapsed since last update (i.e. measurement period).
\param[in] in Filter input endpoints
\param[out] out Filter output endpoints
*/
void complementary_mimu_fusion( const std::array<float, 3>& gyro
                              , const std::array<float, 3>& accl, bool accl_updated
                              , const std::array<float, 3>& magn, bool magn_updated
                              , const unsigned long elapsed
                              , const ComplementaryMimuFusionInputs& in
                              , ComplementaryMimuFusionOutputs& out
                              );

/*! \brief MIMU sensor fusion and runtime calibration component

Based on measurements of the magnetic field, acceleration, and
angular rate from a MIMU sensor, estimate the orientation of the
sensor with respect to the global frame of reference, and various
other signals of interest.
*/
template<MimuComponent Mimu>
struct ComplementaryMimuFusion
: name_<"Complementary MIMU Fusion Filter">
, description_<"Runtime calibration and sensor fusion using a complementary filter approach">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    ComplementaryMimuFusionInputs inputs;

    ComplementaryMimuFusionOutputs outputs;

    /// Initialize the filter
    void init()
    {
        complementary_mimu_fusion_init(inputs, outputs);
    }

    /// Update the filter
    void main(const Mimu& mimu)
    {
        if (gyro_of(mimu))
            complementary_mimu_fusion( gyro_of(mimu)
                                     , accl_of(mimu), accl_of(mimu)
                                     , magn_of(mimu), magn_of(mimu)
                                     , mimu.outputs.elapsed
                                     , inputs, outputs);
    }
};

/// \}
/// \}

} }

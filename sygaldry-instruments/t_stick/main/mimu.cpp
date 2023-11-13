#include "sygsp-icm20948.hpp"
#include "sygsa-two_wire_serif.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"

using namespace sygaldry;
template struct sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                               , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                               >;
template struct sygsp::ComplementaryMimuFusion<sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                               , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                               >>;

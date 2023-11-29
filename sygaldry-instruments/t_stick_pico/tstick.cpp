#include<cmath>
#include "sygbr-runtime.hpp"
#include "sygsr-button.hpp"
#include "sygsa-trill_craft.hpp"
#include "sygsa-two_wire.hpp"
#include "sygsp-icm20948.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"
#include "sygsa-two_wire_serif.hpp"

using namespace sygaldry;

struct TStick {
    sygsa::TwoWire<0,1,400000> i2c;
    sygsr::Button<26> button;
    sygsa::TrillCraft trill;
    sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                   , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                   > mimu;
    sygsp::ComplementaryMimuFusion<decltype(mimu)> mimu_fusion;
};

sygaldry::sygbr::PicoSDKInstrument<TStick> runtime{};
int main(){runtime.main();}

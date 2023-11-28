#include<cmath>
#include "sygbr-runtime.hpp"
#include "sygbp-test_component.hpp"
#include "sygsr-button.hpp"
#include "sygsa-trill_craft.hpp"
#include "sygsa-two_wire.hpp"

using namespace sygaldry;

struct Test {
    sygsa::TwoWire<0,1,400000> i2c;
    sygbp::TestComponent tc;
    sygsr::Button<26> button;
    sygsa::TrillCraft trill;
};

sygaldry::sygbr::PicoSDKInstrument<Test> runtime{};
int main(){runtime.main();}

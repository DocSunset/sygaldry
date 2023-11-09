#include<cmath>
#include "sygbr-runtime.hpp"
#include "sygbp-test_component.hpp"
#include "sygsr-button.hpp"

struct Test {
    sygaldry::sygbp::TestComponent tc;
    sygaldry::sygsr::Button<15, sygaldry::sygsp::ButtonActive::High> button;
};

sygaldry::sygbr::PicoSDKInstrument<Test> runtime{};
int main(){runtime.main();}

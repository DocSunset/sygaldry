#include<cmath>
#include "sygbr-runtime.hpp"
#include "sygbp-test_component.hpp"

struct Test {
    sygaldry::sygbp::TestComponent tc;
};

sygaldry::sygbr::PicoSDKInstrument<Test> runtime{};
int main(){runtime.main();}

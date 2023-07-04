#include <string>
#include <catch2/catch_test_macros.hpp>
#include "concepts/components.hpp"
#include "helpers/endpoints.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/liblo.hpp"

using std::string;

using namespace sygaldry;
using namespace sygaldry::bindings;
using namespace sygaldry::components;

TEST_CASE("liblo osc port is valid")
{
    LibloOsc<TestComponent> osc;
    text_message<"s1"> s1{};
    text_message<"s2"> s2{string("7777")};
    text_message<"s3"> s3{string("77777")};
    CHECK(not osc.port_is_valid(s1));
    CHECK(osc.port_is_valid(s2));
    CHECK(not osc.port_is_valid(s3));
}

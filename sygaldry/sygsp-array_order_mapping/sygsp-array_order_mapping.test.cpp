#include <catch2/catch_test_macros.hpp>
#include "sygah-endpoints.hpp"
#include "sygsp-array_order_mapping.hpp"

using namespace sygaldry;
using namespace sygaldry::sensors;
using namespace sygaldry::sensors::portable;

TEST_CASE("sygsp-array_order_mapping component")
{
    array<"in", 5, "for testing", int, 0, 4> in; in.value = {0,1,2,3,4};
    ArrayOrderMapping<decltype(in), 4, 3, 2, 1, 0> mapping;
    mapping(in);
    CHECK(mapping.outputs.out.value == std::array<int,5>{4,3,2,1,0});
}

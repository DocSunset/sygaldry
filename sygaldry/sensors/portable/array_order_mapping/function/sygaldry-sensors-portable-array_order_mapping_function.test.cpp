/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include <array>
#include <cstring>
#include "sygaldry-helpers-endpoints.hpp"
#include "sygaldry-sensors-portable-array_order_mapping_function.hpp"

using namespace sygaldry;
using namespace sygaldry::sensors;
using namespace sygaldry::sensors::portable;
TEST_CASE("sygaldry-sensors-portable-array_order_mapping_function")
{
    SECTION("c array")
    {
        int in[5] = {0, 1, 2, 3, 4};
        int out[5] = {0};
        int order[5] = {1, 2, 4, 0, 3};
        array_order_mapping(in, out, order);
        CHECK(std::memcmp((const void *)out, (const void *)order, 5 * sizeof(int)) == 0);
    }

    SECTION("std::array")
    {
        std::array<int, 5> in = {0, 1, 2, 3, 4};
        std::array<int, 5> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping(in, out, order);
        CHECK(out == order);
    }

    SECTION("sygaldry::array")
    {
        array<"array1", 5, "for testing", int, 0, 4> in; in.value = {0, 1, 2, 3, 4};
        array<"array2", 5, "for testing", int, 0, 4> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping(in, out, order);
        CHECK(out.value == order);
    }
}

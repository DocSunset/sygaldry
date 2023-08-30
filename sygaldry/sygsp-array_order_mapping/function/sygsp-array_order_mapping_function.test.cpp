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
#include "sygah-endpoints.hpp"
#include "sygsp-array_order_mapping_function.hpp"

using namespace sygaldry;
using namespace sygaldry::sensors;
using namespace sygaldry::sensors::portable;
TEST_CASE("sygsp-array_order_mapping function")
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

    SECTION("sygaldry::array_message")
    {
        array_message<"array1", 5, "for testing", int, 0, 4> in; in.value() = {0, 1, 2, 3, 4};
        array_message<"array2", 5, "for testing", int, 0, 4> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping(in, out, order);
        CHECK(out.value() == order);
    }
}
TEST_CASE("sygsp-array_order_mapping_is_valid")
{
    static constexpr int order[5] = {0,1,2,3,4};
    static constexpr int too_big_order[5] = {5,1,2,3,4};
    static constexpr int negative_order[5] = {5,1,2,3,4};

    static_assert(array_order_mapping_is_valid(5, order));
    static_assert(!array_order_mapping_is_valid(5, too_big_order));
    static_assert(!array_order_mapping_is_valid(5, negative_order));
}
TEST_CASE("sygsp-array_order_mapping safe overloads")
{
    SECTION("c array")
    {
        int in[5] = {0, 1, 2, 3, 4};
        int out[5] = {0};
        int order[5] = {1, 2, 4, 0, 3};
        array_order_mapping<1,2,4,0,3>(in, out);
        CHECK(std::memcmp((const void *)out, (const void *)order, 5 * sizeof(int)) == 0);
    }

    SECTION("std::array")
    {
        std::array<int, 5> in = {0, 1, 2, 3, 4};
        std::array<int, 5> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping<1,2,4,0,3>(in, out);
        CHECK(out == order);
    }

    SECTION("sygaldry::array")
    {
        array<"array1", 5, "for testing", int, 0, 4> in; in.value = {0, 1, 2, 3, 4};
        array<"array2", 5, "for testing", int, 0, 4> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping<1,2,4,0,3>(in, out);
        CHECK(out.value == order);
    }

    SECTION("sygaldry::array_message")
    {
        array_message<"array1", 5, "for testing", int, 0, 4> in; in.value() = {0, 1, 2, 3, 4};
        array_message<"array2", 5, "for testing", int, 0, 4> out{};
        std::array<int, 5> order = {1, 2, 4, 0, 3};
        array_order_mapping<1,2,4,0,3>(in, out);
        CHECK(out.value() == order);
    }
}
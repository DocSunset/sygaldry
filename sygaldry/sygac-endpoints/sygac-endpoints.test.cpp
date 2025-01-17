/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <string>
#include <type_traits>
#include <optional>
#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"
#include "sygac-endpoints.hpp"

using namespace sygaldry;
using std::string_view;
using std::string;

struct struct_with_range : range_<0, 127> {};
struct struct_with_init : range_<0.0f, 100.0f, 42.0f> {};
TEST_CASE("sygaldry has_range", "[components][concepts][has_range]")
{
    static_assert(has_range<struct_with_range>);
    static_assert(has_range<struct_with_init>);
    struct_with_range foo{};
    SECTION("T")
    {
        REQUIRE(get_range(foo).min == 0);
        REQUIRE(get_range(foo).max == 127);
        REQUIRE(get_range(foo).init == 0);
        REQUIRE(get_range<struct_with_range>().min == 0);
        REQUIRE(get_range<struct_with_range>().max == 127);
        REQUIRE(get_range<struct_with_range>().init == 0);
        auto r1 = get_range(foo);
        auto r2 = get_range<struct_with_range>();
    }
    SECTION("T&")
    {
        auto& bar = foo;
        REQUIRE(get_range(bar).min == 0);
        REQUIRE(get_range(bar).max == 127);
        REQUIRE(get_range(bar).init == 0);
        REQUIRE(get_range<struct_with_range&>().min == 0);
        REQUIRE(get_range<struct_with_range&>().max == 127);
        REQUIRE(get_range<struct_with_range&>().init == 0);
        auto r3 = get_range(bar);
        auto r4 = get_range<struct_with_range&>();
    }
    SECTION("constT&")
    {
        const auto& baz = foo;
        REQUIRE(get_range(baz).min == 0);
        REQUIRE(get_range(baz).max == 127);
        REQUIRE(get_range(baz).init == 0);
        REQUIRE(get_range<struct_with_range&>().min == 0);
        REQUIRE(get_range<struct_with_range&>().max == 127);
        REQUIRE(get_range<struct_with_range&>().init == 0);
        auto r5 = get_range(baz);
        auto r6 = get_range<const struct_with_range&>();
    }
}
TEST_CASE("sygaldry PersistentValue", "[components][concepts][PersistentValue]")
{
    SECTION("similar_to")
    {
        static_assert(similar_to<float, float>);
        static_assert(similar_to<float&, float>);
        static_assert(similar_to<const float&, float>);
        static_assert(similar_to<float&&, float>);

        static_assert(not similar_to<const char *, float>);
        static_assert(not similar_to<const char *, int>);
        static_assert(not similar_to<float, const char *>);
        static_assert(not similar_to<int, const char *>);
    }

    SECTION("has_value_variable")
    {
        static_assert(has_value_variable<persistent<float>>);
        static_assert(has_value_variable<persistent<float>&>);
        static_assert(has_value_variable<const persistent<float>&>);
        static_assert(has_value_variable<persistent<float>&&>);
    }

    SECTION("has_value_method")
    {
        static_assert(has_value_variable<slider<"foo">>);
        static_assert(has_value_variable<slider<"foo">&>);
        static_assert(has_value_variable<const slider<"foo">&>);
        static_assert(has_value_variable<slider<"foo">&&>);
    }

    SECTION("persistent<T> produces persistent values")
    {
        static_assert(PersistentValue<persistent<float>>);
        static_assert(PersistentValue<persistent<float>&>);
        static_assert(PersistentValue<const persistent<float>&>);
        static_assert(PersistentValue<persistent<float>&&>);
    }

    SECTION("PersistentValues are similar to their value type")
    {
        static_assert(similar_to<persistent<float>, float>);
        static_assert(similar_to<persistent<float>&, float>);
        static_assert(similar_to<const persistent<float>&, float>);
        static_assert(similar_to<persistent<float>&&, float>);
    }
}
TEST_CASE("sygaldry OccasionalValue", "[components][concepts][OccasionalValue]")
{
    static_assert(OccasionalValue<std::optional<float>>);
    static_assert(OccasionalValue<occasional<float>>);
}
TEST_CASE("sygaldry Bang", "[components][concepts][Bang]")
{
    static_assert(Bang<bng<"foo">>);
    static_assert(has_value<bng<"foo">>);
}
TEST_CASE("sygaldry sygac-endpoints ClearableFlag")
{
    static_assert(Flag<bool>);
    static_assert(Flag<float*>);
    static_assert(Flag<persistent<bool>>);
    static_assert(Flag<bng<"foo">>);
    static_assert(Flag<occasional<float>>);

    static_assert(not ClearableFlag<bool>);
    static_assert(not ClearableFlag<float*>);
    static_assert(not ClearableFlag<persistent<bool>>);
    static_assert(ClearableFlag<bng<"foo">>);
    static_assert(ClearableFlag<occasional<float>>);

    SECTION("clear_flag")
    {
        SECTION("Bang")
        {
            bng<"foo"> flag{true};
            REQUIRE(flag_state_of(flag));
            clear_flag(flag);
            REQUIRE(not flag_state_of(flag));
        }
        SECTION("OccasionalValue")
        {
            occasional<float> flag = 100.0f;
            REQUIRE(flag_state_of(flag));
            clear_flag(flag);
            REQUIRE(not flag_state_of(flag));
        }
    }
}
TEST_CASE("sygaldry Value Access", "[components][concepts][value_of][clear_flag]")
{
    SECTION("value_of")
    {
        SECTION("PersistentValue")
        {
            persistent<float> v{100.0f};
            REQUIRE(value_of(v) == 100.0f);
            static_assert(std::is_same_v<float, value_t<decltype(v)>>);

            const persistent<float> cv{100.0f};
            REQUIRE(value_of(cv) == 100.0f);
            static_assert(std::is_const_v<std::remove_reference_t<decltype(value_of(cv))>>);
            static_assert(std::is_lvalue_reference_v<decltype(value_of(cv))>);
            static_assert(std::is_same_v<float, value_t<decltype(cv)>>);

            persistent<string> s{"hello world"};
            REQUIRE(value_of(s) == string("hello world"));
            static_assert(std::is_same_v<string, value_t<decltype(s)>>);
        }
        SECTION("OccasionalValue")
        {
            occasional<float> v = 100.0f;
            REQUIRE(value_of(v) == 100.0f);
            static_assert(std::is_same_v<float, value_t<decltype(v)>>);

            const occasional<float> cv = 100.0f;
            REQUIRE(value_of(cv) == 100.0f);
            static_assert(std::is_same_v<float, value_t<decltype(cv)>>);

            occasional<string> s{"hello world"};
            REQUIRE(value_of(s) == string("hello world"));
            static_assert(std::is_same_v<string, value_t<decltype(s)>>);
        }
    }
    SECTION("set via value_of")
    {
        SECTION("PersistentValue")
        {
            persistent<float> v = 100.0f;
            value_of(v) = 200.0f;
            REQUIRE(value_of(v) == 200.0f);
        }
        SECTION("OccasionalValue")
        {
            occasional<float> v = 100.0f;
            value_of(v) = 200.0f;
            REQUIRE(value_of(v) == 200.0f);
        }
    }
    SECTION("set_value")
    {
        SECTION("PersistentValue")
        {
            persistent<float> v = 100.0f;
            set_value(v, 200.0f);
            REQUIRE(value_of(v) == 200.0f);

            occasional<string> s{"hello world"};
            set_value(s, "value changed");
            REQUIRE(value_of(s) == string("value changed"));
            static_assert(std::is_same_v<string, value_t<decltype(s)>>);
        }
        SECTION("OccasionalValue")
        {
            occasional<float> v = {};
            REQUIRE(not flag_state_of(v));
            set_value(v, 200.0f);
            REQUIRE(flag_state_of(v));
            REQUIRE(value_of(v) == 200.0f);
        }
        SECTION("array_like value (set from array value)")
        {
            occasional<std::array<float,3>> v = {};
            REQUIRE(not flag_state_of(v));
            set_value(v, std::array<float,3>{5.0f,6.0f,7.0f});
            REQUIRE(flag_state_of(v));
            REQUIRE(value_of(v)[0] == 5);
        }
        SECTION("array_like value (set from element value)")
        {
            occasional<std::array<float,3>> v = {};
            REQUIRE(not flag_state_of(v));
            set_value(v, 5.0f);
            REQUIRE(flag_state_of(v));
            REQUIRE(value_of(v)[0] == 5);
        }
    }
}
static_assert(string_like<value_t<text<"a text">>>);
static_assert(string_like<value_t<text_message<"a text message">>>);
static_assert(array_like<value_t<array<"an array", 3>>>);
static_assert(std::is_same_v<element_t<array<"an array", 3>>, float>);
static_assert(std::is_same_v<element_t<slider<"a slider">>, value_t<slider<"a slider">>>);
static_assert(not array_like<value_t<slider<"a slider">>>);
static_assert(not array_like<value_t<text<"a text">>>);

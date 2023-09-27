/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <string>
#include <catch2/catch_test_macros.hpp>
#include <rapidjson/stream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "sygah-endpoints.hpp"
#include "sygbp-rapid_json.hpp"

using std::string;
using namespace sygaldry;
using namespace sygaldry::sygbp;

struct test_component_t
: name_<"Test">
{
    struct inputs_t {
        text_message<"text", "description goes here", tag_session_data> some_text;
        slider<"slider", "description goes here", float, 0.0f, 1.0f, 0.0f, tag_session_data> my_slider;
        array<"array", 3, "description goes here", float, 0.0f, 1.0f, 0.0f, tag_session_data> my_array;
    } inputs;

    void main() {}
} test_component;
struct OStream{
    inline static rapidjson::StringBuffer obuffer{};
    rapidjson::Writer<rapidjson::StringBuffer> writer;
    OStream() : writer{obuffer} {}
};
using TestStorage = RapidJsonSessionStorage<rapidjson::StringStream, OStream, decltype(test_component)>;
TEST_CASE("sygaldry RapidJSON creates object given empty input stream")
{
    string ibuffer{""};
    rapidjson::StringStream istream{ibuffer.c_str()};
    TestStorage storage{};
    storage.init(istream, test_component);
    CHECK(storage.json.IsObject());
    REQUIRE(storage.json.ObjectEmpty());
}
TEST_CASE("sygaldry RapidJSON sets endpoints based on input stream")
{
    string ibuffer{
R"JSON(
{ "/Test/text" : "hello world"
, "/Test/slider" : 42.0
, "/Test/array" : [1.0,2.0,3.0]
})JSON"};
    rapidjson::StringStream istream{ibuffer.c_str()};
    TestStorage storage{};
    test_component_t tc{};
    storage.init(istream, tc);
    CHECK(tc.inputs.some_text.value() == string("hello world"));
    CHECK(tc.inputs.my_slider.value == 42.0f);
    CHECK(tc.inputs.my_array.value == std::array{1.0f,2.0f,3.0f});
}
TEST_CASE("sygaldry RapidJSON external_destinations")
{
    string ibuffer{""};
    rapidjson::StringStream istream{ibuffer.c_str()};
    OStream::obuffer.Clear();
    TestStorage storage{};
    test_component_t tc{};
    storage.init(istream, tc);

    tc.inputs.some_text = string("foo");
    tc.inputs.my_slider.value = 888;
    tc.inputs.my_array.value = std::array{1.0f,2.0f,3.0f};
    storage.external_destinations(tc);

    CHECK(storage.json.HasMember("/Test/text"));
    CHECK(storage.json["/Test/text"].IsString());
    CHECK(string("foo") == string(storage.json["/Test/text"].GetString()));

    CHECK(storage.json.HasMember("/Test/slider"));
    CHECK(storage.json["/Test/slider"].IsDouble());
    CHECK(888.0 == storage.json["/Test/slider"].GetDouble());

    CHECK(storage.json["/Test/array"].IsArray());
    CHECK(storage.json["/Test/array"].Size() == 3);
    CHECK(storage.json["/Test/array"][0].GetDouble() == 1.0f);
    CHECK(storage.json["/Test/array"][1].GetDouble() == 2.0f);
    CHECK(storage.json["/Test/array"][2].GetDouble() == 3.0f);

    CHECK(string(R"JSON({"/Test/text":"foo","/Test/slider":888.0,"/Test/array":[1.0,2.0,3.0]})JSON") == string(OStream::obuffer.GetString()));

    OStream::obuffer.Clear();

    // following setting the previous values...
    tc.inputs.some_text = string("bar");
    tc.inputs.my_slider.value = 777;
    tc.inputs.my_array.value = std::array{11.0f,22.0f,33.0f};
    storage.external_destinations(tc);
    CHECK(string("bar") == string(storage.json["/Test/text"].GetString()));
    CHECK(777.0 == storage.json["/Test/slider"].GetDouble());
    CHECK(string(R"JSON({"/Test/text":"bar","/Test/slider":777.0,"/Test/array":[11.0,22.0,33.0]})JSON") == string(OStream::obuffer.GetString()));
}

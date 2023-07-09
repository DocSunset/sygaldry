#include <string>
#include <catch2/catch_test_macros.hpp>
#include <rapidjson/stream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "helpers/endpoints.hpp"
#include "bindings/rapidjson.hpp"

using std::string;
using namespace sygaldry;
using namespace sygaldry::bindings;

struct test_component_t
: name_<"Test">
{
    struct inputs_t {
        text_message<"text", "description goes here", tag_session_data> some_text;
        slider<"slider", "description goes here", float, 0.0f, 1.0f, 0.0f, tag_session_data> my_slider;
    } inputs;

    void main() {}
} test_component;
using TestStorage = RapidJsonSessionStorage<rapidjson::StringStream, rapidjson::Writer<rapidjson::StringBuffer>, decltype(test_component)>;
TEST_CASE("RapidJSON creates object given empty input stream")
{
    string ibuffer{""};
    rapidjson::StringStream istream{ibuffer.c_str()};
    TestStorage storage{};
    storage.init(istream, test_component);
    CHECK(storage.json.IsObject());
    REQUIRE(storage.json.ObjectEmpty());
}
TEST_CASE("RapidJSON sets endpoints based on input stream")
{
    string ibuffer{
R"JSON(
{ "/Test/text" : "hello world"
, "/Test/slider" : 42.0
})JSON"};
    rapidjson::StringStream istream{ibuffer.c_str()};
    TestStorage storage{};
    test_component_t tc{};
    storage.init(istream, tc);
    CHECK(tc.inputs.some_text.value() == string("hello world"));
    CHECK(tc.inputs.my_slider.value == 42.0f);
}
TEST_CASE("RapidJSON main")
{
    string ibuffer{""};
    rapidjson::StringStream istream{ibuffer.c_str()};
    rapidjson::StringBuffer obuffer{};
    rapidjson::Writer<decltype(obuffer)> ostream{obuffer};
    TestStorage storage{};
    test_component_t tc{};
    storage.init(istream, tc);

    tc.inputs.some_text = string("foo");
    tc.inputs.my_slider.value = 888;
    storage.main(ostream, tc);
    CHECK(storage.json.HasMember("/Test/text"));
    CHECK(storage.json.HasMember("/Test/slider"));
    CHECK(storage.json["/Test/text"].IsString());
    CHECK(storage.json["/Test/slider"].IsDouble());
    CHECK(string("foo") == string(storage.json["/Test/text"].GetString()));
    CHECK(888.0 == storage.json["/Test/slider"].GetDouble());
    CHECK(string(R"JSON({"/Test/text":"foo","/Test/slider":888.0})JSON") == string(obuffer.GetString()));

    obuffer.Clear();
    ostream.Reset(obuffer);

    // following setting the previous values...
    tc.inputs.some_text = string("bar");
    tc.inputs.my_slider.value = 777;
    storage.main(ostream, tc);
    CHECK(string("bar") == string(storage.json["/Test/text"].GetString()));
    CHECK(777.0 == storage.json["/Test/slider"].GetDouble());
    CHECK(string(R"JSON({"/Test/text":"bar","/Test/slider":777.0})JSON") == string(obuffer.GetString()));
}

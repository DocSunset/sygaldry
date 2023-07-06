#include <string>
#include <catch2/catch_test_macros.hpp>
#include <rapidjson/stream.h>
#include <rapidjson/stringbuffer.h>
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
        slider<"slider", "description goes here", float, 0.0, tag_session_data> a_slider;
    } inputs;

    void main() {}
} test_component;
using TestStorage = RapidJsonSessionStorage<rapidjson::StringStream, rapidjson::StringBuffer, decltype(test_component)>
TEST_CASE("RapidJSON creates object given empty input stream")
{
    string ibuffer{""};
    rapidjson::StringStream istream{ibuffer.c_str()};
    TestStorage storage{};
    storage.init(istream, test_component);
    CHECK(storage.json.IsObject());
    REQUIRE(storage.json.ObjectEmpty());
}

#include <string_view>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "helpers/endpoints.hpp"
#include "bindings/osc_string_constants.hpp"

using std::string_view;
using namespace sygaldry;
using namespace sygaldry::bindings;

TEST_CASE("osc path")
{
    struct root_t { static _consteval const char * name() {return "Root";} };
    struct leaf_t { static _consteval const char * name() {return "leaf";} };
    using Path = std::tuple<root_t, leaf_t>;
    CHECK(string_view(osc_path<Path>::value.data()) == string_view("/Root/leaf"));
}

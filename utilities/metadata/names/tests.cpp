#include <catch2/catch_test_macros.hpp>
#include "names.hpp"
#include <string>
#include <array>

struct SimpleLowpass
{
    static consteval const char * name() {return "Simple Lowpass";}
    static consteval const char * camel_name() {return "SimpleLowpass";}
    static consteval const char * snake_name() {return "simple_lowpass";}
    static consteval const char * kebab_name() {return "simple-lowpass";}

    struct inputs_t
    {
        struct cutoff_t
        {
            static consteval const char * name() {return "cutoff frequency";}
            static consteval const char * camel_name() {return "cutoffFrequency";}
            static consteval const char * snake_name() {return "cutoff_frequency";}
            static consteval const char * kebab_name() {return "cutoff-frequency";}
            static consteval const char * osc_address() {return "SimpleLowpass/cutoff_frequency";}
            float value;
        } cutoff;
    } inputs;

    // low pass implementation here ...
};

using std::string;
using namespace utilities::metadata::names;

TEST_CASE("Snake Case")
{
    struct example_t
    {
        static consteval const char * name() {return "snake case example";}
    };

    REQUIRE(string(snake_case<example_t>) == string("snake_case_example"));
}
TEST_CASE("Kebab Case")
{
    struct example_t
    {
        static consteval const char * name() {return "kebab case example";}
    };

    REQUIRE(string(kebab_case<example_t>) == string("kebab-case-example"));
}

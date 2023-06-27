# Name Dispatch

Many bindings, including initially the CLI, and eventually perhaps also Pd, OSC,
MIDI 2.0, and others, need to match a name provided at run-time to a list of
named entities such as components and endpoints.

[TOC]

# Tests

The dispatcher must accept a name, group of entities, default return value, and
lambda function to apply when a matching name is found. The group of entities
is allowed to be a simple aggregate of entities (i.e. one that can be operated
on with `boost::pfr`), or a component with inputs and outputs that are simple
aggregates of entities. The callback function must be able to access and modify
the state of the matching entity or entities. TODO: this has yet to be
generalised to work with subassemblies and pure assembly components.

```cpp
// @#'tests/name_dispatch/tests.cpp'
#include <string_view>
#include <tuple>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"
#include "components/sensors/button.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/name_dispatch.hpp"

using std::string_view;

using namespace sygaldry;
using namespace sygaldry::bindings;

struct Named1 { static _consteval auto name() {return "name1";} int value;};
struct Named2 { static _consteval auto name() {return "a longer name";} float value;};
struct Named3 : name_<"name3">, persistent<int> {};
struct Named4 : name_<"name4">, persistent<float> {};

struct PseudoComponent1
{
    static _consteval auto name() {return "pseudo-component";}
    struct inputs_t {
        Named1 in1;
        Named3 in2;
    } inputs;
    struct outputs_t {
        Named2 out1;
        Named4 out2;
    } outputs;
    void main() {}
};

TEST_CASE("Dispatch over struct of endpoints", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    auto ret = dispatch("name1", x.inputs, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name1"));
    ret = dispatch("name3", x.inputs, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name3"));
}

TEST_CASE("Dispatch can mutate entities in struct of endpoints", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    REQUIRE(x.inputs.in1.value == 0);
    auto ret = dispatch("name1", x.inputs, 0, [](auto& n) {n.value = 42; return (int)n.value;});
    REQUIRE(ret == 42);
    REQUIRE(x.inputs.in1.value == 42);
    ret = dispatch("name3", x.inputs, 0, [](auto& n) {n.value = 888; return (int)n.value;});
    REQUIRE(ret == 888);
    REQUIRE(x.inputs.in2.value == 888);
}

TEST_CASE("Dispatch over component", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    auto ret = dispatch("name1", x, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name1"));
    ret = dispatch("name3", x, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name3"));
}

TEST_CASE("Dispatch can mutate entities component", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    REQUIRE(x.inputs.in1.value == 0);
    auto ret = dispatch("name1", x, 0, [](auto& n) {n.value = 42; return (int)n.value;});
    REQUIRE(ret == 42);
    REQUIRE(x.inputs.in1.value == 42);
    ret = dispatch("name3", x, 0, [](auto& n) {n.value = 888; return (int)n.value;});
    REQUIRE(ret == 888);
    REQUIRE(x.inputs.in2.value == 888);
}

@{tests}
// @/
```

```cmake
# @#'tests/name_dispatch/CMakeLists.txt'
add_executable(dispatch-tests tests.cpp)
target_link_libraries(dispatch-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(dispatch-tests)
# @/
```

# Matchers

To allow for idiomatic binding-specific spelling and matching rules, the
dispatcher accepts a functor type parameter that is used to confirm a match.
This matcher functor is pass the name string and the candidate named entity and
must return a bool. A default is provided that does simple string comparison.

```cpp
// @='default matcher'
struct dispatch_default_matcher
{
    template<typename stringish, typename NamedT>
    bool operator()(stringish name, const NamedT& candidate)
    {
        return std::string_view(name) == std::string_view(NamedT::name());
    }
};
// @/

// @+'tests'
struct CustomMatcher
{
    template<typename stringish, typename NamedT>
    bool operator()(stringish name, const NamedT& candidate)
    {
        return std::string_view(NamedT::name()) == std::string_view("name4");
    }
};

TEST_CASE("Dispatch with custom matcher", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    auto ret = dispatch<CustomMatcher>("name1", x, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name4"));
}
// @/
```

# Implementation

We assume that the entities are stored as a tuple. Because we may need to
short-circuit the iteration in case we find a matching entity, the only
adequate method for iterating over the commands is using a recursive variadic
function. This can't be a lambda, since it's obnoxious to try to `std::apply` a
tuple to a lambda that has to recursively call itself. Instead, we define a
template function for the purpose, and then apply a lambda that simply calls
this function with the tuple elements expanded into the latter.

```cpp
// @='dispatch implementation'
template<typename Matcher, typename stringish, typename Default, typename Callback, typename NamedT, typename ... NamedTs>
auto _dispatch_impl(stringish name, Default&& d, Callback&& f, NamedT&& t, NamedTs&&... ts)
{
    if (Matcher{}(name, t))
        return f(t);
    else if constexpr (sizeof...(NamedTs) == 0)
        return d;
    else
        return _dispatch_impl<Matcher>(name, d, f, ts...);
}

template <typename Matcher = dispatch_default_matcher, typename stringish, typename TupleOfNamed, typename Default, typename Callback>
auto dispatch_tuple(stringish name, TupleOfNamed& tup, Default&& d, Callback&& f)
{
    if constexpr (std::tuple_size_v<TupleOfNamed> == 0) return d;
    else return std::apply([&](auto&& ... ts)
    {
        return _dispatch_impl<Matcher>(name, d, f, ts...);
    }, tup);
}
// @/
```

Since the caller passes in a simple aggregate of entities or a component with
inputs and outputs, we need to convert the group into a tuple so that we can
pass it to the above `dispatch_tuple` subroutine. An entry point is provided
that extracts a tuple from the input appropriately and forwards it to the above
function.

Note that the tuples returned from boost::pfr::structure_tie *must* be stored
as lvalues, or else the compiler refuses to pass a reference to them into the
lower-level implementation subroutines.

```cpp
// @='dispatch entry point'
template <typename Matcher = dispatch_default_matcher, typename stringish, typename Entities, typename Default, typename Callback>
auto dispatch(stringish name, Entities& entities, Default&& d, Callback&& f)
{
    if constexpr (Component<Entities>)
    {
        if constexpr (has_inputs<Entities> && has_outputs<Entities>)
        {
            auto ins = boost::pfr::structure_tie(inputs_of(entities));
            auto outs = boost::pfr::structure_tie(outputs_of(entities));
            auto tup = std::tuple_cat(ins, outs);
            return dispatch_tuple<Matcher>(name, tup, d, f);
        }
        else if constexpr (has_inputs<Entities>)
        {
            auto tup = boost::pfr::structure_tie(inputs_of(entities));
            return dispatch_tuple<Matcher>(name, tup, d, f);
        }
        else if constexpr (has_outputs<Entities>)
        {
            auto tup = boost::pfr::structure_tie(outputs_of(entities));
            return dispatch_tuple<Matcher>(name, tup, d, f);
        }
        else return d;
    }
    else // Entities is assumed to be a struct of endpoints
    {
        auto tup = boost::pfr::structure_tie(entities);
        return dispatch_tuple<Matcher>(name, tup, d, f);
    }
}
// @/
```

```cpp
// @#'name_dispatch.hpp'
#pragma once
#include <tuple>
#include <string_view>
#include <boost/pfr.hpp>
#include "concepts/components.hpp"

namespace sygaldry { namespace bindings {

@{default matcher}

// TODO: these concept definitions probably don't belong here

template<typename T>
concept tuple_like = requires (T tup)
{
    std::tuple_size<T>::value;
    typename std::tuple_element<0, T>::type;

};

@{dispatch implementation}

@{dispatch entry point}

} }
// @/
```


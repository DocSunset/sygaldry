# Endpoint Helpers

This document is concerned with providing a convenient means for definine
endpoints that adhere to certain concepts. The concepts themselves, along with
generic functions for accessing the signals and metadata associated with
endpoints, are defined seperately in `endpoints/concepts.lili`. It is advised
to read this document first.

[TOC]

# Endpoint Bases

There are many common kinds of endpoints in a digital musical instrument that
vary depending on the type of data sent or received from the endpoint and the
expected temporal semantics with which that data is sent or received. Some
kinds of endpoints are more common than others, so it's convenient to define
helper types to allow instances of these endpoints to be instantiated easily
and to ensure consistency between instances of similar endpoints.

Similarly, certain forms of metadata are shared across numerous different types
of endpoint. Metadata associated with endpoints can be used to guide binding
authors towards the intended interpretation of a component. Some metadata are
required, others are optional.

Considering Avendish as a point of reference, we can identify three different
physical (i.e. C++ source code-level) mechanisms for associating metadata with
a type. There are compile-time methods, such as a `static _consteval auto
name()` method that returns the epynomous label. There are sub-types with
standardized names and structures, such as a `struct range` with members `min`,
`max`, and `init`. And there are other names within the scope of the endpoint
(or component?) such as those introduced by a bare `enum` declaration, or a
`using` declared type alias.

Here we rely mainly on function calls, as this allows metadata and other common
features of endpoints to be composed through inheritance. This provides a
convenient declarative method for users to define their own endpoint types by
combining the basic building blocks themselves. In general, users are
anticipated to use the higher-level helpers and not the base classs, but the
latter remain available for users spinning up a custom type of endpoint.
In general, these helpers can also be used with components, e.g. `struct
mycomponent : name_<"Lowpass Filter">` or what have you.

Note that there is no problem with inheritance for endpoints or components, but
collections of endpoints such as the `inputs` and `outputs` structures of a
component are not allowed to have base classes. Such collections must be simple
aggregates in order to work with `boost::pfr`, which is our chosen reflection
method for the time being. The endpoints within these aggregates, however, have
no such restrictions, so component authors are, in principle, free to employ
whatever design methods they prefer for endpoints. Indeed, even a bare `float`
or similar is perfectly allowed, although it is challenging to associate
metadata with such an endpoint.

## Name and Other Text

Names and other textual metadata are addressed in
[the metadata helpers document](helpers/metadata.lili.md).

## Range and Initial Value

Many endpoints have a fixed range, such as the integral 0 to 127 of MIDI
control change messages. For compatibility with Avendish, we consider a default
initial value to be a part of the range of an endpoint, although arguably the
initial value should be seperate. Similar to the `name_` class, we define a
helper class that endpoints can inherit to specify their range.

Ideally, we would like to specify initial values and the minimum and maximum
boundaries of the range as non-type template parameters, e.g. `struct
myendpoint : range_<0.0f, 1.0f, 0.5f>`. Unfortunately, many compilers
such as `clang` still do not support floating point numbers as non-type
template parameters.

So it was that after going through several possible solutions--

```cpp
struct myendpoint : range_<0.0f, 1.0f> {} // compiler error with clang
struct myendpoint : range_<{0.0f, 1.0f}> {} // init list is not an expression
struct myendpoint : range_<range{0.0f, 1.0f} {}; // redundant and ugly
struct myendpoint : with<range{0.0f, 1.0f} {}; // not bad, but we can do better
```

--it occurred to me while proofreading
[the metadata helpers](helpers/metadata.lili.md) that the same strategy could
likely be employed with numbers as well as strings. Although this is redundant
for integral types, which are allowed in template parameters, it's a minor
tradeoff to accept both floats and integers in order to have a single way of
annotating a range. This gives us the ideal syntax in a way that doesn't upset
clang.

```cpp
// @+'endpoint bases'
template<typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;
template<arithmetic T>
struct num_literal
{
    using type = T;
    T value;
    _consteval num_literal(T f) : value{f} {}
    operator T() {return value;}
};
// @/

// @+'endpoint bases'
template<num_literal _min, num_literal _max, num_literal _init = _min>
struct range_
{
    static _consteval auto range()
    {
        struct {
            decltype(_min.value) min = _min.value;
            decltype(_max.value) max = _max.value;
            decltype(_init.value) init = _init.value;
        } r;
        return r;
    }
};
// @/

// @+'tests'
struct struct_with_range : range_<0, 127> {};
struct struct_with_init : range_<0.0f, 100.0f, 42.0f> {};

TEST_CASE("Range", "[endpoints][bases][range]")
{
    SECTION("With range")
    {
        REQUIRE(struct_with_range::range().min == 0);
        REQUIRE(struct_with_range::range().max == 127);
        REQUIRE(struct_with_range::range().init == 0);
    }
    SECTION("With init")
    {
        REQUIRE(struct_with_init::range().init == 42.0f);
    }
}
// @/
```

## Persistent Values

An endpoint that has a persistent value across activations of a component is
well modelled by storing the value as a persistent member variable. It's
convenient to store this inside a struct to enable metadata to be associated
with the value in the form of compile-time evaluated methods, types, and
enumerators, but this makes accessing the value of the variable cumbersome, as
in e.g. `outputs.parameter.value = inputs.parameter.value`. To ease this
discomfort, we wish to provide conversion and assignment operators so that an
instance of a value class can be treated directly as though it were the value
itself rather than a container for it, while also retaining the benefit of
providing a convenient containment point for metadata. We then wish for our
helpers to inherit these operators to provide the value semantics to
instantiations of the helpers.

Unfortunately, assignment operators in particular are tricky to inherit due to
C++'s automatically generated default assignment operators, which shadow the
explicitly defined ones on our persistent value base class. We can explictly
draw in the base class's assignment into the derived class with a `using`
declaration in public scope, e.g. `using persistent<T>::operator=`, but then
assignment to the derived class would return a reference to the base class.

Another issue with conversion and assignment operators is that they should
ideally be defined differently depending on whether the value type is trivial
or not; in case of trivial value type, it's likely cheaper not to define move
semantics, and to assign by copy instead of reference, whereas more complex
containers may benefit from having these.

For now we accept the tradeoffs of inherited assignment operators and ignore
the performance optimizations associated with trivial types.

```cpp
// @+'endpoint bases'
template <typename T>
struct persistent
{
    using type = T;
    T value;
    constexpr persistent() noexcept : value{} {}

    // convert from T
    constexpr persistent(T&& t) noexcept {value = std::move(t);}
    constexpr persistent(const T& t) noexcept : value{t} {}
    constexpr auto& operator=(T&& t) noexcept {value = std::move(t); return *this;}
    constexpr auto& operator=(const T& t) noexcept {value = t; return *this;}

    // convert to T
    constexpr operator T&() noexcept {return value;}
    constexpr operator const T&() const noexcept {return value;}
};
// @/

// @+'tests'
struct persistent_struct : persistent<int> {using persistent<int>::operator=;};
TEST_CASE("Persistent Value", "[endpoints][helpers][persistent]")
{
    auto s = persistent_struct{42};
    REQUIRE(s == 42);
    s = 88;
    REQUIRE(s == 88);
    static_assert(PersistentValue<persistent_struct>);
}
// @/
```


## Occasional Values

Sometimes updating a value should initiate activation of the processor, and
sometimes the processor outputs values that should be propagated immediately. A
convenient way to represent this, seen in Avendish, is using `std::optional` to
represent the temporal semantics of these updates. The platform is expected to
clear `std::optional` valued endpoints before activating a component so that it
can signal hot outputs and recognize hot inputs. Afterwards, the platform is
expected to inspect hot outputs and take appropriate action if any are present.
An important consequence of this semantics is that, unlike a persistent value
endpoint, the state of hot outputs is not expected to be unaltered by the
platform across invocations of a component, and so cannot be used to carry
state across invocations as with a persistent value endpoint.

Initially, we chose to wholesale subsume `std::optional` by defining our
occasional value class as an alias for the former. There were good reasons not
to do this, not least of all that it gave us no control over the API for our
optional values, but it was a lowly hack that got us moving on quickly.
However, the semantics just described, where the value of an occasional
output vanishes between invocations of the associated component, proved to
be more inconvenient than anticipated. For example, in
[the button gesture model](components/sensors/button.lili.md), the output
state is meant to reflect the debounced state of the button. In order to
recognize when the input state has changed, it needs to be compared with
the output. Since an occasional value implemented as `std::optional` has
its state cleared in between calls, this kind of comparison is not possible.

To support this kind of usage, in addition to the value of the endpoint, we
store a boolean flag that reflects whether the value has been updated. We then
implement `std::optional`-like semantics around these data members. So the
interpretation is not that our occasional type "occasionally has a value", but
that it "has a value that is occasionally updated." Given this, we also allow
our occasional value type to be converted freely to the type of its underlying
value, giving it value semantics. Although this makes it ill-formed to have
an `occasional<bool>`, we consider it an acceptable tradeoff.

Arguably the full `std::optional`-like interface is poorly matched to our
intention here, since it implies "occasionally has a value". We retain it
nevertheless with the assumption that doing so will improve the compatibility
of our components with Avendish.

```cpp
// @+'endpoint bases'
template <typename T>
struct occasional
{
    using type = T;
    T state;
    bool updated;

    constexpr occasional() noexcept : state{}, updated{false} {}
    constexpr occasional(occasional<T>&& other)
    {
        if (other.updated)
        {
            state = std::move(other.state);
            updated = true;
        } else updated = false;
    }
    constexpr occasional(const occasional<T>& other)
    {
        if (other.updated)
        {
            state = other.state;
            updated = true;
        } else updated = false;
    }
    constexpr auto& operator=(const occasional<T>& other)
    {
        if (other.updated)
        {
            state = other.state;
            updated = true;
        } else updated = false; // keep current value
        return *this;
    }
    constexpr operator T&() noexcept {return state;}
    constexpr operator const  T&() const noexcept {return state;}

    // optional-like semantics
    constexpr occasional(T&& t) noexcept : state{std::move(t)}, updated{true} {}
    constexpr occasional(const T& t) noexcept : state{t}, updated{true} {}
    constexpr auto& operator=(T&& t) noexcept {state = std::move(t); updated = true; return *this;}
    constexpr auto& operator=(const T& t) noexcept {state = t; updated = true; return *this;}
    constexpr operator bool() noexcept {return updated;}
    constexpr T& operator *() noexcept {return state;}
    constexpr const T& operator *() const noexcept {return state;}
    constexpr T* operator ->() noexcept {return &state;}
    constexpr const T* operator ->() const noexcept {return &state;}
    constexpr T& value() noexcept {return state;}
    constexpr const T& value() const noexcept {return state;}
    constexpr void reset() noexcept {updated = false;} // maintains current state
};

// @/

// @+'tests'
struct occasional_struct : occasional<int> { using occasional<int>::operator=; };
TEST_CASE("Occasional Value", "[endpoints][helpers][occasional]")
{
    auto s = occasional_struct{42};
    REQUIRE(bool(s) == true);
    REQUIRE(*s == 42);
    *s = 88;
    REQUIRE(*s == 88);
    s = decltype(s){};
    REQUIRE(bool(s) == false);
    static_assert(OccasionalValue<occasional_struct>);
}
// @/
```


## Tags

We can introduce arbitrary symbols into the scope of a class through a bare
`enum` declaration e.g. `enum { bang };` or `enum class bang {};`. Our bindings
can then be designed to look for such symbols, e.g. `if constexpr (requires
{endpoint::bang;}) { ... }` and behave depending on their presence or absence.
Although it's easy enough to add such a tag when defining a custom endpoint,
the following helpers are provided which attach tags with recognized value
within this project. The interpretation is given in
[the endpoints concepts document](concepts/endpoints.lili.md);

First we define structures that each have one recognized tag.

```cpp
// @+'helpers'
#define tag(TAG) struct tag_##TAG {enum {TAG}; }
tag(write_only);
tag(session_data);
#undef tag
// @/
```

Then we define a template class that inherits from all its type arguments:

```cpp
// @+'helpers'
template<typename ... Tags>
struct tagged_ : Tags... {};
// @/
```

Our endpoint helpers then inherit the `tagged_` class with their own
variadic template type argument packs, allowing users to add arbitary
combinations of tags to them. The resultant template instantiations
inherit the enumerations from the tag classes, allowing this kind of
metadata to be readily associated with them.

```cpp
// @+'tests'
struct tag_foo {enum {foo};};
struct tag_bar {enum {bar};};
template<typename ... Tags>
struct tag_helper_test : tagged_<Tags...> {};
tag_helper_test<> t1; // make sure this compiles
tag_helper_test<tag_foo> t2;
static_assert(t2.foo == tag_foo::foo);
tag_helper_test<tag_foo, tag_bar> t3;
static_assert(t3.foo == tag_foo::foo);
static_assert(t3.bar == tag_bar::bar);
// @/
```

# Basic Endpoints

Given the above ingredients, we now define some basic endpoints composed
from them.

```cpp
// @+'helpers'
template<string_literal name_str, string_literal desc = "", char init = 0, typename ... Tags>
struct button
: occasional<char>
, name_<name_str>
, description_<desc>
, range_<0, 1, init>
, tagged_<Tags...>
{
    using occasional<char>::operator=;
};

template<string_literal name_str, string_literal desc = "", char init = 0, typename ... Tags>
struct toggle
: persistent<char>
, name_<name_str>
, description_<desc>
, range_<0, 1, init>
, tagged_<Tags...>
{
    using persistent<char>::operator=;
};

template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct text
: persistent<std::string>
, name_<name_str>
, description_<desc>
, tagged_<Tags...>
{
    using persistent<std::string>::operator=;
};

template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct text_message
: occasional<std::string>
, name_<name_str>
, description_<desc>
, tagged_<Tags...>
{
    using occasional<std::string>::operator=;
};

template< string_literal name_str
        , string_literal desc = ""
        , arithmetic T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        , typename ... Tags
        >
struct slider
: persistent<T>
, name_<name_str>
, description_<desc>
, range_<min, max, init>
, tagged_<Tags...>
{
    using persistent<T>::operator=;
};

template< string_literal name_str
        , std::size_t N
        , string_literal desc = ""
        , arithmetic T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        , typename ... Tags
        >
struct vector
: persistent<std::array<T, N>>
, name_<name_str>
, description_<desc>
, range_<min, max, init>
, tagged_<Tags...>
{
    using persistent<std::array<T, N>>::operator=;
    constexpr auto& operator[](std::size_t i) noexcept
    {
        return persistent<std::array<T,N>>::value[i];
    }
    constexpr auto size() noexcept
    {
        return persistent<std::array<T,N>>::value.size();
    }
};
// @/

As all of the functionality of these endpoints has been tested above where the
base classes were defined, we take the opportunity to make sure that our helper
endpoints adhere to the expected concepts defined elsewhere rather than
redundantly testing their functionality. We also check that our helper
endpoints have the expected sizes, equivalent to their value types.

// @+'tests'
TEST_CASE("Basic Endpoints", "[endpoints][basic]")
{
    static_assert(OccasionalValue<button<"foo">>);
    static_assert(PersistentValue<toggle<"baz">>);
    static_assert(PersistentValue<slider<"baz">>);
    static_assert(has_range<button<"foo">>);
    static_assert(has_range<toggle<"bar">>);
    static_assert(has_range<slider<"baz">>);
    static_assert(has_name<button<"foo">>);
    static_assert(has_name<toggle<"bar">>);
    static_assert(has_name<slider<"baz">>);
    static_assert(OccasionalValue<button<"foo">&>);
    static_assert(PersistentValue<toggle<"bar">&>);
    static_assert(PersistentValue<slider<"baz">&>);
    static_assert(has_range<button<"foo">&>);
    static_assert(has_range<toggle<"bar">&>);
    static_assert(has_range<slider<"baz">&>);
    static_assert(has_name<button<"foo">&>);
    static_assert(has_name<toggle<"bar">&>);
    static_assert(has_name<slider<"baz">&>);
    static_assert(OccasionalValue<const button<"foo">&>);
    static_assert(PersistentValue<const toggle<"bar">&>);
    static_assert(PersistentValue<const slider<"baz">&>);
    static_assert(has_range<const button<"foo">&>);
    static_assert(has_range<const toggle<"bar">&>);
    static_assert(has_range<const slider<"baz">&>);
    static_assert(has_name<const button<"foo">&>);
    static_assert(has_name<const toggle<"bar">&>);
    static_assert(has_name<const slider<"baz">&>);
    static_assert(sizeof(button<"foo">) <= sizeof(std::optional<bool>));
    static_assert(sizeof(toggle<"baz">) == sizeof(bool));
    static_assert(sizeof(slider<"baz">) == sizeof(float));

    auto s1 = slider<"baz">{};
    auto s2 = slider<"baz">{0.5f};
    s2 = 0.0f;
}
// @/
```

## Bang

Often an endpoint doesn't carry any value, but merely serves to convey that
an event has occurred. In Max/MSP and Pure Data, this kind of temporal
impulsive event without data is called a *bang*. We adopt the same naming
convention here.

A bang could be represented as `std::optional` of an empty type, but
measurements suggest that this actually takes more space than a `bool`, so we
instead opt to treat a persistent value endpoint tagged with the symbol `bang` as
our preferred bang representation; this requires us to give our bang class a
different name, but we consider this an acceptable tradeoff.

Another possible approach would be to employ a callback semantics, so that
triggering the bang would immediately pass execution to a function registered
by the platform. For now, we opt to retain value semantics, despite that callback
semantics might allow for a more space efficient implementation. Callback
semantics place a greater burden on the binding author that we would prefer to
avoid at this point.

```cpp
// @+'helpers'
template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct bng
: persistent<bool>
, name_<name_str>
, description_<"">
, tagged_<Tags...>
{
    using persistent<bool>::operator=;
    enum {bang, impulse};
    void operator()() {value = true;}
    void reset() {value = false;}
};
// @/
```

```cpp
// @+'tests'
TEST_CASE("Bang", "[endpoints][bang]")
{
    auto b = bng<"foo">{};
    REQUIRE(bool(b) == false);
    REQUIRE(value_of(b) == false);
    b();
    REQUIRE(bool(b) == true);
    REQUIRE(value_of(b) == true);
    b.reset();
    REQUIRE(bool(b) == false);
    b = true;
    REQUIRE(bool(b) == true);
    b = {};
    REQUIRE(bool(b) == false);
    static_assert(Bang<decltype(b)>);
    static_assert(sizeof(decltype(b)) == sizeof(bool));
}
// @/
```

# Summary

```cpp
// @#'endpoints.hpp'
#pragma once

#include <string_view>
#include <string>
#include <array>
#include "utilities/consteval.hpp"
#include "helpers/metadata.hpp"

namespace sygaldry {

@{endpoint bases}

@{helpers}

} // namespaces
// @/

// @#'tests/endpoints/tests.cpp'

#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>
#include <optional>
#include <boost/pfr.hpp>
#include "concepts/metadata.hpp"
#include "concepts/endpoints.hpp"
#include "helpers/endpoints.hpp"

using namespace sygaldry;
using std::string_view;

@{tests}
// @/
```

```cmake
# @#'tests/endpoints/CMakeLists.txt'
add_executable(endpoints-tests tests.cpp)
target_link_libraries(endpoints-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(endpoints-tests)
# @/
```

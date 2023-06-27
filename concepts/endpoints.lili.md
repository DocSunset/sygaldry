# Endpoint Concepts

[TOC]

Although many components of digital musical instruments are commonly employed
across different designs, their implementation and representation in computer
programs tends to vary in incompatible ways. In `helpers/endpoints.lili`
we have defined a particular set of helper classes that allow components and
endpoints to be conveniently annotated with useful metadata that can guide
binding authors in how to interpret these entities. This document describes
a set of abstract concepts that components and endpoints are expected to
adhere to, as well as providing generic subroutines that allow the signals
and metadata from these entities to be accessed, even when the underlying
representation of this information may vary. It is recommended to read
this document second, after `helpers/endpoints.lili`.

In the interminable future, we can imagine that the whole music technology
community might band together to define a set of standard concepts that are
used to build digital musical applications, enabling widespread compatibility
between implementations of components and runtime environments. We aspire
towards this goal, but acknowledge the limited time and resources available to
this project's development, and compromise on the goal of totally generic
compatibility for the sake of completing other necessary objectives with the
available resources. This means that the concepts and accessors defined here
are expected to be fairly minimal, and the only representations that they are
intended to support as those used in this project. Nevertheless, these
reprsentations are developed with widespread applicability in mind, and it is
hoped that the overall approach can still demonstrate the advantages of this
generic component-oriented development model.

# Information Representations

As discussed in `endpoints.lili`, there are three main methods of associating
information with an endpoint or component: through its member functions,
through its member variables, and through its member types (including
enumerations). In all three cases, the format of the information is as
boundless as what can be represented by a C++ structure with its own methods,
data, and types. However, in most practical cases, it is sufficient to use
methods and enumerators, and their the names of these things within a class.
Further, of all the built in types available, in most cases it suffices to use
only string literals, signed and unsigned integers of 32 or 64 bits, floats,
doubles, and booleans, returned from methods. For a few common special cases,
it makes sense to use structures of the above types, and it is also helpful to
make use of `std::optional` as described in `endpoints.lili`. For our
purposes, we focus on these resources. Eventually, it will be useful to augment
the above with complex numbers, vectors, and even matrices and tensors as
primitive types, as well as function objects and other means of passing around
subroutines and the thread of execution. But as a starting point, the basic
representations described are enough to achieve a great deal of useful work.

# Spelling Convention

As concepts are a relatively new addition to C++, there aren't necessarily as
strong conventions established for their typography. Here we adopt the
following approximate rules of thumb: A concept that establishes a type `T` as
having a certain property is written in lower snake case, e.g. `has_name<T>`. A
concept that establishes T as being alike to certain other type or typical API
is written the same, e.g. `tuple_like`. Finally, if a concept establishes that
a certain T is, for all intents and purposes, an instance of a certain sort of
object, we use camel case, as in `OccasionalValue` or `Component`. As in the
rest of the project, a leading underscore is used to signify a private
implementation detail that the user should likely not access.

Recap:

- has a: `has_name`
- alike: `tuple_like`
- is a: `PersistentValue`
- private: `_not_yours`

# Name and Other Text

Accessors for names and other textual metadata are defined in
`concepts/metadata.lili`

# Range

A range is represented by a structure with min, max, and init member variables
of the same underlying type. An entity is considered to have a range if it
possesses a static member function that returns such a structure. The use
of `std::decay_t` and the accessor subroutines follow the logic seen in the
previous section.

```cpp
// @+'concepts'
template<typename T>
concept has_range = requires
{
    std::decay_t<T>::range().min;
    std::decay_t<T>::range().max;
    std::decay_t<T>::range().init;
};

template<has_range T>
constexpr auto get_range(const T&) { return T::range(); }

template<has_range T>
_consteval auto get_range() { return std::decay_t<T>::range(); }
// @/

// @+'tests'
struct struct_with_range : range_<0, 127> {};
struct struct_with_init : range_<0.0f, 100.0f, 42.0f> {};
TEST_CASE("has_range", "[components][concepts][has_range]")
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
// @/
```

# Persistent Values

A persistent value is one that should remain the same between invocations of a
component's main subroutine. For now, we assume that a persistent value
endpoint will be implemented such that is has a `value` variable or method that
returns something that has a type that the endpoint can be treated as an
instance of due to its conversion and assignment operators.

We first model the notion that an endpoint can be treated as an instance of
its value type with the concept `similar_to`, which asserts that for an endpoint
type `T` with value type `Y`, `T` can be converted to a `Y`, initialized from
one, or assigned from one, and that the result of said assignment can be used
to assign another endoint of the same type.

We could just as well have used the standard concepts `convertible_to`,
`constructible_from`, and `assignable_from`, but these make more strict
assertions that don't necessarily align with every hack and workaround that
component authors may use to meet the described requirements. For instance, the
fact that assignment operators are inherited from the `persistent<T>` base
class helper in the endpoint helpers defined in `endpoints.lili` is in
violation of the requirements of `assignable_from`, which requires the
assignment to `T` to return a reference to `T` (and not one of its bases).

Indeed, the stated requirements are even perhaps too strict for our purposes.
We are willing to fly fast and loose here, so our final `similar_to` concept
also ignores any cv-ref qualifications on the types it checks. This makes
it easier to use the concept with `decltype`, which in a function will
faithfully report the cv-ref qualifications of an argument type even if
those break our concepts.

We check the validity of our concept by making sure that a `float` is similar
to a `float`, and that a `const char *` is not.

```cpp
// @+'concepts'
template <typename T, typename Y>
concept strict_similar_to 
    =  requires (T t) { static_cast<Y>(t); }
    && requires (Y y) { T{y}; }
    && requires (T t, Y y) { t = y; }
    && requires (T a, T b, Y c) { a = b = c; }
    ;

template<typename T, typename Y>
concept similar_to = strict_similar_to<std::remove_cvref_t<T>, std::remove_cvref_t<Y>>;
// @/

// @+'persistent value tests'
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
// @/
```

Recalling our earlier summary, we say that an endpoint has a value if it has a
member variable `value` or a member function `value()` whose return type the
endpoint can be treated as an instance of. We additionally require that
persistent values be default initializable, e.g. so that binding authors do not
have to fuss over passing such values any constructor arguments.

```cpp
// @+'concepts'
template <typename T>
concept has_value_variable = requires (T t)
{
    t.value;
    requires similar_to<T, decltype(t.value)>;
};

template <typename T>
concept has_value_method = requires (T t)
{
    t.value();
    requires similar_to<T, decltype(t.value())>;
};

template <typename T>
concept PersistentValue
    =  has_value_variable<T> || has_value_method<T>
    && std::default_initializable<std::remove_cvref_t<T>>;
// @/

// @+'tests'
TEST_CASE("PersistentValue", "[components][concepts][PersistentValue]")
{
    @{persistent value tests}

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
// @/
```

# Occasional Values

An endpoint is considered to be an occasional value if it has pointer semantics
and can be initialized by an instance of its value type, such as a
`std::optional`. We also rely on a definition that we will examine later when
we define concepts for our `bang` type.

Note that occasional values are not values in the sense defined above. They do
not behave as though they are instances of their value type, since their
current state is meant to be checked before accessing their values.

```cpp
// @+'concepts'
@{flag}

template<typename T>
concept _occasional_value = requires (T t)
{
    *t;
    T{*t};
    *t = *t;
} && Flag<T>;

template<typename T>
concept OccasionalValue = _occasional_value<T> || _occasional_value<std::decay_t<T>>;
// @/

// @+'tests'
TEST_CASE("OccasionalValue", "[components][concepts][OccasionalValue]")
{
    static_assert(OccasionalValue<std::optional<float>>);
    static_assert(OccasionalValue<occasional<float>>);
}
// @/
```

# Bang

Our `bang` concept essentially just checks that the type has an enum value
called `bang` in its scope, and that it is a flag, a concept we also required
for our `OccasionalValue` concept.

```cpp
// @+'concepts'
template<typename T>
concept Bang = requires (T t)
{
    requires std::is_enum_v<decltype(T::bang)>;
} && Flag<T>;
// @/

// @+'tests'
TEST_CASE("Bang", "[components][concepts][Bang]")
{
    static_assert(Bang<bng<"foo">>);
    static_assert(has_value<bng<"foo">>);
}
// @/
```

# Flags

The `Flag` concept requires that we can convert a type to bool, and that the
boolean interpretation of a default constructed value of the type is `false`.
This is true of `std::optional`, `bool`, and also pointer types. We assume that
by assigning a value of this type to a default-constructed value that we can
set its boolean interpretation to `false`, and check at least that constructing
the type in this way has the expected effect. This allows us to define a
generic method for clearing values that meet the requirements of `Flag`. For a
flag to be considered `Clearable`, we require that it model `OccasionalValue`
or `Bang`; these are presently the only supported types with event-like
semantics, and that should therefore be cleared by the platform. The
`ClearableFlag` mechanism support binding authors in detecting and serving this
requirement. A client using e.g. the `bng` class defined in `endpoints.lili`
may prefer to use a different, perhaps more expressive, method of clearing its
state. But for a binding author, it's useful to have a way to clear a flag
without having to know anything about it other than that it is a valid flag.

```cpp
// @='flag'
template<typename T>
concept Flag = requires (T t)
{
    bool(t);
    requires std::is_same_v<std::integral_constant<bool, bool(T{})>, std::false_type>;
    requires std::is_same_v<std::integral_constant<bool, bool(T{T{}})>, std::false_type>;
    t = T{};
};
// @/

// @+'concepts'
template<typename T>
concept ClearableFlag = Flag<T> && (OccasionalValue<T> || Bang<T>);

template<ClearableFlag T>
constexpr void clear_flag(T& t)
{
    t = T{};
}
// @/

// @+'tests'
TEST_CASE("ClearableFlag", "[components][concepts][ClearableFlag]")
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
            REQUIRE(bool(flag));
            clear_flag(flag);
            REQUIRE(not bool(flag));
        }
        SECTION("OccasionalValue")
        {
            occasional<float> flag = 100.0f;
            REQUIRE(bool(flag));
            clear_flag(flag);
            REQUIRE(not bool(flag));
        }
    }
}
// @/
```

# Value Access

In case the accepted implementations of the above value types even changes, we
encourage binding authors to access values using the following generic
subroutines. `value_of` returns an appropriately `const` qualified reference to
the value of an endpoint, allowing the binding author to read and write the
value. To allow setting of `OccasionalValue`s that don't already have a value,
`set_value` is provided, which handles `OccasionalValue`s appropriately and
is also usable with `PersistentValue`s.

```cpp
// @+'tests'
TEST_CASE("Value Access", "[components][concepts][value_of][clear_flag]")
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
            REQUIRE(not v);
            set_value(v, 200.0f);
            REQUIRE(v);
            REQUIRE(value_of(v) == 200.0f);
        }
    }
}
// @/
```

We define a new concept `Value` as the union of occasional and persistent
values. The `value_of` subroutine itself is reasonably straightforward given
the above concepts. The only thing that might be surprising is that the `const`
version of the function is implemented in terms of the non-`const` version.
This is considered a reasonable and idiomatic way of avoiding repeating
ourselves, but we should remain suspicious of this function in case our
assumptions about const-correctness ever seen to be violated. This is unlikely
to ever be an issue though.


```cpp
// @+'concepts'
template <typename T>
concept has_value = OccasionalValue<T> || PersistentValue<T>;

template <has_value T>
auto& value_of(T& v)
{
    static_assert(PersistentValue<T> || OccasionalValue<T>, "value_of: Neither PersistentValue nor OccasionalValue. Did we add a new kind?");
    if constexpr (PersistentValue<T>)
    {
        static_assert(has_value_variable<T> || has_value_method<T>, "value_of: PersistentValue with no method or variable. Did we add a new kind?");
        if constexpr (has_value_variable<T>) return v.value;
        else if constexpr (has_value_method<T>) return v.value();
        else return 0;
    }
    else if constexpr (OccasionalValue<T>)
    {
        return *v;
    }
    else return 0;
}

template<has_value T>
const auto& value_of(const T& v)
{
    return value_of(const_cast<T&>(v));
}

template <has_value T>
auto& set_value(T& v, const auto& arg)
{
    v = arg;
    return v;
}
// @/
```

# Value Reflection

As well as numerical values, we also have helpers for string values. We provide
a basic concept to help identify them.

```cpp
// @+'concepts'
template<has_value T>
using value_t = std::remove_cvref_t<decltype(value_of(std::declval<T&>()))>;

template<typename T> concept string_like = requires (T t, const char * s)
{
    t = s;
};
// @/
```

# Tags

We can introduce arbitrary symbols into the scope of a class through a bare
`enum` declaration e.g. `enum { bang };`. We provide accessors for checking
if a symbol with a known meaning is defined in a class's scope.

```cpp
// @+'concepts'
#define tagged(TAG) template<typename T> concept tagged_##TAG\
    =  std::is_enum_v<decltype(T::TAG)>\
    || std::is_enum_v<typename T::TAG>
tagged(write_only); // don't display or output this endpoint's value
tagged(session_data); // store this endpoint's value across sessions, e.g. across power cycles on an embedded system
#undef tagged
// @/
```

# Summary

```cpp
// @#'endpoints.hpp'
#pragma once

#include <concepts>
#include "utilities/consteval.hpp"

namespace sygaldry {

@{concepts}

} // namespaces
// @/

// @#'tests/endpoints/tests.cpp'

#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <string>
#include <type_traits>
#include <boost/pfr.hpp>
#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"
#include "concepts/endpoints.hpp"

using namespace sygaldry;
using std::string_view;
using std::string;

@{tests}
// @/
```

```cmake
# @#'tests/endpoints/CMakeLists.txt'
add_executable(concepts-endpoints-tests tests.cpp)
target_link_libraries(concepts-endpoints-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(concepts-endpoints-tests)
# @/
```

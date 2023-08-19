\page page-sygac-components Components Concepts

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

We would like to statically determine whether a given type describes a
component. We consider two kinds of general components: regular components, and
component containers.

A regular component has a name and at least one of the following: an
initialization subroutine, a main subroutine, an external sources or
destinations subroutine, inputs, outputs, or parts. If it has parts,
a component's parts must also be valid components.

A component container is merely an aggregate that contains only nested
containers and/or regular components. It may have a name, but it must be
declared directly in the container definition in order to preserve
aggregate-ness (e.g. it must not use the inherited `name_<"name">` helper).
Component containers, as well as parts structures, are allowed to be empty,
contain nested component containers, and contain components. These requirements
are addressed in `validate_container`.

Whereas a regular component's functionality is defined by its main subroutine,
and its subcomponents should not activated by the runtime platform bindings,
containers are defined in terms of their subcomponents, which should be treated
as regular components by the runtime platform bindings, and activated
accordingly.

```cpp
// @='Component Concepts'
@{SimpleAggregate}

@{has_main_subroutine}

@{inputs and outputs and parts container}

@{throughpoints and plugins}

template<typename T>
concept ComponentBasics
    =  has_name<T>
    &&  (  has_init_subroutine<T>
        || has_external_sources_subroutine<T>
        || has_main_subroutine<T>
        || has_external_destinations_subroutine<T>
        || has_inputs<T>
        || has_outputs<T>
        || has_parts<T>
        )
    ;

template<typename T> struct validate_general_component : std::false_type {};

template<typename T>
    requires ComponentBasics<T> && (not has_parts<T>)
struct validate_general_component<T> : std::true_type {};

template<typename T>
    requires ComponentBasics<T> && (has_parts<T>)
struct validate_general_component<T> : validate_general_component<parts_t<T>> {};

@{validate_container}

template<typename T>
concept GeneralComponent = validate_general_component<T>::value;

template<typename T> concept Component = ComponentBasics<T> && GeneralComponent<T>;
template<typename T> concept ComponentContainer = (not ComponentBasics<T>) && GeneralComponent<T>;
// @/

// @+'tests'
struct regular_component_t : name_<"regular component">
{
    struct inputs_t {} inputs;
    struct outputs_t {} outputs;
    struct parts_t {
        struct subcomponent_t : name_<"a subcomponent"> {
            struct inputs_t {} inputs;
            void operator()() {}
        } subcomponent;
    } parts;
    static void main(const inputs_t&, outputs_t&) {}
} regular_component;

static_assert(has_main_subroutine<regular_component_t>);
static_assert(has_inputs<regular_component_t>);
static_assert(has_outputs<regular_component_t>);
static_assert(has_parts<regular_component_t>);
static_assert(Component<regular_component_t>);
static_assert(Component<regular_component_t>);

struct container_component_t
{
    static constexpr auto name() {return "container component";}
    regular_component_t component1;
} container_component;

static_assert(SimpleAggregate<container_component_t>);
static_assert(ComponentContainer<container_component_t>);
static_assert(not Component<container_component_t>);

static_assert(not ComponentContainer<regular_component_t>);
static_assert(ComponentContainer<regular_component_t::parts_t>);
// @/
```

# Simple Aggregate

Component containers, as well as the input, output, and parts structures of
regular components, are required to be simple aggregates. Our notion of a
simple aggregate comes from `boost::pfr`, which we rely on for the ability to
iterate over lists of endpoints and components in the form of structures, an
essential functionality for our purposes. We would ideally defer to
`boost::pfr` for our test of simple aggregate requirements, but a
straightforward concept--

```cpp
template<typename T>
concept SimpleAggregate = requires { boost::pfr::tuple_size_v<T>; };
```

--does not provide the required results. It appears as though the compiler
considers this to be a valid expression regardless of the result, i.e. even if
pfr itself would raise several (overly verbose and difficult to debug) static
assertion failures with the given `T`.

Attempts to defer to `std::is_aggregate_v<T>` in a similar fashion were also
unsatisfactory, with various test structures passing the test even through
`boost::pfr` rejects them at compile time through static assertions. It doesn't
help that `boost::pfr`'s implementation and documentation don't always agree on
what the requirements are:

```cpp
// @+'tests'
// docs say: aggregates may not have base classes
struct not_simple_aggregate1 : name_<"foo"> { };
static_assert(std::is_aggregate_v<not_simple_aggregate1>); // passes
// auto failure = boost::pfr::tuple_size_v<not_simple_aggregate1>; // static assertion failure

// docs say: aggregates may not have const fields
struct not_simple_aggregate2 { const int i; };
static_assert(std::is_aggregate_v<not_simple_aggregate2>); // passes
static_assert(1 == boost::pfr::tuple_size_v<not_simple_aggregate2>); // works fine, even though the docs say it's not allowed

// docs say: aggregates may not have reference fields
struct not_simple_aggregate3 { int& i; };
static_assert(std::is_aggregate_v<not_simple_aggregate3>); // passes
static_assert(1 == boost::pfr::tuple_size_v<not_simple_aggregate3>); // works fine, even though the docs say it's not allowed

// docs say: aggregates may not have c arrays
struct not_simple_aggregate4 { float f; int i[5]; };
static_assert(std::is_aggregate_v<not_simple_aggregate4>); // passes
static_assert(6 == boost::pfr::tuple_size_v<not_simple_aggregate4>); // works fine, even though the docs say it's not allowed, although arguably this should return 2?

// docs say: aggregates may not have constructors
struct not_simple_aggregate5 { float f; not_simple_aggregate5(float a, float b) : f{a + b} {} };
static_assert(not std::is_aggregate_v<not_simple_aggregate5>); // a class with a constructor is not aggregate
//auto failure = boost::pfr::tuple_size_v<not_simple_aggregate5>; // static assertion failure

// docs don't mention scalars, but implementation suggests they're fine
using not_simple_aggregate6 = float;
static_assert(not std::is_aggregate_v<not_simple_aggregate6>); // a float is not aggregate
static_assert(std::is_scalar_v<not_simple_aggregate6>); // a float is scalar
static_assert(1 == boost::pfr::tuple_size_v<not_simple_aggregate6>); // works, in accordance with the docs

// docs say: unions not allowed
union not_simple_aggregate7 {float f; int i;} nope;
static_assert(std::is_aggregate_v<not_simple_aggregate7>); // passes
// auto failure = boost::pfr::structure_to_tuple(nope); // static assertion failure
// @/
```

In most cases, `boost::pfr` seems to work when `std::is_aggregate_v` is true,
and to fail when it is not. The only exceptions we noted in our tests are when
`T` is scalar, such as a single `float`, when `T` is a union, and when `T` has
any base class. We can easily detect when `T` is scalar or union using
`std::is_scalar` and `std::is_union` respectively. However, it is challenging
to detect when `T` has a base class. For now, we defer this issue to
`boost::pfr`, which will in any case raise a static assertion failure if we try
to pass it a type with a base class, giving us the following
incomplete-but-likely-sufficient implementation of our `SimpleAggregate` concept:

```cpp
// @='SimpleAggregate'
template<typename T>
concept SimpleAggregate
    =  not std::is_union_v<T>
    && (  std::is_aggregate_v<T>
       || std::is_scalar_v<T>
       )
    ;
// @/

// @+'tests'
static_assert(SimpleAggregate<regular_component_t::inputs_t>);
static_assert(SimpleAggregate<regular_component_t::outputs_t>);
static_assert(SimpleAggregate<regular_component_t::parts_t>);
static_assert(SimpleAggregate<container_component_t>);
// @/
```

The main limitation we encounter due to our incomplete test of
simple-aggregate-ness is that we cannot admit nested aggregates of endpoints in
case any of our endpoints have inherited a class type; such non-base endpoints
pass our `SimpleAggregate` concept, but trigger static assertion failures from
`boost::pfr`. For this reason, we currently assume that endpoint containers are
not nested.

# Component Basics

Using our small [function reflection library](concepts/functions.lili) we can
tell whether a component has a main subroutine by checking whether the return
type of the expected methods (`T::main` or `T::operator()`) have the expected
type (void). This concept will not be satisfied if `T::main` is a variable,
since function reflection is impossible in this case and checking the return
type is thus an error.

As well as detecting the existence of a component's main subroutine, it
may also be necessary to access its type for function reflection. We provide
simple reflection template structs inheriting most of their functionality
from the `function_reflection` template developed in
[the function concepts document](concepts/functions.lili.md).

```cpp
// @+'has_main_subroutine'
template <typename T> struct main_subroutine_reflection {using exists = std::false_type;};

template <typename T>
    requires std::same_as<void, typename function_reflection<&T::operator()>::return_type>
struct main_subroutine_reflection<T> : function_reflection<&T::operator()> {};

template <typename T>
    requires std::same_as<void, typename function_reflection<&T::main>::return_type>
struct main_subroutine_reflection<T> : function_reflection<&T::main> {};
// @/
```

Similarly, we may want to access function reflection for a component's init and other
recognized subroutines:

```cpp
// TODO: this section needs a prose rewrite
// @+'has_main_subroutine'
template <typename T> struct init_subroutine_reflection {using exists = std::false_type;};

template <typename T>
    requires std::same_as<void, typename function_reflection<&T::init>::return_type>
struct init_subroutine_reflection<T> : function_reflection<&T::init> {};

template <typename T> struct external_sources_subroutine_reflection {using exists = std::false_type;};

template <typename T>
    requires std::same_as<void, typename function_reflection<&T::external_sources>::return_type>
struct external_sources_subroutine_reflection<T> : function_reflection<&T::external_sources> {};

template <typename T> struct external_destinations_subroutine_reflection {using exists = std::false_type;};

template <typename T>
    requires std::same_as<void, typename function_reflection<&T::external_destinations>::return_type>
struct external_destinations_subroutine_reflection<T> : function_reflection<&T::external_destinations> {};
// @/
```

Concepts for checking the existence of these subroutines can be implemented very similarly.

```cpp
// @+'has_main_subroutine'
template<typename T>
concept has_main_subroutine // = main_subroutine_reflection<T>::exists::value;
    =  std::same_as<void, typename function_reflection<&T::operator()>::return_type>
    || std::same_as<void, typename function_reflection<&T::main>::return_type>
    ;
template<typename T>
concept has_init_subroutine
    = std::same_as<void, typename function_reflection<&T::init>::return_type>;
template<typename T>
concept has_external_sources_subroutine
    = std::same_as<void, typename function_reflection<&T::external_sources>::return_type>;
template<typename T>
concept has_external_destinations_subroutine
    = std::same_as<void, typename function_reflection<&T::external_destinations>::return_type>;
// @/

// @+'tests'
struct void_main { void main() {} };
struct void_operator { void operator()() {} };
static_assert(has_main_subroutine<void_main>);
static_assert(has_main_subroutine<void_operator>);

struct member_main { int main; };
struct int_main { int main() {return 1;} };
struct int_operator { int operator()() {return 1;} };
static_assert(not has_main_subroutine<member_main>);
static_assert(not has_main_subroutine<int_main>);
static_assert(not has_main_subroutine<int_operator>);
// @/
```

Along with the `has_name` concept defined in
[the metadata concept header](concepts/metadata.lili.md),
this completes the specification of basic component requirements.

## Accessing the main subroutine type

# Inputs and Outputs

To detect whether a component has parts, inputs, or outputs, we employ the same
structure parameterized on the expected name of the nested simple aggregate
structure. We use a macro to avoid repetition. A similar pattern is used
[to define `has_name`](concepts/metadata.lili).

```cpp
// @='inputs and outputs and parts container'
#define has_type_or_value(NAME)\
template<typename T> concept has_##NAME = requires (T t)\
{\
    t.NAME;\
    requires SimpleAggregate<decltype(t.NAME)>;\
};\
\
template<has_##NAME T> struct type_of_##NAME\
{\
    using type = decltype(std::declval<T>().NAME);\
};\
\
template<typename T> using NAME##_t = typename type_of_##NAME<T>::type;\
\
template<typename T> requires has_##NAME<T> constexpr auto& NAME##_of(T& t) { return t.NAME; }\
template<typename T> requires has_##NAME<T> constexpr const auto& NAME##_of(const T& t) { return t.NAME; }\

has_type_or_value(inputs);
has_type_or_value(outputs);
has_type_or_value(parts);

#undef has_type_or_value
// @/

// @+'tests'
static_assert(std::same_as<regular_component_t::inputs_t&, decltype(inputs_of(regular_component))>);
static_assert(std::same_as<regular_component_t::outputs_t&, decltype(outputs_of(regular_component))>);
static_assert(std::same_as<regular_component_t::parts_t&, decltype(parts_of(regular_component))>);

static_assert(std::same_as<regular_component_t::inputs_t, inputs_t<regular_component_t>>);
static_assert(std::same_as<regular_component_t::outputs_t, outputs_t<regular_component_t>>);
static_assert(std::same_as<regular_component_t::parts_t, parts_t<regular_component_t>>);
// @/
```

## Throughpoints

TODO

## Plugins

TODO

## Validating Containers

```cpp
// @='validate_container'
template<typename T>
    requires std::is_scalar_v<T>
struct validate_general_component<T> : std::false_type {};

template<typename T>
    requires SimpleAggregate<T> && (not ComponentBasics<T>) && (not std::is_scalar_v<T>)
struct validate_general_component<T>
{
    using tup = decltype(boost::pfr::structure_to_tuple(std::declval<T&>()));
    using valid_components = boost::mp11::mp_transform<validate_general_component, tup>;
    using all_valid = boost::mp11::mp_apply<boost::mp11::mp_all, valid_components>;
    static constexpr bool value = all_valid::value;
};
// @/

// @+'tests'
struct almost_container
{
    float nope;
    regular_component_t yep;
};
static_assert(not ComponentContainer<almost_container>);
static_assert(ComponentContainer<regular_component_t::parts_t>);
static_assert(ComponentContainer<container_component_t>);
// @/
```

# Accessing Subcomponents and Endpoints

Given a list of components, such as a parts struct or container component,
there are several common operations that are required: apply a generic function
(especially a lambda) to every every node in the component tree, or a subset of
nodes of the same type; access a tuple of references to a certain type of nodes
in the tree; and find a certain node in the tree, based on its type.

## Nodes

Consider a container of components:

```cpp
// @+'tests'
struct c1_t : name_<"c1"> {
    struct inputs_t {
        struct in1_t : name_<"in1">, persistent<float>
        {
            float extra_value;
        } in1;
        struct in2_t : name_<"in2">, persistent<float>
        {
            float extra_value;
        } in2;
    } inputs;

    struct outputs_t {
        struct out_t : name_<"out">, persistent<float>
        {
            float another_extra;
        } out;
    } outputs;

    struct parts_t {
        struct dummy_part : name_<"dp"> {
            struct parts_t {
                static constexpr auto name() {return "dpp";}
            } parts;
            void main() {};
        } part;
    } parts;

    void main(){}
};

struct c2_t : name_<"c2"> {
    struct inputs_t {
        struct in1_t : name_<"in1">, persistent<float>
        {
            float extra_value;
        } in1;
        struct in2_t : name_<"in2">, persistent<float>
        {
            float extra_value;
        } in2;
    } inputs;

    struct outputs_t {
        struct out_t : name_<"out">, persistent<float>
        {
            float another_extra;
        } out;
    } outputs;

    struct parts_t {
        struct dummy_part : name_<"dp"> {
            struct parts_t {
                static constexpr auto name() {return "dpp";}
            } parts;
            void main() {};
        } part;
    } parts;

    void main(){}
};

struct accessor_test_container_t
{
    c1_t c1;
    c2_t c2;
};

constinit accessor_test_container_t accessor_test_container{};

static_assert(Component<c1_t>);
static_assert(Component<c2_t>);
static_assert(Component<c1_t::parts_t::dummy_part>);
static_assert(ComponentContainer<accessor_test_container_t>);
// @/
```

The container can be seen as the root of a tree-like structure.

```
( container // a component container
, ( c1 // a component
  , ( inputs // an input endpoint container
    , ( in1 // an input endpoint
      , in2 // another input endpoint
      )
    )
  , ( outputs // an output endpoint container
    , ( out // an output endpoint
      )
    )
  , ( parts // a parts container
    , ( dp // a component
      , ( parts // a parts container
        )
      )
    )
  )
, ( c2 // a component
  , /* etc, as above */
  )
)
```

Each node in this tree structure has a certain type, such as described in
comments above. A set of tag classes and type traits are provided to
distinguish between these types at compile time.

```cpp
// @='nodes'
namespace node
{
    struct component_container {};
    struct component {};
    struct part_component {};
    struct inputs_container {};
    struct outputs_container {};
    struct endpoints_container {};
    struct parts_container {};
    struct input_endpoint {};
    struct output_endpoint {};
    struct endpoint {};
    template<typename> struct is_component_container : std::false_type {};
    template<>         struct is_component_container<component_container> : std::true_type {};
    template<typename> struct is_component : std::false_type {};
    template<typename> struct is_part_component : std::false_type {};
    template<>         struct is_part_component<part_component> : std::true_type {};
    template<>         struct is_component<component> : std::true_type {};
    template<typename> struct is_inputs_container : std::false_type {};
    template<>         struct is_inputs_container<inputs_container> : std::true_type {};
    template<typename> struct is_outputs_container : std::false_type {};
    template<>         struct is_outputs_container<outputs_container> : std::true_type {};
    template<typename> struct is_parts_container : std::false_type {};
    template<>         struct is_parts_container<parts_container> : std::true_type {};
    template<typename> struct is_input_endpoint : std::false_type {};
    template<>         struct is_input_endpoint<input_endpoint> : std::true_type {};
    template<typename> struct is_output_endpoint : std::false_type {};
    template<>         struct is_output_endpoint<output_endpoint> : std::true_type {};
    template<typename> struct is_endpoints_container : std::false_type {};
    template<>         struct is_endpoints_container<inputs_container> : std::true_type {};
    template<>         struct is_endpoints_container<outputs_container> : std::true_type {};
    template<>         struct is_endpoints_container<endpoints_container> : std::true_type {};
    template<typename> struct is_endpoint : std::false_type {};
    template<>         struct is_endpoint<input_endpoint> : std::true_type {};
    template<>         struct is_endpoint<output_endpoint> : std::true_type {};
    template<>         struct is_endpoint<endpoint> : std::true_type {};
}
// @/
```

## Tuple-like Access

Generalising `boost::pfr`'s facility of taking a structure and returning a
tuple of references to or copies of its elements, we aim in this section to
enable tuple-like semantics for general components, i.e. regular components and
component containers.

In a first attempt, I used `boost::mp11` to first generate a type list from a
general component. However, this turns out to provide very little use when it
comes time to make a tuple of references. Instead, the first step is to
generate the runtime tuple of references. This runtime structure can be
manipulated more fluently using a combination of runtime programming and
template metaprogramming, and if compile-time execution is required, the
transformations can generally be `constexpr` or `constinit` and thus executed
at compile-time, or easily converted into pure type-generating metaprograms
using e.g. `std::declval` and `decltype`.

## Component Tree

For starters, we define a function `component_to_tree` that generates a tuple
of nested tuples that preserves the tree-like structure of a general component,
augmenting our references with a node-type tag, as in the following expected
output. Each element of the tree is a tuple whose first element is a node and
subsequent elements are that node's subtrees as tuples of the same form. Leaf
nodes are thus represented as tuples containing a single element.

```cpp
// @='tuples of nodes'
template<typename Tag, typename Val>
struct tagged
{
    using tag = Tag;
    using type = Val;
    Val& ref;
};
// @/

// @+'tests'
using atc   = accessor_test_container_t;
using c1    =     c1_t;
using ic1   =          c1::inputs_t;
using in11  =              ic1::in1_t;
using in21  =              ic1::in2_t;
using oc1   =          c1::outputs_t;
using out1  =              oc1::out_t;
using pc1   =          c1::parts_t;
using dp1   =              pc1::dummy_part;
using dppc1 =                  dp1::parts_t;
using c2    =     c2_t;
using ic2   =          c2::inputs_t;
using in12  =              ic2::in1_t;
using in22  =              ic2::in2_t;
using oc2   =          c2::outputs_t;
using out2  =              oc2::out_t;
using pc2   =          c2::parts_t;
using dp2   =              pc2::dummy_part;
using dppc2 =                  dp2::parts_t;

static_assert(std::same_as<decltype(component_to_tree(accessor_test_container))
, std::tuple< tagged<node::component_container,atc>
            , std::tuple< tagged<node::component,c1>
                        , std::tuple< tagged<node::inputs_container,ic1>
                                    , std::tuple<tagged<node::input_endpoint,in11>>
                                    , std::tuple<tagged<node::input_endpoint,in21>>
                                    >
                        , std::tuple< tagged<node::outputs_container,oc1>
                                    , std::tuple<tagged<node::output_endpoint,out1>>
                                    >
                        , std::tuple< tagged<node::parts_container,pc1>
                                    , std::tuple< tagged<node::part_component,dp1>
                                                , std::tuple<tagged<node::parts_container,dppc1>>
                                                >
                                    >
                        >
            , std::tuple< tagged<node::component,c2>
                        , std::tuple< tagged<node::inputs_container,ic2>
                                    , std::tuple<tagged<node::input_endpoint,in12>>
                                    , std::tuple<tagged<node::input_endpoint,in22>>
                                    >
                        , std::tuple< tagged<node::outputs_container,oc2>
                                    , std::tuple<tagged<node::output_endpoint,out2>>
                                    >
                        , std::tuple< tagged<node::parts_container,pc2>
                                    , std::tuple< tagged<node::part_component,dp2>
                                                , std::tuple<tagged<node::parts_container,dppc2>>
                                                >
                                    >
                        >
            >
>);

TEST_CASE("component_to_tree")
{
    constexpr auto tree = component_to_tree(accessor_test_container);
    auto& in1 = std::get<0>(std::get<1>(std::get<1>(std::get<1>(tree)))).ref;
    accessor_test_container.c1.inputs.in1.extra_value = 0.0;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 0.0);
    in1.extra_value = 3.14f;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 3.14f);
}
// @/
```

TODO: simplify this implementation. The cases for a component container,
endpoint container, and parts container are all very very similar, and could
likely be addressed with a single more generic function.

First consider the case where we have been passed a component container `T&
component`. `component_to_tree` should return something of the form: `<head,
<subtree>, <subtree>, ...>` where the head of the tuple is simply the
`component` tagged `node::component_container`, and each subtree in the tail of
the tuple is the tree for one component. To make the tail, we can get a tuple
of references to the subcomponents using `boost::pfr::structure_tie`. Then
using `boost::mp11::tuple_transform` we can turn each subcomponent reference
into its corresponding subtree, completing the tail. We then must use
`tuple_cat` to unpack the tail, which is one tuple containing all of the
sublists, so that we will have `< head, <subtree>, <subtree> ... >` where the
tail is zero or more subtree tuples, instead of `< head, < <subtree>,
<subtree>, ...> >` where the tail is a single tuple containing the subtree
tuples.

```cpp
// @='component container tree case'
auto subcomponents = boost::pfr::structure_tie(component);
auto head = std::make_tuple(tagged<node::component_container, T>{component});
auto tail = tuple_transform([](auto& subcomponent)
{
    return component_to_tree(subcomponent);
}, subcomponents);
return std::tuple_cat( head, tail);
// @/
```

We could access the head or tail with the following methods:

```cpp
// @+'tuples of nodes'
template<typename T> struct is_tuple : std::false_type {};
template<typename ... Ts> struct is_tuple<std::tuple<Ts...>> : std::true_type {};
template<typename T> constexpr const bool is_tuple_v = is_tuple<T>::value;
template<typename T> concept Tuple = is_tuple_v<std::remove_cvref_t<T>>;

template<Tuple T>
constexpr auto tuple_head(T tup)
{
    if constexpr (std::tuple_size_v<T> == 0) return tup;
    else return std::get<0>(tup);
}

template<Tuple T, size_t ... Ns>
constexpr auto tuple_tail_impl(T tup, std::index_sequence<Ns...>)
{
    return std::make_tuple(std::get<Ns + 1>(tup)...);
}

template<Tuple T>
constexpr auto tuple_tail(T tup)
{
    if constexpr (std::tuple_size_v<T> <= 1) return std::tuple<>{};
    else return tuple_tail_impl(tup, std::make_index_sequence<std::tuple_size_v<T> - 1>{});
}
// @/
```

Now consider the case of a regular component. The subtree has the form: `<
head, <maybe inputs>, <maybe outputs>, <maybe parts> >`. Because only one of
inputs, outputs, and parts is required, we have to work around the possibility
that one or two of these subtrees may not exist. Our strategy is to always
`tuple_cat` four tuples, where the inputs, outputs, and parts tuples may be
empty tuples, which will vanish during the `tuple_cat` operation. One awkward
consequence of this strategy is that the `head` of the tree has to be a
`std::tuple` before being passed to `tuple_cat`.

```cpp
// @='component tree case'
constexpr auto parts_subtree = [](T& component)
{
    if constexpr (not has_parts<T>) return std::tuple<>{};
    else
    {
        auto& container = parts_of(component);
        auto subcomponents = boost::pfr::structure_tie(container);
        auto head = std::make_tuple(tagged<node::parts_container, parts_t<T>>{container});
        auto tail = tuple_transform([](auto& subcomponent)
        {
            auto subtree = component_to_tree(subcomponent);
            if constexpr (has_main_subroutine<T>)
            {
                auto component = tuple_head(subtree);
                auto head = std::make_tuple(tagged<node::part_component
                                                  , typename decltype(component)::type
                                                  >{component.ref});
                auto tail = tuple_tail(subtree);
                return std::tuple_cat(head, tail);
            }
            else return subtree;
        }, subcomponents);
        return std::make_tuple(std::tuple_cat(head, tail));
    }
};
return std::tuple_cat( std::make_tuple(tagged<node::component, T>{component})
                     , endpoint_subtree<node::input_endpoint>(component)
                     , endpoint_subtree<node::output_endpoint>(component)
                     , parts_subtree(component)
                     );
// @/
```

We use one function for both input and output endpoints to save a bit of
repetition. The structure is essentially the same as the components container
case, except that there's no need to recurse into the endpoints, which are
always leaves.

```cpp
// @+'tuples of nodes'
template<typename Tag, typename T>
constexpr auto endpoint_subtree(T& component)
{
    using ContainerTag = boost::mp11::mp_if_c< std::same_as<Tag, node::input_endpoint>
                                             , node::inputs_container
                                             , node::outputs_container
                                             >;

    constexpr auto f = []<typename Container>(Container& container)
    {
        auto endpoints = boost::pfr::structure_tie(container);
        auto head = std::make_tuple(tagged<ContainerTag, Container>{container});
        auto tail = tuple_transform([]<typename Ep>(Ep& endpoint)
        {
            return std::make_tuple(tagged<Tag, Ep>{endpoint});
        }, endpoints);
        return std::make_tuple(std::tuple_cat(head, tail));
    };

    constexpr bool inputs = std::same_as<Tag, node::input_endpoint> && has_inputs<T>;
    constexpr bool outputs = std::same_as<Tag, node::output_endpoint> && has_outputs<T>;
         if constexpr (inputs) return f(inputs_of(component));
    else if constexpr (outputs) return f(outputs_of(component));
    else return std::tuple<>{};
}

template<typename T>
constexpr auto component_to_tree(T& component)
{
    if constexpr (has_name<T> || has_main_subroutine<T> || has_inputs<T> || has_outputs<T> || has_parts<T>)
    {
        @{component tree case}
    }
    else
    {
        @{component container tree case}
    }
}
// @/
```

## Component Node List

The tree structure is useful for certain applications, but most of the time it is more
convenient to work with a flat tuple. The following function flattens a given node tree,
returning such a flat tuple.

To be able to flatten the tree, we need to be able to access the head and tail
respectively of the tree's tuples.

```cpp
// TODO: address that this was yanked up to an earlier part of the file
// @+'tuples of nodes'
// @/

// @+'tests'
TEST_CASE("tuple head and tail")
{
    struct {
        int a;
        int b;
        int c;
    } x = {0,0,0};

    auto tup = boost::pfr::structure_tie(x);
    auto head = tuple_head(tup);
    static_assert(std::same_as<int, decltype(head)>);
    auto tail = tuple_head(tup);

    auto empty_tuple = std::tuple<>{};
    auto empty = tuple_head(empty_tuple);
}
// @/
```

Flattening the tree is then a simple matter of recursively peeling apart the
input. The function aims to return a single flat tuple. This is achieved using
`tuple_cat` to join a flat tuple made from the head of the input with a flat
tuple made from the end. In case the head is an element, and not a nested
tuple, a flat tuple is made by simply wrapping the element in a tuple.
Otherwise, the function recurses, so that the nested head tuple will be peeled
apart in successive calls until an element is found and recursion can stop.
Similarly, the tail of the input is always passed down recursively, so that it
can get the same treatment for its new head element. Thus the recursive calls
to `component_tree_to_node_list(head)` can be seen as doing the actual flattening, while the
calls to `component_tree_to_node_list(tail)` merely continue along the tree.

Note that, although labelled `constexpr`, this operation may not be very
efficient. It hasn't been verified whether or to what extent the compiler is
able to optimize this operation into oblivion. In the test case below, compiled
without optimizations enabled, where the flattened tuple is declared there
appear about 18 pairs of `lea, mov` instructions (one pair for each element of
the tuple?), suggesting that the compiler was able to compute the flattened
tuple at compile time, and all it does at runtime is move the computed data
structure into local memory; this is certainly encouraging, but likely relies
on the declaration of the flattened tuple as `constexpr`.

Also note that this implementation is only possible based on the assumption
that the type of every element in the tree will be unique (from the perspective
of the type system), so that the return type of any two invocations of
`component_tree_to_node_list` are guaranteed to be different, meaning there's never an issue
with inconsistent deduced return types. If this assumption ever becomes an
issue, it may be possible to enforce it by passing arbitrary unique template
parameters, such as integer constants, to each recursive call to
`component_tree_to_node_list`.

Notice also that, although the flattened tuple is declared `constexpr`, we are
still able to extract a mutable reference to our component data from it. Neat!

```cpp
// @+'tuples of nodes'
template<Tuple T>
constexpr auto component_tree_to_node_list(T tree)
{
    if constexpr (std::tuple_size_v<T> == 0) return tree;
    auto head = tuple_head(tree);
    auto tail = tuple_tail(tree);
    if constexpr (Tuple<decltype(head)>) return std::tuple_cat(component_tree_to_node_list(head), component_tree_to_node_list(tail));
    else return std::tuple_cat(std::make_tuple(head), component_tree_to_node_list(tail));
}
// @/

// @+'tests'
TEST_CASE("component_tree_to_node_list")
{
    constexpr auto flattened = component_tree_to_node_list(component_to_tree(accessor_test_container));
    static_assert(std::tuple_size_v<decltype(flattened)> == std::tuple_size_v<std::tuple<atc, c1, ic1, in11, in21, oc1, out1, pc1, dp1, dppc1, c2, ic2, in12, in22, oc2, out2, pc2, dp2, dppc2>>);

    auto& in1 = std::get<3>(flattened).ref;
    accessor_test_container.c1.inputs.in1.extra_value = 0.0;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 0.0);
    in1.extra_value = 3.14f;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 3.14f);
}
// @/
```

For convenience, a shortcut is provided that takes a component and directly
returns its node list.

```cpp
// @+'tuples of nodes'
template<typename T>
constexpr auto component_to_node_list(T& component)
{
    return component_tree_to_node_list(component_to_tree(component));
}
// @/
```

## Filtering the Node List

A flat tuple is easy to filter. We can pass a predicate metafunction to the
filter, and then use `tuple_transform` from `mp11` to transform the flat
tuple into one of wrapped values or empty tuples based on the predicate, then
`std::apply` the result to `tuple_cat` to get a filtered tuple out.

```cpp
// @+'tuples of nodes'
template<template<typename>typename F>
constexpr auto node_list_filter(Tuple auto tup)
{
    return std::apply([](auto...args)
    {
        auto ret = std::tuple_cat(args...);
        if constexpr (std::tuple_size_v<decltype(ret)> == 0)
            return;
        else if constexpr (std::tuple_size_v<decltype(ret)> == 1)
            return std::get<0>(ret);
        else return ret;
    }
    , tuple_transform([]<typename E>(E element)
    {
        if constexpr (F<E>::value) return std::make_tuple(element);
        else return std::tuple<>{};
    }, tup));
}
// @/
```

As above, a shortcut is provided to extract a filtered node list directly
from a component:

```cpp
// @+'tuples of nodes'
template<template<typename>typename F, typename T>
constexpr auto component_filter(T& component)
{
    return node_list_filter<F>(component_to_node_list(component));
}
// @/
```

### Searching for a Particular Node

A search can be construed as a filtering operation. Here we leverage
`node_list_filter` to find elements with a particular type within our flattened
tuple of tagged component tree nodes. Based on the assumption that every node
in the tree has a unique type, this is a way of finding a particular node in
the tree. A shortcut is provided for working directly from a component.

In the following test, `only_in1` is fully inlined by the compiler. No runtime
computation is required; the address of the variable is simply loaded from
memory.

```cpp
// @+'tuples of nodes'
template<typename T>
struct tagged_is_same
{
    template<typename Y>
    struct fn : std::is_same<T, typename Y::type> {};
};

template<typename T>
constexpr auto& find(Tuple auto tup)
{
    return node_list_filter<tagged_is_same<T>::template fn>(tup).ref;
}

template<typename T>
constexpr auto& find(auto& component)
{
    return node_list_filter<tagged_is_same<T>::template fn>(component_to_node_list(component)).ref;
}
// @/

// @+'tests'
TEST_CASE("node_list_filter")
{
    constexpr auto& in1 = find<in11>(component_tree_to_node_list(component_to_tree(accessor_test_container)));
    accessor_test_container.c1.inputs.in1.extra_value = 0.0;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 0.0);
    in1.extra_value = 3.14f;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 3.14f);
}
// @/
```

### Filter by Node Type

We can filter for a particular type of node (e.g. input endpoints or parts
containers or components) in a similar fashion. With the definition of a
`for_each_node_in_list` function that runs a callback for each node in a
component node list, this provides a succinct method to visit each node that
matches one of the node types in a variadic list of node tag template type
parameters, allowing the user to select which types of nodes they wish to visit
by first filtering the list.

```cpp
// @+'tuples of nodes'
template<typename ... RequestedNodes>
struct _search_by_tags
{
    template<typename Tag> using fn = boost::mp11::mp_contains<std::tuple<RequestedNodes...>, typename Tag::tag>;
};

template<typename ... RequestedNodes>
constexpr auto node_list_filter_by_tag(Tuple auto tup)
{
    return node_list_filter<_search_by_tags<RequestedNodes...>::template fn>(tup);
}

template<typename ... RequestedNodes>
constexpr auto component_filter_by_tag(auto& component)
{
    return node_list_filter<_search_by_tags<RequestedNodes...>::template fn>(component_to_node_list(component));
}

template<typename ... RequestedNodes>
constexpr auto for_each_node_in_list(const Tuple auto node_list, auto callback)
{
    auto f = [&](auto tagged_node)
    {
        callback(tagged_node.ref, typename decltype(tagged_node)::tag{});
    };
    if constexpr (sizeof...(RequestedNodes) > 0)
    {
        constexpr auto filtered = node_list_filter_by_tag<RequestedNodes...>(node_list);
        boost::mp11::tuple_for_each(filtered, f);
    }
    else boost::mp11::tuple_for_each(node_list, f);
}
// @/
```

## Node Paths

We model the path of a named node as a tuple whose first element is the root of
the component tree in which the node is found, whose last element is the named
node, and whose intervening elements are the named parents of the node in order.

```cpp
// @+'tests'
auto in11_path = path_of<in11>(component_to_tree(accessor_test_container));
static_assert(std::same_as< std::remove_cvref_t<decltype(in11_path)>
                          , std::tuple< tagged<node::component,c1>
                                      , tagged<node::input_endpoint,in11>
                                      >
                          >);

struct deep_component : name_<"root"> {
    void main();
    struct parts_t {
        struct n1 : name_<"n1"> {
            void main();
            struct parts_t {
                struct n2 : name_<"n2"> {
                    void main();
                    struct parts_t {
                        struct n3 : name_<"n3"> {
                            void main();
                            struct inputs_t {
                                struct in : name_<"in"> {} input;
                            } inputs;
                        } part;
                    } parts;
                } part;
            } parts;
        } part;
    } parts;
} deep;

using deep_input = deep_component::parts_t::n1::parts_t::n2::parts_t::n3::inputs_t::in;
auto deep_path = path_of<deep_input>(deep);
static_assert(std::same_as< std::remove_cvref_t<decltype(deep_path)>
        , std::tuple< tagged<node::component,deep_component>
                    , tagged<node::part_component,deep_component::parts_t::n1>
                    , tagged<node::part_component,deep_component::parts_t::n1::parts_t::n2>
                    , tagged<node::part_component,deep_component::parts_t::n1::parts_t::n2::parts_t::n3>
                    , tagged<node::input_endpoint,deep_component::parts_t::n1::parts_t::n2::parts_t::n3::inputs_t::in>
                    >
        >);

// @/
```

Generating a path, given a node's (untagged) type and a component tree, is
similar to flattening the tree while searching for a particular node.

The input to the search is a tuple representing a part of the overall tree of
components. If the tuple is empty, this is the tail of a leaf node of the tree.
We return the empty tuple unchanged.

Otherwise, we split the tuple into its head and its tail. If the head is a node
that matches the one we are looking for, we return a tuple containing it.
Otherwise, if the head is a different node, we search for the sought node in
the tail of the tuple. If it is found, this search will return a non-empty
tuple. If the current node is named, we prepend the current node, which is a
named parent of the sought one, to this tuple, and return it, or else simply
return the found sub-path. Otherwise we return an empty tuple, since this node
is not part of the path.

Finally, if the head is a tuple, then we return the `tuple_cat` of the
recursive search over the head subtree and tail subtree(s). If any of these
find the sought node, the result of the concatenation will be a non-empty
tuple. Otherwise, it will be an empty tuple.

```cpp
// @+'tuples of nodes'
template<typename T, Tuple Tup>
constexpr auto path_of(const Tup tree)
{
    if constexpr (std::tuple_size_v<Tup> == 0) // tail of leaf
        return std::tuple<>{};

    auto head = tuple_head(tree);
    auto tail_path = path_of<T>(tuple_tail(tree));
    if constexpr (Tuple<decltype(head)>) // search subtrees
        return std::tuple_cat(path_of<T>(head), tail_path);
    else if constexpr (std::same_as<typename decltype(head)::type, T>) // located T
        return std::make_tuple(head);
    else if constexpr (std::tuple_size_v<decltype(tail_path)> > 0) // head is a parent
    {
        if constexpr (has_name<typename decltype(head)::type>) // prepend named parent
            return std::tuple_cat(std::make_tuple(head), tail_path);
        else return tail_path; // return sub-path
    }
    else return std::tuple<>{}; // node is in another subtree
}

template<typename T, typename C>
    requires Component<C> || ComponentContainer<C>
constexpr auto path_of(C& component)
{
    return path_of<T>(component_to_tree(component));
}
// @/
```

## Untagged List

```cpp
// @+'tests'
auto outputs = remove_node_tags(node_list_filter_by_tag<node::output_endpoint>(component_tree_to_node_list(component_to_tree(accessor_test_container))));
static_assert(std::same_as<decltype(outputs), std::tuple<out1, out2>>);
// @/

// @+'tuples of nodes'
template<typename Tag> using untagged = typename Tag::type;

// TODO: shouldn't this return references?
template<Tuple T>
constexpr auto remove_node_tags(T tup)
{
    using return_type = mp_transform<untagged, T>;
    return std::make_from_tuple<return_type>(tuple_transform([](auto&& tagged) {return tagged.ref;}, tup));
}
// @/
```

## Common Type List Metafunctions

Often it's not necessary to access a value instance of a node list, but rather
it's `std::tuple` type, i.e. a type list. The following template type aliases
are provided to facilitate direct access to useful type lists.

```cpp
// @+'tests'
static_assert(std::same_as<decltype(outputs), output_endpoints_t<accessor_test_container_t>>);
static_assert(std::same_as<decltype(remove_node_tags(deep_path)), path_t<deep_input, deep_component>>);
// TODO: test the other ones as needed
// @/

// @+'tuples of nodes'
template<typename T> using output_endpoints_t =
    decltype(remove_node_tags(component_filter_by_tag<node::output_endpoint>(std::declval<T&>())));
template<typename T> using input_endpoints_t =
    decltype(remove_node_tags(component_filter_by_tag<node::input_endpoint>(std::declval<T&>())));
template<typename T> using endpoints_t =
    decltype(remove_node_tags(component_filter_by_tag<node::input_endpoint, node::output_endpoint>(std::declval<T&>())));
template<typename T, typename C> using path_t =
    decltype(remove_node_tags(path_of<T>(std::declval<C&>())));
// @/
```

# For each

As an alternative to the node-list based approach above, the following
`for_each` implementation directly iterates over a component tree with compile
time branching. This approach provides nearly identical performance in informal
benchmarks, compared to `for_each_node_in_list`, provided that the list in the
latter case is generated at compile time, i.e. declared constexpr.

```cpp
// @+'tests'
TEST_CASE("for each X")
{
    string allnames{};
    auto add_names = [&](auto& entity)
    {
        allnames += string(entity.name());
    };

    SECTION("for each component")
    {
        for_each_component(accessor_test_container, add_names);
        REQUIRE(allnames == string("c1dpc2dp"));
    }

    SECTION("for each endpoint")
    {
        for_each_endpoint(accessor_test_container, add_names);
        REQUIRE(allnames == string("in1in2outin1in2out"));
    }

    SECTION("for each input")
    {
        for_each_input(accessor_test_container, add_names);
        REQUIRE(allnames == string("in1in2in1in2"));
    }

    SECTION("for each output")
    {
        for_each_output(accessor_test_container, add_names);
        REQUIRE(allnames == string("outout"));

    }

    SECTION("for each output in list (pregen)")
    {
        string allnodes{};
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        constexpr auto filtered = node_list_filter_by_tag<node::output_endpoint>(
                component_tree_to_node_list(component_to_tree(accessor_test_container)));
        for_each_node_in_list(filtered, add_node);
        REQUIRE(allnodes == string("outout"));


    }

    SECTION("for each output in list")
    {
        constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
    }

    SECTION("for each node")
    {
        string allnodes{};
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        for_each_node(accessor_test_container, add_node);
        REQUIRE(allnodes == string("c1in1in2outdpdppc2in1in2outdpdpp"));

    }

    SECTION("for each node in list (pregen)")
    {
        constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
        string allnodes{};
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        for_each_node_in_list(list, add_node);
        REQUIRE(allnodes == string("c1in1in2outdpdppc2in1in2outdpdpp"));

    }

    SECTION("for each node in list (pregen)")
    {
        string allnodes{};
        constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        for_each_node_in_list(list, add_node);
        REQUIRE(allnodes == string("c1in1in2outdpdppc2in1in2outdpdpp"));

    }
}

TEST_CASE("for each benchmarks", "[!benchmark]")
{
    string allnames{};
    auto add_names = [&](auto& entity)
    {
        allnames += string(entity.name());
    };

    constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
    string allnodes{};
    auto add_node = [&]<typename T>(T& entity, auto tag)
    {
        if constexpr (has_name<T>) allnodes += string(entity.name());
    };

    constexpr auto filtered = node_list_filter_by_tag<node::output_endpoint>(
            component_tree_to_node_list(component_to_tree(accessor_test_container)));

    BENCHMARK("for each output bench")
    {
        for_each_output(accessor_test_container, add_names);
        return allnames;
    };

    BENCHMARK("for each output in list (pregen) bench")
    {
        for_each_node_in_list(filtered, add_node);
        return allnodes;
    };

    BENCHMARK("for each output in list bench (from component)")
    {
        constexpr auto filtered = node_list_filter_by_tag<node::output_endpoint>(
                component_tree_to_node_list(component_to_tree(accessor_test_container)));
        for_each_node_in_list(filtered, add_node);
        return allnodes;
    };

    BENCHMARK("for each node bench")
    {
        string allnodes{};
        for_each_node(accessor_test_container, add_node);
        return allnodes;
    };

    BENCHMARK("for each node in list (pregen) bench")
    {
        for_each_node_in_list(list, add_node);
        return allnodes;
    };
}
// @/
```

`for_each_node` runs the user-provided callback function for each of these
nodes. To help the callback distinguish between a component container,
component, input or output endpoint aggregate, parts aggregate, or endpoint, an
empty tag class is passed as the second argument to the callback. Some helpers
are provided to facilitate compile-time branching depending on the type of node.

The function itself takes a variadic list of these tags, allowing the
user to select which types of nodes they wish to visit. The default, in case
no tags are provided, is to visit every node.

```cpp
// @+'for each'
template<typename T, typename ... RequestedNodes>
constexpr auto for_each_node(T& component, auto callback)
{
    using boost::mp11::mp_list;
    using boost::mp11::mp_contains;
    using boost::mp11::mp_empty;

    using nodes = mp_list<RequestedNodes...>;

    @{for each component container case}
    @{for each component case}
}
// @/
```

In case the input is a component container, we optionally call back for said
container, and then recurse for each contained component.

```cpp
// @='for each component container case'
if constexpr (ComponentContainer<T>)
{
    if constexpr (  mp_empty<nodes>::value
                 || mp_contains<nodes, node::component_container>::value
                 )  callback(component, node::component_container{});
    boost::pfr::for_each_field(component, [&]<typename S>(S& subcomponent)
    {
        for_each_node<S, RequestedNodes...>(subcomponent, callback);
    });
}
// @/
```

The pattern is almost identical for each of the optional aggregates of a
component. For endpoint containers, we can skip the iteration over the
aggregate if the user has not requested the corresponding type of endpoints be
visited. However, we always recurse over parts in a parts container since the
user must wish to visit some of its nested nodes, if any exist.

```cpp
// @='for each component case'
else if constexpr (Component<T>)
{
    if constexpr (  mp_empty<nodes>::value
                 || mp_contains<nodes, node::component>::value
                 )  callback(component, node::component{});
    if constexpr (has_inputs<T>)
    {
        auto& inputs = inputs_of(component);

        if constexpr (  mp_empty<nodes>::value
                     || mp_contains<nodes, node::inputs_container>::value
                     || mp_contains<nodes, node::endpoints_container>::value
                     )  callback(inputs, node::inputs_container{});

        // iterate only if we are visiting input endpoints
        if constexpr (  mp_empty<nodes>::value
                     || mp_contains<nodes, node::input_endpoint>::value
                     || mp_contains<nodes, node::endpoint>::value
                     )  boost::pfr::for_each_field(inputs, [&](auto& in)
        {
            callback(in, node::input_endpoint{});
        });
    }
    if constexpr (has_outputs<T>)
    {
        auto& outputs = outputs_of(component);

        if constexpr (  mp_empty<nodes>::value
                     || mp_contains<nodes, node::outputs_container>::value
                     || mp_contains<nodes, node::endpoints_container>::value
                     )  callback(outputs, node::outputs_container{});

        // iterate only if we are visiting output endpoints
        if constexpr (  mp_empty<nodes>::value
                     || mp_contains<nodes, node::output_endpoint>::value
                     || mp_contains<nodes, node::endpoint>::value
                     )  boost::pfr::for_each_field(outputs, [&](auto& out)
        {
            callback(out, node::output_endpoint{});
        });
    }
    if constexpr (has_parts<T>)
    {
        auto& parts = parts_of(component);

        if constexpr (  mp_empty<nodes>::value
                     || mp_contains<nodes, node::parts_container>::value
                     )  callback(parts, node::parts_container{});

        // always recurse over nested components
        boost::pfr::for_each_field(parts, [&]<typename P>(P& part)
        {
            for_each_node<P, RequestedNodes...>(part, callback);
        });
    }
}
// @/
```

The other `for_each_xyz` function are then easily implemented in terms of
`for_each_node`:

```cpp
// @+'for each'
template <typename T> constexpr void for_each_component(T& component, auto callback)
{
    for_each_node<T, node::component>(component, [&](auto& c, auto) { callback(c); });
}

template<typename T> constexpr void for_each_endpoint(T& component, auto callback)
{
    for_each_node<T, node::endpoint>(component, [&](auto& c, auto) { callback(c); });
}

template<typename T> constexpr void for_each_input(T& component, auto callback)
{
    for_each_node<T, node::input_endpoint>(component, [&](auto& c, auto) { callback(c); });
}

template<typename T> constexpr void for_each_output(T& component, auto callback)
{
    for_each_node<T, node::output_endpoint>(component, [&](auto& c, auto) { callback(c); });
}
// @/
```

# Clearable Flags

Many components have endpoints with message semantics, e.g. by its value having
semantics similar to a pointer or `std::optional`. These values are expected to
be cleared so that they have a `false` interpretation unless the value has been
set just before or during the current call to the component's main subroutine.
To facilitate bindings implementing this behavior in a consistent way, the
following subroutine is implemented, that reflects over a component's endpoints
and clears all of its endpoints with this semantics.

A test is not provided for this function here, since its use in the CLI binding
is well tested.

```cpp
// @='clear_flags'
template<typename Y>
void clear_flag(Y& endpoint)
{
    if constexpr (ClearableFlag<Y>) clear_flag(endpoint);
}

void clear_flags(auto& component)
{
    for_each_endpoint(component, [](auto& endpoint) { clear_flag(endpoint); });
}

void clear_output_flags(auto& component)
{
    for_each_output(component, [](auto& endpoint) { clear_flag(endpoint); });
}

void clear_input_flags(auto& component)
{
    for_each_input(component, [](auto& endpoint) { clear_flag(endpoint); });
}
// @/
```

# Init and Activate

The initialization and main subroutines of components can be generically
triggered using the following functions. Overloads are also provided that will
initialize or activate all components in a component container, recursively.
This currently includes parts in subassemblies, but TODO it probably shouldn't.

```cpp
// @='init'
template<Component T>
void init(T& component)
{
    if constexpr (requires {component.init();}) component.init();
}

template<ComponentContainer T>
void init(T& container)
{
    for_each_component(container, [](auto& component) {init(component);});
};
// @/
```

```cpp
// @='activate'
template<Component T>
void activate_inner(T& component)
{
    if constexpr (requires {component.main(component.inputs, component.outputs);})
        component.main(component.inputs, component.outputs);
    else if constexpr (requires {component(component.inputs, component.outputs);})
        component(component.inputs, component.outputs);
    else if constexpr (requires {component();})
        component();

}

template<Component T>
void activate(T& component)
{
    clear_output_flags(component);
    activate_inner(component);
    clear_input_flags(component);
}

template<ComponentContainer T>
void activate(T& container)
{
    clear_output_flags(container);
    for_each_component(container, activate_inner);
    clear_input_flags(container);
};
// @/
```

# Summary

```cpp
// @#'sygac-components.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <boost/pfr.hpp>
#include <boost/mp11.hpp>
#include "sygac-metadata.hpp"
#include "sygac-functions.hpp"
#include "sygac-endpoints.hpp"

namespace sygaldry {

using boost::mp11::mp_transform;
using boost::mp11::tuple_transform;
using boost::mp11::tuple_for_each;

@{Component Concepts}

@{nodes}

@{tuples of nodes}

@{for each}

@{clear_flags}

@{init}

@{activate}

}
// @/
```

```cpp
// @#'sygac-components.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <type_traits>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"
#include "sygah-consteval.hpp"
#include "sygac-components.hpp"

using namespace sygaldry;
using std::string;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygac-components)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE Boost::pfr)
target_link_libraries(${lib} INTERFACE Boost::mp11)
target_link_libraries(${lib} INTERFACE sygac-metadata)
target_link_libraries(${lib} INTERFACE sygac-functions)
target_link_libraries(${lib} INTERFACE sygac-endpoints)

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
target_link_libraries(${lib}-test PRIVATE sygah)
catch_discover_tests(${lib}-test)
endif()
# @/
```

\page page-sygac-runtime sygac-runtime: Sygaldry Runtime

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

The runtime describes the expected behavior of the host platform of an
assemblage of components. The runtime must first call every component's `init`
method, if it has one. Then, in an endless loop, each component's main
subroutine must be called, in node-tree-order. Before main subroutines,
external input subroutines should be called. After main subroutines, external
output subroutines should be called. This sequence of external input, main,
external output is termed as a tick. Between ticks, all of the clearable flag
endpoints should be cleared appropriately: input flags should be cleared before
beginning the tick, and output flags should be cleared after it has resolved.
Plugins and throughpoints should be automatically propagated to components that
need them by extracting them from the assemblage.

# Design Rationale

Originally planned as further overloads of the the `init` and `activate`
functions in [the components concepts library](concepts/components.lili.md),
the runtime class below allows the plugins and throughpoints of components
having them to be automatically extracted by type from a component container
when calling the `init` and `main` methods of components. The decision was
made to construct this as a class in order to ensure that the compiler would
have the best chance of computing the argument extraction at compile time. It
is a kind of ultimate binding that is meant to drive all other components,
including binding components. The runtime is included in the concepts library
so that components in the components library may make use of it to simplify
their implementation using parts, although this is may be unwise at the
time of writing due to compile time limitations of the runtime.

# Example

The following example illustrates the main requirements of the runtime,
including its flag clearing behavior, throughpoint and plugin propagation, and
the order of subroutines.

`testcomponent1_t` has an output bang that should be clear on entry to main
every tick. It always activates this bang. `testcomponent2_t` takes
`testcomponent1_t`'s outputs via an input throughpoint, and the whole
`testcomponent1_t` as a plugin component, as declared by `testcomponent2_t`'s
main subroutine arguments. It checks that `testcomponent1_t`'s output bang is
correctly propagated through the tick, and sets its outputs depending on the
state of `testcomponent1_t`.

The test case verifies that subroutines are called in the expected order, and
that flags and values propagate as expected.

```cpp
// @+'tests'
template<string_literal str>
struct testcomponent1_t : name_<str>
{
    struct inputs_t {
        struct in1_t {
            int value;
        } in1;
    } inputs;

    struct outputs_t {
        struct out1_t {
            int value;
        } out1;
        bng<"bang out"> bang_out;
    } outputs;

    void init() { inputs.in1.value = 42; }
    void main()
    {
        outputs.out1.value = inputs.in1.value + 1;
        CHECK(false == (bool)outputs.bang_out); // output flags are clear on entry to main
        outputs.bang_out();
        CHECK(true == (bool)outputs.bang_out);
    }
};

struct testcomponent2_t : name_<"tc2">
{
    struct outputs_t {
        struct out1_t {
            int value;
        } out1;
        struct out2_t {
            int value;
        } out2;
    } outputs;

    void init() {};

    void main(const testcomponent1_t<"tc1">::outputs_t& sources, testcomponent1_t<"tc1">& plugin)
    {
        CHECK(true == (bool)plugin.outputs.bang_out); // flags propagate during main tick
        outputs.out1.value = sources.out1.value + 1;
        outputs.out2.value = plugin.outputs.out1.value + 1;
    }
};

struct components1_t
{
    testcomponent1_t<"tc1"> tc1;
    testcomponent2_t tc2;
};

struct components2_t
{
    testcomponent1_t<"tc1"> tc1;
};
// @/

// @+'tests'
constinit components1_t components{};
constexpr auto runtime = Runtime{components};
TEST_CASE("sygaldry runtime calls")
{
    runtime.init();
    CHECK(runtime.container.tc1.inputs.in1.value == 42); // init routines are called
    runtime.tick();
    CHECK(false == (bool)runtime.container.tc1.outputs.bang_out); // out flags are clear after call to tick
    CHECK(runtime.container.tc1.outputs.out1.value == 43); // main routines are called
    CHECK(runtime.container.tc2.outputs.out1.value == 44); // throughpoints are propagated
    CHECK(runtime.container.tc2.outputs.out2.value == 44); // plugins are propagated
                                                            // calls proceed in tree order
}

constinit components2_t components2{};
constexpr auto runtime2 = Runtime{components2};
TEST_CASE("sygaldry runtime one component")
{
    runtime2.init();
    CHECK(runtime2.container.tc1.inputs.in1.value == 42); // init routines are called
    runtime2.tick();
    CHECK(false == (bool)runtime2.container.tc1.outputs.bang_out); // out flags are clear after call to tick
    CHECK(runtime2.container.tc1.outputs.out1.value == 43); // main routines are called
}
// @/
```

# Implementation

## Extracting Plugins and Components

Arguably the trickiest aspect of the runtime's responsibilities is to recognize what
arguments a component's subroutine requires, extract them from the assemblage, and
apply them to the subroutine. We would like to ensure that this happens efficiently,
meaning without runtime traversal of the component tree derived from the assemblage.

```cpp
TEST_CASE("sygaldry example test")
{
    components1_t components;
    components.tc1.inputs.in1.value = 0;
    components.tc1.outputs.bang_out();
    activate(components.tc2, components); // << this should compile to something like:
                       // components.tc2.main(components.tc1.outputs, components.tc1);
    CHECK(components.tc2.outputs.out1.value == 1);
}
```

We'll start by extracting the arguments needed to call just one subroutine
(`main`), and then generalise to include `init` and eventually other
subroutines.

### Use a fold

My first attempt resembled the following. We define a template struct whose type parameters give
the arguments a component needs for its main subroutine to be activated. In the `activate`
method of this structure, these type parameters are unpacked to into `find` to locate
the entities with the given types in the component tree. This `runtime_impl` struct is
then instantiated with the appropriate arguments using our function reflection facilities
and a bit of template metaprogramming provided by `mp11`.

```cpp
template<typename ... Args> // Args is a parameter pack corresponding to the arguments of the component main subroutine
struct runtime_impl
{
    template <typename Component, typename ComponentContainer>
    static void activate(Component& component, ComponentContainer& container) // user passes the component and the assembly
    {
        if constexpr (requires {&Component::operator();})
            // we unpack the parameter pack into find calls; resolves to something like a call to the main routine:
            // component(arg1, arg2, arg3, ... argN);
            component(find<Args>(container) ...);
        else if constexpr (requires {&Component::main;})
            component.main(find<Args>(container) ...);
    }
};

template<typename Component, typename ComponentContainer>
void activate(Component& component, ComponentContainer& container)
{
    using args = typename main_subroutine_reflection<Component>::arguments; // get the argument type list
    // change from a tuple to a runtime_impl
    using runtime = boost::mp11::mp_rename<boost::mp11::mp_transform<std::remove_cvref_t, args>, runtime_impl>;
    runtime::activate(component, container);
}
```

### Traverse at compile time

Unfortunately, the compiler is unable to optimize away the traversal of the
component tree implied by the multiple calls to `find` in this implementation.
We need some way of declaring this traversal `constexpr`, or perhaps
`constinit`. This lead to the following implementation. Roughly the same
metaprogramming pattern is used to enable the arguments of the function to be
extracted, by expanding a parameter pack with a fold expression over calls to
`find`. However, instead of doing so in a function call context, a tuple is
initialized which holds the arguments until call time. So that this
traversal can happen at compile time, the constructor that initializes the tuple
is declared `constexpr`.

```cpp
// @='component_runtime 1 tuple'
// unpack the Args parameter pack into a fold of find calls
// resolves to a call to `forward_as_tuple(arg1, arg2, arg3, ..., argN)`
// we get that type, and declare a const tuple with it
using arg_pack_t = decltype(tpl::forward_as_tuple(find<Args>(std::declval<ComponentContainer&>())...));
const arg_pack_t arg_pack;

// then when we construct the runtime impl, we constexpr instantiate the tuple
constexpr component_runtime_impl(ComponentContainer& container) : arg_pack{tpl::forward_as_tuple(find<Args>(container) ...)} {}
// @/
```

This `component_runtime_impl` structure can be declared `constinit`, which
should guarantee that the construction of the arguments tuple, and thus the
traversal of the component tree, happens at compile time; in case the compiler
didn't initialize the tuple at compile time for some reason, there would still
be little runtime performance impact, as the traversal would take place before
the main program properly started running, although there would still be an
increase in executable size if this somehow were to happen (e.g. if we forgot
to declare the impl struct `constinit`).

```cpp
// @+'tests'
components1_t constinit main_runtime_components{};
component_runtime<testcomponent2_t, components1_t> constinit main_component_runtime{main_runtime_components.tc2, main_runtime_components};
TEST_CASE("sygaldry component runtime main")
{
    main_runtime_components.tc1.outputs.out1.value = 0;
    main_runtime_components.tc1.outputs.bang_out();
    main_component_runtime.main();
    CHECK(main_runtime_components.tc2.outputs.out1.value == 1);
    CHECK(main_runtime_components.tc2.outputs.out2.value == 1);
}
// @/
```

Activating the main subroutine then simply requires us to apply the tuple:

```cpp
// @='component_runtime 1'
template<typename Component, typename ComponentContainer, typename ... Args>
struct component_runtime_impl
{
    @{component_runtime 1 tuple}

    void main(Component& component) const
    {
        if constexpr (requires {&Component::operator();})
            tpl::apply(component, arg_pack);
        else if constexpr (requires {&Component::main;})
            tpl::apply([&](auto& ... args) {component.main(args...);}, arg_pack);
    }
};
// @/
```

All that's left then, essentially, is the metaprogram that determines the
appropriate type for the tuple, seen below in the sequence of `using`
declarations: we extract the arguments list, remove constant, volatile, and
reference qualifiers from the types in the list, prepend the component and
assembly types to the list, and then swap whatever tuple container contains
the type list with the component runtime implementation type so that we
can easily instantiate it.

```cpp
// @+'component_runtime 1'
template<typename Component, typename ComponentContainer>
struct component_runtime
{
    using args = typename main_subroutine_reflection<Component>::arguments; // get args list
    using cvref_less_args = boost::mp11::mp_transform<std::remove_cvref_t, args>; // remove cvref qualifiers
    using prepended = boost::mp11::mp_push_front<cvref_less_args, Component, ComponentContainer>; // prepend component and assembly types
    using impl_t = boost::mp11::mp_rename<prepended, component_runtime_impl>; // rename from tuple<...> to component_runtime_impl<...>

    Component& component;
    const impl_t impl;
    constexpr component_runtime(Component& comp, ComponentContainer& cont) : component{comp}, impl{cont} {};

    void init() const
    {
        // TODO
    };

    void main() const
    {
        impl.main(component);
    };
};
// @/
```

### Generalize

The above implementation is only able to activate a single component's main
subroutine. It is reasonably straightforward to generalize this to both
initialize and activate an arbitrary number of components held in a component
container.

First, we pull out the argument pack used in the inner implementation:

```cpp
// @='impl_arg_pack'
template<typename ComponentContainer, typename ... Args>
struct impl_arg_pack
{
    using arg_pack_t = decltype(tpl::forward_as_tuple(find<Args>(std::declval<ComponentContainer&>())...));
    const arg_pack_t pack;

    constexpr impl_arg_pack(ComponentContainer& container) : pack{tpl::forward_as_tuple(find<Args>(container) ...)} {}
};
// @/
```

Having this class allows us to get rid of the inner `impl` structure (e.g.
`component_runtime_impl` above) and makes it easier to generalize the component
runtime so that it can initialize or activate a component, accepting argument
packs for both of these subroutines, as well as others such as
`external_destinations` that were added later.

The full implementation now resembles the following, which essentially simply
gets the types of the `arg_pack` tuples, and applies them to the component's
subroutines:

```cpp
// @='component_runtime 2'
template<typename Component, typename ComponentContainer>
struct component_runtime
{
    using init_args = typename init_subroutine_reflection<Component>::arguments;
    using init_cvref_less_args = boost::mp11::mp_transform<std::remove_cvref_t, init_args>;
    using init_prepended = boost::mp11::mp_push_front<init_cvref_less_args, ComponentContainer>;
    using init_arg_pack = boost::mp11::mp_rename<init_prepended, impl_arg_pack>;

    // note the repeated metaprogramming sequence...
    using main_args = typename main_subroutine_reflection<Component>::arguments;
    using main_cvref_less_args = boost::mp11::mp_transform<std::remove_cvref_t, main_args>;
    using main_prepended = boost::mp11::mp_push_front<main_cvref_less_args, ComponentContainer>;
    using main_arg_pack = boost::mp11::mp_rename<main_prepended, impl_arg_pack>;

    Component& component;
    init_arg_pack init_args;
    main_arg_pack main_args;

    constexpr component_runtime(Component& comp, ComponentContainer& cont) : component{comp}, init_args{container}, main_args{container} {}

    void init(Component& component) const
    {
        if constexpr (requires {&Component::init;})
            tpl::apply([&](auto& ... args) {component.init(args...);}, init_args.pack);
    }

    void main(Component& component) const
    {
        if constexpr (requires {&Component::operator();})
            tpl::apply(component, main_args.pack);
        else if constexpr (requires {&Component::main;})
            tpl::apply([&](auto& ... args) {component.main(args...);}, main_args.pack);
    }
};
// @/
```

We can now test the runtime's ability to run the init method:

```cpp
// @+'tests'
components1_t constinit init_runtime_components{};
component_runtime<testcomponent1_t<"tc1">, components1_t> constinit init_component_runtime{init_runtime_components.tc1, init_runtime_components};
TEST_CASE("sygaldry component runtime init")
{
    init_runtime_components.tc1.inputs.in1.value = 0;
    init_component_runtime.init();
    CHECK(init_runtime_components.tc1.inputs.in1.value == 42);
}
// @/
```

To avoid repeating the series of template metafunctions used to get the
argument pack types, we define a helper template that takes a function
reflection structure and returns an appropriate argument pack type. We allow
this to return a dummy arg pack in case a component lacks an init or main
subroutine, so that components missing one or the other will not trigger a
compiler error.

```cpp
// @='to_arg_pack'
template<typename...>
struct to_arg_pack
{
    struct dummy_t {
        tpl::tuple<> pack;
        constexpr dummy_t(auto&) : pack{} {};
    };
    using pack_t = dummy_t;
};

template<typename Component, typename ComponentContainer, typename FuncRefl>
    requires FuncRefl::exists::value
struct to_arg_pack<Component, ComponentContainer, FuncRefl>
{
    using args = typename FuncRefl::arguments;
    using cvref_less = boost::mp11::mp_transform<std::remove_cvref_t, args>;
    using prepended = boost::mp11::mp_push_front<cvref_less, ComponentContainer>;
    using pack_t = boost::mp11::mp_rename<prepended, impl_arg_pack>;
};
// @/
```

We use this helper to get two argument pack types, one for init and one for
main, that are then used to instantiate the runtime structure. We thus have a
general runtime for a single component that passes our tests.

```cpp
//@='component_runtime 3'
@{impl_arg_pack}

@{to_arg_pack}

template<typename Component, typename ComponentContainer>
struct component_runtime
{
    // no more repeated metafunction sequences
    using init_arg_pack = typename to_arg_pack<Component, ComponentContainer, init_subroutine_reflection<Component>>::pack_t;
    using main_arg_pack = typename to_arg_pack<Component, ComponentContainer, main_subroutine_reflection<Component>>::pack_t;

    Component& component;
    init_arg_pack init_args;
    main_arg_pack main_args;

    constexpr component_runtime(Component& comp, ComponentContainer& cont) : component{comp}, init_args{cont}, main_args{cont} {}

    void init() const
    {
        if constexpr (requires {&Component::init;})
            tpl::apply([&](auto& ... args) {component.init(args...);}, init_args.pack);
    }

    void main() const
    {
        if constexpr (requires {&Component::operator();})
            tpl::apply(component, main_args.pack);
        else if constexpr (requires {&Component::main;})
            tpl::apply([&](auto& ... args) {component.main(args...);}, main_args.pack);
    }
};
// @/
```

### Digression: Other Stages

Thus far we have ignored external sources and destinations and more or less
assumed that all our components would run in the order specified in the
component tree, first all their initialization routines, then all their main
subroutines in a loop forever, with input and output flags cleared in between
loops.

This sequence is adequate when considering only regular components, however,
it fails to reasonably support binding components. Consider a protocol binding
that can set the state of input endpoints and send changes in the state of output
endpoints to external devices via its protocol. There is no way to order such
a binding component so that external inputs can propagate to the bound components
*and* resulting state changes can be sent out.

```
Option 1:

run regular components
run binding (component outputs are sent out)
clear flags (external inputs are cleared)

Option 2:
run bindings (external inputs are set)
run regular components
clear flags (component outputs are cleared)
```

Our chosen solution to this issue is to simply introduce two additional stages
to our main loop, which we call `external_sources` and `external_destinations`.
Components (especially binding components) may declare methods with these
names, and argument packs are added to the component runtime to call them with
any arguments required that can be extracted from the component tree.

```cpp
//@='component_runtime'
@{impl_arg_pack}

@{to_arg_pack}

template<typename Component, typename ComponentContainer>
struct component_runtime
{
    using init_arg_pack = typename to_arg_pack<Component, ComponentContainer, init_subroutine_reflection<Component>>::pack_t;
    using main_arg_pack = typename to_arg_pack<Component, ComponentContainer, main_subroutine_reflection<Component>>::pack_t;
    using ext_src_arg_pack = typename to_arg_pack<Component, ComponentContainer, external_sources_subroutine_reflection<Component>>::pack_t;
    using ext_dst_arg_pack = typename to_arg_pack<Component, ComponentContainer, external_destinations_subroutine_reflection<Component>>::pack_t;

    Component& component;
    init_arg_pack init_args;
    main_arg_pack main_args;
    ext_src_arg_pack ext_src_args;
    ext_dst_arg_pack ext_dst_args;

    constexpr component_runtime(Component& comp, ComponentContainer& cont)
    : component{comp}, init_args{cont}, main_args{cont}, ext_src_args{cont}, ext_dst_args{cont} {}

    void init() const
    {
        if constexpr (requires {&Component::init;})
            tpl::apply([&](auto& ... args) {component.init(args...);}, init_args.pack);
    }

    void external_sources() const
    {
        if constexpr (requires {&Component::external_sources;})
            tpl::apply([&](auto& ... args) {component.external_sources(args...);}, ext_src_args.pack);
    }

    void main() const
    {
        if constexpr (requires {&Component::operator();})
            tpl::apply(component, main_args.pack);
        else if constexpr (requires {&Component::main;})
            tpl::apply([&](auto& ... args) {component.main(args...);}, main_args.pack);
    }

    void external_destinations() const
    {
        if constexpr (requires {&Component::external_destinations;})
            tpl::apply([&](auto& ... args) {component.external_destinations(args...);}, ext_dst_args.pack);
    }
};
// @/
```

### Tuple of runtimes

All that remains is to make a tuple of component runtimes with one for each
runtime managed component in the component tree. First we define a subroutine
that will give us a tuple of runtimes.

```cpp
// @='runtime tuple'
template<typename ComponentContainer>
constexpr auto component_to_runtime_tuple(ComponentContainer& cont)
{
    auto tup = component_filter_by_tag<node::component>(cont);
    if constexpr (is_tuple_v<decltype(tup)>)
        return tup.map([&](auto& tagged_component)
        {
            return component_runtime{tagged_component.ref, cont};
        });
    // if there's only one component, `component_filter_by_tag` returns the tagged component directly
    else return tpl::make_tuple(component_runtime{tup.ref, cont});
}
// @/

// @+'tests'
components1_t constinit runtime_tuple_components{};
constexpr auto runtime_tuple = component_to_runtime_tuple(runtime_tuple_components);
TEST_CASE("sygaldry runtime tuple")
{
    tpl::apply([](auto& ... runtime) {(runtime.init(), ...);}, runtime_tuple);
    CHECK(runtime_tuple_components.tc1.inputs.in1.value == 42); // init routines are called
    tpl::apply([](auto& ... runtime) {(runtime.main(), ...);}, runtime_tuple);
    CHECK(runtime_tuple_components.tc1.outputs.out1.value == 43); // main routines are called
}
// @/
```

Then we can assemble the final runtime class. This requires a somewhat awkward
repetition of the call to `component_to_runtime_tuple`, first to get the type
of the tuple and then to actually initialize it. Otherwise, the top level
runtime simply delegates to the inner component runtimes.

```cpp
// @='Runtime'

/*!

\brief Wrapper for a component assemblage that calls `init` and `main` of all
subcomponents that aren't parts.

\details The Sygaldry runtime implements the two-state model of sygaldry components
given a component container (i.e. a simple aggregate containing only
components), first initializing all components in the container, then running
their main subroutines. Furthermore, if the initialization or main subroutines
of any component depends on endpoints or endpoint containers of another
component (i.e. throughpoints), or another component itself (i.e. plugins), the
`Runtime` is able to automatically extract these dependencies from the
component tree represented by the container and forward them to the component
that requires them when its subroutine is invoked. This makes it reasonably
trivial to define a Sygaldry instrument as a mere assemblage of components. For
a simple example, see [the implementation of the One Bit Bongo][bongo].

[bongo]: instruments/one-bit-bongo/one-bit-bongo-esp32/one-bit-bongo-esp32.lili.md

\warning It is important that this class should be declared `constexpr` when it is
instantiated in order to avoid traversing the component tree at compile time, e.g.
`constexpr auto runtime = Runtime{container};`.

\tparam ComponentContainer The type signature of the component container; this
can usually be deduced from the constructor, and so doesn't need to be explicitly
identified, e.g. `constexpr auto runtime = Runtime{container};`.

*/

template<typename ComponentContainer>
struct Runtime
{
    /*! A reference to the component container managed by the `Runtime`.

    This is intended to refer to the top level component container that
    implements an instrument. In principle, it could also be a lower-level
    subcomponent used as a part in a subassembly.

    */
    ComponentContainer& container;

    /*! \brief A tuple of component-level runtimes.

    \detail Each component runtime contains a reference to a component and
    forwarded argument tuples extracted from the main component tree needed
    to invoke that component's initialization and main subroutines. Users of
    the runtime should not need to be concerned with this member, except
    insofar as its initialization is potentially costly, and is moved to
    compile time by declaring the `Runtime` as `constexpr`.

    */
    decltype(component_to_runtime_tuple(std::declval<ComponentContainer&>())) component_runtimes;

    /*! \brief The constructor, e.g. `constexpr auto runtime = Runtime{container};`

    \detail The constructor permits the template type parameter to be deduced by the compiler. It's
    important for the runtime to be constexpr for the best performance; see the warning in [the
    detailed description of the class](#details).

    \param[in,out] c The component container managed by the `Runtime`. See \ref container.

    */
    constexpr Runtime(ComponentContainer& c) : container{c}, component_runtimes{component_to_runtime_tuple(c)} {};

    /// Initialize all components in the container.
    void init() const
    {
        @{set initial values}
        tuple_for_each(component_runtimes, [](auto& r){r.init();});
    }

    /// Clear input flags, then run the external sources subroutine of all components in the container that have one.
    void external_sources() const
    {
        tuple_for_each(component_runtimes, [](auto& r){clear_input_flags(r.component);});
        tuple_for_each(component_runtimes, [](auto& r){r.external_sources();});
    }

    /// Run the main subroutine of all components in the container
    void main() const { tuple_for_each(component_runtimes, [](auto& r){r.main();}); }

    /// Run the external destinations subroutine of all components in the container that have one, then clear output flags.
    void external_destinations() const
    {
        tuple_for_each(component_runtimes, [](auto& r){r.external_destinations();});
        tuple_for_each(component_runtimes, [](auto& r){clear_output_flags(r.component);});
    }

    /// Run external sources, main, and external destinations, clearing flags appropriately
    void tick() const
    {
        external_sources();
        main();
        external_destinations();
    }

    /// A wrapper for `init` and `tick` that loops indefinitely, intended for instruments with simple requirements.
    int app_main() const { for (init(); true; tick()) {} return 0; }
};
// @/
```

# Initial values

Some endpoints may require initialization. The logic for performing this is
implemented in \ref page-sygac-endpoint-ranges, and here we can simply iterate
over all endpoints and delegate to the appropriate function.

```cpp
// @+'set initial values'
for_each_endpoint(container, []<typename T>(T& ep)
{
    initialize_endpoint(ep);
});
// @/
```

# Runtime summary

```cpp
// @#'sygac-runtime.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <boost/mp11.hpp>
#include "sygac-functions.hpp"
#include "sygac-components.hpp"
#include "sygac-endpoints.hpp"

namespace sygaldry {

/*! \addtogroup sygac
 */
/// \{

/*! \defgroup sygac-runtime sygac-runtime: Sygaldry Runtime
*/
/// \{

/*! \defgroup sygac-runtime-detail sygac-runtime: Runtime Implementation Details
These entities are used in the implementation of the Runtime class and should
be considered private implementation details.
*/
/// \{
@{component_runtime}

@{runtime tuple}
/// \}

@{Runtime}

/// \}
/// \}
}
// @/

// @#'sygac-runtime.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "sygac-runtime.hpp"
#include "sygah-endpoints.hpp"

using namespace sygaldry;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygac-runtime)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygac-components)
target_link_libraries(${lib} INTERFACE sygac-functions)
target_link_libraries(${lib} INTERFACE Boost::mp11)

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
target_link_libraries(${lib}-test PRIVATE sygah)
catch_discover_tests(${lib}-test)
endif()
# @/
```

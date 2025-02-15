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
template<typename ComponentContainer, typename ... Args>
struct impl_arg_pack
{
    using arg_pack_t = decltype(tpl::forward_as_tuple(find<Args>(std::declval<ComponentContainer&>())...));
    const arg_pack_t pack;

    constexpr impl_arg_pack(ComponentContainer& container) : pack{tpl::forward_as_tuple(find<Args>(container) ...)} {}
};

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
/// \}


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
        for_each_endpoint(container, []<typename T>(T& ep)
        {
            initialize_endpoint(ep);
        });
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

/// \}
/// \}
}

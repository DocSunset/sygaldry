#pragma once

#include <boost/mp11.hpp>
#include "concepts/functions.hpp"
#include "concepts/components.hpp"
#include <iostream>

namespace sygaldry {

template<typename ComponentContainer, typename ... Args>
struct impl_arg_pack
{
    using arg_pack_t = decltype(std::forward_as_tuple(find<Args>(std::declval<ComponentContainer&>())...));
    const arg_pack_t pack;

    constexpr impl_arg_pack(ComponentContainer& container) : pack{std::forward_as_tuple(find<Args>(container) ...)} {}
};

template<typename Component, typename ComponentContainer, typename init_arg_pack, typename main_arg_pack>
struct runtime_impl
{
    init_arg_pack init_args;
    main_arg_pack main_args;

    constexpr runtime_impl(ComponentContainer& container) : init_args{container}, main_args{container} {}

    void init(Component& component) const
    {
        if constexpr (requires {&Component::init;})
            std::apply([&](auto& ... args) {component.init(args...);}, init_args.pack);
    }

    void main(Component& component) const
    {
        if constexpr (requires {&Component::operator();})
            std::apply(component, main_args.pack);
        else if constexpr (requires {&Component::main;})
            std::apply([&](auto& ... args) {component.main(args...);}, main_args.pack);
    }
};

template<typename...>
struct to_arg_pack
{
    struct dummy_t {
        std::tuple<> pack;
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

    Component& component;
    const runtime_impl<Component, ComponentContainer, init_arg_pack, main_arg_pack> impl;
    constexpr component_runtime(Component& comp, ComponentContainer& cont) : component{comp}, impl{cont} {};

    void init() const { impl.init(component); }
    void main() const { impl.main(component); };
};

template<typename ComponentContainer>
constexpr auto component_to_runtime_tuple(ComponentContainer& cont)
{
    auto tup = component_filter_by_tag<node::component>(cont);
    return boost::mp11::tuple_transform([&](auto& tagged_component)
    {
        return component_runtime{tagged_component.ref, cont};
    }, tup);
}

template<typename ComponentContainer>
struct Runtime
{
    ComponentContainer& container;

    decltype(component_to_runtime_tuple(std::declval<ComponentContainer&>())) component_runtimes;

    constexpr Runtime(ComponentContainer& c) : container{c}, component_runtimes{component_to_runtime_tuple(c)} {};

    void init() const { tuple_for_each(component_runtimes, [](auto& r){r.init();}); }
    void main() const
    {
        tuple_for_each(component_runtimes, [](auto& r){r.main();});
        tuple_for_each(component_runtimes, [](auto& r){clear_input_flags(r.component);});
        tuple_for_each(component_runtimes, [](auto& r){clear_output_flags(r.component);});
    }
};

}

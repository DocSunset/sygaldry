#pragma once

#include <boost/mp11.hpp>
#include "concepts/functions.hpp"
#include "concepts/components.hpp"
#include <iostream>

namespace sygaldry {


template<typename ComponentContainer>
struct Runtime
{
    ComponentContainer container;

    template<typename Component, typename ... Args>
    struct runtime_impl
    {
        using arg_pack_t = decltype(std::forward_as_tuple(find<Args>(std::declval<ComponentContainer&>())...));
        const arg_pack_t arg_pack;

        constexpr runtime_impl(ComponentContainer& container) : arg_pack{std::forward_as_tuple(find<Args>(container) ...)} {}

        void init(Component& component) const
        {
            if constexpr (requires {&Component::init;})
                std::apply([&](auto& ... args) {component.init(args...);}, arg_pack);
        }

        void main(Component& component) const
        {
            if constexpr (requires {&Component::operator();})
                std::apply(component, arg_pack);
            else if constexpr (requires {&Component::main;})
                std::apply([&](auto& ... args) {component.main(args...);}, arg_pack);
        }
    };
    template<typename...>
    struct to_impl
    {
        struct dummy
        {
            void init(auto&) const {};
            void main(auto&) const {};
        };
        using impl_t = dummy;
    };

    template<typename Component, typename FuncRefl>
        requires FuncRefl::exists::value
    struct to_impl<Component, FuncRefl>
    {
        using args = typename FuncRefl::arguments;
        using cvref_less = boost::mp11::mp_transform<std::remove_cvref_t, args>;
        using prepended = boost::mp11::mp_push_front<cvref_less, Component, ComponentContainer>;
        using impl_t = boost::mp11::mp_rename<prepended, runtime_impl>;
    };
    template<typename Component>
    struct component_runtime
    {
        using main_impl_t = typename to_impl<Component, main_subroutine_reflection<Component>>::impl_t;
        const main_impl_t main_impl;

        using init_impl_t = typename to_impl<Component, init_subroutine_reflection<Component>>::impl_t;
        const init_impl_t init_impl;

        Component& component;

        constexpr component_runtime(Component& c, ComponentContainer& container)
        : component{c}
        , main_impl{container}
        , init_impl{container}
        {};

        void init() const
        {
            init_impl.init(component);
        };

        void main() const
        {
            main_impl.main(component);
        };
    };

    using component_list_t = decltype(component_filter_by_tag<node::component>(std::declval<ComponentContainer&>()));
    using component_runtime_list_t = boost::mp11::mp_transform
            < component_runtime
            , component_list_t
            >;
    component_runtime_list_t component_runtimes;

    constexpr Runtime()
    : container{}
    , component_runtimes{tuple_transform
            ( [&]<typename T>(T& tagged_component)
              {
                  return component_runtime<typename T::type>{tagged_component.ref, container};
              }
            , component_filter_by_tag<node::component>(std::declval<ComponentContainer&>())
            )}
    {};

    void init()
    {
        tuple_for_each(component_runtimes, [](auto& r){r.init();});
    }

    void main()
    {
        tuple_for_each(component_runtimes, [](auto& r){r.main();});
    }
};

}

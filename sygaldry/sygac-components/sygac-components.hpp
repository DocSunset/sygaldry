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

/*! \addtogroup sygac sygac: Sygaldry Concepts
 */
/// \{

/*! \defgroup sygac-components sygac-components: Components Concepts
 */
/// \{

/*! \brief Ensure T is not a union and is either scalar or aggregate. Doesn't catch inheritance!

Note that this concept does not catch when T is a derived type; in this
case you will likely trip an unholy compiler error with a long chain of template
instantiations caused by an assertion in boost::pfr. The solution is to ensure
that your type T does not inherit from any other type.

*/
template<typename T>
concept SimpleAggregate
    =  not std::is_union_v<T>
    && (  std::is_aggregate_v<T>
       || std::is_scalar_v<T>
       )
    ;

/// Default case for main subroutine reflection where the main subroutine does not exist.
template <typename T> struct main_subroutine_reflection {using exists = std::false_type;};

/// Main subroutine reflection for function call operator; this takes precedence over a method called "main"
template <typename T>
    requires (std::same_as<void, typename function_reflection<&T::operator()>::return_type>
          && !std::same_as<void, typename function_reflection<&T::main>::return_type>)
struct main_subroutine_reflection<T> : function_reflection<&T::operator()> {};

/// Main subroutine reflection for a method called "main"
template <typename T>
    requires std::same_as<void, typename function_reflection<&T::main>::return_type>
struct main_subroutine_reflection<T> : function_reflection<&T::main> {};
/// Default case for init subroutine reflection where the init subroutine does not exist.
template <typename T> struct init_subroutine_reflection {using exists = std::false_type;};

/// Init subroutine reflection for a method called "init"
template <typename T>
    requires std::same_as<void, typename function_reflection<&T::init>::return_type>
struct init_subroutine_reflection<T> : function_reflection<&T::init> {};

/// Default case for external sources subroutine reflection where the subroutine does not exist.
template <typename T> struct external_sources_subroutine_reflection {using exists = std::false_type;};

/// External sources subroutine reflection for a method of the same name
template <typename T>
    requires std::same_as<void, typename function_reflection<&T::external_sources>::return_type>
struct external_sources_subroutine_reflection<T> : function_reflection<&T::external_sources> {};

/// Default case for external destinations subroutine reflection where the subroutine does not exist.
template <typename T> struct external_destinations_subroutine_reflection {using exists = std::false_type;};

/// External destinations subroutine reflection for a method of the same name
template <typename T>
    requires std::same_as<void, typename function_reflection<&T::external_destinations>::return_type>
struct external_destinations_subroutine_reflection<T> : function_reflection<&T::external_destinations> {};
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

#undef has_type_or_value


/// Check if T is a Sygaldry component
template<typename T>
concept Component
    =  has_name<T>
    &&  (  has_init_subroutine<T>
        || has_external_sources_subroutine<T>
        || has_main_subroutine<T>
        || has_external_destinations_subroutine<T>
        || has_inputs<T>
        || has_outputs<T>
        )
    ;

namespace detail {

/// Base metafunction case for most types; most types are not assemblies
template<typename T>
struct assembly_predicate : std::false_type {};
/// Metafunction case for components; components are considered as assemblies for the purpose of this recursive predicate
template<typename T>
    requires Component<T>
struct assembly_predicate<T> : std::true_type {};

/// Metafunction case where T satisfies `SimpleAggregate` (but not `Component`); checks if all its members are valid
template<typename T>
    requires SimpleAggregate<T> && (not Component<T>) && (not std::is_scalar_v<T>)
struct assembly_predicate<T>
{
    using tup = decltype(boost::pfr::structure_to_tuple(std::declval<T&>()));
    using valid_components = boost::mp11::mp_transform<assembly_predicate, tup>;
    using all_valid = boost::mp11::mp_apply<boost::mp11::mp_all, valid_components>;
    static constexpr bool value = all_valid::value;
};
}

template<typename T>
concept Assembly = detail::assembly_predicate<T>::value && not Component<T>;

namespace node
{
    struct assembly {};
    struct component {};
    struct inputs_container {};
    struct outputs_container {};
    struct endpoints_container {};
    struct input_endpoint {};
    struct output_endpoint {};
    struct endpoint {};
    template<typename> struct is_assembly : std::false_type {};
    template<>         struct is_assembly<assembly> : std::true_type {};
    template<typename> struct is_component : std::false_type {};
    template<>         struct is_component<component> : std::true_type {};
    template<typename> struct is_inputs_container : std::false_type {};
    template<>         struct is_inputs_container<inputs_container> : std::true_type {};
    template<typename> struct is_outputs_container : std::false_type {};
    template<>         struct is_outputs_container<outputs_container> : std::true_type {};
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

template<typename Tag, typename Val>
struct tagged
{
    using tag = Tag;
    using type = Val;
    Val& ref;
};
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
    if constexpr (Component<T>)
    {
        return std::tuple_cat( std::make_tuple(tagged<node::component, T>{component})
                             , endpoint_subtree<node::input_endpoint>(component)
                             , endpoint_subtree<node::output_endpoint>(component)
                             );
    }
    else
    {
        auto subcomponents = boost::pfr::structure_tie(component);
        auto head = std::make_tuple(tagged<node::assembly, T>{component});
        auto tail = tuple_transform([](auto& subcomponent)
        {
            return component_to_tree(subcomponent);
        }, subcomponents);
        return std::tuple_cat( head, tail);
    }
}
template<Tuple T>
constexpr auto component_tree_to_node_list(T tree)
{
    if constexpr (std::tuple_size_v<T> == 0) return tree;
    auto head = tuple_head(tree);
    auto tail = tuple_tail(tree);
    if constexpr (Tuple<decltype(head)>) return std::tuple_cat(component_tree_to_node_list(head), component_tree_to_node_list(tail));
    else return std::tuple_cat(std::make_tuple(head), component_tree_to_node_list(tail));
}
template<typename T>
constexpr auto component_to_node_list(T& component)
{
    return component_tree_to_node_list(component_to_tree(component));
}
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
template<template<typename>typename F, typename T>
constexpr auto component_filter(T& component)
{
    return node_list_filter<F>(component_to_node_list(component));
}
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
template<typename T, Tuple Tup>
constexpr auto path_of(const Tup tree)
{
    if constexpr (std::tuple_size_v<Tup> == 0) // tail of a leaf node
        return std::tuple<>{};

    // split the tuple into its head and its tail
    auto head = tuple_head(tree);
    auto tail_path = path_of<T>(tuple_tail(tree));

    if constexpr (Tuple<decltype(head)>) // search subtrees
        return std::tuple_cat(path_of<T>(head), tail_path);
    else if constexpr (std::same_as<typename decltype(head)::type, T>) // the node that we are looking for
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
    requires Component<C> || Assembly<C>
constexpr auto path_of(C& component)
{
    return path_of<T>(component_to_tree(component));
}
template<typename Tag> using untagged = typename Tag::type;

// TODO: shouldn't this return references?
template<Tuple T>
constexpr auto remove_node_tags(T tup)
{
    using return_type = mp_transform<untagged, T>;
    return std::make_from_tuple<return_type>(tuple_transform([](auto&& tagged) {return tagged.ref;}, tup));
}
template<typename T> using output_endpoints_t =
    decltype(remove_node_tags(component_filter_by_tag<node::output_endpoint>(std::declval<T&>())));
template<typename T> using input_endpoints_t =
    decltype(remove_node_tags(component_filter_by_tag<node::input_endpoint>(std::declval<T&>())));
template<typename T> using endpoints_t =
    decltype(remove_node_tags(component_filter_by_tag<node::input_endpoint, node::output_endpoint>(std::declval<T&>())));
template<typename T, typename C> using path_t =
    decltype(remove_node_tags(path_of<T>(std::declval<C&>())));

template<typename T, typename ... RequestedNodes>
constexpr auto for_each_node(T& component, auto callback)
{
    using boost::mp11::mp_list;
    using boost::mp11::mp_contains;
    using boost::mp11::mp_empty;

    using nodes = mp_list<RequestedNodes...>;

    if constexpr (Assembly<T>)
    {
        if constexpr (  mp_empty<nodes>::value
                     || mp_contains<nodes, node::assembly>::value
                     )  callback(component, node::assembly{});
        boost::pfr::for_each_field(component, [&]<typename S>(S& subcomponent)
        {
            for_each_node<S, RequestedNodes...>(subcomponent, callback);
        });
    }
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
    }
}
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

template<Component T>
void init(T& component)
{
    if constexpr (requires {component.init();}) component.init();
}

template<Assembly T>
void init(T& container)
{
    for_each_component(container, [](auto& component) {init(component);});
};

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

template<Assembly T>
void activate(T& container)
{
    clear_output_flags(container);
    for_each_component(container, activate_inner);
    clear_input_flags(container);
};

/// \}
/// \}

}

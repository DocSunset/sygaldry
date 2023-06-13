#pragma once

#include <boost/pfr.hpp>
#include <boost/mp11.hpp>
#include "concepts/metadata.hpp"
#include "concepts/functions.hpp"
#include "concepts/endpoints.hpp"

namespace sygaldry { namespace concepts {

using boost::mp11::mp_and;
using boost::mp11::mp_any;
using boost::mp11::mp_append;
using boost::mp11::mp_apply;
using boost::mp11::mp_bool;
using boost::mp11::mp_copy_if;
using boost::mp11::mp_empty;
using boost::mp11::mp_flatten;
using boost::mp11::mp_if;
using boost::mp11::mp_if_c;
using boost::mp11::mp_or;
using boost::mp11::mp_transform;
using boost::mp11::mp_transform_q;

template<typename T>
concept SimpleAggregate
    =  not std::is_union_v<T>
    && (  std::is_aggregate_v<T>
       || std::is_scalar_v<T>
       )
    ;

template<SimpleAggregate T> struct aggregate_members
{
    using agg_t = std::remove_cvref_t<T>;
    using type = decltype(boost::pfr::structure_to_tuple(std::declval<agg_t>()));
};

template<typename T> using aggregate_members_t = aggregate_members<T>::type;
#define has_type_or_value(NAME)\
template<typename T> concept has_##NAME##_member = requires (T t) { t.NAME; requires SimpleAggregate<decltype(t.NAME)>; };\
template<typename T> concept has_##NAME##_type = requires { typename T::NAME; requires SimpleAggregate<typename T::NAME>; };\
template<typename T>\
concept has_##NAME \
    =  has_##NAME##_member<T>\
    || has_##NAME##_type<T>;\
\
template<typename T> struct type_of_##NAME { using type = struct{}; };\
template<has_##NAME##_member T> struct type_of_##NAME<T>\
{\
    using type = decltype(std::declval<T>().NAME);\
};\
template<has_##NAME##_type T> struct type_of_##NAME<T>\
{\
    using type = typename T::NAME;\
};\
\
template<typename T> using type_of_##NAME##_t = type_of_##NAME<T>::type;\
\
template<typename T> requires has_##NAME##_member<T> auto& NAME##_of(T& t) { return t.NAME; }\
template<typename T> requires has_##NAME##_member<T> const auto& NAME##_of(const T& t) { return t.NAME; }

has_type_or_value(inputs);
has_type_or_value(outputs);
has_type_or_value(parts);

#undef has_type_or_value
template<typename T>
concept has_main_subroutine
    =  std::same_as<void, typename function_reflection<&T::operator()>::return_type>
    || std::same_as<void, typename function_reflection<&T::main>::return_type>
    ;

template<typename T>
concept Component = has_main_subroutine<T> && has_name<T> &&
        ( has_inputs<T> || has_outputs<T> || has_parts<T>)
        // TODO || has_throughpoints<T> || has_plugins<T>
        ;


template<typename T> struct contains_component : std::false_type {};
template<Component T> struct contains_component<T> : std::true_type {};
template<typename T>
    requires SimpleAggregate<T> and (not Component<T>)
struct contains_component<T>
{
    using members = aggregate_members_t<T>;
    using type = mp_apply<mp_any, mp_transform<contains_component, members>>;
    static constexpr const bool value = type::value;
};

template <typename T> constexpr const bool contains_component_v = contains_component<T>::value;

template<typename T>
concept ComponentContainer = (not Component<T>)
    && SimpleAggregate<T> && contains_component_v<T>
    ;

namespace node
{
    struct component_container {};
    struct component {};
    struct inputs_container {};
    struct outputs_container {};
    struct endpoints_container {};
    struct parts_container {};
    struct input_endpoint {};
    struct output_endpoint {};
    struct endpoint {};
    template<typename> constexpr bool is_component_container = false;
    template<> constexpr bool is_component_container<component_container> = true;
    template<typename> constexpr bool is_component = false;
    template<> constexpr bool is_component<component> = true;
    template<typename> constexpr bool is_inputs_container = false;
    template<> constexpr bool is_inputs_container<inputs_container> = true;
    template<typename> constexpr bool is_outputs_container = false;
    template<> constexpr bool is_outputs_container<outputs_container> = true;
    template<typename> constexpr bool is_parts_container = false;
    template<> constexpr bool is_parts_container<parts_container> = true;
    template<typename> constexpr bool is_input_endpoint = false;
    template<> constexpr bool is_input_endpoint<input_endpoint> = true;
    template<typename> constexpr bool is_output_endpoint = false;
    template<> constexpr bool is_output_endpoint<output_endpoint> = true;
    template<typename> constexpr bool is_endpoints_container = false;
    template<> constexpr bool is_endpoints_container<inputs_container> = true;
    template<> constexpr bool is_endpoints_container<outputs_container> = true;
    template<> constexpr bool is_endpoints_container<endpoints_container> = true;
    template<typename> constexpr bool is_endpoint = false;
    template<> constexpr bool is_endpoint<input_endpoint> = true;
    template<> constexpr bool is_endpoint<output_endpoint> = true;
    template<> constexpr bool is_endpoint<endpoint> = true;
}

template<typename T, typename ... RequestedNodes>
    requires Component<T> || ComponentContainer<T>
constexpr auto for_each_node(T& component, auto callback)
{
    using boost::mp11::mp_list;
    using boost::mp11::mp_contains;
    using boost::mp11::mp_empty;

    using nodes = mp_list<RequestedNodes...>;

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

template<typename Tag, typename Val>
struct tagged
{
    using tag = Tag;
    using type = Val;
};

template<typename Tag>
struct tag_with
{
    template<typename T> using fn = tagged<Tag, T>;
};

template<typename T> struct subnodes { using list = std::tuple<>; };

template<typename T> using get_list_t = T::list;

template<ComponentContainer T> struct subnodes<T>
{
    using members = aggregate_members_t<T>;
    using subcomponents = mp_copy_if<members, contains_component>;
    using tagged_components = mp_transform_q<tag_with<node::component>, subcomponents>;
    using subsubnodes = mp_transform<subnodes, subcomponents>;
    using sublists = mp_transform<get_list_t, subsubnodes>;
    using interleaved = mp_transform<std::tuple, tagged_components, sublists>;
    using list = mp_flatten<mp_flatten<interleaved>>;
};
template<Component T> struct subnodes<T>
{
    using inputs_t  = mp_if_c<has_inputs<T>,  std::tuple<tagged<node::inputs_container,  type_of_inputs_t<T>>>,  std::tuple<>>;
    using outputs_t = mp_if_c<has_outputs<T>, std::tuple<tagged<node::outputs_container, type_of_outputs_t<T>>>, std::tuple<>>;
    using parts_t   = mp_if_c<has_parts<T>,   std::tuple<tagged<node::parts_container,   type_of_parts_t<T>>>,   std::tuple<>>;
    using inputs  = mp_if_c<has_inputs<T>,  aggregate_members_t<type_of_inputs_t<T>>,  std::tuple<>>;
    using outputs = mp_if_c<has_outputs<T>, aggregate_members_t<type_of_outputs_t<T>>, std::tuple<>>;
    using parts   = mp_if_c<has_parts<T>,   aggregate_members_t<type_of_parts_t<T>>,   std::tuple<>>;
    using component_parts = mp_copy_if<parts, contains_component>;
    using tagged_inputs  = mp_transform_q<tag_with<node::input_endpoint>,  inputs>;
    using tagged_outputs = mp_transform_q<tag_with<node::output_endpoint>, outputs>;
    using tagged_parts = mp_transform_q<tag_with<node::component>, component_parts>;
    using subsubnodes = mp_transform<subnodes, component_parts>;
    using sublists = mp_transform<get_list_t, subsubnodes>;
    using interleaved_components = mp_transform<std::tuple, tagged_parts, sublists>;
    using list = mp_append<inputs_t, tagged_inputs
                          , outputs_t, tagged_outputs
                          , parts_t, mp_flatten<mp_flatten<interleaved_components>>>;
};

template<typename T> using subnodes_t = subnodes<T>::list;

void clear_output_flags(auto& component)
{
    for_each_output(component, []<typename Y>(Y& endpoint)
    {
        if constexpr (ClearableFlag<Y>) clear_flag(endpoint);
    });
}

void clear_input_flags(auto& component)
{
    for_each_input(component, []<typename Y>(Y& endpoint)
    {
        if constexpr (ClearableFlag<Y>) clear_flag(endpoint);
    });
}

template<Component T>
void activate(T& component)
{
    clear_output_flags(component);
    if constexpr (requires {component.main(component.inputs, component.outputs);})
        component.main(component.inputs, component.outputs);
    else if constexpr (requires {component(component.inputs, component.outputs);})
        component(component.inputs, component.outputs);
    else if constexpr (requires {component();})
        component();
    clear_input_flags(component);
}

} }

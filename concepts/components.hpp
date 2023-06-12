#pragma once

#include <boost/pfr.hpp>
#include <boost/mp11.hpp>
#include "concepts/metadata.hpp"
#include "concepts/functions.hpp"
#include "concepts/endpoints.hpp"

namespace sygaldry { namespace concepts {

template<typename T>
concept SimpleAggregate
    =  not std::is_union_v<T>
    && (  std::is_aggregate_v<T>
       || std::is_scalar_v<T>
       )
    ;
#define has_type_or_value(NAME)\
template<typename T> concept has_##NAME##_member = requires (T t) { t.NAME; requires SimpleAggregate<decltype(t.NAME)>; };\
template<typename T> concept has_##NAME##_type = requires { typename T::NAME; requires SimpleAggregate<typename T::NAME>; };\
template<typename T> concept has_##NAME##_t = requires { typename T::NAME##_t; requires SimpleAggregate<typename T::NAME##_t>; };\
template<typename T>\
concept has_##NAME \
    =  has_##NAME##_member<T>\
    || has_##NAME##_type<T>\
    || has_##NAME##_t<T>;\
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
        ( has_inputs<T> || has_outputs<T> || has_parts<T>
        // TODO || has_throughpoints<T> || has_plugins<T>
        );

template<typename T>
concept ComponentContainer = SimpleAggregate<T>;

template<typename T>
    requires Component<T> || ComponentContainer<T>
constexpr auto for_each_component(T& component, auto callback)
{
    if constexpr (Component<T>)
    {
        callback(component);
        if constexpr (has_parts<T>)
            for_each_component(parts_of(component), callback);
    } else /* ComponentContainer */
    {
        boost::pfr::for_each_field(component, [&]<typename Y>(Y& subcomponent) {
            if constexpr (Component<Y> || ComponentContainer<Y>)
                for_each_component(subcomponent, callback);
        });
    }
}

template<typename T>
    requires Component<T> || ComponentContainer<T>
constexpr auto for_each_endpoint(T& component, auto callback)
{
    for_each_component(component, [&]<typename Y>(Y& subcomponent)
    {
        if constexpr (has_inputs<Y>)
            boost::pfr::for_each_field(inputs_of(subcomponent), callback);
        if constexpr (has_outputs<Y>)
            boost::pfr::for_each_field(outputs_of(subcomponent), callback);
    });
}

template<typename T>
    requires Component<T> || ComponentContainer<T>
constexpr auto for_each_input(T& component, auto callback)
{
    for_each_component(component, [&]<typename Y>(Y& subcomponent)
    {
        if constexpr (has_inputs<Y>)
            boost::pfr::for_each_field(inputs_of(subcomponent), callback);
    });
}

template<typename T>
    requires Component<T> || ComponentContainer<T>
constexpr auto for_each_output(T& component, auto callback)
{
    for_each_component(component, [&]<typename Y>(Y& subcomponent)
    {
        if constexpr (has_outputs<Y>)
            boost::pfr::for_each_field(outputs_of(subcomponent), callback);
    });
}
namespace node
{
    struct component_container {};
    struct component {};
    struct inputs_container {};
    struct outputs_container {};
    struct parts_container {};
    struct input_endpoint {};
    struct output_endpoint {};
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
    template<typename> constexpr bool is_endpoint = false;
    template<> constexpr bool is_endpoint<input_endpoint> = true;
    template<> constexpr bool is_endpoint<output_endpoint> = true;
}

template<typename T>
constexpr auto for_each_node(T& component, auto callback)
{
    if constexpr (Component<T>)
    {
        callback(component, node::component{});
        if constexpr (has_inputs<T>)
        {
            auto& inputs = inputs_of(component);
            callback(inputs, node::inputs_container{});
            boost::pfr::for_each_field(inputs, [&](auto& in)
            {
                callback(in, node::input_endpoint{});
            });
        }
        if constexpr (has_outputs<T>)
        {
            auto& outputs = outputs_of(component);
            callback(outputs, node::outputs_container{});
            boost::pfr::for_each_field(outputs, [&](auto& out)
            {
                callback(out, node::output_endpoint{});
            });
        }
        if constexpr (has_parts<T>)
        {
            auto& parts = parts_of(component);
            callback(parts, node::parts_container{});
            boost::pfr::for_each_field(parts, [&](auto& part)
            {
                for_each_node(part, callback);
            });
        }
    } else if constexpr (ComponentContainer<T>)
    {
        callback(component, node::component_container{});
        boost::pfr::for_each_field(component, [&](auto& subcomponent)
        {
            for_each_node(subcomponent, callback);
        });
    } else return; // this should never happen
}

template<typename T>
void clear_flags(T& entities)
{
    if constexpr (Component<T>)
    {
        clear_flags(inputs_of(entities));
        clear_flags(outputs_of(entities));
    }
    else
    {
        boost::pfr::for_each_field(entities, []<typename Y>(Y& endpoint)
        {
            if constexpr (ClearableFlag<Y>) clear_flag(endpoint);
        });
    }
}

template<Component T>
void activate(T& component)
{
    clear_flags(outputs_of(component));
    if constexpr (requires {component.main(component.inputs, component.outputs);})
        component.main(component.inputs, component.outputs);
    else if constexpr (requires {component(component.inputs, component.outputs);})
        component(component.inputs, component.outputs);
    else if constexpr (requires {component();})
        component();
    clear_flags(inputs_of(component));
}

} }

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
concept RegularComponent = has_main_subroutine<T> && has_name<T> &&
        ( has_inputs<T> || has_outputs<T> || has_parts<T>
        // TODO || has_throughpoints<T> || has_plugins<T>
        );

template<typename T>
concept ContainerComponent = SimpleAggregate<T>;

//template<typename T>
//concept Component = RegularComponent<T> || ContainerComponent<T>;
template<typename T>
concept Component = requires (T t) {t.inputs; t.outputs;};

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

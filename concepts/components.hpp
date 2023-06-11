#pragma once

#include <boost/pfr.hpp>
#include "concepts/endpoints.hpp"

namespace sygaldry { namespace concepts {

// TODO it may make sense to move this into its own header
template<typename ... L> struct type_list {};

template<typename NotAFunction>
struct func_reflection { static constexpr bool exists = false; };

template<typename R, typename... Args>
struct func_reflection<R(Args...)> {
    using ret = R;
    using args = type_list<Args...>;
    static constexpr bool exists = true;
};

template<typename R, typename... Args>
struct func_reflection<R(*)(Args...)> : func_reflection<R(Args...)> {};

template<typename R, typename Class, typename... Args>
struct func_reflection<R(Class::*)(Args...)> : func_reflection<R(Args...)> {};

template<typename R, typename Class, typename... Args>
struct func_reflection<R(Class::*)(Args...) const> : func_reflection<R(Args...)> {};

template<auto Val> using func_ret_t = func_reflection<decltype(Val)>::ret;
template<auto Val> using func_args_t = func_reflection<decltype(Val)>::args;

template<typename ... Args>
concept func_reflectable = func_reflection<Args...>::exists;
template<typename T>
concept has_main_subroutine
    =  std::is_same_v<void, func_ret_t<&T::operator()>>
    || std::is_same_v<void, func_ret_t<&T::main>>;

//template<typename T>
//concept RegularComponent = has_main_subroutine<T> && has_name<T> &&
//        ( has_inputs<T> || has_outputs<T> || has_parts<T>
//        // TODO || has_throughpoints<T> || has_plugins<T>
//        );
//
//template<typename T>
//concept PureAssembly = (is_simple_aggregate<T> && has_only_endpoints<T>) ||
//    ( has_parts<T> && not ( has_main_subroutine<T>
//    || has_inputs<T> || has_outputs<T>
//    // TODO || has_throughpoints<T> || has_plugins<T>
//    ) );
//
//template<typename T>
//concept Component = RegularComponent<T> || PureAssembly<T>;
template<typename T>
concept Component = requires (T t) {t.inputs; t.outputs;};

template<Component T>
auto& inputs_of(T& component)
{
    return component.inputs;
};

template<Component T>
auto& outputs_of(T& component)
{
    return component.outputs;
};

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

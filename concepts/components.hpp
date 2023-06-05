#pragma once

#include <boost/pfr.hpp>
#include "concepts/endpoints.hpp"

namespace sygaldry { namespace concepts {

template <typename T>
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

#pragma once
#include <tuple>
#include <string_view>
#include <boost/pfr.hpp>
#include "concepts/components.hpp"

namespace sygaldry { namespace bindings {

struct dispatch_default_matcher
{
    template<typename stringish, typename NamedT>
    bool operator()(stringish name, const NamedT& candidate)
    {
        return std::string_view(name) == std::string_view(NamedT::name());
    }
};

// TODO: these concept definitions probably don't belong here

template<typename T>
concept tuple_like = requires (T tup)
{
    std::tuple_size<T>::value;
    typename std::tuple_element<0, T>::type;

};

template<typename Matcher, typename stringish, typename Default, typename Callback, typename NamedT, typename ... NamedTs>
auto _dispatch_impl(stringish name, Default&& d, Callback&& f, NamedT&& t, NamedTs&&... ts)
{
    if (Matcher{}(name, t))
        return f(t);
    else if constexpr (sizeof...(NamedTs) == 0)
        return d;
    else
        return _dispatch_impl<Matcher>(name, d, f, ts...);
}

template <typename Matcher = dispatch_default_matcher, typename stringish, typename TupleOfNamed, typename Default, typename Callback>
auto dispatch_tuple(stringish name, TupleOfNamed& tup, Default&& d, Callback&& f)
{
    if constexpr (std::tuple_size_v<TupleOfNamed> == 0) return d;
    else return std::apply([&](auto&& ... ts)
    {
        return _dispatch_impl<Matcher>(name, d, f, ts...);
    }, tup);
}

template <typename Matcher = dispatch_default_matcher, typename stringish, typename Entities, typename Default, typename Callback>
auto dispatch(stringish name, Entities& entities, Default&& d, Callback&& f)
{
    if constexpr (Component<Entities>)
    {
        if constexpr (has_inputs<Entities> && has_outputs<Entities>)
        {
            auto ins = boost::pfr::structure_tie(inputs_of(entities));
            auto outs = boost::pfr::structure_tie(outputs_of(entities));
            auto tup = std::tuple_cat(ins, outs);
            return dispatch_tuple<Matcher>(name, tup, d, f);
        }
        else if constexpr (has_inputs<Entities>)
        {
            auto tup = boost::pfr::structure_tie(inputs_of(entities));
            return dispatch_tuple<Matcher>(name, tup, d, f);
        }
        else if constexpr (has_outputs<Entities>)
        {
            auto tup = boost::pfr::structure_tie(outputs_of(entities));
            return dispatch_tuple<Matcher>(name, tup, d, f);
        }
        else return d;
    }
    else // Entities is assumed to be a struct of endpoints
    {
        auto tup = boost::pfr::structure_tie(entities);
        return dispatch_tuple<Matcher>(name, tup, d, f);
    }
}

} }

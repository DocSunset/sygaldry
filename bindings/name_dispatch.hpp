#pragma once
#include <tuple>
#include <string_view>
#include <boost/pfr.hpp>
#include "utilities/spelling.hpp"

namespace sygaldry
{
namespace bindings
{

struct dispatch_default_matcher
{
    template<typename stringish, typename NamedT>
    bool operator()(stringish name, const NamedT& candidate)
    {
        return std::string_view(name) == std::string_view(NamedT::name());
    }
};

// TODO: these concept definitions probably don't belong here

template <typename T>
concept Component = requires (T t) {t.inputs; t.outputs;};

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

template <typename Matcher = dispatch_default_matcher, typename stringish, tuple_like TupleOfNamed, typename Default, typename Callback>
auto dispatch(stringish name, TupleOfNamed& tup, Default&& d, Callback&& f)
{
    if constexpr (std::tuple_size_v<TupleOfNamed> == 0) return d;
    return std::apply([&]<typename ... NamedTs>(NamedTs&& ... ts)
    {
        return _dispatch_impl<Matcher>(name, d, f, ts...);
    }, tup);
}

template <typename Matcher = dispatch_default_matcher, typename stringish, typename Entities, typename Default, typename Callback>
auto dispatch(stringish name, Entities& entities, Default&& d, Callback&& f)
{
    if constexpr (Component<Entities>)
    {
        auto ins = boost::pfr::structure_tie(entities.inputs);
        auto outs = boost::pfr::structure_tie(entities.outputs);
        auto tup = std::tuple_cat(ins, outs);
        return dispatch<Matcher>(name, tup, d, f);
        return d;
    }
    else
    {
        auto tup = boost::pfr::structure_tie(entities);
        return dispatch<Matcher>(name, tup, d, f);
    }
}

}
}

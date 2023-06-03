#pragma once

namespace sygaldry::bindings::cli
{

template<typename stringish, typename Default, typename Callback, typename NamedT, typename ... NamedTs>
auto _try_to_match_and_execute_impl(stringish name, Default&& d, Callback&& f, NamedT&& t, NamedTs&&... ts)
{
    if (std::string_view(name) == std::string_view(t.name()))
        return f(t);
    else if constexpr (sizeof...(NamedTs) == 0)
        return d;
    else
        return _try_to_match_and_execute_impl(name, d, f, ts...);
}

template <typename stringish, typename TupleOfNamed, typename Default, typename Callback>
auto try_to_match_and_execute(stringish name, TupleOfNamed& tup, Default&& d, Callback&& f)
{
    if constexpr (std::tuple_size_v<TupleOfNamed> == 0) return d; // no impl if no t
    else return std::apply([&]<typename ... NamedTs>(NamedTs&& ... ts)
    {
        return _try_to_match_and_execute_impl(name, d, f, ts...);
    }, tup);
}

}

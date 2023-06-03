#pragma once

#include <concepts>
#include "utilities/consteval/consteval.hpp"

namespace sygaldry::concepts
{
    template<typename T>
    concept Named
        =  requires { {T::name()} -> std::convertible_to<std::string>; }
        || requires { {T::name()} -> std::convertible_to<std::string_view>; }
        || requires { {T::name()} -> std::convertible_to<const char *>; }
        ;

    template<Named T>
    _consteval auto get_name(const T&) { return T::name(); }

    template<Named T>
    _consteval auto get_name() { return T::name(); }
}

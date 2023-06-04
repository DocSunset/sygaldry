#pragma once
#include <cstddef>
#include "utilities/consteval.hpp"

namespace sygaldry::utilities
{
template<std::size_t N>
struct string_literal
{
    static _consteval auto size() {return N;}
    char data[N];
    _consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) data[i] = str[i];
    }
    operator const char *() {return data;}
};
}

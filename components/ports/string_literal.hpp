#pragma once

#include <cstddef>

namespace sygaldry
{
namespace ports
{

template<std::size_t N>
struct string_literal
{
    char value[N];
    consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

}
}

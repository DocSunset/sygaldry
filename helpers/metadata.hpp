#pragma once

#include <cstddef>
#include "utilities/consteval.hpp"

namespace sygaldry {

template<std::size_t N>
struct string_literal
{
    char value[N];
    _consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

#define metadata_struct(NAME) template<string_literal str>\
struct NAME##_\
{\
    static _consteval auto NAME() {return str.value;}\
}

metadata_struct(name);
metadata_struct(author);
metadata_struct(email);
metadata_struct(license);
metadata_struct(copyright);
metadata_struct(description);
metadata_struct(uuid);
metadata_struct(unit);
metadata_struct(version);
metadata_struct(date);

#undef metadata_struct

}

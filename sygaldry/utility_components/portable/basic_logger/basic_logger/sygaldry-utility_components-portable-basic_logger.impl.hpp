/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygaldry-utility_components-portable-basic_logger.hpp"

namespace sygaldry { namespace utility_components {

template<typename putter>
struct BasicLogger
{
    [[no_unique_address]] putter put;

    template<typename T> void print_(T& x)
    {
        using T = std::remove_cvref_t<cvrT>;

        constexpr int max_num_digits =
        (
            // floats: mantissa digits + exponent digits + sign + e + exponent sign
            std::is_floating_point_v<T> ? std::numeric_limits<T>::max_digits10
                                          + std::numeric_limits<T>::max_exponent10 + 3

            // ints: digits + sign
            : (std::numeric_limits<T>::digits10 + std::is_signed_v<T>) ? 1 : 0
        );

        constexpr int buffer_size = max_num_digits + 16; // Extra padding for safety

        char buffer[buffer_size];

        std::string_view string_message;
        if constexpr (std::is_same_v<T, bool>)
        {
            string_message = x ? "true" : "false";
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            auto [ptr, ec] = std::to_chars(buffer, buffer + buffer_size, x);

            if (ec == std::errc()) {
                string_message = std::string_view(buffer, ptr - buffer);
            } else {
                string_message = "error:\n\
                    bindings/basic_logger/basic_logger.lili:'convert a number'\n\
                    std::to_chars scratch buffer unexpectedly too small!";
            }
        }
        else if constexpr (requires {string_message = x;})
            string_message = x;
        else if constexpr (requires {x[0]; x.size();})
        {
            print("[", x[0]);
            for (std::size_t i = 1; i < x.size(); ++i) print(" ", x[i]);
            print("]");
            return;
        }
        else string_message = "unknown type for basic logger";

        for (char c : string_message)
            put(c);
    };

    template<typename ... Ts> void print(Ts... x)
    {
        (print_(x), ...);
    }

    template<typename ... Ts> void println(Ts... x)
    {
        print(x...);
        print("\n");
    }
};

} }

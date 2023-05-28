#pragma once
#include <type_traits>
#include <string_view>
#include <limits>
#include <charconv>

namespace bindings::basic_logger
{

template<typename putter>
struct BasicLogger
{
    [[no_unique_address]] putter put;

    template<typename T> void _print(T x)
    {
        std::string_view string_message;
        if constexpr (std::is_arithmetic_v<T>)
        {
            std::is_signed_v<T>;
            ;
            constexpr int max_num_digits =
            (
                // floats: mantissa digits + exponent digits + sign + e + exponent sign
                std::is_floating_point_v<T> ? std::numeric_limits<T>::max_digits10
                                              + std::numeric_limits<T>::max_exponent10 + 3

                // ints: digits + sign
                : std::numeric_limits<T>::digits10 + std::is_signed_v<T> ? 1 : 0
            );

            constexpr int buffer_size = max_num_digits + 16; // Extra padding for safety

            char buffer[buffer_size];
            auto [ptr, ec] = std::to_chars(buffer, buffer + buffer_size, x);

            if (ec == std::errc()) {
                string_message = std::string_view(buffer, ptr - buffer);
            } else {
                string_message = "error:\n\
                    bindings/basic_logger/basic_logger.litlit:'convert a number'\n\
                    std::to_chars scratch buffer unexpectedly too small!";
            }
        }
        else if constexpr (std::is_same_v<T, std::string_view>
                           or std::is_same_v<T, char *>
                           or std::is_same_v<T, const char *>)
        {
            string_message = x;
        }

        for (char c : string_message)
            put(c);
    };

    template<typename ... Ts> void print(Ts... x)
    {
        (_print(x), ...);
    }

    template<typename ... Ts> void println(Ts... x)
    {
        print(x...);
        print("\n");
    }
};

}

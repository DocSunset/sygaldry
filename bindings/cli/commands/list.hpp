#pragma once

#include <array>
#include "utilities/metadata/names/names.hpp"

namespace bindings::cli::commands
{
namespace detail
{
using utilities::metadata::names::name_length;

template<typename ... Devices>
struct list_output_length
{
    static constexpr size_t value = 1;
};

template<typename Device, typename ... Devices>
struct list_output_length<Device, Devices...>
{
    static constexpr size_t value = name_length<Device>() + 1
        + list_output_length<Devices...>::value;
};

template<typename ... Devices>
constexpr size_t list_output_length_v = list_output_length<Devices...>::value;

using utilities::metadata::names::lower_kebab_case_v;

template<typename ... Devices>
struct list
{
    static constexpr size_t N = list_output_length_v<Devices...>;
    static constexpr std::array<char, N> value = []()
    {
        std::array<char, N> ret{};
        size_t offset = 0;

        ret[N-1] = '\0';

        auto copy_one_name =  [&]<typename Device>(Device)
        {
            constexpr auto I = name_length<Device>();
            for (size_t i = 0; i < I; ++i)
            {
                ret[i + offset] = lower_kebab_case_v<Device>[i];
            }
            ret[I + offset] = '\n';
            offset += I+1;
        };

        (copy_one_name(Devices{}), ...); 
        return ret;
    }();
};

template<typename ... Devices>
constexpr const char * list_output = list<Devices...>::value.data();
}

template<typename ... Devices>
struct List
{
    static consteval auto name() { return "List"; }

    struct outputs_t
    {
        struct stdout_t
        {
            static consteval auto name() { return "standard text output"; }
            const char* value;
        } stdout;

        struct return_code_t
        {
            static consteval auto name() { return "return code"; }
            int value;
        } retcode;
    } outputs;

    template<typename... T>
    void main(int argc, char** argv, std::tuple<T...>&)
    {
        outputs.stdout.value = detail::list_output<Devices...>;
        outputs.retcode.value = 0;
    }

    void free() {}
};
}

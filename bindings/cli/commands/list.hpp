#pragma once

#include "utilities/metadata/names/names.hpp"

namespace bindings::cli::commands
{
template<typename Config, typename ... Devices>
struct List
{
    static consteval auto name() { return "list"; }
    static consteval auto usage() { return ""; }
    static consteval auto description() { return "List the devices available to interact with through the CLI"; }

    [[no_unique_address]] typename Config::basic_logger_type log;

    template<typename... T>
    int main(int argc, char** argv, std::tuple<T...>&)
    {
        using utilities::metadata::names::lower_kebab_case_v;
        ( log.println(lower_kebab_case_v<Devices>), ... );
        return 0;
    }
};
}

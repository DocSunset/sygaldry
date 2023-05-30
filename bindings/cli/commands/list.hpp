#pragma once

#include "utilities/metadata/names/names.hpp"

namespace sygaldry::bindings::cli::commands
{

template<typename Config>
struct List
{
    static consteval auto name() { return "list"; }
    static consteval auto usage() { return ""; }
    static consteval auto description() { return "List the devices available to interact with through the CLI"; }

    [[no_unique_address]] typename Config::basic_logger_type log;

    template<typename... Devices>
    int main(int argc, char** argv, std::tuple<Devices...>&)
    {
        using sygaldry::utilities::metadata::names::lower_kebab_case_v;
        ( log.println(lower_kebab_case_v<Devices>), ... );
        return 0;
    }
};

}

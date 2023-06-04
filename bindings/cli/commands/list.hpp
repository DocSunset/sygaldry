#pragma once

#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"

namespace sygaldry
{
namespace bindings::cli::commands
{

template<typename Config>
struct List
{
    static _consteval auto name() { return "list"; }
    static _consteval auto usage() { return ""; }
    static _consteval auto description() { return "List the components available to interact with through the CLI"; }

    [[no_unique_address]] typename Config::basic_logger_type log;

    template<typename... Components>
    int main(int argc, char** argv, std::tuple<Components...>&)
    {
        ( log.println(spelling::lower_kebab_case_v<Components>), ... );
        return 0;
    }
};

}
}

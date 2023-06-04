#pragma once

#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"
#include "components/endpoints/inspectors.hpp"

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

    template<typename Component>
    void list_component()
    {
        using sygaldry::utilities::spelling::lower_kebab_case;
        using sygaldry::endpoints::get_name;
        log.println(lower_kebab_case<get_name<Component>()>());
    }

    template<typename... Components>
    int main(int argc, char** argv, std::tuple<Components...>&)
    {
        ( list_component<Components>(), ... );
        return 0;
    }
};

}
}

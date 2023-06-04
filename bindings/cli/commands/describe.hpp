#pragma once

#include <boost/pfr.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"
#include "matcher.hpp"

namespace sygaldry
{
namespace bindings::cli::commands
{

template<typename Config>
struct Describe
{
    static _consteval auto name() { return "describe"; }
    static _consteval auto usage() { return "component-name [port-name]"; }
    static _consteval auto description() { return "Convey metadata about a component or its port. Pass * to describe all"; }

    [[no_unique_address]] typename Config::basic_logger_type log;

    template<typename... Components>
    int main(int argc, char** argv, std::tuple<Components...>&)
    {
        using utilities::metadata::names;
        if (argc == 2) return try_to_match_and_execute_with_wildcard(arv[1], components, 2, [](auto component)
        {
        });
        else if (argc > 2) return try_to_match_and_execute_with_wildcard(arv[1], components, 2, [](auto component)
        {
        });
    }
};

}
}

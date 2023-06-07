#pragma once

#include "concepts/endpoints.hpp"
#include "concepts/components.hpp"
#include "bindings/name_dispatch.hpp"
namespace sygaldry { namespace bindings { namespace cli { namespace commands {

template<typename Logger>
struct Trigger
{
    static _consteval auto name() { return "trigger"; }
    static _consteval auto usage() { return "component-name"; }
    static _consteval auto description() { return "Activate the given component's main subroutine"; }


    template<typename Components>
    int main(int argc, char** argv, Logger& log, Components& components)
    {
        if (argc < 2)
        {
            log.println("Usage: ", usage());
            return 2;
        }
        return dispatch<CommandMatcher>(argv[1], components, 2, [](auto& component)
        {
            activate(component);
            return 0;
        });
    }
};

} } } }

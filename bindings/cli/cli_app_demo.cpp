#include "utilities/consteval.hpp"
#include "components/sensors/button.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/basic_reader/cstdio_reader.hpp"
#include "bindings/basic_logger/cstdio_logger.hpp"
#include "bindings/cli/cli.hpp"

struct Component1 {
    static _consteval auto name() { return "Test Component 1"; }
    struct {} inputs;
    struct {} outputs;
};

struct Component2 {
    static _consteval auto name() { return "Test Component 2"; }
    struct {} inputs;
    struct {} outputs;
};

using namespace sygaldry::components;

struct AppComponents {
    Component1 cpt1;
    Component2 cpt2;
    TestComponent tc;
    ButtonGestureModel bgm;
};

int main()
{
    auto components = AppComponents{};
    auto cli = sygaldry::bindings::Cli
               < sygaldry::bindings::CstdioReader
               , sygaldry::bindings::CstdioLogger
               , decltype(components)
               >{};
    cli.init();
    for (;;)
    {
        cli(components);
        usleep(30*1000);
    }
}

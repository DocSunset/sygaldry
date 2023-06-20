#include "utilities/consteval.hpp"
#include "components/sensors/button.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/basic_reader/cstdio_reader.hpp"
#include "bindings/basic_logger/cstdio_logger.hpp"
#include "bindings/cli/cli.hpp"

using namespace sygaldry::components;

struct AppComponents {
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

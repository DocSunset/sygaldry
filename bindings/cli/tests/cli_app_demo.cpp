#include "utilities/consteval.hpp"
#include "concepts/runtime.hpp"
#include "helpers/metadata.hpp"
#include "components/sensors/button.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/basic_reader/cstdio_reader.hpp"
#include "bindings/basic_logger/cstdio_logger.hpp"
#include "bindings/cli/cli.hpp"

using namespace sygaldry;
using namespace sygaldry::components;

struct AppComponents {
    struct api_t
    {
        TestComponent tc;
        struct Button : name_<"Button">, ButtonGestureModel {} bgm;
    } api;

    sygaldry::bindings::Cli< sygaldry::bindings::CstdioReader
                           , sygaldry::bindings::CstdioLogger
                           , decltype(api)
                           > cli;
} constinit assemblage{};

constexpr auto runtime = Runtime{assemblage};

int main()
{
    runtime.init();
    for (;;)
    {
        runtime.main();
        usleep(30*1000);
    }
}

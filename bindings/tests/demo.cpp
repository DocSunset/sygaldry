#include "utilities/consteval.hpp"
#include "concepts/runtime.hpp"
#include "helpers/metadata.hpp"
#include "components/sensors/button.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/basic_reader/cstdio_reader.hpp"
#include "bindings/basic_logger/cstdio_logger.hpp"
#include "bindings/cli/cli.hpp"
#include "bindings/liblo.hpp"
#include "bindings/output_logger.hpp"

using namespace sygaldry;
using namespace sygaldry::components;
using namespace sygaldry::bindings;

struct AppComponents {
    struct api_t
    {
        TestComponent tc;
        struct Button : name_<"Button">, ButtonGestureModel {} bgm;
    } api;

    //Cli<CstdioReader, CstdioLogger, decltype(api)> cli;
    LibloOsc<decltype(api)> osc;
    OutputLogger<CstdioLogger, decltype(api)> out;
} constinit assemblage{};

constexpr auto runtime = Runtime{assemblage};

int main()
{
    runtime.init();
    for (;;)
    {
        runtime.tick();
        usleep(30*1000);
    }
}

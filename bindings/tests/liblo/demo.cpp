#include "concepts/runtime.hpp"
#include "bindings/liblo.hpp"
#include "components/tests/testcomponent.hpp"

using namespace sygaldry;
using namespace sygaldry::bindings;

struct Demo
{
    struct api_t
    {
        components::TestComponent tc;
    } api;
    LibloOsc<decltype(api)> lo;
} constinit demo{};

constexpr auto runtime = Runtime<Demo>{demo};

int main() { runtime.app_main(); return 0; }

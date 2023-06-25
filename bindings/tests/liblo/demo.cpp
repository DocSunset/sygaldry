#include "bindings/liblo.hpp"
#include "components/tests/testcomponent.hpp"

struct components_t
{
    struct api_t
    {
        TestComponent tc;
    } api;
    LibloOsc<decltype(api)> lo;
} components;

void main()
{
    init(components);

    for (;;)
    {
        activate(components);
    }
}

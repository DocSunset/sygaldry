#pragma once
#include "components/endpoints.hpp"

namespace sygaldry::components
{
using namespace sygaldry::endpoints;
struct TestComponent : endpoints::named<"Test Component 1">
{
    struct inputs_t {
        button<"button in"> bttn;
        toggle<"toggle in"> tggl;
        slider<"slider in"> sldr;
        bng<"bang in"> bang;
    } inputs;

    struct outputs_t {
        button<"button out"> bttn;
        toggle<"toggle out"> tggl;
        slider<"slider out"> sldr;
        bng<"bang out"> bang;
    } outputs;

    static constexpr void main(const inputs_t& in, outputs_t& out)
    {
        out.bttn = in.bttn;
        out.tggl = in.tggl;
        out.sldr = in.sldr;
        out.bang = in.bang;
    }
};
}

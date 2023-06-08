#pragma once
#include "components/metadata.hpp"
#include "components/endpoints.hpp"

namespace sygaldry { namespace components {

using namespace sygaldry::endpoints;
struct TestComponent : name_<"Test Component 1">
{
    struct inputs_t {
        button<"button in"> button_in;
        toggle<"toggle in"> toggle_in;
        slider<"slider in"> slider_in;
        bng<"bang in"> bang_in;
    } inputs;

    struct outputs_t {
        button<"button out"> button_out;
        toggle<"toggle out"> toggle_out;
        slider<"slider out"> slider_out;
        bng<"bang out"> bang_out;
    } outputs;

    static constexpr void main(const inputs_t& in, outputs_t& out)
    {
        out.button_out = in.button_in;
        out.toggle_out = in.toggle_in;
        out.slider_out = in.slider_in;
        out.bang_out = in.bang_in;
    }
};

} } // namespaces

#pragma once
#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"

namespace sygaldry { namespace components {

struct TestComponent : name_<"Test Component 1">
{
    struct inputs_t {
        button<"button in"> button_in;
        toggle<"toggle in"> toggle_in;
        slider<"slider in"> slider_in;
        bng<"bang in"> bang_in;
        text<"text in"> text_in;
        text_message<"text message in"> text_msg_in;
        vector<"vector in", 3> vector_in;
    } inputs;

    struct outputs_t {
        button<"button out"> button_out;
        toggle<"toggle out"> toggle_out;
        slider<"slider out"> slider_out;
        bng<"bang out"> bang_out;
        text<"text out"> text_out;
        text_message<"text message out"> text_msg_out;
        vector<"vector out", 3> vector_out;
    } outputs;

    constexpr void operator()()
    {
        outputs.button_out = inputs.button_in;
        outputs.toggle_out = inputs.toggle_in;
        outputs.slider_out = inputs.slider_in;
        outputs.bang_out = inputs.bang_in;
        outputs.text_out = inputs.text_in;
        outputs.text_msg_out = inputs.text_msg_in;
        outputs.vector_out = inputs.vector_in;
    }
};

} } // namespaces

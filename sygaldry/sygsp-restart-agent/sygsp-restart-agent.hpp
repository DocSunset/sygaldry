#pragma once
/*
Copyright 2023 Albert-Ngabo Niyonsenga

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"
#include "sygac-components.hpp"



namespace sygaldry { namespace sygsp {

/// \addtogroup sygsp
/// \{

/// \defgroup sygsp-restart-agent sygsp-restart-agent: Component Restart Agent
/// Literate source code: page-sygsp-restart-agent
/// \{

/*! \brief Component for handling the restart policies of other components
*/
struct RestartAgent
: name_<"Restart Agent">
, description_<"Component for handling the restart policies of other components">
, author_<"Albert-Ngabo Niyonsenga">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
        // Necessary Inputs
        slider_message<"restart policy","Set the restart policy for the component", int, 1, 4, 1, tag_session_data> restart_policy;
        slider_message<"restart attempts","Set the max amount of restart attempts", int,0, 10, 0, tag_session_data> max_attempts;
        toggle<"stop signal", "Indicate that the component should stop running", 0, tag_session_data> stop_signal;
        toggle<"attempt restart", "Indicates if the component attempts to restart when it failed."> attempt_restart;
        slider_message<"restart time","Set the time between restart attempts", int,  5000, 30000, 5000, tag_session_data> restart_time;
    } inputs;

    struct outputs_t {
        // Necessary Ouputs
        slider_message<"current attempt", "Current attempt for restarting component"> curr_attempt;
        toggle<"running", "Indicate if the component is running"> running;

        // TODO: use concepts to allow for more flexible spellings
    } outputs;

    void init();

    void restart();
    
    void configureAgent(const Component& component);

    void pollComponent(const Component& component);

    void restartComponent(const Component& component);
};

/// \}
/// \}

} }

/*
Copyright 2023 Albert-Ngabo Niyonsenga

SPDX-License-Identifier: MIT
*/
#include "sygsp-restart-agent.hpp"

namespace sygaldry { namespace sygsp {

void RestartAgent::init()
{
    if (sygaldry_component::inputs.restart_policy == 0) {
        // Set default values restart agent values for the component
        // Configure Restart Policy
        sygaldry_component::inputs.restart_policy.min = 1;
        sygaldry_component::inputs.restart_policy.max = 4;
        sygaldry_component::inputs.restart_policy.value = 2; 

        // Set max attempts and restart time
        sygaldry_component::inputs.max_attempts = 5;
        sygaldry_component::inputs.restart_time = 5000;
    }
    component_name_str += ": ";
    inputs.component_name = component_name_str;
    outputs.agent_running = true;
    return;
}

void RestartAgent::main()
{
    static auto prev = sygsp::micros();
    auto now = sygsp::micros();
    if (!outputs.agent_running) return;

    // Poll at component fixed interval
    if (now-prev > (sygaldry_component::inputs.restart_time*1e3)) {
        prev = now;
        // Check if component needs to be restarted
        restartComponent();
    }
}

void RestartAgent::restartComponent() {
    // Check validity of restart policy
    if ((sygaldry_component::inputs.restart_policy > sygaldry_component::inputs.restart_policy.max) || (sygaldry_component::inputs.restart_policy < sygaldry_component::inputs.restart_policy.min)) {
        sygaldry_component::outputs.error_message = inputs.component_name + ": Invalid restart policy set";
        return;
    }

    // Check if manual restart requested
    if (sygaldry_component::inputs.attempt_restart) {
        sygaldry_component::inputs.attempt_restart = false;
        sygaldry_component::restart();
        return;
    }

    // If the component is not running check restart policy
    if (!sygaldry_component::outputs.running) {
        if (sygaldry_component::inputs.restart_policy == 1) {    
            // No auto restart
            return;
        } else if ((sygaldry_component::inputs.restart_policy == 2) && (sygaldry_component::outputs.curr_attempt < sygaldry_component::inputs.max_attempts)) {
            // Restart for a max number of times
            sygaldry_component::outputs.curr_attempt++;
            sygaldry_component::restart();
            return;
        } else if (sygaldry_component::inputs.restart_policy == 3) {
            // Always restart
            sygaldry_component::restart();
            return;
        } else if ((sygaldry_component::inputs.restart_policy ==4) && (!sygaldry_component::inputs.stop_signal)) {
            // Restart unless stopped by user
            sygaldry_component::restart();
            return;
        } else if (sygaldry_component::inputs.attempt_restart) {
            // Restart if manual restart requested
            sygaldry_component::inputs.attempt_restart = false;
            sygaldry_component::restart();
            return;
        }
    }
}

} 
}

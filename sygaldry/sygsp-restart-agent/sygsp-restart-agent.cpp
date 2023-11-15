/*
Copyright 2023 Albert-Ngabo Niyonsenga

SPDX-License-Identifier: MIT
*/
#include "sygsp-restart-agent.hpp"
#include "sygsp-micros.hpp"
#include "sygac-components.hpp"

namespace sygaldry { namespace sygsp {

void RestartAgent::init() {
    return;
}

void RestartAgent::main() {
    return;
}

void RestartAgent::restart() {
    return;
}

void RestartAgent::configureAgent(auto component)
{
    if (component.inputs.restart_policy == 0) {
        // Set default values restart agent values for the component
        // Configure Restart Policy
        component.inputs.restart_policy.min = 1;
        component.inputs.restart_policy.max = 4;
        component.inputs.restart_policy.value = 2; 

        // Set max attempts and restart time
        component.inputs.max_attempts = 5;
        component.inputs.restart_time = 5000;
    }
    outputs.agent_running = true;
    return;
}


void RestartAgent::pollComponent(auto component)
{
    static auto prev = sygsp::micros();
    auto now = sygsp::micros();
    if (!outputs.agent_running) return;

    // Poll at component fixed interval
    if (now-prev > (component.inputs.restart_time*1e3)) {
        prev = now;
        // Check if component needs to be restarted
        restartComponent(auto component);
    }
}


void RestartAgent::restartComponent(auto component) {
    // Check validity of restart policy
    if ((component.inputs.restart_policy > component.inputs.restart_policy.max) || (component.inputs.restart_policy < component.inputs.restart_policy.min)) {
        component.outputs.error_message = component.name() + ": Invalid restart policy set";
        return;
    }

    // Check if manual restart requested
    if (component.inputs.attempt_restart) {
        component.inputs.attempt_restart = false;
        component.restart();
        return;
    }

    // If the component is not running check restart policy
    if (!component.outputs.running) {
        if (component.inputs.restart_policy == 1) {    
            // No auto restart
            return;
        } else if ((component.inputs.restart_policy == 2) && (component.outputs.curr_attempt < component.inputs.max_attempts)) {
            // Restart for a max number of times
            component.outputs.curr_attempt++;
            component.restart();
            return;
        } else if (component.inputs.restart_policy == 3) {
            // Always restart
            component.restart();
            return;
        } else if ((component.inputs.restart_policy ==4) && (!component.inputs.stop_signal)) {
            // Restart unless stopped by user
            component.restart();
            return;
        } else if (component.inputs.attempt_restart) {
            // Restart if manual restart requested
            component.inputs.attempt_restart = false;
            component.restart();
            return;
        }
    }
}

} 
}


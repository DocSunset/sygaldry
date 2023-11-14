\page page-sygsp-restart-agent sygsp-restart-agent: Component Restart Agent

Copyright 2023 Albert-Ngabo Niyonsenga

SPDX-License-Identifier: SPDX-License-Identifier: MIT

[TOC]

Component for handling the restart policies of other components. Originally this component was merely a part of the MAX17055 fuel gauge, but I realised that its functionality could be useful to other components. To not get ourselves confused for the rest of this guide I will refer to this component as the agent or restart agent, and the components it is interacting with as components.

This agent processes the logic for a couple of different restart policies. For this agent to work with another component the following assumptions are assumed.
1. The inputs described in the [Inputs and Outputs Section](#inputs-and-outputs) are present in the component you are using.
2. The component has a routine called restart. 
3. The restart routine sets the `outputs.running` endpoint to true if it succesfully completes.

```cpp
// @#'sygsp-restart-agent.hpp'
#pragma once
/*
Copyright 2023 Albert-Ngabo Niyonsenga

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace sygsp {

/// \addtogroup sygsp
/// \{

/// \defgroup sygsp-restart-agent sygsp-restart-agent: Component Restart Agent
/// Literate source code: page-sygsp-restart-agent
/// \{

/*! \brief Component for handling the restart policies of other components
*/
template<typename sygaldry_component, string_literal component_name_str>
struct RestartAgent
: name_<"Restart Agent">
, description_<"Component for handling the restart policies of other components">
, author_<"Albert-Ngabo Niyonsenga">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
        text_message<"component name", "Name of the attached component"> component_name
    } inputs;

    struct outputs_t {
        toggle<"running", "indicator for if the restart agent is running"> agent_running;
    } outputs;

    void init();

    void main();

    void restartComponent();
};

/// \}
/// \}

} }
// @/
```

# Inputs and Outputs
We want to be able to set a restart policy for the component so that the system can try to ping the component again to see if it responds. Having recently delved a bit into Docker and Docker containers my main inspiration comes from the restart policies you can assign to docker containers.

1. no: Don't restart the component after failure or stop.
2. on-failure[:max-retries]: Restart a component when it exits due to an error for a specified maximum number of retries.
3. always: Always restart a component.
4. unless-stopped: Always restart a component unless manually stopped.

Seperating between a component error and a manual stop is a bit of a pain, so I decided to go the simplest route, which is to add a new input for a stop signal alongside a couple more inputs. This is not as nice as being able to identify failure sources but it has the benefit of letting the user manually disable the component if they are trying to test another component.We set the restart policy as a number between 1-4, based on the corresponding restart policy. 

```cpp
// Necessary Inputs
slider_message<"restart policy","The restart policy for the component", 1, 4, 1, tag_session_data> restart_policy;
slider_message<"max attempts","The max amount of restart attempts", 0, 10, 0, tag_session_data> max_attempts;
toggle<"stop signal", "Indicate that the component should stop running", 0, tag_session_data> stop_signal;
toggle<"attempt restart", "Indicates if the component attempts to restart when it failed."> attempt_restart;
slider_message<"restart time","Set the time between restart attempts", 5000, 30000, 5000, tag_session_data> restart_time;
```
Given we have a stop signal it makes sense to also have a restart signal. `attempt_restart` is similar to `stop_signal` but works in reverse. When setting the `attempt_restart` to `1` the component will attempt a manual restart, regardless of the restart policy and then set `attempt_restart` back to `0`.

```cpp
// Necessary Ouputs
slider_message<"current attempt", "Current attempt for restarting component"> curr_attempt;

// TODO: this agent would probably benefit from it's own custom endpoints, that MIGHT (doing a lot of heavy lifting here) make it would be possible then to just check for the enpoint in the component that is the type I'm expecting rather than have the name have to be identical
```

For simplicity these endpoints should be defined by the component that is using the Restart Agent. You can check the MAX17055 component for an example. Except for the `restart_policy` endpoint the maximum and minimums of the slider endpoints can be whatever the user desires.

# Main Subroutine

We have a very simple subroutines. Initialise by setting the component name input and restart rate and check if the component needs to be restarted.

```cpp
// @#'sygsp-restart-agent.cpp'
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

// @/
```

We create `restartComponent()` In order to handle the logic for the restarting components. It uses the inputs of the component

```cpp
//@+'sygsp-restart-agent.cpp'
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
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsp-restart-agent)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
#target_link_libraries(${lib}-test PRIVATE OTHERREQUIREDPACKAGESANDCOMPONENTSHERE)
catch_discover_tests(${lib}-test)
endif()
# @/
```

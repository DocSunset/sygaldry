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

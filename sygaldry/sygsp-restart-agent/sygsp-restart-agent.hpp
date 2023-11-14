#pragma once
/*
Copyright 2023 Albert-Ngabo Niyonsenga

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

template<typename sygaldry_component>
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
    void configureAgent(const sygaldry_component& component);

    void pollComponent(const sygaldry_component& component);

    void restartComponent(const sygaldry_component& component);
};

/// \}
/// \}

} }

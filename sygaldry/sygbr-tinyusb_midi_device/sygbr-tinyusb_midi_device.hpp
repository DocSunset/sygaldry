#pragma once
/*
Copyright 2024 Travis J. West

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace sygbr {

/// \addtogroup sygbr
/// \{

/// \defgroup sygbr-tinyusb_midi_device sygbr-tinyusb_midi_device: TinyUSB MIDI Device Driver
/// Literate source code: page-sygbr-tinyusb_midi_device
/// \{

/*! \brief MIDI device driver based on TinyUSB, making TinyUSB MIDI API available to other components
*/
struct MidiDeviceDriver
: name_<"MIDI Device Driver">
, description_<"MIDI device driver based on TinyUSB, making TinyUSB MIDI API available to other components">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
    } inputs;

    struct outputs_t {
    } outputs;

    void init();

    void main();
};

/// \}
/// \}

} }

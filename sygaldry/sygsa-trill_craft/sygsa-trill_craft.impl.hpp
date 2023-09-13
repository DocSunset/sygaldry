/*
Copyright 2021-2023 Edu Meneses https://www.edumeneses.com, Metalab - Société
des Arts Technologiques (SAT), Input Devices and Music Interaction Laboratory
(IDMIL), McGill University

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygsa-trill_craft.hpp"
#include <algorithm>
#include <Trill.h>

namespace sygaldry { namespace components { namespace arduino {

void TrillCraft::init()
{
    auto trill = new Trill();
    pimpl = static_cast<void*>(trill);

    // TODO: initialize *all* input parameters in case there is no session data
    bool initialize_map = false;
    std::array<bool, channels> channel_indexed{0};
    for (std::size_t i = 0; i < channels && !initialize_map; ++i)
    {
        if (inputs.map[i] < 30)
        {
            if (channel_indexed[inputs.map[i]]) initialize_map = true; // channel already indexed
            else channel_indexed[inputs.map[i]] = true;
        }
        else
        {
            initialize_map = true;
        }
    }
    if (initialize_map) for (std::size_t i = 0; i < channels; ++i) inputs.map[i] = i;

    int setup_return_code = trill->setup(Trill::TRILL_CRAFT);
    if (0 != setup_return_code)
    {
        outputs.running = false;

        // See Trill.cpp for an authoritative source of information
        // on the return codes, which are not otherwise documented
        switch (setup_return_code)
        {
        case -2:
            outputs.error_status = "unknown default address";
            break;
        case 2:
            outputs.error_status = "unable to identify device";
            break;
        case -3:
            outputs.error_status = "found unexpected device type";
            break;
        case -1:
            outputs.error_status = "invalid device mode";
            break;
        default:
            outputs.error_status = "unknown error";
        }
        return;
    }
    else outputs.running = true;
}

void TrillCraft::main()
{
    if (not outputs.running) return; // TODO: try to reconnect every so often

    auto trill = static_cast<Trill*>(pimpl);

    // TODO: we should check and constrain boundary conditions
    if (inputs.speed.updated || inputs.resolution.updated) trill->setScanSettings(inputs.speed, inputs.resolution);
    if (inputs.noise_threshold.updated)                    trill->setNoiseThreshold(inputs.noise_threshold);
    //if (inputs.autoscan_interval.updated)                trill->setAutoScanInterval(inputs.autoscan_interval);
    if (inputs.prescaler.updated)                          trill->setPrescaler(inputs.prescaler);
    if (inputs.resolution.updated || inputs.prescaler.updated || inputs.update_baseline)
    {
        for (auto& max : outputs.max_seen.value) max = 0;
        trill->updateBaseline();
    }

    trill->requestRawData();
    for (int i=0; i<30; i++) {
        if (trill->rawDataAvailable() > 0) {
            outputs.raw[i] = trill->rawDataRead();
        }
    }

    for (int i = 0; i < channels; i++) {
        if (outputs.raw[i] != 0) {
            outputs.max_seen[i] = std::max(outputs.max_seen[i], outputs.raw[i]);
            outputs.normalized[inputs.map[i]] = static_cast<float>(outputs.raw[i])
                                              / static_cast<float>(outputs.max_seen[i]);
            outputs.mask[inputs.map[i]] = true;
        } else {
            outputs.normalized[inputs.map[i]] = 0.0f;
            outputs.mask[inputs.map[i]] = false;
        }
    }

    outputs.instant_max = *std::max_element( outputs.normalized.value.data()
                                           , outputs.normalized.value.data()+channels
                                           );

    if (outputs.instant_max == 0.0f) {
        outputs.any = 0;
    } else {
        outputs.any = 1;
    }
}

} } }

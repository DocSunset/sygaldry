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

    if (inputs.speed || inputs.resolution) trill->setScanSettings(inputs.speed, inputs.resolution);
    if (inputs.noise_threshold)            trill->setNoiseThreshold(inputs.noise_threshold);
    //if (inputs.idac_value)                 trill->setIDACValue(inputs.idac_value);
    //if (inputs.autoscan_interval)          trill->setAutoScanInterval(inputs.autoscan_interval);
    if (inputs.prescaler)                  trill->setPrescaler(inputs.prescaler);
    if (inputs.resolution || inputs.prescaler || inputs.update_baseline)
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
            outputs.mask[i] = true;
            outputs.max_seen[i] = std::max(outputs.max_seen[i], outputs.raw[i]);
            outputs.normalized[i] = static_cast<float>(outputs.raw[i])
                                  / static_cast<float>(outputs.max_seen[i]);
        } else {
            outputs.normalized[i] = 0.0f;
            outputs.mask[i] = false;
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

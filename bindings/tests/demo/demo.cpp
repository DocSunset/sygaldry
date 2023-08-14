/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygaldry-utilities-consteval.hpp"
#include "sygaldry-concepts-components.hpp"
#include "sygaldry-concepts-runtime.hpp"
#include "helpers/metadata.hpp"
#include "bindings/testcomponent.hpp"
#include "bindings/cstdio_reader.hpp"
#include "bindings/cstdio_logger.hpp"
#include "bindings/cli.hpp"
#include "bindings/liblo.hpp"
#include "bindings/output_logger.hpp"

using namespace sygaldry;
using namespace sygaldry::components;
using namespace sygaldry::bindings;


struct AppComponents {
    struct api_t
    {
        TestComponent tc;
        //struct Button : name_<"Button">, ButtonGestureModel {} bgm;
    } api;

    //Cli<CstdioReader, CstdioLogger, decltype(api)> cli;
    LibloOsc<decltype(api)> osc;
    OutputLogger<CstdioLogger, decltype(api)> out;
} constinit assemblage{};

static_assert(Component<decltype(assemblage.osc)>);

constexpr auto runtime = Runtime{assemblage};

int main()
{
    runtime.init();
    for (;;)
    {
        runtime.tick();
        usleep(30*1000);
    }
}

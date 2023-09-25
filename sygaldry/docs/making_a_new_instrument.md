\page page-sygin-making_a_new_instrument Making a New Instrument

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Sygaldry currently only facilitates firmware development, so you'll have to
work out the electronic and physical design of your instrument using existing
methods. However, using existing software components in the library, Sygaldry
can greatly simplify development of the firmware for many instrument designs.

# The Runtime

The foundation of a Sygaldry instrument is the runtime, described in detail in
\ref page-sygac-runtime (literate source), with reference documentation
available [here](\ref sygaldry::Runtime).

The runtime is implemented by a C++ class template. The template argument
should be a simple aggregate structure (termed an assembly) that contains
Sygaldry components or nested aggregate structures of components
(subassemblies). The runtime provides a method `init()` that will initialize
all of the components in the order they are instantiated (which may be
different than the order they are declared) in the assembly, and a method
`tick()` that will run their main subroutines in the same order. All you have
to do is call `init()` once, then run `tick()` in a loop forever.

Using the runtime, implementing the firmware for an instrument is as simple as
listing its components, `constexpr` instantiating the runtime, and then calling
init and tick:

```cpp
#include "sygac-runtime.hpp"
// #include various component headers here

struct MyMinimumInstrument
{
    // ... components here
} my_minimum_instrument;

constexpr auto runtime = Runtime{my_minimum_instrument};

int main()
{
    runtime.init();
    while (true) runtime.tick();
}
```

# Bindings

Many components require information about other components in order to realize
their functionality. We implement this by passing the required components as
a template argument to the dependent components. The required components are
termed plugins.

The most common example is binding components. These components automatically
reflect over the endpoints of their plugins and expose those endpoints in some
fashion.

For example, here is the implementation of a very simple instrument, using an
ESP32, that consists of a single button exposed over the network using Open
Sound Control:

```cpp
#include "sygse-button.hpp"
#include "sygbe-wifi.hpp"
#include "sygbp-liblo.hpp"

using namespace sygaldry;

struct OneBitBongo
{
    components::esp32::Button<GPIO_NUM_15> button;
    bindings::esp32::WiFi wifi;
    bindings::LibloOsc<decltype(button)> osc;
} bongo{};

constexpr auto runtime = Runtime{bongo};

extern "C" void app_main(void)
{
    runtime.init();
    vTaskDelay(pdMS_TO_TICKS(100));
    while (true)
    {
        runtime.tick();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

Notice how the button component's type is passed to `LibloOSC` as a template
parameter. This enables the binding to generate appropriate code to expose the
button's endpoints over the network *at compile time*, giving runtime
performance similar to if the binding had been written by hand.

# Subassemblies

In the `OneBitBongo` example, there is only one component worth binding. In
more typical instrument designs, there are generally more sensors than one
button. Rather than listing all of these components explicitly for every
binding, we instead wrap all the components that need to be bound by a given
binding in a simple aggregate struct that we term as a subassembly. We can then
pass the subassembly to bindings, which will expose all of the components
contained in it. For example, supposing we wanted to have two buttons, we could
write the following:

```cpp
struct TwoBitBongo
{
    struct Buttons {
        components::esp32::Button<GPIO_NUM_15> button1;
        components::esp32::Button<GPIO_NUM_16> button2;
    } buttons;
    bindings::esp32::WiFi wifi;
    bindings::LibloOsc<Buttons> osc;
} bongo{};
```

# Important Bindings

There are a few bindings that almost all instruments will want to use.

## Session Storage

Many components need to save information persistently across power cycles.
Session storage binding components serve this requirement by saving the values
of endpoints marked with the session data tag in persistent storage such as
flash memory or EEPROM.

It's important that the session storage component is the first component
instantiated in assembly. Since it also generally needs to bind over numerous
other components, this usually requires a subassembly to be defined before the
session storage, and then instantiated after the session storage, as in the
example below.

## CLI

To facilitate initial set up of parameters like network credentials and OSC
ports, a serial command line interface is provided by a CLI component. This
allows e.g. the network to be initially set up after the firmware is first
flashed, as well as allowing sensitive parameters that should not be globally
exposed over the network to be configured only by those with physical access to
the instrument. The CLI can also be very useful when debugging.

## Example with Nested Subassemblies

On ESP32 for example, most instruments will have a structure similar to the
following:

```cpp
struct MyTypicalInstrument
{
    struct Instrument {
        struct Sensors {
            components::esp32::Button<GPIO_NUM_15> button;
            // ... etc.
        } sensors;
        bindings::esp32::WiFi wifi;
        bindings::LibloOsc<Sensors> osc;
        // ... other bindings
    };
    bindings::esp32::SpiffsSessionStorage<Instrument> session_storage;
    Instrument instrument;
    bindings::CstdioCli<Instrument> cli;
} my_typical_instrument{};
```

Notice how the `session_storage` component is instantiated before the
`instrument`, but after the definition of `struct Instrument`. This allows
`Instrument` to be passed as a template type parameter to the session storage
component (which requires the definition of `Instrument` to be available
already), while still allowing the session storage component to be run by the
runtime before the other components, so that other components will be
initialized after their session data has been restored.

# Platform Specific Extras

Many platforms will require addition information beyond the instrument definition,
such as for setting up the build system or configuring the environment.

## ESP32

See [the implementation of the T-Stick](\ref page-sygin-t_stick)

# Documentation

Since instrument implementations are naturally quite declarative and readable,
we place less emphasis on their literate prose documentation and reference
documentation. However, remember to still include a copyright statement
and license identifier in all documents in the repository.

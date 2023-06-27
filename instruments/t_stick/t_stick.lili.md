# T-Stick

```cpp
//@#'t_stick.cpp'
#include "utilities/consteval.hpp"
#include "concepts/components.hpp"
#include "components/metadata.hpp"
#include "components/esp32/all.hpp"
#include "bindings/esp32/all.hpp"

@{idf preprocessor configuration}

using namespace sygaldry;
using namespace sygaldry::components;
using namespace sygaldry::bindings;

struct TStick : _name("T-Stick")
              , _designer<"Joseph Malloch">
              , _author<"Travis J. West">
              , _copyright<"Travis J. West (C) 2023">
              , _license<"SPDX-License-Identifier: GPL-3.0-or-later">
              , _version<"0.0.0">
{
    using I2C = esp32::I2cDriver<idf_config::i2c_sda, idf_config::i2c_scl>;
    using Time = esp32::TimeProvider;
    using Log = CstdioLogger;

    struct Sensors {
        esp32::GpioButton<Time, idf_config::button_pin> button;
        esp32::AdcSensor<I2C, idf_config::fsr_pin> fsr;
        TStickTouch<I2C, Time, Trill> touch;
        Mimu<I2C, Time, ICM20948> mimu;
    };

    struct Synth {
        /* ... */
    };

    struct parts_t {
        struct instrument_t {
            Sensors sensors;
            Synth synth;
            TStickCrossModalMapping<get_outputs_t<decltype(sensors)>, get_inputs_t<decltype(synth)>> mapping;
        } instrument;

        struct bindings {
            SessionManager<Eeprom<Log, JsonSerializer>, decltype(instrument)>  eeprom;
            esp32::WifiDriver wifi;
            LibloOsc<decltype(instrument), idf_config::default_name> liblo;
            LibmapperDevice<decltype(instrument), idf_config::default_name> libmapper;
        } bindings;

        HttpServer< ConfigWebpage<decltype(bindings), idf_config::config_page_url>
                  , ViewerWebpage<decltype(instrument), idf_config::viewer_page_url>
                  > http_server;
        Cli<Log, decltype(bindings)> cli;
    } parts;
} constinit tstick{};

constexpr auto runtime = Runtime{tstick};

extern "C" int app_main() { runtime.app_main(); return 0; }
//@/
```

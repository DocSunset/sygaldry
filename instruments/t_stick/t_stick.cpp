#include "utilities/consteval.hpp"
#include "concepts/components.hpp"
#include "components/metadata.hpp"
#include "components/esp32/all.hpp"
#include "bindings/esp32/all.hpp"


using namespace sygaldry;
using namespace sygaldry::components;
using namespace sygaldry::bindings;

struct TStick { struct parts_t {
    StdioLogger log; // a part
    // Eeprom<decltype(log), decltype(api), JsonSerializer> eeprom; // a part with two external plugins, one internal plugin
    esp32::I2cDriver<idf_config::i2c_sda, idf_config::i2c_scl> i2c; // a part with two configuration parameters
    struct api_t : _name("T-Stick")
                 , _designer<"Joseph Malloch">
                 , _author<"Travis J. West">
                 , _copyright<"Travis J. West (C) 2023">
                 , _license<"SPDX-License-Identifier: GPL-3.0-or-later">
                 , _version<"0.0.0">
    { struct parts_t {
        esp32::WifiDriver wifi; // a part
        esp32::TimeProvider time; // a part
        struct sensors_t
        {
            esp32::GpioButton<decltype(time), idf_config::button_pin> button; // a part with one external plugin and one configuration parameter
            esp32::AdcSensor<decltype(time), idf_config::fsr_pin> fsr; // a part with one external plugin and one configuration parameter
            TStickTouch<decltype(i2c), decltype(time), Trill> touch; // a part with two external plugins and one internal plugin
            Mimu<decltype(i2c), decltype(time), ICM20948> mimu; // a part with two external plugins and one internal plugin
        } sensors; // a subassembly assemblage
        // TStickSynth<esp32::DacDriver<idf_config::blah blah blah>> synth; // a part with one internal plugin with configuration parameters
        // TStickCrossModalMapping<get_outputs_t<decltype(sensors)>, get_inputs_t<decltype(synth)>> mapping; // a part with two throughpoints
    } parts; } api; // a pure subassembly
    // esp32::GpioInterruptDriver< decltype(time)
    //                           , idf_config::button_pin
    //                           , idf_config::trill_interrupt_pin
    //                           , idf_config::icm20948_interrupt_pin
    //                           > interrupts; // a part with one external plugin and three configuration parameters
    // HttpServer< ConfigWebpage<decltype(api), idf_config::config_page_url>
    //           , ViewerWebpage<decltype(api), idf_config::viewer_page_url>
    //           > http_server; // a part with two internal plugins, each with one external plugin and one configuration parameter
    LibloServer<decltype(api), idf_config::default_name> liblo_server; // a part with one external plugin
    // LibmapperDevice<decltype(api)> libmapper_device; // one external plugin
    Cli<decltype(log), decltype(api)> cli; // a part with two external plugins
} parts; } tstick; // an assemblage

int main()
{
    init(tstick, tstick, tstick.api);
    for (;;)
        activate(tstick, tstick, tstick.api);
}

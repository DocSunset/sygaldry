\page page-sygbe-wifi sygbe-wifi: ESP32 WiFi

Originally adapted from Puara Module Manager, itself adapted from the esp-idf
wifi station and access point examples

Copyright 2022 Edu Meneses https://www.edumeneses.com, Metalab - Société des
Arts Technologiques (SAT), Input Devices and Music Interaction Laboratory
(IDMIL), McGill University

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This document describes the implementation of the WiFi management component on
the ESP32 platform. The WiFi component allows the ESP32 to be connected to
WiFi, and to act as an access point for its own network, facilitating the
configuration of WiFi and other parameters.

[TOC]

# Design Rationale

The WiFi component exposes inputs used to configure the networking setup,
especially the SSID and password of the network that the device should connect
to. Enabling configuration of these parameters is left to other binding
components, as is the persistent storage of these parameters across reboots.
These inputs are tagged as session data so that a session management component
should facilitate this persistent storage, allowing the device to seamlessly
reconnect to the last used WiFi network. In the future, it would be ideal if
the device would remember an arbitrary number of WiFi network credentials. In
principle, this information should also be protected to avoid inadvertantly
compromising the security of the network. Bindings that expose the password
endpoints should respect the `write_only` tag, and not show the passwords to
users. A more robust security policy is left as future work.

The main goal of the WiFi component is to easily enable wireless networking.
Once the network is established, it doesn't really have any role. For this
reason, the component's main subroutine is a no-op. The main drawback of this
approach is that the component will not recognize when its settings have
changed, and will thus not switch to a different network if requested. Instead,
the user must reboot the device, triggering this component's initialization
phase, to implement changes to the WiFi parameters. It remains as future work
to enable a callback when this component's inputs are modified.

As well as connecting the ESP32 to a WiFi network as a station, the WiFi
component also enables the ESP32 to act as an access point, hosting its own
network. This is especially useful for administering the device e.g. when
switching to a location with a different network configuration, as it should
allow the user to modify the network configuration and trigger a reboot
through the planned webpage binding.

Like the `basic_logger` components, this component doesn't actually bind to
anything. It is placed in the bindings library because it is of most direct use
to bindings.

Only a single WiFi component is allowed to exist in an ESP32 app. It must be
initialized before any networking functionality can be used, such as that
required by the planned liblo and webpage bindings.

# Initializing Wifi

As several other bindings depend on WiFi, it is one of the first components
initialized at boot up.

Eventually, the SSID and password for the access point and WiFi networks should
be restored from flash memory, and these inputs should be set. Until then, and
when a freshly built or factory reset device is initializing, these input texts
will be empty and need to be set to reasonable defaults. They are also reset in
case they are somehow set to values with invalid length, e.g. password too
short or SSID too long.

```cpp
// @+'inputs'
text< "hostname"
    , "Name of this device on the network."
      "Must be less than 31 bytes long."
      "Requires reboot to take effect."
    , tag_session_data
    > hostname;
text< "access point SSID"
    , "Name of the device-hosted network."
      "Must be less than 31 bytes long."
      "Requires reboot to take effect."
    , tag_session_data
    > ap_ssid;
text< "access point password"
    , "Password of the device-hosted network."
      "Must be greater than 8 and less than 63 bytes long."
      "Requires reboot to take effect."
    , tag_session_data, tag_write_only
    > ap_password;
text< "WiFi SSID"
    , "Name of the WiFi network to connect to."
      "Must be less than 31 bytes long."
      "Requires reboot to take effect."
    , tag_session_data
    > wifi_ssid;
text< "WiFi password"
    , "Password of the WiFi network to connect to."
      "Must be greater than 8 and less than 63 bytes long."
      "Requires reboot to take effect."
    , tag_session_data, tag_write_only
    > wifi_password;
// @/

// @+'initialize wifi'
if (inputs.hostname.value.empty() || inputs.hostname.value.length() > 31)
{
   inputs.hostname = "sygaldry_instrument";
   log.println("wifi warning: initialized hostname................ '", inputs.hostname.value, "'");
}

if (inputs.ap_ssid.value.empty() || inputs.ap_ssid.value.length() > 31)
{
   inputs.ap_ssid = "sygaldry_admin";
   log.println("wifi warning: initialized access point SSID....... '", inputs.ap_ssid.value, "'");
}

if ( inputs.ap_password.value.empty() || inputs.ap_password.value.length() < 8 || inputs.ap_password.value.length() > 63)
{
    inputs.ap_password = "sygaldry_admin";
   log.println("wifi warning: initialized access point password... '", inputs.ap_password.value, "'");
}

// TODO: just don't bother trying to connect to wifi in this case
if ( inputs.wifi_ssid.value.empty() || inputs.wifi_ssid.value.length() > 31)
{
    inputs.wifi_ssid = "sygaldry_wifi";
   log.println("wifi warning: initialized WiFi SSID............... '", inputs.wifi_ssid.value, "'");
}

if ( inputs.wifi_password.value.empty() || inputs.wifi_password.value.length() < 8 || inputs.wifi_password.value.length() > 63)
{
    inputs.wifi_password = "sygaldry_admin";
   log.println("wifi warning: initialized WiFi password........... '", inputs.wifi_password.value, "'");
}
// @/
```

Once the input data has been validated, and set to valid defaults as necessary,
initialization proceeds by initializing the network interfaces and event loop,
and setting the configuration of the WiFi API. This is all standard boiler
plate adapted from the examples provided with the ESP-IDF.

It also appears necessary to initialize non-volatile storage
(`nvs_flash_init`); this is shown in the ESP-IDF examples for setting up an
access point, but an explanation is not provided for why this is necessary.

```cpp
// @+'initialize wifi'
esp_err_t ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
  ESP_ERROR_CHECK(nvs_flash_erase());
  ret = nvs_flash_init();
}
ESP_ERROR_CHECK(ret);
log.println("wifi: Initialized NVS");

ESP_ERROR_CHECK(esp_netif_init());
log.println("wifi: Initialized network interface");

ESP_ERROR_CHECK(esp_event_loop_create_default());
log.println("wifi: Created default event loop");

// TODO: do we need to store these handles?
esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
esp_netif_t *ap_netif  = esp_netif_create_default_wifi_ap();
ESP_ERROR_CHECK(esp_netif_set_hostname( sta_netif
                                      , inputs.hostname.value.c_str()
                                      )
               );
ESP_ERROR_CHECK(esp_netif_set_hostname( ap_netif
                                      , inputs.hostname.value.c_str()
                                      )
               );
log.println("wifi: Set hostnames");

wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
ESP_ERROR_CHECK(esp_wifi_init(&cfg));
log.println("wifi: Initialized WiFi with default configuration");
// @/
```

Depending on whether the user has enabled the access point, we then
set the configuration for the WiFi interfaces. We always try to
enable the WiFi station mode at first.

```cpp
// @+'inputs'
toggle< "enable access point"
      , "Indicate whether to persistently enable the device-hosted network."
        "When this toggle is disabled, the access point is only enabled if"
        "the device fails to connect to WiFi in station mode."
      , 0
      , tag_session_data
      > enable_ap;
// @/

// @+'initialize wifi'
if (inputs.enable_ap) set_wifi_mode(WIFI_MODE_APSTA);
else set_wifi_mode(WIFI_MODE_STA);
// @/

// @='set_wifi_mode'
void set_wifi_mode(wifi_mode_t mode)
{
    log.print("wifi: setting WiFi mode to ");
    switch(mode)
    {
    case WIFI_MODE_STA: log.println("station"); break;
    case WIFI_MODE_AP: log.println("access point"); break;
    case WIFI_MODE_APSTA: log.println("access point / station"); break;
    default: log.println("unsupported mode..?"); break;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(mode));

    if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
    {
        wifi_config_t sta_config{};
        std::memcpy(sta_config.sta.ssid, inputs.wifi_ssid.value.c_str(), inputs.wifi_ssid.value.length()+1);
        std::memcpy(sta_config.sta.password, inputs.wifi_password.value.c_str(), inputs.wifi_password.value.length()+1);
        log.println("wifi: Enabling station");
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    }

    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
    {
        wifi_config_t ap_config{};
        std::memcpy(ap_config.ap.ssid, inputs.ap_ssid.value.c_str(), inputs.ap_ssid.value.length()+1);
        std::memcpy(ap_config.ap.password, inputs.ap_password.value.c_str(), inputs.ap_password.value.length()+1);
        ap_config.ap.ssid_len = inputs.ap_ssid.value.length();
        ap_config.ap.channel = 5;
        ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        ap_config.ap.ssid_hidden = 0;
        ap_config.ap.max_connection = 5;
        // TODO: add channel, authmode, hidden, and max connections as inputs
        log.println("wifi: Enabling access point");
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    }
}
// @/
```

ESP-IDF requires us to use an event loop to detect when WiFi connects or fails
to connect. When the station is started, we attempt to connect to the network
SSID and password given by the user. If this fails, we retry a few times before
giving up. If it succeeds, we recognize this by the event generated when our
device is assigned an IP address. In either case, we signal the success or
failure to connect using a FreeRTOS event group.

To keep the event handler tidily local to the place of its use, we use a
somewhat unsual construction `+[](/* args... */) { /* body... */ };` where a
lambda with no capture is converted to a raw function pointer using the `+`
operator.

```cpp
// @='handler_state_t'
struct handler_state_t {
    EventGroupHandle_t event_group;
    char connection_attempts;
    sygup::CstdioLogger* log;
    static constexpr int connected_bit = BIT0;
    static constexpr int fail_bit = BIT1;
    static constexpr int maximum_connection_attempts = 2;
};
// @/

// @+'initialize wifi'
auto sta_event_handler = +[](void * arg, esp_event_base_t event_base, long int event_id, void * event_data)
{
    handler_state_t& handler_state = *(handler_state_t*)arg;
    auto& log = *handler_state.log;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        log.println("wifi: WiFi station started. Connecting to network...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && 
               event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        log.print("wifi: Station disconnected. ");
        if (handler_state.connection_attempts < handler_state.maximum_connection_attempts)
        {
            log.println("Attempting to reconnect...");
            esp_wifi_connect();
            handler_state.connection_attempts++; 
        } else {
            log.println("Connection failed.");
            xEventGroupSetBits(handler_state.event_group, handler_state.fail_bit);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        log.println("Connection succeeded.");
        handler_state.connection_attempts = 0;
        xEventGroupSetBits(handler_state.event_group, handler_state.connected_bit);
    }
};
// @/
```

We register the event handler before configuring the WiFi interfaces. Then,
after starting WiFi, we wait on the event group until any bit is set.

```cpp
// @+'initialize wifi'
handler_state_t handler_state{};
handler_state.event_group = xEventGroupCreate();
handler_state.connection_attempts = 0;
handler_state.log = &log;

esp_event_handler_instance_t instance_any_id;
esp_event_handler_instance_t instance_got_ip;
ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    sta_event_handler,
                                                    (void*)&handler_state,
                                                    &instance_any_id)
               );
ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                    IP_EVENT_STA_GOT_IP,
                                                    sta_event_handler,
                                                    (void*)&handler_state,
                                                    &instance_got_ip)
               );
log.println("wifi: Registered WiFi station event handler");

log.println("wifi: Starting WiFi...");
ESP_ERROR_CHECK(esp_wifi_start());

EventBits_t bits = xEventGroupWaitBits( handler_state.event_group
                                      , handler_state.connected_bit | handler_state.fail_bit
                                      , pdFALSE, pdFALSE, portMAX_DELAY
                                      );
log.println("wifi: Finished waiting...");
// @/
```

If the flags returned by the event group indicate that connection was
successful, then we set the output toggle indicating a successful wifi
connection. Otherwise, we enable the access point in case it is not already
running, and disable the WiFi station. The access point is assumed to start
without issue whenever requested, and we set the corresponding output toggle
accordingly.

```cpp
// @+'outputs'
toggle<"WiFi connected", "Indicates when WiFi is successfully connected"> wifi_connected;
toggle<"AP running", "Indicates when the device-hosted network is running"> ap_running;
// @/

// @+'initialize wifi'
if (bits & handler_state.connected_bit)
{
    outputs.wifi_connected = 1;
    outputs.ap_running = inputs.enable_ap;
}
else // fail_bit; TODO: shouldn't we only enable AP if it isn't already?
{
    set_wifi_mode(WIFI_MODE_AP);
    outputs.ap_running = 1;
    outputs.wifi_connected = 0;
}
// @/
```

Finally, we clean up the event handler and its event group, and output useful
information acquired during set up. Note that, in case the station was unable
to connect, the IP address output here will reflect whatever IP address happens
to be stored by the ESP-IDF network interface API, e.g. probably the last IP
address used by the device.

```cpp
// @+'initialize wifi'
//ESP_ERROR_CHECK(esp_event_handler_instance_unregister( IP_EVENT
//                                                     , IP_EVENT_STA_GOT_IP
//                                                     , instance_got_ip
//                                                     )
//               );
//ESP_ERROR_CHECK(esp_event_handler_instance_unregister( WIFI_EVENT
//                                                     , ESP_EVENT_ANY_ID
//                                                     , instance_any_id
//                                                     )
//               );
//vEventGroupDelete(handler_state.event_group);
//log.println("Cleaned up event handler.");
// @/

// @+'outputs'
text<"WiFi MAC", "MAC address of the device as a WiFi station on the main network"> wifi_mac;
text<"AP MAC", "MAC address of the device as a WiFi access point on the device-hosted network"> ap_mac;
text<"IP address", "IP address of the device as a WiFi station on the main network."
                   "Use this address to send network messages to the device when WiFi is connected."> ip_address;
// @/

// @+'initialize wifi'
// TODO: clean up or hide this ugly mess that converts mac and ip addresses to nicely formatted strings
char mac_string[18] = {0};
unsigned char mac[6] = {0};
char * ptr;
auto mac_to_string = [&]()
{
    ptr = mac_string;
    for (int i = 0; i < 6; ++i)
    {
        auto [one_past_written, ec] = std::to_chars(ptr, ptr+2, mac[i], 16);
        if (one_past_written != ptr+2) // if to_chars only wrote one character
        {
            ptr[1] = ptr[0]; // copy written character to second position
            ptr[0] = '0'; // pad first position with 0
        }
        ptr = ptr+2;
        *ptr++ = ':';
    }
    *--ptr = 0;
};

esp_wifi_get_mac(WIFI_IF_STA, mac);
mac_to_string();
outputs.wifi_mac.value = mac_string;

esp_wifi_get_mac(WIFI_IF_AP, mac);
mac_to_string();
outputs.ap_mac.value = mac_string;

char ip_string[16];
esp_netif_ip_info_t ip_info;
esp_netif_get_ip_info(sta_netif, &ip_info);
ptr = ip_string;
auto [a,b] = std::to_chars(ptr, ptr+3, esp_ip4_addr1_16(&ip_info.ip), 10);
ptr = a;
*ptr++ = '.';
auto [c,d] = std::to_chars(ptr, ptr+3, esp_ip4_addr2_16(&ip_info.ip), 10);
ptr = c;
*ptr++ = '.';
auto [e,f] = std::to_chars(ptr, ptr+3, esp_ip4_addr3_16(&ip_info.ip), 10);
ptr = e;
*ptr++ = '.';
auto [g,h] = std::to_chars(ptr, ptr+3, esp_ip4_addr4_16(&ip_info.ip), 10);
ptr = g;
*ptr = 0;
outputs.ip_address = ip_string;
// @/
```

# WiFi Summary

```cpp
// @#'sygbe-wifi.hpp'
/*
Copyright 2022 Edu Meneses https://www.edumeneses.com, Metalab - Société des
Arts Technologiques (SAT), Input Devices and Music Interaction Laboratory
(IDMIL), McGill University

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include <charconv>
#include <cstring>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <sygah-metadata.hpp>
#include <sygah-endpoints.hpp>
#include <sygup-cstdio_logger.hpp>

namespace sygaldry { namespace sygbe {
///\addtogroup sygbe
///\{
///\defgroup sygbe-wifi sygbe-wifi: ESP32 WiFi
///\{

struct WiFi
: name_<"WiFi Manager">
, author_<"Edu Meneses (2022) and Travis J. West (2023)">
, copyright_<"Copyright 2023 Travis J. West">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
, description_<"An ESP-IDF WiFi API wrapper originally adapted from the Puara Module Manager">
{
    struct inputs_t {
        @{inputs}
    } inputs;

    struct outputs_t {
        @{outputs}
    } outputs;

    @{handler_state_t}

    @{set_wifi_mode}

    [[no_unique_address]] sygup::CstdioLogger log;

    void init()
    {
         @{initialize wifi}
    }

    void main() { return; }
};

///\}
///\}
} }
// @/

// TODO: this component should not be header only
```

```cmake
# @#'CMakeLists.txt'
set(lib sygbe-wifi)

add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
    INTERFACE sygup-cstdio_logger
    INTERFACE sygah-endpoints
    INTERFACE sygah-metadata
    INTERFACE idf::nvs_flash
    INTERFACE idf::esp_wifi
    INTERFACE idf::esp_common
    INTERFACE idf::freertos
    )
# @/
```

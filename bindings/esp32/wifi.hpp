#pragma once
#include <esp_wifi.h>
#include <helpers/metadata.hpp>
#include <helpers/endpoints.hpp>

namespace sygaldry { namespace bindings { namespace esp32 {

struct WiFi
: name_<"WiFi Manager">
, author_<"Travis J. West">
, copyright_<"Travis J. West (C) 2023">
, description_<"An ESP-IDF WiFi API wrapper">
{
    struct inputs_t {
        text<"SSID"> ssid;
        struct password_t : text<"password">, tag_write_only { using text::operator=; } password;
    } inputs;

    struct outputs_t {
        // TODO: promiscuous mode
        // TODO: channel state information (csi) callback
    } outputs;

    struct parts_t {
    } parts;

    struct state_t {
    } state;

    void init()
    {
        wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
        esp_err_t e = esp_wifi_init(&wifi_config);
    }

    void main()
    {
    }
};

} } }
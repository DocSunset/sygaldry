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
        toggle< "enable access point"
              , "Indicate whether to persistently enable the device-hosted network."
                "When this toggle is disabled, the access point is only enabled if"
                "the device fails to connect to WiFi in station mode."
              , 0
              , tag_session_data
              > enable_ap;
    } inputs;

    struct outputs_t {
        toggle<"WiFi connected", "Indicates when WiFi is successfully connected"> wifi_connected;
        toggle<"AP running", "Indicates when the device-hosted network is running"> ap_running;
        text<"WiFi MAC", "MAC address of the device as a WiFi station on the main network"> wifi_mac;
        text<"AP MAC", "MAC address of the device as a WiFi access point on the device-hosted network"> ap_mac;
        text<"IP address", "IP address of the device as a WiFi station on the main network."
                           "Use this address to send network messages to the device when WiFi is connected."> ip_address;
    } outputs;

    struct handler_state_t {
        EventGroupHandle_t event_group;
        char connection_attempts;
        sygup::CstdioLogger* log;
        static constexpr int connected_bit = BIT0;
        static constexpr int fail_bit = BIT1;
        static constexpr int maximum_connection_attempts = 2;
    };

    void set_wifi_mode(wifi_mode_t mode)
    {
        esp_wifi_set_mode(mode);

        if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
        {
            wifi_config_t sta_config{};
            std::memcpy(sta_config.sta.ssid, inputs.wifi_ssid.value.c_str(), inputs.wifi_ssid.value.length()+1);
            std::memcpy(sta_config.sta.password, inputs.wifi_password.value.c_str(), inputs.wifi_password.value.length()+1);
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
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
        }
    }

    [[no_unique_address]] sygup::CstdioLogger log;

    void init()
    {
         if (inputs.hostname.value.empty() || inputs.hostname.value.length() > 31)
         {
            inputs.hostname = "sygaldry_instrument";
            log.println("Warning: initialized hostname................ '", inputs.hostname.value, "'");
         }

         if (inputs.ap_ssid.value.empty() || inputs.ap_ssid.value.length() > 31)
         {
            inputs.ap_ssid = "sygaldry_admin";
            log.println("Warning: initialized access point SSID....... '", inputs.ap_ssid.value, "'");
         }

         if ( inputs.ap_password.value.empty() || inputs.ap_password.value.length() < 8 || inputs.ap_password.value.length() > 63)
         {
             inputs.ap_password = "sygaldry_admin";
            log.println("Warning: initialized access point password... '", inputs.ap_password.value, "'");
         }

         // TODO: just don't bother trying to connect to wifi in this case
         if ( inputs.wifi_ssid.value.empty() || inputs.wifi_ssid.value.length() > 31)
         {
             inputs.wifi_ssid = "sygaldry_wifi";
            log.println("Warning: initialized WiFi SSID............... '", inputs.wifi_ssid.value, "'");
         }

         if ( inputs.wifi_password.value.empty() || inputs.wifi_password.value.length() < 8 || inputs.wifi_password.value.length() > 63)
         {
             inputs.wifi_password = "sygaldry_admin";
            log.println("Warning: initialized WiFi password........... '", inputs.wifi_password.value, "'");
         }
         esp_err_t ret = nvs_flash_init();
         if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
           ESP_ERROR_CHECK(nvs_flash_erase());
           ret = nvs_flash_init();
         }
         ESP_ERROR_CHECK(ret);
         log.println("Initialized NVS");

         ESP_ERROR_CHECK(esp_netif_init());
         log.println("Initialized network interface");

         ESP_ERROR_CHECK(esp_event_loop_create_default());
         log.println("Created default event loop");

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
         log.println("Set hostnames");

         wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
         ESP_ERROR_CHECK(esp_wifi_init(&cfg));
         log.println("Initialized WiFi with default configuration");
         if (inputs.enable_ap) set_wifi_mode(WIFI_MODE_APSTA);
         else set_wifi_mode(WIFI_MODE_STA);
         auto sta_event_handler = +[](void * arg, esp_event_base_t event_base, long int event_id, void * event_data)
         {
             handler_state_t& handler_state = *(handler_state_t*)arg;
             auto& log = *handler_state.log;
             if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
             {
                 log.println("WiFi station started. Connecting to network...");
                 esp_wifi_connect();
             }
             else if (event_base == WIFI_EVENT && 
                        event_id == WIFI_EVENT_STA_DISCONNECTED)
             {
                 log.print("WiFi station disconnected. ");
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
         log.println("Registered WiFi station event handler");

         log.println("Started WiFi.\nWaiting for WiFi connection...");
         ESP_ERROR_CHECK(esp_wifi_start());

         EventBits_t bits = xEventGroupWaitBits( handler_state.event_group
                                               , handler_state.connected_bit | handler_state.fail_bit
                                               , pdFALSE, pdFALSE, portMAX_DELAY
                                               );
         if (bits & handler_state.connected_bit)
         {
             outputs.wifi_connected = 1;
             outputs.ap_running = inputs.enable_ap;
         }
         else // fail_bit
         {
             set_wifi_mode(WIFI_MODE_AP);
             outputs.ap_running = 1;
             outputs.wifi_connected = 0;
         }
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
    }

    void main() { return; }
};

///\}
///\}
} }

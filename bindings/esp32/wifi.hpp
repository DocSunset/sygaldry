#pragma once
#include <esp_wifi.h>
#include <helpers/metadata.hpp>
#include <helpers/endpoints.hpp>

namespace sygaldry { namespace bindings { namespace esp32 {

struct WiFi
: name_<"WiFi Manager">
, author_<"Edu Meneses (2022) and Travis J. West (2023)">
, copyright_<"Travis J. West (C) 2023">
, description_<"An ESP-IDF WiFi API wrapper adapted from the Puara Module Manager">
{
    struct inputs_t {
        struct hostname_t
        : text<"hostname", "Name of this device on the network">
        , tag_session_data { using text::operator=; } hostname;

        struct ap_ssid_t
        : text<"access point SSID", "Name of the device-hosted network">
        , tag_session_data { using text::operator=; } ap_ssid;

        struct ap_password_t
        : text<"access point password", "Password of the device-hosted network">
        , tag_write_only, tag_session_data { using text::operator=; } ap_password;

        struct wifi_ssid_t
        : text<"WiFi SSID", "Name of the WiFi network to connect to">
        , tag_session_data { using text::operator=; } wifi_ssid;

        struct password_t
        : text<"WiFi password", "Password of the WiFi network to connect to">
        , tag_write_only, tag_session_data { using text::operator=; } wifi_password;
    } inputs;

    struct outputs_t {
        button<"access point started"> ap_started;
        // TODO: promiscuous mode
        // TODO: channel state information (csi) callback
    } outputs;

    struct parts_t {
    } parts;

    struct state_t {
    } state;

    void init()
    {
        /* Originally adapted from Puara Module Manager

         * Metalab - Société des Arts Technologiques (SAT)
         * Input Devices and Music Interaction Laboratory (IDMIL), McGill University
         * Edu Meneses (2022) - https://www.edumeneses.com

         * Used under license:
         * MIT License

         * Copyright (c) 2022 Puara

         * Permission is hereby granted, free of charge, to any person obtaining a copy
         * of this software and associated documentation files (the "Software"), to deal
         * in the Software without restriction, including without limitation the rights
         * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
         * copies of the Software, and to permit persons to whom the Software is
         * furnished to do so, subject to the following conditions:

         * The above copyright notice and this permission notice shall be included in all
         * copies or substantial portions of the Software.

         * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
         * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
         * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
         * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
         * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
         * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
         * SOFTWARE.
         */
        outputs.ap_started = false;

        if (inputs.hostname.value.empty())
           inputs.hostname = "sygaldry_instrument";

        if (inputs.ap_ssid.value.empty())
           inputs.ap_ssid = "sygaldry_network";

        if ( inputs.ap_password.value.empty() || inputs.ap_password.value.length() < 8)
            inputs.ap_password = "sygaldry_admin";

        if ( inputs.wifi_ssid.value.empty())
            inputs.wifi_ssid = "sygaldry_wifi";

         /*

        Puara::connect_counter = 0;
        wifi_init();
        ApStarted = true;       esp_err_t e = esp_wifi_init(&wifi_config);
        s_wifi_event_group = xEventGroupCreate();

        ESP_ERROR_CHECK(esp_netif_init());

        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();
        esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap(); // saving pointer to 
                                                                    // retrieve AP ip later

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        // Set device hostname
        esp_err_t setname = tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, 
                                                      dmiName.c_str());
        if(setname != ESP_OK ){
            std::cout << "wifi_init: failed to set hostname: " << dmiName  << std::endl;  
        } else {
            std::cout << "wifi_init: hostname: " << dmiName << std::endl;  
        }

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &Puara::sta_event_handler,
                                                            NULL,
                                                            &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &Puara::sta_event_handler,
                                                            NULL,
                                                            &instance_got_ip));

        std::cout << "wifi_init: setting wifi mode" << std::endl;
        if (persistentAP) {
            std::cout << "wifi_init:     AP-STA mode" << std::endl;
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
            std::cout << "wifi_init: loading AP config" << std::endl;
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap));
        } else {
            std::cout << "wifi_init:     STA mode" << std::endl;
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        }
        std::cout << "wifi_init: loading STA config" << std::endl;
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_sta) );
        std::cout << "wifi_init: esp_wifi_start" << std::endl;
        ESP_ERROR_CHECK(esp_wifi_start());

        std::cout << "wifi_init: wifi_init finished." << std::endl;

        // Waiting until either the connection is established (Puara::wifi_connected_bit)
        // or connection failed for the maximum number of re-tries (Puara::wifi_fail_bit).
        // The bits are set by event_handler() (see above)
        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                Puara::wifi_connected_bit | Puara::wifi_fail_bit,
                pdFALSE,
                pdFALSE,
                portMAX_DELAY);

        // xEventGroupWaitBits() returns the bits before the call returned, hence we
        // can test which event actually happened.
        if (bits & Puara::wifi_connected_bit) {
            std::cout << "wifi_init: Connected to SSID: " << Puara::wifiSSID  << std::endl;
            currentSSID = wifiSSID;
            Puara::StaIsConnected = true;
        } else if (bits & Puara::wifi_fail_bit) {
            std::cout << "wifi_init: Failed to connect to SSID: " << Puara::wifiSSID  << std::endl;
            if (!persistentAP) {
                std::cout << "wifi_init: Failed to connect to SSID: " << Puara::wifiSSID << "Switching to AP/STA mode" << std::endl;
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
                std::cout << "wifi_init: loading AP config" << std::endl;
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap));
                std::cout << "wifi_init: Trying to connect one more time to SSID before giving up." << std::endl;
                ESP_ERROR_CHECK(esp_wifi_start());
            } else {
                Puara::StaIsConnected = false;
            }
        } else {
            std::cout << "wifi_init: UNEXPECTED EVENT" << std::endl;
        }

        // The event will not be processed after unregister
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, 
                                                              IP_EVENT_STA_GOT_IP, 
                                                              instance_got_ip));
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, 
                                                              ESP_EVENT_ANY_ID, 
                                                              instance_any_id));
        vEventGroupDelete(s_wifi_event_group);

        // getting extra info
        unsigned char temp_info[6] = {0};
        esp_wifi_get_mac(WIFI_IF_STA, temp_info);
        std::ostringstream tempBuf;
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[0] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[1] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[2] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[3] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[4] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[5];
        Puara::currentSTA_MAC = tempBuf.str();
        tempBuf.clear();            // preparing the ostringstream 
        tempBuf.str(std::string()); // buffer for reuse
        esp_wifi_get_mac(WIFI_IF_AP, temp_info);
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[0] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[1] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[2] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[3] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[4] << ":";
        tempBuf << std::setfill('0') << std::setw(2) << std::hex << (int)temp_info[5];
        Puara::currentAP_MAC = tempBuf.str();

        esp_netif_ip_info_t ip_temp_info;
        esp_netif_get_ip_info(ap_netif, &ip_temp_info);
        tempBuf.clear();
        tempBuf.str(std::string());
        tempBuf << std::dec << esp_ip4_addr1_16(&ip_temp_info.ip) << ".";
        tempBuf << std::dec << esp_ip4_addr2_16(&ip_temp_info.ip) << ".";
        tempBuf << std::dec << esp_ip4_addr3_16(&ip_temp_info.ip) << ".";
        tempBuf << std::dec << esp_ip4_addr4_16(&ip_temp_info.ip);
        Puara::currentAP_IP = tempBuf.str();
        */
    }

    void main()
    {
    }
};

} } }

# ESP32 SPIFFS Session Storage Component

This document describes the implementation of the SPIFFS session data storage
component for ESP32, compatible with the
[RapidJSON session manager](bindings/rapidjson.lili.md).

[TOC]

TODO: license text

# Overview

# Summary

```cpp
// @#'spiffs.hpp'
#pragma once

#include <cstdio>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include "esp_spiffs.h"
#include "bindings/rapidjson.hpp"

namespace sygaldry { namespace bindings {

template<typename Components>
struct SpiffsSessionStorage
: name_<"SPIFFS Session Storage">
{
    static constexpr const char * spiffs_base_path = "/spiffs";
    static constexpr const char * file_path = "/spiffs/session_storage.json";
    static constexpr std::size_t buffer_size = 1024;

    struct OStream : public rapidjson::Writer<rapidjson::FileWriteStream> {
        std::FILE * fp;
        char buffer[buffer_size];
        rapidjson::FileWriteStream ostream;
        OStream()
        : fp{std::fopen(file_path, "w")}, ostream{fp, buffer, buffer_size}
        , rapidjson::Writer<rapidjson::FileWriteStream>(ostream)
        {}
        OStream~() {std::fclose(fp);}
    };

    struct parts_t {
        RapidJsonSessionStorage<rapidjson::FileReadStream, OStream, Components> json;
    } parts;

    void init(Components& components)
    {
        esp_vfs_spiffs_conf_t conf = {
              .base_path = "/spiffs",
              .partition_label = NULL,
              .max_files = 5,
              .format_if_mount_failed = true
            };
        esp_err_t ret = esp_vfs_spiffs_register(&conf);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        if (ret != ESP_OK) return;

        size_t total = 0, used = 0;
        ret = esp_spiffs_info(conf.partition_label, &total, &used);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
            esp_spiffs_format(conf.partition_label);
            return;
        }
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);

        // Check consistency of reported partiton size info.
        if (used > total) {
            ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
            ret = esp_spiffs_check(conf.partition_label);
            // Could be also used to mend broken files, to clean unreferenced pages, etc.
            // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
                return;
            } else {
                ESP_LOGI(TAG, "SPIFFS_check() successful");
            }
        }

        // open existing file or create an empty one
        std::FILE * fp = std::fopen(file_path, "r");
        if (fp == nullptr) fp = std::fopen(file_path, "w+");
        char buffer[buffer_size];
        rapidjson::FileReadStream istream{fp, buffer, buffer_size);
        parts.json.init(istream, components);
        std::fclose(fp);
    }

    void external_destnations(Components& components)
    {
        parts.json.external_destnations(components);
    }
};

} }
// @/
```

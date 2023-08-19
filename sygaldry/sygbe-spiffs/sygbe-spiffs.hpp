#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/


#include <cstdio>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <esp_spiffs.h>
#include "sygbp-rapid_json.hpp"

namespace sygaldry { namespace bindings { namespace esp32 {

struct SpiffsJsonOStream
{
    static constexpr const char * file_path = "/spiffs/session_storage.json";
    static constexpr std::size_t buffer_size = 1024;
    std::FILE * fp;
    char buffer[buffer_size];
    rapidjson::FileWriteStream ostream;
    rapidjson::Writer<rapidjson::FileWriteStream> writer;
    SpiffsJsonOStream()
    : fp{std::fopen(file_path, "w")}, buffer{0}
    , ostream{fp, buffer, buffer_size}, writer{ostream}
    {
        if (fp == nullptr) printf("spiffs: unable to open file for writing!\n");
        // The program will probably crash if this happens for some reason...
    }
    ~SpiffsJsonOStream() {std::fclose(fp);}
};

template<typename Components>
using Storage = RapidJsonSessionStorage<rapidjson::FileReadStream, SpiffsJsonOStream, Components>;

template<typename Components>
struct SpiffsSessionStorage
: Storage<Components>, name_<"SPIFFS Session Storage">
{
    static constexpr const char * spiffs_base_path = "/spiffs";
    static constexpr const char * file_path = SpiffsJsonOStream::file_path;
    static constexpr std::size_t buffer_size = SpiffsJsonOStream::buffer_size;

    void init(Components& components)
    {
        // Set up spiffs
        esp_vfs_spiffs_conf_t conf = {
              .base_path = "/spiffs",
              .partition_label = NULL,
              .max_files = 5,
              .format_if_mount_failed = true
            };
        esp_err_t ret = esp_vfs_spiffs_register(&conf);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        if (ret != ESP_OK) return;
        // Check partition size info
        size_t total = 0, used = 0;
        ret = esp_spiffs_info(conf.partition_label, &total, &used);
        if (ret != ESP_OK) {
            printf("spiffs: Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
            esp_spiffs_format(conf.partition_label);
            return;
        }
        printf("spiffs: Partition size: total: %d, used: %d", total, used);

        // Check consistency of reported partiton size info.
        if (used > total) {
            printf("spiffs: Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
            ret = esp_spiffs_check(conf.partition_label);
            // Could be also used to mend broken files, to clean unreferenced pages, etc.
            // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
            if (ret != ESP_OK) {
                printf("spiffs: SPIFFS_check() failed (%s)", esp_err_to_name(ret));
                return;
            } else {
                printf("spiffs: SPIFFS_check() successful");
            }
        }
        // Open existing file or create an empty one
        std::FILE * fp = std::fopen(file_path, "r");
        if (fp == nullptr) fp = std::fopen(file_path, "w+");
        if (fp == nullptr) {
            printf("spiffs: Unable to open file for initialization!\n");
            return;
        }
        char buffer[buffer_size];
        rapidjson::FileReadStream istream{fp, buffer, buffer_size};
        Storage<Components>::init(istream, components);
        std::fclose(fp);
    }

    void external_destinations(Components& components)
    {
        Storage<Components>::external_destinations(components);
    }
};

} } }

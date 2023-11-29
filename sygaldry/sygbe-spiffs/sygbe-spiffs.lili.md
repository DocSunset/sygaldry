\page page-sygbe-spiffs sygbe-spiffs: ESP32 SPIFFS Session Storage

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This document describes the implementation of the SPIFFS session data storage
component for ESP32, compatible with the
[RapidJSON session manager](\ref page-sygbp-rapid_json).

[TOC]

# Overview

Much of the functionality of this component is derived from the underlying JSON
session management component, which formats the stored data when session
parameters change. This component, the SPIFFS storage component, has these main
responsibilities: to set up the SPIFFS virtual filesystem, open and close files
appropriately, and pass streams to `RapidJsonSessionStorage` so that it can
read and write from these files.

# Implementation

## Init

Most of the initialization subroutine is copied with permission from
[the ESP-IDF examples for using the spiffs filesystem](https://github.com/espressif/esp-idf/tree/v5.1-rc1/examples/storage/spiffs).

The filesystem is registered, causing the framework to initialize the driver.

```cpp
// @='init'
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
// @/
```

We then check the partition size and usage, and verify that it seems
reasonable; if it doesn't, we attempt to repair the filesystem.

```cpp
// @+'init'
// Check partition size info
size_t total = 0, used = 0;
ret = esp_spiffs_info(conf.partition_label, &total, &used);
if (ret != ESP_OK) {
    printf("spiffs: Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
    esp_spiffs_format(conf.partition_label);
    return;
}
printf("spiffs: Partition size: total: %d, used: %d\n", total, used);

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
// @/
```

We then open
the file for reading the stored JSON data, creating one if it doesn't already
exist--

```cpp
// @+'init'
// Open existing file or create an empty one
std::FILE * fp = std::fopen(file_path, "r");
if (fp == nullptr) fp = std::fopen(file_path, "w+");
if (fp == nullptr) {
    printf("spiffs: Unable to open file for initialization!\n");
    return;
}
// @/
```

--and call on the storage class's init subroutine to read the data.

```cpp
// @+'init'
char buffer[buffer_size];
rapidjson::FileReadStream istream{fp, buffer, buffer_size};
storage->init(istream, components);
std::fclose(fp);
// @/
```

## Main

The main subroutine simply delegates to the session management class. Most of
the SPIFFS-specific functionality for this subroutine is wrapped in the output
stream class.

### SpiffsJsonOStream

As seen in the tests for `RapidJsonSessionStorage`, the output stream needs to
be wrapped in a class that prepares the stream on construction and cleans it up
on destruction. This is achieved very simply here using `rapidjson::FileWriteStream`
and the standard library. The constructor simply opens a file for writing (truncating
its contents), zero initializes a buffer, passes these to the `rapidjson` stream,
and passes the stream to a `rapidjson` writer. The stream is then ready to accept
data. When the object destructor is called, the file is closed.

Wrapping the stream in this way allows the overhead of opening and closing a file,
and the strong side effect of truncating the file when opening it in write mode,
to be avoided when the file does not need to be updated.

```cpp
// @='SpiffsJsonOStream'
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
// @/
```

# Summary

```cpp
// @#'sygbe-spiffs.hpp'
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

namespace sygaldry { namespace sygbe {
///\addtogroup sygbe
///\{
///\defgroup sygbe-spiffs sygbe-spiffs: ESP32 SPIFFS Session Storage
///\{

@{SpiffsJsonOStream}

template<typename Components>
using Storage = sygbp::RapidJsonSessionStorage<rapidjson::FileReadStream, SpiffsJsonOStream, Components>;

template<typename Components>
struct SpiffsSessionStorage
: name_<"SPIFFS Session Storage">
{
    Storage<Components> * storage;
    static constexpr const char * spiffs_base_path = "/spiffs";
    static constexpr const char * file_path = SpiffsJsonOStream::file_path;
    static constexpr std::size_t buffer_size = SpiffsJsonOStream::buffer_size;

    void init(Components& components)
    {
        storage = new Storage<Components>();
        @{init}
    }

    void external_destinations(Components& components)
    {
        storage->external_destinations(components);
    }
};

///\}
///\}
} }
// @/
```

TODO: this implementation is currently vulnerable to power-cycling after the file is opened
but before the data is written to it.

```cmake
# @#'CMakeLists.txt'
set(lib sygbe-spiffs)

add_library(${lib} INTERFACE)
target_include_directories(${lib}
        INTERFACE ${SYGALDRY_ROOT}/dependencies/rapidjson/include
        INTERFACE .
        )
target_link_libraries(${lib}
    INTERFACE sygbp-rapid_json
    INTERFACE idf::spiffs
    )
# @/
```

## Partition Table

In order to use SPIFFS, we are required to provide a custom partition table
that declares a data partition with `spiffs` subtype where the SPIFFS will be
located, such as the following:

```csv
# name,   type, subtype,   offset,     size,   flags
nvs,      data, nvs,       0x9000,   0x4000,
phy_init, data, phy,       0xf000,   0x1000,
main,     app,  factory,  0x10000, 0x290000,
storage,  data, spiffs,  0x300000,       1M,
```

The `sdkconfig` is then directed to use this partition table by setting
the following two lines:

```
CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"
```

Clients using SPIFFS must remember to manually make these changes.

\page page-sygbp-liblo Liblo OSC Binding

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later

[TOC]

This document describes the implementation of the liblo OSC binding component.

TODO: this binding needs tests!

# Overview

The liblo binding has the following responsibilities:

- Create at least one `lo_address`.
- Start and manage interaction with the liblo server thread
    - including creating the thread with `lo_server_new`,
    - registering an error handler callback with the same,
    - registering at least one handler with `lo_server_add_method`,
- Route incoming messages to the appropriate endpoints, e.g. within a server
  thread method or methods
- Recognize when output signals have changed, and produce output messages
  accordingly using `lo_send` or another method

# Limitations

Liblo requires the availability of POSIX networking and pthreads APIs. This is
currently available on most personal computer operating systems, SOC embedded
processors such as Raspberry Pi and Bela, as well as ESP32, but not on most MCU
platforms. Due to discrepancies in the quality of the timebase on these
platforms, especially those that may or may not have any form of clock
synchronization available such as the ESP32, OSC functionality relying on
accurate time stamps may not behave as intended.

# Init

## Server

First, we wish to set up a liblo server.

```cpp
// @+'data members'
lo_server server{};
// @/
```

This requires a port. We declare a text input to hold this parameter.

```cpp
// @+'inputs'
text_message< "source port"
            , "The UDP port on which to receive incoming messages."
            , tag_session_data
            > src_port;
// @/
```

Setting up the server requires the source port and an error handler which we
will describe momentarily. Liblo helpfully tries to find an unused port in case
the server port argument is null; we can take advantage of this in case the
user has not specified a source port. However, we have to take into account
the possibility that this process will fail, or that the user will pass an
invalid port number or one that is already taken. We signal such conditions
with an output toggle.

```cpp
// @+'outputs'
toggle<"server running"> server_running;
// @/
```

To detect when the port is not set, or is invalid, we define a helper method
that checks whether the port can be parsed as an int in the valid range for UPD
ports. Ports less than 1024 are generally reserved by operating systems, so are
not allowed here since they should generally not be used for OSC.

See the helpful
[list of TCP and UDP ports on Wikipedia](https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers)
for more information.

```cpp
// @+'set_server'
bool port_is_valid(auto& port)
{
    int port_num = -1;
    auto [ _, ec ] = std::from_chars(port->c_str(), port->c_str() + port->length(), port_num);
    bool ret = ec == std::errc{} && (1024 <= port_num && port_num <= 65535);
    if (not ret) fprintf(stderr, "liblo: invalid port %s (parsed as %d)\n", port->c_str(), port_num);
    return ret;
}
// @/

// @+'tests'
TEST_CASE("liblo osc port is valid")
{
    LibloOsc<TestComponent> osc;
    text_message<"s1"> s1{};
    text_message<"s2"> s2{string("7777")};
    text_message<"s3"> s3{string("77777")};
    CHECK(not osc.port_is_valid(s1));
    CHECK(osc.port_is_valid(s2));
    CHECK(not osc.port_is_valid(s3));
}
// @/
```

```cpp
// @+'set_server'
void set_server(auto& components)
{
    bool user_src_port = port_is_valid(inputs.src_port);

    // set the server if the server is not running or the source port has been validly updated
    if (outputs.server_running && not (inputs.src_port.updated && user_src_port)) return;

    fprintf(stdout, "liblo: setting up server\n");

    if (server) lo_server_free(server);

    if (not user_src_port)
    {
        fprintf(stdout, "liblo: searching for unused port\n");
        server = lo_server_new(NULL, &LibloOsc::server_error_handler);
    }
    else
    {
        fprintf(stdout, "liblo: using given port %s\n", inputs.src_port->c_str());
        server = lo_server_new(inputs.src_port->c_str(), &LibloOsc::server_error_handler);
    }
    if (server == NULL)
    {
        fprintf(stderr, "liblo: server setup failed\n");
        outputs.server_running = 0;
        return;
    }

    fprintf(stderr, "liblo: server setup successful\n");

    if (not user_src_port)
    {
        char port_str[6] = {0};
        int port_num = lo_server_get_port(server);
        snprintf(port_str, 6, "%d", port_num);
        inputs.src_port.value() = port_str;
        clear_flag(inputs.src_port); // clear flag to avoid triggering set_server again on tick
        fprintf(stdout, "liblo: connected on port %s\n", inputs.src_port->c_str());
    }
    else
        fprintf(stdout, "liblo: connected on port %s\n", inputs.src_port->c_str());

    fprintf(stderr, "liblo: registering callbacks\n");
    @{register callbacks}
    fprintf(stderr, "liblo: done registering callbacks\n");

    outputs.server_running = 1;
    return;
}
// @/

// @+'init'
fprintf(stdout, "liblo: initializing\n");
outputs.server_running = 0; // so that set_server doesn't short circuit immediately
set_server(components);
// @/
```

The error handler simply logs the error. Liblo's error codes are inherited from
whatever Posix network API implementation generates them, so it's very
challenging to handle them. In normal use, errors should not occur.

```cpp
// @='server_error_handler'
static void server_error_handler(int num, const char *msg, const char *where)
{
    fprintf(stderr, "liblo error: %s %s\n", msg, where);
}
// @/
```

## Destination Address

Setting up the address is a similar matter, however, we require an address
*and* port before we can create the `lo_address`. Ideally we could have
multiple destinations, but for now this is left as future work.

```cpp
// @+'data members'
lo_address dst{};
// @/
```

```cpp
// @+'inputs'
text_message< "destination port"
            , "The UDP port on which to send outgoing messages."
            , tag_session_data
            > dst_port;
text_message< "destination address"
            , "The IP address to send outgoing messages to."
            , tag_session_data
            > dst_addr;
// @/
```

We define a subroutine that we will call when all the necessary information is
available; if the address and port are ready during initialization, the address
will be set immediately. Otherwise, it will be run in the main subroutine when
the address and port have been set.

```cpp
// @+'outputs'
toggle<"output running"> output_running;
// @/

// @='set_dst'
bool ip_addr_is_valid(auto& ip)
{
    // TODO: implement a more robust verification of the dst ip address
    bool ret = ip->length() >= 7;
    if (not ret) fprintf(stderr, "liblo: invalid IP address %s\n", ip->c_str());
    return ret;
}

bool dst_inputs_are_valid()
{
    return ip_addr_is_valid(inputs.dst_addr) and port_is_valid(inputs.dst_port);
}

void set_dst()
{
    bool dst_updated = (inputs.dst_port.updated || inputs.dst_addr.updated);
    if (not (dst_updated && dst_inputs_are_valid()) ) return;
    fprintf(stdout, "liblo: setting destination address to %s:%s\n"
           , inputs.dst_addr->c_str(), inputs.dst_port->c_str()
           );
    if (dst) lo_address_free(dst);
    dst = lo_address_new(inputs.dst_addr->c_str(), inputs.dst_port->c_str());
    if (dst) outputs.output_running = 1;
    else
    {
        outputs.output_running = 0;
        fprintf(stderr, "liblo: unable to set destination address\n");
    }
}
// @/

// @+'init'
outputs.output_running = 0;
set_dst();
// @/
```

## Registering callbacks

There are broadly two appoaches that we could take for the server callback
methods. On one hand, we could register one method for each input endpoint in
the component tree. This may use more memory at runtime, as each method
requires a dynamic allocation for a `lo_method` struct. In exchange, liblo
handles all of the pattern and argument type matching without our intervention.
On the other hand, we could register a single callback, and perform the pattern
matching and type checking ourselves, perhaps making use of liblo's pattern
matching functionality. This may increase the compiled executable size, since
our template-heavy framework is likely to generate a lot of similar code paths
for each endpoint. For now, we opt for the former approach, as it enables us to
get to a working implementation more quickly. If runtime memory becomes an
issue, this may be one area where gains could be made.

For each input endpoint in the component tree, we register a callback that
defers setting the input to an overloaded template function that does most of
the work; the callback registered with liblo basically just holds the type
information of the endpoint.

```cpp
// @='register callbacks'
for_each_input(components, [&]<typename T>(T& in)
{
    fprintf(stdout, "liblo: registering callback for %s ... ", name_of(in));
    constexpr auto handler = +[]( const char *path, const char *types
                                , lo_arg **argv, int argc, lo_message msg
                                , void *user_data
                                )
    {
        #ifndef NDEBUG
            fprintf(stdout, "liblo: got message %s", path);
            lo_message_pp(msg);
        #endif
        T& in = *(T*)user_data;
        LibloOsc::set_input(path, types, argv, argc, msg, in);
        return 0;
    };
    lo_server_add_method( server
                        , osc_path_v<T, Components>, osc_type_string_v<T>+1
                        , handler, (void*)&in
                        );
    fprintf(stdout, "done\n");
});
// @/
```

The inner callback simply inspects the endpoint and attempts to apply the OSC
arguments to it. This implementation bears a lot of duplication that should be
reduced in future work.

```cpp
// @='set_input'
template<typename T> static void
set_input(const char *path, const char *types
         , lo_arg **argv, int argc, lo_message msg
         , T& in
         )
{
    if constexpr (Bang<T>) in = true;
    else if constexpr (std::integral<value_t<T>>)
    {
        if (types[0] != 'i')
        {
            fprintf(stderr, "liblo: wrong type; expected 'i', got '%c'\n", types[0]);
            return;
        }
        value_of(in) = argv[0]->i;
    }
    else if constexpr (std::floating_point<value_t<T>>)
    {
        if (types[0] != 'f')
        {
            fprintf(stderr, "liblo: wrong type; expected 'f', got '%c'\n", types[0]);
            return;
        }
        value_of(in) = argv[0]->f;
    }
    else if constexpr (string_like<value_t<T>>)
    {
        if (types[0] != 's')
        {
            fprintf(stderr, "liblo: wrong type; expected 's', got '%c'\n", types[0]);
            return;
        }
        value_of(in) = &argv[0]->s;
    }
    else if constexpr (array_like<value_t<T>>) for (std::size_t i = 0; i < size<value_t<T>>(); ++i)
    {
        auto& element = value_of(in)[i];
        if constexpr (std::integral<element_t<T>>)
        {
            if (types[i] != 'i')
            {
                fprintf(stderr, "liblo: wrong type; expected 'i', got '%c'\n", types[i]);
                return;
            }
            element = argv[i]->i;
        }
        else if constexpr (std::floating_point<element_t<T>>)
        {
            if (types[i] != 'f')
            {
                fprintf(stderr, "liblo: wrong type; expected 'f', got '%c'\n", types[i]);
                return;
            }
            element = argv[i]->f;
        }
        else if constexpr (string_like<element_t<T>>)
        {
            if (types[i] != 's')
            {
                fprintf(stderr, "liblo: wrong type; expected 's', got '%c'\n", types[i]);
                return;
            }
            element = &argv[i]->s;
        }
    }
};
// @/
```

# Tick

On each tick, we perform three main tasks. External sources: We poll the server.

```cpp
// @='tick'
void external_sources()
{
    if (outputs.server_running) lo_server_recv_noblock(server, 0);
}
// @/
```

Main: We update the server and destination address parameters if they have
changed. TODO: this should happen in input callbacks

```cpp
// @+'tick'
void main(Components& components)
{
    set_server(components);
    set_dst();
}
// @/
```

External destinations: we send output messages. This step is a bit more
involved. An earlier implementation of this subroutine simply called `lo_send`
and its relatives to send each output endpoint's current value as an OSC
message. This results in a large number of calls to the underlying sockets API
`sendto` function, which was enough to overwhelm the ESP32 socket driver
buffers. To avoid this, and likely improve performance on all platforms, we
place our messages into a bundle so that only one socket `sendto` is issued for
each tick.

First, we only send messages if the output is running, i.e. we have a
destination IP address and port number. If so, we populate the message with
data from the endpoint. Once this is done, whatever messages were added
are sent over the network.

```cpp
// @+'tick'
void external_destinations(Components& components)
{
    if (outputs.output_running)
    {
        lo_bundle bundle = lo_bundle_new(LO_TT_IMMEDIATE);
        for_each_output(components, [&]<typename T>(T& output)
        {
            @{populate output messages}
        });
        int ret = lo_send_bundle(dst, bundle);
        if (ret < 0) fprintf( stderr, "liblo: error %d sending bundle --- %s\n"
                            , lo_address_errno(dst)
                            , lo_address_errstr(dst)
                            );
        lo_bundle_free_recursive(bundle);
    }
}
// @/
```

The way in which an output endpoint should be converted to OSC depends on its
type. Bangs and occasional values only need to be sent when they have been
updated. We check these types of endpoints for readiness before allocating a
message. Notice that the return statements here function as a way to break out
of the loop over output endpoints; they don't short circuit the overall
`external_destinations` subroutine.

```cpp
// @+'populate output messages'
if constexpr (OccasionalValue<T> || Bang<T>)
{
    if (not bool(output))
        return;
}

lo_message message = lo_message_new();
if (!message)
{
    perror("liblo: unable to malloc new message. perror reports: \n");
    return;
}
// @/
```

For endpoints with values, the message needs to be populated by calling
`lo_message_add`. We currently get the type tag string for one element in this
process. Future work should move this to `osc_string_constants.hpp`. Single
valued endpoints are sent relatively simply; we just stuff the value of the
endpoint into `lo_message_add`. Array's are similar, requiring us to iterate
over the array.

```cpp
// @+'populate output messages'
if constexpr (has_value<T> && not Bang<T>)
{
    int ret = 0;
    // TODO: this type tag string logic should be moved to osc_string_constants.lili.md
    constexpr auto type = std::integral<element_t<T>> ? "i"
                        : std::floating_point<element_t<T>> ? "f"
                        : string_like<element_t<T>> ? "s" : "" ;
    // TODO: we should have a more generic way to get a char * from a string_like value
    if constexpr (string_like<value_t<T>>)
        ret = lo_message_add_string(message, value_of(output).c_str());
    else if constexpr (array_like<value_t<T>>)
    {
        for (auto& element : value_of(output))
        {
            ret = lo_message_add(message, type, element);
            if (ret < 0) break;
        }
    }
    else ret = lo_message_add(message, type, value_of(output));

    if (ret < 0)
    {
        lo_message_free(message);
        return;
    }
}

int ret = lo_bundle_add_message(bundle, osc_path_v<T, Components>, message);
if (ret < 0) fprintf(stderr, "liblo: unable to add message to bundle.\n");
//lo_message_free(message); // bundle makes its own ref to message on success, so we need to free ours regardless
return;
// @/
```

# Liblo OSC Binding Summary

```cpp
// @#'sygbp-liblo.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <stdio.h>
#include <charconv>
#include <lo/lo.h>
#include <lo/lo_lowlevel.h>
#include <lo/lo_types.h>
#include "sygac-metadata.hpp"
#include "sygac-endpoints.hpp"
#include "sygah-endpoints.hpp"
#include "sygbp-osc_string_constants.hpp"

namespace sygaldry { namespace bindings {

template<typename Components>
struct LibloOsc
: name_<"Liblo OSC">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Travis J. West">
, license_<"SPDX-License-Identifier: LGPL-2.1-or-later">
, version_<"0.0.0">
, description_<"Open Sound Control bindings using the liblo library">
{
    struct inputs_t {
        @{inputs}
    } inputs;

    struct outputs_t {
        @{outputs}
    } outputs;

    @{data members}

    @{set_input}

    @{set_server}

    @{set_dst}

    @{server_error_handler}

    void init(Components& components)
    {
        @{init}
    }

    @{tick}
};

} }
// @/
```

```cpp
// @#'sygbp-liblo.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <string>
#include <catch2/catch_test_macros.hpp>
#include "sygac-components.hpp"
#include "sygah-endpoints.hpp"
#include "sygbp-test_component.hpp"
#include "sygbp-liblo.hpp"

using std::string;

using namespace sygaldry;
using namespace sygaldry::bindings;
using namespace sygaldry::components;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: LGPL-2.1-or-later

set(lib sygbp-liblo)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygac
        INTERFACE sygah
        INTERFACE sygbp-osc_string_constants
        )


if (SYGALDRY_BUILD_TESTS)
# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ${LIBLO_LIBRARIES}
        PRIVATE ${lib}
        PRIVATE sygac-components
        PRIVATE sygbp-test_component
        )
target_include_directories(${lib}-test PRIVATE ${LIBLO_INCLUDE_DIRS})
catch_discover_tests(${lib}-test)
endif()
# @/
```

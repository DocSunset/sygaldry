# Liblo OSC Binding

This document describes the implementation of the liblo OSC binding component.

```
@='license notice'
Copyright 2023 Travis J. West

This program (liblo.lili and liblo.hpp) is free software: you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
@/
```

[TOC]

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

// @+'set_server'
bool port_is_valid(auto& port)
{
    int port_num = -1;
    auto [ _, ec ] = std::from_chars(port->c_str(), port->c_str() + port->length(), port_num);
    return ec != std::errc{} && (1024 <= port_num && port_num <= 49151);
}

void set_server(auto& components)
{
    bool port_updated = inputs.src_port.updated && port_is_valid(inputs.src_port);
    if (outputs.server_running && not port_updated)
    {
        return;
    }

    fprintf(stdout, "liblo: setting up server\n");

    if (server) lo_server_free(server);

    bool no_user_src_port = not port_is_valid(inputs.src_port);

    if (no_user_src_port)
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

    if (no_user_src_port)
    {
        fprintf(stderr, "liblo: checking source port ... ");
        char port_str[6] = {0};
        int port_num = lo_server_get_port(server);
        snprintf(port_str, 6, "%d", port_num);
        inputs.src_port.value() = port_str;
        fprintf(stdout, "connected on port %s\n", inputs.src_port->c_str());
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
bool dst_inputs_are_valid()
{
    // TODO: implement a more robust verification of the dst ip address
    return inputs.dst_addr->length() >= 7 and port_is_valid(inputs.dst_port);
}

void set_dst()
{
    bool dst_updated = (inputs.dst_port.updated || inputs.dst_addr.updated) && dst_inputs_are_valid();
    if (outputs.output_running && not dst_updated) return;
    if (dst) lo_address_free(dst);
    dst = lo_address_new(inputs.dst_addr->c_str(), inputs.dst_port->c_str());
    if (dst) outputs.output_running = 1;
    else outputs.output_running = 0;
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

The inner callback simply inspects the endpoint and attempts to apply the
OSC arguments to it.

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
};
// @/
```

# Tick

On each tick, we perform three main tasks:
- external sources: We poll the server
- main: We update the server and destination address parameters if they have changed
    - TODO: this should happen in input callbacks
- external destinations: And we send output messages

```cpp
// @='tick'
void external_sources()
{
    if (outputs.server_running) lo_server_recv_noblock(server, 0);
}

void main(Components& components)
{
    set_server(components);
    set_dst();
}

void external_destinations(Components& components)
{
    if (outputs.output_running)
    {
        for_each_output(components, [&]<typename T>(const T& output)
        {
            if constexpr (Bang<T>)
            {
                if (value_of(output)) lo_send(dst, osc_path_v<T, Components>, NULL);
                return;
            }
            else if constexpr (has_value<T>)
            {
                if constexpr (OccasionalValue<T>)
                {
                    if (not bool(output)) return;
                }
                // TODO: we should have a more generic way to get a char * from a string_like value
                if constexpr (string_like<value_t<T>>)
                    lo_send(dst, osc_path_v<T, Components>, osc_type_string_v<T>+1, value_of(output).c_str());
                else
                    lo_send(dst, osc_path_v<T, Components>, osc_type_string_v<T>+1, value_of(output));
                return;
            }
        });
    }
}
// @/
```

# Liblo OSC Binding Summary

```cpp
// @#'liblo.hpp'
/*
@{copyright statement}
*/
#pragma once
#include <stdio.h>
#include <charconv>
#include <lo/lo.h>
#include <lo/lo_lowlevel.h>
#include <lo/lo_types.h>
#include "concepts/metadata.hpp"
#include "concepts/endpoints.hpp"
#include "helpers/endpoints.hpp"
#include "bindings/osc_string_constants.hpp"

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

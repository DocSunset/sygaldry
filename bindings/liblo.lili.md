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
    - including creating the thread with `lo_server_thread_new`,
    - registering an error handler callback with the same,
    - registering at least one handler with `lo_server_thread_add_method`,
    - and starting the thread with `lo_server_thread_start`.
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

## Server and Output Address

First, we wish to set up a server and output address.

```cpp
// @+'data members'
lo_server server;
lo_address dst;
// @/
```

Both of these require a port, and the output address additionally requires a
destination IP address. Ideally we could have multiple destinations, but for
now this is left as future work. We declare text inputs to hold these
parameters.

```cpp
// @+'inputs'
struct src_port_t
: text<"source port"
      , "The UDP port on which to receive incoming messages."
      >
, tag_session_data { using text::operator=; } src_port;

struct dst_port_t
: text<"destination port"
      , "The UDP port on which to send outgoing messages."
      >
, tag_session_data { using text::operator=; } dst_port;

struct dst_addr_t
: text<"destination address"
      , "The IP address to send outgoing messages to."
      >
, tag_session_data { using text::operator=; } dst_addr;
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
    auto [ ptr, ec ] = std::from_chars(port.value.c_str(), port.value.c_str() + port.value.length(), port_num);
    return 1024 <= port_num && port_num <= 49151;
}

void set_server()
{
    if (server) lo_server_thread_free();
    outputs.server_running = 0;

    bool no_user_src_port = not port_is_valid(inputs.src_port);

    if (no_user_src_port)
        server = lo_server_thread_new(inputs.src_port.value.c_str(), &LibloOsc::server_error_handler);
    else
        server = lo_server_thread_new(NULL, &LibloOsc::server_error_handler);
    if (server == NULL) return;

    if (no_user_src_port)
    {
        char port_str[6] = {0};
        int port_num = lo_server_thread_get_port(server);
        snprintf(port_str, 6, "%d", port_num);
        inputs.src_port.value = port_str;
    }

    @{register callbacks}

    outputs.server_running = lo_server_thread_start(server) == 0 ? 1 : 0;
}
// @/

// @+'init'
set_server();
// @/
```

Setting up the address is a similar matter, however, we require an address and
port before we can create the `lo_address`. We define a subroutine that we will
call when all the necessary information is available; if the address and port
are ready during initialization, the address will be set immediately.
Otherwise, it will be run in the main subroutine when the address and port
have been set.

```cpp
// @+'outputs'
toggle<"output running"> output_running;
// @/

// @='set_dst'
void dst_inputs_are_valid()
{
    // TODO: implement a more robust verification of the dst ip address
    return inputs.dst_addr.value.length() >= 7 and port_is_valid(inputs.dst_port);
}

void set_dst()
{
    if (not dst_inputs_are_valid()) return;
    if (dst) lo_address_free(dst);
    dst = lo_address_new(inputs.dst_addr.value.c_str(), inputs.dst_port.value.c_str());
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
methods. One one hand, we could register one method for each input endpoint in
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
defers setting the input to an overloaded template function that does most
of the work; the callback basically just holds the type information of the
endpoint.

```cpp
// @='register callbacks'
for_each_input(components, [&]<typename T>(T& in)
{
    void * handler = +[]( const char *path, const char *types
                        , lo_arg **argv, int argc, lo_message msg
                        , void *user_data
                        )
    {
        T in = *(T*)user_data;
        set_input(path, types, argv, argc, msg, in);
    };
    auto method = lo_server_thread_add_method( server
                                             , osc_address_v<T>, osc_types_v<T>
                                             , handler, (void*)&in
                                             );
});
// @/
// @+'init'
// @/
```

# Main

On each call to the main subroutine, we perform three main tasks:
- We update the server and destination address parameters if they have changed
    - TODO: this should happen in input callbacks
- We safely apply changes from the server thread message queue
- And we send output messages if any endpoints have been updated

# Liblo OSC Binding Summary

```cpp
// @#'liblo.hpp'
/*
@{copyright statement}
*/
#pragma once
#include <charconv>
#include <lo.h>
#include <lo_serverthread.h>
#include <lo_types.h>

namespace sygaldry { namespace bindings {

// TODO: define a conversion from endpoint to typespec
template<typename T> constexpr const char * osc_types_v = "f";

// TODO: implement set_input
template<typename T> void set_input(const char *path, const char *types
                                   , lo_arg **argv, int argc, lo_message msg
                                   , T& in
                                   ) {};

template<typename Components>
struct LibloOsc
: name_<"Liblo OSC">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Travis J. West">
, license_<"SPDX-License-Identifier: LGPL-2.1-or-later"
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

    @{set_server}

    @{set_dst}

    void init(Components& components)
    {
        @{init}
    }

    void main(Components& components)
    {
        @{main}
    }
};

} }
// @/
```

```cpp
// @#'tests/liblo/demo.cpp'
#include "bindings/liblo.hpp"
#include "components/tests/testcomponent.hpp"

struct components_t
{
    struct api_t
    {
        TestComponent tc;
    } api;
    LibloOsc<decltype(api)> lo;
} components;

void main()
{
    init(components);

    for (;;)
    {
        activate(components);
    }
}
// @/
```

```cmake
# @#'tests/liblo/CMakeLists.txt'
# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(liblo-demo demo.cpp)
target_link_libraries(liblo-demo PRIVATE ${LIBLO_LIBRARIES})
target_include_directories(liblo-demo PRIVATE ${LIBLO_INCLUDE_DIRS})
# @/
```

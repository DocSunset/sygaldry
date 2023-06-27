/*
*/
#pragma once
#include <charconv>
#include <lo.h>
#include <lo_lowlevel.h>
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
        text< "source port"
            , "The UDP port on which to receive incoming messages."
            , tag_session_data
            > src_port;
        text< "destination port"
            , "The UDP port on which to send outgoing messages."
            , tag_session_data
            > dst_port;
        text< "destination address"
            , "The IP address to send outgoing messages to."
            , tag_session_data
            > dst_addr;
    } inputs;

    struct outputs_t {
        toggle<"server running"> server_running;
        toggle<"output running"> output_running;
    } outputs;

    lo_server server;
    lo_address dst;

    bool port_is_valid(auto& port)
    {
        int port_num = -1;
        auto [ ptr, ec ] = std::from_chars(port.value.c_str(), port.value.c_str() + port.value.length(), port_num);
        return 1024 <= port_num && port_num <= 49151;
    }

    void set_server()
    {
        if (server) lo_server_free();
        outputs.server_running = 0;

        bool no_user_src_port = not port_is_valid(inputs.src_port);

        if (no_user_src_port)
            server = lo_server_new(inputs.src_port.value.c_str(), &LibloOsc::server_error_handler);
        else
            server = lo_server_new(NULL, &LibloOsc::server_error_handler);
        if (server == NULL) return;


        if (no_user_src_port)
        {
            char port_str[6] = {0};
            int port_num = lo_server_get_port(server);
            snprintf(port_str, 6, "%d", port_num);
            inputs.src_port.value = port_str;
        }

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
            auto method = lo_server_add_method( server
                                              , osc_address_v<T>, osc_types_v<T>
                                              , handler, (void*)&in
                                              );
        });

        outputs.server_running = 1;
    }

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

    void init(Components& components)
    {
        set_server();
        outputs.output_running = 0;
        set_dst();
    }

    void main(Components& components)
    {
    }
};

} }

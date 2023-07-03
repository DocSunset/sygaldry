/*
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
        text_message< "source port"
                    , "The UDP port on which to receive incoming messages."
                    , tag_session_data
                    > src_port;
        text_message< "destination port"
                    , "The UDP port on which to send outgoing messages."
                    , tag_session_data
                    > dst_port;
        text_message< "destination address"
                    , "The IP address to send outgoing messages to."
                    , tag_session_data
                    > dst_addr;
    } inputs;

    struct outputs_t {
        toggle<"server running"> server_running;
        toggle<"output running"> output_running;
    } outputs;

    lo_server server{};
    lo_address dst{};

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
        fprintf(stderr, "liblo: done registering callbacks\n");

        outputs.server_running = 1;
        return;
    }

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

    static void server_error_handler(int num, const char *msg, const char *where)
    {
        fprintf(stderr, "liblo error: %s %s\n", msg, where);
    }

    void init(Components& components)
    {
        set_server(components);
        outputs.output_running = 0;
        set_dst();
    }

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
};

} }

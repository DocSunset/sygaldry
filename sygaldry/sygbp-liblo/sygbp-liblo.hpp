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

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp
///\{
///\defgroup sygbp-liblo sygbp-liblo: Liblo OSC Binding
///\{

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

    bool port_is_valid(auto& port)
    {
        int port_num = -1;
        auto [ _, ec ] = std::from_chars(port->c_str(), port->c_str() + port->length(), port_num);
        bool ret = ec == std::errc{} && (1024 <= port_num && port_num <= 65535);
        if (not ret) fprintf(stderr, "liblo: invalid port %s (parsed as %d)\n", port->c_str(), port_num);
        return ret;
    }
    void set_server(auto& components)
    {
        bool valid_user_src_port = port_is_valid(inputs.src_port);

        // do not reset the server if the server is already running and the source port has not been validly updated
        if (outputs.server_running && not (inputs.src_port.updated && valid_user_src_port)) return;

        fprintf(stdout, "liblo: setting up server\n");
        if (server) lo_server_free(server);

        if (not valid_user_src_port)
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

        if (not valid_user_src_port)
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

    static void server_error_handler(int num, const char *msg, const char *where)
    {
        fprintf(stderr, "liblo error: %s %s\n", msg, where);
    }

    void init(Components& components)
    {
        fprintf(stdout, "liblo: initializing\n");
        outputs.server_running = 0; // so that set_server doesn't short circuit immediately
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
            lo_bundle bundle = lo_bundle_new(LO_TT_IMMEDIATE);
            for_each_output(components, [&]<typename T>(T& output)
            {
                if constexpr (OccasionalValue<T> || Bang<T>)
                {
                    if (not flag_state_of(output))
                        return;
                }
                else if constexpr (requires (T t) {t == output;})
                {
                    static T prev{};
                    if (output == prev)
                        return;
                    prev = output;
                }

                lo_message message = lo_message_new();
                if (!message)
                {
                    perror("liblo: unable to malloc new message. perror reports: \n");
                    return;
                }
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
            });
            int ret = lo_send_bundle(dst, bundle);
            //if (ret < 0) fprintf( stderr, "liblo: error %d sending bundle --- %s\n"
            //                    , lo_address_errno(dst)
            //                    , lo_address_errstr(dst)
            //                    );
            lo_bundle_free_recursive(bundle);
        }
    }
};

///\}
///\}
} }

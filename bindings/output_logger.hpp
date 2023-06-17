#pragma once
#include "bindings/spelling.hpp"
#include "concepts/metadata.hpp"
#include "concepts/components.hpp"
#include "bindings/spelling.hpp"
#include "bindings/basic_logger/cstdio_logger.hpp"

namespace sygaldry { namespace bindings {

template<typename Logger, typename Components>
struct OutputLogger
{
    struct parts_t {
        Logger log;
    } parts;

    output_endpoints_t<Components> last_out_list{};

    void operator()(Components& components)
    {
        tuple_for_each(last_out_list, [&]<typename T>(T& last_out)
        {
            const T& current_out = find<T>(components);
            if (value_of(current_out) != value_of(last_out))
            {
                if constexpr (Bang<T>)
                {
                    if (value_of(current_out))
                        parts.log.println(osc_address_v<path_t<T, Components>>);
                    return;
                }
                else
                {
                    last_out = current_out;
                    parts.log.print(osc_address_v<path_t<T, Components>>);
                    if constexpr (has_value<T>)
                        parts.log.print(" ", value_of(current_out));
                    parts.log.println();
                }
            }
        });
    }
};

template<typename Components> using CstdioOutputLogger = OutputLogger<CstdioLogger, Components>;

} }

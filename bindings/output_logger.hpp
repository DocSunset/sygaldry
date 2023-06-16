#pragma once
#include "bindings/spelling.hpp"
#include "concepts/components.hpp"
#include "bindings/basic_logger/test_logger.hpp"

namespace sygaldry { namespace bindings {

template<typename Components>
struct CstdioOutputLogger
{
    struct parts_t {
        TestLogger log;
    } parts;

//    endpoints_tuple<outputs_of<Components>> last_endpoints_out{};

    void operator()(Components& components)
    {
//        std::apply([&]<typename T>(T& last_out)
//        {
//            using Path = path_t<T, Components>;
//            const T& current_out = find<T>(components);
//            if (current_out != last_out)
//            {
//                last_out = current_out;
//
//                if constexpr (Bang<T>) if (not current_out) return;
//
//                for_each_type<Path>([&]<typename Y>() {
//                    if constexpr (not has_name<Y>) return;
//                    parts.log.print(snake_case_v<T>);
//                    if constexpr (not std::is_same_v<T, Y>)
//                        parts.log.print("/");
//                });
//
//                if constexpr (has_value<T>)
//                    parts.log.print(value_of(current_out));
//
//                parts.log.println();
//            }
//        }, last_endpoints_out);
    }
};

} }

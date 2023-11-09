set(lib sygbr-runtime)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygac-runtime
        INTERFACE pico_stdlib
        #INTERFACE sygsr-two_wire
        #INTERFACE sygbr-flash
        #INTERFACE sygbr-wifi
        #INTERFACE sygbp-liblo
        #INTERFACE sygbp-cli
        )
#target_compile_definitions(${lib} INTERFACE
#        PICO_STDIO_DEFAULT_CRLF=0
#        )

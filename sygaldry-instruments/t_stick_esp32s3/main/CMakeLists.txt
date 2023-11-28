idf_component_register(SRCS "t_stick.cpp"
        )

set(SYGALDRY_I2C_PINS 1)
set(SYGALDRY_SDA 12)
set(SYGALDRY_SCL 11)
add_subdirectory(../../../ sygbuild) # add sygaldry as a subdirectory

target_compile_options(${COMPONENT_LIB} PRIVATE
        "-Wfatal-errors"
        "-Wno-error=unused-but-set-parameter"
        "-ftemplate-backtrace-limit=0"
        )
target_link_libraries(${COMPONENT_LIB} PRIVATE sygaldry)

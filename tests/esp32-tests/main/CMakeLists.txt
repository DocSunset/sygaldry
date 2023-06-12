idf_component_register(SRCS "test_runner.cpp"
        INCLUDE_DIRS ${SYGALDRY_ROOT}
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/pfr/include
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/mp11/include
        REQUIRES unity driver freertos
        )

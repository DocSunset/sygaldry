add_subdirectory(narrative)

include(ExternalProject)

set(esp32_project_name one-bit-bongo-esp32)

ExternalProject_Add(${esp32_project_name}
        PREFIX ${esp32_project_name}
        INSTALL_COMMAND ${CMAKE_CURRENT_LIST_DIR}/esp-install.sh ${ESPPORT} ${BUILD_COMMAND}
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/${esp32_project_name}
        CMAKE_CACHE_ARGS -DSYGALDRY_ROOT:STRING=${SYGALDRY_ROOT})

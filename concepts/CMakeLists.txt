add_library(sygaldry_concepts INTERFACE)
target_link_libraries(sygaldry_concepts INTERFACE Sygaldry::Utilities)
target_include_directories(sygaldry_concepts INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../dependencies/mp11/include"
        INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../dependencies/pfr/include"
        INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../dependencies/mp11/include"
        INTERFACE "${CMAKE_CURRENT_LIST_DIR}"
        )
add_library(Sygaldry::Concepts ALIAS sygaldry_concepts)

if (SYGALDRY_BUILD_TESTS)
add_subdirectory("tests/functions")
add_subdirectory("tests/endpoints")
add_subdirectory("tests/components")
add_subdirectory("tests/mimu")
add_subdirectory("tests/runtime")
endif()

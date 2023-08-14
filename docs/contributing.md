# Contributors Guide

This is currently a work in progress--not so much a guide as
a disorganized dumping ground where I note things that contributors
should probably be aware of.

- physical design
    - Read Lakos 2019
    - Watch this talk: https://mropert.github.io/2017/10/14/modern_cmake_video/
    - Every (Lakosian) component should have:
        - a cmake library target (`add_library(fully-qualified-component_name, ...)`)
        - one header file (`target_include_directories(name PUBLIC .)`)
        - maybe one impl header file
        - probably just one implementation file (`target_sources(name PRIVATE name.cpp`)
        - a test executable that depends only on that component's interface and its transitive dependencies
            - `add_executable(name-test name.test.cpp)`
            - `target_link_libraries(name-test PRIVATE name)`
            - anything required for the platform-specific testing framework
    - every sygaldry component (regular or binding) should be a Lakosian software component
    - each component should live in its own directory, typically containing a subset of the following files:
        - component.lili.md
        - doc/title of a tutorial.tutorial.md
        - doc/title of a guide.guide.md
        - sygaldry-package-component.meta.yaml
        - sygaldry-package-component.hpp
        - sygaldry-package-component.impl.hpp
        - sygaldry-package-component.cpp
        - sygaldry-package-component.test.cpp
        - CMakeLists.txt
    - machine source file naming convention: sygaldry-(package_name-)*componentname(.hpp|-impl.hpp|.cpp)
        - e.g. sygaldry-components-esp32_adc.hpp
        - start with the global namespace "sygaldry"
        - write each package or package group name separated by dashes
        - write the name of the component
        - this is fairly verbose, but necessary to avoid naming conflicts with external projects etc.
        - we avoid directory structures in include statements, since these enforce limits on how we can organize the library
            - CMake will take care of making sure the compiler gets all the -I flags it needs as long as we use `target_link_libraries` appropriately, and follow the physical design rules so that this is possible
    - template compile time optimization
        - complex templates, especially bindings, should have separate interface (`.hpp`) and implementation (`.impl.hpp`)
- implementation conventions
    - a trailing underscore denotes a private entity and/or an implementation detail that shouldn't be reused or overly scrutinized by component clients
        - this is an idea borrowed from python, except in python we use a leading underscore, but in c++ leading underscores are reserved for standard libraries and using them is a bad idea, so we use a trailing underscore instead
    - we use `UpperCamelCase` for typenames and concepts
    - we use `lower_camel_case` for variables, functions, methods, and type metafunctions
    - it's very early days, so if you notice an inconsistency feel free to fix it

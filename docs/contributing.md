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
        - we are currently not strict about Lakos's policy that there must always be an `hpp` and a `cpp`.
        - the existence of a single lili.md and CMakeLists.txt in the same directory is a strong signal that the directory contains a self-contained component
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
    - use `"quotes"` for includes within the project, `<angle brackets>` for 3rd party and platform dependencies
    - generally, the first non-comment line of any implementation file should be the header file with the same name
    - include project-local headers in this order: utilities, concepts, helpers, components (platform independent followed by platform specific), bindings (ditto), etc. (i.e. most general to most specific).
        - sort alphabetically within each package, e.g. sygaldry-concepts-metadata before sygaldry-concepts-mimu before sygaldry-concepts-runtime.
    - in CMakeLists:
        - `target_include_directories`, `target_sources`, `target_link_libraries`, and similar commands with more than on argument should have each argument on a line of its own, double indented wrt to command, and with the closing paren on its own line e.g.:
```cmake
target_sources(${lib}
        PRIVATE src1.cpp
        PRIVATE src2.cpp
        )
```
        - this provides the cleanest diffs (the last line never has to change simply because a new source had to be added alphabetically after it that stole the close paren), and we like how the close paren sticks out real obvious like so you are less likely to forget to put it
        - prefer sources, include directories, one each, then link libraries one for the component, one to link it to the package, in that order, not more, as necessary.

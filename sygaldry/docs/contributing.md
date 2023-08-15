\page docs-contributing Contributors' Guide

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This is currently a work in progress--not so much a guide as a disorganized
dumping ground where I note things that contributors should probably be aware
of, as well as a record of conventions I aim to follow myself.

# Licenses

Contributors must mark all non-trivial new documents added to the project with
an appropriate copyright notice, which should include the word "Copyright", the
current year, the name of the author, and the affiliations of the author who
may have a stake in the copyright of their contribution, as well as an SPDX
license identifier. Contributors are free to select a license according to
their preferences, legal obligations, and the requirements of the components
they contribute. The MIT license is recommended for new components without
copyleft dependencies, except for hardware components for which one of the CERN
open hardware licenses is advised. As well as marking the main literate source
file, every individual file tangled from it should also be marked with a
copyright and license notice. Exceptions may be made for trivial documents
to which copyright arguably does not apply, e.g. a single `add_subdirectory`
call in a `CMakeLists.txt` file.

If contributions are made to an existing file, the contributor should update
the list of authors and years accordingly, e.g. adding their name as an author
and adding the current year if no modifications have previously been made to
the file in the current year.

# Documentation

There are [four kinds of documentation](https://documentation.divio.com/), and
this project aims to provide them all. Documentation is extremely important.

Currently, tutorials and guides remain as future work. However, the literate
sources provide explanation, and the doxygen annotations provide reference.
Please include this level of documentation with anything you add to the project.

## Literate Programming

The literate sources should provide context for the design rationale and
implementation details of the source code, especially *why* a certain design
and implementation is used, any *alternatives* that may have been considered or
previously used, the *opinions* of the authors, and any other information about
the design and implementation that is not communicated by the source code
itself. This type of documentation is rarely provided; its inclusion is a
deliberate and important facet of the aims of the project.

## Doxygen

The doxygen annotations are meant to serve as a
[technical reference](https://documentation.divio.com/reference/). It should
describe the code as clearly and accurately as possible, avoiding "explanation,
discussion, instruction, speculation, and opinion," which belong in the
literate source, tutorials, and guides.

## Notations

When referring to the project in prose, use preformatted text, as in `sygaldry`.
Same when referring to the name of any software component, C++ entity, shell
command, directory, file, etc.

# Components and Physical Design Conventions

Components are identifiable functional modules that group together a
characteristic functionality or group of functionalities that the component
provides when used. Examples include sensors, protocol bindings, and sound
synthesis modules. Components are intended to be fairly modular and
independent, so that more complex functionalities can be built up by combining
several components together in an assembly.

Rules for component implementations:

- components meant to be used with bindings should strictly follow [the Sygaldry component structure](concepts/README.md).
    - input endpoints are declared in a simple aggregate member struct called `inputs`
    - ditto outputs
    - subcomponents are declared in a simple aggregate member struct called `parts`
    - components may declare an initialization subroutine `void init(...)`, and a main
      subroutine `void main(...)`
    - that's it! Bindings may reasonably ignore other members of the component, as should assemblies that use the component
        - in other words, everything else is conceptually private. It could even be declared as such, though it doesn't really matter.
- components that implement bindings should access bound components generically using the concepts library
    - i.e. components are considered as part of a seperate library about which we should try to avoid making assumptions
    - the assumptions that we do make are meant to be encapsulated and isolated in the concepts library, so that changes in those assumptions are insulated from the bindings library
- components that are platform independent (in the portable subgroup) should be declared in the package group namespace rather than a platform-specific namespace
    - e.g. `namespace sygaldry { namespace sensors {` rather than something like `namespace sygaldry { namespace sensors { namespace portable {`
    - this facilitates reuse of these components for specific platforms
- freely use the explicit API of other components within the same package group (e.g. sensors, bindings)
    - i.e. all components in a group are part of one library, and our assumptions
      about other components in the library should be enforced within it
    - components for one platform may not use components specific to another one, except the portable components, which may be used by all
- Read Lakos 2019
- Watch this talk: https://mropert.github.io/2017/10/14/modern_cmake_video/
    - Every (Lakosian) component should have:
        - a `cmake` library target (`add_library(fully-qualified-component_name, ...)`)
        - one header file (`target_include_directories(name PUBLIC .)`)
        - maybe one impl header file
        - probably just one implementation file (`target_sources(name PRIVATE name.cpp`)
        - a test executable that depends only on that component's interface and its transitive dependencies
            - `add_executable(name-test name.test.cpp)`
            - `target_link_libraries(name-test PRIVATE name)`
            - anything required for the platform-specific testing framework
    - component identifiers: `sygaldry-package-platform-component_name(.hpp|-impl.hpp|.cpp|.test.cpp)`
        - e.g. sygaldry-sensors-esp32-trill_craft.hpp
        - the name of a machine source files associated with a component should be its component identifier followed by the file extension
        - the component identifier (its fully-qualified name) should also be used as the `doxygen` page identifier in the literate source for that component (prefixed `page-`), and the `doxygen` documentation module group used with `\defgroup`
        - this is fairly verbose, but necessary to avoid naming conflicts with external projects etc.
        - it also allows us to avoid directory structures in include statements, which allows us to easily detect the use of a component just by a regex search
        - we avoid directory structures in include statements, since these enforce limits on how we can organize the library
            - `cmake` will take care of making sure the compiler gets all the -I flags it needs as long as we use `target_link_libraries` appropriately, and follow the physical design rules so that this is possible
    - every sygaldry component (regular or binding) should be a Lakosian software component
    - each component should live in its own directory, typically containing a subset of the following files:
        - `component.lili.md`
        - `doc/title of a tutorial.tutorial.md`
        - `doc/title of a guide.guide.md`
        - `sygaldry-package-component.meta.yaml`
        - `sygaldry-package-component.hpp`
        - `sygaldry-package-component.impl.hpp`
        - `sygaldry-package-component.cpp`
        - `sygaldry-package-component.test.cpp`
        - `CMakeLists.txt`
        - we are currently not strict about Lakos's policy that there must always be an `hpp` and a `cpp`.
            - the existence of a `CMakeLists.txt` in the same directory as an `hpp` is a strong enough signal that the directory contains a self-contained component
    - template compile time optimization
        - complex templates, especially bindings, should have separate interface (`.hpp`) and implementation (`.impl.hpp`)

# Coding Conventions

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

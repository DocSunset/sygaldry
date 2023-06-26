# Sygaldry

TODO: write a summary of the project

# The Sygaldry Library

## utilities

This library contains low level utilities used by all of the other libraries,
currently only the `_conseval` macro that allows to avoid incompatibility with
compilers that haven't yet correctly implemented `consteval`.

## concepts

This library contains C++20 concepts that aim to operationalize the high-level
conceptual framework that guides the design of the project, as well as generic
methods for reflecting over and accessing the data and functionality associated
a component adhering to this conceptual framework. The concepts library depends
on `boost::pfr`, `boost::mp11`, and the utilities library, but should have no
other dependencies (except for its tests, which regularly make use of the
helpers library). It is most useful to binding authors, but may also be helpful
for component authors who wish to make use of plugins and throughpoints
generically. This library should be platform independent. It is defined in
the namespace `sygaldry`.

## helpers

Whereas the concepts library defines generic tools for inspecting and using
components, the helpers library defines specific tools that facilitate
authoring components. The helpers library is intended to be compatible with the
concepts library, but without any physical (i.e. compiled) dependency on the
latter. The helpers library depends only on the utilities library (except for
its tests), and should be platform independent. It is also defined in the
namespace `sygaldry`; it is should be guaranteed that the concepts and helpers
libraries should not have any conflicting names, and if ones are encounter this
is a bug.

## components

The components library contains a collection of `sygaldry` components useful
for building digital musical instruments, implemented using the utilities,
concepts, and helpers libraries, and without any other dependencies (except for
its tests). The components library provides platform independent logical
components in the `sylgadry::components` namespace, including especially
cross-modal mapping, sensor conditioning, and gesture modelling components. In
addition, platform-specific components also are provided, each in their own
subdirectory and namespace, e.g. `sygaldry::components::esp32`, including
hardware-dependent components such as GPIO, ADC, serial interface, and other
peripheral drivers. A platform-independent component is allowed to directly
physically depend on other platform-independent components. A platform-specific
component is allowed to directly physically depend on other components for the
same platform, and on platform-independent components.

## bindings

The bindings library contains components that mainly reflect over other
components generically, providing functionality such as control protocol
bindings. It is defined in the namespace `sygaldry::bindings`. Binding
components are allowed to directly utilize the interface of other binding
components, but must generically other components, such as those in the
`sygaldry::components` library, using the generic methods defined in the
concepts library. The bindings library depends on the utilities, concepts, and
helpers library, and is not allowed to access the components library directly.
Like the component library, the bindings library provides both
platform-independent and platform-specific components in appropriately nested
namespaces.

# The Sygaldry Instruments

Taken together, the utilities, concepts, helpers, components, and bindings
libraries make up the `sygladry` library. The instruments library contains a
collection of digital musical instruments implemented using the `sygaldry`
library, and completing the `sygaldry` project.

# Other Directories

## Build Tools

The `sh` and `cmake` directories contain scripts used in the build process.
For more detail, refer to [the build system document](build-system.lili.md).

## Platform-Specific Tests

The `tests` directory contains projects for building platform-specific tests.
See [tests/README.md](tests/README.md) for more detail.

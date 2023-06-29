# Sygaldry

A conceptual framework and library for developing digital musical instruments
that are replicable, readable, and reliable, using C++20 reflection and
metaprogramming, a subcomponent-oriented development focus, and literate
programming.

[TOC]

# Quick Start

The Doxygen-generated documentation is not yet available online. In the
meantime, the reader is invited to check out this repository and run `doxygen`
in the root directory of the project, and then open the index file generated
by Doxygen at `_build_doxygen/html/index.html` before continuing. Github
and Doxygen have incompatible conventions for relative linking, and some
Doxygen-specific markup commands are used that will not render correctly
on Github, so reading the Doxygen-generated documentation is strongly
advised.

# Project Motivation

## Replicability and Longevity

Most DMIs that are developed fall into
disuse after a short amount of time \cite morreale2017nime_design-for-longevity. The poor longevity of DMIs
is an important subject for DMI research, and numerous approaches have been
proposed to enable long-lasting DMI design and use, from socio-ecologic
approaches \cite mcpherson2012cmj_problem-of-the-second-performer, to musical
\cite marquez-borbon2018nime_dmi-adoption-and-longevity and technical \cite zayas-garin2021nime_dmi-apprenticeship pedagogy, and implementation
strategies that can improve longevity \cite franco2017prynth.

A major challenge facing long-term DMI use is technical failure.
\cite morreale2017nime_design-for-longevity report that as many as 47% of
DMIs may fall into disuse due to the instrument becoming broken or inoperable,
such as due to software updates. \cite sullivan2021thesis found that
reliability was essential for long-term DMI use. For a DMI to sustain long-term
engagement, it must be maintained, updated, repaired, and eventually replaced
\cite calegario2021nime_replicability. As technology platforms inevitably
change and the tools and materials originally used to make a DMI become
obsolete or otherwise unavailable, i.e. as a DMI ages, a maintainer must
eventually replicate the design of the instrument using available tools and
materials \cite calegario2021nime_replicability. Recent calls have argued that
it is essential for the longevity of a DMI that its design be documented in
sufficient detail to enable replication \cite calegario2021nime_replicability.

## Portability and Reuse:

DMIs are almost always (e.g. 73% of DMIs
reported at NIME from 2020 to 2022) implemented using an embedded hardware
processor such as a microcontroller unit (MCU) or single-board computer (SBC),
often (40\% ibid) in combination with another more powerful computer such as a
laptop. Many different languages and protocols
are employed, often simultaneously.

The combination of hardware devices, programming languages, and communication
protocols makes the code that implements a DMI generally incompatible with a
different combination of platforms. This can make it difficult to reuse code
(whether one's own, or that of a third party) when developing a new DMI,
leading to wasteful repetition of effort. As design requirements change or a
DMI ages, code must inevitably be ported to new platforms and environments, and
issues of portability and code reuse become issues of maintenance,
replicability, and reliability, further harming instruments' longevity.

Current efforts to improve the portability and reuse of DMI software components
are largely restricted to sound signal processing (e.g. Faust) and are
generally not applicable to the development of DMI components such as sensor
signal conditioning or mapping. Other approaches to portability often impose
significant runtime performance impacts, safety issues, and can eventually
become yet another platform to target for portable implementations
\cite celerier2022icmc_rage-against-the-glue.

## Specificity and Generalisation

The possibility of scientifically
advancing DMI design through evaluation of DMIs has recently been called into
question \cite goudard2019nime_ephemeral-instruments \cite rodger2020nime_what-good.
The assemblage of DMI components makes each DMI a unique system
\cite goudard2019nime_ephemeral-instruments. The further embedding of
these systems in co-constitutive socio-musical ecologies makes study and
evaluation of DMIs deeply specific \cite rodger2020nime_what-good. Coupled
with a general lack of consensus about how to evaluate DMIs
\cite barbosa2015evaluation, this makes it extremely difficult to derive
generalisable insight from evaluations of DMIs, and poses severe challenges for
the scientific study and advancement of DMI design.

# Proposed Solution

Sygaldry aims to explore a framework that addresses the replicability,
portability, and generalisable study of DMIs by employing three interlocking
development techniques: reflection and metaprogramming in C++
\cite celerier2022icmc_rage-against-the-glue; a paradigmatic approach that
gives a strong emphasis to constructing reusable components primarily, with
musical instruments emerging as a side-effect; and literate programming
\cite knuth1984literateprogramming.

Reflection in C++ enables software components to be implemented in
dependency-free C++ and then deployed to numerous heterogeneous computing
environments, such as MCUs, SBCs, and host languages like Max/MSP and Pure
Data. The approach proposed by Celerier
\cite celerier2022icmc_rage-against-the-glue reduces the code-size complexity
of implementing portable media processors, dependent on the number of components $N$ and
target environments $M$, from quadratic $N * M$ to linear $N + M$. Originally
proposed for binding media processing algorithms to plugin formats and software
host environments, Sygaldry applies Celerier's approach to DMI development and
maintenance.

Whereas whole DMIs are highly specific, DMI components such as sensors and
mapping strategies are regularly found in many different DMIs, and can be
assembled \cite goudard2019nime_ephemeral-instruments to make new
DMIs. By focusing development and evaluation on DMI components, we
should be able to build a library of reusable parts that can be leveraged by
many designers. Focusing on modular components should also enable automated
performance and correctness testing, improving reliability. Evaluations facing
components should have better generality, and allow insights uncovered to be
applied to any instrument that leverages the evaluated component. Using
C++, especially with the techniques just described, provides the best possible code
portability and reusability of these components for the least amount of effort,
allowing the components developed to eventually be leveraged in a wide variety of
programming languages, hardware platforms, and other runtime environments.

Modularity, reliability and portability, however, are insufficient qualities to
enable reuse and replication. Literate programming
\cite knuth1984literateprogramming advocates for computer code to be written
with a human reader in mind. This practice improves the readability and
understandability of source code, allowing low level details and design praxis
to be clearly documented as part of the process of implementation. By acting as a kind of textual apprenticeship
\cite zayas-garin2021nime_dmi-apprenticeship literate source code is hoped
facilitate the transmission of research and design products that are often not reported
and provide the necessary comprehension to encourage future maintainers and
other researchers to adopt the reusable components thus documented, rather
than starting over from scratch as is so often done.

# Library Overview

Sygaldry consists of five main parts: the concepts and helpers libraries, the
components and bindings libraries, and the instruments collection.

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

## The Sygaldry Instruments

Taken together, the utilities, concepts, helpers, components, and bindings
libraries make up the `sygladry` library. The instruments library contains a
collection of digital musical instruments implemented using the `sygaldry`
library, and completing the `sygaldry` project.

## Other Directories

When viewing the source code of the project, the above libraries account
for most of the top level directories. The rest are briefly described below.

### utilities

This library contains low level utilities used by all of the other libraries,
currently only the `_conseval` macro that allows to avoid incompatibility with
compilers that haven't yet correctly implemented `consteval`.

### Build Tools

The `sh` and `cmake` directories contain scripts used in the build process.
For more detail, refer to [the build system document](build-system.lili.md).

### Platform-Specific Tests

The `tests` directory contains projects for building platform-specific tests.
See [tests/README.md](tests/README.md) for more detail.

### dependencies

This directory contains 3rd party submodules used by other components,
including the `boost` libraries required for reflection and metaprogramming,
and libraries meeting other platform- or component-specific requirements.

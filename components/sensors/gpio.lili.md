# General-Purpose Input/Output

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

Embedded systems such as microcontrollers and system-on-chip devices commonly
have user-programmable hardware pins (i.e. points on the exterior of the
integrated circuit where electrical connections can be made) that are used for
reading general digital signals, and outputting general digital signals. As
they are general in purpose, and used for both input and output, they are
descriptively called *general-purpose input/output pins*, most often
abbreviated or simply *GPIO*.

The capabilities and method of interacting with GPIO vary substantially across
different platforms. Furthermore, the ways in which GPIO may be employed vary
substantially between applications, and in general, it would be inappropriate
to expose low-level control over a raw GPIO to bindings. For these reasons, a
high-level component for GPIO is not provided. Platform-specific components may
be found in each platform's respective directory.

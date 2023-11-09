\page page-sygbp-basic_reader sygbp-basic_reader: Basic Reader

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

# Motivation

As outlined in [the CLI binding](\ref page-sygbp-cli), some bindings may require
to accept textual input, such as from a serial UART device or the
standard input given by an operating system. As with
[the logging plugin](\ref page-sygup-basic_logger)
we would like to provide a plugin type that compenents can use
easily, and that binding authors can easily port to different
environments. The plugin design described here borrows much
of its design thinking from the basic logger, which was implemented
first.

# Design Thinking

It is simplest for the plugin client if it is in control of the thread of
execution, which means that we can rule out a push-based strategy where the
reader calls on the client when input is available, and we're instead required
to employ a pull-based approach where the plugin client requests the next
available character or characters when it needs them.

Given the pull-based approach, the plugin has to decide what to do about
the possibility that new input may not be available when the client is ready
for it. There are essentially two options: blocking and non-blocking. We're
concerned with retaining deterministic timing behaviors, so that rules out a
blocking approach where the plugin stalls execution until input becomes
available inf situations where a request for input is made before input is
available. Instead, we will adopt a non-blocking approach.

Given the non-blocking approach, the plugin has to decide how to signal that
input is not available. A strategy using `std::optional` is certainly possible,
e.g.:

```cpp
while(reader().and_then([&](auto& char_in) {process(char_in);})) {}
```

But we opt for a more idiomatically C++ flavoured strategy where the client
checks if input is available, and then proceed to consume it so long as it
remains available, something like the following:

```cpp
while(reader.ready()) process(reader.getchar());
```

Technically we could also consider employing an approach using callbacks, where
the plugin client registers a function that should be called when input becomes
available, but this would be somewhat more complex to implement and use, so we
stick with a simpler non-blocking approach. This gives us the following plan:

```cpp
#pragma once

namespace sygaldry { namespace bindings {

struct BasicReader
{
    [[no_unique_address]] getter get;

    bool ready()
    {
        // check if input is available
    }

    char getchar()
    {
        // return one character of available input
    }
};

} }
```

Basic readers are implemented for [testing](\ref sygbp-test_reader) and for
platforms with [cstdio support](\ref sygbp-cstdio_reader) in seperate
documents.

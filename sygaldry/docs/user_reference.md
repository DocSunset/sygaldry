# User Reference Overview

TODO

For now, just read the hpp files of the things you need to use. A user of the
library making a new instrument should only need to examine the headers for
components and bindings required by the design, as well as the `Runtime` class.
It may not even be necessary to examine the headers; start by checking out
other instruments. A user making a new component may want to examine
`helpers/metadata.hpp` and `helpers/endpoints.hpp`, as well as studying other
components. A user making a new binding should study the `concepts` library,
especially the endpoints and components concepts, and examine some other
bindings.

# I2C

For I2C, we simply wrap the `WireHack` class from the `Trill-ESP-IDF` library.

```cpp
// @#'i2c.hpp'
#include "Wire.h"
#include "helpers/metadata.hpp"

namespace sygaldry { namespace components { namespace esp32 {

struct I2C
{
};

} } }
// @/
```

\page page-sygsa-icm20948 ICM20948

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

# Serial Interface

The ICM20948 is interacted with via its control registers--essentially a bank
of memory-mapped switches accessed over either I2C or SPI. Most interactions
will be to read the data output registers; besides this, almost all
interactions will be to read and/or write a single byte to/from a single
register. To allow a greater number of registers to be addressed, the device
also has four "banks" of registers that are switched between by writing to a
particular register that is always at the same address no matter which bank is
currently enabled.

In line with this operational structure, we wish to define an interface that is
independent of the particular serial bus used (whether I2C or SPI), as well as
the specificies of that bus (such as the I2C address or SPI CS pin). We
presently accomplish this by making the top level ICM20948 class a template
that accepts a `Serif` type that is expected to define methods `read` and
`write` for single byte interaction with a particular register at a given
address, and an overload of `read` taking an `uint8_t` array and size for
reading multiple sequential bytes.

# Arduino TwoWire Serial Interface

We declare a realization of this interface for the Arduino `TwoWire` API. As
well as a straightforward `struct` that requires the I2C address to be passed,
we define *a very simple template* that passes this information as a compile-time
non-type template parameter. We declare *explicit external instantiations* of
this template for the two default I2C addresses and compile them in the
implementation file for the `Serif`.

```cpp
// @#'sygsa-icm20948-two_wire_serif.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>

namespace sygaldry { namespace sygsa {

namespace detail {
struct ICM20948TwoWireSerif
{
    [[nodiscard]] static uint8_t read(uint8_t i2c_address, uint8_t register_address);
    static uint8_t read(uint8_t i2c_address, uint8_t register_address, uint8_t * buffer, uint8_t bytes);
    static void write(uint8_t i2c_address, uint8_t register_address, uint8_t value);
};
}

// *a very simple template*
/*! Serial interface for ICM20948 using the Arduino TwoWire API

\tparam i2c_address The I2C address of the MIMU device
*/
template<uint8_t i2c_address>
struct ICM20948TwoWireSerif
{
    /// Read one byte and return it
    [[nodiscard]] static uint8_t read(uint8_t register_address)
    {
        return detail::ICM20948TwoWireSerif::read(i2c_address, register_address);
    }

    /// Read many bytes; returns the number of bytes read
    static uint8_t read(uint8_t register_address, uint8_t * buffer, uint8_t bytes)
    {
        return detail::ICM20948TwoWireSerif::read(i2c_address, register_address, buffer, bytes);
    }

    /// Write one byte
    static void write(uint8_t register_address, uint8_t value)
    {
        detail::ICM20948TwoWireSerif::write(i2c_address, register_address, value);
    }
};

// *explicit external instantiations*
extern template struct ICM20948TwoWireSerif<0b1101000>;
extern template struct ICM20948TwoWireSerif<0b1101001>;

} }
// @/
```

The definition of these functions involves very typical use of the `TwoWire` API.
We define the single byte read in terms of the single byte write.

```cpp
// @#'sygsa-icm20948-two_wire_serif.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsa-icm20948-two_wire_serif.hpp"
#include <Wire.h>

namespace sygaldry { namespace sygsa {

namespace detail {
uint8_t ICM20948TwoWireSerif::read(uint8_t i2c_address, uint8_t register_address)
{
    uint8_t out = 0;
    read(i2c_address, register_address, &out, 1);
    return out;
}

uint8_t ICM20948TwoWireSerif::read(uint8_t i2c_address, uint8_t register_address, uint8_t * buffer, uint8_t bytes)
{
    @{two wire serif read many}
    Wire.beginTransmission(i2c_address);
    Wire.write(register_address);
    Wire.endTransmission(false); // repeated start
    Wire.requestFrom(i2c_address, 1);
    for (uint8_t i = 0; i < bytes; ++i)
    {
        if (Wire.available()) buffer[i] = Wire.read();
        else return i;
    }
    return bytes;
}

void ICM20948TwoWireSerif::write(uint8_t i2c_address, uint8_t register_address, uint8_t value)
{
    Wire.beginTransmission(i2c_address);
    Wire.write(register_address);
    Wire.write(value);
    Wire.endTransmission();
}
}

extern template struct ICM20948TwoWireSerif<0b1101000>;
extern template struct ICM20948TwoWireSerif<0b1101001>;

} }
// @/
```

```cmake
# @+'cmake snippets'
target_sources(${lib}
        PRIVATE ${lib}-two_wire_serif.cpp
        )
target_link_libraries(${lib}
        PUBLIC sygsp-arduino_hack
        )
# @/
```

## Basic Sanity Check Test

Before proceeding with the rest of the implementation, we consider
a couple of very simple tests to ensure that the above serial interface
appears to be working.

In the first test, we simply read the `WHO_AM_I` register of the ICM20948
and confirm that it has the expected value. This test confirms that
the ICM20948 is connected and the serial interface `read` functions seem
to be implemented correctly.

```cpp
// @+'esp32 tests includes'
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// @/

// @+'esp32 tests'
{
    printf("icm20948-test: read test... ");
    constexpr uint8_t who_am_i_address = 0;
    constexpr uint8_t who_am_i_value = 0xea;
    uint8_t ret = Serif::read(who_am_i_address);
    if (ret == who_am_i_value) printf("passed!\n");
    else
    {
        printf("unexpected who am I value?!\n");
        return;
    }
}
// @/
```

In the second test, we reset SRAM, then read the `USER_CTRL` register to
confirm that it has the expected value following a reset. We then toggle a
switch in the `USER_CTRL` register, confirm that the change was recorded, then
finally reset SRAM again. This test confirms that the `write` function seems to
be implemented correctly.

```cpp
// @+'esp32 tests'
{
    printf("icm20948-test: write test...");

    constexpr uint8_t pwr_mgmt_1_address = 6;
    constexpr uint8_t expected_value_after_reset = 0x41;
    constexpr uint8_t wake_up_value = 0x01;
    constexpr uint8_t reset_trigger_value = 0x81;

    // Reset device
    printf(".");
    Serif::write(pwr_mgmt_1_address, reset_trigger_value);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Read USER_CTRL register after reset
    uint8_t value_after_reset = Serif::read(pwr_mgmt_1_address);
    if (value_after_reset != expected_value_after_reset)
    {
        printf(" unexpected value after reset?!\n");
        return;
    }

    // Toggle a switch in USER_CTRL register
    printf(".");
    Serif::write(pwr_mgmt_1_address, wake_up_value);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Confirm the change was recorded
    uint8_t value_after_wake_up = Serif::read(pwr_mgmt_1_address);
    if (value_after_wake_up != wake_up_value)
    {
        printf(" write operation unsuccessful?!\n");
        return;
    }

    // Reset device
    printf(".");
    Serif::write(pwr_mgmt_1_address, reset_trigger_value);
    vTaskDelay(pdMS_TO_TICKS(10));

    printf(" passed!\n");
}
// @/
```

# Registers

Now that we have a way to read and write the control registers, most of what
remains is to simply define constant values for the addresses, allowed values,
bit masks, etc. needed to actually manipulate the registers. All of this data
is contained in the datasheet and need to be rendered in source code form. This
is commonly accomplished using plain C style enumerations, preprocessor
definitions, and simple structs, but these methods don't provide much
compile-time safety, and generally don't take advantage of the affordances of
C++ to help us avoid errors, so we wish to employ a more elaborate approach
that can make better use of the language.

Our first thought is to transcribe the datasheet so that each register
description becomes a struct that contains all the relevant data; this can
potentially make it impossible to use e.g. a bit mask meant for one register
with another one, e.g.:

```cpp
struct USER_CTRL
{
    static constexpr uint8_t address = 0x03;
    static constexpr uint8_t bank = 0;
    static constexpr bool read = true;
    static constexpr bool write = true;
    static constexpr uint8_t reset_value = 0x00;

    struct DMP_EN
    {
        static constexpr uint8_t mask = 1 << 7;
        static constexpr uint8_t enable = mask;
        static constexpr uint8_t disable = 0;
    };

    struct FIFO_EN
    {
        static constexpr uint8_t mask = 1 << 6;
        static constexpr uint8_t enable = mask;
        static constexpr uint8_t disable = 0;
    };

    struct I2C_MST_EN
    {
        static constexpr uint8_t mask = 1 << 5;
        static constexpr uint8_t enable = mask;
        static constexpr uint8_t disable = 0;
    };

    struct I2C_IF_DIS
    {
        static constexpr uint8_t mask = 1 << 4;
        static constexpr uint8_t reset = mask;
    };

    struct DMP_RST
    {
        static constexpr uint8_t mask = 1 << 3;
        static constexpr uint8_t reset = mask;
    };

    struct SRAM_RST
    {
        static constexpr uint8_t mask = 1 << 2;
        static constexpr uint8_t reset = mask;
    };

    struct I2C_MST_RST
    {
        static constexpr uint8_t mask = 1 << 1;
        static constexpr uint8_t reset = mask;
    };
};
```

We want to declare a function that will let us set one bit field atomically
given only the bit field as an argument; we want to make C++ do the tedious
work of associating that bit field value with the relevant register address,
bank, and bit mask, which shouldn't be too difficult if we structure our
transcription of the datasheet appropriately.

My first thought is to try to define something like this:

```cpp
template<uint8_t bitmask_value>
void read_modify_write()
{
    /* get the address etc. and do the thing */
}
```

On reflection, it's clear that there will be no way to access e.g. `USER_CTRL`
from an unsigned integer value such as `USER_CTRL::SRAM_RST::reset`, as
structured above. My next thought is to make `SRAM_RST::reset` a type that has
a `using` directive that points back to the register, e.g.:

```cpp
struct USER_CTRL
{
    static constexpr uint8_t address = 0x03;
    static constexpr uint8_t bank = 0;
    static constexpr bool read = true;
    static constexpr bool write = true;
    static constexpr uint8_t reset_value = 0x00;
};

struct DMP_EN
{
    using REGISTER = USER_CTRL;
    static constexpr uint8_t mask = 1 << 7;
};

struct DMP_EN_enable
{
    using BitField = DMP_EN;
    static constexpr uint8_t value = mask;
};

struct DMP_EN_disable
{
    using BitField = DMP_EN;
    static constexpr uint8_t value = 0;
};

// etc.

template<typename BitFieldValue>
void read_modify_write()
{
    serif_write(BankRegister::Address, BitFieldValue::BitField::Register::bank);
    uint8_t r = serif_read(BitFieldValue::BitField::Register::address);
    uint8_t m = (r & ~BitFieldValue::BitField::mask) | (BitFieldValue::value & BitFieldValue::BitField::mask);
    serif_write(BitFieldValue::BitField::Register::address, m);
}
```

This turns out to be cumbersome for several reasons. Accessing the values,
such as `address`, requires manually traversing the links implemented by the
using directives. This also involves writing a lot of `static constexpr`.

Another possibility is to use inheritance to propagate and protect
register-level information when moving down to bit-fields, instead of trying to
use nested scopes or using directives. This might reduce the amount of writing
we have to do, make the implementation of `read_modify_write` simpler, and
it may also allow us to propagate functionality through inheritance, which
might be convenient.

So we rewrite the above listing such that `USER_CTRL` is struct of type
`Register` that only defines its address, bank, etc., and then each bit field
is an independent struct that inherits these values from the register type and
adds a static member for its bit mask, and each possible bit field state, e.g.
`enabled` or `disabled` for `I2C_MST_EN` is another independent type that
inherits from the bit field and declares an API for manipulating that bit field
in a certain way.

This seems like a nice approach, and we can use templates to further reduce the
amount of writing we have to do, saving ourselves from having to write `static
constexpr` so many times:

```cpp
// @='base classes'
/*! The currently selected user bank;

Defaults to an invalid bank so that the first interaction with a device is
always on a known bank.
*/
static uint8_t current_bank_;

/// Switch to a particular user bank, unless it is already selected
template<uint8_t bank>
static void select_bank_()
{
    static_assert(0 <= bank && bank <= 3);
    if (bank != current_bank_) Serif::write(127, bank << 4);
}

/*! Base class for registers

The template parameters are stored in similarly-named `static constexpr`
variables that encode the available information about a given control register.

\tparam addr_ The address of the register in the ICM20948 memory space
\tparam bank_ The user bank in which the register is located (0, 1, 2, or 3)
\tparam reset_ The value of the register after a device reset (e.g. at boot)
*/
template <uint8_t addr_, uint8_t bank_, uint8_t reset_>
struct Register
{
    static constexpr uint8_t address = addr_;
    static constexpr uint8_t bank = bank_;
    static constexpr uint8_t after_reset = reset_;

    static void select_bank()
    {
        select_bank_<bank>();
    }

    [[nodiscard]] static uint8_t read()
    {
        select_bank();
        return Serif::read(address);
    }

    static void write(uint8_t value)
    {
        select_bank();
        Serif::write(address, value);
    }
};

/// Switch to a bank, read the current state of a register, clear the masked bits of a field, set those bits according to the value
template<typename RegisterField, uint8_t value>
static void read_modify_write()
{
    static_assert(RegisterField::read && RegisterField::write);
    RegisterField::select_bank();
    uint8_t read   = RegisterField::read();
    uint8_t modify = (read & ~RegisterField::mask) | (value & RegisterField::mask);
    /*      write */ RegisterField::write(modify);
}


/*! Base class for bit fields

Every register has numerous bit fields comprising one or more bits.
This base class stores the bit mask with the bit or bits concerned
by the given field set, so that these bits can be cleared from an
existing state of the register by `(reg & ~mask)` or the state of
only these bits can be extracted by `(reg & mask)`.

\tparam Register register for which this bit field applies
\tparam mask_ The bitmask, saved in the `static constexpr uint8_t` member `mask`.
*/
template<typename Register, uint8_t mask_>
struct BitField : Register
{
    static constexpr uint8_t mask = mask_;
    [[nodiscard]] static uint8_t read_field()
    {
        return mask & Register::read();
    }
};

/*! Base class for bit field states with no archetypical semantic e.g. where
there are several allowed states.

\tparam BitField The struct defining the register and bit field for which this state is valid.
\tparam value The value for this allowed state of the field.
*/
template<typename BitField, uint8_t value>
struct BitFieldState : BitField
{
    static void set() { read_modify_write<BitFieldState<BitField, value>, value>(); }
};

/*! Base class for single-bit fields where a set bit triggers an immediate
action and an unset bit has no particular meaning or influence.

\tparam Register The struct defining the register (address, bank, etc.) for this bit field.
\tparam mask The mask for the single bit, which is also used to set the bit.
*/
template<typename Register, uint8_t mask>
struct BitTrigger : BitField<Register, mask>
{
    static void trigger()
    {
        static_assert(std::has_single_bit(mask));
        read_modify_write<BitTrigger<Register, mask>, mask>();
    }
};

/*! Base class for single-bit fields where a set bit indicates an enabled (or
disabled) state and an unset bit indicates a disabled (or enabled) state.

The default assumption is that a set bit indicates an enabled state.

\tparam Register The struct defining the register (address, bank, etc.) for this bit field.
\tparam mask The mask for the single bit, which is also used to set the bit.
\tparam invert Flip the default assumption, so that a set bit indicates a disabled state.
*/
template<typename Register, uint8_t mask, bool invert = false>
struct BitSwitch : BitField<Register, mask>
{
    using This = BitSwitch<Register, mask, invert>;
    static void enable()
    {
        static_assert(std::has_single_bit(mask));
        if constexpr (invert)
            read_modify_write<This, 0>();
        else
            read_modify_write<This, mask>();
    }
    static void disable()
    {
        static_assert(std::has_single_bit(mask));
        if constexpr (invert)
            read_modify_write<This, mask>();
        else
            read_modify_write<This, 0>();
    }
};
// @/
```

Then we can transcribe a few registers from the datasheet like this:

```cpp
// @+'registers'
struct WHO_AM_I : Register<0x00, 0, 0xEA> {};
// read only, no bit fields

struct USER_CTRL   : Register<0x03, 0, 0x00>
{
    struct DMP_EN      :  BitSwitch<USER_CTRL, 1 << 7> {};
    struct FIFO_EN     :  BitSwitch<USER_CTRL, 1 << 6> {};
    struct I2C_MST_EN  :  BitSwitch<USER_CTRL, 1 << 5> {};
    struct I2C_IF_DIS  : BitTrigger<USER_CTRL, 1 << 4> {};
    struct DMP_RST     : BitTrigger<USER_CTRL, 1 << 3> {};
    struct SRAM_RST    : BitTrigger<USER_CTRL, 1 << 2> {};
    struct I2C_MST_RST : BitTrigger<USER_CTRL, 1 << 1> {};
};

struct PWR_MGMT_1   : Register<0x06, 0, 0x41>
{
    struct DEVICE_RESET : BitTrigger<PWR_MGMT_1, 1 << 7> {};
    struct SLEEP        :  BitSwitch<PWR_MGMT_1, 1 << 6> {};
    struct LP_EN        :  BitSwitch<PWR_MGMT_1, 1 << 5> {};
    struct TEMP_DIS     :  BitSwitch<PWR_MGMT_1, 1 << 3> {};
    struct CLKSEL       :   BitField<PWR_MGMT_1, 0b111>
    {
        struct InternalOscillator : BitFieldState<CLKSEL, 0> {};
        struct AutoSelect         : BitFieldState<CLKSEL, 1> {};
        struct Stop               : BitFieldState<CLKSEL, 7> {};
    };
};
// @/
```

And use them like this:

```cpp
// @+'esp32 tests'
{
    printf("icm20948-test: register bases test...");

    if (Registers::WHO_AM_I::read() != Registers::WHO_AM_I::after_reset)
    {
        printf("unexpected who am I value?!\n");
        return;
    }

    printf(".");
    Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
    vTaskDelay(pdMS_TO_TICKS(10));

    if (Registers::PWR_MGMT_1::read() != Registers::PWR_MGMT_1::after_reset)
    {
        printf(" unexpected value after reset?!\n");
        return;
    }

    if (Registers::PWR_MGMT_1::read() != Registers::PWR_MGMT_1::after_reset)
    {
        printf(" unexpected value after reset?!\n");
        return;
    }

    printf(".");
    Registers::PWR_MGMT_1::SLEEP::disable();
    vTaskDelay(pdMS_TO_TICKS(10));

    // Confirm the change was recorded
    uint8_t value_after_wake_up = Registers::PWR_MGMT_1::read();
    uint8_t expected = Registers::PWR_MGMT_1::after_reset & ~Registers::PWR_MGMT_1::SLEEP::mask;
    if (value_after_wake_up != expected)
    {
        printf(" write operation unsuccessful?!\n");
        return;
    }

    printf(".");
    Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
    vTaskDelay(pdMS_TO_TICKS(10));

    printf(" passed!\n");
}
// @/
```

The description is highly declarative, with each bit field declared in terms of
how it should be interpreted. There is relatively little repetition; we
tolerate the repeated register type since we can highlight the repetition
through formatting so that it's easier to catch typos and otherwise ignore the
repetition. We also generate an imperative API for interacting with the
registers as, it feels like, a side effect of declaring their semantics. Very
nice!

Here is the complete register description header. Many registers are not yet
transcribed from the datasheet, as they aren't in use in the current version of
the driver.

```cpp
// @+'registers'
struct LP_CONFIG     : Register<0x05, 0, 0x40>
{
    struct I2C_MST_CYCLE : BitSwitch<LP_CONFIG, 1 << 6> {};
    struct ACCEL_CYCLE   : BitSwitch<LP_CONFIG, 1 << 5> {};
    struct GYRO_CYCLE    : BitSwitch<LP_CONFIG, 1 << 4> {};
};
// @/
```

```cpp
// @#'sygsp-icm20948_registers.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include <bit>

namespace sygaldry { namespace sygsp {

template<typename Serif>
struct ICM20948Registers
{

@{base classes}

@{registers}

};

template<typename Serif>
uint8_t ICM20948Registers<Serif>::current_bank_ = 0xFF;

} }
// @/

```

# ESP32 Test Library

This component was originally implemented targeting the ESP32 platform using
the `TwoWire` API implemented in \ref sygse-arduino_hack. At that time, the
following test library was developed to facilitate test driven development.
Several tests are filled in above.

```cpp
// @#'sygse-icm20948-tests.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>
#include <cstdio>
#include "sygsa-icm20948-two_wire_serif.hpp"
#include "sygsp-icm20948_registers.hpp"
@{esp32 tests includes}

namespace sygaldry { namespace sygse {

template<uint8_t i2c_address>
struct ICM20948Tests
{
    using Serif =     sygsa::ICM20948TwoWireSerif<i2c_address>;
    using Registers = sygsp::ICM20948Registers<Serif>;
    static void test()
    {
        @{esp32 tests}
    }
};

} }
// @/
```

# Build

The various sub-components are collected together into one CMake library.

```cmake
# @#'CMakeLists.txt'
set(lib sygsa-icm20948)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
@{cmake snippets}
# @/
```
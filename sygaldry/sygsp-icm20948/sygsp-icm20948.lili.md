\page page-sygsp-icm20948 sygsp-icm20948: ICM20948 MIMU Driver

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

# Serial Interface

The ICM20948 is interacted with via its control registers according to the
concept of [a byte-wise serial interface](\ref page-sygsp-byte_serif) described
elsewhere.

Before proceeding with the rest of the implementation, we consider
a couple of very simple tests to ensure that the above serial interface
appears to be working.

In the first test, we simply read the `WHO_AM_I` register of the ICM20948
and confirm that it has the expected value. This test confirms that
the ICM20948 is connected and the serial interface `read` functions seem
to be implemented correctly.

```cpp
// @+'tests'
{
    printf("icm20948-test: read test... ");
    constexpr uint8_t who_am_i_address = 0;
    constexpr uint8_t who_am_i_value = 0xea;
    uint8_t ret = Serif::read(who_am_i_address);
    if (ret == who_am_i_value) printf("passed!\n");
    else
    {
        printf("unexpected who am I value %d?!\n", ret);
        return false;
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
// @+'tests'
{
    printf("icm20948-test: write test...");

    constexpr uint8_t pwr_mgmt_1_address = 6;
    constexpr uint8_t expected_value_after_reset = 0x41;
    constexpr uint8_t wake_up_value = 0x01;
    constexpr uint8_t reset_trigger_value = 0x81;

    // Reset device
    Serif::write(pwr_mgmt_1_address, reset_trigger_value);
    delay(10);

    // Read USER_CTRL register after reset
    uint8_t value_after_reset = Serif::read(pwr_mgmt_1_address);
    if (value_after_reset != expected_value_after_reset)
    {
        printf(" unexpected value after reset?!\n");
        return false;
    }

    // Toggle a switch in USER_CTRL register
    printf(".");
    Serif::write(pwr_mgmt_1_address, wake_up_value);
    delay(10);

    // Confirm the change was recorded
    uint8_t value_after_wake_up = Serif::read(pwr_mgmt_1_address);
    if (value_after_wake_up != wake_up_value)
    {
        printf(" write operation unsuccessful?!\n");
        return false;
    }

    // Reset device
    printf(".");
    Serif::write(pwr_mgmt_1_address, reset_trigger_value);
    delay(10);

    printf(" passed!\n");
}
// @/
```

# Registers

## Design

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

## Base Classes

So we rewrite the above listing such that `USER_CTRL` is struct of type
`Register` that only defines its address, bank, etc., and then each bit field
is an independent struct that inherits these values from the register type and
adds a static member for its bit mask, and each possible bit field state, e.g.
`enabled` or `disabled` for `I2C_MST_EN` is another independent type that
inherits from the bit field and declares an API for manipulating that bit field
in a certain way.

This seems like a nice approach, and we can use templates to further reduce the
amount of writing we have to do, saving ourselves from having to write `static
constexpr` so many times.

We realize this approach with the following set of base class templates:

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
    if (bank != current_bank_)
    {
        Serif::write(127, bank << 4);
        current_bank_ = bank;
    }
}

/*! Base class for registers

The template parameters are stored in similarly-named `static constexpr`
variables that encode the available information about a given control register.

\tparam addr_ The address of the register in the ICM20948 memory space
\tparam bank_ The user bank in which the register is located (0, 1, 2, or 3)
\tparam reset_ The value of the register after a device reset (e.g. at boot)
*/
template <string_literal name, uint8_t addr_, uint8_t bank_, uint8_t reset_>
struct Register
{
    static constexpr uint8_t address = addr_;
    static constexpr uint8_t bank = bank_;
    static constexpr uint8_t after_reset = reset_;
    static constexpr const char * register_name() {return name.value;}

    static void select_bank()
    {
        select_bank_<bank>();
    }

    [[nodiscard]] static uint8_t read()
    {
        select_bank();
        //printf("sygsp-icm20948_registers: %s (%x) read\n", register_name(), address);
        return Serif::read(address);
    }

    static void write(uint8_t value)
    {
        select_bank();
        //printf("sygsp-icm20948_registers: %s (%x) write %x\n", register_name(), address, value);
        Serif::write(address, value);
    }
};

/// read the current state of a register, clear the masked bits of a field, set those bits according to the value
template<typename RegisterField, uint8_t value>
static void read_modify_write()
{
    //printf("sygsp-icm20948_registers: %s::%s (%x::%x) rmw\n", RegisterField::register_name(), RegisterField::field_name(), RegisterField::address, RegisterField::mask);
    static_assert(RegisterField::read && RegisterField::write);
    uint8_t read   = RegisterField::read();
    uint8_t modify = (read & ~RegisterField::mask) | (value & RegisterField::mask);
    /*      write */ RegisterField::write(modify);
    //printf("sygsp-icm20948_registers: %s::%s %#04x -> %#04x\n", RegisterField::register_name(), RegisterField::field_name(), read, modify);
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
template<string_literal name, typename Register, uint8_t mask_>
struct BitField : Register
{
    static constexpr uint8_t mask = mask_;
    static constexpr const char * field_name() {return name.value;}
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
template<string_literal name, typename BitField, uint8_t value>
struct BitFieldState : BitField
{
    using This = BitFieldState<name, BitField, value>;
    static constexpr const char * state_name() {return name.value;}
    static void set() { read_modify_write<This, value>(); }
};

/*! Base class for single-bit fields where a set bit triggers an immediate
action and an unset bit has no particular meaning or influence.

\tparam Register The struct defining the register (address, bank, etc.) for this bit field.
\tparam mask The mask for the single bit, which is also used to set the bit.
*/
template<string_literal name, typename Register, uint8_t mask>
struct BitTrigger : BitField<name, Register, mask>
{
    using This = BitTrigger<name, Register, mask>;
    static void trigger()
    {
        static_assert(std::has_single_bit(mask));
        read_modify_write<This, mask>();
    }
};

/*! Base class for single-bit fields where a set bit indicates an enabled (or
disabled) state and an unset bit indicates a disabled (or enabled) state.

The default assumption is that a set bit indicates an enabled state.

\tparam Register The struct defining the register (address, bank, etc.) for this bit field.
\tparam mask The mask for the single bit, which is also used to set the bit.
\tparam invert Flip the default assumption, so that a set bit indicates a disabled state.
*/
template<string_literal name, typename Register, uint8_t mask, bool invert = false>
struct BitSwitch : BitField<name, Register, mask>
{
    using This = BitSwitch<name, Register, mask, invert>;
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
struct WHO_AM_I : Register<"WHO_AM_I", 0x00, 0, 0xEA> {};
// read only, no bit fields

struct USER_CTRL   : Register<"USER_CTRL", 0x03, 0, 0x00>
{
    struct DMP_EN      :  BitSwitch<"DMP_EN",      USER_CTRL, 1 << 7> {};
    struct FIFO_EN     :  BitSwitch<"FIFO_EN",     USER_CTRL, 1 << 6> {};
    struct I2C_MST_EN  :  BitSwitch<"I2C_MST_EN",  USER_CTRL, 1 << 5> {};
    struct I2C_IF_DIS  : BitTrigger<"I2C_IF_DIS",  USER_CTRL, 1 << 4> {};
    struct DMP_RST     : BitTrigger<"DMP_RST",     USER_CTRL, 1 << 3> {};
    struct SRAM_RST    : BitTrigger<"SRAM_RST",    USER_CTRL, 1 << 2> {};
    struct I2C_MST_RST : BitTrigger<"I2C_MST_RST", USER_CTRL, 1 << 1> {};
};

struct PWR_MGMT_1   : Register<"PWR_MGMT_1", 0x06, 0, 0x41>
{
    struct DEVICE_RESET : BitTrigger<"DEVICE_RESET", PWR_MGMT_1, 1 << 7> {};
    struct SLEEP        :  BitSwitch<"SLEEP",        PWR_MGMT_1, 1 << 6> {};
    struct LP_EN        :  BitSwitch<"LP_EN",        PWR_MGMT_1, 1 << 5> {};
    struct TEMP_DIS     :  BitSwitch<"TEMP_DIS",     PWR_MGMT_1, 1 << 3> {};
    struct CLKSEL       :   BitField<"CLKSEL",       PWR_MGMT_1, 0b111>
    {
        struct InternalOscillator : BitFieldState<"InternalOscillator", CLKSEL, 0> {};
        struct AutoSelect         : BitFieldState<"AutoSelect",         CLKSEL, 1> {};
        struct Stop               : BitFieldState<"Stop",               CLKSEL, 7> {};
    };
};
// @/
```

And use them like this:

```cpp
// @+'tests'
{
    printf("icm20948-test: register bases test...\n");

    if (Registers::WHO_AM_I::read() != Registers::WHO_AM_I::after_reset)
    {
        printf("    unexpected who am I value?!\n");
        return false;
    }
    printf("    WHO_AM_I good...\n");

    Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
    delay(1);

    auto v = Registers::PWR_MGMT_1::read();
    if (v != Registers::PWR_MGMT_1::after_reset)
    {
        printf("    unexpected value %x after reset?!\n", v);
        return false;
    }
    printf("    value after reset good...\n");

    Registers::PWR_MGMT_1::SLEEP::disable();
    delay(1);

    // Confirm the change was recorded
    uint8_t value_after_wake_up = Registers::PWR_MGMT_1::read();
    uint8_t expected = Registers::PWR_MGMT_1::after_reset & ~Registers::PWR_MGMT_1::SLEEP::mask;
    if (value_after_wake_up != expected)
    {
        printf("    write operation unsuccessful?!\n");
        return false;
    }
    printf("    write operation good...\n");

    Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
    delay(1);

    printf("   passed!\n");
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
struct LP_CONFIG     : Register<"LP_CONFIG", 0x05, 0, 0x40>
{
    struct I2C_MST_CYCLE : BitSwitch<"I2C_MST_CYCLE", LP_CONFIG, 1 << 6> {};
    struct ACCEL_CYCLE   : BitSwitch<"ACCEL_CYCLE",   LP_CONFIG, 1 << 5> {};
    struct GYRO_CYCLE    : BitSwitch<"GYRO_CYCLE",    LP_CONFIG, 1 << 4> {};
};

struct PWR_MGMT_2   : Register<"PWR_MGMT_2", 0x07, 0, 0x00>
{
    struct DISABLE_ACCEL : BitSwitch<"DISABLE_ACCEL", PWR_MGMT_2, 0b111 << 3> {};
    struct DISABLE_GYRO  : BitSwitch<"DISABLE_GYRO",  PWR_MGMT_2, 0b111 << 0> {};
};

struct INT_PIN_CFG : Register<"INT_PIN_CFG", 0x0F, 0, 0x00>
{
    struct INT1_ACTL         : BitSwitch<"INT1_ACTL",         INT_PIN_CFG, 1 << 7> {};
    struct INT1_OPEN         : BitSwitch<"INT1_OPEN",         INT_PIN_CFG, 1 << 6> {};
    struct INT1_Latch__EN    : BitSwitch<"INT1_Latch__EN",    INT_PIN_CFG, 1 << 5> {};
    struct INT_ANYRD_2CLEAR  : BitSwitch<"INT_ANYRD_2CLEAR",  INT_PIN_CFG, 1 << 4> {};
    struct ACTL_FSYNC        : BitSwitch<"ACTL_FSYNC",        INT_PIN_CFG, 1 << 3> {};
    struct FSYNC_INT_MODE_EN : BitSwitch<"FSYNC_INT_MODE_EN", INT_PIN_CFG, 1 << 2> {};
    struct BYPASS_EN         : BitSwitch<"BYPASS_EN",         INT_PIN_CFG, 1 << 1> {};
};

// interrupt status registers
struct INT_STATUS_1 : Register<"INT_STATUS_1", 0x1A, 0, 0> {};
struct INT_STATUS_2 : Register<"INT_STATUS_2", 0x1B, 0, 0> {};
struct INT_STATUS_3 : Register<"INT_STATUS_3", 0x1C, 0, 0> {};

// delay time registers
struct DELAY_TIME_H : Register<"DELAY_TIME_H", 0x28, 0, 0> {};
struct DELAY_TIME_L : Register<"DELAY_TIME_L", 0x29, 0, 0> {};

// main IMU sensor data registers
struct ACCEL_XOUT_H         : Register<"ACCEL_XOUT_H",         0x2D, 0, 0> {};
struct ACCEL_XOUT_L         : Register<"ACCEL_XOUT_L",         0x2E, 0, 0> {};
struct ACCEL_YOUT_H         : Register<"ACCEL_YOUT_H",         0x2F, 0, 0> {};
struct ACCEL_YOUT_L         : Register<"ACCEL_YOUT_L",         0x30, 0, 0> {};
struct ACCEL_ZOUT_H         : Register<"ACCEL_ZOUT_H",         0x31, 0, 0> {};
struct ACCEL_ZOUT_L         : Register<"ACCEL_ZOUT_L",         0x32, 0, 0> {};
struct GYRO_XOUT_H          : Register<"GYRO_XOUT_H",          0x33, 0, 0> {};
struct GYRO_XOUT_L          : Register<"GYRO_XOUT_L",          0x34, 0, 0> {};
struct GYRO_YOUT_H          : Register<"GYRO_YOUT_H",          0x35, 0, 0> {};
struct GYRO_YOUT_L          : Register<"GYRO_YOUT_L",          0x36, 0, 0> {};
struct GYRO_ZOUT_H          : Register<"GYRO_ZOUT_H",          0x37, 0, 0> {};
struct GYRO_ZOUT_L          : Register<"GYRO_ZOUT_L",          0x38, 0, 0> {};
struct TEMP_OUT_H           : Register<"TEMP_OUT_H",           0x39, 0, 0> {};
struct TEMP_OUT_L           : Register<"TEMP_OUT_L",           0x3A, 0, 0> {};

// external sensor data registers
struct EXT_SLV_SENS_DATA_00 : Register<"EXT_SLV_SENS_DATA_00", 0x3B, 0, 0> {};
struct EXT_SLV_SENS_DATA_01 : Register<"EXT_SLV_SENS_DATA_01", 0x3C, 0, 0> {};
struct EXT_SLV_SENS_DATA_02 : Register<"EXT_SLV_SENS_DATA_02", 0x3D, 0, 0> {};
struct EXT_SLV_SENS_DATA_03 : Register<"EXT_SLV_SENS_DATA_03", 0x3E, 0, 0> {};
struct EXT_SLV_SENS_DATA_04 : Register<"EXT_SLV_SENS_DATA_04", 0x3F, 0, 0> {};
struct EXT_SLV_SENS_DATA_05 : Register<"EXT_SLV_SENS_DATA_05", 0x40, 0, 0> {};
struct EXT_SLV_SENS_DATA_06 : Register<"EXT_SLV_SENS_DATA_06", 0x41, 0, 0> {};
struct EXT_SLV_SENS_DATA_07 : Register<"EXT_SLV_SENS_DATA_07", 0x42, 0, 0> {};
struct EXT_SLV_SENS_DATA_08 : Register<"EXT_SLV_SENS_DATA_08", 0x43, 0, 0> {};
struct EXT_SLV_SENS_DATA_09 : Register<"EXT_SLV_SENS_DATA_09", 0x44, 0, 0> {};
struct EXT_SLV_SENS_DATA_10 : Register<"EXT_SLV_SENS_DATA_10", 0x45, 0, 0> {};
struct EXT_SLV_SENS_DATA_11 : Register<"EXT_SLV_SENS_DATA_11", 0x46, 0, 0> {};
struct EXT_SLV_SENS_DATA_12 : Register<"EXT_SLV_SENS_DATA_12", 0x47, 0, 0> {};
struct EXT_SLV_SENS_DATA_13 : Register<"EXT_SLV_SENS_DATA_13", 0x48, 0, 0> {};
struct EXT_SLV_SENS_DATA_14 : Register<"EXT_SLV_SENS_DATA_14", 0x49, 0, 0> {};
struct EXT_SLV_SENS_DATA_15 : Register<"EXT_SLV_SENS_DATA_15", 0x4A, 0, 0> {};
struct EXT_SLV_SENS_DATA_16 : Register<"EXT_SLV_SENS_DATA_16", 0x4B, 0, 0> {};
struct EXT_SLV_SENS_DATA_17 : Register<"EXT_SLV_SENS_DATA_17", 0x4C, 0, 0> {};
struct EXT_SLV_SENS_DATA_18 : Register<"EXT_SLV_SENS_DATA_18", 0x4D, 0, 0> {};
struct EXT_SLV_SENS_DATA_19 : Register<"EXT_SLV_SENS_DATA_19", 0x4E, 0, 0> {};
struct EXT_SLV_SENS_DATA_20 : Register<"EXT_SLV_SENS_DATA_20", 0x4F, 0, 0> {};
struct EXT_SLV_SENS_DATA_21 : Register<"EXT_SLV_SENS_DATA_21", 0x50, 0, 0> {};
struct EXT_SLV_SENS_DATA_22 : Register<"EXT_SLV_SENS_DATA_22", 0x51, 0, 0> {};
struct EXT_SLV_SENS_DATA_23 : Register<"EXT_SLV_SENS_DATA_23", 0x52, 0, 0> {};

// gyro configuration registers
struct GYRO_SMPLRT_DIV : Register<"GYRO_SMPLRT_DIV", 0x00, 2, 0> {};

struct GYRO_CONFIG_1 : Register<"GYRO_CONFIG_1", 0x01, 2, 0x01>
{
    struct GYRO_DLPFCFG : BitField<"GYRO_DLPFCFG", GYRO_CONFIG_1, (0b111 << 3)>
    {
        struct LPF_196_6Hz : BitFieldState<"LPF_196_6Hz", GYRO_DLPFCFG, 0b000 << 3> {};
        struct LPF_151_8Hz : BitFieldState<"LPF_151_8Hz", GYRO_DLPFCFG, 0b001 << 3> {};
        struct LPF_119_5Hz : BitFieldState<"LPF_119_5Hz", GYRO_DLPFCFG, 0b010 << 3> {};
        struct LPF_51_2Hz  : BitFieldState<"LPF_51_2Hz",  GYRO_DLPFCFG, 0b011 << 3> {};
        struct LPF_23_9Hz  : BitFieldState<"LPF_23_9Hz",  GYRO_DLPFCFG, 0b100 << 3> {};
        struct LPF_11_6Hz  : BitFieldState<"LPF_11_6Hz",  GYRO_DLPFCFG, 0b101 << 3> {};
        struct LPF_5_7Hz   : BitFieldState<"LPF_5_7Hz",   GYRO_DLPFCFG, 0b110 << 3> {};
        struct LPF_361_4Hz : BitFieldState<"LPF_361_4Hz", GYRO_DLPFCFG, 0b111 << 3> {};
    };
    struct GYRO_FS_SEL : BitField<"GYRO_FS_SEL", GYRO_CONFIG_1, (0b11 << 1)>
    {
        struct DPS_250  : BitFieldState<"DPS_250",  GYRO_FS_SEL, 0b000> {};
        struct DPS_500  : BitFieldState<"DPS_500",  GYRO_FS_SEL, 0b010> {};
        struct DPS_1000 : BitFieldState<"DPS_1000", GYRO_FS_SEL, 0b100> {};
        struct DPS_2000 : BitFieldState<"DPS_2000", GYRO_FS_SEL, 0b110> {};
    };

    struct GYRO_FCHOICE : BitSwitch<"GYRO_FCHOICE", GYRO_CONFIG_1, 0b1> {};
};

struct ACCEL_SMPLRT_DIV_1 : Register<"ACCEL_SMPLRT_DIV_1", 0x10, 2, 0> {};
struct ACCEL_SMPLRT_DIV_2 : Register<"ACCEL_SMPLRT_DIV_2", 0x11, 2, 0> {};

struct ACCEL_CONFIG : Register<"ACCEL_CONFIG", 0x14, 2, 0x01>
{
    struct ACCEL_DLPFCFG : BitField<"ACCEL_DLPFCFG", ACCEL_CONFIG, (0b111 << 3)>
    {
        struct LPF_246_0Hz     : BitFieldState<"LPF_246_0Hz",     ACCEL_DLPFCFG, 0b000 << 3> {};
        struct LPF_111_4Hz     : BitFieldState<"LPF_111_4Hz",     ACCEL_DLPFCFG, 0b010 << 3> {};
        struct LPF_50_4Hz      : BitFieldState<"LPF_50_4Hz",      ACCEL_DLPFCFG, 0b011 << 3> {};
        struct LPF_23_9Hz      : BitFieldState<"LPF_23_9Hz",      ACCEL_DLPFCFG, 0b100 << 3> {};
        struct LPF_11_5Hz      : BitFieldState<"LPF_11_5Hz",      ACCEL_DLPFCFG, 0b101 << 3> {};
        struct LPF_5_7Hz       : BitFieldState<"LPF_5_7Hz",       ACCEL_DLPFCFG, 0b110 << 3> {};
        struct LPF_473Hz       : BitFieldState<"LPF_473Hz",       ACCEL_DLPFCFG, 0b111 << 3> {};
    };
    
    struct ACCEL_FS_SEL : BitField<"ACCEL_FS_SEL", ACCEL_CONFIG, (0b11 << 1)>
    {
        struct G_2  : BitFieldState<"G_2",  ACCEL_FS_SEL, 0b00 << 1> {};
        struct G_4  : BitFieldState<"G_4",  ACCEL_FS_SEL, 0b01 << 1> {};
        struct G_8  : BitFieldState<"G_8",  ACCEL_FS_SEL, 0b10 << 1> {};
        struct G_16 : BitFieldState<"G_16", ACCEL_FS_SEL, 0b11 << 1> {};
    };
    
    struct ACCEL_FCHOICE : BitSwitch<"ACCEL_FCHOICE", ACCEL_CONFIG, 0b1>
    {
        struct BYPASS_DLPF : BitFieldState<"BYPASS_DLPF", ACCEL_FCHOICE, 0b0> {};
        struct ENABLE_DLPF : BitFieldState<"ENABLE_DLPF", ACCEL_FCHOICE, 0b1> {};
    };
   
};
// @/
```

## Auxiliary I2C Controllers

The ICM20948 has hardware support for controlling external sensors attached to
an auxiliary I2C bus. There are five controllers. The first four are suited for
continuously reading up to 15 bytes from I2C devices on the auxiliary bus, while
the final controller is suited for single byte read and write operations. None
of these controller is capable of multi-byte writes.

Presently we only make use of the final controller. It registers are declared thus:

```cpp
// @+'registers'

// Aux I2C registers

// remember: addr bit 7 is read/write (1 - read, 0 - write), 6:0 are I2C address to access
struct I2C_SLV4_ADDR : Register<"I2C_SLV4_ADDR", 0x13, 3, 0> {};
struct I2C_SLV4_REG  : Register<"I2C_SLV4_REG",  0x14, 3, 0> {};
struct I2C_SLV4_DO   : Register<"I2C_SLV4_DO",  0x16, 3, 0> {};
struct I2C_SLV4_DI   : Register<"I2C_SLV4_DI",  0x16, 3, 0> {};
struct I2C_SLV4_CTRL : Register<"I2C_SLV4_ADDR", 0x15, 3, 0>
{
    struct I2C_SLV4_EN : BitTrigger<"I2C_SLV4_EN", I2C_SLV4_CTRL, 1 << 7> {};
    struct I2C_SLV4_INT_EN : BitSwitch<"I2C_SLV4_INT_EN", I2C_SLV4_CTRL, 1 << 6> {};
    struct I2C_SLV4_REG_DIS : BitSwitch<"I2C_SLV4_REG_DIS", I2C_SLV4_CTRL, 1 << 5> {};
    struct I2C_SLV4_DLY : BitField<"", I2C_SLV4_CTRL, 0b11111> {};
};

struct I2C_MST_STATUS : Register<"I2C_MST_STATUS", 0x17, 0, 0> {};
// @/
```

We implement the serial interface API using this controller:

```cpp
// @#'sygsp-icm20948_aux_serif.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>
#include "sygsp-delay.hpp"
#include "sygsp-icm20948_registers.hpp"

namespace sygaldry { namespace sygsp {
/// \addtogroup sygsp-icm20948
/// \{
/// \defgroup sygsp-icm20948_aux_serif sygsp-icm20948_aux_serif: ICM20948 Auxiliary I2C Bus Controller Serial Interface

/*! Limited I2C serial interface using the ICM20948 aux bus

\pre

This API requires that the ICM20948 accessible through the given serial
interface should be connecting, accessible, not sleeping, and not in low power
mode mode. The user is requird to check or enforce this precondition, since
this API does not.

\tparam Serif the serial interface over which to access the ICM20948
*/
template<typename Serif, uint8_t i2c_address>
struct ICM20948AuxSerif
{
    using Registers = ICM20948Registers<Serif>;

    /// Read one byte and return it
    [[nodiscard]] static uint8_t read(uint8_t register_address)
    {
        Registers::I2C_SLV4_ADDR::write(1<<8 | i2c_address);
        Registers::I2C_SLV4_REG::write(register_address);
        Registers::I2C_SLV4_CTRL::I2C_SLV4_EN::trigger();
        delay(1);
        return Registers::I2C_SLV4_DI::read();
    }

    /// Write one byte
    static void write(uint8_t register_address, uint8_t value)
    {
        Registers::I2C_SLV4_ADDR::write(1<<8 | i2c_address);
        Registers::I2C_SLV4_REG::write(register_address);
        Registers::I2C_SLV4_DO::write(value);
        Registers::I2C_SLV4_CTRL::I2C_SLV4_EN::trigger();
    }
};

/// \}
/// \}
} }
// @/
```

## AK09916 Magnetometer Support

As well as its accelerometer and gyroscope, the ICM20948 also includes an
embedded magnetometer with a seperate I2C address and register space. The
AK09916 doesn't have use banks like the ICM20948, but otherwise its API is very
similar. We define a seperate register base class for this sub-device, and
document its registers as above.

```cpp
// @+'base classes'

/*! Base class for AK09916 registers

Similar to `Register`, but without user banks.
*/
template <string_literal name, uint8_t addr_, uint8_t reset_>
struct AK09916Register
{
    static constexpr uint8_t address = addr_;
    static constexpr uint8_t after_reset = reset_;
    static constexpr const char * register_name() {return name.value;}

    [[nodiscard]] static uint8_t read()
    {
        return Serif::read(address);
    }

    static void write(uint8_t value)
    {
        Serif::write(address, value);
    }
};
// @/

// @+'registers'
// AK09916 registers

struct WIA1 : AK09916Register<"Company ID", 0x00, 0x48> {};
struct WIA2 : AK09916Register<"Device ID",  0x01, 0x09> {};
// reserved 1                               0x02
// reserved 2                               0x03
// sequential reads in one transaction (I2C auto increment) skip to 10

struct ST1 : AK09916Register<"Status 1", 0x10, 0>
{
    struct DOR  : BitField<"DOR",  ST1, 0b10> {};
    struct DRDY : BitField<"DRDY", ST1, 0b01> {};
};
struct HXL : AK09916Register<"HXL", 0x11, 0> {};
struct HXH : AK09916Register<"HXH", 0x12, 0> {};
struct HYL : AK09916Register<"HYL", 0x13, 0> {};
struct HYH : AK09916Register<"HYH", 0x14, 0> {};
struct HZL : AK09916Register<"HZL", 0x15, 0> {};
struct HZH : AK09916Register<"HZH", 0x16, 0> {};
// TMPS "dummy" registers           0x17
struct ST2 : AK09916Register<"Status 2", 0x18, 0> {}; // must be read to signal end of read transaction!
// sequential reads in one transaction (I2C auto increment) roll over to 0

struct CNTL2 : AK09916Register<"CNTL2", 0x31, 0>
{
    // The datasheet recommends (sec 9.3, pg 10) that to switch modes,
    // first change to power down, wait at least 100 us, then switch to the desired mode.
    // It seems from later sections (9.4.3) that this may not be a strict requirement?
    struct MODE : BitField<"MODE", CNTL2, 0b00011111>
    {
        struct PowerDown           : BitFieldState<"PowerDown",         MODE, 0b00000> {};
        struct SingleMeasurement   : BitFieldState<"SingleMeasurement", MODE, 0b00001> {};
        // sampling rates given in the AK09916 datasheet
        struct ContinuousMode10HZ  : BitFieldState<"ContinuousMode1",   MODE, 0b00010> {};
        struct ContinuousMode20Hz  : BitFieldState<"ContinuousMode2",   MODE, 0b00100> {};
        struct ContinuousMode50Hz  : BitFieldState<"ContinuousMode3",   MODE, 0b00110> {};
        struct ContinuousMode100Hz : BitFieldState<"ContinuousMode4",   MODE, 0b01000> {};
        struct SelfTest            : BitFieldState<"SelfTest",          MODE, 0b10000> {};
    };
};

struct CNTL3 : AK09916Register<"CNTL3", 0x32, 0>
{
    struct SRST : BitTrigger<"Soft Reset", CNTL3, 1> {};
};
// @/
```

The test here sets up the ICM20948 so that the magnetometer can be accessed via
the main I2C bus. Then the device ID is checked and a self-test measurement is
performed. The test serves to demonstrate that the register definitions and
bases are working, and that the magnetometer self-test is in the expected
range.

```cpp
// @+'tests'
{
    printf("icm20948-test: AK09916 test... \n");
    Registers::PWR_MGMT_1::DEVICE_RESET::trigger(); delay(10);
    Registers::PWR_MGMT_1::SLEEP::disable(); delay(10);
    Registers::PWR_MGMT_1::LP_EN::disable(); delay(1);
    Registers::INT_PIN_CFG::BYPASS_EN::enable(); delay(1);
    Registers::USER_CTRL::I2C_MST_EN::disable(); delay(1);

    if (AK09916Registers::WIA2::read() != AK09916Registers::WIA2::after_reset)
    {
        printf("unable to connect to AK09916\n");
        return false;
    }
    else printf("AK09916 connected...\n");
    AK09916Registers::CNTL3::SRST::trigger(); delay(1);
    AK09916Registers::CNTL2::MODE::PowerDown::set(); delay(1);
    AK09916Registers::CNTL2::MODE::SelfTest::set(); delay(1);
    int i = 0;
    while (i < 100 && !AK09916Registers::ST1::DRDY::read_field()) { delay(10); }
    if (i >= 100)
    {
        printf("timeout while waiting for data ready?!\n");
        return false;
    }
    constexpr uint8_t N_OUT = 8;
    uint8_t measurement_data[N_OUT] = {0};
    AK09916Serif::read(AK09916Registers::HXL::address, measurement_data, N_OUT);
    int16_t x = measurement_data[1] << 8 | (measurement_data[0] & 0xFF);
    int16_t y = measurement_data[3] << 8 | (measurement_data[2] & 0xFF);
    int16_t z = measurement_data[5] << 8 | (measurement_data[4] & 0xFF);
    if (!(-200 <= x && x <= 200))
    {
        printf("x data %d outside self-test range?! x: %d  y: %d  z: %d\n", x, x, y, z);
        return false;
    }
    if (!(-200 <= y && y <= 200))
    {
        printf("y data %d outside self-test range?! x: %d  y: %d  z: %d\n", y, x, y, z);
        return false;
    }
    if (!(-1000 <= z && z <= -200))
    {
        printf("z data %d outside self-test range?! x: %d  y: %d  z: %d\n", z, x, y, z);
        return false;
    }
    printf("AK09916 self test pass! x: %d  y: %d  z: %d\n", x, y, z);
}
// @/
```

## Registers Summary

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
#include "sygah-string_literal.hpp"

namespace sygaldry { namespace sygsp {

/// \addtogroup sygsp-icm20948
/// \{
/// \defgroup sygsp-icm20948_registers sygsp-icm20948_registers: Registers for ICM20948 MIMU
/// \{

static constexpr uint8_t AK09916_I2C_ADDRESS    = 0b0001100;
static constexpr uint8_t ICM20948_I2C_ADDRESS_0 = 0b1101000;
static constexpr uint8_t ICM20948_I2C_ADDRESS_1 = 0b1101001;

template<typename Serif>
struct ICM20948Registers
{

@{base classes}

@{registers}

};

template<typename Serif>
uint8_t ICM20948Registers<Serif>::current_bank_ = 0xFF;

/// \}
/// \}
} }
// @/
```

# Main API

The main API then ties these resources together, along with the MIMU endpoints.

```cpp
// @#'sygsp-icm20948.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sygah-mimu.hpp"
#include "sygah-metadata.hpp"
#include "sygsp-icm20948_registers.hpp"
#include "sygsp-icm20948_tests.hpp"
#include "sygsp-delay.hpp"
#include "sygsp-micros.hpp"
#include "sygsp-mimu_units.hpp"

namespace sygaldry { namespace sygsp {
/// \addtogroup sygsp
/// \{
/// \defgroup sygsp-icm20948 sygsp-icm20948: ICM20948 MIMU Driver
/// \{

template<typename Serif, typename AK09916Serif>
struct ICM20948
: name_<"ICM20948 MIMU">
{
    struct inputs_t {
        // TODO: sensitivity, digital low pass filter controls, measurement rate, etc.
    } inputs;

    struct outputs_t {
        vec3_message<"accelerometer raw", int, -32768, 32767, "LSB"> accl_raw;
        slider<"accelerometer sensitivity", "g/LSB", float, 1/16384.0f, 1/2048.0f, 1/4096.0f> accl_sensitivity;
        vec3_message<"accelerometer", float, -16, 16, "g"> accl;

        vec3_message<"gyroscope raw", int, -32768, 32767, "LSB"> gyro_raw;
        slider<"gyroscope sensitivity", "(rad/s)/LSB", float, 1/131.0f * rad_per_deg, 1/16.4f * rad_per_deg, 1/16.4f * rad_per_deg> gyro_sensitivity;
        vec3_message<"gyroscope", float, -2000.0f * rad_per_deg, 2000.0f * rad_per_deg, "rad/s"> gyro;

        vec3_message<"magnetometer raw", int, -32768, 32767, "LSB"> magn_raw;
        slider<"magnetometer sensitivity", "uT/LSB", float, 0.15f, 0.15f> magn_sensitivity;
        vec3_message<"magnetometer", float, -4900, 4900, "uT"> magn;

        slider_message<"elapsed", "time in microseconds elapsed since last measurement", unsigned long, 0, 1000000, 0> elapsed;

        text_message<"error message"> error_message;

        toggle<"running"> running;
    } outputs;

    using Registers = ICM20948Registers<Serif>;
    using AK09916Registers = ICM20948Registers<AK09916Serif>;

    /// initialize the ICM20948 for continuous reading
    void init()
    {
        @{init}
    }

    // poll the ICM20948 for new data and update endpoints
    void main()
    {
        @{main}
    }
};

/// \}
/// \}
} }
// @/
```

## Initialization

We begin initialization by checking that all tests are passing. If this fails
for some reason, the device is disabled via a flag that is checked in the main
subroutine.

```cpp
// @='init'
outputs.running = true;
if (!ICM20948Tests<Serif, AK09916Serif>::test()) outputs.running = false;
if (!outputs.running) return;
// @/
```

Assuming all the tests pass, we set up the device.

```cpp
// @+'init'
Registers::PWR_MGMT_1::DEVICE_RESET::trigger(); delay(10); // reset (establish known preconditions)
Registers::PWR_MGMT_1::SLEEP::disable(); delay(10); // disable sleep
Registers::INT_PIN_CFG::BYPASS_EN::enable(); delay(1); // bypass the I2C controller, connecting the aux bus to the main bus
Registers::GYRO_CONFIG_1::GYRO_FS_SEL::DPS_2000::set();
Registers::ACCEL_CONFIG::ACCEL_FS_SEL::G_8::set();
AK09916Registers::CNTL3::SRST::trigger(); delay(1); // soft-reset the magnetometer (establish known preconditions)
AK09916Registers::CNTL2::MODE::ContinuousMode100Hz::set(); delay(1); // enable continuous reads
// @/
```

Finally, we set the sensitivity output endpoints to their default values.

```cpp
// @+'init'
outputs.accl_sensitivity = outputs.accl_sensitivity.init();
outputs.gyro_sensitivity = outputs.gyro_sensitivity.init();
outputs.magn_sensitivity = outputs.magn_sensitivity.init();
// @/
```

## Main

In the main subroutine, we read from the sensors.

The number of bytes to read is fixed at compile time based on the addresses of
the range of registers that should be read. We statically allocate a buffer on
the stack for reading the data.

```cpp
// @='main'
if (!outputs.running) return; // TODO: retry connecting every so often

static constexpr uint8_t IMU_N_OUT = 1 + Registers::GYRO_ZOUT_L::address
                                       - Registers::ACCEL_XOUT_H::address;
static constexpr uint8_t MAG_N_OUT = 1 + AK09916Registers::ST2::address
                                       - AK09916Registers::HXL::address;
static_assert(IMU_N_OUT == 12);
static_assert(MAG_N_OUT == 8);

static uint8_t raw[IMU_N_OUT];
// @/
```

The sensor fusion algorithm requires knowledge of the time between measurements.
We statically record an initial timestamp, and in each loop note the time before
attempting to read data. If a new measurement is read, then we update the time
elapsed since the previous measurement and adjust the timestamp of the previous
measurement.

```cpp
// @+'main'
static auto prev = micros();
auto now = micros();
bool read = false;
@{read data}
if (read)
{
    outputs.elapsed = now - prev;
    prev = now;
}
// @/
```

We poll the status registers of the two sensor modules (the ICM20948 and its
built-in magnetometer). When data is available, we proceed to read it and update
the relevant endpoints.

```cpp
// @='read data'
if (Registers::INT_STATUS_1::read())
{
    read = true;
    Serif::read(Registers::ACCEL_XOUT_H::address, raw, IMU_N_OUT);
    @{update IMU endpoints}
}
if (AK09916Registers::ST1::DRDY::read_field())
{
    read = true;
    AK09916Serif::read(AK09916Registers::HXL::address, raw, MAG_N_OUT);
    @{update magnetometer endpoints}
}
// @/
```

Updating the endpoints proceeds according to the endianness of the data as read
from the registers of the devices. We shuffle the upper and lower bytes
appropriately, transiting from `uint8_t`s to `int16_t`s to `int`s. The explicit
conversion ensure that the sign of the 16-bit values is preserved when
converting them to `int`; a more elegant expression of this conversion may be
possible, but this works for now. We then convert the raw data to more
meaningful units according to the current sensitivity of the sensors.

Note that the y and z axes of the magnetometer are flipped; this brings them
into agreement with those of the accelerometer and gyroscope, so that all three
coordinate systems are right-handed and (in principle) aligned.

```cpp
// @='update IMU endpoints'
outputs.accl_raw = { (int)(int16_t)( raw[0] << 8 | ( raw[1] & 0xFF))
                   , (int)(int16_t)( raw[2] << 8 | ( raw[3] & 0xFF))
                   , (int)(int16_t)( raw[4] << 8 | ( raw[5] & 0xFF))
                   };
outputs.gyro_raw = { (int)(int16_t)( raw[6] << 8 | ( raw[7] & 0xFF))
                   , (int)(int16_t)( raw[8] << 8 | ( raw[9] & 0xFF))
                   , (int)(int16_t)(raw[10] << 8 | (raw[11] & 0xFF))
                   };
outputs.accl = { outputs.accl_raw.x() * outputs.accl_sensitivity
               , outputs.accl_raw.y() * outputs.accl_sensitivity
               , outputs.accl_raw.z() * outputs.accl_sensitivity
               };
outputs.gyro = { outputs.gyro_raw.x() * outputs.gyro_sensitivity
               , outputs.gyro_raw.y() * outputs.gyro_sensitivity
               , outputs.gyro_raw.z() * outputs.gyro_sensitivity
               };
// @/
// @='update magnetometer endpoints'
outputs.magn_raw = { (int)(int16_t)( raw[1] << 8 | ( raw[0] & 0xFF))
                   , (int)(int16_t)( raw[3] << 8 | ( raw[2] & 0xFF))
                   , (int)(int16_t)( raw[5] << 8 | ( raw[4] & 0xFF))
                   };
outputs.magn = { outputs.magn_raw.x() * outputs.magn_sensitivity
               , -outputs.magn_raw.y() * outputs.magn_sensitivity
               , -outputs.magn_raw.z() * outputs.magn_sensitivity
               };
// @/
```

# Test Library

A software subcomponent is provided that collects various tests into a static
method that is called while initializing the MIMU to make sure everything seems
to be working as expected. Several tests are filled in above.

```cpp
// @#'sygsp-icm20948_tests.hpp'
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
#include "sygsp-icm20948_registers.hpp"
#include "sygsp-delay.hpp"

namespace sygaldry { namespace sygsp {

/// \addtogroup sygsp-icm20948
/// \{

template<typename Serif, typename AK09916Serif>
struct ICM20948Tests
{
    using Registers =        ICM20948Registers<Serif>;
    using AK09916Registers = ICM20948Registers<AK09916Serif>;
    static bool test()
    {
        @{tests}
        return true;
    }
};

/// \}
} }
// @/
```

# Build

The various sub-components are collected together into one CMake library.

```cmake
# @#'CMakeLists.txt'
set(lib sygsp-icm20948)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE
        sygah-mimu
        sygah-metadata
        sygsp-delay
        sygsp-delay
        sygsp-micros
        sygsp-mimu_units
        )
@{cmake snippets}
# @/
```

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
struct LP_CONFIG     : Register<0x05, 0, 0x40>
{
    struct I2C_MST_CYCLE : BitSwitch<LP_CONFIG, 1 << 6> {};
    struct ACCEL_CYCLE   : BitSwitch<LP_CONFIG, 1 << 5> {};
    struct GYRO_CYCLE    : BitSwitch<LP_CONFIG, 1 << 4> {};
};

};

template<typename Serif>
uint8_t ICM20948Registers<Serif>::current_bank_ = 0xFF;

} }

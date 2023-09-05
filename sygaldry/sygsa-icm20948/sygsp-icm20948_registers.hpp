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

static constexpr uint8_t AK09916_I2C_ADDRESS    = 0b0001100;
static constexpr uint8_t ICM20948_I2C_ADDRESS_0 = 0b1101000;
static constexpr uint8_t ICM20948_I2C_ADDRESS_1 = 0b1101001;

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
        printf("sygsp-icm20948_registers: %s (%x) read\n", register_name(), address);
        return Serif::read(address);
    }

    static void write(uint8_t value)
    {
        select_bank();
        printf("sygsp-icm20948_registers: %s (%x) write %x\n", register_name(), address, value);
        Serif::write(address, value);
    }
};

/// read the current state of a register, clear the masked bits of a field, set those bits according to the value
template<typename RegisterField, uint8_t value>
static void read_modify_write()
{
    printf("sygsp-icm20948_registers: %s::%s (%x::%x) rmw\n", RegisterField::register_name(), RegisterField::field_name(), RegisterField::address, RegisterField::mask);
    static_assert(RegisterField::read && RegisterField::write);
    uint8_t read   = RegisterField::read();
    uint8_t modify = (read & ~RegisterField::mask) | (value & RegisterField::mask);
    /*      write */ RegisterField::write(modify);
    printf("sygsp-icm20948_registers: %s::%s %#04x -> %#04x\n", RegisterField::register_name(), RegisterField::field_name(), read, modify);
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

};

template<typename Serif>
uint8_t ICM20948Registers<Serif>::current_bank_ = 0xFF;

} }

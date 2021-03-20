// SPDX-FileCopyrightText: 2021 Mark Roberts
// SPDX-License-Identifier: MIT

#include "isl94208.h"
#include "i2c.h"

#define I2C_ADDR        (0x50)

#define FET_CTRL_REG        (0x04)
#define AN_CTR_REG          (0x03)
#define FEATURE_SET_REG     (0x07)
#define DISCHARGE_SET_REG   (0x05)
#define WR_ENABLE_REG       (0x08)


void isl94208_init(void)
{
    i2c_init();
}

bool isl94208_set_fet(uint8_t fet)
{
    uint8_t data[3] = {I2C_ADDR, FET_CTRL_REG, fet};

    return i2c_write(data, sizeof(data));
}

bool isl94208_set_analog(eAnalogSelect channel)
{
    uint8_t data[3] = {I2C_ADDR, AN_CTR_REG, channel};

    return i2c_write(data, sizeof(data));
}

bool isl94208_set_feature(uint8_t features)
{
    uint8_t data[3] = {I2C_ADDR, FEATURE_SET_REG, features};

    return i2c_write(data, sizeof(data));
}

bool isl94208_set_discharge_cfg(uint8_t discharge_cfg)
{
    uint8_t data[3] = {I2C_ADDR, DISCHARGE_SET_REG, discharge_cfg};

    return i2c_write(data, sizeof(data));
}

bool isl94208_set_write_enable(uint8_t enable)
{
    uint8_t data[3] = {I2C_ADDR, WR_ENABLE_REG, enable};

    return i2c_write(data, sizeof(data));
}

/* SPDX-FileCopyrightText: 2021 Mark Roberts */
/* SPDX-License-Identifier: MIT */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize I2C subsystem
 */
void i2c_init(void);

/**
 *  Write specified amount of bytes to I2C bus
 *
 *  Return true if ACK is received
 */
bool i2c_write(uint8_t *data, uint8_t size);


/**
 * Read sequecne of bytes from the specifed slave
 * Return true if ACK is received
 */
bool i2c_read(uint8_t slave, uint8_t addr, uint8_t* buf, uint8_t size);

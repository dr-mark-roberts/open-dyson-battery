/* SPDX-FileCopyrightText: 2021 Mark Roberts */
/* SPDX-License-Identifier: MIT */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize i2c bus for communication
 */
void isl94208_init(void);


typedef enum {
    FET_DFET = 0,
    FET_CFET = 1,
    FET_LDMONEN = 6,
    FET_SLEEP = 7
} eFetCtrlBits;


typedef enum {
    AN_OFF = 0,   // high impedance output, power save
    AN_CELL1 = 1,
    AN_CELL2 = 2,
    AN_CELL3 = 3,
    AN_CELL4 = 4,
    AN_CELL5 = 5,
    AN_CELL6 = 6,
    AN_EXT_TEMP = 8,
    AN_INT_TEMP = 9

} eAnalogSelect;

typedef enum {
    WKPOL = 0,          // WakeUp polarity, 0 - falling edge, 1 - rising edge
} eFeatureBits;

typedef enum {
    OCDT0 = 0,
    OCDT1 = 1,
    SCDV0 = 2,
    SCDV1 = 3,
    DENSCD = 4,
    OCDV0 = 5,
    OCDV1 = 6,
    DENOCD = 7
} eDischargeCfgBits;


typedef enum {
    DISSETEN = 5,
    CHSETEN = 6,
    FSETEN = 7
} eWriteEnableBits;


/**
 * Set FET control register
 * Use eFetCtrBits
 */
bool isl94208_set_fet(uint8_t fet);

/**
 * Select analog channel
 */
bool isl94208_set_analog(eAnalogSelect channel);


/**
 * Set feature set configuration register
 */
bool isl94208_set_feature(uint8_t features);

/**
 * Set discharge configuration set
 */
bool isl94208_set_discharge_cfg(uint8_t discharge_cfg);

/**
 * Enable write to some configuration registers
 * Use eWriteEnableBits to set desired bits
 */
bool isl94208_set_write_enable(uint8_t enable);

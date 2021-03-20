// SPDX-FileCopyrightText: 2021 Mark Roberts
// SPDX-License-Identifier: MIT

#include "i2c.h"
#include "xc.h"

void i2c_init(void)
{
    SSPCON1 = 1<<5 | 0b1000;   // SSPEN, i2c master mode
    SSPCON2 = 0;               // default values
    SSPADD = 128;               // ~33kHz
    SSPSTAT = 0;               // default values
    TRISBbits.TRISB1 = 1;      // RB1 as input
    TRISBbits.TRISB4 = 1;      // RB4 as input
}

static void i2c_wait()
{
    while ( (SSPCON2 & 0x1F) || SSPSTATbits.R_nW ) ;
}

static bool i2c_write_no_stop(uint8_t *data, uint8_t size)
{
    // start condition
    i2c_wait();
    SSPCON2bits.SEN = 1;
    i2c_wait();

    for (uint8_t i = 0; i < size; i++) {
        SSPBUF = data[i];
        i2c_wait();
        if (SSPCON2bits.ACKSTAT) {
            return false;
        }
    }
    return true;
}

bool i2c_write(uint8_t *data, uint8_t size)
{
    if (!i2c_write_no_stop(data, size)) {
        // stop condition to release the bus
        SSPCON2bits.PEN = 1;
        return false;
    }

    // stop condition
    SSPCON2bits.PEN = 1;
    return true;
}

bool i2c_read(uint8_t slave, uint8_t addr, uint8_t* buf, uint8_t size)
{
    uint8_t start[2] = {slave, addr};

    if (!i2c_write_no_stop(start, sizeof(start))) {
        SSPCON2bits.PEN = 1;  // stop condition to release the bus
        return false;
    }

    // repeated start condition
    i2c_wait();
    SSPCON2bits.RSEN = 1;
    i2c_wait();

    SSPBUF = slave | (1<<0);
    i2c_wait();
    if (SSPCON2bits.ACKSTAT) {
        SSPCON2bits.PEN = 1;  // stop condition to release the bus
        return false;
    }

    for (uint8_t i = 0; i < size; i++) {
        i2c_wait();
        SSPCON2bits.RCEN = 1;   // initiate read

        i2c_wait();
        buf[i] = SSPBUF;

        i2c_wait();
        SSPCON2bits.ACKDT = (i == (size-1)) ? 1 : 0;       // set ACK bit
        SSPCON2bits.ACKEN = 1;       // send ACK bit
    }
    i2c_wait();
    SSPCON2bits.PEN = 1;
    return true;
}

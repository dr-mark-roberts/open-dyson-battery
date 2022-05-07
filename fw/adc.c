// SPDX-FileCopyrightText: 2021 Mark Roberts
// SPDX-License-Identifier: MIT

#include "adc.h"
#include "config.h"
#include <xc.h>
#include "isl94208.h"


#define IDLE_THRESHOLD                  (40)
#define BUTTON_PRESSED_THRESHOLD        (80)
#define CHARGER_CONNECTED_THRESHOLD     (720)

#define WAIT_ADC_RESULT()   while (ADCON0bits.GO);
#define WRITE_ADC_VAL(x)        (x) = ADRESH;   \
                                (x) = (x) << 8; \
                                (x) |= ADRESL;


/**
 * Current voltages of cells in ADC format
 */
static uint16_t cells[CELLS_NUMBER];

/**
 * Button or charger pin voltage value in ADC format
 */
static uint16_t button_charger_val;


static bool available = false;


void adc_init(void)
{
    ANSELBbits.ANSB5 = 1;       // RB5 as analog input
    TRISBbits.TRISB5 = 1;       // RB5 as input

    ANSELAbits.ANSA4 = 1;       // RA4 as analog input
    TRISAbits.TRISA4 = 1;       // RA4 as input

    ADCON0bits.ADON = 1;
    ADCON1bits.ADPREF = 1<<1;       // external reference voltage
    ADCON1bits.ADFM = 1;            // value right justified
    ADCON1bits.ADCS1 = 1;           // conversion clock = Fosc/32
}

void adc_process(void)
{
    ADCON0bits.CHS = 7;            // RB5 AN7 channel
    __delay_us(10);
    ADCON0bits.GO = 1;             // start ADC
    WAIT_ADC_RESULT();
    WRITE_ADC_VAL(button_charger_val);

    ADCON0bits.CHS = 4;
    __delay_us(10);
    for (uint8_t i = 0; i < CELLS_NUMBER; i++) {
        isl94208_set_analog(AN_CELL1 + i);
        /* isl94208_set_analog(AN_INT_TEMP); */
        /* __delay_ms(2); */
        ADCON0bits.GO = 1;             // start ADC
        WAIT_ADC_RESULT();
        WRITE_ADC_VAL(cells[i]);
    }
    isl94208_set_analog(AN_OFF);        // to reduce power consumption
    available = true;
}

bool adc_available(void)
{
    return available;
}

uint16_t adc_get_cell(uint8_t cell_index)
{
    return cells[cell_index];
}

bool adc_is_button_pressed(void)
{
    return  (button_charger_val > BUTTON_PRESSED_THRESHOLD &&
             button_charger_val < CHARGER_CONNECTED_THRESHOLD);
}

bool adc_is_charger_connected(void)
{
    return button_charger_val > CHARGER_CONNECTED_THRESHOLD;
}

bool adc_ctrl_idle(void)
{
    return button_charger_val < IDLE_THRESHOLD;
}


// SPDX-FileCopyrightText: 2021 Mark Roberts
// SPDX-License-Identifier: MIT

#include "config.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (ECH, External Clock, High Power Mode (4-32 MHz): device clock supplied to CLKIN pin)
#pragma config WDTE = OFF        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


#include <xc.h>
#include <pic.h>
#include <stdint.h>

#include "isl94208.h"
#include "i2c.h"
#include "adc.h"

#ifdef HW_V6_188002
  #define RED_LED_PORT PORTAbits.RA7
  #define RED_LED_TRIS TRISAbits.TRISA7

  #define BLUE_LED_PORT PORTAbits.RA6
  #define BLUE_LED_TRIS TRISAbits.TRISA6

  #define GREEN_LED_PORT PORTBbits.RB3
  #define GREEN_LED_TRIS TRISBbits.TRISB3
#elif HW_V8_180207
  #define RED_LED_PORT PORTAbits.RA7
  #define RED_LED_TRIS TRISAbits.TRISA7

  #define BLUE_LED_PORT PORTBbits.RB3
  #define BLUE1_LED_PORT PORTBbits.RB3
  #define BLUE1_LED_TRIS TRISBbits.TRISB3

  #define BLUE2_LED_PORT PORTBbits.RB0
  #define BLUE2_LED_TRIS TRISBbits.TRISB0

  #define BLUE3_LED_PORT PORTAbits.RA6
  #define BLUE3_LED_TRIS TRISAbits.TRISA6
#endif


typedef enum {
    STATE_IDLE = 0,
    STATE_WAIT_IDLE,
    STATE_ON,
    STATE_CHARGING,
    STATE_UNDERVOLTAGE,
    STATE_CHARGED,
}eState;


int main(void)
{
    uint32_t idle_ms = 0;
    bool result;
    eState state = STATE_IDLE;

    OSCCON = 0b01111010;

    isl94208_init();
    adc_init();

	RED_LED_TRIS = 0; // Pin as output
	RED_LED_PORT = 1; // LED off

#ifdef HW_V6_188002
	BLUE_LED_TRIS = 0; // Pin as output
	BLUE_LED_PORT = 1; // LED off

	GREEN_LED_TRIS = 0; // Pin as output
	GREEN_LED_PORT = 1; // LED off
#elif HW_V8_180207
	BLUE1_LED_TRIS = 0; // Pin as output
	BLUE1_LED_PORT = 1; // LED off
	BLUE2_LED_TRIS = 0; // Pin as output
	BLUE2_LED_PORT = 1; // LED off
	BLUE3_LED_TRIS = 0; // Pin as output
	BLUE3_LED_PORT = 1; // LED off
#endif

    isl94208_set_write_enable(1<<FSETEN);
    isl94208_set_feature(1<<WKPOL);         // use high level for wake up

    isl94208_set_write_enable(1<<DISSETEN);
    isl94208_set_discharge_cfg((1<<OCDV0) | (1<<OCDV1) | (1<<SCDV0) | (1<<SCDV1));  // turn off overcurrent protection for now

	while (1) {
        __delay_ms(10);

        adc_process();
        if (!adc_available()) {
            __delay_ms(10);
            continue;
        }
        for (uint8_t i = 0; i < CELLS_NUMBER; i++) {
            uint16_t cell_v = adc_get_cell(i);
            if (cell_v < VOLT_TO_ADC(2.8)) {
                if (state == STATE_ON) {
                    BLUE_LED_PORT = 1;
                    RED_LED_PORT = 0;
                    state = STATE_UNDERVOLTAGE;
                    isl94208_set_fet(0);
                }
            } else if (cell_v > VOLT_TO_ADC(4.2)) {
                if (state == STATE_CHARGING) {
#ifdef HW_V6_188002
                    GREEN_LED_PORT = 0;
#endif
                    BLUE_LED_PORT = 1;
                    state = STATE_CHARGED;
                    isl94208_set_fet(0);
                }
            }
        }
        if (adc_is_button_pressed()) {
           if(state == STATE_IDLE) {
                BLUE_LED_PORT = 0;
                result = isl94208_set_fet(1<<FET_DFET);
                state = STATE_ON;
                idle_ms = 0;
           }
        } else {
            if (state == STATE_ON) {
                BLUE_LED_PORT = 1;
                result = isl94208_set_fet(0);
                state = STATE_IDLE;
                idle_ms = 0;
            }
        }

        if (adc_is_charger_connected()) {
            if ( state == STATE_IDLE ||
                 state == STATE_UNDERVOLTAGE ||
                 state == STATE_WAIT_IDLE ) {
                BLUE_LED_PORT = 0;
                RED_LED_PORT = 1;
                result = isl94208_set_fet(1<<FET_CFET);
                state = STATE_CHARGING;
                idle_ms = 0;
            }
        } else {
            if (state == STATE_CHARGING || state == STATE_CHARGED) {
                BLUE_LED_PORT = 1;
#ifdef HW_V6_188002
                GREEN_LED_PORT = 1;
#endif
                result = isl94208_set_fet(0);
                state = STATE_WAIT_IDLE;
                idle_ms = 0;
            }
        }

        if (state == STATE_WAIT_IDLE) {
            idle_ms = 0;        // don't go to sleep untill idle state is reached
            if (adc_ctrl_idle()) {
                state = STATE_IDLE;
            }
        }

        idle_ms += 10;

        if (idle_ms > 10000 &&
                (state == STATE_IDLE || state == STATE_UNDERVOLTAGE)) {
#ifdef HW_V6_188002
            GREEN_LED_PORT = 0;
#endif
            RED_LED_PORT = 1;
            BLUE_LED_PORT = 1;
            __delay_ms(200);        // delay to give time to go to sleep
#ifdef HW_V6_188002
            GREEN_LED_PORT = 1;
#endif
            result = isl94208_set_fet(1<<FET_SLEEP);  // go to sleep
            __delay_ms(1000);        // delay to give time to go to sleep
        }
	}
    return 0;
}

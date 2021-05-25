/* SPDX-FileCopyrightText: 2021 Mark Roberts */
/* SPDX-License-Identifier: MIT */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define CELLS_NUMBER    (6)

#define ADC_REF_V       (2.5)
#define ADC_RESOLUTION  (1024)

/**
 * Convert volts to the corresponding ADC value
 */
#define VOLT_TO_ADC(v)  (uint16_t)((v) / 2 / (ADC_REF_V / ADC_RESOLUTION))

/**
 * Initialize ADC
 */
void adc_init(void);

/**
 * Perform ADC conversion and store values
 */
void adc_process(void);

/**
 * Check if ADC data is available and can be fetched
 */
bool adc_available(void);

/**
 * Get ADC value of the specified cell
 */
uint16_t adc_get_cell(uint8_t cell_index);

/**
 * Check if button is pressed
 */
bool adc_is_button_pressed(void);

/**
 * Check if charger is plugged in
 */
bool adc_is_charger_connected(void);

/**
 * Check if neither charger nor button is active and input is in idle state
 */
bool adc_ctrl_idle(void);

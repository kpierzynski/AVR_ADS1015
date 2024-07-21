/**
 * @file ads1015.h
 * @author kpierzynski (kpierzynski@hotmail.com)
 * @brief Header file for ADS1015 - ADC ic
 * @version 1.0
 * @date 2024-07-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __ADS1015_H_
#define __ADS1015_H_

#include <avr/io.h>
#include "i2c.h"

#define USE_FLOAT

#define ADS1015_ADDRESS_ADDR_GND 0b1001000
#define ADS1015_ADDRESS_ADDR_VDD 0b1001001
#define ADS1015_ADDRESS_ADDR_SDA 0b1001010
#define ADS1015_ADDRESS_ADDR_SCL 0b1001011

#define ADS1015_ADDRESS ADS1015_ADDRESS_ADDR_GND

#define ADS1015_REG_CONVERSION 0b00
#define ADS1015_REG_CONFIG 0b01
#define ADS1015_REG_LO_THRESH 0b10
#define ADS1015_REG_HI_THRESH 0b11

#define ADS1015_DEFAULT_CONFIG 0x8583

#define ADS1015_REG_CONFIG_PGA 9
#define ADS1015_REG_CONFIG_PGA_LEN 3
#define ADS1015_GAIN_6V144 0b000
#define ADS1015_GAIN_4V096 0b001
#define ADS1015_GAIN_2V048 0b010
#define ADS1015_GAIN_1V024 0b011
#define ADS1015_GAIN_0V512 0b100
#define ADS1015_GAIN_0V256 0b101

#define ADS1015_REG_CONFIG_OS 15

#define ADS1015_REG_CONFIG_MUX 12
#define ADS1015_REG_CONFIG_MUX_LEN 3
#define ADS1015_MUX_DIFF_A0_A1 0b000
#define ADS1015_MUX_DIFF_A0_A3 0b001
#define ADS1015_MUX_DIFF_A1_A3 0b010
#define ADS1015_MUX_DIFF_A2_A3 0b011
#define ADS1015_MUX_SINGLE_A0 0b100
#define ADS1015_MUX_SINGLE_A1 0b101
#define ADS1015_MUX_SINGLE_A2 0b110
#define ADS1015_MUX_SINGLE_A3 0b111

#define ADS1015_REG_CONFIG_MODE 8
#define ADS1015_MODE_CONTINUOUS 0b0
#define ADS1015_MODE_SINGLE 0b1

#define ADS1015_REG_COMP_POL 3
#define ADS1015_COMP_POL_ACTIVE_LOW 0
#define ADS1015_COMP_POL_ACTIVE_HIGH 1

#define ADS1015_REG_CONFIG_DATA_RATE 5
#define ADS1015_REG_CONFIG_DATA_RATE_LEN 3
#define ADS1015_DATA_RATE_128_SPS 0b000
#define ADS1015_DATA_RATE_250_SPS 0b001
#define ADS1015_DATA_RATE_490_SPS 0b010
#define ADS1015_DATA_RATE_920_SPS 0b011
#define ADS1015_DATA_RATE_1600_SPS 0b100
#define ADS1015_DATA_RATE_2400_SPS 0b101
#define ADS1015_DATA_RATE_3300_SPS 0b110

int16_t ads1015_read_conversion();
#ifdef USE_FLOAT
float ads1015_read_conversion_f();
#endif

void ads1015_init();
void ads1015_start();

void ads1015_set_mux(uint8_t mux);
void ads1015_set_gain(uint8_t gain);
void ads1015_set_mode(uint8_t mode);
void ads1015_set_irq_polarity(uint8_t polarity);
void ads1015_set_data_rate(uint8_t data_rate);

void ads1015_enable_alarm_irq();

#endif
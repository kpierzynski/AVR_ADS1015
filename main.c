/**
 * @file main.c
 * @author kpierzynski (kpierzynski@hotmail.com)
 * @brief Main file that provides example usage for ADS1015 sensor
 * @version 1.0
 * @date 2024-07-21
 *
 * @copyright Copyright (c) 2024
 *
 */

/*!
 * \mainpage Driver code for AVR to control ADS1015 ADC sensor
 *
 * \section ADS1015
 *
 * This project involves developing a driver code for the AVR microcontroller to interface with the ADS1015 Analog-to-Digital Converter (ADC) sensor. The ADS1015 is a high-precision, 12-bit ADC with an I2C interface, allowing for easy communication with the AVR microcontroller. The driver code will enable the AVR to accurately read analog signals, convert them to digital values, and process the data for various applications. This project will include initializing the I2C communication, configuring the ADS1015 settings, and implementing functions to read the ADC values, ensuring reliable and efficient performance in embedded systems.
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "ads1015.h"
#include "i2c.h"
#include "uart.h"

volatile uint8_t conversion_ready = 0;

void uart_putd16(int16_t byte)
{
    if (byte < 0)
        uart_putc('-');
    uart_putc((byte / 10000) % 10 + '0');
    uart_putc((byte / 1000) % 10 + '0');
    uart_putc((byte / 100) % 10 + '0');
    uart_putc((byte / 10) % 10 + '0');
    uart_putc((byte / 1) % 10 + '0');
}

int main()
{
    _delay_ms(2000);

    EICRA |= (1 << ISC11) | (1 << ISC10);
    EIMSK |= (1 << INT1);

    i2c_init();
    ads1015_init();
    ads1015_set_mux(ADS1015_MUX_SINGLE_A0);
    ads1015_set_irq_polarity(ADS1015_COMP_POL_ACTIVE_HIGH);
    ads1015_set_gain(ADS1015_GAIN_1V024);
    ads1015_enable_alarm_irq();
    uart_init();

    sei();
    uart_puts("\r\nSTARTING...\r\n");

    _delay_ms(100);

    ads1015_start();

    while (1)
    {
        if (conversion_ready)
        {
#ifndef USE_FLOAT
            int16_t result = ads1015_read_conversion();
            uart_putd16(result);
            uart_puts(" - IRQ\r\n");
#else
            float result = ads1015_read_conversion_f();
            char msg[64];
            // do not use stdio, it requires a lot of flash space.
            // i did because its just simple demo
            sprintf(msg, "Conversion: %f\r\n", result);
            uart_puts(msg);
#endif

            conversion_ready = 0;
            _delay_ms(500);
            ads1015_start();
        }
    }

    return -1;
}

ISR(INT1_vect)
{
    conversion_ready = 1;
}
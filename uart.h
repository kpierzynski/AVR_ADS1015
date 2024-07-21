/**
 * @file uart.h
 * @author kpierzynski (kpierzynski@hotmail.com)
 * @brief Header for UART driver.
 * @version 1.0
 * @date 2024-07-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

//================CONFIG================
/**
 * @brief Baud rate.
 *
 * It is used to calculate UBRR register in AVR that controls baud rate.
 * Note that not all baud rates are achivable at all core speeds.
 *
 */
#define BAUD 9600

/**
 * @brief Specify transmit buffer size.
 *
 * Note that for optimization reasons, tx_buff_size must be power of two.
 * This way saves some time on processing cyclic buffer.
 *
 */
#define TX_BUFF_SIZE 32
//======================================

#define UBRR ((F_CPU + BAUD * 8UL) / (16UL * BAUD) - 1)

#if TX_BUFF_SIZE & (TX_BUFF_SIZE - 1)
#error TX_BUFF_SIZE must be power of 2.
#endif

void uart_init();

void uart_putc(uint8_t byte);
void uart_puts(char *c);
void uart_puts_P(const char *s);

void uart_putd(uint8_t byte);
void uart_puth(uint8_t byte);
void uart_putb(uint8_t byte);

#endif /* UART_H_ */
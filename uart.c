/**
 * @file uart.c
 * @author kpierzynski (kpierzynski@hotmail.com)
 * @brief UART driver that implements transmit only functions
 * @version 1.0
 * @date 2024-07-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "uart.h"

/**
 * @brief Transmitting buffer. Size can be changed in header file.
 *
 */
volatile uint8_t tx_buff[TX_BUFF_SIZE];

/**
 * @brief Pointer to tail of a cyclic buffer
 *
 */
volatile uint8_t tx_tail;
/**
 * @brief Pointer to head of a cyclic buffer
 *
 */
volatile uint8_t tx_head;

/**
 * @brief UART initialization
 *
 */
void uart_init()
{
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // 8 - bit
	// UCSR0C &= ~( 1 << USBS0 ); //1 stop bit
	// UCSR0C &= ~( ( 1 << UPM00 ) | ( 1 << UPM01 ) ); //No parity

	// Set ubrr (desired baud)
	UBRR0L = (uint8_t)(UBRR);
	UBRR0H = (uint8_t)(UBRR >> 8);

	// Turn on transmitter hardware
	UCSR0B |= (1 << TXEN0);
}

/**
 * @brief Sends single byte/char to tx buffer
 *
 * @param byte Character/byte to send
 */
void uart_putc(uint8_t byte)
{
	uint8_t head = (tx_head + 1) & (TX_BUFF_SIZE - 1);

	while (head == tx_tail)
		;

	tx_buff[head] = byte;
	tx_head = head;

	UCSR0B |= (1 << UDRIE0);
}

/**
 * @brief Sends string via UART
 *
 * @param c Pointer to string
 */
void uart_puts(char *c)
{
	while (*c)
		uart_putc(*c++);
}

/**
 * @brief Sends string stored in prog (flash) memory via UART
 *
 * @param s Pointer to string. Use PSTR() macro.
 */
void uart_puts_P(const char *s)
{
	register char c;
	while ((c = pgm_read_byte(s++)))
		uart_putc(c);
}

/**
 * @brief Puts byte to UART in binary format
 *
 * @param byte Byte to send
 */
void uart_putb(uint8_t byte)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
	{
		if (byte & (1 << (8 - i - 1)))
			uart_putc('1');
		else
			uart_putc('0');
	}
}

/**
 * @brief Puts byte to UART in hex format
 *
 * @param byte Byte to send
 */
void uart_puth(uint8_t byte)
{
	char c = '0';

	c += ((byte >> 4) & 0x0F);
	if (c > '9')
		c = 'A' + (c - '9' - 1);
	uart_putc(c);

	c = '0';
	c += (byte & 0x0F);
	if (c > '9')
		c = 'A' + (c - '9' - 1);
	uart_putc(c);
}

/**
 * @brief Puts byte to UART in decimal format
 *
 * @param byte Byte to send
 */
void uart_putd(uint8_t byte)
{
	uart_putc((byte / 100) + '0');
	uart_putc((byte / 10) % 10 + '0');
	uart_putc((byte / 1) % 10 + '0');
}

/**
 * @brief Sender IRQ
 *
 * Function used to push data from tx cyclic buffer using UART data register
 *
 */
ISR(USART_UDRE_vect)
{
	uint8_t tail;

	if (tx_head != tx_tail)
	{
		tail = (tx_tail + 1) & (TX_BUFF_SIZE - 1);
		tx_tail = tail;
		UDR0 = tx_buff[tail];
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
	}
}
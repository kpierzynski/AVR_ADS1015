/**
 * @file i2c.c
 * @author kpierzynski (kpierzynski@hotmail.com)
 * @brief I2C hardware driver
 * @version 1.0
 * @date 2024-07-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "i2c.h"

/**
 * @brief Initialize i2c hardware
 *
 */
void i2c_init()
{
#if (F_CPU / I2C_F - 16) / 2 > 255
#error TWBR outside range
#endif
	TWBR = (F_CPU / I2C_F - 16) / 2;
}

/**
 * @brief Sends i2c start condition
 *
 * This also sends repeated start contition.
 *
 */
void i2c_start()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	while (!(TWCR & (1 << TWINT)))
		;
}

/**
 * @brief Sends i2c stop condition
 *
 */
void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	while (!(TWCR & (1 << TWSTO)))
		;
}

/**
 * @brief Sends one byte on i2c bus
 *
 * This function also sends slave address.
 * Note that address is 7bit number, so shift to left is needed. Then use I2C_READ or I2C_WRITE macro to select operation type.
 *
 * @param byte Byte to send
 */
void i2c_write(uint8_t byte)
{
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
}

/**
 * @brief Reads one byte from i2c bus
 *
 * @param ack Parameter that decive if master want keep receiving data or stop transaction. I2C_ACK and I2C_NACK macros are defined.
 * @return uint8_t Data received from slave
 */
uint8_t i2c_read(uint8_t ack)
{
	TWCR = (1 << TWINT) | ((ack ? 1 : 0) << TWEA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	return TWDR;
}
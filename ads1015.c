/**
 * @file ads1015.c
 * @author kpierzynski (kpierzynski@hotmail.com)
 * @brief Driver for ADS1015 - an ADC ic
 * @version 1.0
 * @date 2024-07-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ads1015.h"

/**
 * @brief Config of sensor, used to start single conversion.
 *
 */
uint16_t ads1015_config = ADS1015_DEFAULT_CONFIG;

/**
 * @brief Writes two-byte data to register in ads1015
 *
 * @param reg Register address
 * @param value Value to write
 */
void ads1015_write(uint8_t reg, uint16_t value)
{
    i2c_start();
    i2c_write(ADS1015_ADDRESS << 1 | I2C_WRITE);
    i2c_write(reg);
    i2c_write(value >> 8);
    i2c_write(value & 0xFF);
    i2c_stop();
}

/**
 * @brief Reads two-byte data from ads1015
 *
 * @param reg Register to read
 * @return uint16_t Value read from sensor.
 */
uint16_t ads1015_read(uint8_t reg)
{
    i2c_start();
    i2c_write(ADS1015_ADDRESS << 1 | I2C_WRITE);
    i2c_write(reg);
    i2c_start();
    i2c_write(ADS1015_ADDRESS << 1 | I2C_READ);
    uint16_t value = i2c_read(I2C_ACK) << 8;
    value |= i2c_read(I2C_NACK);
    i2c_stop();

    return value;
}

/**
 * @brief Reads conversion result
 *
 * This function handles scaling by gain factor automatically.
 *
 * @return int16_t Result of conversion in mV (mili-volt) unit.
 */
int16_t ads1015_read_conversion()
{
    // Register contains D11-D0 0000 values in two's complement.
    // Value sign should stay, so instead of shifting, division is used.
    int16_t value = ads1015_read(ADS1015_REG_CONVERSION) / 16;
    uint8_t gain = (ads1015_config >> ADS1015_REG_CONFIG_PGA) & ((1 << ADS1015_REG_CONFIG_PGA_LEN) - 1);

    // Scale value by gain modifier

    if (gain == 0)
        return value * 3;

    int8_t shift = 2 - gain;

    if (shift > 0)
        return (value << shift);
    else
        return (value >> -shift);
}

#ifdef USE_FLOAT
/**
 * @brief Reads conversion result
 *
 * @return float Conversion result in mV unit.
 */
float ads1015_read_conversion_f()
{
    int16_t raw = ads1015_read(ADS1015_REG_CONVERSION);
    float value = raw / 16;
    uint8_t gain = (ads1015_config >> ADS1015_REG_CONFIG_PGA) & ((1 << ADS1015_REG_CONFIG_PGA_LEN) - 1);

    switch (gain)
    {
    case 0:
        return value * 3;
    case 1:
        return value * 2;
    case 2:
        return value;
    case 3:
        return value / 2;
    case 4:
        return value / 4;
    case 5:
        return value / 8;
    }

    return value;
}
#endif

/**
 * @brief Initialize sensor. Resets config register and check if read properly.
 *
 */
void ads1015_init()
{
    ads1015_write(ADS1015_REG_CONFIG, ADS1015_DEFAULT_CONFIG & 0x7FFF);
    uint16_t config = ads1015_read(ADS1015_REG_CONFIG);

    if (config != ADS1015_DEFAULT_CONFIG)
        while (1)
            ;
}

/**
 * @brief Starts single conversion
 *
 */
void ads1015_start()
{
    ads1015_write(ADS1015_REG_CONFIG, ads1015_config | (1 << ADS1015_REG_CONFIG_OS));
}

/**
 * @brief Sets channel to read from
 *
 * @param mux Use one of prepared macros to select appropriate channel
 */
void ads1015_set_mux(uint8_t mux)
{
    ads1015_config &= ~(((1 << ADS1015_REG_CONFIG_MUX_LEN) - 1) << ADS1015_REG_CONFIG_MUX);
    ads1015_config |= (mux << ADS1015_REG_CONFIG_MUX);
}

/**
 * @brief Set gain of a sensor
 *
 * Gain represents maximum possible voltage that can be measured.
 * The smaller gain, the more accurate results
 *
 * @param gain Gain argument from provided macros. Default: ADS1015_GAIN_2V048
 */
void ads1015_set_gain(uint8_t gain)
{
    ads1015_config &= ~(((1 << ADS1015_REG_CONFIG_PGA_LEN) - 1) << ADS1015_REG_CONFIG_PGA);
    ads1015_config |= (gain << ADS1015_REG_CONFIG_PGA);
}

/**
 * @brief Sets operation mode.
 *
 * @param mode One of defined macros, select ADS1015_MODE_SINGLE or ADS1015_MODE_CONTINUOUS mode.
 */
void ads1015_set_mode(uint8_t mode)
{
    ads1015_config &= ~(1 << ADS1015_REG_CONFIG_MODE);
    ads1015_config |= (mode << ADS1015_REG_CONFIG_MODE);
}

/**
 * @brief Selects polarity of IRQ (alarm/rdy) pin/.
 *
 * @param polarity Select ADS1015_COMP_POL_ACTIVE_LOW or ADS1015_COMP_POL_ACTIVE_HIGH.
 */
void ads1015_set_irq_polarity(uint8_t polarity)
{
    ads1015_config &= ~(1 << ADS1015_REG_COMP_POL);
    ads1015_config |= (polarity << ADS1015_REG_COMP_POL);
}

/**
 * @brief Select data rate speed in continuous mode
 *
 * @param data_rate Select from defined macros.
 */
void ads1015_set_data_rate(uint8_t data_rate)
{
    ads1015_config &= ~(((1 << ADS1015_REG_CONFIG_DATA_RATE_LEN) - 1) << ADS1015_REG_CONFIG_DATA_RATE);
    ads1015_config |= (data_rate << ADS1015_REG_CONFIG_DATA_RATE);
}

/**
 * @brief Enables IRQ (alarm/rdy) pin.
 *
 */
void ads1015_enable_alarm_irq()
{
    ads1015_write(ADS1015_REG_HI_THRESH, 0xFFFF);
    ads1015_write(ADS1015_REG_LO_THRESH, 0x0000);

    ads1015_config &= ~0b11;
}
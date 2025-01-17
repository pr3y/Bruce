
/*
 * MIT License
 *
 * Copyright (c) 2017 David Antliff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file smbus.h
 * @brief Interface definitions for the ESP32-compatible SMBus Protocol component.
 *
 * This component provides structures and functions that are useful for communicating
 * with SMBus-compatible I2C slave devices.
 */

#ifndef __SMBUS__
#define __SMBUS__

#include "driver/i2c.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SMBUS_DEFAULT_TIMEOUT (1000 / portTICK_RATE_MS) ///< Default transaction timeout in ticks
    /**
     * @brief 7-bit or 10-bit I2C slave address.
     */
    typedef uint16_t i2c_address_t;

    /**
     * @brief Structure containing information related to the SMBus protocol.
     */
    typedef struct
    {
        bool          init;     ///< True if struct has been initialised, otherwise false
        i2c_port_t    i2c_port; ///< ESP-IDF I2C port number
        i2c_address_t address;  ///< I2C address of slave device
        portBASE_TYPE timeout;  ///< Number of ticks until I2C operation timeout
    } smbus_info_t;

    /**
     * @brief Construct a new SMBus info instance.
     *        New instance should be initialised before calling other functions.
     * @return Pointer to new device info instance, or NULL if it cannot be created.
     */
    smbus_info_t *smbus_malloc(void);

    /**
     * @brief Delete an existing SMBus info instance.
     * @param[in,out] smbus_info Pointer to SMBus info instance that will be freed and set to NULL.
     */
    void smbus_free(smbus_info_t **smbus_info);

    /**
     * @brief Initialise a SMBus info instance with the specified I2C information.
     *        The I2C timeout defaults to approximately 1 second.
     * @param[in] smbus_info Pointer to SMBus info instance.
     * @param[in] i2c_port I2C port to associate with this SMBus instance.
     * @param[in] address Address of I2C slave device.
     */
    esp_err_t smbus_init(smbus_info_t *smbus_info, i2c_port_t i2c_port, i2c_address_t address);

    /**
     * @brief Set the I2C timeout.
     *        I2C transactions that do not complete within this period are considered an error.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] timeout Number of ticks to wait until the transaction is considered in error.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_set_timeout(smbus_info_t *smbus_info, portBASE_TYPE timeout);

    /**
     * @brief Send a single bit to a slave device in the place of the read/write bit.
     *        May be used to simply turn a device function on or off, or enable or disable
     *        a low-power standby mode. There is no data sent or received.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] bit Data bit to send.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_quick(const smbus_info_t *smbus_info, bool bit);

    /**
     * @brief Send a single byte to a slave device.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] data Data byte to send to slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_send_byte(const smbus_info_t *smbus_info, uint8_t data);

    /**
     * @brief Receive a single byte from a slave device.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[out] data Data byte received from slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_receive_byte(const smbus_info_t *smbus_info, uint8_t *data);

    /**
     * @brief Write a single byte to a slave device with a command code.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[in] data Data byte to send to slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_write_byte(const smbus_info_t *smbus_info, uint8_t command, uint8_t data);

    /**
     * @brief Write a single word (two bytes) to a slave device with a command code.
     *        The least significant byte is transmitted first.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[in] data Data word to send to slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_write_word(const smbus_info_t *smbus_info, uint8_t command, uint16_t data);

    /**
     * @brief Read a single byte from a slave device with a command code.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[out] data Data byte received from slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_read_byte(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data);

    /**
     * @brief Read a single word (two bytes) from a slave device with a command code.
     *        The first byte received is the least significant byte.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[out] data Data byte received from slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_read_word(const smbus_info_t *smbus_info, uint8_t command, uint16_t *data);

    /**
     * @brief Write up to 255 bytes to a slave device with a command code.
     *        This uses a byte count to negotiate the length of the transaction.
     *        The first byte in the data array is transmitted first.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[in] data Data bytes to send to slave.
     * @param[in] len Number of bytes to send to slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_write_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, uint8_t len);

    /**
     * @brief Read up to 255 bytes from a slave device with a command code.
     *        This uses a byte count to negotiate the length of the transaction.
     *        The first byte received is placed in the first array location.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[out] data Data bytes received from slave.
     * @param[in/out] len Size of data array, and number of bytes actually received.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_read_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, uint8_t *len);

    /**
     * @brief Write bytes to a slave device with a command code.
     *        No byte count is used - the transaction lasts as long as the master requires.
     *        The first byte in the data array is transmitted first.
     *        This operation is not defined by the SMBus specification.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[in] data Data bytes to send to slave.
     * @param[in] len Number of bytes to send to slave.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_i2c_write_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, size_t len);

    /**
     * @brief Read bytes from a slave device with a command code (combined format).
     *        No byte count is used - the transaction lasts as long as the master requires.
     *        The first byte received is placed in the first array location.
     *        This operation is not defined by the SMBus specification.
     * @param[in] smbus_info Pointer to initialised SMBus info instance.
     * @param[in] command Device-specific command byte.
     * @param[out] data Data bytes received from slave.
     * @param[in/out] len Size of data array. If the slave fails to provide sufficient bytes, ESP_ERR_TIMEOUT will be returned.
     * @return ESP_OK if successful, ESP_FAIL or ESP_ERR_* if an error occurred.
     */
    esp_err_t smbus_i2c_read_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SMBUS__ */

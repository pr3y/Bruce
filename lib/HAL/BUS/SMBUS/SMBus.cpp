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
 * @file smbus.c
 *
 * SMBus diagrams are represented as a chain of "piped" symbols, using the following symbols:
 *
 *   - ADDR  : the 7-bit I2C address of a bus slave.
 *   - S     : the START condition sent by a bus master.
 *   - Sr    : the REPEATED START condition sent by a master.
 *   - P     : the STOP condition sent by a master.
 *   - Wr    : bit 0 of the address byte indicating a write operation. Value is 0.
 *   - Rd    : bit 0 of the address byte indicating a read operation. Value is 1.
 *   - R/W   : bit 0 of the address byte, indicating a read or write operation.
 *   - A     : ACKnowledge bit sent by a master.
 *   - N     : Not ACKnowledge bit set by a master.
 *   - As    : ACKnowledge bit sent by a slave.
 *   - Ns    : Not ACKnowledge bit set by a slave.
 *   - DATA  : data byte sent by a master.
 *   - DATAs : data byte sent by a slave.
 */

// TODO: add proper error checking around all i2c functions

#include "smbus.h"

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <inttypes.h>
#include <stddef.h>
#include <string.h>

static const char *TAG = "smbus";

#define WRITE_BIT     I2C_MASTER_WRITE
#define READ_BIT      I2C_MASTER_READ
#define ACK_CHECK     true
#define NO_ACK_CHECK  false
#define ACK_VALUE     (i2c_ack_type_t)0x0
#define NACK_VALUE    (i2c_ack_type_t)0x1
#define MAX_BLOCK_LEN 255 // SMBus v3.0 increases this from 32 to 255
// #define MEASURE             // enable measurement and reporting of I2C transaction duration

static bool _is_init(const smbus_info_t *smbus_info)
{
    bool ok = false;
    if (smbus_info != NULL)
    {
        if (smbus_info->init)
        {
            ok = true;
        }
        else
        {
            ESP_LOGE(TAG, "smbus_info is not initialised");
        }
    }
    else
    {
        ESP_LOGE(TAG, "smbus_info is NULL");
    }
    return ok;
}

static esp_err_t _check_i2c_error(esp_err_t err)
{
    switch (err)
    {
        case ESP_OK : // Success
            break;
        case ESP_ERR_INVALID_ARG : // Parameter error
            ESP_LOGE(TAG, "I2C parameter error");
            break;
        case ESP_FAIL : // Sending command error, slave doesn't ACK the transfer.
            ESP_LOGE(TAG, "I2C no slave ACK");
            break;
        case ESP_ERR_INVALID_STATE : // I2C driver not installed or not in master mode.
            ESP_LOGE(TAG, "I2C driver not installed or not master");
            break;
        case ESP_ERR_TIMEOUT : // Operation timeout because the bus is busy.
            ESP_LOGE(TAG, "I2C timeout");
            break;
        default :
            ESP_LOGE(TAG, "I2C error %d", err);
    }
    return err;
}

esp_err_t _write_bytes(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, size_t len)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | (DATA | As){*len} | P]
    esp_err_t err = ESP_FAIL;
    if (_is_init(smbus_info) && data)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | WRITE_BIT, ACK_CHECK);
        i2c_master_write_byte(cmd, command, ACK_CHECK);
        i2c_master_write(cmd, data, len, ACK_CHECK);
        i2c_master_stop(cmd);
#ifdef MEASURE
        uint64_t start_time = esp_timer_get_time();
#endif
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
#ifdef MEASURE
        ESP_LOGI(TAG, "_write_bytes: i2c_master_cmd_begin took %" PRIu64 " us", esp_timer_get_time() - start_time);
#endif
        i2c_cmd_link_delete(cmd);
    }
    return err;
}

esp_err_t _read_bytes(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, size_t len)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | Sr | ADDR | Rd | As | (DATAs | A){*len-1} | DATAs | N | P]
    esp_err_t err = ESP_FAIL;
    if (_is_init(smbus_info) && data)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | WRITE_BIT, ACK_CHECK);
        i2c_master_write_byte(cmd, command, ACK_CHECK);
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | READ_BIT, ACK_CHECK);
        if (len > 1)
        {
            i2c_master_read(cmd, data, len - 1, ACK_VALUE);
        }
        i2c_master_read_byte(cmd, &data[len - 1], NACK_VALUE);
        i2c_master_stop(cmd);
#ifdef MEASURE
        uint64_t start_time = esp_timer_get_time();
#endif
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
#ifdef MEASURE
        ESP_LOGI(TAG, "_read_bytes: i2c_master_cmd_begin took %" PRIu64 " us", esp_timer_get_time() - start_time);
#endif
        i2c_cmd_link_delete(cmd);
    }
    return err;
}

// Public API

smbus_info_t *smbus_malloc(void)
{
    smbus_info_t *smbus_info = (smbus_info_t*)malloc(sizeof(*smbus_info));
    if (smbus_info != NULL)
    {
        memset(smbus_info, 0, sizeof(*smbus_info));
        ESP_LOGD(TAG, "malloc smbus_info_t %p", smbus_info);
    }
    else
    {
        ESP_LOGE(TAG, "malloc smbus_info_t failed");
    }
    return smbus_info;
}

void smbus_free(smbus_info_t **smbus_info)
{
    if (smbus_info != NULL && (*smbus_info != NULL))
    {
        ESP_LOGD(TAG, "free smbus_info_t %p", *smbus_info);
        free(*smbus_info);
        *smbus_info = NULL;
    }
    else
    {
        ESP_LOGE(TAG, "free smbus_info_t failed");
    }
}

esp_err_t smbus_init(smbus_info_t *smbus_info, i2c_port_t i2c_port, i2c_address_t address)
{
    if (smbus_info != NULL)
    {
        smbus_info->i2c_port = i2c_port;
        smbus_info->address  = address;
        smbus_info->timeout  = SMBUS_DEFAULT_TIMEOUT;
        smbus_info->init     = true;
    }
    else
    {
        ESP_LOGE(TAG, "smbus_info is NULL");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t smbus_set_timeout(smbus_info_t *smbus_info, portBASE_TYPE timeout)
{
    esp_err_t err = ESP_FAIL;
    if (_is_init(smbus_info))
    {
        smbus_info->timeout = timeout;
        err                 = ESP_OK;
    }
    return err;
}

esp_err_t smbus_quick(const smbus_info_t *smbus_info, bool bit)
{
    // Protocol: [S | ADDR | R/W | As | P]
    esp_err_t err = ESP_FAIL;
    if (_is_init(smbus_info))
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | bit, ACK_CHECK);
        i2c_master_stop(cmd);
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
        i2c_cmd_link_delete(cmd);
    }
    return err;
}

esp_err_t smbus_send_byte(const smbus_info_t *smbus_info, uint8_t data)
{
    // Protocol: [S | ADDR | Wr | As | DATA | As | P]
    esp_err_t err = ESP_FAIL;
    if (_is_init(smbus_info))
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | WRITE_BIT, ACK_CHECK);
        i2c_master_write_byte(cmd, data, ACK_CHECK);
        i2c_master_stop(cmd);
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
        i2c_cmd_link_delete(cmd);
    }
    return err;
}

esp_err_t smbus_receive_byte(const smbus_info_t *smbus_info, uint8_t *data)
{
    // Protocol: [S | ADDR | Rd | As | DATAs | N | P]
    esp_err_t err = ESP_FAIL;
    if (_is_init(smbus_info))
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | READ_BIT, ACK_CHECK);
        i2c_master_read_byte(cmd, data, NACK_VALUE);
        i2c_master_stop(cmd);
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
        i2c_cmd_link_delete(cmd);
    }
    return err;
}

esp_err_t smbus_write_byte(const smbus_info_t *smbus_info, uint8_t command, uint8_t data)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | DATA | As | P]
    return _write_bytes(smbus_info, command, &data, 1);
}

esp_err_t smbus_write_word(const smbus_info_t *smbus_info, uint8_t command, uint16_t data)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | DATA-LOW | As | DATA-HIGH | As | P]
    uint8_t temp[2] = {data & 0xff, (data >> 8) & 0xff};
    return _write_bytes(smbus_info, command, temp, 2);
}

esp_err_t smbus_read_byte(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | Sr | ADDR | Rd | As | DATA | N | P]
    return _read_bytes(smbus_info, command, data, 1);
}

esp_err_t smbus_read_word(const smbus_info_t *smbus_info, uint8_t command, uint16_t *data)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | Sr | ADDR | Rd | As | DATA-LOW | A | DATA-HIGH | N | P]
    esp_err_t err     = ESP_FAIL;
    uint8_t   temp[2] = {0};
    if (data)
    {
        err = _read_bytes(smbus_info, command, temp, 2);
        if (err == ESP_OK)
        {
            *data = (temp[1] << 8) + temp[0];
        }
        else
        {
            *data = 0;
        }
    }
    return err;
}

esp_err_t smbus_write_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, uint8_t len)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | LEN | As | DATA-1 | As | DATA-2 | As ... | DATA-LEN | As | P]
    esp_err_t err = ESP_FAIL;
    if (_is_init(smbus_info) && data)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | WRITE_BIT, ACK_CHECK);
        i2c_master_write_byte(cmd, command, ACK_CHECK);
        i2c_master_write_byte(cmd, len, ACK_CHECK);
        for (size_t i = 0; i < len; ++i)
        {
            i2c_master_write_byte(cmd, data[i], ACK_CHECK);
        }
        i2c_master_stop(cmd);
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
        i2c_cmd_link_delete(cmd);
    }
    return err;
}

esp_err_t smbus_read_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, uint8_t *len)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | Sr | ADDR | Rd | As | LENs | A | DATA-1 | A | DATA-2 | A ... | DATA-LEN | N | P]
    esp_err_t err = ESP_FAIL;

    if (_is_init(smbus_info) && data && len)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | WRITE_BIT, ACK_CHECK);
        i2c_master_write_byte(cmd, command, ACK_CHECK);
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, smbus_info->address << 1 | READ_BIT, ACK_CHECK);
        uint8_t slave_len = 0;
        i2c_master_read_byte(cmd, &slave_len, ACK_VALUE);
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
        i2c_cmd_link_delete(cmd);

        if (err != ESP_OK)
        {
            *len = 0;
            return err;
        }

        if (slave_len > *len)
        {
            ESP_LOGW(TAG, "slave data length %d exceeds data len %d bytes", slave_len, *len);
            slave_len = *len;
        }

        cmd = i2c_cmd_link_create();
        for (size_t i = 0; i < slave_len - 1; ++i)
        {
            i2c_master_read_byte(cmd, &data[i], ACK_VALUE);
        }
        i2c_master_read_byte(cmd, &data[slave_len - 1], NACK_VALUE);
        i2c_master_stop(cmd);
        err = _check_i2c_error(i2c_master_cmd_begin(smbus_info->i2c_port, cmd, smbus_info->timeout));
        i2c_cmd_link_delete(cmd);

        if (err == ESP_OK)
        {
            *len = slave_len;
        }
        else
        {
            *len = 0;
        }
    }
    return err;
}

esp_err_t smbus_i2c_write_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, size_t len)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | (DATA | As){*len} | P]
    return _write_bytes(smbus_info, command, data, len);
}

esp_err_t smbus_i2c_read_block(const smbus_info_t *smbus_info, uint8_t command, uint8_t *data, size_t len)
{
    // Protocol: [S | ADDR | Wr | As | COMMAND | As | Sr | ADDR | Rd | As | (DATAs | A){*len-1} | DATAs | N | P]
    return _read_bytes(smbus_info, command, data, len);
}
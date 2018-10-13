/*
** File   : gsdk_uart.h
**
** Copyright (C) 2013-2018 Gosuncn. All rights reserved.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**      http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Author : qubo@gosuncn.cn
**
**  $Date: 2018/03/12 09:32:36GMT+08:00 $
**
*/

#ifndef _GSDK_I2C_H_
#define _GSDK_I2C_H_

typedef enum {
    //I2C_MASTER_0 = 0,                /**< I2C master 0. */
    I2C_MASTER_1 = 1,                /**< I2C master 1. */
    I2C_MASTER_2 = 2,                /**< I2C master 2. */
    I2C_MASTER_MAX                   /**< The total number of I2C masters (invalid I2C Master number). */
}i2c_port_t;

typedef enum {
    I2C_STATUS_ERROR = -4,                        /**<  An error occurred and the transaction has failed. */
    I2C_STATUS_ERROR_BUSY = -3,                   /**<  The I2C bus is busy, an error occurred. */
    I2C_STATUS_INVALID_PORT_NUMBER = -2,          /**<  A wrong port number is given. */
    I2C_STATUS_INVALID_PARAMETER = -1,            /**<  A wrong parameter is given. */
    I2C_STATUS_OK = 0                             /**<  No error occurred during the function call. */
}i2c_status_t;

/** @brief This enum defines the I2C bus status. */
typedef enum {
    I2C_STATUS_IDLE = 0,                         /**<  The I2C bus is idle. */
    I2C_STATUS_BUS_BUSY = 1                    /**<  The I2C bus is busy. */
}i2c_running_type_t;

/** @brief This enum defines the I2C transaction speed.  */
typedef enum {
    I2C_FREQUENCY_50K  = 0,          /**<  50kbps. */
    I2C_FREQUENCY_100K = 1,          /**<  100kbps. */
    I2C_FREQUENCY_200K = 2,          /**<  200kbps. */
    I2C_FREQUENCY_300K = 3,          /**<  300kbps. */
    I2C_FREQUENCY_400K = 4,          /**<  400kbps. */
    I2C_FREQUENCY_1M   = 5,          /**<  1mbps. */
    I2C_FREQUENCY_MAX                /**<  The total number of supported I2C frequencies (invalid I2C frequency).*/
}i2c_frequency_t;

typedef struct {
    i2c_frequency_t frequency;             /**<  The transfer speed. Please refer to #hal_i2c_frequency_t for speed definition. */
}i2c_config_t;

gsdk_status_t gsdk_i2c_master_init(i2c_port_t i2c_port, i2c_frequency_t i2c_freq, gsdk_handle_t *hi2c);
gsdk_status_t gsdk_i2c_write(gsdk_handle_t hi2c, uint8_t slave_addr, uint8_t *data, uint16_t len);
gsdk_status_t gsdk_i2c_read(gsdk_handle_t hi2c, uint8_t slave_addr, uint8_t *data, uint16_t len, int timeout_ms);
void gsdk_i2c_master_close(gsdk_handle_t hi2c);
#endif

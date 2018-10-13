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

#ifndef _HW_UART_H_
#define _HW_UART_H_

typedef enum {
    UART_0 = 0,                            /**< UART port 0. */
    UART_1 = 1,                            /**< UART port 1. */
    UART_2 = 2,                            /**< UART port 0. */
    UART_3 = 3,                            /**< UART port 1. */
    UART_MAX                               /**< The total number of UART ports (invalid UART port number). */
} uart_port_t;

/** @brief This enum defines baud rate of the UART frame. */
typedef enum {
    UART_BAUDRATE_110     = 0,          /**< Defines UART baudrate as 110 bps. */
    UART_BAUDRATE_300     = 1,          /**< Defines UART baudrate as 300 bps. */
    UART_BAUDRATE_1200    = 2,          /**< Defines UART baudrate as 1200 bps. */
    UART_BAUDRATE_2400    = 3,          /**< Defines UART baudrate as 2400 bps. */
    UART_BAUDRATE_4800    = 4,          /**< Defines UART baudrate as 4800 bps. */
    UART_BAUDRATE_9600    = 5,          /**< Defines UART baudrate as 9600 bps. */
    UART_BAUDRATE_19200   = 6,          /**< Defines UART baudrate as 19200 bps. */
    UART_BAUDRATE_38400   = 7,          /**< Defines UART baudrate as 38400 bps. */
    UART_BAUDRATE_57600   = 8,          /**< Defines UART baudrate as 57600 bps. */
    UART_BAUDRATE_115200  = 9,          /**< Defines UART baudrate as 115200 bps. */
    UART_BAUDRATE_230400  = 10,         /**< Defines UART baudrate as 230400 bps. */
    UART_BAUDRATE_460800  = 11,         /**< Defines UART baudrate as 460800 bps. */
    UART_BAUDRATE_921600  = 12,         /**< Defines UART baudrate as 921600 bps. */
    UART_BAUDRATE_MAX                   /**< Defines maximum enum value of UART baudrate. */
} uart_baudrate_t;


/** @brief This enum defines parity of the UART frame. */
typedef enum {
    UART_PARITY_NONE = 0,             /**< Defines UART parity as none. */
    UART_PARITY_ODD  = 1,             /**< Defines UART parity as odd. */
    UART_PARITY_EVEN = 2              /**< Defines UART parity as even. */
} uart_parity_t;

/** @brief This enum defines stop bit of the UART frame. */
typedef enum {
    UART_STOP_BIT_1 = 0,              /**< Defines UART stop bit as 1 bit per frame. */
    UART_STOP_BIT_2 = 1,              /**< Defines UART stop bit as 2 bits per frame. */
} uart_stop_bit_t;

/** @brief This enum defines word length of the UART frame. */
typedef enum {
    UART_WORD_LENGTH_5 = 0,           /**< Defines UART word length as 5 bits per frame. */
    UART_WORD_LENGTH_6 = 1,           /**< Defines UART word length as 6 bits per frame. */
    UART_WORD_LENGTH_7 = 2,           /**< Defines UART word length as 7 bits per frame. */
    UART_WORD_LENGTH_8 = 3            /**< Defines UART word length as 8 bits per frame. */
} uart_word_length_t;

/** @brief This struct defines UART configure parameters. */
typedef struct {
    uart_baudrate_t baudrate;              /**< This field represents the baudrate of the UART frame. */
    uart_word_length_t word_length;        /**< This field represents the word length of the UART frame. */
    uart_stop_bit_t stop_bit;              /**< This field represents the stop bit of the UART frame. */
    uart_parity_t parity;                  /**< This field represents the parity of the UART frame. */
} uart_config_t;

typedef enum {
    UART_IOCTL_SET_READ_MODE = 0,             /**< Write data command. */
    UART_IOCTL_MAX,                           /**< The maximum number of commands. */
} uart_ioctl_code_t;

typedef enum {
    UART_READ_MODE_START = 0,
    UART_READ_MODE_1BYTE_RETURN = 0,        /**< Return if more than 1 byte data is read */
    UART_READ_MODE_FRAME_RETURN,            /**< Return if a frame data is read */
    UART_READ_MODE_WAIT_FULL,                /**< Return when read buffer is full */
    UART_READ_MODE_MAX,
} uart_read_mode_t;

/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
 * @brief     This function initializes the UART hardware with basic functionality.
 * @param[in] uart_port   initializes the specified UART port number.
 * @param[in] uart_config   specifies configure parameters for UART hardware initialization.
 * @return
 *                #GSDK_SUCCESS if OK. \n
 */
gsdk_status_t gsdk_uart_open(uart_port_t port, uart_config_t *config, gsdk_handle_t *phuart);

/**
 * @brief     This function deinitializes the UART hardware to its default status.
 * @param[in] port   deinitializes the specified UART port number.
 * @return
 * @note
 *            user must call this function when they don't use this UART port.
 */
void gsdk_uart_close(gsdk_handle_t huart);

/**
 * @brief     This function receive data from UART port.
 *               User should check the returen value to make sure that data is received actually.
 * @param[in] port   initializes the specified UART port number.
 * @param[in] data   specifies pointer to the user's data buffer.
 * @param[in] len   specifies size of the user's data buffer.
 * @param[in] timeout_ms   operation timeout in milliseconds.
 * @return
 *                 bytes of transmitted data.
 * @note
 *            User must call gsdk_uart_open() before calling this function.
 */
int gsdk_uart_read(gsdk_handle_t huart, uint8_t *data, int len, int timeout_ms);

/**
 * @brief     This function sends user data to UART port.
 *               User should check the outcome to make sure that data is sent.
 * @param[in] port   initializes the specified UART port number.
 * @param[in] data   specifies pointer to the user's data buffer.
 * @param[in] len   specifies size of the user's data buffer.
 * @param[in] timeout_ms   operation timeout in milliseconds.
 * @return
 *                 bytes of transmitted data.
 * @note
 *            User must call gsdk_uart_open() before calling this function.
 */
int gsdk_uart_write(gsdk_handle_t huart, const uint8_t *data, int len, int timeout_ms);

/**
 * @brief     This function sends user data to UART port.
 *               User should check the outcome to make sure that data is sent.
 * @param[in] port   initializes the specified UART port number.
 * @param[in] data   specifies pointer to the user's data buffer.
 * @param[in] len   specifies size of the user's data buffer.
 * @param[in] timeout_ms   operation timeout in milliseconds.
 * @return
 *                 bytes of transmitted data.
 * @note
 *            User must call gsdk_uart_open() before calling this function.
 */
int gsdk_uart_ioctl(gsdk_handle_t huart, uart_ioctl_code_t code, void *param, int param_len);

#endif

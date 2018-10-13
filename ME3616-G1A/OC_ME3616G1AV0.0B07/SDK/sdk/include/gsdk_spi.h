/*
** File   : gsdk_spi.h
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
**  $Date: 2018/05/29 08:45:36GMT+08:00 $
**
*/

#ifndef _GSDK_SPI_H_
#define _GSDK_SPI_H_

/*****************************************************************************
* SPI master
*****************************************************************************/
/** @brief This enum defines the SPI master port.
 *  The chip supports total of 4 SPI master ports, each of them supports
 *  DMA mode. For more details about DMA mode, please refer to @ref
 *  SPI_MASTER_Features_Chapter.
 */

/** @brief SPI master status. */
typedef enum {
    SPI_MASTER_STATUS_ERROR              = -4,            /**< SPI master function error occurred. */
    SPI_MASTER_STATUS_ERROR_BUSY         = -3,            /**< SPI master is busy. */
    SPI_MASTER_STATUS_ERROR_PORT         = -2,            /**< SPI master invalid port. */
    SPI_MASTER_STATUS_INVALID_PARAMETER = -1,            /**< SPI master invalid input parameter. */
    SPI_MASTER_STATUS_OK                  = 0              /**< SPI master operation completed successfully. */
} spi_master_status_t;

/** @brief SPI master transaction bit order definition. */
typedef enum {
    SPI_MASTER_LSB_FIRST = 0,                              /**< Both send and receive data transfer LSB first. */
    SPI_MASTER_MSB_FIRST = 1                               /**< Both send and receive data transfer MSB first. */
} spi_master_bit_order_t;

/** @brief SPI master clock polarity definition. */
typedef enum {
    SPI_MASTER_CLOCK_POLARITY0 = 0,                       /**< Clock polarity is 0. */
    SPI_MASTER_CLOCK_POLARITY1 = 1                        /**< Clock polarity is 1. */
} spi_master_clock_polarity_t;

/** @brief SPI master clock format definition. */
typedef enum {
    SPI_MASTER_CLOCK_PHASE0 = 0,                          /**< Clock format is 0. */
    SPI_MASTER_CLOCK_PHASE1 = 1                           /**< Clock format is 1. */
} spi_master_clock_phase_t;

/** @brief SPI master CS mode definition. */
typedef enum {
    SPI_MASTER_CS_SPI_MODE = 0,
    SPI_MASTER_CS_GPIO_MODE = 1,
} spi_master_cs_mode_t;

/** @brief SPI master CS pin level in CS gpio mode definition. */
typedef enum {
    SPI_MASTER_CS_LO = 0,
    SPI_MASTER_CS_HI = 1,
} spi_master_cs_gpio_t;

/** @brief SPI master configuration structure. */
typedef struct {
    uint32_t clock_frequency;                               /**< SPI master clock frequency setting. */
    spi_master_bit_order_t bit_order;                       /**< SPI master bit order setting. */
    spi_master_clock_polarity_t polarity;                   /**< SPI master clock polarity setting. */
    spi_master_clock_phase_t phase;                         /**< SPI master clock phase setting. */
    spi_master_cs_mode_t cs_mode;
} spi_master_config_t;

/**
 * @brief     This function is mainly used to initialize the SPI master and set user defined common parameters like clock frequency,
 *            bit order, clock polarity, clock phase and default settings.
 * @param[in] spi_config is the SPI master configure parameters. Details are described at #spi_master_config_t.
 * @return    #GSDK_ERROR means function error;
 *            #GSDK_ERROR_DEVCIE_BUSY means this port of SPI master is busy and not available for use;
 *            #GSDK_ERROR_INSUFFICIENT_RESOURCE means master_port parameter is an invalid port number;
 *            #GSDK_ERROR_INVALID_PARAMETER means an invalid parameter is given by user;
 *            #GSDK_SUCCESS means this function return successfully.
 * @note      Please DO NOT call #gsdk_spi_open() in interrupt handler, it may cause deadlock.
 *            In a multi-task applications, if #gsdk_spi_open() returns error #HAL_SPI_MASTER_STATUS_ERROR_BUSY, it is suggested to
 *            call functions that release the CPU and try
 *            again later.
**/
gsdk_status_t gsdk_spi_open(gsdk_handle_t *pspi, spi_master_config_t *spi_config);

/**
 * @brief     This function is used to send or receive data asynchronously with DMA mode. This function returns immediately when timeout
 *            is 0. When rx_data equals NULL, this function is a sending data function.
 * @return    #GSDK_ERROR_INSUFFICIENT_RESOURCE, master_port parameter is an invalid port number;
 *            #GSDK_ERROR_DEVCIE_BUSY, the port of SPI master is busy and not available for use;
 *            #GSDK_ERROR_INVALID_PARAMETER, an invalid parameter in spi_send_and_receive_config is given by the user;
 *            #GSDK_SUCCESS, the operation completed successfully.
 */
gsdk_status_t gsdk_spi_transmit_and_receive(gsdk_handle_t pspi, uint8_t *tx_data, uint8_t *rx_data,
        int tx_size, int rx_size, int timeout);

/**
 * @brief     This function closes the SPI master device, gates its clock, disables interrupts.
 */
void gsdk_spi_close(gsdk_handle_t pspi);

#endif

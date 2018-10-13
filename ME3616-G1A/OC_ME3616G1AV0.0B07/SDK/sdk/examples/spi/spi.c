/*
** File   : spi.c
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

/* this example show how to use spi bus */

#include "gsdk_sdk.h"

#define FLOGD(fmt, ...)         printf(fmt, ##__VA_ARGS__)
#define FLOGI(fmt, ...)         printf(fmt, ##__VA_ARGS__)
#define FLOGE(fmt, ...)         printf(fmt, ##__VA_ARGS__)

#define FLASHIF_OK     0
#define FLASHIF_ERROR -1

#define TEST_ADDR 0x20000

/* Max falsh cmd length + page size */
#define FLASH_EXT_SECTOR_SIZE   (1024 * 4)
#define FLASH_EXT_BLOCK_SIZE    (1024 * 64)
#define FLASH_EXT_PAGE_SIZE     (256)
#define FLASH_EXT_CMD_SIZE      (5)
#define SPI_BUF_SIZE            (FLASH_EXT_CMD_SIZE + FLASH_EXT_PAGE_SIZE)

#define FLASH_SPI_TIMEOUT       (~0U)

static gsdk_handle_t pspi;
static gsdk_handle_t g_huart;

static int Flash_Spi_Init(void)
{
    gsdk_status_t status;
    spi_master_config_t spi_config;

    spi_config.bit_order       = SPI_MASTER_MSB_FIRST;
    spi_config.clock_frequency = 10000000;
    spi_config.phase           = SPI_MASTER_CLOCK_PHASE1;
    spi_config.polarity        = SPI_MASTER_CLOCK_POLARITY1;
    spi_config.cs_mode         = SPI_MASTER_CS_SPI_MODE;

    status = gsdk_spi_open(&pspi, &spi_config);
    if (status != GSDK_SUCCESS) {
        FLOGE("[SPI_DEMO] Wakeup in callback init failed\r\n");
        return FLASHIF_ERROR;
    }

    return FLASHIF_OK;
}

static uint32_t Flash_Spi_Xfer(const void *tx_buf, unsigned int tx_len, void *rx_buf, unsigned int rx_len, int ms)
{
    uint32_t max_len = tx_len + rx_len;
    uint8_t spi_tx_buf[SPI_BUF_SIZE] = {0};
    uint8_t spi_rx_buf[SPI_BUF_SIZE];
    gsdk_status_t status;

    if (max_len > SPI_BUF_SIZE) {
        FLOGE("Flash spi buffer overflow!\r\n");
        goto _fail;
    }

    memcpy(spi_tx_buf, tx_buf, tx_len);

    FLOGD("Flash_Spi_Xfer: tx_len: %u, rx_len: %u\r\n", tx_len, rx_len);

    if (rx_buf == NULL) {
        status = gsdk_spi_transmit_and_receive(pspi, spi_tx_buf, NULL, tx_len, 0, ms);
    } else {
        status = gsdk_spi_transmit_and_receive(pspi, spi_tx_buf, spi_rx_buf, tx_len, rx_len + tx_len, ms);
    }

    if (status != GSDK_SUCCESS) {
        FLOGE("HAL_SPI_TransmitReceive error %d!\r\n", status);
        goto _fail;
    }

    if (rx_len) {
        memcpy(rx_buf, &spi_rx_buf[tx_len], rx_len);
    }

    return max_len;

_fail:
    return FLASH_SPI_TIMEOUT;
}

static int Flash_Ext_ReadStatus(uint8_t *status)
{
    static const uint8_t tx_buf[] = {0x05};
    uint8_t rx_buf[1];
    uint32_t ret;

    ret = Flash_Spi_Xfer(tx_buf, sizeof(tx_buf), rx_buf, sizeof(rx_buf), 1000);
    if (ret ==  FLASH_SPI_TIMEOUT) {
        FLOGE("Read status timeout!\r\n");
        return FLASHIF_ERROR;
    }

    if (status)
        *status = rx_buf[0];

    return FLASHIF_OK;
}

static int Flash_Ext_WriteStatus(uint8_t status)
{
    uint8_t tx_buf[2] = {0x01, 0};
    uint32_t ret;

    tx_buf[1] = status;
    ret = Flash_Spi_Xfer(tx_buf, sizeof(tx_buf), NULL, 0, 1000);
    if (ret ==  FLASH_SPI_TIMEOUT) {
        FLOGE("Write status timeout!\r\n");
        return FLASHIF_ERROR;
    }

    return FLASHIF_OK;
}

static int Flash_Ext_WriteEnable(int enable)
{
    uint8_t tx_buf[1];
    uint32_t ret;

    if (enable)
        tx_buf[0] = 0x06;
    else
        tx_buf[0] = 0x04;

    ret = Flash_Spi_Xfer(tx_buf, sizeof(tx_buf), NULL, 0, 1000);
    if (ret ==  FLASH_SPI_TIMEOUT) {
        FLOGE("Write enable timeout!\r\n");
        return FLASHIF_ERROR;
    }

    return FLASHIF_OK;
}

static int Flash_Ext_Program(uint32_t dst, const char * src, uint32_t len)
{
    uint8_t tx_buf[4 + FLASH_EXT_PAGE_SIZE];
    uint32_t data_len;
    uint32_t page_num;
    uint32_t ret;
    uint8_t status;
    uint32_t start_tick;

#if 0
    if (dst != ALIGN(dst, FLASH_EXT_PAGE_SIZE)) {
        LOGE("Flash alignment error 0x%08lx\r\n", dst);
        return FLASHIF_ERROR;
    }
#endif
    page_num = ALIGN(len, FLASH_EXT_PAGE_SIZE) / FLASH_EXT_PAGE_SIZE;

    FLOGD("ExtProg: 0x%08lx %ld %ld\r\n", dst, len, page_num);

    tx_buf[0] = 0x02;
    while (page_num--) {
        if (Flash_Ext_WriteEnable(1)) {
            return FLASHIF_ERROR;
        }
        tx_buf[1] = (dst >> 16) & 0xff;
        tx_buf[2] = (dst >> 8) & 0xff;
        tx_buf[3] = dst & 0xff;

        if (len >= FLASH_EXT_PAGE_SIZE) {
            data_len = FLASH_EXT_PAGE_SIZE;
        } else {
            data_len = len;
            memset(&tx_buf[4], 0xff, FLASH_EXT_PAGE_SIZE);
        }

        memcpy(&tx_buf[4], (void *)src, data_len);

        ret = Flash_Spi_Xfer(tx_buf, sizeof(tx_buf), NULL, 0, 1000);
        if (ret ==  FLASH_SPI_TIMEOUT) {
            return FLASHIF_ERROR;
        }

        start_tick = 0;
        do {
            ret = Flash_Ext_ReadStatus(&status);
            if (ret ==  FLASH_SPI_TIMEOUT) {
                return FLASHIF_ERROR;
            }
            vTaskDelay(1);
            if (start_tick > 10) {
                break;
            }
            start_tick++;
        } while (status & 0x01);

        src += data_len;
        dst += data_len;
    }

    return FLASHIF_OK;
}

static int Flash_Ext_Erase(uint32_t start, uint32_t size)
{
    uint32_t block_num, block_size;
    uint8_t tx_buf[4];
    uint32_t ret;
    uint8_t status;
    uint32_t start_tick;

    if (size < FLASH_EXT_BLOCK_SIZE) {
        block_size = FLASH_EXT_SECTOR_SIZE;
        tx_buf[0] = 0x20;
    } else {
        block_size = FLASH_EXT_BLOCK_SIZE;
        tx_buf[0] = 0x52;
    }
    start &= ~(block_size - 1);
    size = ALIGN(size, block_size);
    block_num = size / block_size;

    FLOGD("ExtErase: 0x%08lx 0x%08lx %ld\r\n", start, size, block_num);

    while (block_num--) {
        FLOGD("0x%08lx %ld\r\n", start, block_num);
        if (Flash_Ext_WriteEnable(1)) {
            return FLASHIF_ERROR;
        }
        tx_buf[1] = (start >> 16) & 0xff;
        tx_buf[2] = (start >> 8) & 0xff;
        tx_buf[3] = start & 0xff;
        ret = Flash_Spi_Xfer(tx_buf, sizeof(tx_buf), NULL, 0, 1000);
        if (ret ==  FLASH_SPI_TIMEOUT) {
            return FLASHIF_ERROR;
        }

        start_tick = 0;
        do {
            vTaskDelay(1);
            ret = Flash_Ext_ReadStatus(&status);
            if (ret ==  FLASH_SPI_TIMEOUT) {
                return FLASHIF_ERROR;
            }
            if (start_tick > 10) {
                break;
            }
            start_tick++;
        } while (status & 0x01);
        start += block_size;
    }

    return FLASHIF_OK;
}

static int Flash_Ext_Read(void *buf, uint32_t start, uint32_t len)
{
    uint8_t tx_buf[4];
    uint32_t ret;
    uint32_t rx_len, offset = 0;

    FLOGD("ExtRead: 0x%08lx %ld\r\n", start, len);

    tx_buf[0] = 0x03; /* read  */

    while (len) {
        rx_len = len > FLASH_EXT_PAGE_SIZE ? FLASH_EXT_PAGE_SIZE : len;
        tx_buf[1] = ((start + offset) >> 16) & 0xff;
        tx_buf[2] = ((start + offset) >> 8) & 0xff;
        tx_buf[3] = (start + offset) & 0xff;
        ret = Flash_Spi_Xfer(tx_buf, sizeof(tx_buf), (uint8_t *)buf + offset, rx_len, 1000);
        if (ret ==  FLASH_SPI_TIMEOUT) {
            return FLASHIF_ERROR;
        }
        len -= rx_len;
        offset += rx_len;
    }
    return FLASHIF_OK;
}

static int Flash_Ext_ReadID(void *id)
{
    static const uint8_t tx_buf[] = {0x9f};
    uint8_t rx_buf[4];
    uint32_t ret;

    ret = Flash_Spi_Xfer(tx_buf, sizeof(tx_buf), rx_buf, sizeof(rx_buf), 1000);
    if (ret ==  FLASH_SPI_TIMEOUT) {
        return FLASHIF_ERROR;
    }

    if (id) {
        memcpy(id, rx_buf, sizeof(rx_buf));
    }

    return FLASHIF_OK;
}

static void Flash_Ext_Init(void)
{
    uint8_t status;
    uint8_t flash_id[4] = {0};

    if (Flash_Spi_Init())
        goto _fail;

    if (Flash_Ext_ReadID(flash_id) != FLASHIF_OK) {
        goto _fail;
    }
    FLOGD("Ext flash ID: 0x%02x 0x%02x 0x%02x 0x%02x\r\n", flash_id[0], flash_id[1], flash_id[2], flash_id[3]);
    if (Flash_Ext_ReadStatus(&status))
        goto _fail;
    FLOGD("Ext flash status: 0x%02x\r\n", status);
    if (status != 0) {
        if (Flash_Ext_WriteStatus(0))
            goto _fail;
        if (Flash_Ext_ReadStatus(&status))
            goto _fail;
        FLOGD("Ext flash status: 0x%02x\r\n", status);
    }
    return;

_fail:
    FLOGE("External flash init failed!\r\n");
    return;
}

int __io_puts(char *data, int len)
{
    int ret = 0;
    if (g_huart) {
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

int log_init(void)
{
    uart_config_t uart_config;
    gsdk_status_t status;

    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if (status != GSDK_SUCCESS) {
        gsdk_syslog_printf("[OEM_DEMO]: failed to open uart %d\r\n", status);
        return -1;
    }

    printf("[OEM_DEMO] log init....OK\r\n");

    return 0;
}

static const char gsdk_test_data[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDhTCCAm2gAwIBAgIBBDANBgkqhkiG9w0BAQsFADA7MQswCQYDVQQGEwJOTDER\r\n"
    "MA8GA1UECgwIUG9sYXJTU0wxGTAXBgNVBAMMEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"
    "MTcwNTA1MTMwNzU5WhcNMjcwNTA2MTMwNzU5WjA8MQswCQYDVQQGEwJOTDERMA8G\r\n"
    "A1UECgwIUG9sYXJTU0wxGjAYBgNVBAMMEVBvbGFyU1NMIENsaWVudCAyMIIBIjAN\r\n"
    "BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyHTEzLn5tXnpRdkUYLB9u5Pyax6f\r\n"
    "M60Nj4o8VmXl3ETZzGaFB9X4J7BKNdBjngpuG7fa8H6r7gwQk4ZJGDTzqCrSV/Uu\r\n"
    "1C93KYRhTYJQj6eVSHD1bk2y1RPD0hrt5kPqQhTrdOrA7R/UV06p86jt0uDBMHEw\r\n"
    "MjDV0/YI0FZPRo7yX/k9Z5GIMC5Cst99++UMd//sMcB4j7/Cf8qtbCHWjdmLao5v\r\n"
    "4Jv4EFbMs44TFeY0BGbH7vk2DmqV9gmaBmf0ZXH4yqSxJeD+PIs1BGe64E92hfx/\r\n"
    "/DZrtenNLQNiTrM9AM+vdqBpVoNq0qjU51Bx5rU2BXcFbXvI5MT9TNUhXwIDAQAB\r\n"
    "o4GSMIGPMB0GA1UdDgQWBBRxoQBzckAvVHZeM/xSj7zx3WtGITBjBgNVHSMEXDBa\r\n"
    "gBS0WuSls97SUva51aaVD+s+vMf9/6E/pD0wOzELMAkGA1UEBhMCTkwxETAPBgNV\r\n"
    "BAoMCFBvbGFyU1NMMRkwFwYDVQQDDBBQb2xhclNTTCBUZXN0IENBggEAMAkGA1Ud\r\n"
    "EwQCMAAwDQYJKoZIhvcNAQELBQADggEBAC7yO786NvcHpK8UovKIG9cB32oSQQom\r\n"
    "LoR0eHDRzdqEkoq7yGZufHFiRAAzbMqJfogRtxlrWAeB4y/jGaMBV25IbFOIcH2W\r\n"
    "iCEaMMbG+VQLKNvuC63kmw/Zewc9ThM6Pa1Hcy0axT0faf1B/U01j0FIcw/6mTfK\r\n"
    "D8w48OIwc1yr0JtutCVjig5DC0yznGMt32RyseOLcUe+lfq005v2PAiCozr5X8rE\r\n"
    "ofGZpiM2NqRPePgYy+Vc75Zk28xkRQq1ncprgQb3S4vTsZdScpM9hLf+eMlrgqlj\r\n"
    "c5PLSkXBeLE5+fedkyfTaLxxQlgCpuoOhKBm04/R1pWNzUHyqagjO9Q=\r\n"
    "-----END CERTIFICATE-----\r\n";

/*************************************************************************
  * first: initialise spi controller, read spi flash ID and status
  * second: erase spi flash
  * write: write the gsdk_test_data to spi flash
  * read: read data from spi flash
  * compare: compare gsdk_test_data with the data read from spi flash
  ***********************************************************************/
int main(void)
{
    int  ret;
    int  buf_size = sizeof(gsdk_test_data);
    char buf[buf_size];

    log_init();

    printf("Flash_Ext_Init\r\n");

    Flash_Ext_Init();

    printf("Flash_Ext_Erase\r\n");

    ret = Flash_Ext_Erase(TEST_ADDR, buf_size);
    if (ret != FLASHIF_OK) {
        printf("Flash_Ext_Erase Failed.\r\n");
        return -1;
    }

    printf("Flash_Ext_Program\r\n");

    ret = Flash_Ext_Program(TEST_ADDR, gsdk_test_data, buf_size);
    if (ret != FLASHIF_OK) {
        printf("Flash_Ext_Program Failed.\r\n");
        return -1;
    }

    printf("Flash_Ext_Read\r\n");

    ret = Flash_Ext_Read(buf, TEST_ADDR, buf_size);
    if (ret != FLASHIF_OK) {
        printf("Flash_Ext_Read Failed.\r\n");
        return -1;
    }

    ret = strcmp(gsdk_test_data, buf);
    if (!ret) {
        printf("Flash Test OK!\r\n");
    } else {
        printf("Flash Test Failed: %d!\r\n", ret);
    }
    return 0;
}

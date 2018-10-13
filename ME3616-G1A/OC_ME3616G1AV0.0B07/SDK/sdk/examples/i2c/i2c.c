/** File   : i2c.c
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
** Author : zhangxin2@gosuncn.cn
**
**  $Date: 2018/09/04 08:45:36GMT+08:00 $
**
*/

/*
 * This example shows how to use i2c bus to control the bmp180 sensor.
 */
 
#include "gsdk_sdk.h"

typedef unsigned char       UB8;
typedef unsigned short      UW16;
typedef unsigned long       UL32;
typedef char                SB8;
typedef short               SW16;
typedef long                SL32;

#define BMP180_NOT_EXISTENCE                0
#define BMP180_EXISTENCE                    1
#define BMP180_ID_REGISTER_ADDRESS          0xD0
#define BMP180_DEVICE_ADDRESS_BASE_VALUE    0x77
#define BMP180_DEVICE_ID                    0x55
#define MAX_WRITE_LENGTH                    4
#define OSS                                 2


#define BMP180_DIG_AC1_MSB_REG  0xAA
#define BMP180_DIG_AC1_LSB_REG  0xAB
#define BMP180_DIG_AC2_MSB_REG  0xAC
#define BMP180_DIG_AC2_LSB_REG  0XAD
#define BMP180_DIG_AC3_MSB_REG  0xAE
#define BMP180_DIG_AC3_LSB_REG  0xAF
#define BMP180_DIG_AC4_MSB_REG  0xB0
#define BMP180_DIG_AC4_LSB_REG  0xB1
#define BMP180_DIG_AC5_MSB_REG  0xB2
#define BMP180_DIG_AC5_LSB_REG  0xB3
#define BMP180_DIG_AC6_MSB_REG  0xB4
#define BMP180_DIG_AC6_LSB_REG  0xB5
#define BMP180_DIG_B1_MSB_REG   0xB6
#define BMP180_DIG_B1_LSB_REG   0xB7
#define BMP180_DIG_B2_MSB_REG   0xB8
#define BMP180_DIG_B2_LSB_REG   0xB9
#define BMP180_DIG_MB_MSB_REG   0xBA
#define BMP180_DIG_MB_LSB_REG   0xBB
#define BMP180_DIG_MC_MSB_REG   0xBC
#define BMP180_DIG_MC_LSB_REG   0xBD
#define BMP180_DIG_MD_MSB_REG   0xBE
#define BMP180_DIG_MD_LSB_REG   0xBF

static gsdk_handle_t g_huart;
static gsdk_handle_t g_hi2c;
static uint8_t DEVICEID;
//static uint8_t bmp180_txbuffer[MAX_WRITE_LENGTH * 2];
static uint8_t bmp180_reg_addr;

typedef struct {

    SW16    AC1;
    SW16    AC2;
    SW16    AC3;
    UW16    AC4;
    UW16    AC5;
    UW16    AC6;
    SW16    B1;
    SW16    B2;
    SW16    MB;
    SW16    MC;
    SW16    MD;

} BMP180_cal_param;

typedef struct {
    uint8_t ExistFlag;      //存在标志

    BMP180_cal_param cal_param; //修正系数

    SL32 UnsetTemperature;  //未校正的温度值
    SL32 UnsetGasPress; //未校正的气压值

    float Temperature;      //校正后的温度值
    SL32  GasPress;         //校正后的气压值
} BMP180_info;

int __io_puts(char *data, int len)
{
    int ret = 0;
    if (g_huart) {
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

static int bmp180_I2C_multi_read(uint8_t addr, uint8_t *result, uint16_t len)
{
    int ret;
    bmp180_reg_addr = addr;
    int timeout_ms = 5000;

    ret = gsdk_i2c_write(g_hi2c, BMP180_DEVICE_ADDRESS_BASE_VALUE, &bmp180_reg_addr, 1);
    if (ret != GSDK_SUCCESS) {
        gsdk_syslog_printf("I2C Transmit error (%ld)\n", ret);
        return -1;
    }

    ret = gsdk_i2c_read(g_hi2c, BMP180_DEVICE_ADDRESS_BASE_VALUE, result, len, timeout_ms);
    if (ret != GSDK_SUCCESS) {
        gsdk_syslog_printf("I2C Read error (%ld)\n", ret);
        return -1;
    }
    return ret;
}

/*static int bmp180_I2C_multi_write(uint8_t addr, uint8_t *value, uint16_t len)
{
    uint8_t reg_addr = addr;
    uint8_t reg_len  = len;
    uint8_t *ptxbuffer = bmp180_txbuffer;
    int ret;

    if (len > MAX_WRITE_LENGTH) {
        gsdk_syslog_printf("I2C Max Write length exceeds! \n");
        return -1;
    }

    while (reg_len != 0) {

        *ptxbuffer = reg_addr;
        reg_addr++;
        ptxbuffer++;

        *ptxbuffer = *value;
        value++;
        ptxbuffer++;

        reg_len--;
    }

    ret = gsdk_i2c_write(g_hi2c, BMP180_DEVICE_ADDRESS_BASE_VALUE, bmp180_txbuffer, (len * 2));

    if (ret != GSDK_SUCCESS) {
        gsdk_syslog_printf("I2C Transmit error(%ld)\n", ret);
        return -1;
    }
    return ret;
}*/

static SL32 BMP180ReadUnsetTemperature(void)
{
    uint8_t param[] = {0xF4, 0x2E};
    uint8_t *data;
	data = (uint8_t *)malloc(2);
    //bmp180_I2C_multi_write(BMP180_DEVICE_ADDRESS_BASE_VALUE,&param,2);
    gsdk_i2c_write(g_hi2c, BMP180_DEVICE_ADDRESS_BASE_VALUE, &param[0], 1);
    gsdk_i2c_write(g_hi2c, BMP180_DEVICE_ADDRESS_BASE_VALUE, &param[1], 1);
    vTaskDelay(50);
    bmp180_I2C_multi_read(0xF6, data, 2);
    SL32 temp = (data[0] << 8) | data[1];
	free(data);
    return temp;
}
static SL32 BMP180ReadUnsetPressure(void)
{
    uint8_t param[] = {0xF4, 0x34 + (OSS << 6)};
    uint8_t *data;
	data = (uint8_t *)malloc(3);
    //bmp180_I2C_multi_write(BMP180_DEVICE_ADDRESS_BASE_VALUE,&param,2);
    gsdk_i2c_write(g_hi2c, BMP180_DEVICE_ADDRESS_BASE_VALUE, &param[0], 1);
    gsdk_i2c_write(g_hi2c, BMP180_DEVICE_ADDRESS_BASE_VALUE, &param[1], 1);
    vTaskDelay(50);
    bmp180_I2C_multi_read(0xF6, data, 3);
    SL32 temp = ((data[0] << 16) + (data[1] << 8) + data[2]) >> (8 - OSS);
	free(data);
    return temp;
}

static void BMP180Convert(BMP180_info *temp)
{
    SL32 x1, x2, B5, B6, x3, B3, p;
    unsigned long b4, b7;

    temp->UnsetTemperature = BMP180ReadUnsetTemperature();
    temp->UnsetGasPress =  BMP180ReadUnsetPressure();

    gsdk_syslog_printf("UT:%d", temp->UnsetTemperature);
    gsdk_syslog_printf("UP:%d", temp->UnsetGasPress);

    x1 = ((temp->UnsetTemperature) - temp->cal_param.AC6) * (temp->cal_param.AC5) >> 15;
    x2 = ((SL32)(temp->cal_param.MC) << 11) / (x1 + temp->cal_param.MD);
    B5 = x1 + x2;
    temp->Temperature = ((B5 + 8) >> 4);

    B6 = B5 - 4000;
    x1 = ((SL32)(temp->cal_param.B2) * (B6 * B6 >> 12)) >> 11;
    x2 = ((SL32)temp->cal_param.AC2) * B6 >> 11;
    x3 = x1 + x2;
    B3 = ((((SL32)(temp->cal_param.AC1) * 4 + x3) << OSS) + 2) / 4;
    x1 = ((SL32)temp->cal_param.AC3) * B6 >> 13;
    x2 = ((SL32)(temp->cal_param.B1) * (B6 * B6 >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = ((SL32)(temp->cal_param.AC4) * (unsigned long)(x3 + 32768)) >> 15;
    b7 = ((unsigned long)(temp->UnsetGasPress) - B3) * (50000 >> OSS);
    if (b7 < 0x80000000) {
        p = (b7 * 2) / b4 ;
    } else {
        p = (b7 / b4) * 2;
    }
    x1 = (p >> 8) * (p >> 8);
    x1 = ((SL32)x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    temp->GasPress = p + ((x1 + x2 + 3791) >> 4);
}

static void BMP180ReadCalibrateParam(BMP180_info *p)
{

    uint8_t *bmp180_a_data_u8r;					//read the bmp180 calibration parameters
	bmp180_a_data_u8r = (uint8_t *)malloc(2);
	
    bmp180_I2C_multi_read(BMP180_DIG_AC1_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.AC1 = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_AC2_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.AC2 = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_AC3_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.AC3 = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_AC4_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.AC4 = (uint16_t)((((uint16_t)((unsigned char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_AC5_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.AC5 = (uint16_t)((((uint16_t)((unsigned char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_AC6_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.AC6 = (uint16_t)((((uint16_t)((unsigned char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_B1_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.B1  = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_B2_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.B2  = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_MB_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.MB  = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_MC_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.MC  = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);
	bmp180_I2C_multi_read(BMP180_DIG_MD_MSB_REG, bmp180_a_data_u8r, 2);
    p->cal_param.MD  = (int16_t)((((int16_t)((signed char)bmp180_a_data_u8r[0])) << 8) | bmp180_a_data_u8r[1]);

    gsdk_syslog_printf("The p->cal_param.AC1:%d\n", p->cal_param.AC1);
    gsdk_syslog_printf("The p->cal_param.AC2:%d\n", p->cal_param.AC2);
    gsdk_syslog_printf("The p->cal_param.AC3:%d\n", p->cal_param.AC3);
    gsdk_syslog_printf("The p->cal_param.AC4:%d\n", p->cal_param.AC4);
    gsdk_syslog_printf("The p->cal_param.AC5:%d\n", p->cal_param.AC5);
    gsdk_syslog_printf("The p->cal_param.AC6:%d\n", p->cal_param.AC6);
    gsdk_syslog_printf("The p->cal_param.B1:%d\n", p->cal_param.B1);
    gsdk_syslog_printf("The p->cal_param.B2:%d\n", p->cal_param.B2);
    gsdk_syslog_printf("The p->cal_param.MB:%d\n", p->cal_param.MB);
    gsdk_syslog_printf("The p->cal_param.MC:%d\n", p->cal_param.MC);
    gsdk_syslog_printf("The p->cal_param.MD:%d\n", p->cal_param.MD);
	
	free(bmp180_a_data_u8r);
}

static void BMP180_init(BMP180_info *p)
{
    bmp180_I2C_multi_read(BMP180_ID_REGISTER_ADDRESS, &DEVICEID, 1);  //read bmp180 device id by i2c bus
    if (DEVICEID == BMP180_DEVICE_ID) {
        gsdk_syslog_printf("The BMP180 device id 0x%x\n\r", DEVICEID);
        p->ExistFlag = BMP180_EXISTENCE;
        BMP180ReadCalibrateParam(p); 							//the bmp180 calibration parameters
    } else {
        p->ExistFlag = BMP180_NOT_EXISTENCE;
    }
}

static void display(BMP180_info temp)
{
    gsdk_syslog_printf("The Pressure : %4.2f Kpa\n\r", (float)(temp.GasPress) / 1000);
}

int main(void)
{
    uart_config_t uart_config;
    gsdk_status_t status;
    BMP180_info temp;

    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;
    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if (status != GSDK_SUCCESS) {
        gsdk_syslog_printf("[PRINTF_DEMO]: failed to open uart %d\n", status);
		return -1;
    }

    status = gsdk_i2c_master_init(I2C_MASTER_2, I2C_FREQUENCY_400K, &g_hi2c);//welinkopen only support i2c master 2 or i2c master 1
    if (status != GSDK_SUCCESS){
		gsdk_syslog_printf("[PRINTF_DEMO]: failed to open i2c %d\n", status);
        return -1;
    }

    BMP180_init(&temp);    //The bmp180 sensor init
    while (1) {
        if (temp.ExistFlag == BMP180_EXISTENCE) {
            BMP180Convert(&temp);		
            display(temp);				//display data
            break;
        }
    }
	
    gsdk_i2c_master_close(g_hi2c);		//clsoe i2c master
    gsdk_uart_close(g_huart);	//close uart
	g_hi2c = NULL;	
    g_huart = NULL;
	
    return 0;
}

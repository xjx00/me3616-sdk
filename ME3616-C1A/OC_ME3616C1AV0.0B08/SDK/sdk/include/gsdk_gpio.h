/*
** File   : gsdk_gpio.h
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
** Author : lixingyuan@gosuncn.cn
**
**  $Date: 2018/02/08 08:45:36GMT+08:00 $
**
*/

#ifndef _GSDK_GPIO_H_
#define _GSDK_GPIO_H_

typedef enum {
    GPIO_PIN_0 = 0,                            /**< GPIO pin0. */
    GPIO_PIN_1 = 1,                            /**< GPIO pin1. */
    GPIO_PIN_2 = 2,                            /**< GPIO pin2. */
    GPIO_PIN_3 = 3,                            /**< GPIO pin3. */
    GPIO_PIN_4 = 4,                            /**< GPIO pin4. */
    GPIO_PIN_5 = 5,                            /**< GPIO pin5. */
    GPIO_PIN_6 = 6,                            /**< GPIO pin6. */
    GPIO_PIN_7 = 7,                            /**< GPIO pin7. */
    GPIO_PIN_8 = 8,                            /**< GPIO pin8. */
    GPIO_PIN_9 = 9,                            /**< GPIO pin9. */
    GPIO_PIN_10 = 10,                          /**< GPIO pin10. */
    GPIO_PIN_11 = 11,                          /**< GPIO pin11. */
    GPIO_PIN_12 = 12,                          /**< GPIO pin12. */
    GPIO_PIN_13 = 13,                          /**< GPIO pin13. */
    GPIO_PIN_14 = 14,                          /**< GPIO pin14. */
    GPIO_PIN_15 = 15,                          /**< GPIO pin15. */
    GPIO_PIN_16 = 16,                          /**< GPIO pin16. */
    GPIO_PIN_17 = 17,                          /**< GPIO pin17. */
    GPIO_PIN_18 = 18,                          /**< GPIO pin18. */
    GPIO_PIN_19 = 19,                          /**< GPIO pin19. */
    GPIO_PIN_20 = 20,                          /**< GPIO pin20. */
    GPIO_PIN_21 = 21,                          /**< GPIO pin21. */
    GPIO_PIN_22 = 22,                          /**< GPIO pin22. */
    GPIO_PIN_23 = 23,                          /**< GPIO pin23. */
    GPIO_PIN_24 = 24,                          /**< GPIO pin24. */
    GPIO_PIN_25 = 25,                          /**< GPIO pin25. */
    GPIO_PIN_26 = 26,                          /**< GPIO pin26. */
    GPIO_PIN_27 = 27,                          /**< GPIO pin27. */
    GPIO_PIN_28 = 28,                          /**< GPIO pin28. */
    GPIO_PIN_29 = 29,                          /**< GPIO pin29. */
    GPIO_PIN_30 = 30,                          /**< GPIO pin30. */
    GPIO_PIN_31 = 31,                          /**< GPIO pin31. */
    GPIO_PIN_32 = 32,                          /**< GPIO pin32. */
    GPIO_PIN_33 = 33,                          /**< GPIO pin33. */
    GPIO_PIN_34 = 34,                          /**< GPIO pin34. */
    GPIO_PIN_35 = 35,                          /**< GPIO pin35. */
    GPIO_PIN_36 = 36,                          /**< GPIO pin36. */
    GPIO_PIN_MAX                               /**< The total number of GPIO pins (invalid GPIO pin number). */
} gpio_pin_t;

/** @brief This enum defines the GPIO direction. */
typedef enum {
    GPIO_DIRECTION_INPUT  = 0,              /**<  GPIO input direction. */
    GPIO_DIRECTION_OUTPUT = 1,               /**<  GPIO output direction. */
    GPIO_DIRECTION_MAX
} gpio_direction_t;


/** @brief This enum defines the data type of GPIO. */
typedef enum {
    GPIO_DATA_LOW  = 0,                     /**<  GPIO data low. */
    GPIO_DATA_HIGH = 1,                      /**<  GPIO data high. */
    GPIO_DATA_MAX
} gpio_data_t;

/** @brief This enum defines the data type of GPIO. */
typedef enum {
    GPIO_PULL_NONE      = 0,
    GPIO_PULL_UP        = 1,                     /**<  GPIO data low. */
    GPIO_PULL_DOWN      = 2,                     /**<  GPIO data high. */
} gpio_pull_t;


/** @brief This emun defines the EINT trigger mode.  */
typedef enum {
    GPIO_INT_DISABLE       = 0,
    GPIO_INT_LEVEL_LOW     = 1,                 /**< Level and low trigger. */
    GPIO_INT_LEVEL_HIGH    = 2,                 /**< Level and high trigger. */
    GPIO_INT_EDGE_FALLING  = 3,                 /**< Edge and falling trigger. */
    GPIO_INT_EDGE_RISING   = 4,                 /**< Edge and rising trigger. */
    GPIO_INT_EDGE_FALLING_AND_RISING = 5,       /**< Edge and falling or rising trigger. */
    GPIO_INT_MAX
} gpio_int_t;

/** @defgroup hal_eint_typedef Typedef
  * @{
  */
/** @brief  This defines the callback function prototype.
 *          A callback function should be registered for every EINT in use.
 *          This function will be called after an EINT interrupt is triggered in the EINT ISR.
 *          For more details about the callback function, please refer to hal_eint_register_callback().
 *  @param [out] user_data is the parameter which is set manually using hal_eint_register_callback() function.
 */
typedef void (*gpio_callback_t)(void *user_data);

typedef struct {
    gpio_direction_t    direction;
    gpio_pull_t         pull;
    gpio_int_t          int_mode;          /**< INT trigger mode. */
    uint32_t            debounce_time;     /**< INT hardware debounce time in milliseconds. EINT debounce is disabled when the debounce time is set to zero. */
    gpio_callback_t     callback;
    void                *user_data;
} gpio_config_t;

typedef enum{
	GPIO_DRIVING_CURRENT_4MA    = 0,        /**< Defines GPIO driving current as 4mA.  */
    GPIO_DRIVING_CURRENT_8MA    = 1,        /**< Defines GPIO driving current as 8mA.  */
    GPIO_DRIVING_CURRENT_12MA   = 2,        /**< Defines GPIO driving current as 12mA. */
    GPIO_DRIVING_CURRENT_16MA   = 3         /**< Defines GPIO driving current as 16mA. */
}gpio_driving_current_t;

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
gsdk_status_t gsdk_gpio_open(gpio_pin_t pin, gpio_config_t *config, gsdk_handle_t *pgpio);

/**
 * @brief     This function deinitializes the UART hardware to its default status.
 * @param[in] port   deinitializes the specified UART port number.
 * @return
 * @note
 *            user must call this function when they don't use this UART port.
 */
void gsdk_gpio_close(gsdk_handle_t hgpio);

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
gsdk_status_t gsdk_gpio_read(gsdk_handle_t hgpio, gpio_data_t *pdata);

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
gsdk_status_t gsdk_gpio_write(gsdk_handle_t hgpio, gpio_data_t data);

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
gsdk_status_t gsdk_gpio_toggle(gsdk_handle_t hgpio);

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
gsdk_status_t gsdk_gpio_interrupt_enable(gsdk_handle_t hgpio, int enable);

gsdk_status_t gsdk_gpio_get_driving_current(gsdk_handle_t hgpio, gpio_pin_t pin, gpio_driving_current_t * driving);

gsdk_status_t gsdk_gpio_set_driving_current(gsdk_handle_t hgpio, gpio_pin_t pin, gpio_driving_current_t driving);

gsdk_status_t gsdk_gpio_set_high_impedance(gpio_pin_t pin);

gsdk_status_t gsdk_gpio_clear_high_impedance(gpio_pin_t pin);

gsdk_status_t gsdk_gpio_set_pinmux(gpio_pin_t pin, uint8_t function_index );

#endif


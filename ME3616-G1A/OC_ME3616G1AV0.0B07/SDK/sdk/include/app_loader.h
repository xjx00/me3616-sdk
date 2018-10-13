/*
** File   : welinkopen_loader.h
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

#ifndef _APP_LOADER_H_
#define _APP_LOADER_H_

#define OEM_APP_MAGIC   0x5050412e
#define DEFAULT_GPIO_MAX_NUM  	18

#define IS_CHANGE_OFFSET        0
#define PIN_NUM_OFFSET          1
#define FUNCTION_MODE_OFFSET    2
#define DIRECTION_OFFSET        3
#define PULL_OFFSET             4
#define DATA_OFFSET             5
#define OTHER_PIN_OFFSET        6

/*function mode*/
#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3
#define MODE_4 4
#define MODE_5 5


typedef enum{
    gpio_pin_0 = 0,                            /**< GPIO pin0. */
    gpio_pin_1 = 1,                            /**< GPIO pin1. */
    gpio_pin_2 = 2,                            /**< GPIO pin2. */
    gpio_pin_3 = 3,                            /**< GPIO pin3. */
    gpio_pin_4 = 4,                            /**< GPIO pin4. */
    gpio_pin_5 = 5,                            /**< GPIO pin5. */
    gpio_pin_6 = 6,                            /**< GPIO pin6. */
    gpio_pin_7 = 7,                            /**< GPIO pin7. */
    gpio_pin_8 = 8,                            /**< GPIO pin8. */
    gpio_pin_9 = 9,                            /**< GPIO pin9. */
    gpio_pin_10 = 10,                          /**< GPIO pin10. */
    gpio_pin_11 = 11,                          /**< GPIO pin11. */
    gpio_pin_12 = 12,                          /**< GPIO pin12. */
    gpio_pin_13 = 13,                          /**< GPIO pin13. */
    gpio_pin_14 = 14,                          /**< GPIO pin14. */
    gpio_pin_15 = 15,                          /**< GPIO pin15. */
    gpio_pin_16 = 16,                          /**< GPIO pin16. */
    gpio_pin_17 = 17,                          /**< GPIO pin17. */
    gpio_pin_18 = 18,                          /**< GPIO pin18. */
    gpio_pin_19 = 19,                          /**< GPIO pin19. */
    gpio_pin_20 = 20,                          /**< GPIO pin20. */
    gpio_pin_21 = 21,                          /**< GPIO pin21. */
    gpio_pin_22 = 22,                          /**< GPIO pin22. */
    gpio_pin_23 = 23,                          /**< GPIO pin23. */
    gpio_pin_24 = 24,                          /**< GPIO pin24. */
    gpio_pin_25 = 25,                          /**< GPIO pin25. */
    gpio_pin_26 = 26,                          /**< GPIO pin26. */
    gpio_pin_27 = 27,                          /**< GPIO pin27. */
    gpio_pin_28 = 28,                          /**< GPIO pin28. */
    gpio_pin_29 = 29,                          /**< GPIO pin29. */
    gpio_pin_30 = 30,                          /**< GPIO pin30. */
    gpio_pin_31 = 31,                          /**< GPIO pin31. */
    gpio_pin_32 = 32,                          /**< GPIO pin32. */
    gpio_pin_33 = 33,                          /**< GPIO pin33. */
    gpio_pin_34 = 34,                          /**< GPIO pin34. */
    gpio_pin_35 = 35,                          /**< GPIO pin35. */
    gpio_pin_36 = 36,                          /**< GPIO pin36. */
    gpio_pin_MAX                               /**< The total number of GPIO pins (invalid GPIO pin number). */
}default_gpio_pin_t;

typedef enum{
	gpio_direction_input  = 0,
	gpio_direction_output = 1,
	gpio_direction_none
	
}default_gpio_direction_t;

typedef enum{
	gpio_pull_none 	= 0,
	gpio_pull_up 	= 1,
	gpio_pull_down 	= 2,

}default_gpio_pull_t;

typedef enum{
	gpio_data_low  = 0,
	gpio_data_high = 1,
	gpio_data_none

}default_gpio_data_t;

typedef struct default_gpio_config{
	uint8_t 					is_changed;
	default_gpio_pin_t 			gpio_pin;
	uint8_t 					function_index;
	default_gpio_direction_t 	direction;
	default_gpio_pull_t			pull;
	default_gpio_data_t 		output;
}default_gpio_config_t;

typedef struct app_preamble {
    uint32_t    magic;
    uint32_t    checksum;
    uint32_t    preamble_limit;
    uint32_t    load_address;
    uint32_t    api_version;
    uint32_t    app_properties;
    uint32_t    app_version;
    uint32_t    app_id;
    uint32_t    app_limit;
    void        *main_entry;
    uint32_t    main_priority;
    uint32_t    main_stack_base;
    uint32_t    main_stack_size;
    void        *callback_entry;
    uint32_t    callback_priority;
    uint32_t    callback_stack_base;
    uint32_t    callback_stack_size;
    uint32_t 	change_default_gpio;
    uint32_t    reserved2;
    uint32_t    reserved3;
} app_preamble_t;

typedef struct callback_struct {
    void (*func)(struct callback_struct *callback);
    void *context;
} callback_struct_t;

int app_queue_callback(callback_struct_t *callback, uint32_t ms_wait);

/* Warning: these definitions must be consistent with the definitions in freertos_stub.c */
#define TASK_LOCAL_STORAGE_NUM      6
#define TASK_LOCAL_STORAGE_MAGIC    0x544c5356

typedef struct task_local_storage {
    uint32_t magic;
    uint32_t size;
    void *pointers[TASK_LOCAL_STORAGE_NUM];
} task_local_storage_t;

uint32_t change_default_gpio_config(void);
#endif

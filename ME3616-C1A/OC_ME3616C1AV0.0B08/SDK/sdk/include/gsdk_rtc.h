/*
** File   : gsdk_rtc.h
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
**  $Date: 2018/02/08 08:45:36GMT+08:00 $
**
*/

#ifndef _GSDK_RTC_H_
#define _GSDK_RTC_H_

/** @brief RTC time structure definition. */
typedef struct {
    uint8_t rtc_sec;                                  /**< Seconds after minutes     - [0,59]  */
    uint8_t rtc_min;                                  /**< Minutes after the hour    - [0,59]  */
    uint8_t rtc_hour;                                 /**< Hours after midnight      - [0,23]  */
    uint8_t rtc_day;                                  /**< Day of the month          - [1,31]  */
    uint8_t rtc_mon;                                  /**< Months                    - [1,12]  */
    uint8_t rtc_week;                                 /**< Days in a week            - [0,6]   */
    uint8_t rtc_year;                                 /**< Years                     - [0,127] */
    uint16_t rtc_milli_sec;                           /**< Millisecond value, when in time API, this represents the read only register rtc_int_cnt - [0,65535]; when in alarm API, this represents millisecond alarm value - [0,15], equals to 0 to 937.5ms */
} rtc_time_t;

typedef void (*rtc_alarm_callback_t)(void *user_data);

/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
* @brief       This function open a rtc handle
* @param[in]   prtc is handle
* @return
* #GSDK_SUCCESS if OK.
*/
gsdk_status_t gsdk_rtc_open(gsdk_handle_t *prtc);

/**
 * @brief This function gets the RTC current time.
 * @param[out] time is a pointer to the #rtc_time_t structure to store the date and time settings received from the RTC current time.
 * @return #GSDK_SUCCESS, the operation completed successfully.
 */
gsdk_status_t gsdk_rtc_time_read(gsdk_handle_t prtc, rtc_time_t *ptime);

/**
 * @brief This function sets the RTC current time.
 * @param[in] time is a pointer to the #hal_rtc_time_t structure that contains the date and time settings for the RTC current time.
 * @return #GSDK_SUCCESS, the operation completed successfully.
 */
gsdk_status_t gsdk_rtc_time_write(gsdk_handle_t prtc, rtc_time_t *ptime);

/**
 * @brief This function sets the RTC alarm by second.
 * @param[in] second is the RTC alarm time.
 * @param[in] callback_function is the user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #GSDK_SUCCESS, the operation completed successfully.
 */
 #if 0
gsdk_status_t gsdk_rtc_sec_alarm_enable(gsdk_handle_t prtc, int second, rtc_alarm_callback_t callback_function, void *user_data);

/**
 * @brief This function sets the RTC alarm by calendar.
 * @param[in] time is a pointer to the hal_rtc_time_t structure that contains the date and time settings to configure the RTC alarm time.
 * @param[in] callback_function is the user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #GSDK_SUCCESS, the operation completed successfully.
 */
 #endif
gsdk_status_t gsdk_rtc_cal_alarm_enable(gsdk_handle_t prtc, rtc_time_t *ptime, rtc_alarm_callback_t callback_function, void *user_data);

/**
 * @brief This function disables an alarm. Call this function if the alarm notification is no longer required.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
gsdk_status_t gsdk_rtc_alarm_disable(gsdk_handle_t prtc);

/**
* @brief       This function close a rtc handle
* @param[in]   prtc is handle
*/
void gsdk_rtc_close(gsdk_handle_t prtc);

#endif


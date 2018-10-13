/*
** File   : gsdk_sl.h
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

#ifndef _GSDK_SL_H_
#define _GSDK_SL_H_

typedef enum {
    SLEEP_LOCK    = 1,
    SLEEP_UNLOCK  = 0,
} gsdk_lc_t;

typedef void (*wakeup_in_eint_callback_t)(void *user_data);

/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
 * @brief       This function sets up a sleeplock handle to control the sleep state of the system. \n
 *              This handle enables the Sleep Manager to identify the sleep states based on the sleep handles assigned.
 * @param[in]   handle_name is the name of the sleep handle of type string.
 * @param[in]   psl is handle
 * @return
 * #GSDK_SUCCESS if OK.
 */
gsdk_status_t gsdk_sleeplock_open(const char *handle_name, gsdk_handle_t *psl);

/**
 * @brief       This function prevents the MCU from getting into sleep or deep sleep mode.
 * @param[in]   psl is handle
 * @return      #GSDK_SUCCESS, Sleep Lock is locked.
 * @note        Call #gsdk_sleeplock_open() to set up the sleep handle, before calling this function.
 */
gsdk_status_t gsdk_sleeplock_acquire(gsdk_handle_t psl);

/**
 * @brief       This function unlocks the specific Sleep Lock and permits the MCU going into sleep mode when needed if all Sleep Locks are unlocked.
 * @param[in]   psl is handle
 * @return      #GSDK_SUCCESS, sleep lock is unlocked.
 * @note        Call #gsdk_sleeplock_open() to set up the sleep handle, before calling this function.
 */
gsdk_status_t gsdk_sleeplock_release(gsdk_handle_t psl);

/**
 * @brief       This function releases the sleep handle if it is no longer in use. \n
 * @param[in]   psl is handle
 */
void gsdk_sleeplock_close(gsdk_handle_t psl);

/**
 * @brief       This function checks if there are any Sleep Locks with specific level in the system.
 *              Apply it before entering the sleep mode.
 * @param[in]   psl is handle
 * @return      True if any locks with specific level are on hold.
 */
gsdk_lc_t gsdk_sleep_islocked(void);

/**
 * @brief This function sets the eint callback. This callback is executed if the RTC EINT happens.
 * @param[in] pwk is handle
 * @param[in] callback_function is the user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #GSDK_SUCCESS, the operation completed successfully.
 */
gsdk_status_t gsdk_wakeup_in_open(gsdk_handle_t *pwk, wakeup_in_eint_callback_t user_callback, void *user_data);

/**
 * @brief       This function releases interrupt function if it is no longer in use. \n
 * @param[in]   pwk is handle
 */
void gsdk_wakeup_in_close(gsdk_handle_t *pwk);



#endif


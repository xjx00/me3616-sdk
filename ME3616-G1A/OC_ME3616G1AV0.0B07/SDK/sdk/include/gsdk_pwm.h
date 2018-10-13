#ifndef _GSDK_PWM_H_
#define _GSDK_PWM_H_

/** @brief This enum defines the PWM channel number. */
typedef enum{
    PWM_CHANNEL_0 = 0,
    PWM_CHANNEL_1 = 1,
    PWM_CHANNEL_2 = 2,
    PWM_CHANNEL_3 = 3,
    PWM_CHANNEL_MAX
}pwm_channel_t;

/** @brief This enum defines the clock source for PWM module. */
typedef enum{
    PWM_CLOCK_13MHZ = 0,
    PWM_CLOCK_32KHZ = 1,
    PWM_CLOCK_MAX
}pwm_source_clock_t;

/** @brief This structure defines the basic parameters to configure PWM. */
typedef struct{
    uint32_t duty_ratio;
    uint32_t frequency;
    pwm_source_clock_t src_clock;
    gpio_pin_t pwm_pin;
}pwm_config_t;

/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
 * @brief     This function retrieves the duty cycle of specified pwm channel.
 * @param[in]  hpwm         is the handle for opened pwm channel.
 * @param[out] duty_cycle   where the obtained value would be stored.
 * @return
 *                #GSDK_SUCCESS if OK. \n
 */
gsdk_status_t gsdk_pwm_get_duty_cycle(gsdk_handle_t *hpwm, uint32_t *duty_cycle);

/**
 * @brief     This function initializes the UART hardware with basic functionality.
 * @param[in] hpwm         is the handle for opened pwm channel.
 * @param[out] frequency   where the obtained value of frequency would be stored.
 * @return
 *                #GSDK_SUCCESS if OK. \n
 */
 gsdk_status_t gsdk_pwm_get_frequency(gsdk_handle_t *hpwm, uint32_t *frequency);

/**
 * @brief     This function initializes the UART hardware with basic functionality.
 * @param[in] channel  specifies the explicit pwm channel to be used.
 * @param[in] config   is the basic parameters needed to configure pwm channel.
 * @param[in] hpwm     is the handle for opened pwm channel.
 * @return
 *                #GSDK_SUCCESS if OK. \n
 */
gsdk_status_t gsdk_pwm_open(pwm_channel_t channel, pwm_config_t *config, gsdk_handle_t *hpwm);

/**
 * @brief     This function makes initialized pwm channel start working..
 * @param[in] hpwm     is the handle for opened pwm channel.
 * @return
 *                #GSDK_SUCCESS if OK. \n
 */
gsdk_status_t gsdk_pwm_start(gsdk_handle_t hpwm);

/**
 * @brief     This function stops the pwm waveform.
 * @param[in] hpwm     is the handle for opened pwm channel.
 * @return
 *                #GSDK_SUCCESS if OK. \n
 */
gsdk_status_t gsdk_pwm_stop(gsdk_handle_t hpwm);

/**
 * @brief     This function close the pwm channel.
 * @param[in] hpwm     is the handle for opened pwm channel.
 * @return
 *                #None \n
 */
void gsdk_pwm_close(gsdk_handle_t hpwm);

#endif

#ifndef __LL_PWM_H__
#define __LL_PWM_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/**
 * @brief Start PWM of one channel.
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 */
void LL_PWM_Start(uint8_t pwm_chan_x);

/**
 * @brief Stop PWM of one channel
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 */
void LL_PWM_Stop(uint8_t pwm_chan_x);

/**
 * @brief PWM output invert, active high.
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 * @param en
 */
void LL_PWM_Invert_Set(uint8_t pwm_chan_x, uint8_t en);

/**
 * @brief PWM count mode settting.
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 * @param mode valid value is 0~3.
 * 0: count up, 0-->load, 0-->load, 0-->load
 * 1: count down, load-->0, load-->0, load-->0
 * 2/3: count up/down, 0-->load-->0-->load...
 */
void LL_PWM_CntMode_Set(uint8_t pwm_chan_x, uint8_t mode);

/**
 * @brief Set clock division of one channel.
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 * @param clk_div clock divide by 0:1, 1:2, ..., 63:64.
 */
void LL_PWM_Div_Set(uint8_t pwm_chan_x, uint8_t clk_div);

uint8_t LL_PWM_Div_Get(uint8_t pwm_chan_x);

/**
 * @brief Set count load for PWM. It cann't be set on the fly.
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 * @param load
 */
void LL_PWM_Load_Set(uint8_t pwm_chan_x, uint16_t load);

/**
 * @brief Set channel compersion value.
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 * @param cmp
 */
void LL_PWM_Compare_Set(uint8_t pwm_chan_x, uint16_t cmp);

/**
 * @brief Read count value of one channel.
 *
 * @param pwm_chan_x channel index, valid value is 0~11.
 * @return uint32_t
 */
uint32_t LL_PWM_Count_Read(uint8_t pwm_chan_x);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __LL_PWM_H__

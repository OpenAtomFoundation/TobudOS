#include "types.h"
#include "proj_config.h"
#include "ll/ll_pwm.h"
#include "hal/hal_pwm.h"
#include "hal/hal_common.h"

/**
 * @brief Start one PWM channel.
 *
 * @param chan
 */
void HAL_PWM_Start(pwm_channel_enum_t chan)
{
    assert_param(IS_PWM_CHAN(chan));

    LL_PWM_Start(chan);
}

/**
 * @brief Stop one PWM channel.
 *
 * @param chan
 */
void HAL_PWM_Stop(pwm_channel_enum_t chan)
{
    assert_param(IS_PWM_CHAN(chan));

    LL_PWM_Stop(chan);
}

/**
 * @brief Config frequency and duty cycle of one channel.
 *
 * @param chan
 * @param freq  100Hz <= freq <= 800KHz
 * @param duty 0~1.0
 */
void HAL_PWM_Config(pwm_channel_enum_t chan, uint32_t freq, float duty)
{
    assert_param(IS_PWM_CHAN(chan));

    uint8_t pwm_div = 0;
    LL_PWM_Div_Set(chan, pwm_div);

    uint32_t realfreq = APBUS0_CLOCK / (pwm_div+1);
    uint32_t load_val = realfreq / freq;
    uint32_t cmp_val = duty * load_val;

    LL_PWM_Load_Set(chan, load_val);
    LL_PWM_Compare_Set(chan, cmp_val);
}

/**
 * @brief Invert output polarity.
 * The default duty cycle is low, after invert, the output is high.
 * @param chan
 * @param en
 */
void HAL_PWM_Invert(pwm_channel_enum_t chan, uint8_t en)
{
    assert_param(IS_PWM_CHAN(chan));

    LL_PWM_Invert_Set(chan, en);
}

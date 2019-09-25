/**
 * @file    wm_pwm.c
 *
 * @brief   pwm driver module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include <string.h>

#include "wm_debug.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_pwm.h"
#include "wm_gpio.h"
#include "wm_cpu.h"
#include "tls_common.h"


typedef void (*pwm_irq_callback)(void);
static pwm_irq_callback pwm_callback;

void PWM_IRQHandler(void)
{
    if (pwm_callback)
        pwm_callback();
}

/**
 * @brief          This function is used to register the pwm interrupt callback function
 *
 * @param[in]      callback     the pwm interrupt callback function
 *
 * @return         None
 *
 * @note           None
 */
void tls_pwm_isr_register(void (*callback)(void))
{
    pwm_callback = callback;
    tls_irq_enable(PWM_INT);
}

/**
 * @brief          This function is used to set duty ratio
 *
 * @param[in]      channel    pwm channel NO.,range form 0 to 4
 * @param[in]      duty       Number of active levels
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_duty_config(u8 channel, u8 duty)
{
    u32 temp = 0;
    
	if(channel > (PWM_CHANNEL_MAX_NUM - 1))
	{
		TLS_DBGPRT_ERR("duty param err\n");
		return WM_FAILED;
	}

    if (4 == channel)
    {
        temp = tls_reg_read32(HR_PWM_CH4_REG2) & ~0x0000FF00;
        temp |= (duty << 8);
        tls_reg_write32(HR_PWM_CH4_REG2, temp);          /* duty radio */
    }
    else
    {
        temp = tls_reg_read32(HR_PWM_CMPDAT) & ~(0xFF << channel * 8);
        temp |= (duty << (channel * 8));
        tls_reg_write32(HR_PWM_CMPDAT, temp); /* duty radio */
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set frequency
 *
 * @param[in]      channel    pwm channel NO., range form 0 to 4
 * @param[in]      clkdiv     clock divider, range 0 to 65535
 * @param[in]      period     the number of the counting clock cycle
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_freq_config(u8 channel, u16 clkdiv, u8 period)
{
    u32 temp = 0;
    
	if(channel > (PWM_CHANNEL_MAX_NUM - 1))
	{
		TLS_DBGPRT_ERR("freq param err\n");
		return WM_FAILED;
	}

    if (4 == channel)
    {
        temp = tls_reg_read32(HR_PWM_CH4_REG1) & ~0xFFFF0000;
        temp |= (clkdiv << 16);
        tls_reg_write32(HR_PWM_CH4_REG1, temp);/* clock divider */

        temp = tls_reg_read32(HR_PWM_CH4_REG1) & ~0x0000FF00;
        temp |= (period << 8);
        tls_reg_write32(HR_PWM_CH4_REG1, temp); /* the number of the counting clock cycle */
    }
    else
    {
        temp = tls_reg_read32(HR_PWM_CLKDIV01 + (channel / 2) * 4) & ~(0xFFFF << ((channel % 2) * 16));
        temp |= (clkdiv << ((channel % 2) * 16));
        tls_reg_write32(HR_PWM_CLKDIV01 + (channel / 2) * 4, temp);/* clock divider */

        temp = tls_reg_read32(HR_PWM_PERIOD) & ~(0xFF << channel * 8);
        temp |= (period << (channel * 8));
        tls_reg_write32(HR_PWM_PERIOD, temp);/* the number of the counting clock cycle */
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set the output mode
 *
 * @param[in]      channel    pwm channel NO.,range form 0 to 4
 * @param[in]      mode       pwm work mode for signal generate
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_out_mode_config(u8 channel, enum tls_pwm_out_mode mode)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if (WM_PWM_OUT_MODE_BRAKE == mode)
        tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) | BIT(11 + channel));/* the brake mode */
    else if (WM_PWM_OUT_MODE_ALLSYC == mode)
    {
        if (channel != 0)
            return WM_FAILED;
        tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) & ~0xF800); /* disable the brake mode */
        tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) | BIT(6));       /* enable the all synchronous mode mode */
    }
    else if (WM_PWM_OUT_MODE_2SYC == mode)
    {
        if (channel != 0 && channel != 2)
            return WM_FAILED;
        tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) & ~(0x1800<<channel)); /* disable the brake mode */
        tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) & ~BIT(6));                 /* disable the all synchronous mode mode */
        tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) | BIT(14 + channel / 2));   /* enable the two channel synchronous mode */
    }
    else if (WM_PWM_OUT_MODE_MC == mode)
    {
        if (channel != 0 && channel != 2)
            return WM_FAILED;
        tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) & ~(0x1800<<channel));/* disable the brake mode */
        tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) & ~BIT(6));                /* disable the all synchronous mode mode */
        tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) & ~BIT(14 + channel / 2)); /* disable the two channel synchronous mode */
        tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) | BIT(0 + channel / 2));   /* enable the complementary mode */
    }
    else if(WM_PWM_OUT_MODE_INDPT == mode)
    {
        tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL)    & (~BIT(6)));
        if (channel != 4 )
        {
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL)    & (~BIT(0 + channel / 2)));
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL)    & (~BIT(14 + channel / 2)));
        }
        tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) & (~BIT(11 + channel)));    /* enable the independent mode */
    }
    else
        return WM_FAILED;

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set the counting mode
 *
 * @param[in]      channel     pwm channel NO.,range form 0 to 4
 * @param[in]      cnt_type    counting mode
 *
 * @retval         WM_SUCCESS  success
 * @retval         WM_FAILED   failed
 *
 * @note           None
 */
int tls_pwm_cnt_type_config(u8 channel, enum tls_pwm_cnt_type cnt_type)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if (4 == channel)
    {
        if (WM_PWM_CNT_TYPE_EDGE_ALLGN_CAP == cnt_type)
        {
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) & (~BIT(4)));
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) & (~BIT(3)));
        }
        if (WM_PWM_CNT_TYPE_EDGE_ALIGN_OUT == cnt_type)
        {
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) & (~BIT(4)));
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) |   BIT(3));
        }
        else if (WM_PWM_CNT_TYPE_CENTER_ALIGN == cnt_type)
        {
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) |   BIT(4));
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) & (~BIT(3)));
        }

    }
    else
    {
        if (WM_PWM_CNT_TYPE_EDGE_ALLGN_CAP == cnt_type && channel == 0)
        {
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) & (~BIT(17)));
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) & (~BIT(16)));
        }
        if (WM_PWM_CNT_TYPE_EDGE_ALIGN_OUT == cnt_type)
        {
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) & (~BIT(17 + channel * 2)));
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) |   BIT(16 + channel * 2));
        }
        else if (WM_PWM_CNT_TYPE_CENTER_ALIGN == cnt_type)
        {
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) |   BIT(17 + channel * 2));
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) & (~BIT(16 + channel * 2)));
        }
        else
            return WM_FAILED;
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set whether to loop
 *
 * @param[in]      channel      pwm channel NO.,range form 0 to 4
 * @param[in]      loop_mode    whether to loop
 *
 * @retval         WM_SUCCESS   success
 * @retval         WM_FAILED    failed
 *
 * @note           None
 */
int tls_pwm_loop_mode_config(u8 channel, enum tls_pwm_loop_type loop_mode)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if (4 == channel)
    {
        if (WM_PWM_LOOP_TYPE_LOOP == loop_mode)
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) |   BIT(1));
        else
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) & (~BIT(1)));
    }
    else
    {
        if (WM_PWM_LOOP_TYPE_LOOP == loop_mode)
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) |   BIT(8 + channel));
        else
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) & (~BIT(8 + channel)));
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set whether to inverse the output
 *
 * @param[in]      channel    pwm channel NO.,range form 0 to 4
 * @param[in]      en         ENABLE or DISABLE
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_out_inverse_cmd(u8 channel, bool en)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if (4 == channel)
    {
        if (ENABLE == en)
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) |   BIT(0));
        else
            tls_reg_write32(HR_PWM_CH4_REG2, tls_reg_read32(HR_PWM_CH4_REG2) & (~BIT(0)));
    }
    else
    {
        if (ENABLE == en)
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) |   BIT(2 + channel));
        else
            tls_reg_write32(HR_PWM_CTL,    tls_reg_read32(HR_PWM_CTL) & (~BIT(2 + channel)));
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set the number of period to be generated
 *
 * @param[in]      channel    pwm channel NO.,range form 0 to 4
 * @param[in]      pnum       the number of period to be generated,range from 0 to 255
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_stoptime_by_period_config(u8 channel, u8 pnum)
{
    u32 temp = 0;
    
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if (4 == channel)
    {
        temp = tls_reg_read32(HR_PWM_CH4_REG1) & ~0x000000FF;
        temp |= pnum;
        tls_reg_write32(HR_PWM_CH4_REG1, temp);
    }
    else
    {
        temp = tls_reg_read32(HR_PWM_PNUM) & ~(0xFF << channel * 8);
        temp |= (pnum << (channel * 8));
        tls_reg_write32(HR_PWM_PNUM, temp);
        
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set output enable
 *
 * @param[in]      channel    pwm channel NO.,channel 0 or channel 4
 * @param[in]      en         ENABLE or DISABLE
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_output_en_cmd(u8 channel, bool en)
{
    if(channel != 0 && channel != 4)
		return WM_FAILED;

    if (4 == channel)
    {
        if (ENABLE == en)
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) &   (~BIT(2)));
        else
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) |   BIT(2));
    }
    else
    {
        if (ENABLE == en)
            tls_reg_write32(HR_PWM_CTL, tls_reg_read32(HR_PWM_CTL) &   (~BIT(12)));
        else
            tls_reg_write32(HR_PWM_CTL, tls_reg_read32(HR_PWM_CTL) |   BIT(12));
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set the dead time
 *
 * @param[in]      channel     pwm channel NO.,channel 0 or channel 2
 * @param[in]      dten        whether enalbe the deat time, ENABLE or DISABLE
 * @param[in]      dtclkdiv    dead zone clock divider, range 0 to 3
 * @param[in]      dtcnt       the number of the counting clock cycle, range 0 to 255
 *
 * @retval         WM_SUCCESS  success
 * @retval         WM_FAILED   failed
 *
 * @note           None
 */
int tls_pwm_deadzone_config(u8 channel, bool dten, u8 dtclkdiv, u8 dtcnt)
{
    u32 temp = 0;
    
    if ((channel !=0 && channel != 2) || dtclkdiv > 3)
        return WM_FAILED;

    if(ENABLE == dten)
    {
        temp = tls_reg_read32(HR_PWM_DTCTL) & ~0x00030000;
        temp |= (dtclkdiv<<16);
        tls_reg_write32(HR_PWM_DTCTL, temp);/* dead zone clock divider */

        if (channel == 0 || channel == 1)
        {
            temp = tls_reg_read32(HR_PWM_DTCTL) & ~0x000000FF;
            temp |= dtcnt;
            tls_reg_write32(HR_PWM_DTCTL, temp);/* the number of the counting clock cycle */

            tls_reg_write32(HR_PWM_DTCTL, tls_reg_read32(HR_PWM_CTL) | BIT(20));       /* whether enalbe the deat time */

        }
        else if (channel == 2 || channel == 3)
        {
            temp = tls_reg_read32(HR_PWM_DTCTL) & ~0x0000FF00;
            temp |= (dtcnt<<8);
            tls_reg_write32(HR_PWM_DTCTL, temp);/* the number of the counting clock cycle */

            tls_reg_write32(HR_PWM_DTCTL, tls_reg_read32(HR_PWM_CTL) | BIT(21));       /* whether enalbe the deat time */
        }
    }
    else
    {
        if (channel == 0 || channel == 1)
        {
            tls_reg_write32(HR_PWM_DTCTL, tls_reg_read32(HR_PWM_CTL) & (~BIT(20)));    /* whether enalbe the deat time */
        }
        else if (channel == 2 || channel == 3)
        {
            tls_reg_write32(HR_PWM_DTCTL, tls_reg_read32(HR_PWM_CTL) & (~BIT(21)));    /* whether enalbe the deat time */
        }
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set whether to inverse the capture input
 *
 * @param[in]      channel    pwm channel NO.,channel 0 or channel 4
 * @param[in]      en         ENABLE or DISABLE
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_capture_inverse_cmd(u8 channel, bool en)
{
    if (channel != 0 && channel != 4)
        return WM_FAILED;

    if (channel == 0)
    {
        if (ENABLE == en)
            tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) | BIT(25));
        else
            tls_reg_write32(HR_PWM_CTL,  tls_reg_read32(HR_PWM_CTL) & (~BIT(25)));
    }
    else
    {
        if (ENABLE == en)
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) |   BIT(0));
        else
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(0)));
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set break mode
 *
 * @param[in]      channel    pwm channel NO.,channel 0 or channel 4
 * @param[in]      en         whether enable the break mode,ENABLE or DISABLE
 * @param[in]      brok       when break
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_brake_mode_config(u8 channel, bool en, enum tls_pwm_brake_out_level brok)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if (ENABLE == en)
    {
        if (WM_PWM_BRAKE_OUT_HIGH == brok)
            tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) | BIT(3+channel));
        else
            tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) & (~BIT(3+channel)));
        tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) | BIT(11+channel));
    }
    else
    {
        tls_reg_write32(HR_PWM_BRKCTL, tls_reg_read32(HR_PWM_BRKCTL) & (~BIT(11+channel)));
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to enable the capture mode
 *
 * @param[in]      channel    pwm channel NO.,channel 0 or channel 4
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_capture_mode_config(u8 channel)
{
    if (channel != 0 && channel != 4)
        return WM_FAILED;
    if (channel == 0)
    {
        tls_reg_write32(HR_PWM_CTL,      tls_reg_read32(HR_PWM_CTL) |   BIT(24));
    }
    else
    {
        tls_reg_write32(HR_PWM_CAP2CTL,  tls_reg_read32(HR_PWM_CAP2CTL)  |   BIT(1));
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set the interrupt about the number of period
 *
 * @param[in]      channel    pwm channel,range from 0 to 4
 * @param[in]      en         enble or disable
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_stoptime_irq_cmd(u8 channel, bool en)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if (4 == channel)
    {
        if (en)
            tls_reg_write32(HR_PWM_INTEN,   tls_reg_read32(HR_PWM_INTEN) |   BIT(4));
        else
            tls_reg_write32(HR_PWM_INTEN,   tls_reg_read32(HR_PWM_INTEN) & (~BIT(4)));
    }
    else
    {
        if (en)
            tls_reg_write32(HR_PWM_INTEN,  tls_reg_read32(HR_PWM_INTEN) |    BIT(channel));
        else
            tls_reg_write32(HR_PWM_INTEN,  tls_reg_read32(HR_PWM_INTEN) &  (~BIT(channel)));
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to set the interrupt about the
                   capture
 *
 * @param[in]      channel     pwm channel,channel 0 or channel 4
 * @param[in]      int_type    interrupt type
 *
 * @retval         WM_SUCCESS  success
 * @retval         WM_FAILED   failed
 *
 * @note           None
 */
int tls_pwm_capture_irq_type_config(u8 channel, enum tls_pwm_cap_int_type int_type)
{
    if (channel != 0 && channel != 4)
        return WM_FAILED;

    if (0 == channel)
    {
        if (WM_PWM_CAP_RISING_FALLING_EDGE_INT == int_type)
        {
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) |   BIT(5));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) |   BIT(6));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) & (~BIT(7)));
        }
        else if (WM_PWM_CAP_RISING_EDGE_INT == int_type)
        {
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) |   BIT(5));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) & (~BIT(6)));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) & (~BIT(7)));
        }
        else if (WM_PWM_CAP_FALLING_EDGE_INT == int_type)
        {
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) |   BIT(6));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) & (~BIT(5)));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) & (~BIT(7)));
        }
        else if(WM_PWM_CAP_DMA_INT == int_type)
        {
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) |   BIT(7));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) & (~BIT(5)));
            tls_reg_write32(HR_PWM_INTEN, tls_reg_read32(HR_PWM_INTEN) & (~BIT(6)));
        }
    }
    else if (4 == channel)
    {
        if (WM_PWM_CAP_RISING_FALLING_EDGE_INT == int_type)
        {
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) |   BIT(8));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) |   BIT(9));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(10)));
        }
        else if (WM_PWM_CAP_RISING_EDGE_INT == int_type)
        {
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) |   BIT(8));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(9)));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(10)));
        }
        else if (WM_PWM_CAP_FALLING_EDGE_INT == int_type)
        {
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) |   BIT(9));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(8)));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(10)));
        }
        else if(WM_PWM_CAP_DMA_INT == int_type)
        {
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) |   BIT(10));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(8)));
            tls_reg_write32(HR_PWM_CAP2CTL, tls_reg_read32(HR_PWM_CAP2CTL) & (~BIT(9)));
        }
    }

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to initial pwm(out mode)
 *
 * @param[in]      pwm_param    structure containing the initialization parameters
 *
 * @retval         WM_SUCCESS   success
 * @retval         WM_FAILED    failed
 *
 * @note           None
 */
int tls_pwm_out_init(pwm_init_param pwm_param)
{
    int ret=0;
    int pwm_status = 0;

	if (pwm_param.channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    if(tls_reg_read32(HR_PWM_CTL) & BIT(27 + pwm_param.channel))
    {
        tls_pwm_stop(pwm_param.channel);
        pwm_status = 1;
    }

    /* set output mode */
    ret = tls_pwm_out_mode_config(pwm_param.channel, pwm_param.mode);
    if (ret!=WM_SUCCESS)
        return WM_FAILED;

    if (WM_PWM_OUT_MODE_MC == pwm_param.mode)
    {
        /* set dead time */
        ret = tls_pwm_deadzone_config(pwm_param.channel, pwm_param.dten, pwm_param.dtclkdiv, pwm_param.dtcnt);
        if (ret!=WM_SUCCESS)
            return WM_FAILED;
    }

    /* set count type */
    tls_pwm_cnt_type_config(pwm_param.channel, pwm_param.cnt_type);

    /* set period value and duty radio */
    tls_pwm_freq_config(pwm_param.channel, pwm_param.clkdiv, pwm_param.period);
    tls_pwm_duty_config(pwm_param.channel, pwm_param.duty);

    /* set cycle type */
    tls_pwm_loop_mode_config(pwm_param.channel, pwm_param.loop_type);

    /* set output whether is inverse */
    tls_pwm_out_inverse_cmd(pwm_param.channel, pwm_param.inverse_en);

    /* set period number of generating */
    tls_pwm_stoptime_by_period_config(pwm_param.channel, pwm_param.pnum);

    /* set interrupt of period number whether is enable */
    tls_pwm_stoptime_irq_cmd(pwm_param.channel, pwm_param.pnum_int);

    /* set output status */
    if (pwm_param.channel == 0 || pwm_param.channel == 4)
        tls_pwm_output_en_cmd(pwm_param.channel, WM_PWM_OUT_EN_STATE_OUT);
    if (pwm_param.mode == WM_PWM_OUT_MODE_ALLSYC && pwm_param.channel == 0)
        tls_pwm_output_en_cmd(4, WM_PWM_OUT_EN_STATE_OUT);
    if(pwm_status)
    {
        tls_pwm_start(pwm_param.channel);
    }
    return WM_SUCCESS;
}

/**
 * @brief          This function is used to initial pwm(capture mode)
 *
 * @param[in]      channel       pwm channel, channel 0 or channel 4
 * @param[in]      clkdiv        clock divider, range 0 to 65535
 * @param[in]      inverse_en    whether the input signal is reversed
 * @param[in]      int_type      interrupt type
 *
 * @retval         WM_SUCCESS    success
 * @retval         WM_FAILED     failed
 *
 * @note           None
 */
int tls_pwm_cap_init(u8 channel, u16 clkdiv, bool inverse_en, enum tls_pwm_cap_int_type int_type)
{
    if (channel != 0 && channel != 4)
        return WM_FAILED;

    /* set clock divider and period value */
    tls_pwm_freq_config(channel, clkdiv, 0xFF);

    /* set input of capture mode whether is inverse */
    tls_pwm_capture_inverse_cmd(channel, inverse_en);

    /* set the capture mode */
    tls_pwm_capture_mode_config(channel);

    /* set count type (only edge alignment in the capture mode) */
    tls_pwm_cnt_type_config(channel, WM_PWM_CNT_TYPE_EDGE_ALLGN_CAP);

    /* set output status */
    tls_pwm_output_en_cmd(channel, WM_PWM_OUT_EN_STATE_TRI);

    /* set cycle mode (must be set int the capture mode) */
    tls_pwm_loop_mode_config(channel, WM_PWM_LOOP_TYPE_LOOP);

    /* set interrupt type */
    tls_pwm_capture_irq_type_config(channel, int_type);

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to start pwm
 *
 * @param[in]      channel    pwm channel, range from 0 to 4
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_start(u8 channel)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    tls_reg_write32(HR_PWM_CTL, tls_reg_read32(HR_PWM_CTL) | BIT(27 + channel)); /* start counter */

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to stop pwm
 *
 * @param[in]      channel    pwm channel, range from 0 to 4
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_stop(u8 channel)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return WM_FAILED;

    tls_reg_write32(HR_PWM_CTL, tls_reg_read32(HR_PWM_CTL) & (~BIT(27 + channel)));/* stop counter */

    return WM_SUCCESS;
}

/**
 * @brief          This function is used to stop pwm
 *
 * @param[in]      channel    pwm channel no, range form 0 to 4
 * @param[in]      freq       frequency, range from 1 to 156250
 *
 * @return         None
 *
 * @note           None
 */
void tls_pwm_freq_set(u8 channel, u32 freq)
{
    u16 clkdiv=0;
	tls_sys_clk sysclk;
	
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return;

	tls_sys_clk_get(&sysclk);

    clkdiv = sysclk.apbclk*UNIT_MHZ/256/freq;
    tls_pwm_stop(channel);
    tls_pwm_freq_config(channel, clkdiv, 255);
    tls_pwm_start(channel);
}

/**
 * @brief          This function is used to set duty radio
 *
 * @param[in]      channel    pwm channel NO., range form 0 to 4
 * @param[in]      duty       duty radio, range from 0 to 255
 *
 * @return         None
 *
 * @note           None
 */
void tls_pwm_duty_set(u8 channel, u8 duty)
{
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
		return;

    tls_pwm_duty_config(channel, duty);
}

/**
 * @brief          This function is used to initial pwm
 *
 * @param[in]      	channel    pwm channel, range from 0 to 4
 * @param[in]      	freq       freq range from 1 to 156250
 * @param[in]      	duty       duty range from 0 to 255
 * @param[in]      	pnum       period num,range from 0 to 255
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 *
 * @note           None
 */
int tls_pwm_init(u8 channel,u32 freq, u8 duty, u8 pnum)
{
    pwm_init_param pwm_param;
    int ret=-1;
	tls_sys_clk sysclk;
	
	tls_sys_clk_get(&sysclk);

    memset(&pwm_param, 0, sizeof(pwm_init_param));
    pwm_param.period = 255;
    pwm_param.cnt_type = WM_PWM_CNT_TYPE_EDGE_ALIGN_OUT;
    pwm_param.loop_type = WM_PWM_LOOP_TYPE_LOOP;
    pwm_param.mode = WM_PWM_OUT_MODE_INDPT;
    pwm_param.inverse_en = DISABLE;
    pwm_param.pnum = pnum;
    pwm_param.pnum_int = DISABLE;
    pwm_param.duty = duty;
    pwm_param.channel = channel;
    pwm_param.clkdiv = sysclk.apbclk*UNIT_MHZ/256/freq;
//	printf("clkdiv:%d\n", pwm_param.clkdiv);
    ret = tls_pwm_out_init(pwm_param);
//    tls_pwm_start(channel);

    return 	ret;
}
/**
 * @brief          This function is used to get pwm info
 *
 * @param[in]      channel    pwm channel, range from 0 to 4
 * @param[in]      clkdiv     is a pointer to clkdiv, clkdiv range from 1 to 65535
 * @param[in]      duty       is a pointer to duty, duty range from 0 to 255
 * @param[in]      period     is a pointer to period, period range from 0 to 255
 *
 * @retval         WM_SUCCESS success
 * @retval         WM_FAILED  failed
 */
int tls_pwm_get_info(u8 channel, u32 *clkdiv, u8 *duty, u8 *period)
{
    u32 temp = 0;
    
    if(channel > (PWM_CHANNEL_MAX_NUM - 1))
        return WM_FAILED;

    if (4 == channel)
    {
        temp = tls_reg_read32(HR_PWM_CH4_REG1) & 0xFFFF0000;
        *clkdiv = (temp >> 16);

        temp = tls_reg_read32(HR_PWM_CH4_REG2) & 0x0000FF00;
        *duty = (temp >> 8);

        temp = tls_reg_read32(HR_PWM_CH4_REG1) & 0x0000FF00;
        *period = (temp >> 8);
    }
    else
    {
        temp = tls_reg_read32(HR_PWM_CLKDIV01 + (channel / 2) * 4) & (0xFFFF << ((channel % 2) * 16));
        *clkdiv = (temp >> ((channel % 2) * 16));

        temp = tls_reg_read32(HR_PWM_CMPDAT) & (0xFF << channel * 8);
        *duty = (temp >> (channel * 8));

        temp = tls_reg_read32(HR_PWM_PERIOD) & (0xFF << channel * 8);
        *period = (temp >> (channel * 8));
    }
    
    return WM_SUCCESS;
}




/**
 * @file    wm_rtc.c
 *
 * @brief   rtc Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_regs.h"
#include "wm_rtc.h"

#include "wm_irq.h"
#include "tls_common.h"

struct rtc_irq_context {
    tls_rtc_irq_callback callback;
    void *arg;
};

static struct rtc_irq_context rtc_context = {0};


/**
 * @brief          This function is used to set pmu rtc time
 *
 * @param[in]      tblock    time value
 *
 * @return         None
 *
 * @note           None
 */
void tls_set_rtc(struct tm *tblock)
{
    int ctrl1 = 0;
    int ctrl2 = 0;

	ctrl2  = tls_reg_read32(HR_PMU_RTC_CTRL2);	/* disable */
	ctrl2 &= ~(1 << 16);
	tls_reg_write32(HR_PMU_RTC_CTRL2, ctrl2);

    ctrl1 |= tblock->tm_sec;
    ctrl1 |= tblock->tm_min  << 8;
    ctrl1 |= tblock->tm_hour << 16;
    ctrl1 |= tblock->tm_mday << 24;
    tls_reg_write32(HR_PMU_RTC_CTRL1, ctrl1);

    ctrl2  = 0;
    ctrl2 |= tblock->tm_mon;
    ctrl2 |= tblock->tm_year << 8;
    tls_reg_write32(HR_PMU_RTC_CTRL2, ctrl2);

	ctrl2  = tls_reg_read32(HR_PMU_RTC_CTRL2);		/* enable */
	ctrl2 |= (1 << 16);
	tls_reg_write32(HR_PMU_RTC_CTRL2, ctrl2);
}

/**
 * @brief          This function is used to get pmu rtc time
 *
 * @param[out]     tblock    time value
 *
 * @return         None
 *
 * @note           None
 */
void tls_get_rtc(struct tm *tblock)
{
	int ctrl1 = 0;
	int ctrl2 = 0;

	ctrl1 = tls_reg_read32(HR_PMU_RTC_CTRL1);
	ctrl2 = tls_reg_read32(HR_PMU_RTC_CTRL2);
	tblock->tm_year = ((int)((int)ctrl2 & 0x00007f00) >> 8);
	tblock->tm_mon  = (ctrl2 & 0x0000000f);
	tblock->tm_mday = (ctrl1 & 0x1f000000) >> 24;
	tblock->tm_hour = (ctrl1 & 0x001f0000) >> 16;
	tblock->tm_min  = (ctrl1 & 0x00003f00) >>  8;
	tblock->tm_sec  =  ctrl1 & 0x0000003f;
}

void PMU_RTC_IRQHandler(void)
{
    if (tls_reg_read32(HR_PMU_INTERRUPT_SRC) & BIT(5)) /* rtc interrupt */
    {
        tls_reg_write32(HR_PMU_INTERRUPT_SRC, BIT(5)); /* clear rtc interrupt */

        if (NULL != rtc_context.callback)
            rtc_context.callback(rtc_context.arg);
    }
    return;
}

/**
 * @brief          This function is used to register pmu rtc interrupt
 *
 * @param[in]      callback    the rtc interrupt call back function
 * @param[in]      arg         parameter of call back function
 *
 * @return         None
 *
 * @note
 * user not need clear interrupt flag.
 * rtc callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
void tls_rtc_isr_register(tls_rtc_irq_callback callback, void *arg)
{
    rtc_context.callback = callback;
    rtc_context.arg      = arg;

//    tls_reg_write32(HR_PMU_INTERRUPT_MASK, tls_reg_read32(HR_PMU_INTERRUPT_MASK) & (~BIT(4)));

    tls_irq_enable(PMU_RTC_INT);

    return;
}

/**
 * @brief          This function is used to start pmu rtc timer
 *
 * @param[in]      tblock    timer value
 *
 * @return         None
 *
 * @note           None
 */
void tls_rtc_timer_start(struct tm *tblock)
{
    int ctrl1 = 0;
    int ctrl2 = 0;

    tls_irq_enable(PMU_RTC_INT);

    ctrl1 |= tblock->tm_sec;
    ctrl1 |= tblock->tm_min  << 8;
    ctrl1 |= tblock->tm_hour << 16;
    ctrl1 |= tblock->tm_mday << 24;

    ctrl2 |= tblock->tm_mon;
    ctrl2 |= tblock->tm_year << 8;
    tls_reg_write32(HR_PMU_RTC_CTRL2, ctrl2 | BIT(16));

    tls_reg_write32(HR_PMU_RTC_CTRL1, ctrl1 | BIT(31));/* must set the enable */

    return;
}

/**
 * @brief          This function is used to stop pmu rtc timer
 *
 * @param          None
 *
 * @return         None
 *
 * @note           This function also is used to clear rtc timer interrupt
 */
void tls_rtc_timer_stop(void)
{
    tls_reg_write32(HR_PMU_RTC_CTRL1, tls_reg_read32(HR_PMU_RTC_CTRL1) & (~BIT(31)));

    tls_irq_disable(PMU_RTC_INT);

    return;
}


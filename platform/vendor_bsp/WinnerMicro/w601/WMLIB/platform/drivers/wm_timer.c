/**
 * @file    wm_timer.c
 *
 * @brief   Timer Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_type_def.h"
#include "wm_timer.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_cpu.h"
#include "stdio.h"
#include "tls_common.h"

enum tls_timer_id{
    TLS_TIMER_ID_0 = 0,	// used by delay (sleep, msleep, usleep)
    TLS_TIMER_ID_1,
    TLS_TIMER_ID_2,
    TLS_TIMER_ID_3,
    TLS_TIMER_ID_4,
    TLS_TIMER_ID_5,
    TLS_TIMER_ID_MAX
};

#define TIM0_USED_BY_DELAY		0

struct timer_irq_context {
    tls_timer_irq_callback callback;
    void *arg;
};

static struct timer_irq_context timer_context[TLS_TIMER_ID_MAX] = {{0,0}};
static u8 wm_timer_bitmap = 0;

static void timer_clear_irq(int timer_id)
{
	//printf("timer_clear_irq\r\n");
    tls_reg_write32(HR_TIMER0_5_CSR, tls_reg_read32(HR_TIMER0_5_CSR)|TLS_TIMER_INT_CLR(timer_id));
}

static void timer_irq_callback(void *p)
{
    u8 timer_id;

    timer_id = (u8)(u32)p;

	//printf("timer_irq_callback\r\n");
    timer_clear_irq(timer_id);

    if (NULL != timer_context[timer_id].callback)
        timer_context[timer_id].callback(timer_context[timer_id].arg);

    return;
}

void TIM0_IRQHandler(void)
{
    timer_irq_callback((void *)TLS_TIMER_ID_0);
}
void TIM1_IRQHandler(void)
{
    timer_irq_callback((void *)TLS_TIMER_ID_1);
}
void TIM2_IRQHandler(void)
{
    timer_irq_callback((void *)TLS_TIMER_ID_2);
}
void TIM3_IRQHandler(void)
{
    timer_irq_callback((void *)TLS_TIMER_ID_3);
}
void TIM4_IRQHandler(void)
{
    timer_irq_callback((void *)TLS_TIMER_ID_4);
}
void TIM5_IRQHandler(void)
{
    timer_irq_callback((void *)TLS_TIMER_ID_5);
}

/**
 * @brief          This function is used to create the timer
 *
 * @param[in]      cfg     timer configuration
 *
 * @retval         	WM_TIMER_ID_INVALID     failed
 * @retval         	other                   timer id[0~5]
 *
 * @note
 * user not need clear interrupt flag.
 * timer callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
u8 tls_timer_create(struct tls_timer_cfg *cfg)
{
    u8 i;
    int timer_csr;

    for (i = TLS_TIMER_ID_1; i < TLS_TIMER_ID_MAX; i++)
    {
        if (!(wm_timer_bitmap & BIT(i)))
            break;
    }

    if (TLS_TIMER_ID_MAX == i)
        return WM_TIMER_ID_INVALID;

    wm_timer_bitmap  |= BIT(i);
    timer_context[i].callback = cfg->callback;
    timer_context[i].arg = cfg->arg;

	tls_sys_clk sysclk;	

	tls_sys_clk_get(&sysclk);
	tls_reg_write32(HR_TIMER_CFG, sysclk.apbclk-1);		
	
    timer_csr = tls_reg_read32(HR_TIMER0_5_CSR);
    if (!cfg->is_repeat)
	    timer_csr |=  TLS_TIMER_ONE_TIME(i);
	else
	    timer_csr &= ~(TLS_TIMER_ONE_TIME(i));
	if (TLS_TIMER_UNIT_MS == cfg->unit)
	    timer_csr |=  TLS_TIMER_MS_UNIT(i);
	else
	    timer_csr &= ~(TLS_TIMER_MS_UNIT(i));
    tls_reg_write32(HR_TIMER0_5_CSR, timer_csr | TLS_TIMER_INT_CLR(i));
    if(cfg->timeout){
        tls_reg_write32(HR_TIMER0_PRD + 0x04 * i, cfg->timeout);
    }

    tls_irq_enable(TIMER0_INT + i);

    return i;
}

/**
 * @brief          This function is used to start the timer
 *
 * @param[in]      	timer_id    timer id[0~5]
 *
 * @return         None
 *
 * @note           None
 */
void tls_timer_start(u8 timer_id)
{
    if (!(wm_timer_bitmap & BIT(timer_id)))
        return;

    tls_reg_write32(HR_TIMER0_5_CSR, tls_reg_read32(HR_TIMER0_5_CSR)|TLS_TIMER_INT_EN(timer_id)| TLS_TIMER_EN(timer_id));

    return;
}

/**
 * @brief          This function is used to stop the timer
 *
 * @param[in]      	timer_id    timer id[0~5]
 *
 * @return         None
 *
 * @note           None
 */
void tls_timer_stop(u8 timer_id)
{
    if (!(wm_timer_bitmap & BIT(timer_id)))
        return;

    tls_reg_write32(HR_TIMER0_5_CSR, tls_reg_read32(HR_TIMER0_5_CSR)|TLS_TIMER_INT_CLR(timer_id));
    tls_reg_write32(HR_TIMER0_5_CSR, tls_reg_read32(HR_TIMER0_5_CSR) &~ TLS_TIMER_EN(timer_id));
	
    return;
}

/**
 * @brief           This function is used to change a timer wait time
 *
 * @param[in]      	timer_id    timer id[0~5]
 *
 * @param[in]      	newtime     new wait time
 *
 * @retval         	None
 *
 * @note            If the timer does not start, this function will start the timer
 */
void tls_timer_change(u8 timer_id, u32 newtime)
{
    if (!(wm_timer_bitmap & BIT(timer_id)))
        return;

    tls_timer_stop(timer_id);
    if (newtime)
        tls_reg_write32(HR_TIMER0_PRD + 0x04 * timer_id, newtime);
    tls_timer_start(timer_id);

    return;
}

/**
 * @brief          This function is used to delete the timer
 *
 * @param[in]      	timer_id    timer id[0~5]
 *
 * @return         None
 *
 * @note           None
 */
void tls_timer_destroy(u8 timer_id)
{
    if (!(wm_timer_bitmap & BIT(timer_id)))
        return;

    tls_timer_stop(timer_id);

    timer_context[timer_id].callback = NULL;
    timer_context[timer_id].arg      = NULL;

    wm_timer_bitmap &= ~BIT(timer_id);

    return;
}

/**
 * @brief          This function is create a delay to elapse
 *
 * @param[in]      timeout the value writed into TMR0_PRD
 * @param[in]      m_flag millisecond flag in TIMER0
 *
 * @return         None
 *
 * @note           None
 */
int tls_delay_via_timer(unsigned int timeout, unsigned int m_flag)
{
	int ret = 0;
	tls_sys_clk sysclk;
	int timer_csr = 0;
	
	if (0 == timeout)
		return ret;

	/*
	 * no lock
	 * 
	 */
	tls_irq_disable(TIMER0_INT + TIM0_USED_BY_DELAY);
	tls_reg_write32(HR_TIMER0_PRD + 0x04 * TIM0_USED_BY_DELAY,
		timeout);
	
	tls_sys_clk_get(&sysclk);
	tls_reg_write32(HR_TIMER_CFG, sysclk.apbclk-1);
	
	timer_csr = tls_reg_read32(HR_TIMER0_5_CSR);
	//printf("%d\r\n",timer_csr);
	
	
	if (TIMER_MS_UNIT_FLAG == m_flag)
		timer_csr |= TLS_TIMER_MS_UNIT(TIM0_USED_BY_DELAY);
	else if (TIMER_US_UNIT_FLAG == m_flag)
		timer_csr &= ~(TLS_TIMER_MS_UNIT(TIM0_USED_BY_DELAY));
	
	timer_csr |= TLS_TIMER_ONE_TIME(TIM0_USED_BY_DELAY);
	timer_csr |= TLS_TIMER_INT_CLR(TIM0_USED_BY_DELAY);
	timer_csr |= (TLS_TIMER_INT_EN(TIM0_USED_BY_DELAY));
	timer_csr |= TLS_TIMER_EN(TIM0_USED_BY_DELAY);
	
	tls_reg_write32(HR_TIMER0_5_CSR, timer_csr);
	//printf("wait\r\n");
	while (!(tls_reg_read32(HR_TIMER0_5_CSR)
			& (TLS_TIMER_INT_CLR(TIM0_USED_BY_DELAY))))
			;
	//printf("timeout\r\n");
	timer_csr |= TLS_TIMER_INT_CLR(TIM0_USED_BY_DELAY);
	tls_reg_write32(HR_TIMER0_5_CSR, timer_csr);
	tls_irq_enable(TIMER0_INT + TIM0_USED_BY_DELAY);
	/*
	 * no unlock
	 * 
	 */
	
	return ret;
}


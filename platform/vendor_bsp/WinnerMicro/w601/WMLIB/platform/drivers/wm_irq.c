/**
 * @file    wm_irq.c
 *
 * @brief   interupt driver module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_config.h"
//#include "wm_mem.h"
void tls_irq_handler_dummy(void *data)
{
}


tls_irq_handler_t *intr_handler_vect[INTR_CNT];

u8 intr_counter;

/**
 * @brief          	This function is used to initial system interrupt.
 *
 * @param[in]      	None
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_irq_init(void)
{
#if 0
    int i;

    for (i = 0; i < INTR_CNT; i++)
    {
        intr_handler_vect[i].handler = tls_irq_handler_dummy;
        intr_handler_vect[i].data = NULL;
        intr_handler_vect[i].name = "intr_dummy";
        intr_handler_vect[i].counter = 0;
    }
#endif
    intr_counter = 0;

/* initailize VIC */
    tls_reg_write32(HR_VIC_INT_SELECT, 0);
    tls_reg_write32(HR_VIC_VECT_ENABLE, 0); /* disable vector interrupt */
    tls_reg_write32(HR_VIC_INT_EN_CLR, 0xFFFFFFFF);
}


/**
 * @brief          	This function is used to register interrupt.
 *
 * @param[in]      	vec_no           interrupt no
 * @param[in]      	handler
 * @param[in]      	*data
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_irq_register_handler(u8 vec_no, intr_handler_func handler, void *data)
{
// ASSERT (vec_no >= 0 && vec_no <= 0x1f);
    if (NULL == intr_handler_vect[vec_no])
    {
        intr_handler_vect[vec_no] = malloc(sizeof(tls_irq_handler_t));
    }
    if (intr_handler_vect[vec_no])
    {
        intr_handler_vect[vec_no]->handler = handler;
        intr_handler_vect[vec_no]->data = data;
    // intr_handler_vect[vec_no].name = NULL;
        intr_handler_vect[vec_no]->counter = 0;
    }

}

extern void NVIC_Configration(int irqno, FunctionalState irqstatus);

/**
 * @brief          	This function is used to enable interrupt.
 *
 * @param[in]      	vec_no       interrupt no
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_irq_enable(u8 vec_no)
{
    NVIC_ClearPendingIRQ((IRQn_Type)vec_no);
    NVIC_Configration(vec_no, ENABLE);
}

/**
 * @brief          	This function is used to disable interrupt.
 *
 * @param[in]      	vec_no       interrupt no
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_irq_disable(u8 vec_no)
{
    NVIC_Configration(vec_no, DISABLE);
}

void OS_CPU_IRQ_ISR_Handler(void)
{
    u32 irq_status = tls_reg_read32(HR_VIC_IRQ_STATUS);
    int i = 0;
    intr_counter++;

    for (i = 0; i < INTR_CNT; i++)
    {
        if (irq_status & (1UL << i))
        {
            intr_handler_vect[i]->handler((void *) intr_handler_vect[i]->data);
            intr_handler_vect[i]->counter++;
        }
    }
    intr_counter--;
/* clear interrupt */
    tls_reg_write32(HR_VIC_VECT_ADDR, 0);
}

void UART1_IRQHandler(void)
{
}

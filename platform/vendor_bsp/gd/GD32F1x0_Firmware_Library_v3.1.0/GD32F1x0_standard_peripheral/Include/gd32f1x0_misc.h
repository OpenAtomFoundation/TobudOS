/*!
    \file  gd32f1x0_misc.h
    \brief definitions for the MISC
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_MISC_H
#define GD32F1X0_MISC_H

#include "gd32f1x0.h"

/* constants definitions */
/* set the RAM and FLASH base address */
#define NVIC_VECTTAB_RAM            ((uint32_t)0x20000000U) /*!< RAM base address */
#define NVIC_VECTTAB_FLASH          ((uint32_t)0x08000000U) /*!< Flash base address */

/* set the NVIC vector table offset mask */
#define NVIC_VECTTAB_OFFSET_MASK    ((uint32_t)0x1FFFFF80U)

/* the register key mask, if you want to do the write operation, you should write 0x5FA to VECTKEY bits */
#define NVIC_AIRCR_VECTKEY_MASK     ((uint32_t)0x05FA0000U)

/* priority group - define the pre-emption priority and the subpriority */
#define NVIC_PRIGROUP_PRE0_SUB4     ((uint32_t)0x700U) /*!< 0 bits for pre-emption priority 4 bits for subpriority */
#define NVIC_PRIGROUP_PRE1_SUB3     ((uint32_t)0x600U) /*!< 1 bits for pre-emption priority 3 bits for subpriority */
#define NVIC_PRIGROUP_PRE2_SUB2     ((uint32_t)0x500U) /*!< 2 bits for pre-emption priority 2 bits for subpriority */
#define NVIC_PRIGROUP_PRE3_SUB1     ((uint32_t)0x400U) /*!< 3 bits for pre-emption priority 1 bits for subpriority */
#define NVIC_PRIGROUP_PRE4_SUB0     ((uint32_t)0x300U) /*!< 4 bits for pre-emption priority 0 bits for subpriority */

/* choose the method to enter or exit the lowpower mode */
#define SCB_SCR_SLEEPONEXIT         ((uint8_t)0x02U) /*!< choose the the system whether enter low power mode by exiting from ISR */
#define SCB_SCR_SLEEPDEEP           ((uint8_t)0x04U) /*!< choose the the system enter the DEEPSLEEP mode or SLEEP mode */
#define SCB_SCR_SEVONPEND           ((uint8_t)0x10U) /*!< choose the interrupt source that can wake up the lowpower mode */

#define SCB_LPM_SLEEP_EXIT_ISR      SCB_SCR_SLEEPONEXIT
#define SCB_LPM_DEEPSLEEP           SCB_SCR_SLEEPDEEP
#define SCB_LPM_WAKE_BY_ALL_INT     SCB_SCR_SEVONPEND

/* choose the systick clock source */
#define SYSTICK_CLKSOURCE_HCLK_DIV8 ((uint32_t)0xFFFFFFFBU) /*!< systick clock source is from HCLK/8 */
#define SYSTICK_CLKSOURCE_HCLK      ((uint32_t)0x00000004U) /*!< systick clock source is from HCLK */

/* function declarations */
/* set the priority group */
void nvic_priority_group_set(uint32_t nvic_prigroup);

/* enable NVIC request */
void nvic_irq_enable(uint8_t nvic_irq, uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority);
/* disable NVIC request */
void nvic_irq_disable(uint8_t nvic_irq);

/* set the NVIC vector table base address */
void nvic_vector_table_set(uint32_t nvic_vict_tab, uint32_t offset);

/* set the state of the low power mode */
void system_lowpower_set(uint8_t lowpower_mode);
/* reset the state of the low power mode */
void system_lowpower_reset(uint8_t lowpower_mode);

/* set the systick clock source */
void systick_clksource_set(uint32_t systick_clksource);

#endif /* GD32F1X0_MISC_H */

/*!
    \file  gd32f1x0_dac.h
    \brief definitions for the DAC
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/


#ifndef GD32F1X0_DAC_H
#define GD32F1X0_DAC_H

#include "gd32f1x0.h"

/* DACx(x=0,1) definitions */
#define DAC                     DAC_BASE
#define DAC0                    0U
#ifdef GD32F170_190
#define DAC1                    1U
#endif  /* GD32F170_190 */

/* registers definitions */
#define DAC_CTL                 REG32(DAC + 0x00U) /*!< DAC control register */
#define DAC_SWT                 REG32(DAC + 0x04U) /*!< DAC software trigger register */
#define DAC0_R12DH              REG32(DAC + 0x08U) /*!< DAC0 12-bit right-aligned data holding register */
#define DAC0_L12DH              REG32(DAC + 0x0CU) /*!< DAC0 12-bit left-aligned data holding register */
#define DAC0_R8DH               REG32(DAC + 0x10U) /*!< DAC0 8-bit right-aligned data holding register */
#ifdef GD32F170_190
#define DAC1_R12DH              REG32(DAC + 0x14U) /*!< DAC1 12-bit right-aligned data holding register */
#define DAC1_L12DH              REG32(DAC + 0x18U) /*!< DAC1 12-bit left-aligned data holding register */
#define DAC1_R8DH               REG32(DAC + 0x1CU) /*!< DAC1 8-bit right-aligned data holding register */
#define DACC_R12DH              REG32(DAC + 0x20U) /*!< DAC concurrent mode 12-bit right-aligned data holding register */
#define DACC_L12DH              REG32(DAC + 0x24U) /*!< DAC concurrent mode 12-bit left-aligned data holding register */
#define DACC_R8DH               REG32(DAC + 0x28U) /*!< DAC concurrent mode 8-bit right-aligned data holding register */
#endif  /* GD32F170_190 */
#define DAC0_DO                 REG32(DAC + 0x2CU) /*!< DAC0 output data register */
#ifdef GD32F170_190
#define DAC1_DO                 REG32(DAC + 0x30U) /*!< DAC1 output data register */
#endif  /* GD32F170_190 */
#define DAC_STAT                REG32(DAC + 0x34U) /*!< DAC status register */

/* bits definitions */
/* DAC_CTL */
#define DAC_CTL_DEN0            BIT(0)             /*!< DAC0 enable/disable bit */
#define DAC_CTL_DBOFF0          BIT(1)             /*!< DAC0 output buffer turn on/turn off bit */
#define DAC_CTL_DTEN0           BIT(2)             /*!< DAC0 trigger enable/disable bit */
#define DAC_CTL_DTSEL0          BITS(3,5)          /*!< DAC0 trigger source selection enable/disable bits */
#define DAC_CTL_DDMAEN0         BIT(12)            /*!< DAC0 DMA enable/disanle bit */
#define DAC_CTL_DDUDRIE0        BIT(13)            /*!< DAC0 DMA underrun Interrupt enable/disable bit */
#ifdef GD32F170_190
#define DAC_CTL_DEN1            BIT(16)            /*!< DAC1 enable/disable bit */ 
#define DAC_CTL_DBOFF1          BIT(17)            /*!< DAC1 output buffer turn on/turn off bit */
#define DAC_CTL_DTEN1           BIT(18)            /*!< DAC1 trigger enable/disable bit */
#define DAC_CTL_DTSEL1          BITS(19,21)        /*!< DAC1 trigger source selection enable/disable bits */
#define DAC_CTL_DDMAEN1         BIT(28)            /*!< DAC1 DMA enable/disable bit */
#define DAC_CTL_DDUDRIE1        BIT(29)            /*!< DAC1 DMA underrun interrupt enable/disable bit */
#endif  /* GD32F170_190 */

/* DAC_SWT */
#define DAC_SWT_SWTR0           BIT(0)             /*!< DAC0 software trigger bit,cleared by hardware */
#ifdef GD32F170_190
#define DAC_SWT_SWTR1           BIT(1)             /*!< DAC1 software trigger bit,cleared by hardware */
#endif  /* GD32F170_190 */

/* DAC0_R12DH */
#define DAC0_R12DH_DAC0_DH      BITS(0,11)         /*!< DAC0 12-bit right-aligned data bits */

/* DAC0_L12DH */
#define DAC0_L12DH_DAC0_DH      BITS(4,15)         /*!< DAC0 12-bit left-aligned data bits */

/* DAC0_R8DH */
#define DAC0_R8DH_DAC0_DH       BITS(0,7)          /*!< DAC0 8-bit right-aligned data bits */

#ifdef GD32F170_190
/* DAC1_R12DH */
#define DAC1_R12DH_DAC1_DH      BITS(0,11)         /*!< DAC1 12-bit right-aligned data bits */

/* DAC1_L12DH */
#define DAC1_L12DH_DAC1_DH      BITS(4,15)         /*!< DAC1 12-bit left-aligned data bits */

/* DAC1_R8DH */
#define DAC1_R8DH_DAC1_DH       BITS(0,7)          /*!< DAC1 8-bit right-aligned data bits */

/* DACC_R12DH */
#define DACC_R12DH_DAC0_DH      BITS(0,11)         /*!< DAC concurrent mode DAC0 12-bit right-aligned data bits */
#define DACC_R12DH_DAC1_DH      BITS(16,27)        /*!< DAC concurrent mode DAC1 12-bit right-aligned data bits */

/* DACC_L12DH */
#define DACC_L12DH_DAC0_DH      BITS(4,15)         /*!< DAC concurrent mode DAC0 12-bit left-aligned data bits */
#define DACC_L12DH_DAC1_DH      BITS(20,31)        /*!< DAC concurrent mode DAC1 12-bit left-aligned data bits */

/* DACC_R8DH */
#define DACC_R8DH_DAC0_DH       BITS(0,7)          /*!< DAC concurrent mode DAC0 8-bit right-aligned data bits */
#define DACC_R8DH_DAC1_DH       BITS(8,15)         /*!< DAC concurrent mode DAC1 8-bit right-aligned data bits */
#endif  /* GD32F170_190 */

/* DAC0_DO */
#define DAC0_DO_DAC0_DO         BITS(0,11)         /*!< DAC0 12-bit output data bits */

#ifdef GD32F170_190
/* DAC1_DO */
#define DAC1_DO_DAC1_DO         BITS(0,11)         /*!< DAC1 12-bit output data bits */
#endif  /* GD32F170_190 */

/* DAC_STAT */
#define DAC_STAT_DDUDR0         BIT(13)            /*!< DAC0 DMA underrun flag */
#ifdef GD32F170_190
#define DAC_STAT_DDUDR1         BIT(29)            /*!< DAC1 DMA underrun flag */
#endif  /* GD32F170_190 */

/* constants definitions */
/* DAC trigger source */
#define CTL_DTSEL(regval)       (BITS(3,5) & ((uint32_t)(regval) << 3))
#define DAC_TRIGGER_T5_TRGO     CTL_DTSEL(0)       /*!< TIMER5 TRGO */
#define DAC_TRIGGER_T2_TRGO     CTL_DTSEL(1)       /*!< TIMER2 TRGO */
#define DAC_TRIGGER_T14_TRGO    CTL_DTSEL(3)       /*!< TIMER14 TRGO */
#define DAC_TRIGGER_T1_TRGO     CTL_DTSEL(4)       /*!< TIMER1 TRGO */
#define DAC_TRIGGER_EXTI_9    CTL_DTSEL(6)         /*!< EXTI interrupt line9 event */
#define DAC_TRIGGER_SOFTWARE    CTL_DTSEL(7)       /*!< software trigger */

/* dac data alignment */
#define DATA_ALIGN(regval)      (BITS(0,1) & ((uint32_t)(regval) << 0))
#define DAC_ALIGN_12B_R         DATA_ALIGN(0)      /*!< data right 12b alignment */
#define DAC_ALIGN_12B_L         DATA_ALIGN(1)      /*!< data left 12b alignment */
#define DAC_ALIGN_8B_R          DATA_ALIGN(2)      /*!< data right 8b alignment */

/* function declarations */
/* deinit DAC */
void dac_deinit(void);

/* enable DAC0 function */
void dac0_enable(void);
/* disable DAC0 function */
void dac0_disable(void);
/* enable DAC0 DMA function */
void dac0_dma_enable(void);
/* disable DAC0 DMA function */
void dac0_dma_disable(void);
/* enable DAC0 output buffer function */
void dac0_output_buffer_enable(void);
/* disable DAC0 output buffer function */
void dac0_output_buffer_disable(void);
/* enable DAC0 trigger function */
void dac0_trigger_enable(void);
/* disable DAC0 trigger function */
void dac0_trigger_disable(void);
/* enable DAC0 software trigger function */
void dac0_software_trigger_enable(void);
/* disable DAC0 software trigger function */
void dac0_software_trigger_disable(void);
/* enable DAC0 interrupt(DAC0 DMA underrun interrupt) */
void dac0_interrupt_enable(void);
/* disable DAC0 interrupt(DAC0 DMA underrun interrupt) */
void dac0_interrupt_disable(void);

/* set DAC0 tgigger source function */
void dac0_trigger_source_config(uint32_t triggersource);
/* get the last data output value */
uint16_t dac0_output_value_get(void);

/* get the specified DAC0 flag(DAC0 DMA underrun flag) */
FlagStatus dac0_flag_get(void);
/* clear the specified DAC0 flag(DAC0 DMA underrun flag) */
void dac0_flag_clear(void);
/* get the specified DAC0 interrupt flag(DAC0 DMA underrun interrupt flag) */
FlagStatus dac0_interrupt_flag_get(void);
/* clear the specified DAC0 interrupt flag(DAC0 DMA underrun interrupt flag) */
void dac0_interrupt_flag_clear(void); 

/* set DAC0 data holding register value */
void dac0_data_set(uint32_t dac_align, uint16_t data);

#ifdef GD32F170_190
/* enable DAC */
void dac_enable(uint32_t dac_periph);
/* disable DAC */
void dac_disable(uint32_t dac_periph);
/* enable DAC DMA */
void dac_dma_enable(uint32_t dac_periph);
/* disable DAC DMA */
void dac_dma_disable(uint32_t dac_periph); 
/* enable DAC output buffer */
void dac_output_buffer_enable(uint32_t dac_periph);
/* disable DAC output buffer */
void dac_output_buffer_disable(uint32_t dac_periph);
/* enable DAC trigger */
void dac_trigger_enable(uint32_t dac_periph);
/* disable DAC trigger */
void dac_trigger_disable(uint32_t dac_periph);
/* enable DAC software trigger */
void dac_software_trigger_enable(uint32_t dac_periph);
/* disable DAC software trigger */
void dac_software_trigger_disable(uint32_t dac_periph);
/* enable DAC interrupt(DAC0 DMA underrun interrupt) */
void dac_interrupt_enable(uint32_t dac_periph);
/* disable DAC interrupt(DAC0 DMA underrun interrupt) */
void dac_interrupt_disable(uint32_t dac_periph);

/* set DAC tgigger source */
void dac_trigger_source_config(uint32_t dac_periph,uint32_t triggersource);
/* get the last data output value */
uint16_t dac_output_value_get(uint32_t dac_periph);

/* get the specified DAC flag(DAC DMA underrun flag) */
FlagStatus dac_flag_get(uint32_t dac_periph);
/* clear the specified DAC flag(DAC DMA underrun flag) */
void dac_flag_clear(uint32_t dac_periph);
/* get the specified DAC interrupt flag(DAC DMA underrun interrupt flag) */
FlagStatus dac_interrupt_flag_get(uint32_t dac_periph);
/* clear the specified DAC interrupt flag(DAC DMA underrun interrupt flag) */
void dac_interrupt_flag_clear(uint32_t dac_periph);

/* enable DAC concurrent mode */
void dac_concurrent_enable(void);
/* disable DAC concurrent mode */
void dac_concurrent_disable(void);
/* enable DAC concurrent software trigger */
void dac_concurrent_software_trigger_enable(void);
/* disable DAC concurrent software trigger */
void dac_concurrent_software_trigger_disable(void);
/* enable DAC concurrent buffer */
void dac_concurrent_output_buffer_enable(void);
/* disable DAC concurrent buffer */
void dac_concurrent_output_buffer_disable(void);
/* enable DAC concurrent interrupt */
void dac_concurrent_interrupt_enable(void);
/* disable DAC concurrent interrupt */
void dac_concurrent_interrupt_disable(void);

/* set DAC data holding register value */
void dac_data_set(uint32_t dac_periph, uint32_t dac_align, uint16_t data);
/* set DAC concurrent mode data holding register value */
void dac_concurrent_data_set(uint32_t dac_align, uint16_t data1, uint16_t data2);

#endif /* GD32F170_190 */

#endif /* GD32F1X0_DAC_H */

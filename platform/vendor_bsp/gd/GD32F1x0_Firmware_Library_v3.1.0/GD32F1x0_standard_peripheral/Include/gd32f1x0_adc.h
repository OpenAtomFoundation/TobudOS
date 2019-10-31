/*!
    \file  gd32f1x0_adc.h
    \brief definitions for the ADC
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_ADC_H
#define GD32F1X0_ADC_H

#include "gd32f1x0.h"

/* ADC definitions */
#define ADC                             ADC_BASE

/* registers definitions */
#define ADC_STAT                        REG32(ADC + 0x00U)                  /*!< ADC status register */
#define ADC_CTL0                        REG32(ADC + 0x04U)                  /*!< ADC control register 0 */
#define ADC_CTL1                        REG32(ADC + 0x08U)                  /*!< ADC control register 1 */
#define ADC_SAMPT0                      REG32(ADC + 0x0CU)                  /*!< ADC sampling time register 0 */
#define ADC_SAMPT1                      REG32(ADC + 0x10U)                  /*!< ADC sampling time register 1 */
#define ADC_IOFF0                       REG32(ADC + 0x14U)                  /*!< ADC inserted channel data offset register 0 */
#define ADC_IOFF1                       REG32(ADC + 0x18U)                  /*!< ADC inserted channel data offset register 1 */
#define ADC_IOFF2                       REG32(ADC + 0x1CU)                  /*!< ADC inserted channel data offset register 2 */
#define ADC_IOFF3                       REG32(ADC + 0x20U)                  /*!< ADC inserted channel data offset register 3 */
#define ADC_WDHT                        REG32(ADC + 0x24U)                  /*!< ADC watchdog high threshold register */
#define ADC_WDLT                        REG32(ADC + 0x28U)                  /*!< ADC watchdog low threshold register */
#define ADC_RSQ0                        REG32(ADC + 0x2CU)                  /*!< ADC regular sequence register 0 */
#define ADC_RSQ1                        REG32(ADC + 0x30U)                  /*!< ADC regular sequence register 1 */
#define ADC_RSQ2                        REG32(ADC + 0x34U)                  /*!< ADC regular sequence register 2 */
#define ADC_ISQ                         REG32(ADC + 0x38U)                  /*!< ADC inserted sequence register */
#define ADC_IDATA0                      REG32(ADC + 0x3CU)                  /*!< ADC inserted data register 0 */
#define ADC_IDATA1                      REG32(ADC + 0x40U)                  /*!< ADC inserted data register 1 */
#define ADC_IDATA2                      REG32(ADC + 0x44U)                  /*!< ADC inserted data register 2 */
#define ADC_IDATA3                      REG32(ADC + 0x48U)                  /*!< ADC inserted data register 3 */
#define ADC_RDATA                       REG32(ADC + 0x4CU)                  /*!< ADC regular data register */

#ifdef GD32F170_190
#define ADC_OVSAMPCTL                   REG32(ADC + 0x80U)                  /*!< ADC oversampling control register */
#endif /* GD32F170_190 */

/* bits definitions */
/* ADC_STAT */
#define ADC_STAT_WDE                    BIT(0)                              /*!< analog watchdog flag */
#define ADC_STAT_EOC                    BIT(1)                              /*!< end of conversion */
#define ADC_STAT_EOIC                   BIT(2)                              /*!< injected channel end of conversion */
#define ADC_STAT_STIC                   BIT(3)                              /*!< injected channel start flag */
#define ADC_STAT_STRC                   BIT(4)                              /*!< regular channel start flag */

/* ADC_CTL0 */
#define ADC_CTL0_WDCHSEL                BITS(0,4)                           /*!< analog watchdog channel select bits */
#define ADC_CTL0_EOCIE                  BIT(5)                              /*!< interrupt enable for EOC */
#define ADC_CTL0_WDEIE                  BIT(6)                              /*!< analog watchdog interrupt enable */
#define ADC_CTL0_EOICIE                 BIT(7)                              /*!< interrupt enable for injected channels */
#define ADC_CTL0_SM                     BIT(8)                              /*!< scan mode */
#define ADC_CTL0_WDSC                   BIT(9)                              /*!< enable the watchdog on a single channel in scan mode */
#define ADC_CTL0_ICA                    BIT(10)                             /*!< automatic injected group conversion */
#define ADC_CTL0_DISRC                  BIT(11)                             /*!< discontinuous mode on regular channels */
#define ADC_CTL0_DISIC                  BIT(12)                             /*!< discontinuous mode on injected channels */
#define ADC_CTL0_DISNUM                 BITS(13,15)                         /*!< discontinuous mode channel count */
#define ADC_CTL0_IWDEN                  BIT(22)                             /*!< inserted channel analog watchdog enable */
#define ADC_CTL0_RWDEN                  BIT(23)                             /*!< regular channel analog watchdog enable */

#ifdef GD32F170_190
#define ADC_CTL0_DRES                   BITS(24,25)                         /*!< ADC resolution */
#endif /* GD32F170_190 */

/* ADC_CTL1 */
#define ADC_CTL1_ADCON                  BIT(0)                              /*!< ADC converter ON */
#define ADC_CTL1_CTN                    BIT(1)                              /*!< continuous conversion */
#define ADC_CTL1_CLB                    BIT(2)                              /*!< ADC calibration */
#define ADC_CTL1_RSTCLB                 BIT(3)                              /*!< reset calibration */
#define ADC_CTL1_DMA                    BIT(8)                              /*!< direct memory access mode */
#define ADC_CTL1_DAL                    BIT(11)                             /*!< data alignment */
#define ADC_CTL1_ETSIC                  BITS(12,14)                         /*!< external event select for injected group */
#define ADC_CTL1_ETEIC                  BIT(15)                             /*!< external trigger conversion mode for injected channels */
#define ADC_CTL1_ETSRC                  BITS(17,19)                         /*!< external event select for regular group */
#define ADC_CTL1_ETERC                  BIT(20)                             /*!< external trigger conversion mode for regular channels */
#define ADC_CTL1_SWICST                 BIT(21)                             /*!< start conversion of injected channels */
#define ADC_CTL1_SWRCST                 BIT(22)                             /*!< start conversion of regular channels */
#define ADC_CTL1_TSVREN                 BIT(23)                             /*!< temperature sensor and VREFINT enable */
#define ADC_CTL1_VBETEN                 BIT(24)                             /*!< VBAT enable */

/* ADC_SAMPTx x=0..1 */
#define ADC_SAMPTX_SPTN                 BITS(0,2)                           /*!< channel x sample time selection */

/* ADC_IOFFx x=0..3 */
#define ADC_IOFFX_IOFF                  BITS(0,11)                          /*!< data offset for injected channel x */

/* ADC_WHT */
#define ADC_WDHT_WDHT                   BITS(0,11)                          /*!< analog watchdog high threshold */

/* ADC_WLT */
#define ADC_WDLT_WDLT                   BITS(0,11)                          /*!< analog watchdog low threshold */

/* ADC_RSQx */
#define ADC_RSQX_RSQN                   BITS(0,4)                           /*!< x conversion in regular sequence */
#define ADC_RSQ0_RL                     BITS(20,23)                         /*!< regular channel sequence length */

/* ADC_ISQ */
#define ADC_ISQ_ISQN                    BITS(0,4)                           /*!< x conversion in regular sequence */
#define ADC_ISQ_IL                      BITS(20,21)                         /*!< injected sequence length */

/* ADC_IDTx x=0..3*/
#define ADC_IDATAX_IDATAN               BITS(0,15)                          /*!< injected data x */

/* ADC_RDT */
#define ADC_RDATA_RDATA                 BITS(0,15)                          /*!< regular data */

#ifdef GD32F170_190
/* ADC_OVCTL */
#define ADC_OVSAMPCTL_OVSEN             BIT(0)                              /*!< oversampling enable */
#define ADC_OVSAMPCTL_OVSR              BITS(2,4)                           /*!< oversampling ratio */
#define ADC_OVSAMPCTL_OVSS              BITS(5,8)                           /*!< oversampling shift */
#define ADC_OVSAMPCTL_TOVS              BIT(9)                              /*!< triggered oversampling */
#endif /* GD32F170_190 */

/* constants definitions */
/* ADC channel group definitions */
#define ADC_REGULAR_CHANNEL             ((uint8_t)0x01U)                    /*!< adc regular channel group */
#define ADC_INSERTED_CHANNEL            ((uint8_t)0x02U)                    /*!< adc injected channel group */
#define ADC_REGULAR_INSERTED_CHANNEL    ((uint8_t)0x03U)                    /*!< both regular and injected channel group */

/* ADC inserted channel definitions */
#define ADC_INSERTED_CHANNEL_0          ((uint8_t)0x00U)                    /*!< adc injected channel 0 */
#define ADC_INSERTED_CHANNEL_1          ((uint8_t)0x01U)                    /*!< adc injected channel 1 */
#define ADC_INSERTED_CHANNEL_2          ((uint8_t)0x02U)                    /*!< adc injected channel 2 */
#define ADC_INSERTED_CHANNEL_3          ((uint8_t)0x03U)                    /*!< adc injected channel 3 */

/* ADC special function definitions */
#define ADC_SCAN_MODE                   ADC_CTL0_SM                         /*!< scan mode */
#define ADC_INSERTED_CHANNEL_AUTO       ADC_CTL0_ICA                        /*!< inserted channel group convert automatically */
#define ADC_CONTINUOUS_MODE             ADC_CTL1_CTN                        /*!< continuous mode */

/* ADC channel definitions */
#define ADC_CHANNEL_0                   ((uint8_t)0x00U)                    /*!< ADC Channel 0 */
#define ADC_CHANNEL_1                   ((uint8_t)0x01U)                    /*!< ADC Channel 1 */
#define ADC_CHANNEL_2                   ((uint8_t)0x02U)                    /*!< ADC Channel 2 */
#define ADC_CHANNEL_3                   ((uint8_t)0x03U)                    /*!< ADC Channel 3 */
#define ADC_CHANNEL_4                   ((uint8_t)0x04U)                    /*!< ADC Channel 4 */
#define ADC_CHANNEL_5                   ((uint8_t)0x05U)                    /*!< ADC Channel 5 */
#define ADC_CHANNEL_6                   ((uint8_t)0x06U)                    /*!< ADC Channel 6 */
#define ADC_CHANNEL_7                   ((uint8_t)0x07U)                    /*!< ADC Channel 7 */
#define ADC_CHANNEL_8                   ((uint8_t)0x08U)                    /*!< ADC Channel 8 */
#define ADC_CHANNEL_9                   ((uint8_t)0x09U)                    /*!< ADC Channel 9 */
#define ADC_CHANNEL_10                  ((uint8_t)0x0AU)                    /*!< ADC Channel 10 */
#define ADC_CHANNEL_11                  ((uint8_t)0x0BU)                    /*!< ADC Channel 11 */
#define ADC_CHANNEL_12                  ((uint8_t)0x0CU)                    /*!< ADC Channel 12 */
#define ADC_CHANNEL_13                  ((uint8_t)0x0DU)                    /*!< ADC Channel 13 */
#define ADC_CHANNEL_14                  ((uint8_t)0x0EU)                    /*!< ADC Channel 14 */
#define ADC_CHANNEL_15                  ((uint8_t)0x0FU)                    /*!< ADC Channel 15 */
#define ADC_CHANNEL_16                  ((uint8_t)0x10U)                    /*!< ADC Channel 16 */
#define ADC_CHANNEL_17                  ((uint8_t)0x11U)                    /*!< ADC Channel 17 */
#define ADC_CHANNEL_18                  ((uint8_t)0x12U)                    /*!< ADC Channel 18 */

/* ADC channel sample time */
#define SAMPTX_SPT(regval)               (BITS(0,2) & ((uint32_t)(regval) << 0))
#define ADC_SAMPLETIME_1POINT5           SAMPTX_SPT(0)                      /*!< 1.5 sampling cycles */
#define ADC_SAMPLETIME_7POINT5           SAMPTX_SPT(1)                      /*!< 7.5 sampling cycles */
#define ADC_SAMPLETIME_13POINT5          SAMPTX_SPT(2)                      /*!< 13.5 sampling cycles */
#define ADC_SAMPLETIME_28POINT5          SAMPTX_SPT(3)                      /*!< 28.5 sampling cycles */
#define ADC_SAMPLETIME_41POINT5          SAMPTX_SPT(4)                      /*!< 41.5 sampling cycles */
#define ADC_SAMPLETIME_55POINT5          SAMPTX_SPT(5)                      /*!< 55.5 sampling cycles */
#define ADC_SAMPLETIME_71POINT5          SAMPTX_SPT(6)                      /*!< 71.5 sampling cycles */
#define ADC_SAMPLETIME_239POINT5         SAMPTX_SPT(7)                      /*!< 239.5 sampling cycles */

/* ADC data alignment */
#define ADC_DATAALIGN_RIGHT             ((uint32_t)0x00U)                   /*!< LSB alignment */
#define ADC_DATAALIGN_LEFT              ((uint32_t)0x01U)                   /*!< MSB alignment */

/* ADC status flag */
#define ADC_FLAG_WDE                    ADC_STAT_WDE                        /*!< analog watchdog flag */
#define ADC_FLAG_EOC                    ADC_STAT_EOC                        /*!< end of conversion */
#define ADC_FLAG_EOIC                   ADC_STAT_EOIC                       /*!< injected channel end of conversion */
#define ADC_FLAG_STIC                   ADC_STAT_STIC                       /*!< injected channel start flag */
#define ADC_FLAG_STRC                   ADC_STAT_STRC                       /*!< regular channel start flag */

/* ADC interrupt flag */
#define ADC_INT_WDE                     ADC_STAT_WDE                        /*!< analog watchdog interrupt */
#define ADC_INT_EOC                     ADC_STAT_EOC                        /*!< end of group conversion interrupt */
#define ADC_INT_EOIC                    ADC_STAT_EOIC                       /*!< end of inserted group conversion interrupt */

/* ADC interrupt flag */
#define ADC_INT_FLAG_WDE                ADC_STAT_WDE                        /*!< analog watchdog event interrupt flag */
#define ADC_INT_FLAG_EOC                ADC_STAT_EOC                        /*!< end of group conversion interrupt flag */
#define ADC_INT_FLAG_EOIC               ADC_STAT_EOIC                       /*!< end of inserted group conversion interrupt flag */

/* ADC resolution definitions */
#define CTL0_DRES(regval)               (BITS(24,25) & ((uint32_t)(regval) << 24))
#define ADC_RESOLUTION_12B              CTL0_DRES(0)                        /*!< 12-bit ADC resolution */
#define ADC_RESOLUTION_10B              CTL0_DRES(1)                        /*!< 10-bit ADC resolution */
#define ADC_RESOLUTION_8B               CTL0_DRES(2)                        /*!< 8-bit ADC resolution */
#define ADC_RESOLUTION_6B               CTL0_DRES(3)                        /*!< 6-bit ADC resolution */

/* ADC external trigger select for regular channel */
#define CTL1_ETSRC(regval)              (BITS(17,19) & ((uint32_t)(regval) << 17))
#define ADC_EXTTRIG_REGULAR_T0_CH0      CTL1_ETSRC(0)                       /*!< external trigger timer 0 CH0 event select for regular channel */
#define ADC_EXTTRIG_REGULAR_T0_CH1      CTL1_ETSRC(1)                       /*!< external trigger timer 0 CH1 event select for regular channel */
#define ADC_EXTTRIG_REGULAR_T0_CH2      CTL1_ETSRC(2)                       /*!< external trigger timer 0 CH2 event select for regular channel */
#define ADC_EXTTRIG_REGULAR_T1_CH1      CTL1_ETSRC(3)                       /*!< external trigger timer 1 CH1 event select for regular channel */
#define ADC_EXTTRIG_REGULAR_T2_TRGO     CTL1_ETSRC(4)                       /*!< external trigger timer 2 TRGO event select for regular channel */
#define ADC_EXTTRIG_REGULAR_T14_CH0     CTL1_ETSRC(5)                       /*!< external trigger timer 14 CH0 event select for regular channel */
#define ADC_EXTTRIG_REGULAR_EXT_IT11    CTL1_ETSRC(6)                       /*!< external trigger extiline 11 select for regular channel */
#define ADC_EXTTRIG_REGULAR_SWRCST      CTL1_ETSRC(7)                       /*!< software trigger select for regular channel */

/* ADC external trigger select for inserted channel */
#define CTL1_ETSIC(regval)              (BITS(12,14) & ((uint32_t)(regval) << 12))
#define ADC_EXTTRIG_INSERTED_T0_TRGO    CTL1_ETSIC(0)                       /*!< external trigger timer0 TRGO event select for inserted channel */
#define ADC_EXTTRIG_INSERTED_T0_CH3     CTL1_ETSIC(1)                       /*!< external trigger timer0 CH3 event select for inserted channel */
#define ADC_EXTTRIG_INSERTED_T1_TRGO    CTL1_ETSIC(2)                       /*!< external trigger timer1 TRGO event select for inserted channel */
#define ADC_EXTTRIG_INSERTED_T1_CH0     CTL1_ETSIC(3)                       /*!< external trigger timer1 CH0 event select for inserted channel */
#define ADC_EXTTRIG_INSERTED_T2_CH3     CTL1_ETSIC(4)                       /*!< external trigger timer2 CH3 event select for inserted channel */
#define ADC_EXTTRIG_INSERTED_T14_TRGO   CTL1_ETSIC(5)                       /*!< external trigger timer14 TRGO event select for inserted channel */
#define ADC_EXTTRIG_INSERTED_EXT_IT15   CTL1_ETSIC(6)                       /*!< external interrupt line 15 select for inserted channel */
#define ADC_EXTTRIG_INSERTED_SWRCST     CTL1_ETSIC(7)                       /*!< software trigger select for inserted channel */

/* adc_ioffx register value */
#define IOFFX_IOFF(regval)              (BITS(0,11) & ((uint32_t)(regval) << 0))    

#ifdef GD32F170_190
/* ADC oversampling mode */
#define ADC_OVERSAMPLING_ALL_CONVERT    0                                   /*!< all oversampled conversions for a channel are done consecutively after a trigger */
#define ADC_OVERSAMPLING_ONE_CONVERT    1                                   /*!< each oversampled conversion for a channel needs a trigger */

/* ADC oversampling shift */
#define OVSAMPCTL_OVSS(regval)          (BITS(5,8) & ((uint32_t)(regval) << 5))
#define ADC_OVERSAMPLING_SHIFT_NONE     OVSAMPCTL_OVSS(0)                   /*!< no oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_1B       OVSAMPCTL_OVSS(1)                   /*!< 1-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_2B       OVSAMPCTL_OVSS(2)                   /*!< 2-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_3B       OVSAMPCTL_OVSS(3)                   /*!< 3-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_4B       OVSAMPCTL_OVSS(4)                   /*!< 4-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_5B       OVSAMPCTL_OVSS(5)                   /*!< 5-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_6B       OVSAMPCTL_OVSS(6)                   /*!< 6-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_7B       OVSAMPCTL_OVSS(7)                   /*!< 7-bit oversampling shift */
#define ADC_OVERSAMPLING_SHIFT_8B       OVSAMPCTL_OVSS(8)                   /*!< 8-bit oversampling shift */

/* ADC oversampling ratio */
#define OVSAMPCTL_OVSR(regval)          (BITS(2,4) & ((uint32_t)(regval) << 2))
#define ADC_OVERSAMPLING_RATIO_MUL2     OVSAMPCTL_OVSR(0)                   /*!< oversampling ratio X2 */
#define ADC_OVERSAMPLING_RATIO_MUL4     OVSAMPCTL_OVSR(1)                   /*!< oversampling ratio X4 */
#define ADC_OVERSAMPLING_RATIO_MUL8     OVSAMPCTL_OVSR(2)                   /*!< oversampling ratio X8 */
#define ADC_OVERSAMPLING_RATIO_MUL16    OVSAMPCTL_OVSR(3)                   /*!< oversampling ratio X16 */
#define ADC_OVERSAMPLING_RATIO_MUL32    OVSAMPCTL_OVSR(4)                   /*!< oversampling ratio X32 */
#define ADC_OVERSAMPLING_RATIO_MUL64    OVSAMPCTL_OVSR(5)                   /*!< oversampling ratio X64 */
#define ADC_OVERSAMPLING_RATIO_MUL128   OVSAMPCTL_OVSR(6)                   /*!< oversampling ratio X128 */
#define ADC_OVERSAMPLING_RATIO_MUL256   OVSAMPCTL_OVSR(7)                   /*!< oversampling ratio X256 */
#endif /* GD32F170_190 */

/* function declarations */
/* ADC reset */
void adc_deinit(void);
/* enable ADC interface */
void adc_enable(void);
/* disable ADC interface */
void adc_disable(void);
/* ADC calibration and reset calibration */
void adc_calibration_enable(void);
/* enable DMA request */
void adc_dma_mode_enable(void);
/* disable DMA request */
void adc_dma_mode_disable(void);
/* enable the temperature sensor and Vrefint channel */
void adc_tempsensor_vrefint_enable(void);
/* disable the temperature sensor and Vrefint channel */
void adc_tempsensor_vrefint_disable(void);
/* enable the vbat channel */
void adc_vbat_enable(void);
/* disable the vbat channel */
void adc_vbat_disable(void);

/* ADC discontinuous mode */
void adc_discontinuous_mode_config(uint8_t channel_group,uint8_t length);
/* ADC special function config */
void adc_special_function_config(uint32_t function, ControlStatus newvalue);
/* ADC data alignment config */
void adc_data_alignment_config(uint32_t data_alignment);
/* ADC channel length config */

void adc_channel_length_config(uint8_t channel_group,uint32_t length);
/* ADC regular channel config */
void adc_regular_channel_config(uint8_t rank,uint8_t channel,uint32_t sample_time);
/* ADC inserted channel config */
void adc_inserted_channel_config(uint8_t rank,uint8_t channel,uint32_t sample_time);
/* ADC inserted channel offset config */
void adc_inserted_channel_offset_config(uint8_t inserted_channel,uint16_t offset);

/* ADC external trigger enable */
void adc_external_trigger_config(uint8_t channel_group,ControlStatus newvalue);
/* ADC external trigger source config */
void adc_external_trigger_source_config(uint8_t channel_group,uint32_t external_trigger_source);
/* ADC software trigger enable */
void adc_software_trigger_enable(uint8_t channel_group);

/* ADC regular group data register read */
uint16_t adc_regular_data_read(void);
/* ADC inserted group data register read */
uint16_t adc_inserted_data_read(uint8_t inserted_channel);

/* get the ADC status flag */
FlagStatus adc_flag_get(uint32_t flag);
/* clear the ADC status flag */
void adc_flag_clear(uint32_t flag);
/* get the ADC interrupt flag */
FlagStatus adc_interrupt_flag_get(uint32_t flag);
/* clear the ADC interrupt or status flag */
void adc_interrupt_flag_clear(uint32_t flag);
/* ADC interrupt enable */
void adc_interrupt_enable(uint32_t interrupt);
/* ADC interrupt disable */
void adc_interrupt_disable(uint32_t interrupt);

/* ADC analog watchdog single channel config */
void adc_watchdog_single_channel_enable(uint8_t channel);
/* ADC analog watchdog group channel config */
void adc_watchdog_group_channel_enable(uint8_t channel_group);
/* ADC analog watchdog disable */
void adc_watchdog_disable(void);
/* ADC analog watchdog threshold config */
void adc_watchdog_threshold_config(uint16_t low_threshold,uint16_t high_threshold);

#ifdef GD32F170_190
/* ADC resolution config */
void adc_resolution_config(uint32_t resolution);
/* ADC oversample mode config */
void adc_oversample_mode_config(uint8_t mode,uint16_t shift,uint8_t ratio);
/* enable ADC oversample mode */
void adc_oversample_mode_enable(void);
/* disable ADC oversample mode */
void adc_oversample_mode_disable(void);
#endif /* GD32F170_190 */

#endif /* GD32F1X0_ADC_H */

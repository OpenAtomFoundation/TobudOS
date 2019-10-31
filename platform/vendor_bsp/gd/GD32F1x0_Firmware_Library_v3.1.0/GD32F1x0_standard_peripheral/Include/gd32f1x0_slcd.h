/*!
    \file  gd32f1x0_slcd.h
    \brief definitions for the SLCD
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifdef GD32F170_190
#ifndef GD32F1X0_SLCD_H
#define GD32F1X0_SLCD_H

#include "gd32f1x0.h"

/* SLCD definitions */
#define SLCD                            SLCD_BASE

/* registers definitions */
#define SLCD_CTL                        REG32(SLCD + 0x00U)                 /*!< SLCD controller register */
#define SLCD_CFG                        REG32(SLCD + 0x04U)                 /*!< SLCD configuration register */
#define SLCD_STAT                       REG32(SLCD + 0x08U)                 /*!< SLCD status flag register */
#define SLCD_STATC                      REG32(SLCD + 0x0CU)                 /*!< SLCD status flag clear register */
#define SLCD_DATA0                      REG32(SLCD + 0x14U)                 /*!< SLCD display data register 0 */
#define SLCD_DATA1                      REG32(SLCD + 0x1CU)                 /*!< SLCD display data register 1 */
#define SLCD_DATA2                      REG32(SLCD + 0x24U)                 /*!< SLCD display data register 2 */
#define SLCD_DATA3                      REG32(SLCD + 0x2CU)                 /*!< SLCD display data register 3 */
#define SLCD_DATA4                      REG32(SLCD + 0x34U)                 /*!< SLCD display data register 4 */
#define SLCD_DATA5                      REG32(SLCD + 0x3CU)                 /*!< SLCD display data register 5 */
#define SLCD_DATA6                      REG32(SLCD + 0x44U)                 /*!< SLCD display data register 6 */
#define SLCD_DATA7                      REG32(SLCD + 0x4CU)                 /*!< SLCD display data register 7 */

/* bits definitions */
/* SLCD_CTL */
#define SLCD_CTL_LCDON                  BIT(0)                              /*!< SLCD controller start */
#define SLCD_CTL_VSRC                   BIT(1)                              /*!< SLCD Voltage source */
#define SLCD_CTL_DUTY                   BITS(2,4)                           /*!< duty select */
#define SLCD_CTL_BIAS                   BITS(5,6)                           /*!< SLCD bias voltage select */
#define SLCD_CTL_COMS                   BIT(7)                              /*!< regular channel start flag */

/* SLCD_CFG */
#define SLCD_CFG_HDEN                   BIT(0)                              /*!< high drive enable */
#define SLCD_CFG_SOFIE                  BIT(1)                              /*!< start of frame interrupt enable */
#define SLCD_CFG_UPDIE                  BIT(3)                              /*!< SLCD update done interrupt enable */
#define SLCD_CFG_PULSE                  BITS(4,6)                           /*!< pulse on duration */
#define SLCD_CFG_DTD                    BITS(7,9)                           /*!< dead time duration */
#define SLCD_CFG_CONR                   BITS(10,12)                         /*!< contrast ratio */
#define SLCD_CFG_BLKDIV                 BITS(13,15)                         /*!< blink frequency divider */
#define SLCD_CFG_BLKMOD                 BITS(16,17)                         /*!< blink mode */
#define SLCD_CFG_DIV                    BITS(18,21)                         /*!< SLCD clock divider */
#define SLCD_CFG_PSC                    BITS(22,25)                         /*!< SLCD clock prescaler */

/* SLCD_STAT */
#define SLCD_STAT_ONF                   BIT(0)                              /*!< SLCD controller on flag */
#define SLCD_STAT_SOF                   BIT(1)                              /*!< start of frame flag */
#define SLCD_STAT_UPRF                  BIT(2)                              /*!< SLCD data update request flag */
#define SLCD_STAT_UPDF                  BIT(3)                              /*!< update data done flag */
#define SLCD_STAT_VRDYF                 BIT(4)                              /*!< SLCD voltage ready flag */
#define SLCD_STAT_SYNF                  BIT(5)                              /*!< SLCD register synchronization flag */

/* SLCD_STATC */
#define SLCD_STATC_SOFC                 BIT(1)                              /*!< start of frame flag clear */
#define SLCD_STATC_UPDC                 BIT(3)                              /*!< SLCD data update done clear bit */

/* SLCD_DATAx */
#define SLCD_DATAx_DATA                 BITS(0,31)                          /*!< each bit corresponds to one segment to display */

/* constants definitions */
/* status flag */
#define SLCD_FLAG_ON                    SLCD_STAT_ONF                       /*!< SLCD controller on flag */
#define SLCD_FLAG_SOF                   SLCD_STAT_SOF                       /*!< start of frame flag */
#define SLCD_FLAG_UPR                   SLCD_STAT_UPRF                      /*!< SLCD data update request flag */
#define SLCD_FLAG_UPD                   SLCD_STAT_UPDF                      /*!< update data done flag */
#define SLCD_FLAG_VRDY                  SLCD_STAT_VRDYF                     /*!< SLCD voltage ready flag */
#define SLCD_FLAG_SYN                   SLCD_STAT_SYNF                      /*!< SLCD register synchronization flag */

/* interrupt flag */
#define SLCD_INT_FLAG_SOF               ((uint8_t)0x00U)                    /*!< start of frame interrupt */
#define SLCD_INT_FLAG_UPD               ((uint8_t)0x01U)                    /*!< update data done interrupt */

/* interrupt source */
#define SLCD_INT_SOF                    ((uint8_t)0x00U)                    /*!< start of frame interrupt */
#define SLCD_INT_UPD                    ((uint8_t)0x01U)                    /*!< update data done interrupt */

/* voltage source definitions */
#define SLCD_VOLTAGE_INTERNAL           ((uint8_t)0x00U)                    /*!< SLCD internal voltage source */
#define SLCD_VOLTAGE_EXTERNAL           ((uint8_t)0x01U)                    /*!< SLCD external voltage source */

/* data register definitions */
#define SLCD_DATA_REG0                  ((uint32_t)0x00000000U)             /*!< SLCD display data register 0 */
#define SLCD_DATA_REG1                  ((uint32_t)0x00000002U)             /*!< SLCD display data register 1 */
#define SLCD_DATA_REG2                  ((uint32_t)0x00000004U)             /*!< SLCD display data register 2 */
#define SLCD_DATA_REG3                  ((uint32_t)0x00000006U)             /*!< SLCD display data register 3 */
#define SLCD_DATA_REG4                  ((uint32_t)0x00000008U)             /*!< SLCD display data register 4 */
#define SLCD_DATA_REG5                  ((uint32_t)0x0000000AU)             /*!< SLCD display data register 5 */
#define SLCD_DATA_REG6                  ((uint32_t)0x0000000CU)             /*!< SLCD display data register 6 */
#define SLCD_DATA_REG7                  ((uint32_t)0x0000000EU)             /*!< SLCD display data register 7 */

/* bias voltage definitions */
#define CTL_BIAS(regval)                (BITS(5,6) & ((uint32_t)(regval) << 5U))
#define SLCD_BIAS_1_4                   CTL_BIAS(0)                         /*!< 1/4 voltage bias */
#define SLCD_BIAS_1_2                   CTL_BIAS(1)                         /*!< 1/2 voltage bias */
#define SLCD_BIAS_1_3                   CTL_BIAS(2)                         /*!< 1/3 voltage bias */

/* duty select definitions */
#define CTL_DUTY(regval)                (BITS(2,4) & ((uint32_t)(regval) << 2U))
#define SLCD_DUTY_STATIC                CTL_DUTY(0)                         /*!< static dutycycle */
#define SLCD_DUTY_1_2                   CTL_DUTY(1)                         /*!< 1/2 dutycycle */
#define SLCD_DUTY_1_3                   CTL_DUTY(2)                         /*!< 1/3 dutycycle */
#define SLCD_DUTY_1_4                   CTL_DUTY(3)                         /*!< 1/4 dutycycle */
#define SLCD_DUTY_1_6                   CTL_DUTY(5)                         /*!< 1/6 dutycycle */
#define SLCD_DUTY_1_8                   CTL_DUTY(4)                         /*!< 1/8 dutycycle */

/* SLCD clock prescaler */
#define CFG_PRE(regval)                 (BITS(22,25) & ((uint32_t)(regval) << 22U))
#define SLCD_PRESCALER_1                CFG_PRE(0)                          /*!< PRE = 0 */
#define SLCD_PRESCALER_2                CFG_PRE(1)                          /*!< PRE = 1 */
#define SLCD_PRESCALER_4                CFG_PRE(2)                          /*!< PRE = 2 */
#define SLCD_PRESCALER_8                CFG_PRE(3)                          /*!< PRE = 3 */
#define SLCD_PRESCALER_16               CFG_PRE(4)                          /*!< PRE = 4 */
#define SLCD_PRESCALER_32               CFG_PRE(5)                          /*!< PRE = 5 */
#define SLCD_PRESCALER_64               CFG_PRE(6)                          /*!< PRE = 6 */
#define SLCD_PRESCALER_128              CFG_PRE(7)                          /*!< PRE = 7 */
#define SLCD_PRESCALER_256              CFG_PRE(8)                          /*!< PRE = 8 */
#define SLCD_PRESCALER_512              CFG_PRE(9)                          /*!< PRE = 9 */
#define SLCD_PRESCALER_1024             CFG_PRE(10)                         /*!< PRE = 10 */
#define SLCD_PRESCALER_2048             CFG_PRE(11)                         /*!< PRE = 11 */
#define SLCD_PRESCALER_4096             CFG_PRE(12)                         /*!< PRE = 12 */
#define SLCD_PRESCALER_8192             CFG_PRE(13)                         /*!< PRE = 13 */
#define SLCD_PRESCALER_16384            CFG_PRE(14)                         /*!< PRE = 14 */
#define SLCD_PRESCALER_32768            CFG_PRE(15)                         /*!< PRE = 15 */

/* SLCD clock divider */
#define CFG_DIV(regval)                 (BITS(18,21) & ((uint32_t)(regval) << 18U))
#define SLCD_DIVIDER_16                 CFG_DIV(0)                          /*!< DIV = 16 */
#define SLCD_DIVIDER_17                 CFG_DIV(1)                          /*!< DIV = 17 */
#define SLCD_DIVIDER_18                 CFG_DIV(2)                          /*!< DIV = 18 */
#define SLCD_DIVIDER_19                 CFG_DIV(3)                          /*!< DIV = 19 */
#define SLCD_DIVIDER_20                 CFG_DIV(4)                          /*!< DIV = 20 */
#define SLCD_DIVIDER_21                 CFG_DIV(5)                          /*!< DIV = 21 */
#define SLCD_DIVIDER_22                 CFG_DIV(6)                          /*!< DIV = 22 */
#define SLCD_DIVIDER_23                 CFG_DIV(7)                          /*!< DIV = 23 */
#define SLCD_DIVIDER_24                 CFG_DIV(8)                          /*!< DIV = 24 */
#define SLCD_DIVIDER_25                 CFG_DIV(9)                          /*!< DIV = 25 */
#define SLCD_DIVIDER_26                 CFG_DIV(10)                         /*!< DIV = 26 */
#define SLCD_DIVIDER_27                 CFG_DIV(11)                         /*!< DIV = 27 */
#define SLCD_DIVIDER_28                 CFG_DIV(12)                         /*!< DIV = 28 */
#define SLCD_DIVIDER_29                 CFG_DIV(13)                         /*!< DIV = 29 */
#define SLCD_DIVIDER_30                 CFG_DIV(14)                         /*!< DIV = 30 */
#define SLCD_DIVIDER_31                 CFG_DIV(15)                         /*!< DIV = 31 */

/* SLCD blink mode */
#define CFG_BLKM(regval)                (BITS(16,17) & ((uint32_t)(regval) << 16U))
#define SLCD_BLINKMODE_OFF              CFG_BLKM(0)                         /*!< blink disabled */
#define SLCD_BLINKMODE_SEG0_COM0        CFG_BLKM(1)                         /*!< blink enabled on SEG[0], COM[0] */
#define SLCD_BLINKMODE_SEG0_ALLCOM      CFG_BLKM(2)                         /*!< blink enabled on SEG[0], all COM */
#define SLCD_BLINKMODE_ALLSEG_ALLCOM    CFG_BLKM(3)                         /*!< blink enabled on all SEG and all COM */

/* SLCD blink frequency divider */
#define CFG_BLKDIV(regval)              (BITS(13,15) & ((uint32_t)(regval) << 13U))
#define SLCD_BLINK_FREQUENCY_DIV8       CFG_BLKDIV(0)                       /*!< blink frequency = fSLCD/8 */
#define SLCD_BLINK_FREQUENCY_DIV16      CFG_BLKDIV(1)                       /*!< blink frequency = fSLCD/16 */
#define SLCD_BLINK_FREQUENCY_DIV32      CFG_BLKDIV(2)                       /*!< blink frequency = fSLCD/32 */
#define SLCD_BLINK_FREQUENCY_DIV64      CFG_BLKDIV(3)                       /*!< blink frequency = fSLCD/64 */
#define SLCD_BLINK_FREQUENCY_DIV128     CFG_BLKDIV(4)                       /*!< blink frequency = fSLCD/128 */
#define SLCD_BLINK_FREQUENCY_DIV256     CFG_BLKDIV(5)                       /*!< blink frequency = fSLCD/256 */
#define SLCD_BLINK_FREQUENCY_DIV512     CFG_BLKDIV(6)                       /*!< blink frequency = fSLCD/512 */
#define SLCD_BLINK_FREQUENCY_DIV1024    CFG_BLKDIV(7)                       /*!< blink frequency = fSLCD/1024 */

/* SLCD Contrast ratio */
#define CFG_CONR(regval)                (BITS(10,12) & ((uint32_t)(regval) << 10U))
#define SLCD_CONTRAST_LEVEL_0           CFG_CONR(0)                         /*!< maximum SLCD Voltage = 2.60V */
#define SLCD_CONTRAST_LEVEL_1           CFG_CONR(1)                         /*!< maximum SLCD Voltage = 2.73V */
#define SLCD_CONTRAST_LEVEL_2           CFG_CONR(2)                         /*!< maximum SLCD Voltage = 2.86V */
#define SLCD_CONTRAST_LEVEL_3           CFG_CONR(3)                         /*!< maximum SLCD Voltage = 2.99V */
#define SLCD_CONTRAST_LEVEL_4           CFG_CONR(4)                         /*!< maximum SLCD Voltage = 3.12V */
#define SLCD_CONTRAST_LEVEL_5           CFG_CONR(5)                         /*!< maximum SLCD Voltage = 3.25V */
#define SLCD_CONTRAST_LEVEL_6           CFG_CONR(6)                         /*!< maximum SLCD Voltage = 3.38V */
#define SLCD_CONTRAST_LEVEL_7           CFG_CONR(7)                         /*!< maximum SLCD Voltage = 3.51V */

/* dead time duration */
#define CFG_DD(regval)                  (BITS(7,9) & ((uint32_t)(regval) << 7U))
#define SLCD_DEADTIME_PERIOD_0          CFG_DD(0)                           /*!< no dead time */
#define SLCD_DEADTIME_PERIOD_1          CFG_DD(1)                           /*!< 1 phase inserted between couple of frame */
#define SLCD_DEADTIME_PERIOD_2          CFG_DD(2)                           /*!< 2 phase inserted between couple of frame */
#define SLCD_DEADTIME_PERIOD_3          CFG_DD(3)                           /*!< 3 phase inserted between couple of frame */
#define SLCD_DEADTIME_PERIOD_4          CFG_DD(4)                           /*!< 4 phase inserted between couple of frame */
#define SLCD_DEADTIME_PERIOD_5          CFG_DD(5)                           /*!< 5 phase inserted between couple of frame */
#define SLCD_DEADTIME_PERIOD_6          CFG_DD(6)                           /*!< 6 phase inserted between couple of frame */
#define SLCD_DEADTIME_PERIOD_7          CFG_DD(7)                           /*!< 7 phase inserted between couple of frame */

/* pulse on duration */
#define CFG_PULSE(regval)               (BITS(4,6) & ((uint32_t)(regval) << 4U))
#define SLCD_PULSEON_DURATION_0         CFG_PULSE(0)                        /*!< pulse on duration = 0 */
#define SLCD_PULSEON_DURATION_1         CFG_PULSE(1)                        /*!< pulse on duration = 1*1/fPRE */
#define SLCD_PULSEON_DURATION_2         CFG_PULSE(2)                        /*!< pulse on duration = 2*1/fPRE */
#define SLCD_PULSEON_DURATION_3         CFG_PULSE(3)                        /*!< pulse on duration = 3*1/fPRE */
#define SLCD_PULSEON_DURATION_4         CFG_PULSE(4)                        /*!< pulse on duration = 4*1/fPRE */
#define SLCD_PULSEON_DURATION_5         CFG_PULSE(5)                        /*!< pulse on duration = 5*1/fPRE */
#define SLCD_PULSEON_DURATION_6         CFG_PULSE(6)                        /*!< pulse on duration = 6*1/fPRE */
#define SLCD_PULSEON_DURATION_7         CFG_PULSE(7)                        /*!< pulse on duration = 7*1/fPRE */

/* function declarations */
/* check the SLCD status flag */
FlagStatus slcd_flag_get(uint8_t slcd_flag);
/* check the SLCD interrupt flag */
FlagStatus slcd_interrupt_flag_get(uint8_t slcd_interrupt);
/* clear the SLCD flag */
void slcd_flag_clear(uint8_t slcd_flag);
/* clear the SLCD interrupt flag */
void slcd_interrupt_flag_clear(uint8_t slcd_interrupt);
/* the SLCD interrupt config */
void slcd_interrupt_config(uint8_t slcd_interrupt,ControlStatus newvalue);

/* SLCD bias voltage select */
void slcd_bias_voltage_select(uint32_t bias_voltage);
/* SLCD duty select */
void slcd_duty_select(uint32_t duty);
/* SLCD input clock config */
void slcd_clock_config(uint32_t prescaler,uint32_t divider);
/* SLCD blink mode config */
void slcd_blink_mode_config(uint32_t mode,uint32_t blink_divider);
/* SLCD contrast ratio config */
void slcd_contrast_ratio_config(uint32_t contrast_ratio);
/* SLCD dead time duration config */
void slcd_dead_time_config(uint32_t dead_time);
/* SLCD pulse on duration config */
void slcd_pulse_on_duration_config(uint32_t duration);
/* SLCD common/segment pad select */
void slcd_com_seg_remap(ControlStatus newvalue);
/* SLCD voltage source select */
void slcd_voltage_source_select(uint8_t voltage_source);
/* SLCD high drive enable */
void slcd_high_drive_config(ControlStatus newvalue);

/* SLCD data register write */
void slcd_data_register_write(uint32_t data_reg,uint32_t data);
/* SLCD data update request */
void slcd_data_update_request(void);

/* SLCD reset */
void slcd_deinit(void);
/* enable SLCD interface */
void slcd_enable(void);
/* disable SLCD interface */
void slcd_disable(void);

#endif /* GD32F1X0_SLCD_H */

#endif /* GD32F170_190 */

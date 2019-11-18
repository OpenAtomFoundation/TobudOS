/*!
    \file  gd32f1x0_ivref.h
    \brief definitions for the IVREF
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifdef GD32F170_190

#ifndef GD32F1X0_IVREF_H
#define GD32F1X0_IVREF_H

#include "gd32f1x0.h"

/* IVREF definitions */
#define IVREF                           IVREF_BASE

/* registers definitions */
#define IVREF_CTL                       REG32(IVREF + 0x300U)  /*!< IVREF control register */

/* bits definitions */
/* IVREF_CTL */
#define IVREF_CTL_CSDT                  BITS(0,5)              /*!< current step data */
#define IVREF_CTL_SCMOD                 BIT(7)                 /*!< sink current mode */
#define IVREF_CTL_CPT                   BITS(8,12)             /*!< current precision trim */
#define IVREF_CTL_SSEL                  BIT(14)                /*!< step selection */ 
#define IVREF_CTL_CREN                  BIT(15)                /*!< current reference enable */
#define IVREF_CTL_VPT                   BITS(24,28)            /*!< voltage precision trim */
#define IVREF_CTL_DECAP                 BIT(30)                /*!< connect/disconnect external capacitor */
#define IVREF_CTL_VREN                  BIT(31)                /*!< voltage reference enable */

/* constants definitions */
/* vref mode selection */
#define VREF_DISCONNECT_EXTERNAL_CAP    BIT(30)                /*!< VREF disconnect external capacitor */
#define VREF_CONNECT_EXTERNAL_CAP       ((uint32_t)0x00000000) /*!< VREF connect external capacitor */

/* vref voltage precision trim */
#define CTL_VPT(regval)                 (BITS(24,28) & ((regval) << 24))
#define VREF_VOLT_PRECISION_TRIM_0      CTL_VPT(0)             /*!< VREF voltage precision trim 0 */
#define VREF_VOLT_PRECISION_TRIM_1      CTL_VPT(1)             /*!< VREF voltage precision trim 1 */
#define VREF_VOLT_PRECISION_TRIM_2      CTL_VPT(2)             /*!< VREF voltage precision trim 2 */
#define VREF_VOLT_PRECISION_TRIM_3      CTL_VPT(3)             /*!< VREF voltage precision trim 3 */
#define VREF_VOLT_PRECISION_TRIM_4      CTL_VPT(4)             /*!< VREF voltage precision trim 4 */
#define VREF_VOLT_PRECISION_TRIM_5      CTL_VPT(5)             /*!< VREF voltage precision trim 5 */
#define VREF_VOLT_PRECISION_TRIM_6      CTL_VPT(6)             /*!< VREF voltage precision trim 6 */
#define VREF_VOLT_PRECISION_TRIM_7      CTL_VPT(7)             /*!< VREF voltage precision trim 7 */
#define VREF_VOLT_PRECISION_TRIM_8      CTL_VPT(8)             /*!< VREF voltage precision trim 8 */
#define VREF_VOLT_PRECISION_TRIM_9      CTL_VPT(9)             /*!< VREF voltage precision trim 9 */
#define VREF_VOLT_PRECISION_TRIM_10     CTL_VPT(10)            /*!< VREF voltage precision trim 10 */
#define VREF_VOLT_PRECISION_TRIM_11     CTL_VPT(11)            /*!< VREF voltage precision trim 11 */
#define VREF_VOLT_PRECISION_TRIM_12     CTL_VPT(12)            /*!< VREF voltage precision trim 12 */
#define VREF_VOLT_PRECISION_TRIM_13     CTL_VPT(13)            /*!< VREF voltage precision trim 13 */
#define VREF_VOLT_PRECISION_TRIM_14     CTL_VPT(14)            /*!< VREF voltage precision trim 14 */
#define VREF_VOLT_PRECISION_TRIM_15     CTL_VPT(15)            /*!< VREF voltage precision trim 15 */
#define VREF_VOLT_PRECISION_TRIM_16     CTL_VPT(16)            /*!< VREF voltage precision trim 16 */
#define VREF_VOLT_PRECISION_TRIM_17     CTL_VPT(17)            /*!< VREF voltage precision trim 17 */
#define VREF_VOLT_PRECISION_TRIM_18     CTL_VPT(18)            /*!< VREF voltage precision trim 18 */
#define VREF_VOLT_PRECISION_TRIM_19     CTL_VPT(19)            /*!< VREF voltage precision trim 19 */
#define VREF_VOLT_PRECISION_TRIM_20     CTL_VPT(20)            /*!< VREF voltage precision trim 20 */
#define VREF_VOLT_PRECISION_TRIM_21     CTL_VPT(21)            /*!< VREF voltage precision trim 21 */
#define VREF_VOLT_PRECISION_TRIM_22     CTL_VPT(22)            /*!< VREF voltage precision trim 22 */
#define VREF_VOLT_PRECISION_TRIM_23     CTL_VPT(23)            /*!< VREF voltage precision trim 23 */
#define VREF_VOLT_PRECISION_TRIM_24     CTL_VPT(24)            /*!< VREF voltage precision trim 24 */
#define VREF_VOLT_PRECISION_TRIM_25     CTL_VPT(25)            /*!< VREF voltage precision trim 25 */
#define VREF_VOLT_PRECISION_TRIM_26     CTL_VPT(26)            /*!< VREF voltage precision trim 26 */
#define VREF_VOLT_PRECISION_TRIM_27     CTL_VPT(27)            /*!< VREF voltage precision trim 27 */
#define VREF_VOLT_PRECISION_TRIM_28     CTL_VPT(28)            /*!< VREF voltage precision trim 28 */
#define VREF_VOLT_PRECISION_TRIM_29     CTL_VPT(29)            /*!< VREF voltage precision trim 29 */
#define VREF_VOLT_PRECISION_TRIM_30     CTL_VPT(30)            /*!< VREF voltage precision trim 30 */
#define VREF_VOLT_PRECISION_TRIM_31     CTL_VPT(31)            /*!< VREF voltage precision trim 31 */

/* iref current precision trim */
#define CTL_CPT(regval)                 (BITS(8,12) & ((regval) << 8))
#define IREF_CUR_PRECISION_TRIM_0       CTL_CPT(0)             /*!< IREF current precision trim 0 */
#define IREF_CUR_PRECISION_TRIM_1       CTL_CPT(1)             /*!< IREF current precision trim 1 */
#define IREF_CUR_PRECISION_TRIM_2       CTL_CPT(2)             /*!< IREF current precision trim 2 */
#define IREF_CUR_PRECISION_TRIM_3       CTL_CPT(3)             /*!< IREF current precision trim 3 */
#define IREF_CUR_PRECISION_TRIM_4       CTL_CPT(4)             /*!< IREF current precision trim 4 */
#define IREF_CUR_PRECISION_TRIM_5       CTL_CPT(5)             /*!< IREF current precision trim 5 */
#define IREF_CUR_PRECISION_TRIM_6       CTL_CPT(6)             /*!< IREF current precision trim 6 */
#define IREF_CUR_PRECISION_TRIM_7       CTL_CPT(7)             /*!< IREF current precision trim 7 */
#define IREF_CUR_PRECISION_TRIM_8       CTL_CPT(8)             /*!< IREF current precision trim 8 */
#define IREF_CUR_PRECISION_TRIM_9       CTL_CPT(9)             /*!< IREF current precision trim 9 */
#define IREF_CUR_PRECISION_TRIM_10      CTL_CPT(10)            /*!< IREF current precision trim 10 */
#define IREF_CUR_PRECISION_TRIM_11      CTL_CPT(11)            /*!< IREF current precision trim 11 */
#define IREF_CUR_PRECISION_TRIM_12      CTL_CPT(12)            /*!< IREF current precision trim 12 */
#define IREF_CUR_PRECISION_TRIM_13      CTL_CPT(13)            /*!< IREF current precision trim 13 */
#define IREF_CUR_PRECISION_TRIM_14      CTL_CPT(14)            /*!< IREF current precision trim 14 */
#define IREF_CUR_PRECISION_TRIM_15      CTL_CPT(15)            /*!< IREF current precision trim 15 */
#define IREF_CUR_PRECISION_TRIM_16      CTL_CPT(16)            /*!< IREF current precision trim 16 */
#define IREF_CUR_PRECISION_TRIM_17      CTL_CPT(17)            /*!< IREF current precision trim 17 */
#define IREF_CUR_PRECISION_TRIM_18      CTL_CPT(18)            /*!< IREF current precision trim 18 */
#define IREF_CUR_PRECISION_TRIM_19      CTL_CPT(19)            /*!< IREF current precision trim 19 */
#define IREF_CUR_PRECISION_TRIM_20      CTL_CPT(20)            /*!< IREF current precision trim 20 */
#define IREF_CUR_PRECISION_TRIM_21      CTL_CPT(21)            /*!< IREF current precision trim 21 */
#define IREF_CUR_PRECISION_TRIM_22      CTL_CPT(22)            /*!< IREF current precision trim 22 */
#define IREF_CUR_PRECISION_TRIM_23      CTL_CPT(23)            /*!< IREF current precision trim 23 */
#define IREF_CUR_PRECISION_TRIM_24      CTL_CPT(24)            /*!< IREF current precision trim 24 */
#define IREF_CUR_PRECISION_TRIM_25      CTL_CPT(25)            /*!< IREF current precision trim 25 */
#define IREF_CUR_PRECISION_TRIM_26      CTL_CPT(26)            /*!< IREF current precision trim 26 */
#define IREF_CUR_PRECISION_TRIM_27      CTL_CPT(27)            /*!< IREF current precision trim 27 */
#define IREF_CUR_PRECISION_TRIM_28      CTL_CPT(28)            /*!< IREF current precision trim 28 */
#define IREF_CUR_PRECISION_TRIM_29      CTL_CPT(29)            /*!< IREF current precision trim 29 */
#define IREF_CUR_PRECISION_TRIM_30      CTL_CPT(30)            /*!< IREF current precision trim 30 */
#define IREF_CUR_PRECISION_TRIM_31      CTL_CPT(31)            /*!< IREF current precision trim 31 */
 
/* iref mode selection */
#define IREF_MODE_LOW_POWER             ((uint32_t)0x00000000)
#define IREF_MODE_HIGH_CURRENT          BIT(14)
 
/* iref current step */
#define CTL_CSDA(regval)                (BITS(0,5) & ((regval) << 5))
#define IREF_CUR_STEP_DATA_0            CTL_CSDA(0)            /*!< IREF current step data 0 */
#define IREF_CUR_STEP_DATA_1            CTL_CSDA(1)            /*!< IREF current step data 1 */
#define IREF_CUR_STEP_DATA_2            CTL_CSDA(2)            /*!< IREF current step data 2 */
#define IREF_CUR_STEP_DATA_3            CTL_CSDA(3)            /*!< IREF current step data 3 */
#define IREF_CUR_STEP_DATA_4            CTL_CSDA(4)            /*!< IREF current step data 4 */
#define IREF_CUR_STEP_DATA_5            CTL_CSDA(5)            /*!< IREF current step data 5 */
#define IREF_CUR_STEP_DATA_6            CTL_CSDA(6)            /*!< IREF current step data 6 */
#define IREF_CUR_STEP_DATA_7            CTL_CSDA(7)            /*!< IREF current step data 7 */
#define IREF_CUR_STEP_DATA_8            CTL_CSDA(8)            /*!< IREF current step data 8 */
#define IREF_CUR_STEP_DATA_9            CTL_CSDA(9)            /*!< IREF current step data 9 */
#define IREF_CUR_STEP_DATA_10           CTL_CSDA(10)           /*!< IREF current step data 10 */
#define IREF_CUR_STEP_DATA_11           CTL_CSDA(11)           /*!< IREF current step data 11 */
#define IREF_CUR_STEP_DATA_12           CTL_CSDA(12)           /*!< IREF current step data 12 */
#define IREF_CUR_STEP_DATA_13           CTL_CSDA(13)           /*!< IREF current step data 13 */
#define IREF_CUR_STEP_DATA_14           CTL_CSDA(14)           /*!< IREF current step data 14 */
#define IREF_CUR_STEP_DATA_15           CTL_CSDA(15)           /*!< IREF current step data 15 */
#define IREF_CUR_STEP_DATA_16           CTL_CSDA(16)           /*!< IREF current step data 16 */
#define IREF_CUR_STEP_DATA_17           CTL_CSDA(17)           /*!< IREF current step data 17 */
#define IREF_CUR_STEP_DATA_18           CTL_CSDA(18)           /*!< IREF current step data 18 */
#define IREF_CUR_STEP_DATA_19           CTL_CSDA(19)           /*!< IREF current step data 19 */
#define IREF_CUR_STEP_DATA_20           CTL_CSDA(20)           /*!< IREF current step data 20 */
#define IREF_CUR_STEP_DATA_21           CTL_CSDA(21)           /*!< IREF current step data 21 */
#define IREF_CUR_STEP_DATA_22           CTL_CSDA(22)           /*!< IREF current step data 22 */
#define IREF_CUR_STEP_DATA_23           CTL_CSDA(23)           /*!< IREF current step data 23 */
#define IREF_CUR_STEP_DATA_24           CTL_CSDA(24)           /*!< IREF current step data 24 */
#define IREF_CUR_STEP_DATA_25           CTL_CSDA(25)           /*!< IREF current step data 25 */
#define IREF_CUR_STEP_DATA_26           CTL_CSDA(26)           /*!< IREF current step data 26 */
#define IREF_CUR_STEP_DATA_27           CTL_CSDA(27)           /*!< IREF current step data 27 */
#define IREF_CUR_STEP_DATA_28           CTL_CSDA(28)           /*!< IREF current step data 28 */
#define IREF_CUR_STEP_DATA_29           CTL_CSDA(29)           /*!< IREF current step data 29 */
#define IREF_CUR_STEP_DATA_30           CTL_CSDA(30)           /*!< IREF current step data 30 */
#define IREF_CUR_STEP_DATA_31           CTL_CSDA(31)           /*!< IREF current step data 31 */
#define IREF_CUR_STEP_DATA_32           CTL_CSDA(32)           /*!< IREF current step data 32 */
#define IREF_CUR_STEP_DATA_33           CTL_CSDA(33)           /*!< IREF current step data 33 */
#define IREF_CUR_STEP_DATA_34           CTL_CSDA(34)           /*!< IREF current step data 34 */
#define IREF_CUR_STEP_DATA_35           CTL_CSDA(35)           /*!< IREF current step data 35 */
#define IREF_CUR_STEP_DATA_36           CTL_CSDA(36)           /*!< IREF current step data 36 */
#define IREF_CUR_STEP_DATA_37           CTL_CSDA(37)           /*!< IREF current step data 37 */
#define IREF_CUR_STEP_DATA_38           CTL_CSDA(38)           /*!< IREF current step data 38 */
#define IREF_CUR_STEP_DATA_39           CTL_CSDA(39)           /*!< IREF current step data 39 */
#define IREF_CUR_STEP_DATA_40           CTL_CSDA(40)           /*!< IREF current step data 40 */
#define IREF_CUR_STEP_DATA_41           CTL_CSDA(41)           /*!< IREF current step data 41 */
#define IREF_CUR_STEP_DATA_42           CTL_CSDA(42)           /*!< IREF current step data 42 */
#define IREF_CUR_STEP_DATA_43           CTL_CSDA(43)           /*!< IREF current step data 43 */
#define IREF_CUR_STEP_DATA_44           CTL_CSDA(44)           /*!< IREF current step data 44 */
#define IREF_CUR_STEP_DATA_45           CTL_CSDA(45)           /*!< IREF current step data 45 */
#define IREF_CUR_STEP_DATA_46           CTL_CSDA(46)           /*!< IREF current step data 46 */
#define IREF_CUR_STEP_DATA_47           CTL_CSDA(47)           /*!< IREF current step data 47 */
#define IREF_CUR_STEP_DATA_48           CTL_CSDA(48)           /*!< IREF current step data 48 */
#define IREF_CUR_STEP_DATA_49           CTL_CSDA(49)           /*!< IREF current step data 49 */
#define IREF_CUR_STEP_DATA_50           CTL_CSDA(50)           /*!< IREF current step data 50 */
#define IREF_CUR_STEP_DATA_51           CTL_CSDA(51)           /*!< IREF current step data 51 */
#define IREF_CUR_STEP_DATA_52           CTL_CSDA(52)           /*!< IREF current step data 52 */
#define IREF_CUR_STEP_DATA_53           CTL_CSDA(53)           /*!< IREF current step data 53 */
#define IREF_CUR_STEP_DATA_54           CTL_CSDA(54)           /*!< IREF current step data 54 */
#define IREF_CUR_STEP_DATA_55           CTL_CSDA(55)           /*!< IREF current step data 54 */
#define IREF_CUR_STEP_DATA_56           CTL_CSDA(56)           /*!< IREF current step data 54 */
#define IREF_CUR_STEP_DATA_57           CTL_CSDA(57)           /*!< IREF current step data 57 */
#define IREF_CUR_STEP_DATA_58           CTL_CSDA(58)           /*!< IREF current step data 58 */
#define IREF_CUR_STEP_DATA_59           CTL_CSDA(59)           /*!< IREF current step data 59 */
#define IREF_CUR_STEP_DATA_60           CTL_CSDA(60)           /*!< IREF current step data 60 */
#define IREF_CUR_STEP_DATA_61           CTL_CSDA(61)           /*!< IREF current step data 61 */
#define IREF_CUR_STEP_DATA_62           CTL_CSDA(62)           /*!< IREF current step data 62 */
#define IREF_CUR_STEP_DATA_63           CTL_CSDA(63)           /*!< IREF current step data 63 */

/* iref sink current mode*/ 
#define IREF_SOURCE_CURRENT             ((uint32_t)0x00000000) /*!< IREF source current */
#define IREF_SINK_CURRENT               BIT(7)                 /*!< IREF sink current */

/* function declarations */

/* deinit vref */
void ivref_deinit(void);
/* enable vref */
void vref_enable(void);
/* disable vref */
void vref_disable(void);
/* enable vref */
void iref_enable(void);
/* disable iref */
void iref_disable(void);

/* set verf mode */
void vref_mode_set(uint32_t vrefmode);
/* set vrer voltage precision trim value */
void vref_precision_trim_value_set(uint32_t precisiontrim);
/* set iref mode*/
void iref_mode_set(uint32_t irefmode);
/* set iref sink current mode*/
void iref_sink_set(uint32_t irefsinkmode);
/* set iref current precision trim value */
void iref_precision_trim_value_set(uint32_t precisiontrim);
/* set iref step data*/
void iref_step_data_config(uint32_t irefstepdata);

#endif /* GD32F1X0_IVREF_H */

#endif /* GD32F170_190 */

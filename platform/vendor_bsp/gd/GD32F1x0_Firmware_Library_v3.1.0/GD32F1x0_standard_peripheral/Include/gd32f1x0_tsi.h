/*!
    \file  gd32f1x0_tsi.h
    \brief definitions for the TSI
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_TSI_H
#define GD32F1X0_TSI_H

#include "gd32f1x0.h"

/* TSI definitions */
#define TSI                     TSI_BASE          /*!< TSI base address */

/* registers definitions */
#define TSI_CTL                 REG32(TSI + 0x00U)  /*!< TSI control register */
#define TSI_INTEN               REG32(TSI + 0x04U)  /*!< TSI interrupt enable register */
#define TSI_INTC                REG32(TSI + 0x08U)  /*!< TSI interrupt flag clear register */
#define TSI_INTF                REG32(TSI + 0x0CU)  /*!< TSI interrupt flag register */
#define TSI_PHM                 REG32(TSI + 0x10U)  /*!< TSI pin hysteresis mode register */
#define TSI_ASW                 REG32(TSI + 0x18U)  /*!< TSI analog switch register */
#define TSI_SAMPCFG             REG32(TSI + 0x20U)  /*!< TSI sample configuration register */
#define TSI_CHCFG               REG32(TSI + 0x28U)  /*!< TSI channel configuration register */
#define TSI_GCTL                REG32(TSI + 0x30U)  /*!< TSI group control register */
#define TSI_G0CYCN              REG32(TSI + 0x34U)  /*!< TSI group 0 cycle number register */
#define TSI_G1CYCN              REG32(TSI + 0x38U)  /*!< TSI group 1 cycle number register */
#define TSI_G2CYCN              REG32(TSI + 0x3CU)  /*!< TSI group 2 cycle number register */
#define TSI_G3CYCN              REG32(TSI + 0x40U)  /*!< TSI group 3 cycle number register */
#define TSI_G4CYCN              REG32(TSI + 0x44U)  /*!< TSI group 4 cycle number register */
#define TSI_G5CYCN              REG32(TSI + 0x48U)  /*!< TSI group 5 cycle number register */

/* bits definitions */
/* TSI_CTL */
#define TSI_CTL_TSIEN           BIT(0)              /*!< TSI enable */
#define TSI_CTL_TSIS            BIT(1)              /*!< TSI start */
#define TSI_CTL_TRGMOD          BIT(2)              /*!< trigger mode selection */
#define TSI_CTL_EGSEL           BIT(3)              /*!< edge selection */
#define TSI_CTL_PINMOD          BIT(4)              /*!< pin mode */
#define TSI_CTL_MCN             BITS(5,7)           /*!< max cycle number of a sequence */
#define TSI_CTL_CTCDIV          BITS(12,14)         /*!< CTCLK clock division factor */
#define TSI_CTL_ECDIV           BIT(15)             /*!< ECCLK clock division factor */
#define TSI_CTL_ECEN            BIT(16)             /*!< extend charge state enable */
#define TSI_CTL_ECDT            BITS(17,23)         /*!< extend charge State maximum duration time */
#define TSI_CTL_CTDT            BITS(24,27)         /*!< charge transfer state duration time */
#define TSI_CTL_CDT             BITS(28,31)         /*!< charge state duration time */

/* TSI_INTEN */
#define TSI_INTEN_CTCFIE        BIT(0)              /*!< charge transfer complete flag interrupt enable */
#define TSI_INTEN_MNERRIE       BIT(1)              /*!< max cycle number error interrupt enable */

/* TSI_INTC */
#define TSI_INTC_CCTCF          BIT(0)              /*!< clear charge transfer complete flag */
#define TSI_INTC_CMNERR         BIT(1)              /*!< clear max cycle number error */

/* TSI_INTF */
#define TSI_INTF_CTCF           BIT(0)              /*!< charge transfer complete flag */
#define TSI_INTF_MNERR          BIT(1)              /*!< max cycle number error */

/* TSI_PHM */
#define TSI_PHM_G0P0            BIT(0)              /*!< pin G0P0 Schmitt trigger hysteresis state */
#define TSI_PHM_G0P1            BIT(1)              /*!< pin G0P1 Schmitt trigger hysteresis state */
#define TSI_PHM_G0P2            BIT(2)              /*!< pin G0P2 Schmitt trigger hysteresis state */
#define TSI_PHM_G0P3            BIT(3)              /*!< pin G0P3 Schmitt trigger hysteresis state */
#define TSI_PHM_G1P0            BIT(4)              /*!< pin G1P0 Schmitt trigger hysteresis state */
#define TSI_PHM_G1P1            BIT(5)              /*!< pin G1P1 Schmitt trigger hysteresis state */
#define TSI_PHM_G1P2            BIT(6)              /*!< pin G1P2 Schmitt trigger hysteresis state */
#define TSI_PHM_G1P3            BIT(7)              /*!< pin G1P3 Schmitt trigger hysteresis state */
#define TSI_PHM_G2P0            BIT(8)              /*!< pin G2P0 Schmitt trigger hysteresis state */
#define TSI_PHM_G2P1            BIT(9)              /*!< pin G2P1 Schmitt trigger hysteresis state */
#define TSI_PHM_G2P2            BIT(10)             /*!< pin G2P2 Schmitt trigger hysteresis state */
#define TSI_PHM_G2P3            BIT(11)             /*!< pin G2P3 Schmitt trigger hysteresis state */
#define TSI_PHM_G3P0            BIT(12)             /*!< pin G3P0 Schmitt trigger hysteresis state */
#define TSI_PHM_G3P1            BIT(13)             /*!< pin G3P1 Schmitt trigger hysteresis state */
#define TSI_PHM_G3P2            BIT(14)             /*!< pin G3P2 Schmitt trigger hysteresis state */
#define TSI_PHM_G3P3            BIT(15)             /*!< pin G3P3 Schmitt trigger hysteresis state */
#define TSI_PHM_G4P0            BIT(16)             /*!< pin G4P0 Schmitt trigger hysteresis state */
#define TSI_PHM_G4P1            BIT(17)             /*!< pin G4P1 Schmitt trigger hysteresis state */
#define TSI_PHM_G4P2            BIT(18)             /*!< pin G4P2 Schmitt trigger hysteresis state */
#define TSI_PHM_G4P3            BIT(19)             /*!< pin G4P3 Schmitt trigger hysteresis state */
#define TSI_PHM_G5P0            BIT(20)             /*!< pin G5P0 Schmitt trigger hysteresis state */
#define TSI_PHM_G5P1            BIT(21)             /*!< pin G5P1 Schmitt trigger hysteresis state */
#define TSI_PHM_G5P2            BIT(22)             /*!< pin G5P2 Schmitt trigger hysteresis state */
#define TSI_PHM_G5P3            BIT(23)             /*!< pin G5P3 Schmitt trigger hysteresis state */

/* TSI_ASW */
#define TSI_ASW_G0P0            BIT(0)              /*!< pin G0P0 analog switch state */
#define TSI_ASW_G0P1            BIT(1)              /*!< pin G0P2 analog switch state */
#define TSI_ASW_G0P2            BIT(2)              /*!< pin G0P3 analog switch state */
#define TSI_ASW_G0P3            BIT(3)              /*!< pin G0P4 analog switch state */
#define TSI_ASW_G1P0            BIT(4)              /*!< pin G1P0 analog switch state */
#define TSI_ASW_G1P1            BIT(5)              /*!< pin G1P1 analog switch state */
#define TSI_ASW_G1P2            BIT(6)              /*!< pin G1P2 analog switch state */
#define TSI_ASW_G1P3            BIT(7)              /*!< pin G1P3 analog switch state */
#define TSI_ASW_G2P0            BIT(8)              /*!< pin G2P0 analog switch state */
#define TSI_ASW_G2P1            BIT(9)              /*!< pin G2P1 analog switch state */
#define TSI_ASW_G2P2            BIT(10)             /*!< pin G2P2 analog switch state */
#define TSI_ASW_G2P3            BIT(11)             /*!< pin G2P3 analog switch state */
#define TSI_ASW_G3P0            BIT(12)             /*!< pin G3P0 analog switch state */
#define TSI_ASW_G3P1            BIT(13)             /*!< pin G3P1 analog switch state */
#define TSI_ASW_G3P2            BIT(14)             /*!< pin G3P2 analog switch state */
#define TSI_ASW_G3P3            BIT(15)             /*!< pin G3P3 analog switch state */
#define TSI_ASW_G4P0            BIT(16)             /*!< pin G4P0 analog switch state */
#define TSI_ASW_G4P1            BIT(17)             /*!< pin G4P1 analog switch state */
#define TSI_ASW_G4P2            BIT(18)             /*!< pin G4P2 analog switch state */
#define TSI_ASW_G4P3            BIT(19)             /*!< pin G4P3 analog switch state */
#define TSI_ASW_G5P0            BIT(20)             /*!< pin G5P0 analog switch state */
#define TSI_ASW_G5P1            BIT(21)             /*!< pin G5P1 analog switch state */
#define TSI_ASW_G5P2            BIT(22)             /*!< pin G5P2 analog switch state */
#define TSI_ASW_G5P3            BIT(23)             /*!< pin G5P3 analog switch state */

/* TSI_SAMPCFG */
#define TSI_SAMPCFG_G0P0        BIT(0)              /*!< pin G0P0 sample pin mode */
#define TSI_SAMPCFG_G0P1        BIT(1)              /*!< pin G0P1 sample pin mode */
#define TSI_SAMPCFG_G0P2        BIT(2)              /*!< pin G0P2 sample pin mode */
#define TSI_SAMPCFG_G0P3        BIT(3)              /*!< pin G0P3 sample pin mode */
#define TSI_SAMPCFG_G1P0        BIT(4)              /*!< pin G1P0 sample pin mode */
#define TSI_SAMPCFG_G1P1        BIT(5)              /*!< pin G1P1 sample pin mode */
#define TSI_SAMPCFG_G1P2        BIT(6)              /*!< pin G1P2 sample pin mode */
#define TSI_SAMPCFG_G1P3        BIT(7)              /*!< pin G1P3 sample pin mode */
#define TSI_SAMPCFG_G2P0        BIT(8)              /*!< pin G2P0 sample pin mode */
#define TSI_SAMPCFG_G2P1        BIT(9)              /*!< pin G2P1 sample pin mode */
#define TSI_SAMPCFG_G2P2        BIT(10)             /*!< pin G2P2 sample pin mode */
#define TSI_SAMPCFG_G2P3        BIT(11)             /*!< pin G2P3 sample pin mode */
#define TSI_SAMPCFG_G3P0        BIT(12)             /*!< pin G3P0 sample pin mode */
#define TSI_SAMPCFG_G3P1        BIT(13)             /*!< pin G3P1 sample pin mode */
#define TSI_SAMPCFG_G3P2        BIT(14)             /*!< pin G3P2 sample pin mode */
#define TSI_SAMPCFG_G3P3        BIT(15)             /*!< pin G3P3 sample pin mode */
#define TSI_SAMPCFG_G4P0        BIT(16)             /*!< pin G4P0 sample pin mode */
#define TSI_SAMPCFG_G4P1        BIT(17)             /*!< pin G4P1 sample pin mode */
#define TSI_SAMPCFG_G4P2        BIT(18)             /*!< pin G4P2 sample pin mode */
#define TSI_SAMPCFG_G4P3        BIT(19)             /*!< pin G4P3 sample pin mode */
#define TSI_SAMPCFG_G5P0        BIT(20)             /*!< pin G5P0 sample pin mode */
#define TSI_SAMPCFG_G5P1        BIT(21)             /*!< pin G5P1 sample pin mode */
#define TSI_SAMPCFG_G5P2        BIT(22)             /*!< pin G5P2 sample pin mode */
#define TSI_SAMPCFG_G5P3        BIT(23)             /*!< pin G5P3 sample pin mode */

/* TSI_CHCFG */
#define TSI_CHCFG_G0P0          BIT(0)              /*!< pin G0P0 channel pin mode */
#define TSI_CHCFG_G0P1          BIT(1)              /*!< pin G0P1 channel pin mode */
#define TSI_CHCFG_G0P2          BIT(2)              /*!< pin G0P2 channel pin mode */
#define TSI_CHCFG_G0P3          BIT(3)              /*!< pin G0P3 channel pin mode */
#define TSI_CHCFG_G1P0          BIT(4)              /*!< pin G1P0 channel pin mode */
#define TSI_CHCFG_G1P1          BIT(5)              /*!< pin G1P1 channel pin mode */
#define TSI_CHCFG_G1P2          BIT(6)              /*!< pin G1P2 channel pin mode */
#define TSI_CHCFG_G1P3          BIT(7)              /*!< pin G1P3 channel pin mode */
#define TSI_CHCFG_G2P0          BIT(8)              /*!< pin G2P0 channel pin mode */
#define TSI_CHCFG_G2P1          BIT(9)              /*!< pin G2P1 channel pin mode */
#define TSI_CHCFG_G2P2          BIT(10)             /*!< pin G2P2 channel pin mode */
#define TSI_CHCFG_G2P3          BIT(11)             /*!< pin G2P3 channel pin mode */
#define TSI_CHCFG_G3P0          BIT(12)             /*!< pin G3P0 channel pin mode */
#define TSI_CHCFG_G3P1          BIT(13)             /*!< pin G3P1 channel pin mode */
#define TSI_CHCFG_G3P2          BIT(14)             /*!< pin G3P2 channel pin mode */
#define TSI_CHCFG_G3P3          BIT(15)             /*!< pin G3P3 channel pin mode */
#define TSI_CHCFG_G4P0          BIT(16)             /*!< pin G4P0 channel pin mode */
#define TSI_CHCFG_G4P1          BIT(17)             /*!< pin G4P1 channel pin mode */
#define TSI_CHCFG_G4P2          BIT(18)             /*!< pin G4P2 channel pin mode */
#define TSI_CHCFG_G4P3          BIT(19)             /*!< pin G4P3 channel pin mode */
#define TSI_CHCFG_G5P0          BIT(20)             /*!< pin G5P0 channel pin mode */
#define TSI_CHCFG_G5P1          BIT(21)             /*!< pin G5P1 channel pin mode */
#define TSI_CHCFG_G5P2          BIT(22)             /*!< pin G5P2 channel pin mode */
#define TSI_CHCFG_G5P3          BIT(23)             /*!< pin G5P3 channel pin mode */

/* TSI_GCTL */
#define TSI_GCTL_GE0            BIT(0)              /*!< group0 enable */
#define TSI_GCTL_GE1            BIT(1)              /*!< group1 enable */
#define TSI_GCTL_GE2            BIT(2)              /*!< group2 enable */
#define TSI_GCTL_GE3            BIT(3)              /*!< group3 enable */
#define TSI_GCTL_GE4            BIT(4)              /*!< group4 enable */
#define TSI_GCTL_GE5            BIT(5)              /*!< group5 enable */
#define TSI_GCTL_GC0            BIT(16)             /*!< group0 complete */
#define TSI_GCTL_GC1            BIT(17)             /*!< group1 complete */
#define TSI_GCTL_GC2            BIT(18)             /*!< group2 complete */
#define TSI_GCTL_GC3            BIT(19)             /*!< group3 complete */
#define TSI_GCTL_GC4            BIT(20)             /*!< group4 complete */
#define TSI_GCTL_GC5            BIT(21)             /*!< group5 complete */

/* constants definitions */
/* CTCLK clock division factor */
#define CTL_CTCDIV(regval)      (BITS(12,14) & ((regval) << 12U))   /*!< CTCLK clock division factor */
#define TSI_CTCDIV_DIV1         CTL_CTCDIV(0)                       /*!< fCTCLK = fHCLK */
#define TSI_CTCDIV_DIV2         CTL_CTCDIV(1)                       /*!< fCTCLK = fHCLK/2 */
#define TSI_CTCDIV_DIV4         CTL_CTCDIV(2)                       /*!< fCTCLK = fHCLK/4 */
#define TSI_CTCDIV_DIV8         CTL_CTCDIV(3)                       /*!< fCTCLK = fHCLK/8 */
#define TSI_CTCDIV_DIV16        CTL_CTCDIV(4)                       /*!< fCTCLK = fHCLK/16 */
#define TSI_CTCDIV_DIV32        CTL_CTCDIV(5)                       /*!< fCTCLK = fHCLK/32 */
#define TSI_CTCDIV_DIV64        CTL_CTCDIV(6)                       /*!< fCTCLK = fHCLK/64 */
#define TSI_CTCDIV_DIV128       CTL_CTCDIV(7)                       /*!< fCTCLK = fHCLK/128 */

/* charge transfer state duration Time */
#define CTL_CTDT(regval)        (BITS(24,27) & ((regval) << 24U))   /*!< charge transfer state duration time */
#define TSI_TRANSFER_1CTCLK     CTL_CTDT(0)                         /*!< the duration time of transfer state is 1 CTCLK */
#define TSI_TRANSFER_2CTCLK     CTL_CTDT(1)                         /*!< the duration time of transfer state is 2 CTCLK */
#define TSI_TRANSFER_3CTCLK     CTL_CTDT(2)                         /*!< the duration time of transfer state is 3 CTCLK */
#define TSI_TRANSFER_4CTCLK     CTL_CTDT(3)                         /*!< the duration time of transfer state is 4 CTCLK */
#define TSI_TRANSFER_5CTCLK     CTL_CTDT(4)                         /*!< the duration time of transfer state is 5 CTCLK */
#define TSI_TRANSFER_6CTCLK     CTL_CTDT(5)                         /*!< the duration time of transfer state is 6 CTCLK */
#define TSI_TRANSFER_7CTCLK     CTL_CTDT(6)                         /*!< the duration time of transfer state is 7 CTCLK */
#define TSI_TRANSFER_8CTCLK     CTL_CTDT(7)                         /*!< the duration time of transfer state is 8 CTCLK */
#define TSI_TRANSFER_9CTCLK     CTL_CTDT(8)                         /*!< the duration time of transfer state is 9 CTCLK */
#define TSI_TRANSFER_10CTCLK    CTL_CTDT(9)                         /*!< the duration time of transfer state is 10 CTCLK */
#define TSI_TRANSFER_11CTCLK    CTL_CTDT(10)                        /*!< the duration time of transfer state is 11 CTCLK */
#define TSI_TRANSFER_12CTCLK    CTL_CTDT(11)                        /*!< the duration time of transfer state is 12 CTCLK */
#define TSI_TRANSFER_13CTCLK    CTL_CTDT(12)                        /*!< the duration time of transfer state is 13 CTCLK */
#define TSI_TRANSFER_14CTCLK    CTL_CTDT(13)                        /*!< the duration time of transfer state is 14 CTCLK */
#define TSI_TRANSFER_15CTCLK    CTL_CTDT(14)                        /*!< the duration time of transfer state is 15 CTCLK */
#define TSI_TRANSFER_16CTCLK    CTL_CTDT(15)                        /*!< the duration time of transfer state is 16 CTCLK */

/* charge state duration time */
#define CTL_CDT(regval)         (BITS(28,31) & ((regval) << 28U))   /*!< charge state duration time */
#define TSI_CHARGE_1CTCLK       CTL_CDT(0)                          /*!< the duration time of charge state is 1 CTCLK */
#define TSI_CHARGE_2CTCLK       CTL_CDT(1)                          /*!< the duration time of charge state is 2 CTCLK */
#define TSI_CHARGE_3CTCLK       CTL_CDT(2)                          /*!< the duration time of charge state is 3 CTCLK */
#define TSI_CHARGE_4CTCLK       CTL_CDT(3)                          /*!< the duration time of charge state is 4 CTCLK */
#define TSI_CHARGE_5CTCLK       CTL_CDT(4)                          /*!< the duration time of charge state is 5 CTCLK */
#define TSI_CHARGE_6CTCLK       CTL_CDT(5)                          /*!< the duration time of charge state is 6 CTCLK */
#define TSI_CHARGE_7CTCLK       CTL_CDT(6)                          /*!< the duration time of charge state is 7 CTCLK */
#define TSI_CHARGE_8CTCLK       CTL_CDT(7)                          /*!< the duration time of charge state is 8 CTCLK */
#define TSI_CHARGE_9CTCLK       CTL_CDT(8)                          /*!< the duration time of charge state is 9 CTCLK */
#define TSI_CHARGE_10CTCLK      CTL_CDT(9)                          /*!< the duration time of charge state is 10 CTCLK */
#define TSI_CHARGE_11CTCLK      CTL_CDT(10)                         /*!< the duration time of charge state is 11 CTCLK */
#define TSI_CHARGE_12CTCLK      CTL_CDT(11)                         /*!< the duration time of charge state is 12 CTCLK */
#define TSI_CHARGE_13CTCLK      CTL_CDT(12)                         /*!< the duration time of charge state is 13 CTCLK */
#define TSI_CHARGE_14CTCLK      CTL_CDT(13)                         /*!< the duration time of charge state is 14 CTCLK */
#define TSI_CHARGE_15CTCLK      CTL_CDT(14)                         /*!< the duration time of charge state is 15 CTCLK */
#define TSI_CHARGE_16CTCLK      CTL_CDT(15)                         /*!< the duration time of charge state is 16 CTCLK */

/* max cycle number of a sequence */
#define CTL_MCN(regval)         (BITS(5,7) & ((regval) << 5U))      /*!< max cycle number of a sequence */
#define TSI_MAXNUM255           CTL_MCN(0)                          /*!< the max cycle number of a sequence is 255 */
#define TSI_MAXNUM511           CTL_MCN(1)                          /*!< the max cycle number of a sequence is 511 */
#define TSI_MAXNUM1023          CTL_MCN(2)                          /*!< the max cycle number of a sequence is 1023 */
#define TSI_MAXNUM2047          CTL_MCN(3)                          /*!< the max cycle number of a sequence is 2047 */
#define TSI_MAXNUM4095          CTL_MCN(4)                          /*!< the max cycle number of a sequence is 4095 */
#define TSI_MAXNUM8191          CTL_MCN(5)                          /*!< the max cycle number of a sequence is 8191 */
#define TSI_MAXNUM16383         CTL_MCN(6)                          /*!< the max cycle number of a sequence is 16383 */

/* ECCLK clock division factor */
#define TSI_EXTEND_DIV1         ((uint32_t)0x00000000U)             /*!< fECCLK = fHCLK */
#define TSI_EXTEND_DIV2         ((uint32_t)0x00000001U)             /*!< fECCLK = fHCLK/2 */

/* Extend Charge State Maximum Duration Time */
#define TSI_EXTENDMAX(regval)   (BITS(17,23) & ((regval) << 17U))   /* value range 1...128,extend charge state maximum duration time */

/* hardware trigger mode */
#define TSI_FALLING_TRIGGER     ((uint32_t)0x00000000U)             /*!< falling edge trigger TSI charge transfer sequence */
#define TSI_RISING_TRIGGER      ((uint32_t)0x00000001U)             /*!< rising edge trigger TSI charge transfer sequence */

/* pin mode */
#define TSI_OUTPUT_LOW          ((uint32_t)0x00000000U)             /*!< TSI pin will output low when IDLE */
#define TSI_INPUT_FLOATING      ((uint32_t)0x00000001U)             /*!< TSI pin will keep input_floating when IDLE */

/* interrupt enable bits */
#define TSI_INT_CTCF            TSI_INTEN_CTCFIE                    /*!< charge transfer complete flag interrupt enable */
#define TSI_INTEN_MNERR         TSI_INTEN_MNERRIE                   /*!< max cycle number error interrupt enable */

/* interrupt flag bits */
#define TSI_INT_FLAG_CTC        TSI_INTF_CTCF                       /*!< charge transfer complete flag */
#define TSI_INT_FLAG_MNERR      TSI_INTF_MNERR                      /*!< max cycle number error */

/* function declarations */
/* reset TSI peripheral */
void tsi_deinit(void);
/* initialize TSI plus prescaler,charge plus,transfer plus,max cycle number */
void tsi_init(uint32_t prescaler,uint32_t charge_duration,uint32_t transfer_duration,uint32_t max_number);
/* enable TSI module */
void tsi_enable(void);
/* disable TSI module */
void tsi_disable(void);
/* enable sample pin */
void tsi_sample_pin_enable(uint32_t sample);
/* disable sample pin */
void tsi_sample_pin_disable(uint32_t sample);
/* enable channel pin */
void tsi_channel_pin_enable(uint32_t channel);
/* disable channel pin */
void tsi_channel_pin_disable(uint32_t channel);

/* configure TSI triggering by software */
void tsi_sofeware_mode_config(void);
/* start a charge-transfer sequence when TSI is in software trigger mode */
void tsi_software_start(void);
/* stop a charge-transfer sequence when TSI is in software trigger mode */
void tsi_software_stop(void);
/* configure TSI triggering by hardware */
void tsi_hardware_mode_config(uint8_t trigger_edge);
/* configure TSI pin mode when charge-transfer sequence is IDLE */
void tsi_pin_mode_config(uint8_t pin_mode);
/* configure extend charge state */
void tsi_extend_charge_config(ControlStatus extend,uint8_t prescaler,uint32_t max_duration);

/* configure charge plus and transfer plus */
void tsi_plus_config(uint32_t prescaler,uint32_t charge_duration,uint32_t transfer_duration);
/* configure the max cycle number of a charge-transfer sequence */
void tsi_max_number_config(uint32_t max_number);
/* switch on hysteresis pin */
void tsi_hysteresis_on(uint32_t group_pin);
/* switch off hysteresis pin */
void tsi_hysteresis_off(uint32_t group_pin);
/* switch on analog pin */
void tsi_analog_on(uint32_t group_pin);
/* switch off analog pin */
void tsi_analog_off(uint32_t group_pin);

/* enable TSI interrupt */
void tsi_interrupt_enable(uint32_t source);
/* disable TSI interrupt */
void tsi_interrupt_disable(uint32_t source);
/* clear interrupt flag */
void tsi_interrupt_flag_clear(uint32_t flag);
/* get TSI module current status */
FlagStatus tsi_interrupt_flag_get(uint32_t status);

/* enbale group */
void tsi_group_enable(uint32_t group);
/* disbale group */
void tsi_group_disable(uint32_t group);
/* get group complete status */
FlagStatus tsi_group_status_get(uint32_t group);
/* get the cycle number for group0 as soon as a charge-transfer sequence completes */
uint16_t tsi_group0_cycle_get(void);
/* get the cycle number for group1 as soon as a charge-transfer sequence completes */
uint16_t tsi_group1_cycle_get(void);
/* get the cycle number for group2 as soon as a charge-transfer sequence completes */
uint16_t tsi_group2_cycle_get(void);
/* get the cycle number for group3 as soon as a charge-transfer sequence completes */
uint16_t tsi_group3_cycle_get(void);
/* get the cycle number for group4 as soon as a charge-transfer sequence completes */
uint16_t tsi_group4_cycle_get(void);
/* get the cycle number for group5 as soon as a charge-transfer sequence completes */
uint16_t tsi_group5_cycle_get(void);

#endif /* GD32F1X0_TSI_H */

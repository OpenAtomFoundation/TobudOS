/**************************************************************************//**
 * @file     pwm_reg.h
 * @version  V1.00
 * @brief    PWM register definition header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __PWM_REG_H__
#define __PWM_REG_H__

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup PWM Pulse Width Modulation Controller (PWM)
    Memory Mapped Structure for PWM Controller
@{ */

typedef struct
{
    /**
     * @var CAPDAT_T::RCAPDAT
     * Offset: 0x20C~0x238  PWM Rising Capture Data Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RCAPDAT   |PWM Rising Capture Data (Read Only)
     * |        |          |When rising capture condition happened, the PWM counter value will be saved in this register.
     * @var CAPDAT_T::FCAPDAT
     * Offset: 0x210  PWM Falling Capture Data Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |FCAPDAT   |PWWM Falling Capture Data (Read Only)
     * |        |          |When falling capture condition happened, the PWWM counter value will be saved in this register.
     */
    __IO uint32_t RCAPDAT; /*!< [0x20C/0x214/0x21C/0x224/0x22C/0x234] PWM Rising Capture Data Register 0~5 */
    __IO uint32_t FCAPDAT; /*!< [0x210/0x218/0x220/0x228/0x230/0x238] PWM Falling Capture Data Register 0~5 */
} CAPDAT_T;

typedef struct
{


    /**
     * @var PWM_T::CTL0
     * Offset: 0x00  PWM Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CTRLDn    |Center Load Enable Bits
     * |        |          |In up-down counter type, PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the center point of a period
     * |[16]    |IMMLDENn  |Immediately Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD/CMPDAT will load to PBUF and CMPBUF immediately when software update PERIOD/CMPDAT.
     * |        |          |Note: If IMMLDENn is enabled, WINLDENn and CTRLDn will be invalid.
     * |[30]    |DBGHALT   |ICE Debug Mode Counter Halt (Write Protect)
     * |        |          |If counter halt is enabled, PWM all counters will keep current value until exit ICE debug mode.
     * |        |          |0 = ICE debug mode counter halt Disable.
     * |        |          |1 = ICE debug mode counter halt Enable.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[31]    |DBGTRIOFF |ICE Debug Mode Acknowledge Disable Bit (Write Protect)
     * |        |          |0 = ICE debug mode acknowledgement effects PWM output.
     * |        |          |PWM pin will be forced as tri-state while ICE debug mode acknowledged.
     * |        |          |1 = ICE debug mode acknowledgement disabled.
     * |        |          |PWM pin will keep output no matter ICE debug mode acknowledged or not.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * @var PWM_T::CTL1
     * Offset: 0x04  PWM Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |CNTTYPE0  |PWM Counter Behavior Type 0
     * |        |          |The two bits control channel1 and channel0
     * |        |          |00 = Up counter type (supported in capture mode).
     * |        |          |01 = Down count type (supported in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[5:4]   |CNTTYPE2  |PWM Counter Behavior Type 2
     * |        |          |The two bits control channel3 and channel2
     * |        |          |00 = Up counter type (supported in capture mode).
     * |        |          |01 = Down count type (supported in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[9:8]   |CNTTYPE4  |PWM Counter Behavior Type 4
     * |        |          |The two bits control channel5 and channel4
     * |        |          |00 = Up counter type (supported in capture mode).
     * |        |          |01 = Down count type (supported in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[26:24] |OUTMODEn  |PWM Output Mode
     * |        |          |Each bit n controls the output mode of corresponding PWM channel n.
     * |        |          |0 = PWM independent mode.
     * |        |          |1 = PWM complementary mode.
     * |        |          |Note: When operating in group function, these bits must all set to the same mode.
     * @var PWM_T::CLKSRC
     * Offset: 0x10  PWM Clock Source Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |ECLKSRC0  |PWM_CH01 External Clock Source Select
     * |        |          |000 = PWMx_CLK, x denotes 0 or 1.
     * |        |          |001 = TIMER0 overflow.
     * |        |          |010 = TIMER1 overflow.
     * |        |          |011 = TIMER2 overflow.
     * |        |          |100 = TIMER3 overflow.
     * |        |          |Others = Reserved.
     * |[10:8]  |ECLKSRC2  |PWM_CH23 External Clock Source Select
     * |        |          |000 = PWMx_CLK, x denotes 0 or 1.
     * |        |          |001 = TIMER0 overflow.
     * |        |          |010 = TIMER1 overflow.
     * |        |          |011 = TIMER2 overflow.
     * |        |          |100 = TIMER3 overflow.
     * |        |          |Others = Reserved.
     * |[18:16] |ECLKSRC4  |PWM_CH45 External Clock Source Select
     * |        |          |000 = PWMx_CLK, x denotes 0 or 1.
     * |        |          |001 = TIMER0 overflow.
     * |        |          |010 = TIMER1 overflow.
     * |        |          |011 = TIMER2 overflow.
     * |        |          |100 = TIMER3 overflow.
     * |        |          |Others = Reserved.
     * @var PWM_T::CLKPSC[3]
     * Offset: 0x14~0x1C  PWM Clock Prescale Register 0/1, 2/3, 4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |CLKPSC    |PWM Counter Clock Prescale
     * |        |          |The clock of PWM counter is decided by clock prescaler
     * |        |          |Each PWM pair share one PWM counter clock prescaler
     * |        |          |The clock of PWM counter is divided by (CLKPSC+ 1)
     * @var PWM_T::CNTEN
     * Offset: 0x20  PWM Counter Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTEN0    |PWM Counter Enable Bit 0
     * |        |          |0 = PWM Counter and clock prescaler Stop Running.
     * |        |          |1 = PWM Counter and clock prescaler Start Running.
     * |[2]     |CNTEN2    |PWM Counter Enable Bit 2
     * |        |          |0 = PWM Counter and clock prescaler Stop Running.
     * |        |          |1 = PWM Counter and clock prescaler Start Running.
     * |[4]     |CNTEN4    |PWM Counter Enable Bit 4
     * |        |          |0 = PWM Counter and clock prescaler Stop Running.
     * |        |          |1 = PWM Counter and clock prescaler Start Running.
     * @var PWM_T::CNTCLR
     * Offset: 0x24  PWM Clear Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTCLR0   |Clear PWM Counter Control Bit 0
     * |        |          |It is automatically cleared by hardware.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit PWM counter to 0000H.
     * |[2]     |CNTCLR2   |Clear PWM Counter Control Bit 2
     * |        |          |It is automatically cleared by hardware.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit PWM counter to 0000H.
     * |[4]     |CNTCLR4   |Clear PWM Counter Control Bit 4
     * |        |          |It is automatically cleared by hardware.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit PWM counter to 0000H.
     * @var PWM_T::PERIOD[6]
     * Offset: 0x30~0x44  PWM Period Register 0, 2, 4
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PERIOD    |PWM Period Register
     * |        |          |Up-Count mode: In this mode, PWM counter counts from 0 to PERIOD, and restarts from 0.
     * |        |          |Down-Count mode: In this mode, PWM counter counts from PERIOD to 0, and restarts from PERIOD.
     * |        |          |PWM period time = (PERIOD+1) * PWM_CLK period.
     * |        |          |Up-Down-Count mode: In this mode, PWM counter counts from 0 to PERIOD, then decrements to 0 and repeats again.
     * |        |          |PWM period time = 2 * PERIOD * PWM_CLK period.
     * @var PWM_T::CMPDAT[6]
     * Offset: 0x50~0x64  PWM Comparator Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CMP       |PWM Comparator Register
     * |        |          |CMP is used to compare with CNTR to generate PWM waveform, interrupt and trigger EADC.
     * |        |          |In independent mode, PWM_CMPDAT0~5 denote as 6 independent PWM_CH0~5 compared point.
     * |        |          |In complementary mode, PWM_CMPDAT0, 2, 4 denote as first compared point, and PWM_CMPDAT1, 3, 5 denote as second compared point for the corresponding 3 complementary pairs PWM_CH0 and PWM_CH1, PWM_CH2 and PWM_CH3, PWM_CH4 and PWM_CH5.
     * @var PWM_T::DTCTL[3]
     * Offset: 0x70~0x78  PWM Dead-time Control Register 0/1, 2/3, 4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |DTCNT     |Dead-time Counter (Write Protect)
     * |        |          |The dead-time can be calculated from the following formula:
     * |        |          |DTCKSEL=0: Dead-time = (DTCNT[11:0]+1) * PWM_CLK period.
     * |        |          |DTCKSEL=1: Dead-time = (DTCNT[11:0]+1) * PWM_CLK period * (CLKPSC+1).
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[16]    |DTEN      |Enable Dead-time Insertion for PWM Pair (PWM_CH0, PWM_CH1) (PWM_CH2, PWM_CH3) (PWM_CH4, PWM_CH5) (Write Protect)
     * |        |          |Dead-time insertion is only active when this pair of complementary PWM is enabled
     * |        |          |If dead- time insertion is inactive, the outputs of pin pair are complementary without any delay.
     * |        |          |0 = Dead-time insertion Disabled on the pin pair.
     * |        |          |1 = Dead-time insertion Enabled on the pin pair.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[24]    |DTCKSEL   |Dead-time Clock Select (Write Protect)
     * |        |          |0 = Dead-time clock source from PWM_CLK.
     * |        |          |1 = Dead-time clock source from prescaler output.
     * |        |          |Note: This bit is write protected. Refer to REGWRPROT register.
     * @var PWM_T::CNT[6]
     * Offset: 0x90~0xA4  PWM Counter Register 0, 2, 4
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CNT       |PWM Data Register (Read Only)
     * |        |          |User can monitor CNTR to know the current value in 16-bit period counter.
     * |[16]    |DIRF      |PWM Direction Indicator Flag (Read Only)
     * |        |          |0 = Counter is counting down.
     * |        |          |1 = Counter is counting up.
     * @var PWM_T::WGCTL0
     * Offset: 0xB0  PWM Generation Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |ZPCTL0    |PWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM zero point output Low.
     * |        |          |10 = PWM zero point output High.
     * |        |          |11 = PWM zero point output Toggle.
     * |        |          |Note: PWM can control output level when PWM counter counts to 0.
     * |[3:2]   |ZPCTL1    |PWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM zero point output Low.
     * |        |          |10 = PWM zero point output High.
     * |        |          |11 = PWM zero point output Toggle.
     * |        |          |Note: PWM can control output level when PWM counter counts to 0.
     * |[5:4]   |ZPCTL2    |PWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM zero point output Low.
     * |        |          |10 = PWM zero point output High.
     * |        |          |11 = PWM zero point output Toggle.
     * |        |          |Note: PWM can control output level when PWM counter counts to 0.
     * |[7:6]   |ZPCTL3    |PWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM zero point output Low.
     * |        |          |10 = PWM zero point output High.
     * |        |          |11 = PWM zero point output Toggle.
     * |        |          |Note: PWM can control output level when PWM counter counts to 0.
     * |[9:8]   |ZPCTL4    |PWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM zero point output Low.
     * |        |          |10 = PWM zero point output High.
     * |        |          |11 = PWM zero point output Toggle.
     * |        |          |Note: PWM can control output level when PWM counter counts to 0.
     * |[11:10] |ZPCTL5    |PWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM zero point output Low.
     * |        |          |10 = PWM zero point output High.
     * |        |          |11 = PWM zero point output Toggle.
     * |        |          |Note: PWM can control output level when PWM counter counts to 0.
     * |[17:16] |PRDPCTL0  |PWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM period (center) point output Low.
     * |        |          |10 = PWM period (center) point output High.
     * |        |          |11 = PWM period (center) point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts to (PERIODn+1).
     * |        |          |Note2: This bit is center point control when PWM counter operating in up-down counter type.
     * |[19:18] |PRDPCTL1  |PWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM period (center) point output Low.
     * |        |          |10 = PWM period (center) point output High.
     * |        |          |11 = PWM period (center) point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts to (PERIODn+1).
     * |        |          |Note2: This bit is center point control when PWM counter operating in up-down counter type.
     * |[21:20] |PRDPCTL2  |PWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM period (center) point output Low.
     * |        |          |10 = PWM period (center) point output High.
     * |        |          |11 = PWM period (center) point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts to (PERIODn+1).
     * |        |          |Note2: This bit is center point control when PWM counter operating in up-down counter type.
     * |[23:22] |PRDPCTL3  |PWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM period (center) point output Low.
     * |        |          |10 = PWM period (center) point output High.
     * |        |          |11 = PWM period (center) point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts to (PERIODn+1).
     * |        |          |Note2: This bit is center point control when PWM counter operating in up-down counter type.
     * |[25:24] |PRDPCTL4  |PWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM period (center) point output Low.
     * |        |          |10 = PWM period (center) point output High.
     * |        |          |11 = PWM period (center) point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts to (PERIODn+1).
     * |        |          |Note2: This bit is center point control when PWM counter operating in up-down counter type.
     * |[27:26] |PRDPCTL5  |PWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM period (center) point output Low.
     * |        |          |10 = PWM period (center) point output High.
     * |        |          |11 = PWM period (center) point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts to (PERIODn+1).
     * |        |          |Note2: This bit is center point control when PWM counter operating in up-down counter type.
     * @var PWM_T::WGCTL1
     * Offset: 0xB4  PWM Generation Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |CMPUCTL0  |PWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare up point output Low.
     * |        |          |10 = PWM compare up point output High.
     * |        |          |11 = PWM compare up point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts up to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPUCTL1, 3, 5 is used as another CMPUCTL for channel 0, 2, 4.
     * |[3:2]   |CMPUCTL1  |PWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare up point output Low.
     * |        |          |10 = PWM compare up point output High.
     * |        |          |11 = PWM compare up point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts up to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPUCTL1, 3, 5 is used as another CMPUCTL for channel 0, 2, 4.
     * |[5:4]   |CMPUCTL2  |PWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare up point output Low.
     * |        |          |10 = PWM compare up point output High.
     * |        |          |11 = PWM compare up point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts up to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPUCTL1, 3, 5 is used as another CMPUCTL for channel 0, 2, 4.
     * |[7:6]   |CMPUCTL3  |PWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare up point output Low.
     * |        |          |10 = PWM compare up point output High.
     * |        |          |11 = PWM compare up point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts up to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPUCTL1, 3, 5 is used as another CMPUCTL for channel 0, 2, 4.
     * |[9:8]   |CMPUCTL4  |PWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare up point output Low.
     * |        |          |10 = PWM compare up point output High.
     * |        |          |11 = PWM compare up point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts up to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPUCTL1, 3, 5 is used as another CMPUCTL for channel 0, 2, 4.
     * |[11:10] |CMPUCTL5  |PWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare up point output Low.
     * |        |          |10 = PWM compare up point output High.
     * |        |          |11 = PWM compare up point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts up to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPUCTL1, 3, 5 is used as another CMPUCTL for channel 0, 2, 4.
     * |[17:16] |CMPDCTL0  |PWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare down point output Low.
     * |        |          |10 = PWM compare down point output High.
     * |        |          |11 = PWM compare down point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts down to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPDCTL1, 3, 5 is used as another CMPDCTL for channel 0, 2, 4.
     * |[19:18] |CMPDCTL1  |PWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare down point output Low.
     * |        |          |10 = PWM compare down point output High.
     * |        |          |11 = PWM compare down point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts down to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPDCTL1, 3, 5 is used as another CMPDCTL for channel 0, 2, 4.
     * |[21:20] |CMPDCTL2  |PWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare down point output Low.
     * |        |          |10 = PWM compare down point output High.
     * |        |          |11 = PWM compare down point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts down to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPDCTL1, 3, 5 is used as another CMPDCTL for channel 0, 2, 4.
     * |[23:22] |CMPDCTL3  |PWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare down point output Low.
     * |        |          |10 = PWM compare down point output High.
     * |        |          |11 = PWM compare down point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts down to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPDCTL1, 3, 5 is used as another CMPDCTL for channel 0, 2, 4.
     * |[25:24] |CMPDCTL4  |PWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare down point output Low.
     * |        |          |10 = PWM compare down point output High.
     * |        |          |11 = PWM compare down point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts down to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPDCTL1, 3, 5 is used as another CMPDCTL for channel 0, 2, 4.
     * |[27:26] |CMPDCTL5  |PWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = PWM compare down point output Low.
     * |        |          |10 = PWM compare down point output High.
     * |        |          |11 = PWM compare down point output Toggle.
     * |        |          |Note1: PWM can control output level when PWM counter counts down to CMPDAT.
     * |        |          |Note2: In complementary mode, CMPDCTL1, 3, 5 is used as another CMPDCTL for channel 0, 2, 4.
     * @var PWM_T::MSKEN
     * Offset: 0xB8  PWM Mask Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MSKEN0    |PWM Mask Enable Bits
     * |        |          |The PWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding PWM channel n will output MSKDATn (PWM_MSK[5:0]) data.
     * |        |          |0 = PWM output signal is non-masked.
     * |        |          |1 = PWM output signal is masked and output MSKDATn data.
     * |[1]     |MSKEN1    |PWM Mask Enable Bits
     * |        |          |The PWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding PWM channel n will output MSKDATn (PWM_MSK[5:0]) data.
     * |        |          |0 = PWM output signal is non-masked.
     * |        |          |1 = PWM output signal is masked and output MSKDATn data.
     * |[2]     |MSKEN2    |PWM Mask Enable Bits
     * |        |          |The PWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding PWM channel n will output MSKDATn (PWM_MSK[5:0]) data.
     * |        |          |0 = PWM output signal is non-masked.
     * |        |          |1 = PWM output signal is masked and output MSKDATn data.
     * |[3]     |MSKEN3    |PWM Mask Enable Bits
     * |        |          |The PWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding PWM channel n will output MSKDATn (PWM_MSK[5:0]) data.
     * |        |          |0 = PWM output signal is non-masked.
     * |        |          |1 = PWM output signal is masked and output MSKDATn data.
     * |[4]     |MSKEN4    |PWM Mask Enable Bits
     * |        |          |The PWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding PWM channel n will output MSKDATn (PWM_MSK[5:0]) data.
     * |        |          |0 = PWM output signal is non-masked.
     * |        |          |1 = PWM output signal is masked and output MSKDATn data.
     * |[5]     |MSKEN5    |PWM Mask Enable Bits
     * |        |          |The PWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding PWM channel n will output MSKDATn (PWM_MSK[5:0]) data.
     * |        |          |0 = PWM output signal is non-masked.
     * |        |          |1 = PWM output signal is masked and output MSKDATn data.
     * @var PWM_T::MSK
     * Offset: 0xBC  PWM Mask Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MSKDAT0   |PWM Mask Data Bit
     * |        |          |This data bit control the state of PWMn output pin, if corresponding mask function is enabled
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Output logic low to PWM channel n.
     * |        |          |1 = Output logic high to PWM channel n.
     * |[1]     |MSKDAT1   |PWM Mask Data Bit
     * |        |          |This data bit control the state of PWMn output pin, if corresponding mask function is enabled
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Output logic low to PWM channel n.
     * |        |          |1 = Output logic high to PWM channel n.
     * |[2]     |MSKDAT2   |PWM Mask Data Bit
     * |        |          |This data bit control the state of PWMn output pin, if corresponding mask function is enabled
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Output logic low to PWM channel n.
     * |        |          |1 = Output logic high to PWM channel n.
     * |[3]     |MSKDAT3   |PWM Mask Data Bit
     * |        |          |This data bit control the state of PWMn output pin, if corresponding mask function is enabled
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Output logic low to PWM channel n.
     * |        |          |1 = Output logic high to PWM channel n.
     * |[4]     |MSKDAT4   |PWM Mask Data Bit
     * |        |          |This data bit control the state of PWMn output pin, if corresponding mask function is enabled
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Output logic low to PWM channel n.
     * |        |          |1 = Output logic high to PWM channel n.
     * |[5]     |MSKDAT5   |PWM Mask Data Bit
     * |        |          |This data bit control the state of PWMn output pin, if corresponding mask function is enabled
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Output logic low to PWM channel n.
     * |        |          |1 = Output logic high to PWM channel n.
     * @var PWM_T::BNF
     * Offset: 0xC0  PWM Brake Noise Filter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRK0NFEN  |PWM Brake 0 Noise Filter Enable Bit
     * |        |          |0 = Noise filter of PWM Brake 0 Disabled.
     * |        |          |1 = Noise filter of PWM Brake 0 Enabled.
     * |[3:1]   |BRK0NFSEL |Brake 0 Edge Detector Filter Clock Selection
     * |        |          |000 = Filter clock = HCLK.
     * |        |          |001 = Filter clock = HCLK/2.
     * |        |          |010 = Filter clock = HCLK/4.
     * |        |          |011 = Filter clock = HCLK/8.
     * |        |          |100 = Filter clock = HCLK/16.
     * |        |          |101 = Filter clock = HCLK/32.
     * |        |          |110 = Filter clock = HCLK/64.
     * |        |          |111 = Filter clock = HCLK/128.
     * |[6:4]   |BRK0NFCNT |Brake 0 Edge Detector Filter Count
     * |        |          |The register bits control the Brake0 filter counter to count from 0 to BRK1FCNT.
     * |[7]     |BRK0PINV  |Brake 0 Pin Inverse
     * |        |          |0 = The state of pin PWMx_BRAKE0 is passed to the negative edge detector.
     * |        |          |1 = The inversed state of pin PWMx_BRAKE10 is passed to the negative edge detector.
     * |[8]     |BRK1FEN   |PWM Brake 1 Noise Filter Enable Bit
     * |        |          |0 = Noise filter of PWM Brake 1 Disabled.
     * |        |          |1 = Noise filter of PWM Brake 1 Enabled.
     * |[11:9]  |BRK1NFSEL |Brake 1 Edge Detector Filter Clock Selection
     * |        |          |000 = Filter clock = HCLK.
     * |        |          |001 = Filter clock = HCLK/2.
     * |        |          |010 = Filter clock = HCLK/4.
     * |        |          |011 = Filter clock = HCLK/8.
     * |        |          |100 = Filter clock = HCLK/16.
     * |        |          |101 = Filter clock = HCLK/32.
     * |        |          |110 = Filter clock = HCLK/64.
     * |        |          |111 = Filter clock = HCLK/128.
     * |[14:12] |BRK1FCNT  |Brake 1 Edge Detector Filter Count
     * |        |          |The register bits control the Brake1 filter counter to count from 0 to BRK1FCNT.
     * |[15]    |BRK1PINV  |Brake 1 Pin Inverse
     * |        |          |0 = The state of pin PWMx_BRAKE1 is passed to the negative edge detector.
     * |        |          |1 = The inversed state of pin PWMx_BRAKE1 is passed to the negative edge detector.
     * |[16]    |BK0SRC    |Brake 0 Pin Source Select
     * |        |          |For PWM0 setting:
     * |        |          |0 = Brake 0 pin source come from PWM0_BRAKE0.
     * |        |          |1 = Brake 0 pin source come from PWM1_BRAKE0.
     * |        |          |For PWM1 setting:
     * |        |          |0 = Brake 0 pin source come from PWM1_BRAKE0.
     * |        |          |1 = Brake 0 pin source come from PWM0_BRAKE0.
     * |[24]    |BK1SRC    |Brake 1 Pin Source Select
     * |        |          |For PWM0 setting:
     * |        |          |0 = Brake 1 pin source come from PWM0_BRAKE1.
     * |        |          |1 = Brake 1 pin source come from PWM1_BRAKE1.
     * |        |          |For PWM1 setting:
     * |        |          |0 = Brake 1 pin source come from PWM1_BRAKE1.
     * |        |          |1 = Brake 1 pin source come from PWM0_BRAKE1.
     * @var PWM_T::FAILBRK
     * Offset: 0xC4  PWM System Fail Brake Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CSSBRKEN  |Clock Security System Detection Trigger PWM Brake Function 0 Enable Bit
     * |        |          |0 = Brake Function triggered by CSS detection Disabled.
     * |        |          |1 = Brake Function triggered by CSS detection Enabled.
     * |[1]     |BODBRKEN  |Brown-out Detection Trigger PWM Brake Function 0 Enable Bit
     * |        |          |0 = Brake Function triggered by BOD Disabled.
     * |        |          |1 = Brake Function triggered by BOD Enabled.
     * |[3]     |CORBRKEN  |Core Lockup Detection Trigger PWM Brake Function 0 Enable Bit
     * |        |          |0 = Brake Function triggered by Core lockup detection Disabled.
     * |        |          |1 = Brake Function triggered by Core lockup detection Enabled.
     * @var PWM_T::BRKCTL[3]
     * Offset: 0xC8~0xD0  PWM Brake Edge Detect Control Register 0/1, 2/3, 4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CPO0EBEN  |Enable ACMP0_O Digital Output As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP0_O as edge-detect brake source Disabled.
     * |        |          |1 = ACMP0_O as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[1]     |CPO1EBEN  |Enable ACMP1_O Digital Output As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP1_O as edge-detect brake source Disabled.
     * |        |          |1 = ACMP1_O as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[4]     |BRKP0EEN  |Enable PWMx_BRAKE0 Pin As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = BKP0 pin as edge-detect brake source Disabled.
     * |        |          |1 = BKP0 pin as edge-detect brake source Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[5]     |BRKP1EEN  |Enable PWMx_BRAKE1 Pin As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = BKP1 pin as edge-detect brake source Disabled.
     * |        |          |1 = BKP1 pin as edge-detect brake source Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[7]     |SYSEBEN   |Enable System Fail As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = System Fail condition as edge-detect brake source Disabled.
     * |        |          |1 = System Fail condition as edge-detect brake source Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[8]     |CPO0LBEN  |Enable ACMP0_O Digital Output As Level-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP0_O as level-detect brake source Disabled.
     * |        |          |1 = ACMP0_O as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[9]     |CPO1LBEN  |Enable ACMP1_O Digital Output As Level-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP1_O as level-detect brake source Disabled.
     * |        |          |1 = ACMP1_O as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[12]    |BRKP0LEN  |Enable BKP0 Pin As Level-detect Brake Source (Write Protect)
     * |        |          |0 = PWMx_BRAKE0 pin as level-detect brake source Disabled.
     * |        |          |1 = PWMx_BRAKE0 pin as level-detect brake source Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[13]    |BRKP1LEN  |Enable BKP1 Pin As Level-detect Brake Source (Write Protect)
     * |        |          |0 = PWMx_BRAKE1 pin as level-detect brake source Disabled.
     * |        |          |1 = PWMx_BRAKE1 pin as level-detect brake source Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[15]    |SYSLBEN   |Enable System Fail As Level-detect Brake Source (Write Protect)
     * |        |          |0 = System Fail condition as level-detect brake source Disabled.
     * |        |          |1 = System Fail condition as level-detect brake source Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[17:16] |BRKAEVEN  |PWM Brake Action Select for Even Channel (Write Protect)
     * |        |          |00 = PWM even channel level-detect brake function not affect channel output.
     * |        |          |01 = PWM even channel output tri-state when level-detect brake happened.
     * |        |          |10 = PWM even channel output low level when level-detect brake happened.
     * |        |          |11 = PWM even channel output high level when level-detect brake happened.
     * |        |          |Note: These bits are write protected. Refer to SYS_REGLCTL register.
     * |[19:18] |BRKAODD   |PWM Brake Action Select for Odd Channel (Write Protect)
     * |        |          |00 = PWM odd channel level-detect brake function not affect channel output.
     * |        |          |01 = PWM odd channel output tri-state when level-detect brake happened.
     * |        |          |10 = PWM odd channel output low level when level-detect brake happened.
     * |        |          |11 = PWM odd channel output high level when level-detect brake happened.
     * |        |          |Note: These bits are write protected. Refer to SYS_REGLCTL register.
     * @var PWM_T::POLCTL
     * Offset: 0xD4  PWM Pin Polar Inverse Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PINV0     |PWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of PWM output.
     * |        |          |0 = PWM output polar inverse Disabled.
     * |        |          |1 = PWM output polar inverse Enabled.
     * |[1]     |PINV1     |PWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of PWM output.
     * |        |          |0 = PWM output polar inverse Disabled.
     * |        |          |1 = PWM output polar inverse Enabled.
     * |[2]     |PINV2     |PWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of PWM output.
     * |        |          |0 = PWM output polar inverse Disabled.
     * |        |          |1 = PWM output polar inverse Enabled.
     * |[3]     |PINV3     |PWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of PWM output.
     * |        |          |0 = PWM output polar inverse Disabled.
     * |        |          |1 = PWM output polar inverse Enabled.
     * |[4]     |PINV4     |PWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of PWM output.
     * |        |          |0 = PWM output polar inverse Disabled.
     * |        |          |1 = PWM output polar inverse Enabled.
     * |[5]     |PINV5     |PWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of PWM output.
     * |        |          |0 = PWM output polar inverse Disabled.
     * |        |          |1 = PWM output polar inverse Enabled.
     * @var PWM_T::POEN
     * Offset: 0xD8  PWM Output Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |POEN0     |PWM Pin Output Enable Bits
     * |        |          |0 = PWM pin at tri-state.
     * |        |          |1 = PWM pin in output mode.
     * |[1]     |POEN1     |PWM Pin Output Enable Bits
     * |        |          |0 = PWM pin at tri-state.
     * |        |          |1 = PWM pin in output mode.
     * |[2]     |POEN2     |PWM Pin Output Enable Bits
     * |        |          |0 = PWM pin at tri-state.
     * |        |          |1 = PWM pin in output mode.
     * |[3]     |POEN3     |PWM Pin Output Enable Bits
     * |        |          |0 = PWM pin at tri-state.
     * |        |          |1 = PWM pin in output mode.
     * |[4]     |POEN4     |PWM Pin Output Enable Bits
     * |        |          |0 = PWM pin at tri-state.
     * |        |          |1 = PWM pin in output mode.
     * |[5]     |POEN5     |PWM Pin Output Enable Bits
     * |        |          |0 = PWM pin at tri-state.
     * |        |          |1 = PWM pin in output mode.
     * @var PWM_T::SWBRK
     * Offset: 0xDC  PWM Software Brake Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKETRG0  |PWM Edge Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger Edge brake, and set BRKEIFn to 1 in PWM_INTSTS1 register.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[1]     |BRKETRG2  |PWM Edge Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger Edge brake, and set BRKEIFn to 1 in PWM_INTSTS1 register.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[2]     |BRKETRG4  |PWM Edge Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger Edge brake, and set BRKEIFn to 1 in PWM_INTSTS1 register.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[8]     |BRKLTRG0  |PWM Level Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger level brake, and set BRKLIFn to 1 in PWM_INTSTS1 register.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[9]     |BRKLTRG2  |PWM Level Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger level brake, and set BRKLIFn to 1 in PWM_INTSTS1 register.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[10]    |BRKLTRG4  |PWM Level Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger level brake, and set BRKLIFn to 1 in PWM_INTSTS1 register.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * @var PWM_T::INTEN0
     * Offset: 0xE0  PWM Interrupt Enable Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ZIEN0     |PWM Zero Point Interrupt Enable Bit 0
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[2]     |ZIEN2     |PWM Zero Point Interrupt Enable Bit 2
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[4]     |ZIEN4     |PWM Zero Point Interrupt Enable Bit 4
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[8]     |PIEN0     |PWM Period Point Interrupt Enable Bit 0
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note: When up-down counter type, period point means center point.
     * |[10]    |PIEN2     |PWM Period Point Interrupt Enable Bit 2
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note: When up-down counter type, period point means center point.
     * |[12]    |PIEN4     |PWM Period Point Interrupt Enable Bit 4
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note: When up-down counter type, period point means center point.
     * |[16]    |CMPUIEN0  |PWM Compare Up Count Interrupt Enable Bits
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 is used as another CMPUIEN for channel 0, 2, 4.
     * |[17]    |CMPUIEN1  |PWM Compare Up Count Interrupt Enable Bits
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 is used as another CMPUIEN for channel 0, 2, 4.
     * |[18]    |CMPUIEN2  |PWM Compare Up Count Interrupt Enable Bits
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 is used as another CMPUIEN for channel 0, 2, 4.
     * |[19]    |CMPUIEN3  |PWM Compare Up Count Interrupt Enable Bits
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 is used as another CMPUIEN for channel 0, 2, 4.
     * |[20]    |CMPUIEN4  |PWM Compare Up Count Interrupt Enable Bits
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 is used as another CMPUIEN for channel 0, 2, 4.
     * |[21]    |CMPUIEN5  |PWM Compare Up Count Interrupt Enable Bits
     * |        |          |Each bit n controls the corresponding PWM channel n.
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 is used as another CMPUIEN for channel 0, 2, 4.
     * |[24]    |CMPDIEN0  |PWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 is used as another CMPDIEN for channel 0, 2, 4.
     * |[25]    |CMPDIEN1  |PWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 is used as another CMPDIEN for channel 0, 2, 4.
     * |[26]    |CMPDIEN2  |PWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 is used as another CMPDIEN for channel 0, 2, 4.
     * |[27]    |CMPDIEN3  |PWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 is used as another CMPDIEN for channel 0, 2, 4.
     * |[28]    |CMPDIEN4  |PWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 is used as another CMPDIEN for channel 0, 2, 4.
     * |[29]    |CMPDIEN5  |PWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 is used as another CMPDIEN for channel 0, 2, 4.
     * @var PWM_T::INTEN1
     * Offset: 0xE4  PWM Interrupt Enable Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKEIEN0_1|PWM Edge-detect Brake Interrupt Enable for Channel0/1 (Write Protect)
     * |        |          |0 = Edge-detect Brake interrupt for channel0/1 Disabled.
     * |        |          |1 = Edge-detect Brake interrupt for channel0/1 Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[1]     |BRKEIEN2_3|PWM Edge-detect Brake Interrupt Enable for Channel2/3 (Write Protect)
     * |        |          |0 = Edge-detect Brake interrupt for channel2/3 Disabled.
     * |        |          |1 = Edge-detect Brake interrupt for channel2/3 Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[2]     |BRKEIEN4_5|PWM Edge-detect Brake Interrupt Enable for Channel4/5 (Write Protect)
     * |        |          |0 = Edge-detect Brake interrupt for channel4/5 Disabled.
     * |        |          |1 = Edge-detect Brake interrupt for channel4/5 Enabled.
     * |        |          |Note: This bitr is write protected. Refer to SYS_REGLCTL register.
     * |[8]     |BRKLIEN0_1|PWM Level-detect Brake Interrupt Enable for Channel0/1 (Write Protect)
     * |        |          |0 = Level-detect Brake interrupt for channel0/1 Disabled.
     * |        |          |1 = Level-detect Brake interrupt for channel0/1 Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[9]     |BRKLIEN2_3|PWM Level-detect Brake Interrupt Enable for Channel2/3 (Write Protect)
     * |        |          |0 = Level-detect Brake interrupt for channel2/3 Disabled.
     * |        |          |1 = Level-detect Brake interrupt for channel2/3 Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[10]    |BRKLIEN4_5|PWM Level-detect Brake Interrupt Enable for Channel4/5 (Write Protect)
     * |        |          |0 = Level-detect Brake interrupt for channel4/5 Disabled.
     * |        |          |1 = Level-detect Brake interrupt for channel4/5 Enabled.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * @var PWM_T::INTSTS0
     * Offset: 0xE8  PWM Interrupt Flag Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ZIF0      |PWM Zero Point Interrupt Flag 0
     * |        |          |This bit is set by hardware when PWM_CH0 counter reaches 0.
     * |        |          |Note: This bit can be cleared to 0 by software writing 1.
     * |[2]     |ZIF2      |PWM Zero Point Interrupt Flag 2
     * |        |          |This bit is set by hardware when PWM_CH2 counter reaches 0.
     * |        |          |Note: This bit can be cleared to 0 by software writing 1.
     * |[4]     |ZIF4      |PWM Zero Point Interrupt Flag 4
     * |        |          |This bit is set by hardware when PWM_CH4 counter reaches 0.
     * |        |          |Note: This bit can be cleared to 0 by software writing 1.
     * |[8]     |PIF0      |PWM Period Point Interrupt Flag 0
     * |        |          |This bit is set by hardware when PWM_CH0 counter reaches PWM_PERIOD0.
     * |        |          |Note: This bit can be cleared to 0 by software writing 1.
     * |[10]    |PIF2      |PWM Period Point Interrupt Flag 2
     * |        |          |This bit is set by hardware when PWM_CH2 counter reaches PWM_PERIOD2.
     * |        |          |Note: This bit can be cleared to 0 by software writing 1.
     * |[12]    |PIF4      |PWM Period Point Interrupt Flag 4
     * |        |          |This bit is set by hardware when PWM_CH4 counter reaches PWM_PERIOD4.
     * |        |          |Note: This bit can be cleared to 0 by software writing 1.
     * |[21:16] |CMPUIFn   |PWM Compare Up Count Interrupt Flag
     * |        |          |Flag is set by hardware when PWM counter up count and reaches PWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note: In complementary mode, CMPUIF1, 3, 5 is used as another CMPUIF for channel 0, 2, 4.
     * |[24]    |CMPDIF0   |PWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when PWM counter down count and reaches PWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note: In complementary mode, CMPDIF1, 3, 5 is used as another CMPDIF for channel 0, 2, 4.
     * |[25]    |CMPDIF1   |PWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when PWM counter down count and reaches PWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note: In complementary mode, CMPDIF1, 3, 5 is used as another CMPDIF for channel 0, 2, 4.
     * |[26]    |CMPDIF2   |PWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when PWM counter down count and reaches PWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note: In complementary mode, CMPDIF1, 3, 5 is used as another CMPDIF for channel 0, 2, 4.
     * |[27]    |CMPDIF3   |PWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when PWM counter down count and reaches PWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note: In complementary mode, CMPDIF1, 3, 5 is used as another CMPDIF for channel 0, 2, 4.
     * |[28]    |CMPDIF4   |PWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when PWM counter down count and reaches PWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note: In complementary mode, CMPDIF1, 3, 5 is used as another CMPDIF for channel 0, 2, 4.
     * |[29]    |CMPDIF5   |PWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when PWM counter down count and reaches PWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note: In complementary mode, CMPDIF1, 3, 5 is used as another CMPDIF for channel 0, 2, 4.
     * @var PWM_T::INTSTS1
     * Offset: 0xEC  PWM Interrupt Flag Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKEIF0   |PWM Channel0 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel0 edge-detect brake event do not happened.
     * |        |          |1 = When PWM channel0 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[1]     |BRKEIF1   |PWM Channel1 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel1 edge-detect brake event do not happened.
     * |        |          |1 = When PWM channel1 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[2]     |BRKEIF2   |PWM Channel2 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel2 edge-detect brake event do not happened.
     * |        |          |1 = When PWM channel2 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[3]     |BRKEIF3   |PWM Channel3 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel3 edge-detect brake event do not happened.
     * |        |          |1 = When PWM channel3 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[4]     |BRKEIF4   |PWM Channel4 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel4 edge-detect brake event do not happened.
     * |        |          |1 = When PWM channel4 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[5]     |BRKEIF5   |PWM Channel5 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel5 edge-detect brake event do not happened.
     * |        |          |1 = When PWM channel5 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[8]     |BRKLIF0   |PWM Channel0 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel0 level-detect brake event do not happened.
     * |        |          |1 = When PWM channel0 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[9]     |BRKLIF1   |PWM Channel1 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel1 level-detect brake event do not happened.
     * |        |          |1 = When PWM channel1 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[10]    |BRKLIF2   |PWM Channel2 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel2 level-detect brake event do not happened.
     * |        |          |1 = When PWM channel2 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[11]    |BRKLIF3   |PWM Channel3 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel3 level-detect brake event do not happened.
     * |        |          |1 = When PWM channel3 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[12]    |BRKLIF4   |PWM Channel4 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel4 level-detect brake event do not happened.
     * |        |          |1 = When PWM channel4 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[13]    |BRKLIF5   |PWM Channel5 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = PWM channel5 level-detect brake event do not happened.
     * |        |          |1 = When PWM channel5 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This bit is write protected. Refer to SYS_REGLCTL register.
     * |[16]    |BRKESTS0  |PWM Channel0 Edge-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel0 edge-detect brake state is released.
     * |        |          |1 = When PWM channel0 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel0 at brake state.
     * |[17]    |BRKESTS1  |PWM Channel1 Edge-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel1 edge-detect brake state is released.
     * |        |          |1 = When PWM channel1 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel1 at brake state.
     * |[18]    |BRKESTS2  |PWM Channel2 Edge-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel2 edge-detect brake state is released.
     * |        |          |1 = When PWM channel2 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel2 at brake state.
     * |[19]    |BRKESTS3  |PWM Channel3 Edge-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel3 edge-detect brake state is released.
     * |        |          |1 = When PWM channel3 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel3 at brake state.
     * |[20]    |BRKESTS4  |PWM Channel4 Edge-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel4 edge-detect brake state is released.
     * |        |          |1 = When PWM channel4 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel4 at brake state.
     * |[21]    |BRKESTS5  |PWM Channel5 Edge-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel5 edge-detect brake state is released.
     * |        |          |1 = When PWM channel5 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel5 at brake state.
     * |[24]    |BRKLSTS0  |PWM Channel0 Level-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel0 level-detect brake state is released.
     * |        |          |1 = When PWM channel0 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel0 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, PWM will release brake state until current PWM period finished
     * |        |          |The PWM waveform will start output from next full PWM period.
     * |[25]    |BRKLSTS1  |PWM Channel1 Level-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel1 level-detect brake state is released.
     * |        |          |1 = When PWM channel1 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel1 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, PWM will release brake state until current PWM period finished
     * |        |          |The PWM waveform will start output from next full PWM period.
     * |[26]    |BRKLSTS2  |PWM Channel2 Level-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel2 level-detect brake state is released.
     * |        |          |1 = When PWM channel2 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel2 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, PWM will release brake state until current PWM period finished
     * |        |          |The PWM waveform will start output from next full PWM period.
     * |[27]    |BRKLSTS3  |PWM Channel3 Level-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel3 level-detect brake state is released.
     * |        |          |1 = When PWM channel3 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel3 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, PWM will release brake state until current PWM period finished
     * |        |          |The PWM waveform will start output from next full PWM period.
     * |[28]    |BRKLSTS4  |PWM Channel4 Level-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel4 level-detect brake state is released.
     * |        |          |1 = When PWM channel4 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel4 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, PWM will release brake state until current PWM period finished
     * |        |          |The PWM waveform will start output from next full PWM period.
     * |[29]    |BRKLSTS5  |PWM Channel5 Level-detect Brake Status (Read Only)
     * |        |          |0 = PWM channel5 level-detect brake state is released.
     * |        |          |1 = When PWM channel5 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the PWM channel5 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, PWM will release brake state until current PWM period finished
     * |        |          |The PWM waveform will start output from next full PWM period.
     * @var PWM_T::EADCTS0
     * Offset: 0xF8  PWM Trigger EADC Source Select Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |TRGSEL0   |PWM_CH0 Trigger EADC Source Select
     * |        |          |0000 = PWM_CH0 zero point.
     * |        |          |0001 = PWM_CH0 period point.
     * |        |          |0010 = PWM_CH0 zero or period point.
     * |        |          |0011 = PWM_CH0 up-count CMPDAT point.
     * |        |          |0100 = PWM_CH0 down-count CMPDAT point.
     * |        |          |0101 = Reserved.
     * |        |          |0110 = Reserved.
     * |        |          |0111 = Reserved.
     * |        |          |1000 = PWM_CH1 up-count CMPDAT point.
     * |        |          |1001 = PWM_CH1 down-count CMPDAT point.
     * |        |          |Others = reserved.
     * |[7]     |TRGEN0    |PWM_CH0 Trigger EADC Enable Bit
     * |        |          |0 = PWM_CH0 Trigger EADC function Disabled.
     * |        |          |1 = PWM_CH0 Trigger EADC function Enabled.
     * |[11:8]  |TRGSEL1   |PWM_CH1 Trigger EADC Source Select
     * |        |          |0000 = PWM_CH0 zero point.
     * |        |          |0001 = PWM_CH0 period point.
     * |        |          |0010 = PWM_CH0 zero or period point.
     * |        |          |0011 = PWM_CH0 up-count CMPDAT point.
     * |        |          |0100 = PWM_CH0 down-count CMPDAT point.
     * |        |          |0101 = Reserved.
     * |        |          |0110 = Reserved.
     * |        |          |0111 = Reserved.
     * |        |          |1000 = PWM_CH1 up-count CMPDAT point.
     * |        |          |1001 = PWM_CH1 down-count CMPDAT point.
     * |        |          |Others = reserved.
     * |[15]    |TRGEN1    |PWM_CH1 Trigger EADC Enable Bit
     * |        |          |0 = PWM_CH1 Trigger EADC function Disabled.
     * |        |          |1 = PWM_CH1 Trigger EADC function Enabled.
     * |[19:16] |TRGSEL2   |PWM_CH2 Trigger EADC Source Select
     * |        |          |0000 = PWM_CH2 zero point.
     * |        |          |0001 = PWM_CH2 period point.
     * |        |          |0010 = PWM_CH2 zero or period point.
     * |        |          |0011 = PWM_CH2 up-count CMPDAT point.
     * |        |          |0100 = PWM_CH2 down-count CMPDAT point.
     * |        |          |0101 = Reserved.
     * |        |          |0110 = Reserved.
     * |        |          |0111 = Reserved.
     * |        |          |1000 = PWM_CH3 up-count CMPDAT point.
     * |        |          |1001 = PWM_CH3 down-count CMPDAT point.
     * |        |          |Others = reserved.
     * |[23]    |TRGEN2    |PWM_CH2 Trigger EADC Enable Bit
     * |        |          |0 = PWM_CH2 Trigger EADC function Disabled.
     * |        |          |1 = PWM_CH2 Trigger EADC function Enabled.
     * |[27:24] |TRGSEL3   |PWM_CH3 Trigger EADC Source Select
     * |        |          |0000 = PWM_CH2 zero point.
     * |        |          |0001 = PWM_CH2 period point.
     * |        |          |0010 = PWM_CH2 zero or period point.
     * |        |          |0011 = PWM_CH2 up-count CMPDAT point.
     * |        |          |0100 = PWM_CH2 down-count CMPDAT point.
     * |        |          |0101 = Reserved.
     * |        |          |0110 = Reserved.
     * |        |          |0111 = Reserved.
     * |        |          |1000 = PWM_CH3 up-count CMPDAT point.
     * |        |          |1001 = PWM_CH3 down-count CMPDAT point.
     * |        |          |Others = reserved.
     * |[31]    |TRGEN3    |PWM_CH3 Trigger EADC Enable Bit
     * |        |          |0 = PWM_CH3 Trigger EADC function Disabled.
     * |        |          |1 = PWM_CH3 Trigger EADC function Enabled.
     * @var PWM_T::EADCTS1
     * Offset: 0xFC  PWM Trigger EADC Source Select Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |TRGSEL4   |PWM_CH4 Trigger EADC Source Select
     * |        |          |0000 = PWM_CH4 zero point.
     * |        |          |0001 = PWM_CH4 period point.
     * |        |          |0010 = PWM_CH4 zero or period point.
     * |        |          |0011 = PWM_CH4 up-count CMPDAT point.
     * |        |          |0100 = PWM_CH4 down-count CMPDAT point.
     * |        |          |0101 = Reserved.
     * |        |          |0110 = Reserved.
     * |        |          |0111 = Reserved.
     * |        |          |1000 = PWM_CH5 up-count CMPDAT point.
     * |        |          |1001 = PWM_CH5 down-count CMPDAT point.
     * |        |          |Others = reserved.
     * |[7]     |TRGEN4    |PWM_CH4 Trigger EADC Enable Bit
     * |        |          |0 = PWM_CH4 Trigger EADC function Disabled.
     * |        |          |1 = PWM_CH4 Trigger EADC function Enabled.
     * |[11:8]  |TRGSEL5   |PWM_CH5 Trigger EADC Source Select
     * |        |          |0000 = PWM_CH4 zero point.
     * |        |          |0001 = PWM_CH4 period point.
     * |        |          |0010 = PWM_CH4 zero or period point.
     * |        |          |0011 = PWM_CH4 up-count CMPDAT point.
     * |        |          |0100 = PWM_CH4 down-count CMPDAT point.
     * |        |          |0101 = Reserved.
     * |        |          |0110 = Reserved.
     * |        |          |0111 = Reserved.
     * |        |          |1000 = PWM_CH5 up-count CMPDAT point.
     * |        |          |1001 = PWM_CH5 down-count CMPDAT point.
     * |        |          |Others = reserved.
     * |[15]    |TRGEN5    |PWM_CH5 Trigger EADC Enable Bit
     * |        |          |0 = PWM_CH5 Trigger EADC function Disabled.
     * |        |          |1 = PWM_CH5 Trigger EADC function Enabled.
     * @var PWM_T::SSCTL
     * Offset: 0x110  PWM Synchronous Start Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SSEN0     |PWM Synchronous Start Function Enable Bit 0
     * |        |          |When synchronous start function is enabled, the PWM_CH0 counter enable bit (CNTEN0) can be enabled by writing PWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = PWM synchronous start function Disabled.
     * |        |          |1 = PWM synchronous start function Enabled.
     * |[2]     |SSEN2     |PWM Synchronous Start Function Enable Bit 2
     * |        |          |When synchronous start function is enabled, the PWM_CH2 counter enable bit (CNTEN2) can be enabled by writing PWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = PWM synchronous start function Disabled.
     * |        |          |1 = PWM synchronous start function Enabled.
     * |[4]     |SSEN4     |PWM Synchronous Start Function Enable Bit 4
     * |        |          |When synchronous start function is enabled, the PWM_CH4 counter enable bit (CNTEN4) can be enabled by writing PWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = PWM synchronous start function Disabled.
     * |        |          |1 = PWM synchronous start function Enabled.
     * |[9:8]   |SSRC      |PWM Synchronous Start Source Select Bits
     * |        |          |00 = Synchronous start source come from PWM0.
     * |        |          |01 = Synchronous start source come from PWM1.
     * |        |          |10 = Synchronous start source come from BPWM0.
     * |        |          |11 = Synchronous start source come from BPWM1.
     * @var PWM_T::SSTRG
     * Offset: 0x114  PWM Synchronous Start Trigger Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTSEN    |PWM Counter Synchronous Start Enable (Write Only)
     * |        |          |PMW counter synchronous enable function is used to make selected PWM channels (include PWM0_CHx and PWM1_CHx) start counting at the same time.
     * |        |          |Writing this bit to 1 will also set the counter enable bit (CNTENn, n denotes channel 0 to 5) if correlated PWM channel counter synchronous start function is enabled.
     * @var PWM_T::STATUS
     * Offset: 0x120  PWM Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTMAX0   |Time-base Counter 0 Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[2]     |CNTMAX2   |Time-base Counter 2 Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[4]     |CNTMAX4   |Time-base Counter 4 Equal to 0xFFFF Latched Flag
     * |        |          |0 = The time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = The time-base counter reached its maximum value.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[16]    |EADCTRG0  |EADC Start of Conversion Status
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = An EADC start of conversion trigger event has occurred.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[17]    |EADCTRG1  |EADC Start of Conversion Status
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = An EADC start of conversion trigger event has occurred.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[18]    |EADCTRG2  |EADC Start of Conversion Status
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = An EADC start of conversion trigger event has occurred.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[19]    |EADCTRG3  |EADC Start of Conversion Status
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = An EADC start of conversion trigger event has occurred.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[20]    |EADCTRG4  |EADC Start of Conversion Status
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = An EADC start of conversion trigger event has occurred.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * |[21]    |EADCTRG5  |EADC Start of Conversion Status
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = An EADC start of conversion trigger event has occurred.
     * |        |          |Note: This bit can be cleared by software writing 1.
     * @var PWM_T::CAPINEN
     * Offset: 0x200  PWM Capture Input Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPINEN0  |Capture Input Enable Bits
     * |        |          |0 = PWM Channel capture input path Disabled
     * |        |          |The input of PWM channel capture function is always regarded as 0.
     * |        |          |1 = PWM Channel capture input path Enabled
     * |        |          |The input of PWM channel capture function comes from correlative multifunction pin.
     * |[1]     |CAPINEN1  |Capture Input Enable Bits
     * |        |          |0 = PWM Channel capture input path Disabled
     * |        |          |The input of PWM channel capture function is always regarded as 0.
     * |        |          |1 = PWM Channel capture input path Enabled
     * |        |          |The input of PWM channel capture function comes from correlative multifunction pin.
     * |[2]     |CAPINEN2  |Capture Input Enable Bits
     * |        |          |0 = PWM Channel capture input path Disabled
     * |        |          |The input of PWM channel capture function is always regarded as 0.
     * |        |          |1 = PWM Channel capture input path Enabled
     * |        |          |The input of PWM channel capture function comes from correlative multifunction pin.
     * |[3]     |CAPINEN3  |Capture Input Enable Bits
     * |        |          |0 = PWM Channel capture input path Disabled
     * |        |          |The input of PWM channel capture function is always regarded as 0.
     * |        |          |1 = PWM Channel capture input path Enabled
     * |        |          |The input of PWM channel capture function comes from correlative multifunction pin.
     * |[4]     |CAPINEN4  |Capture Input Enable Bits
     * |        |          |0 = PWM Channel capture input path Disabled
     * |        |          |The input of PWM channel capture function is always regarded as 0.
     * |        |          |1 = PWM Channel capture input path Enabled
     * |        |          |The input of PWM channel capture function comes from correlative multifunction pin.
     * |[5]     |CAPINEN5  |Capture Input Enable Bits
     * |        |          |0 = PWM Channel capture input path Disabled
     * |        |          |The input of PWM channel capture function is always regarded as 0.
     * |        |          |1 = PWM Channel capture input path Enabled
     * |        |          |The input of PWM channel capture function comes from correlative multifunction pin.
     * @var PWM_T::CAPCTL
     * Offset: 0x204  PWM Capture Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPEN0    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the PWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[1]     |CAPEN1    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the PWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[2]     |CAPEN2    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the PWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[3]     |CAPEN3    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the PWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[4]     |CAPEN4    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the PWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[5]     |CAPEN5    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the PWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[8]     |CAPINV0   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[9]     |CAPINV1   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[10]    |CAPINV2   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[11]    |CAPINV3   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[12]    |CAPINV4   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[13]    |CAPINV5   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[16]    |RCRLDEN0  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[17]    |RCRLDEN1  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[18]    |RCRLDEN2  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[19]    |RCRLDEN3  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[20]    |RCRLDEN4  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[21]    |RCRLDEN5  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[24]    |FCRLDEN0  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[25]    |FCRLDEN1  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[26]    |FCRLDEN2  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[27]    |FCRLDEN3  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[28]    |FCRLDEN4  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[29]    |FCRLDEN5  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * @var PWM_T::CAPSTS
     * Offset: 0x208  PWM Capture Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CRLIFOV0  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[1]     |CRLIFOV1  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[2]     |CRLIFOV2  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[3]     |CRLIFOV3  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[4]     |CRLIFOV4  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[5]     |CRLIFOV5  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[8]     |CFLIFOV0  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[9]     |CFLIFOV1  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[10]    |CFLIFOV2  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[11]    |CFLIFOV3  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[12]    |CFLIFOV4  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[13]    |CFLIFOV5  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * @var PWM_T::PDMACTL
     * Offset: 0x23C  PWM PDMA Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CHEN0_1   |Channel 0/1 PDMA Enable Bit
     * |        |          |0 = Channel 0/1 PDMA function Disabled.
     * |        |          |1 = Channel 0/1 PDMA function Enabled for the channel 0/1 captured data and transfer to memory.
     * |[2:1]   |CAPMOD0_1 |Select PWM_RCAPDAT0/1 or PWM_FCAPDAT0/1 to Do PDMA Transfer
     * |        |          |00 = Reserved.
     * |        |          |01 = PWM_RCAPDAT0/1.
     * |        |          |10 = PWM_FCAPDAT0/1.
     * |        |          |11 = Both PWM_RCAPDAT0/1 and PWM_FCAPDAT0/1.
     * |[3]     |CAPORD0_1 |Capture Channel 0/1 Rising/Falling Order
     * |        |          |Set this bit to determine whether the PWM_RCAPDAT0/1 or PWM_FCAPDAT0/1 is the first captured data transferred to memory through PDMA when CAPMOD0_1 =11.
     * |        |          |0 = PWM_FCAPDAT0/1 is the first captured data to memory.
     * |        |          |1 = PWM_RCAPDAT0/1 is the first captured data to memory.
     * |[4]     |CHSEL0_1  |Select Channel 0/1 to Do PDMA Transfer
     * |        |          |0 = Channel0.
     * |        |          |1 = Channel1.
     * |[8]     |CHEN2_3   |Channel 2/3 PDMA Enable Bit
     * |        |          |0 = Channel 2/3 PDMA function Disabled.
     * |        |          |1 = Channel 2/3 PDMA function Enabled for the channel 2/3 captured data and transfer to memory.
     * |[10:9]  |CAPMOD2_3 |Select PWM_RCAPDAT2/3 or PWM_FCAODAT2/3 to Do PDMA Transfer
     * |        |          |00 = Reserved.
     * |        |          |01 = PWM_RCAPDAT2/3.
     * |        |          |10 = PWM_FCAPDAT2/3.
     * |        |          |11 = Both PWM_RCAPDAT2/3 and PWM_FCAPDAT2/3.
     * |[11]    |CAPORD2_3 |Capture Channel 2/3 Rising/Falling Order
     * |        |          |Set this bit to determine whether the PWM_RCAPDAT2/3 or PWM_FCAPDAT2/3 is the first captured data transferred to memory through PDMA when CAPMOD2_3 =11.
     * |        |          |0 = PWM_FCAPDAT2/3 is the first captured data to memory.
     * |        |          |1 = PWM_RCAPDAT2/3 is the first captured data to memory.
     * |[12]    |CHSEL2_3  |Select Channel 2/3 to Do PDMA Transfer
     * |        |          |0 = Channel2.
     * |        |          |1 = Channel3.
     * |[16]    |CHEN4_5   |Channel 4/5 PDMA Enable Bit
     * |        |          |0 = Channel 4/5 PDMA function Disabled.
     * |        |          |1 = Channel 4/5 PDMA function Enabled for the channel 4/5 captured data and transfer to memory.
     * |[18:17] |CAPMOD4_5 |Select PWM_RCAPDAT4/5 or PWM_FCAPDAT4/5 to Do PDMA Transfer
     * |        |          |00 = Reserved.
     * |        |          |01 = PWM_RCAPDAT4/5.
     * |        |          |10 = PWM_FCAPDAT4/5.
     * |        |          |11 = Both PWM_RCAPDAT4/5 and PWM_FCAPDAT4/5.
     * |[19]    |CAPORD4_5 |Capture Channel 4/5 Rising/Falling Order
     * |        |          |Set this bit to determine whether the PWM_RCAPDAT4/5 or PWM_FCAPDAT4/5 is the first captured data transferred to memory through PDMA when CAPMOD4_5 =11.
     * |        |          |0 = PWM_FCAPDAT4/5 is the first captured data to memory.
     * |        |          |1 = PWM_RCAPDAT4/5 is the first captured data to memory.
     * |[20]    |CHSEL4_5  |Select Channel 4/5 to Do PDMA Transfer
     * |        |          |0 = Channel4.
     * |        |          |1 = Channel5.
     * @var PWM_T::PDMACAP[3]
     * Offset: 0x240~0x248  PWM Capture Channel 01 23 45 PDMA Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CAPBUF    |PWM Capture PDMA Register (Read Only)
     * |        |          |This register is used as a buffer to transfer PWM capture rising or falling data to memory by PDMA.
     * @var PWM_T::CAPIEN
     * Offset: 0x250  PWM Capture Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPRIEN0  |PWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[1]     |CAPRIEN1  |PWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[2]     |CAPRIEN2  |PWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[3]     |CAPRIEN3  |PWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[4]     |CAPRIEN4  |PWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[5]     |CAPRIEN5  |PWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[8]     |CAPFIEN0  |PWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[9]     |CAPFIEN1  |PWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[10]    |CAPFIEN2  |PWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[11]    |CAPFIEN3  |PWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[12]    |CAPFIEN4  |PWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[13]    |CAPFIEN5  |PWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * @var PWM_T::CAPIF
     * Offset: 0x254  PWM Capture Interrupt Flag Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CRLIF0    |PWM Capture Rising Latch Interrupt Flag
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[1]     |CRLIF1    |PWM Capture Rising Latch Interrupt Flag
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[2]     |CRLIF2    |PWM Capture Rising Latch Interrupt Flag
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[3]     |CRLIF3    |PWM Capture Rising Latch Interrupt Flag
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[4]     |CRLIF4    |PWM Capture Rising Latch Interrupt Flag
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[5]     |CRLIF5    |PWM Capture Rising Latch Interrupt Flag
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[8]     |CFLIF0    |PWM Capture Falling Latch Interrupt Flag
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[9]     |CFLIF1    |PWM Capture Falling Latch Interrupt Flag
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[10]    |CFLIF2    |PWM Capture Falling Latch Interrupt Flag
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[11]    |CFLIF3    |PWM Capture Falling Latch Interrupt Flag
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[12]    |CFLIF4    |PWM Capture Falling Latch Interrupt Flag
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[13]    |CFLIF5    |PWM Capture Falling Latch Interrupt Flag
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note1: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will be cleared by hardware after PDMA transfer data.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * @var PWM_T::PBUF[6]
     * Offset: 0x304~0x318  PWM PERIOD0, 2, 4 Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PBUF      |PWM Period Register Buffer (Read Only)
     * |        |          |Used as PERIOD active register.
     * @var PWM_T::CMPBUF[6]
     * Offset: 0x31C~0x330  PWM CMPDAT0~5 Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CMPBUF    |PWM Comparator Register Buffer (Read Only)
     * |        |          |Used as CMP active register.
     */
    __IO uint32_t CTL0;                  /*!< [0x0000] PWM Control Register 0                                           */
    __IO uint32_t CTL1;                  /*!< [0x0004] PWM Control Register 1                                           */
    __I  uint32_t RESERVE0[2];
    __IO uint32_t CLKSRC;                /*!< [0x0010] PWM Clock Source Register                                        */
    __IO uint32_t CLKPSC[3];             /*!< [0x0014 ~ 0x001c] PWM Clock Prescale Register                             */
    __IO uint32_t CNTEN;                 /*!< [0x0020] PWM Counter Enable Register                                      */
    __IO uint32_t CNTCLR;                /*!< [0x0024] PWM Clear Counter Register                                       */
    __I  uint32_t RESERVE1[2];
    __IO uint32_t PERIOD[6];            /*!< [0x0030 ~ 0x0044] PWM Period Register 0                                    */
    __I  uint32_t RESERVE4[2];
    __IO uint32_t CMPDAT[6];             /*!< [0x0050 ~ 0x0064] PWM Comparator Register 0 ~ 5                           */
    __I  uint32_t RESERVE5[2];
    __IO uint32_t DTCTL[3];              /*!< [0x0070 ~ 0x0078] PWM Dead-time Control Register                          */
    __I  uint32_t RESERVE6[5];
    __I  uint32_t CNT[6];                /*!< [0x0090] PWM Counter Register 0,2,4                                       */
    __I  uint32_t RESERVE9[2];
    __IO uint32_t WGCTL0;                /*!< [0x00b0] PWM Generation Register 0                                        */
    __IO uint32_t WGCTL1;                /*!< [0x00b4] PWM Generation Register 1                                        */
    __IO uint32_t MSKEN;                 /*!< [0x00b8] PWM Mask Enable Register                                         */
    __IO uint32_t MSK;                   /*!< [0x00bc] PWM Mask Data Register                                           */
    __IO uint32_t BNF;                   /*!< [0x00c0] PWM Brake Noise Filter Register                                  */
    __IO uint32_t FAILBRK;               /*!< [0x00c4] PWM System Fail Brake Control Register                           */
    __IO uint32_t BRKCTL[3];             /*!< [0x00c8 ~ 0x00d0] PWM Brake Edge Detect Control Register                  */
    __IO uint32_t POLCTL;                /*!< [0x00d4] PWM Pin Polar Inverse Register                                   */
    __IO uint32_t POEN;                  /*!< [0x00d8] PWM Output Enable Register                                       */
    __O  uint32_t SWBRK;                 /*!< [0x00dc] PWM Software Brake Control Register                              */
    __IO uint32_t INTEN0;                /*!< [0x00e0] PWM Interrupt Enable Register 0                                  */
    __IO uint32_t INTEN1;                /*!< [0x00e4] PWM Interrupt Enable Register 1                                  */
    __IO uint32_t INTSTS0;               /*!< [0x00e8] PWM Interrupt Flag Register 0                                    */
    __IO uint32_t INTSTS1;               /*!< [0x00ec] PWM Interrupt Flag Register 1                                    */
    __I  uint32_t RESERVE10[2];
    __IO uint32_t EADCTS0;               /*!< [0x00f8] PWM Trigger EADC Source Select Register 0                        */
    __IO uint32_t EADCTS1;               /*!< [0x00fc] PWM Trigger EADC Source Select Register 1                        */
    __I  uint32_t RESERVE11[4];
    __IO uint32_t SSCTL;                 /*!< [0x0110] PWM Synchronous Start Control Register                           */
    __O  uint32_t SSTRG;                 /*!< [0x0114] PWM Synchronous Start Trigger Register                           */
    __I  uint32_t RESERVE12[2];
    __IO uint32_t STATUS;                /*!< [0x0120] PWM Status Register                                              */
    __I  uint32_t RESERVE13[55];
    __IO uint32_t CAPINEN;               /*!< [0x0200] PWM Capture Input Enable Register                                */
    __IO uint32_t CAPCTL;                /*!< [0x0204] PWM Capture Control Register                                     */
    __I  uint32_t CAPSTS;                /*!< [0x0208] PWM Capture Status Register                                      */
    CAPDAT_T CAPDAT[6];                 /*!< [0x020C] PWM Rising and Falling Capture Data Register 0~5                  */
    __IO uint32_t PDMACTL;               /*!< [0x023c] PWM PDMA Control Register                                        */
    __I  uint32_t PDMACAP[3];            /*!< [0x0240 ~ 0x0248] PWM Capture Channel 01, 23, 45 PDMA Register            */
    __I  uint32_t RESERVE14[1];
    __IO uint32_t CAPIEN;                /*!< [0x0250] PWM Capture Interrupt Enable Register                            */
    __IO uint32_t CAPIF;                 /*!< [0x0254] PWM Capture Interrupt Flag Register                              */
    __I  uint32_t RESERVE15[43];
    __I  uint32_t PBUF[6];               /*!< [0x0304 ~ 0x0318] PWM PERIOD0 ~ 6 Buffer                                  */
    __I  uint32_t CMPBUF[6];             /*!< [0x031c ~ 0x0330] PWM CMPDAT0 ~ 5 Buffer                                  */

} PWM_T;

/**
    @addtogroup PWM_CONST PWM Bit Field Definition
    Constant Definitions for PWM Controller
@{ */

#define PWM_CTL0_CTRLDn_Pos              (0)                                               /*!< PWM_T::CTL0: CTRLDn Position           */
#define PWM_CTL0_CTRLDn_Msk              (0x3ful << PWM_CTL0_CTRLDn_Pos)                   /*!< PWM_T::CTL0: CTRLDn Mask               */

#define PWM_CTL0_CTRLD0_Pos              (0)                                               /*!< PWM_T::CTL0: CTRLD0 Position           */
#define PWM_CTL0_CTRLD0_Msk              (0x1ul << PWM_CTL0_CTRLD0_Pos)                    /*!< PWM_T::CTL0: CTRLD0 Mask               */

#define PWM_CTL0_CTRLD1_Pos              (1)                                               /*!< PWM_T::CTL0: CTRLD1 Position           */
#define PWM_CTL0_CTRLD1_Msk              (0x1ul << PWM_CTL0_CTRLD1_Pos)                    /*!< PWM_T::CTL0: CTRLD1 Mask               */

#define PWM_CTL0_CTRLD2_Pos              (2)                                               /*!< PWM_T::CTL0: CTRLD2 Position           */
#define PWM_CTL0_CTRLD2_Msk              (0x1ul << PWM_CTL0_CTRLD2_Pos)                    /*!< PWM_T::CTL0: CTRLD2 Mask               */

#define PWM_CTL0_CTRLD3_Pos              (3)                                               /*!< PWM_T::CTL0: CTRLD3 Position           */
#define PWM_CTL0_CTRLD3_Msk              (0x1ul << PWM_CTL0_CTRLD3_Pos)                    /*!< PWM_T::CTL0: CTRLD3 Mask               */

#define PWM_CTL0_CTRLD4_Pos              (4)                                               /*!< PWM_T::CTL0: CTRLD4 Position           */
#define PWM_CTL0_CTRLD4_Msk              (0x1ul << PWM_CTL0_CTRLD4_Pos)                    /*!< PWM_T::CTL0: CTRLD4 Mask               */

#define PWM_CTL0_CTRLD5_Pos              (5)                                               /*!< PWM_T::CTL0: CTRLD5 Position           */
#define PWM_CTL0_CTRLD5_Msk              (0x1ul << PWM_CTL0_CTRLD5_Pos)                    /*!< PWM_T::CTL0: CTRLD5 Mask               */

#define PWM_CTL0_IMMLDENn_Pos            (16)                                              /*!< PWM_T::CTL0: IMMLDENn Position         */
#define PWM_CTL0_IMMLDENn_Msk            (0x3ful << PWM_CTL0_IMMLDENn_Pos)                 /*!< PWM_T::CTL0: IMMLDENn Mask             */

#define PWM_CTL0_IMMLDEN0_Pos            (16)                                              /*!< PWM_T::CTL0: IMMLDEN0 Position         */
#define PWM_CTL0_IMMLDEN0_Msk            (0x1ul << PWM_CTL0_IMMLDEN0_Pos)                  /*!< PWM_T::CTL0: IMMLDEN0 Mask             */

#define PWM_CTL0_IMMLDEN1_Pos            (17)                                              /*!< PWM_T::CTL0: IMMLDEN1 Position         */
#define PWM_CTL0_IMMLDEN1_Msk            (0x1ul << PWM_CTL0_IMMLDEN1_Pos)                  /*!< PWM_T::CTL0: IMMLDEN1 Mask             */

#define PWM_CTL0_IMMLDEN2_Pos            (18)                                              /*!< PWM_T::CTL0: IMMLDEN2 Position         */
#define PWM_CTL0_IMMLDEN2_Msk            (0x1ul << PWM_CTL0_IMMLDEN2_Pos)                  /*!< PWM_T::CTL0: IMMLDEN2 Mask             */

#define PWM_CTL0_IMMLDEN3_Pos            (19)                                              /*!< PWM_T::CTL0: IMMLDEN3 Position         */
#define PWM_CTL0_IMMLDEN3_Msk            (0x1ul << PWM_CTL0_IMMLDEN3_Pos)                  /*!< PWM_T::CTL0: IMMLDEN3 Mask             */

#define PWM_CTL0_IMMLDEN4_Pos            (20)                                              /*!< PWM_T::CTL0: IMMLDEN4 Position         */
#define PWM_CTL0_IMMLDEN4_Msk            (0x1ul << PWM_CTL0_IMMLDEN4_Pos)                  /*!< PWM_T::CTL0: IMMLDEN4 Mask             */

#define PWM_CTL0_IMMLDEN5_Pos            (21)                                              /*!< PWM_T::CTL0: IMMLDEN5 Position         */
#define PWM_CTL0_IMMLDEN5_Msk            (0x1ul << PWM_CTL0_IMMLDEN5_Pos)                  /*!< PWM_T::CTL0: IMMLDEN5 Mask             */

#define PWM_CTL0_DBGHALT_Pos             (30)                                              /*!< PWM_T::CTL0: DBGHALT Position          */
#define PWM_CTL0_DBGHALT_Msk             (0x1ul << PWM_CTL0_DBGHALT_Pos)                   /*!< PWM_T::CTL0: DBGHALT Mask              */

#define PWM_CTL0_DBGTRIOFF_Pos           (31)                                              /*!< PWM_T::CTL0: DBGTRIOFF Position        */
#define PWM_CTL0_DBGTRIOFF_Msk           (0x1ul << PWM_CTL0_DBGTRIOFF_Pos)                 /*!< PWM_T::CTL0: DBGTRIOFF Mask            */

#define PWM_CTL1_CNTTYPE0_Pos            (0)                                               /*!< PWM_T::CTL1: CNTTYPE0 Position         */
#define PWM_CTL1_CNTTYPE0_Msk            (0x3ul << PWM_CTL1_CNTTYPE0_Pos)                  /*!< PWM_T::CTL1: CNTTYPE0 Mask             */

#define PWM_CTL1_CNTTYPE2_Pos            (4)                                               /*!< PWM_T::CTL1: CNTTYPE2 Position         */
#define PWM_CTL1_CNTTYPE2_Msk            (0x3ul << PWM_CTL1_CNTTYPE2_Pos)                  /*!< PWM_T::CTL1: CNTTYPE2 Mask             */

#define PWM_CTL1_CNTTYPE4_Pos            (8)                                               /*!< PWM_T::CTL1: CNTTYPE4 Position         */
#define PWM_CTL1_CNTTYPE4_Msk            (0x3ul << PWM_CTL1_CNTTYPE4_Pos)                  /*!< PWM_T::CTL1: CNTTYPE4 Mask             */

#define PWM_CTL1_OUTMODEn_Pos            (24)                                              /*!< PWM_T::CTL1: OUTMODEn Position         */
#define PWM_CTL1_OUTMODEn_Msk            (0x7ul << PWM_CTL1_OUTMODEn_Pos)                  /*!< PWM_T::CTL1: OUTMODEn Mask             */

#define PWM_CTL1_OUTMODE0_Pos            (24)                                              /*!< PWM_T::CTL1: OUTMODE0 Position         */
#define PWM_CTL1_OUTMODE0_Msk            (0x1ul << PWM_CTL1_OUTMODE0_Pos)                  /*!< PWM_T::CTL1: OUTMODE0 Mask             */

#define PWM_CTL1_OUTMODE2_Pos            (25)                                              /*!< PWM_T::CTL1: OUTMODE2 Position         */
#define PWM_CTL1_OUTMODE2_Msk            (0x1ul << PWM_CTL1_OUTMODE2_Pos)                  /*!< PWM_T::CTL1: OUTMODE2 Mask             */

#define PWM_CTL1_OUTMODE4_Pos            (26)                                              /*!< PWM_T::CTL1: OUTMODE4 Position         */
#define PWM_CTL1_OUTMODE4_Msk            (0x1ul << PWM_CTL1_OUTMODE4_Pos)                  /*!< PWM_T::CTL1: OUTMODE4 Mask             */

#define PWM_CLKSRC_ECLKSRC0_Pos          (0)                                               /*!< PWM_T::CLKSRC: ECLKSRC0 Position       */
#define PWM_CLKSRC_ECLKSRC0_Msk          (0x7ul << PWM_CLKSRC_ECLKSRC0_Pos)                /*!< PWM_T::CLKSRC: ECLKSRC0 Mask           */

#define PWM_CLKSRC_ECLKSRC2_Pos          (8)                                               /*!< PWM_T::CLKSRC: ECLKSRC2 Position       */
#define PWM_CLKSRC_ECLKSRC2_Msk          (0x7ul << PWM_CLKSRC_ECLKSRC2_Pos)                /*!< PWM_T::CLKSRC: ECLKSRC2 Mask           */

#define PWM_CLKSRC_ECLKSRC4_Pos          (16)                                              /*!< PWM_T::CLKSRC: ECLKSRC4 Position       */
#define PWM_CLKSRC_ECLKSRC4_Msk          (0x7ul << PWM_CLKSRC_ECLKSRC4_Pos)                /*!< PWM_T::CLKSRC: ECLKSRC4 Mask           */

#define PWM_CLKPSC_CLKPSC_Pos            (0)                                               /*!< PWM_T::CLKPSC: CLKPSC Position         */
#define PWM_CLKPSC_CLKPSC_Msk            (0xffful << PWM_CLKPSC_CLKPSC_Pos)                /*!< PWM_T::CLKPSC: CLKPSC Mask             */

#define PWM_CLKPSC0_1_CLKPSC_Pos         (0)                                               /*!< PWM_T::CLKPSC0_1: CLKPSC Position      */
#define PWM_CLKPSC0_1_CLKPSC_Msk         (0xffful << PWM_CLKPSC0_1_CLKPSC_Pos)             /*!< PWM_T::CLKPSC0_1: CLKPSC Mask          */

#define PWM_CLKPSC2_3_CLKPSC_Pos         (0)                                               /*!< PWM_T::CLKPSC2_3: CLKPSC Position      */
#define PWM_CLKPSC2_3_CLKPSC_Msk         (0xffful << PWM_CLKPSC2_3_CLKPSC_Pos)             /*!< PWM_T::CLKPSC2_3: CLKPSC Mask          */

#define PWM_CLKPSC4_5_CLKPSC_Pos         (0)                                               /*!< PWM_T::CLKPSC4_5: CLKPSC Position      */
#define PWM_CLKPSC4_5_CLKPSC_Msk         (0xffful << PWM_CLKPSC4_5_CLKPSC_Pos)             /*!< PWM_T::CLKPSC4_5: CLKPSC Mask          */

#define PWM_CNTEN_CNTEN0_Pos             (0)                                               /*!< PWM_T::CNTEN: CNTEN0 Position          */
#define PWM_CNTEN_CNTEN0_Msk             (0x1ul << PWM_CNTEN_CNTEN0_Pos)                   /*!< PWM_T::CNTEN: CNTEN0 Mask              */

#define PWM_CNTEN_CNTEN2_Pos             (2)                                               /*!< PWM_T::CNTEN: CNTEN2 Position          */
#define PWM_CNTEN_CNTEN2_Msk             (0x1ul << PWM_CNTEN_CNTEN2_Pos)                   /*!< PWM_T::CNTEN: CNTEN2 Mask              */

#define PWM_CNTEN_CNTEN4_Pos             (4)                                               /*!< PWM_T::CNTEN: CNTEN4 Position          */
#define PWM_CNTEN_CNTEN4_Msk             (0x1ul << PWM_CNTEN_CNTEN4_Pos)                   /*!< PWM_T::CNTEN: CNTEN4 Mask              */

#define PWM_CNTCLR_CNTCLR0_Pos           (0)                                               /*!< PWM_T::CNTCLR: CNTCLR0 Position        */
#define PWM_CNTCLR_CNTCLR0_Msk           (0x1ul << PWM_CNTCLR_CNTCLR0_Pos)                 /*!< PWM_T::CNTCLR: CNTCLR0 Mask            */

#define PWM_CNTCLR_CNTCLR2_Pos           (2)                                               /*!< PWM_T::CNTCLR: CNTCLR2 Position        */
#define PWM_CNTCLR_CNTCLR2_Msk           (0x1ul << PWM_CNTCLR_CNTCLR2_Pos)                 /*!< PWM_T::CNTCLR: CNTCLR2 Mask            */

#define PWM_CNTCLR_CNTCLR4_Pos           (4)                                               /*!< PWM_T::CNTCLR: CNTCLR4 Position        */
#define PWM_CNTCLR_CNTCLR4_Msk           (0x1ul << PWM_CNTCLR_CNTCLR4_Pos)                 /*!< PWM_T::CNTCLR: CNTCLR4 Mask            */

#define PWM_PERIOD_PERIOD_Pos            (0)                                               /*!< PWM_T::PERIOD: PERIOD Position         */
#define PWM_PERIOD_PERIOD_Msk            (0xfffful << PWM_PERIOD_PERIOD_Pos)               /*!< PWM_T::PERIOD: PERIOD Mask             */

#define PWM_CMPDAT_CMP_Pos               (0)                                               /*!< PWM_T::CMPDAT: CMP Position            */
#define PWM_CMPDAT_CMP_Msk               (0xfffful << PWM_CMPDAT_CMP_Pos)                  /*!< PWM_T::CMPDAT: CMP Mask                */

#define PWM_DTCTL_DTCNT_Pos              (0)                                               /*!< PWM_T::DTCTL: DTCNT Position           */
#define PWM_DTCTL_DTCNT_Msk              (0xffful << PWM_DTCTL_DTCNT_Pos)                  /*!< PWM_T::DTCTL: DTCNT Mask               */

#define PWM_DTCTL_DTEN_Pos               (16)                                              /*!< PWM_T::DTCTL: DTEN Position            */
#define PWM_DTCTL_DTEN_Msk               (0x1ul << PWM_DTCTL_DTEN_Pos)                     /*!< PWM_T::DTCTL: DTEN Mask                */

#define PWM_DTCTL_DTCKSEL_Pos            (24)                                              /*!< PWM_T::DTCTL: DTCKSEL Position         */
#define PWM_DTCTL_DTCKSEL_Msk            (0x1ul << PWM_DTCTL_DTCKSEL_Pos)                  /*!< PWM_T::DTCTL: DTCKSEL Mask             */

#define PWM_DTCTL0_1_DTCNT_Pos           (0)                                               /*!< PWM_T::DTCTL0_1: DTCNT Position        */
#define PWM_DTCTL0_1_DTCNT_Msk           (0xffful << PWM_DTCTL0_1_DTCNT_Pos)               /*!< PWM_T::DTCTL0_1: DTCNT Mask            */

#define PWM_DTCTL0_1_DTEN_Pos            (16)                                              /*!< PWM_T::DTCTL0_1: DTEN Position         */
#define PWM_DTCTL0_1_DTEN_Msk            (0x1ul << PWM_DTCTL0_1_DTEN_Pos)                  /*!< PWM_T::DTCTL0_1: DTEN Mask             */

#define PWM_DTCTL0_1_DTCKSEL_Pos         (24)                                              /*!< PWM_T::DTCTL0_1: DTCKSEL Position      */
#define PWM_DTCTL0_1_DTCKSEL_Msk         (0x1ul << PWM_DTCTL0_1_DTCKSEL_Pos)               /*!< PWM_T::DTCTL0_1: DTCKSEL Mask          */

#define PWM_DTCTL2_3_DTCNT_Pos           (0)                                               /*!< PWM_T::DTCTL2_3: DTCNT Position        */
#define PWM_DTCTL2_3_DTCNT_Msk           (0xffful << PWM_DTCTL2_3_DTCNT_Pos)               /*!< PWM_T::DTCTL2_3: DTCNT Mask            */

#define PWM_DTCTL2_3_DTEN_Pos            (16)                                              /*!< PWM_T::DTCTL2_3: DTEN Position         */
#define PWM_DTCTL2_3_DTEN_Msk            (0x1ul << PWM_DTCTL2_3_DTEN_Pos)                  /*!< PWM_T::DTCTL2_3: DTEN Mask             */

#define PWM_DTCTL2_3_DTCKSEL_Pos         (24)                                              /*!< PWM_T::DTCTL2_3: DTCKSEL Position      */
#define PWM_DTCTL2_3_DTCKSEL_Msk         (0x1ul << PWM_DTCTL2_3_DTCKSEL_Pos)               /*!< PWM_T::DTCTL2_3: DTCKSEL Mask          */

#define PWM_DTCTL4_5_DTCNT_Pos           (0)                                               /*!< PWM_T::DTCTL4_5: DTCNT Position        */
#define PWM_DTCTL4_5_DTCNT_Msk           (0xffful << PWM_DTCTL4_5_DTCNT_Pos)               /*!< PWM_T::DTCTL4_5: DTCNT Mask            */

#define PWM_DTCTL4_5_DTEN_Pos            (16)                                              /*!< PWM_T::DTCTL4_5: DTEN Position         */
#define PWM_DTCTL4_5_DTEN_Msk            (0x1ul << PWM_DTCTL4_5_DTEN_Pos)                  /*!< PWM_T::DTCTL4_5: DTEN Mask             */

#define PWM_DTCTL4_5_DTCKSEL_Pos         (24)                                              /*!< PWM_T::DTCTL4_5: DTCKSEL Position      */
#define PWM_DTCTL4_5_DTCKSEL_Msk         (0x1ul << PWM_DTCTL4_5_DTCKSEL_Pos)               /*!< PWM_T::DTCTL4_5: DTCKSEL Mask          */

#define PWM_CNT_CNT_Pos                  (0)                                               /*!< PWM_T::CNT: CNT Position               */
#define PWM_CNT_CNT_Msk                  (0xfffful << PWM_CNT_CNT_Pos)                     /*!< PWM_T::CNT: CNT Mask                   */

#define PWM_CNT_DIRF_Pos                 (16)                                              /*!< PWM_T::CNT: DIRF Position              */
#define PWM_CNT_DIRF_Msk                 (0x1ul << PWM_CNT_DIRF_Pos)                       /*!< PWM_T::CNT: DIRF Mask                  */

#define PWM_WGCTL0_ZPCTL0_Pos            (0)                                               /*!< PWM_T::WGCTL0: ZPCTL0 Position         */
#define PWM_WGCTL0_ZPCTL0_Msk            (0x3ul << PWM_WGCTL0_ZPCTL0_Pos)                  /*!< PWM_T::WGCTL0: ZPCTL0 Mask             */

#define PWM_WGCTL0_ZPCTL1_Pos            (2)                                               /*!< PWM_T::WGCTL0: ZPCTL1 Position         */
#define PWM_WGCTL0_ZPCTL1_Msk            (0x3ul << PWM_WGCTL0_ZPCTL1_Pos)                  /*!< PWM_T::WGCTL0: ZPCTL1 Mask             */

#define PWM_WGCTL0_ZPCTL2_Pos            (4)                                               /*!< PWM_T::WGCTL0: ZPCTL2 Position         */
#define PWM_WGCTL0_ZPCTL2_Msk            (0x3ul << PWM_WGCTL0_ZPCTL2_Pos)                  /*!< PWM_T::WGCTL0: ZPCTL2 Mask             */

#define PWM_WGCTL0_ZPCTL3_Pos            (6)                                               /*!< PWM_T::WGCTL0: ZPCTL3 Position         */
#define PWM_WGCTL0_ZPCTL3_Msk            (0x3ul << PWM_WGCTL0_ZPCTL3_Pos)                  /*!< PWM_T::WGCTL0: ZPCTL3 Mask             */

#define PWM_WGCTL0_ZPCTL4_Pos            (8)                                               /*!< PWM_T::WGCTL0: ZPCTL4 Position         */
#define PWM_WGCTL0_ZPCTL4_Msk            (0x3ul << PWM_WGCTL0_ZPCTL4_Pos)                  /*!< PWM_T::WGCTL0: ZPCTL4 Mask             */

#define PWM_WGCTL0_ZPCTL5_Pos            (10)                                              /*!< PWM_T::WGCTL0: ZPCTL5 Position         */
#define PWM_WGCTL0_ZPCTL5_Msk            (0x3ul << PWM_WGCTL0_ZPCTL5_Pos)                  /*!< PWM_T::WGCTL0: ZPCTL5 Mask             */

#define PWM_WGCTL0_PRDPCTL0_Pos          (16)                                              /*!< PWM_T::WGCTL0: PRDPCTL0 Position       */
#define PWM_WGCTL0_PRDPCTL0_Msk          (0x3ul << PWM_WGCTL0_PRDPCTL0_Pos)                /*!< PWM_T::WGCTL0: PRDPCTL0 Mask           */

#define PWM_WGCTL0_PRDPCTL1_Pos          (18)                                              /*!< PWM_T::WGCTL0: PRDPCTL1 Position       */
#define PWM_WGCTL0_PRDPCTL1_Msk          (0x3ul << PWM_WGCTL0_PRDPCTL1_Pos)                /*!< PWM_T::WGCTL0: PRDPCTL1 Mask           */

#define PWM_WGCTL0_PRDPCTL2_Pos          (20)                                              /*!< PWM_T::WGCTL0: PRDPCTL2 Position       */
#define PWM_WGCTL0_PRDPCTL2_Msk          (0x3ul << PWM_WGCTL0_PRDPCTL2_Pos)                /*!< PWM_T::WGCTL0: PRDPCTL2 Mask           */

#define PWM_WGCTL0_PRDPCTL3_Pos          (22)                                              /*!< PWM_T::WGCTL0: PRDPCTL3 Position       */
#define PWM_WGCTL0_PRDPCTL3_Msk          (0x3ul << PWM_WGCTL0_PRDPCTL3_Pos)                /*!< PWM_T::WGCTL0: PRDPCTL3 Mask           */

#define PWM_WGCTL0_PRDPCTL4_Pos          (24)                                              /*!< PWM_T::WGCTL0: PRDPCTL4 Position       */
#define PWM_WGCTL0_PRDPCTL4_Msk          (0x3ul << PWM_WGCTL0_PRDPCTL4_Pos)                /*!< PWM_T::WGCTL0: PRDPCTL4 Mask           */

#define PWM_WGCTL0_PRDPCTL5_Pos          (26)                                              /*!< PWM_T::WGCTL0: PRDPCTL5 Position       */
#define PWM_WGCTL0_PRDPCTL5_Msk          (0x3ul << PWM_WGCTL0_PRDPCTL5_Pos)                /*!< PWM_T::WGCTL0: PRDPCTL5 Mask           */

#define PWM_WGCTL1_CMPUCTLn_Pos          (0)                                               /*!< PWM_T::WGCTL1: CMPUCTLn Position       */
#define PWM_WGCTL1_CMPUCTLn_Msk          (0xffful << PWM_WGCTL1_CMPUCTLn_Pos)              /*!< PWM_T::WGCTL1: CMPUCTLn Mask           */

#define PWM_WGCTL1_CMPUCTL0_Pos          (0)                                               /*!< PWM_T::WGCTL1: CMPUCTL0 Position       */
#define PWM_WGCTL1_CMPUCTL0_Msk          (0x3ul << PWM_WGCTL1_CMPUCTL0_Pos)                /*!< PWM_T::WGCTL1: CMPUCTL0 Mask           */

#define PWM_WGCTL1_CMPUCTL1_Pos          (2)                                               /*!< PWM_T::WGCTL1: CMPUCTL1 Position       */
#define PWM_WGCTL1_CMPUCTL1_Msk          (0x3ul << PWM_WGCTL1_CMPUCTL1_Pos)                /*!< PWM_T::WGCTL1: CMPUCTL1 Mask           */

#define PWM_WGCTL1_CMPUCTL2_Pos          (4)                                               /*!< PWM_T::WGCTL1: CMPUCTL2 Position       */
#define PWM_WGCTL1_CMPUCTL2_Msk          (0x3ul << PWM_WGCTL1_CMPUCTL2_Pos)                /*!< PWM_T::WGCTL1: CMPUCTL2 Mask           */

#define PWM_WGCTL1_CMPUCTL3_Pos          (6)                                               /*!< PWM_T::WGCTL1: CMPUCTL3 Position       */
#define PWM_WGCTL1_CMPUCTL3_Msk          (0x3ul << PWM_WGCTL1_CMPUCTL3_Pos)                /*!< PWM_T::WGCTL1: CMPUCTL3 Mask           */

#define PWM_WGCTL1_CMPUCTL4_Pos          (8)                                               /*!< PWM_T::WGCTL1: CMPUCTL4 Position       */
#define PWM_WGCTL1_CMPUCTL4_Msk          (0x3ul << PWM_WGCTL1_CMPUCTL4_Pos)                /*!< PWM_T::WGCTL1: CMPUCTL4 Mask           */

#define PWM_WGCTL1_CMPUCTL5_Pos          (10)                                              /*!< PWM_T::WGCTL1: CMPUCTL5 Position       */
#define PWM_WGCTL1_CMPUCTL5_Msk          (0x3ul << PWM_WGCTL1_CMPUCTL5_Pos)                /*!< PWM_T::WGCTL1: CMPUCTL5 Mask           */

#define PWM_WGCTL1_CMPDCTL0_Pos          (16)                                              /*!< PWM_T::WGCTL1: CMPDCTL0 Position       */
#define PWM_WGCTL1_CMPDCTL0_Msk          (0x3ul << PWM_WGCTL1_CMPDCTL0_Pos)                /*!< PWM_T::WGCTL1: CMPDCTL0 Mask           */

#define PWM_WGCTL1_CMPDCTL1_Pos          (18)                                              /*!< PWM_T::WGCTL1: CMPDCTL1 Position       */
#define PWM_WGCTL1_CMPDCTL1_Msk          (0x3ul << PWM_WGCTL1_CMPDCTL1_Pos)                /*!< PWM_T::WGCTL1: CMPDCTL1 Mask           */

#define PWM_WGCTL1_CMPDCTL2_Pos          (20)                                              /*!< PWM_T::WGCTL1: CMPDCTL2 Position       */
#define PWM_WGCTL1_CMPDCTL2_Msk          (0x3ul << PWM_WGCTL1_CMPDCTL2_Pos)                /*!< PWM_T::WGCTL1: CMPDCTL2 Mask           */

#define PWM_WGCTL1_CMPDCTL3_Pos          (22)                                              /*!< PWM_T::WGCTL1: CMPDCTL3 Position       */
#define PWM_WGCTL1_CMPDCTL3_Msk          (0x3ul << PWM_WGCTL1_CMPDCTL3_Pos)                /*!< PWM_T::WGCTL1: CMPDCTL3 Mask           */

#define PWM_WGCTL1_CMPDCTL4_Pos          (24)                                              /*!< PWM_T::WGCTL1: CMPDCTL4 Position       */
#define PWM_WGCTL1_CMPDCTL4_Msk          (0x3ul << PWM_WGCTL1_CMPDCTL4_Pos)                /*!< PWM_T::WGCTL1: CMPDCTL4 Mask           */

#define PWM_WGCTL1_CMPDCTL5_Pos          (26)                                              /*!< PWM_T::WGCTL1: CMPDCTL5 Position       */
#define PWM_WGCTL1_CMPDCTL5_Msk          (0x3ul << PWM_WGCTL1_CMPDCTL5_Pos)                /*!< PWM_T::WGCTL1: CMPDCTL5 Mask           */

#define PWM_MSKEN_MSKEN0_Pos             (0)                                               /*!< PWM_T::MSKEN: MSKEN0 Position          */
#define PWM_MSKEN_MSKEN0_Msk             (0x1ul << PWM_MSKEN_MSKEN0_Pos)                   /*!< PWM_T::MSKEN: MSKEN0 Mask              */

#define PWM_MSKEN_MSKEN1_Pos             (1)                                               /*!< PWM_T::MSKEN: MSKEN1 Position          */
#define PWM_MSKEN_MSKEN1_Msk             (0x1ul << PWM_MSKEN_MSKEN1_Pos)                   /*!< PWM_T::MSKEN: MSKEN1 Mask              */

#define PWM_MSKEN_MSKEN2_Pos             (2)                                               /*!< PWM_T::MSKEN: MSKEN2 Position          */
#define PWM_MSKEN_MSKEN2_Msk             (0x1ul << PWM_MSKEN_MSKEN2_Pos)                   /*!< PWM_T::MSKEN: MSKEN2 Mask              */

#define PWM_MSKEN_MSKEN3_Pos             (3)                                               /*!< PWM_T::MSKEN: MSKEN3 Position          */
#define PWM_MSKEN_MSKEN3_Msk             (0x1ul << PWM_MSKEN_MSKEN3_Pos)                   /*!< PWM_T::MSKEN: MSKEN3 Mask              */

#define PWM_MSKEN_MSKEN4_Pos             (4)                                               /*!< PWM_T::MSKEN: MSKEN4 Position          */
#define PWM_MSKEN_MSKEN4_Msk             (0x1ul << PWM_MSKEN_MSKEN4_Pos)                   /*!< PWM_T::MSKEN: MSKEN4 Mask              */

#define PWM_MSKEN_MSKEN5_Pos             (5)                                               /*!< PWM_T::MSKEN: MSKEN5 Position          */
#define PWM_MSKEN_MSKEN5_Msk             (0x1ul << PWM_MSKEN_MSKEN5_Pos)                   /*!< PWM_T::MSKEN: MSKEN5 Mask              */

#define PWM_MSK_MSKDAT0_Pos              (0)                                               /*!< PWM_T::MSK: MSKDAT0 Position           */
#define PWM_MSK_MSKDAT0_Msk              (0x1ul << PWM_MSK_MSKDAT0_Pos)                    /*!< PWM_T::MSK: MSKDAT0 Mask               */

#define PWM_MSK_MSKDAT1_Pos              (1)                                               /*!< PWM_T::MSK: MSKDAT1 Position           */
#define PWM_MSK_MSKDAT1_Msk              (0x1ul << PWM_MSK_MSKDAT1_Pos)                    /*!< PWM_T::MSK: MSKDAT1 Mask               */

#define PWM_MSK_MSKDAT2_Pos              (2)                                               /*!< PWM_T::MSK: MSKDAT2 Position           */
#define PWM_MSK_MSKDAT2_Msk              (0x1ul << PWM_MSK_MSKDAT2_Pos)                    /*!< PWM_T::MSK: MSKDAT2 Mask               */

#define PWM_MSK_MSKDAT3_Pos              (3)                                               /*!< PWM_T::MSK: MSKDAT3 Position           */
#define PWM_MSK_MSKDAT3_Msk              (0x1ul << PWM_MSK_MSKDAT3_Pos)                    /*!< PWM_T::MSK: MSKDAT3 Mask               */

#define PWM_MSK_MSKDAT4_Pos              (4)                                               /*!< PWM_T::MSK: MSKDAT4 Position           */
#define PWM_MSK_MSKDAT4_Msk              (0x1ul << PWM_MSK_MSKDAT4_Pos)                    /*!< PWM_T::MSK: MSKDAT4 Mask               */

#define PWM_MSK_MSKDAT5_Pos              (5)                                               /*!< PWM_T::MSK: MSKDAT5 Position           */
#define PWM_MSK_MSKDAT5_Msk              (0x1ul << PWM_MSK_MSKDAT5_Pos)                    /*!< PWM_T::MSK: MSKDAT5 Mask               */

#define PWM_BNF_BRK0FEN_Pos              (0)                                               /*!< PWM_T::BNF: BRK0NFEN Position          */
#define PWM_BNF_BRK0FEN_Msk              (0x1ul << PWM_BNF_BRK0FEN_Pos)                   /*!< PWM_T::BNF: BRK0NFEN Mask              */

#define PWM_BNF_BRK0NFSEL_Pos            (1)                                               /*!< PWM_T::BNF: BRK0NFSEL Position         */
#define PWM_BNF_BRK0NFSEL_Msk            (0x7ul << PWM_BNF_BRK0NFSEL_Pos)                  /*!< PWM_T::BNF: BRK0NFSEL Mask             */

#define PWM_BNF_BRK0NFCNT_Pos            (4)                                               /*!< PWM_T::BNF: BRK0NFCNT Position         */
#define PWM_BNF_BRK0NFCNT_Msk            (0x7ul << PWM_BNF_BRK0NFCNT_Pos)                  /*!< PWM_T::BNF: BRK0NFCNT Mask             */

#define PWM_BNF_BRK0PINV_Pos             (7)                                               /*!< PWM_T::BNF: BRK0PINV Position          */
#define PWM_BNF_BRK0PINV_Msk             (0x1ul << PWM_BNF_BRK0PINV_Pos)                   /*!< PWM_T::BNF: BRK0PINV Mask              */

#define PWM_BNF_BRK1FEN_Pos              (8)                                               /*!< PWM_T::BNF: BRK1FEN Position           */
#define PWM_BNF_BRK1FEN_Msk              (0x1ul << PWM_BNF_BRK1FEN_Pos)                    /*!< PWM_T::BNF: BRK1FEN Mask               */

#define PWM_BNF_BRK1NFSEL_Pos            (9)                                               /*!< PWM_T::BNF: BRK1NFSEL Position         */
#define PWM_BNF_BRK1NFSEL_Msk            (0x7ul << PWM_BNF_BRK1NFSEL_Pos)                  /*!< PWM_T::BNF: BRK1NFSEL Mask             */

#define PWM_BNF_BRK1FCNT_Pos             (12)                                              /*!< PWM_T::BNF: BRK1FCNT Position          */
#define PWM_BNF_BRK1FCNT_Msk             (0x7ul << PWM_BNF_BRK1FCNT_Pos)                   /*!< PWM_T::BNF: BRK1FCNT Mask              */

#define PWM_BNF_BRK1PINV_Pos             (15)                                              /*!< PWM_T::BNF: BRK1PINV Position          */
#define PWM_BNF_BRK1PINV_Msk             (0x1ul << PWM_BNF_BRK1PINV_Pos)                   /*!< PWM_T::BNF: BRK1PINV Mask              */

#define PWM_BNF_BK0SRC_Pos               (16)                                              /*!< PWM_T::BNF: BK0SRC Position            */
#define PWM_BNF_BK0SRC_Msk               (0x1ul << PWM_BNF_BK0SRC_Pos)                     /*!< PWM_T::BNF: BK0SRC Mask                */

#define PWM_BNF_BK1SRC_Pos               (24)                                              /*!< PWM_T::BNF: BK1SRC Position            */
#define PWM_BNF_BK1SRC_Msk               (0x1ul << PWM_BNF_BK1SRC_Pos)                     /*!< PWM_T::BNF: BK1SRC Mask                */

#define PWM_FAILBRK_CSSBRKEN_Pos         (0)                                               /*!< PWM_T::FAILBRK: CSSBRKEN Position      */
#define PWM_FAILBRK_CSSBRKEN_Msk         (0x1ul << PWM_FAILBRK_CSSBRKEN_Pos)               /*!< PWM_T::FAILBRK: CSSBRKEN Mask          */

#define PWM_FAILBRK_BODBRKEN_Pos         (1)                                               /*!< PWM_T::FAILBRK: BODBRKEN Position      */
#define PWM_FAILBRK_BODBRKEN_Msk         (0x1ul << PWM_FAILBRK_BODBRKEN_Pos)               /*!< PWM_T::FAILBRK: BODBRKEN Mask          */

#define PWM_FAILBRK_CORBRKEN_Pos         (3)                                               /*!< PWM_T::FAILBRK: CORBRKEN Position      */
#define PWM_FAILBRK_CORBRKEN_Msk         (0x1ul << PWM_FAILBRK_CORBRKEN_Pos)               /*!< PWM_T::FAILBRK: CORBRKEN Mask          */

#define PWM_BRKCTL_CPO0EBEN_Pos          (0)                                               /*!< PWM_T::BRKCTL: CPO0EBEN Position    */
#define PWM_BRKCTL_CPO0EBEN_Msk          (0x1ul << PWM_BRKCTL_CPO0EBEN_Pos)             /*!< PWM_T::BRKCTL: CPO0EBEN Mask        */

#define PWM_BRKCTL_CPO1EBEN_Pos          (1)                                               /*!< PWM_T::BRKCTL: CPO1EBEN Position    */
#define PWM_BRKCTL_CPO1EBEN_Msk          (0x1ul << PWM_BRKCTL_CPO1EBEN_Pos)             /*!< PWM_T::BRKCTL: CPO1EBEN Mask        */

#define PWM_BRKCTL_BRKP0EEN_Pos          (4)                                               /*!< PWM_T::BRKCTL: BRKP0EEN Position    */
#define PWM_BRKCTL_BRKP0EEN_Msk          (0x1ul << PWM_BRKCTL_BRKP0EEN_Pos)             /*!< PWM_T::BRKCTL: BRKP0EEN Mask        */

#define PWM_BRKCTL_BRKP1EEN_Pos          (5)                                               /*!< PWM_T::BRKCTL: BRKP1EEN Position    */
#define PWM_BRKCTL_BRKP1EEN_Msk          (0x1ul << PWM_BRKCTL_BRKP1EEN_Pos)             /*!< PWM_T::BRKCTL: BRKP1EEN Mask        */

#define PWM_BRKCTL_SYSEBEN_Pos           (7)                                               /*!< PWM_T::BRKCTL: SYSEBEN Position     */
#define PWM_BRKCTL_SYSEBEN_Msk           (0x1ul << PWM_BRKCTL_SYSEBEN_Pos)              /*!< PWM_T::BRKCTL: SYSEBEN Mask         */

#define PWM_BRKCTL_CPO0LBEN_Pos          (8)                                               /*!< PWM_T::BRKCTL: CPO0LBEN Position    */
#define PWM_BRKCTL_CPO0LBEN_Msk          (0x1ul << PWM_BRKCTL_CPO0LBEN_Pos)             /*!< PWM_T::BRKCTL: CPO0LBEN Mask        */

#define PWM_BRKCTL_CPO1LBEN_Pos          (9)                                               /*!< PWM_T::BRKCTL: CPO1LBEN Position    */
#define PWM_BRKCTL_CPO1LBEN_Msk          (0x1ul << PWM_BRKCTL_CPO1LBEN_Pos)             /*!< PWM_T::BRKCTL: CPO1LBEN Mask        */

#define PWM_BRKCTL_BRKP0LEN_Pos          (12)                                              /*!< PWM_T::BRKCTL: BRKP0LEN Position    */
#define PWM_BRKCTL_BRKP0LEN_Msk          (0x1ul << PWM_BRKCTL_BRKP0LEN_Pos)             /*!< PWM_T::BRKCTL: BRKP0LEN Mask        */

#define PWM_BRKCTL_BRKP1LEN_Pos          (13)                                              /*!< PWM_T::BRKCTL: BRKP1LEN Position    */
#define PWM_BRKCTL_BRKP1LEN_Msk          (0x1ul << PWM_BRKCTL_BRKP1LEN_Pos)             /*!< PWM_T::BRKCTL: BRKP1LEN Mask        */

#define PWM_BRKCTL_SYSLBEN_Pos           (15)                                              /*!< PWM_T::BRKCTL: SYSLBEN Position     */
#define PWM_BRKCTL_SYSLBEN_Msk           (0x1ul << PWM_BRKCTL_SYSLBEN_Pos)              /*!< PWM_T::BRKCTL: SYSLBEN Mask         */

#define PWM_BRKCTL_BRKAEVEN_Pos          (16)                                              /*!< PWM_T::BRKCTL: BRKAEVEN Position    */
#define PWM_BRKCTL_BRKAEVEN_Msk          (0x3ul << PWM_BRKCTL_BRKAEVEN_Pos)             /*!< PWM_T::BRKCTL: BRKAEVEN Mask        */

#define PWM_BRKCTL_BRKAODD_Pos           (18)                                              /*!< PWM_T::BRKCTL: BRKAODD Position     */
#define PWM_BRKCTL_BRKAODD_Msk           (0x3ul << PWM_BRKCTL_BRKAODD_Pos)              /*!< PWM_T::BRKCTL: BRKAODD Mask         */

#define PWM_BRKCTL0_1_CPO0EBEN_Pos       (0)                                               /*!< PWM_T::BRKCTL0_1: CPO0EBEN Position    */
#define PWM_BRKCTL0_1_CPO0EBEN_Msk       (0x1ul << PWM_BRKCTL0_1_CPO0EBEN_Pos)             /*!< PWM_T::BRKCTL0_1: CPO0EBEN Mask        */

#define PWM_BRKCTL0_1_CPO1EBEN_Pos       (1)                                               /*!< PWM_T::BRKCTL0_1: CPO1EBEN Position    */
#define PWM_BRKCTL0_1_CPO1EBEN_Msk       (0x1ul << PWM_BRKCTL0_1_CPO1EBEN_Pos)             /*!< PWM_T::BRKCTL0_1: CPO1EBEN Mask        */

#define PWM_BRKCTL0_1_BRKP0EEN_Pos       (4)                                               /*!< PWM_T::BRKCTL0_1: BRKP0EEN Position    */
#define PWM_BRKCTL0_1_BRKP0EEN_Msk       (0x1ul << PWM_BRKCTL0_1_BRKP0EEN_Pos)             /*!< PWM_T::BRKCTL0_1: BRKP0EEN Mask        */

#define PWM_BRKCTL0_1_BRKP1EEN_Pos       (5)                                               /*!< PWM_T::BRKCTL0_1: BRKP1EEN Position    */
#define PWM_BRKCTL0_1_BRKP1EEN_Msk       (0x1ul << PWM_BRKCTL0_1_BRKP1EEN_Pos)             /*!< PWM_T::BRKCTL0_1: BRKP1EEN Mask        */

#define PWM_BRKCTL0_1_SYSEBEN_Pos        (7)                                               /*!< PWM_T::BRKCTL0_1: SYSEBEN Position     */
#define PWM_BRKCTL0_1_SYSEBEN_Msk        (0x1ul << PWM_BRKCTL0_1_SYSEBEN_Pos)              /*!< PWM_T::BRKCTL0_1: SYSEBEN Mask         */

#define PWM_BRKCTL0_1_CPO0LBEN_Pos       (8)                                               /*!< PWM_T::BRKCTL0_1: CPO0LBEN Position    */
#define PWM_BRKCTL0_1_CPO0LBEN_Msk       (0x1ul << PWM_BRKCTL0_1_CPO0LBEN_Pos)             /*!< PWM_T::BRKCTL0_1: CPO0LBEN Mask        */

#define PWM_BRKCTL0_1_CPO1LBEN_Pos       (9)                                               /*!< PWM_T::BRKCTL0_1: CPO1LBEN Position    */
#define PWM_BRKCTL0_1_CPO1LBEN_Msk       (0x1ul << PWM_BRKCTL0_1_CPO1LBEN_Pos)             /*!< PWM_T::BRKCTL0_1: CPO1LBEN Mask        */

#define PWM_BRKCTL0_1_BRKP0LEN_Pos       (12)                                              /*!< PWM_T::BRKCTL0_1: BRKP0LEN Position    */
#define PWM_BRKCTL0_1_BRKP0LEN_Msk       (0x1ul << PWM_BRKCTL0_1_BRKP0LEN_Pos)             /*!< PWM_T::BRKCTL0_1: BRKP0LEN Mask        */

#define PWM_BRKCTL0_1_BRKP1LEN_Pos       (13)                                              /*!< PWM_T::BRKCTL0_1: BRKP1LEN Position    */
#define PWM_BRKCTL0_1_BRKP1LEN_Msk       (0x1ul << PWM_BRKCTL0_1_BRKP1LEN_Pos)             /*!< PWM_T::BRKCTL0_1: BRKP1LEN Mask        */

#define PWM_BRKCTL0_1_SYSLBEN_Pos        (15)                                              /*!< PWM_T::BRKCTL0_1: SYSLBEN Position     */
#define PWM_BRKCTL0_1_SYSLBEN_Msk        (0x1ul << PWM_BRKCTL0_1_SYSLBEN_Pos)              /*!< PWM_T::BRKCTL0_1: SYSLBEN Mask         */

#define PWM_BRKCTL0_1_BRKAEVEN_Pos       (16)                                              /*!< PWM_T::BRKCTL0_1: BRKAEVEN Position    */
#define PWM_BRKCTL0_1_BRKAEVEN_Msk       (0x3ul << PWM_BRKCTL0_1_BRKAEVEN_Pos)             /*!< PWM_T::BRKCTL0_1: BRKAEVEN Mask        */

#define PWM_BRKCTL0_1_BRKAODD_Pos        (18)                                              /*!< PWM_T::BRKCTL0_1: BRKAODD Position     */
#define PWM_BRKCTL0_1_BRKAODD_Msk        (0x3ul << PWM_BRKCTL0_1_BRKAODD_Pos)              /*!< PWM_T::BRKCTL0_1: BRKAODD Mask         */

#define PWM_BRKCTL2_3_CPO0EBEN_Pos       (0)                                               /*!< PWM_T::BRKCTL2_3: CPO0EBEN Position    */
#define PWM_BRKCTL2_3_CPO0EBEN_Msk       (0x1ul << PWM_BRKCTL2_3_CPO0EBEN_Pos)             /*!< PWM_T::BRKCTL2_3: CPO0EBEN Mask        */

#define PWM_BRKCTL2_3_CPO1EBEN_Pos       (1)                                               /*!< PWM_T::BRKCTL2_3: CPO1EBEN Position    */
#define PWM_BRKCTL2_3_CPO1EBEN_Msk       (0x1ul << PWM_BRKCTL2_3_CPO1EBEN_Pos)             /*!< PWM_T::BRKCTL2_3: CPO1EBEN Mask        */

#define PWM_BRKCTL2_3_BRKP0EEN_Pos       (4)                                               /*!< PWM_T::BRKCTL2_3: BRKP0EEN Position    */
#define PWM_BRKCTL2_3_BRKP0EEN_Msk       (0x1ul << PWM_BRKCTL2_3_BRKP0EEN_Pos)             /*!< PWM_T::BRKCTL2_3: BRKP0EEN Mask        */

#define PWM_BRKCTL2_3_BRKP1EEN_Pos       (5)                                               /*!< PWM_T::BRKCTL2_3: BRKP1EEN Position    */
#define PWM_BRKCTL2_3_BRKP1EEN_Msk       (0x1ul << PWM_BRKCTL2_3_BRKP1EEN_Pos)             /*!< PWM_T::BRKCTL2_3: BRKP1EEN Mask        */

#define PWM_BRKCTL2_3_SYSEBEN_Pos        (7)                                               /*!< PWM_T::BRKCTL2_3: SYSEBEN Position     */
#define PWM_BRKCTL2_3_SYSEBEN_Msk        (0x1ul << PWM_BRKCTL2_3_SYSEBEN_Pos)              /*!< PWM_T::BRKCTL2_3: SYSEBEN Mask         */

#define PWM_BRKCTL2_3_CPO0LBEN_Pos       (8)                                               /*!< PWM_T::BRKCTL2_3: CPO0LBEN Position    */
#define PWM_BRKCTL2_3_CPO0LBEN_Msk       (0x1ul << PWM_BRKCTL2_3_CPO0LBEN_Pos)             /*!< PWM_T::BRKCTL2_3: CPO0LBEN Mask        */

#define PWM_BRKCTL2_3_CPO1LBEN_Pos       (9)                                               /*!< PWM_T::BRKCTL2_3: CPO1LBEN Position    */
#define PWM_BRKCTL2_3_CPO1LBEN_Msk       (0x1ul << PWM_BRKCTL2_3_CPO1LBEN_Pos)             /*!< PWM_T::BRKCTL2_3: CPO1LBEN Mask        */

#define PWM_BRKCTL2_3_BRKP0LEN_Pos       (12)                                              /*!< PWM_T::BRKCTL2_3: BRKP0LEN Position    */
#define PWM_BRKCTL2_3_BRKP0LEN_Msk       (0x1ul << PWM_BRKCTL2_3_BRKP0LEN_Pos)             /*!< PWM_T::BRKCTL2_3: BRKP0LEN Mask        */

#define PWM_BRKCTL2_3_BRKP1LEN_Pos       (13)                                              /*!< PWM_T::BRKCTL2_3: BRKP1LEN Position    */
#define PWM_BRKCTL2_3_BRKP1LEN_Msk       (0x1ul << PWM_BRKCTL2_3_BRKP1LEN_Pos)             /*!< PWM_T::BRKCTL2_3: BRKP1LEN Mask        */

#define PWM_BRKCTL2_3_SYSLBEN_Pos        (15)                                              /*!< PWM_T::BRKCTL2_3: SYSLBEN Position     */
#define PWM_BRKCTL2_3_SYSLBEN_Msk        (0x1ul << PWM_BRKCTL2_3_SYSLBEN_Pos)              /*!< PWM_T::BRKCTL2_3: SYSLBEN Mask         */

#define PWM_BRKCTL2_3_BRKAEVEN_Pos       (16)                                              /*!< PWM_T::BRKCTL2_3: BRKAEVEN Position    */
#define PWM_BRKCTL2_3_BRKAEVEN_Msk       (0x3ul << PWM_BRKCTL2_3_BRKAEVEN_Pos)             /*!< PWM_T::BRKCTL2_3: BRKAEVEN Mask        */

#define PWM_BRKCTL2_3_BRKAODD_Pos        (18)                                              /*!< PWM_T::BRKCTL2_3: BRKAODD Position     */
#define PWM_BRKCTL2_3_BRKAODD_Msk        (0x3ul << PWM_BRKCTL2_3_BRKAODD_Pos)              /*!< PWM_T::BRKCTL2_3: BRKAODD Mask         */

#define PWM_BRKCTL4_5_CPO0EBEN_Pos       (0)                                               /*!< PWM_T::BRKCTL4_5: CPO0EBEN Position    */
#define PWM_BRKCTL4_5_CPO0EBEN_Msk       (0x1ul << PWM_BRKCTL4_5_CPO0EBEN_Pos)             /*!< PWM_T::BRKCTL4_5: CPO0EBEN Mask        */

#define PWM_BRKCTL4_5_CPO1EBEN_Pos       (1)                                               /*!< PWM_T::BRKCTL4_5: CPO1EBEN Position    */
#define PWM_BRKCTL4_5_CPO1EBEN_Msk       (0x1ul << PWM_BRKCTL4_5_CPO1EBEN_Pos)             /*!< PWM_T::BRKCTL4_5: CPO1EBEN Mask        */

#define PWM_BRKCTL4_5_BRKP0EEN_Pos       (4)                                               /*!< PWM_T::BRKCTL4_5: BRKP0EEN Position    */
#define PWM_BRKCTL4_5_BRKP0EEN_Msk       (0x1ul << PWM_BRKCTL4_5_BRKP0EEN_Pos)             /*!< PWM_T::BRKCTL4_5: BRKP0EEN Mask        */

#define PWM_BRKCTL4_5_BRKP1EEN_Pos       (5)                                               /*!< PWM_T::BRKCTL4_5: BRKP1EEN Position    */
#define PWM_BRKCTL4_5_BRKP1EEN_Msk       (0x1ul << PWM_BRKCTL4_5_BRKP1EEN_Pos)             /*!< PWM_T::BRKCTL4_5: BRKP1EEN Mask        */

#define PWM_BRKCTL4_5_SYSEBEN_Pos        (7)                                               /*!< PWM_T::BRKCTL4_5: SYSEBEN Position     */
#define PWM_BRKCTL4_5_SYSEBEN_Msk        (0x1ul << PWM_BRKCTL4_5_SYSEBEN_Pos)              /*!< PWM_T::BRKCTL4_5: SYSEBEN Mask         */

#define PWM_BRKCTL4_5_CPO0LBEN_Pos       (8)                                               /*!< PWM_T::BRKCTL4_5: CPO0LBEN Position    */
#define PWM_BRKCTL4_5_CPO0LBEN_Msk       (0x1ul << PWM_BRKCTL4_5_CPO0LBEN_Pos)             /*!< PWM_T::BRKCTL4_5: CPO0LBEN Mask        */

#define PWM_BRKCTL4_5_CPO1LBEN_Pos       (9)                                               /*!< PWM_T::BRKCTL4_5: CPO1LBEN Position    */
#define PWM_BRKCTL4_5_CPO1LBEN_Msk       (0x1ul << PWM_BRKCTL4_5_CPO1LBEN_Pos)             /*!< PWM_T::BRKCTL4_5: CPO1LBEN Mask        */

#define PWM_BRKCTL4_5_BRKP0LEN_Pos       (12)                                              /*!< PWM_T::BRKCTL4_5: BRKP0LEN Position    */
#define PWM_BRKCTL4_5_BRKP0LEN_Msk       (0x1ul << PWM_BRKCTL4_5_BRKP0LEN_Pos)             /*!< PWM_T::BRKCTL4_5: BRKP0LEN Mask        */

#define PWM_BRKCTL4_5_BRKP1LEN_Pos       (13)                                              /*!< PWM_T::BRKCTL4_5: BRKP1LEN Position    */
#define PWM_BRKCTL4_5_BRKP1LEN_Msk       (0x1ul << PWM_BRKCTL4_5_BRKP1LEN_Pos)             /*!< PWM_T::BRKCTL4_5: BRKP1LEN Mask        */

#define PWM_BRKCTL4_5_SYSLBEN_Pos        (15)                                              /*!< PWM_T::BRKCTL4_5: SYSLBEN Position     */
#define PWM_BRKCTL4_5_SYSLBEN_Msk        (0x1ul << PWM_BRKCTL4_5_SYSLBEN_Pos)              /*!< PWM_T::BRKCTL4_5: SYSLBEN Mask         */

#define PWM_BRKCTL4_5_BRKAEVEN_Pos       (16)                                              /*!< PWM_T::BRKCTL4_5: BRKAEVEN Position    */
#define PWM_BRKCTL4_5_BRKAEVEN_Msk       (0x3ul << PWM_BRKCTL4_5_BRKAEVEN_Pos)             /*!< PWM_T::BRKCTL4_5: BRKAEVEN Mask        */

#define PWM_BRKCTL4_5_BRKAODD_Pos        (18)                                              /*!< PWM_T::BRKCTL4_5: BRKAODD Position     */
#define PWM_BRKCTL4_5_BRKAODD_Msk        (0x3ul << PWM_BRKCTL4_5_BRKAODD_Pos)              /*!< PWM_T::BRKCTL4_5: BRKAODD Mask         */

#define PWM_POLCTL_PINVn_Pos             (0)                                               /*!< PWM_T::POLCTL: PINVn Position          */
#define PWM_POLCTL_PINVn_Msk             (0x3ful << PWM_POLCTL_PINVn_Pos)                  /*!< PWM_T::POLCTL: PINVn Mask              */

#define PWM_POLCTL_PINV0_Pos             (0)                                               /*!< PWM_T::POLCTL: PINV0 Position          */
#define PWM_POLCTL_PINV0_Msk             (0x1ul << PWM_POLCTL_PINV0_Pos)                   /*!< PWM_T::POLCTL: PINV0 Mask              */

#define PWM_POLCTL_PINV1_Pos             (1)                                               /*!< PWM_T::POLCTL: PINV1 Position          */
#define PWM_POLCTL_PINV1_Msk             (0x1ul << PWM_POLCTL_PINV1_Pos)                   /*!< PWM_T::POLCTL: PINV1 Mask              */

#define PWM_POLCTL_PINV2_Pos             (2)                                               /*!< PWM_T::POLCTL: PINV2 Position          */
#define PWM_POLCTL_PINV2_Msk             (0x1ul << PWM_POLCTL_PINV2_Pos)                   /*!< PWM_T::POLCTL: PINV2 Mask              */

#define PWM_POLCTL_PINV3_Pos             (3)                                               /*!< PWM_T::POLCTL: PINV3 Position          */
#define PWM_POLCTL_PINV3_Msk             (0x1ul << PWM_POLCTL_PINV3_Pos)                   /*!< PWM_T::POLCTL: PINV3 Mask              */

#define PWM_POLCTL_PINV4_Pos             (4)                                               /*!< PWM_T::POLCTL: PINV4 Position          */
#define PWM_POLCTL_PINV4_Msk             (0x1ul << PWM_POLCTL_PINV4_Pos)                   /*!< PWM_T::POLCTL: PINV4 Mask              */

#define PWM_POLCTL_PINV5_Pos             (5)                                               /*!< PWM_T::POLCTL: PINV5 Position          */
#define PWM_POLCTL_PINV5_Msk             (0x1ul << PWM_POLCTL_PINV5_Pos)                   /*!< PWM_T::POLCTL: PINV5 Mask              */

#define PWM_POEN_POENn_Pos               (0)                                               /*!< PWM_T::POEN: POENn Position            */
#define PWM_POEN_POENn_Msk               (0x3ful << PWM_POEN_POENn_Pos)                    /*!< PWM_T::POEN: POENn Mask                */

#define PWM_POEN_POEN0_Pos               (0)                                               /*!< PWM_T::POEN: POEN0 Position            */
#define PWM_POEN_POEN0_Msk               (0x1ul << PWM_POEN_POEN0_Pos)                     /*!< PWM_T::POEN: POEN0 Mask                */

#define PWM_POEN_POEN1_Pos               (1)                                               /*!< PWM_T::POEN: POEN1 Position            */
#define PWM_POEN_POEN1_Msk               (0x1ul << PWM_POEN_POEN1_Pos)                     /*!< PWM_T::POEN: POEN1 Mask                */

#define PWM_POEN_POEN2_Pos               (2)                                               /*!< PWM_T::POEN: POEN2 Position            */
#define PWM_POEN_POEN2_Msk               (0x1ul << PWM_POEN_POEN2_Pos)                     /*!< PWM_T::POEN: POEN2 Mask                */

#define PWM_POEN_POEN3_Pos               (3)                                               /*!< PWM_T::POEN: POEN3 Position            */
#define PWM_POEN_POEN3_Msk               (0x1ul << PWM_POEN_POEN3_Pos)                     /*!< PWM_T::POEN: POEN3 Mask                */

#define PWM_POEN_POEN4_Pos               (4)                                               /*!< PWM_T::POEN: POEN4 Position            */
#define PWM_POEN_POEN4_Msk               (0x1ul << PWM_POEN_POEN4_Pos)                     /*!< PWM_T::POEN: POEN4 Mask                */

#define PWM_POEN_POEN5_Pos               (5)                                               /*!< PWM_T::POEN: POEN5 Position            */
#define PWM_POEN_POEN5_Msk               (0x1ul << PWM_POEN_POEN5_Pos)                     /*!< PWM_T::POEN: POEN5 Mask                */

#define PWM_SWBRK_BRKETRGn_Pos           (0)                                               /*!< PWM_T::SWBRK: BRKETRGn Position        */
#define PWM_SWBRK_BRKETRGn_Msk           (0x7ul << PWM_SWBRK_BRKETRGn_Pos)                 /*!< PWM_T::SWBRK: BRKETRGn Mask            */

#define PWM_SWBRK_BRKETRG0_Pos           (0)                                               /*!< PWM_T::SWBRK: BRKETRG0 Position        */
#define PWM_SWBRK_BRKETRG0_Msk           (0x1ul << PWM_SWBRK_BRKETRG0_Pos)                 /*!< PWM_T::SWBRK: BRKETRG0 Mask            */

#define PWM_SWBRK_BRKETRG2_Pos           (1)                                               /*!< PWM_T::SWBRK: BRKETRG2 Position        */
#define PWM_SWBRK_BRKETRG2_Msk           (0x1ul << PWM_SWBRK_BRKETRG2_Pos)                 /*!< PWM_T::SWBRK: BRKETRG2 Mask            */

#define PWM_SWBRK_BRKETRG4_Pos           (2)                                               /*!< PWM_T::SWBRK: BRKETRG4 Position        */
#define PWM_SWBRK_BRKETRG4_Msk           (0x1ul << PWM_SWBRK_BRKETRG4_Pos)                 /*!< PWM_T::SWBRK: BRKETRG4 Mask            */

#define PWM_SWBRK_BRKLTRGn_Pos           (8)                                               /*!< PWM_T::SWBRK: BRKLTRGn Position        */
#define PWM_SWBRK_BRKLTRGn_Msk           (0x7ul << PWM_SWBRK_BRKLTRGn_Pos)                 /*!< PWM_T::SWBRK: BRKLTRGn Mask            */

#define PWM_SWBRK_BRKLTRG0_Pos           (8)                                               /*!< PWM_T::SWBRK: BRKLTRG0 Position        */
#define PWM_SWBRK_BRKLTRG0_Msk           (0x1ul << PWM_SWBRK_BRKLTRG0_Pos)                 /*!< PWM_T::SWBRK: BRKLTRG0 Mask            */

#define PWM_SWBRK_BRKLTRG2_Pos           (9)                                               /*!< PWM_T::SWBRK: BRKLTRG2 Position        */
#define PWM_SWBRK_BRKLTRG2_Msk           (0x1ul << PWM_SWBRK_BRKLTRG2_Pos)                 /*!< PWM_T::SWBRK: BRKLTRG2 Mask            */

#define PWM_SWBRK_BRKLTRG4_Pos           (10)                                              /*!< PWM_T::SWBRK: BRKLTRG4 Position        */
#define PWM_SWBRK_BRKLTRG4_Msk           (0x1ul << PWM_SWBRK_BRKLTRG4_Pos)                 /*!< PWM_T::SWBRK: BRKLTRG4 Mask            */

#define PWM_INTEN0_ZIEN0_Pos             (0)                                               /*!< PWM_T::INTEN0: ZIEN0 Position          */
#define PWM_INTEN0_ZIEN0_Msk             (0x1ul << PWM_INTEN0_ZIEN0_Pos)                   /*!< PWM_T::INTEN0: ZIEN0 Mask              */

#define PWM_INTEN0_ZIEN2_Pos             (2)                                               /*!< PWM_T::INTEN0: ZIEN2 Position          */
#define PWM_INTEN0_ZIEN2_Msk             (0x1ul << PWM_INTEN0_ZIEN2_Pos)                   /*!< PWM_T::INTEN0: ZIEN2 Mask              */

#define PWM_INTEN0_ZIEN4_Pos             (4)                                               /*!< PWM_T::INTEN0: ZIEN4 Position          */
#define PWM_INTEN0_ZIEN4_Msk             (0x1ul << PWM_INTEN0_ZIEN4_Pos)                   /*!< PWM_T::INTEN0: ZIEN4 Mask              */

#define PWM_INTEN0_PIEN0_Pos             (8)                                               /*!< PWM_T::INTEN0: PIEN0 Position          */
#define PWM_INTEN0_PIEN0_Msk             (0x1ul << PWM_INTEN0_PIEN0_Pos)                   /*!< PWM_T::INTEN0: PIEN0 Mask              */

#define PWM_INTEN0_PIEN2_Pos             (10)                                              /*!< PWM_T::INTEN0: PIEN2 Position          */
#define PWM_INTEN0_PIEN2_Msk             (0x1ul << PWM_INTEN0_PIEN2_Pos)                   /*!< PWM_T::INTEN0: PIEN2 Mask              */

#define PWM_INTEN0_PIEN4_Pos             (12)                                              /*!< PWM_T::INTEN0: PIEN4 Position          */
#define PWM_INTEN0_PIEN4_Msk             (0x1ul << PWM_INTEN0_PIEN4_Pos)                   /*!< PWM_T::INTEN0: PIEN4 Mask              */

#define PWM_INTEN0_CMPUIENn_Pos          (16)                                              /*!< PWM_T::INTEN0: CMPUIENn Position       */
#define PWM_INTEN0_CMPUIENn_Msk          (0x3ful << PWM_INTEN0_CMPUIENn_Pos)               /*!< PWM_T::INTEN0: CMPUIENn Mask           */

#define PWM_INTEN0_CMPUIEN0_Pos          (16)                                              /*!< PWM_T::INTEN0: CMPUIEN0 Position       */
#define PWM_INTEN0_CMPUIEN0_Msk          (0x1ul << PWM_INTEN0_CMPUIEN0_Pos)                /*!< PWM_T::INTEN0: CMPUIEN0 Mask           */

#define PWM_INTEN0_CMPUIEN1_Pos          (17)                                              /*!< PWM_T::INTEN0: CMPUIEN1 Position       */
#define PWM_INTEN0_CMPUIEN1_Msk          (0x1ul << PWM_INTEN0_CMPUIEN1_Pos)                /*!< PWM_T::INTEN0: CMPUIEN1 Mask           */

#define PWM_INTEN0_CMPUIEN2_Pos          (18)                                              /*!< PWM_T::INTEN0: CMPUIEN2 Position       */
#define PWM_INTEN0_CMPUIEN2_Msk          (0x1ul << PWM_INTEN0_CMPUIEN2_Pos)                /*!< PWM_T::INTEN0: CMPUIEN2 Mask           */

#define PWM_INTEN0_CMPUIEN3_Pos          (19)                                              /*!< PWM_T::INTEN0: CMPUIEN3 Position       */
#define PWM_INTEN0_CMPUIEN3_Msk          (0x1ul << PWM_INTEN0_CMPUIEN3_Pos)                /*!< PWM_T::INTEN0: CMPUIEN3 Mask           */

#define PWM_INTEN0_CMPUIEN4_Pos          (20)                                              /*!< PWM_T::INTEN0: CMPUIEN4 Position       */
#define PWM_INTEN0_CMPUIEN4_Msk          (0x1ul << PWM_INTEN0_CMPUIEN4_Pos)                /*!< PWM_T::INTEN0: CMPUIEN4 Mask           */

#define PWM_INTEN0_CMPUIEN5_Pos          (21)                                              /*!< PWM_T::INTEN0: CMPUIEN5 Position       */
#define PWM_INTEN0_CMPUIEN5_Msk          (0x1ul << PWM_INTEN0_CMPUIEN5_Pos)                /*!< PWM_T::INTEN0: CMPUIEN5 Mask           */

#define PWM_INTEN0_CMPDIENn_Pos          (24)                                              /*!< PWM_T::INTEN0: CMPDIENn Position       */
#define PWM_INTEN0_CMPDIENn_Msk          (0x3ful << PWM_INTEN0_CMPDIENn_Pos)               /*!< PWM_T::INTEN0: CMPDIENn Mask           */

#define PWM_INTEN0_CMPDIEN0_Pos          (24)                                              /*!< PWM_T::INTEN0: CMPDIEN0 Position       */
#define PWM_INTEN0_CMPDIEN0_Msk          (0x1ul << PWM_INTEN0_CMPDIEN0_Pos)                /*!< PWM_T::INTEN0: CMPDIEN0 Mask           */

#define PWM_INTEN0_CMPDIEN1_Pos          (25)                                              /*!< PWM_T::INTEN0: CMPDIEN1 Position       */
#define PWM_INTEN0_CMPDIEN1_Msk          (0x1ul << PWM_INTEN0_CMPDIEN1_Pos)                /*!< PWM_T::INTEN0: CMPDIEN1 Mask           */

#define PWM_INTEN0_CMPDIEN2_Pos          (26)                                              /*!< PWM_T::INTEN0: CMPDIEN2 Position       */
#define PWM_INTEN0_CMPDIEN2_Msk          (0x1ul << PWM_INTEN0_CMPDIEN2_Pos)                /*!< PWM_T::INTEN0: CMPDIEN2 Mask           */

#define PWM_INTEN0_CMPDIEN3_Pos          (27)                                              /*!< PWM_T::INTEN0: CMPDIEN3 Position       */
#define PWM_INTEN0_CMPDIEN3_Msk          (0x1ul << PWM_INTEN0_CMPDIEN3_Pos)                /*!< PWM_T::INTEN0: CMPDIEN3 Mask           */

#define PWM_INTEN0_CMPDIEN4_Pos          (28)                                              /*!< PWM_T::INTEN0: CMPDIEN4 Position       */
#define PWM_INTEN0_CMPDIEN4_Msk          (0x1ul << PWM_INTEN0_CMPDIEN4_Pos)                /*!< PWM_T::INTEN0: CMPDIEN4 Mask           */

#define PWM_INTEN0_CMPDIEN5_Pos          (29)                                              /*!< PWM_T::INTEN0: CMPDIEN5 Position       */
#define PWM_INTEN0_CMPDIEN5_Msk          (0x1ul << PWM_INTEN0_CMPDIEN5_Pos)                /*!< PWM_T::INTEN0: CMPDIEN5 Mask           */

#define PWM_INTEN1_BRKEIEN0_1_Pos        (0)                                               /*!< PWM_T::INTEN1: BRKEIEN0_1 Position     */
#define PWM_INTEN1_BRKEIEN0_1_Msk        (0x1ul << PWM_INTEN1_BRKEIEN0_1_Pos)              /*!< PWM_T::INTEN1: BRKEIEN0_1 Mask         */

#define PWM_INTEN1_BRKEIEN2_3_Pos        (1)                                               /*!< PWM_T::INTEN1: BRKEIEN2_3 Position     */
#define PWM_INTEN1_BRKEIEN2_3_Msk        (0x1ul << PWM_INTEN1_BRKEIEN2_3_Pos)              /*!< PWM_T::INTEN1: BRKEIEN2_3 Mask         */

#define PWM_INTEN1_BRKEIEN4_5_Pos        (2)                                               /*!< PWM_T::INTEN1: BRKEIEN4_5 Position     */
#define PWM_INTEN1_BRKEIEN4_5_Msk        (0x1ul << PWM_INTEN1_BRKEIEN4_5_Pos)              /*!< PWM_T::INTEN1: BRKEIEN4_5 Mask         */

#define PWM_INTEN1_BRKLIEN0_1_Pos        (8)                                               /*!< PWM_T::INTEN1: BRKLIEN0_1 Position     */
#define PWM_INTEN1_BRKLIEN0_1_Msk        (0x1ul << PWM_INTEN1_BRKLIEN0_1_Pos)              /*!< PWM_T::INTEN1: BRKLIEN0_1 Mask         */

#define PWM_INTEN1_BRKLIEN2_3_Pos        (9)                                               /*!< PWM_T::INTEN1: BRKLIEN2_3 Position     */
#define PWM_INTEN1_BRKLIEN2_3_Msk        (0x1ul << PWM_INTEN1_BRKLIEN2_3_Pos)              /*!< PWM_T::INTEN1: BRKLIEN2_3 Mask         */

#define PWM_INTEN1_BRKLIEN4_5_Pos        (10)                                              /*!< PWM_T::INTEN1: BRKLIEN4_5 Position     */
#define PWM_INTEN1_BRKLIEN4_5_Msk        (0x1ul << PWM_INTEN1_BRKLIEN4_5_Pos)              /*!< PWM_T::INTEN1: BRKLIEN4_5 Mask         */

#define PWM_INTSTS0_ZIF0_Pos             (0)                                               /*!< PWM_T::INTSTS0: ZIF0 Position          */
#define PWM_INTSTS0_ZIF0_Msk             (0x1ul << PWM_INTSTS0_ZIF0_Pos)                   /*!< PWM_T::INTSTS0: ZIF0 Mask              */

#define PWM_INTSTS0_ZIF2_Pos             (2)                                               /*!< PWM_T::INTSTS0: ZIF2 Position          */
#define PWM_INTSTS0_ZIF2_Msk             (0x1ul << PWM_INTSTS0_ZIF2_Pos)                   /*!< PWM_T::INTSTS0: ZIF2 Mask              */

#define PWM_INTSTS0_ZIF4_Pos             (4)                                               /*!< PWM_T::INTSTS0: ZIF4 Position          */
#define PWM_INTSTS0_ZIF4_Msk             (0x1ul << PWM_INTSTS0_ZIF4_Pos)                   /*!< PWM_T::INTSTS0: ZIF4 Mask              */

#define PWM_INTSTS0_PIF0_Pos             (8)                                               /*!< PWM_T::INTSTS0: PIF0 Position          */
#define PWM_INTSTS0_PIF0_Msk             (0x1ul << PWM_INTSTS0_PIF0_Pos)                   /*!< PWM_T::INTSTS0: PIF0 Mask              */

#define PWM_INTSTS0_PIF2_Pos             (10)                                              /*!< PWM_T::INTSTS0: PIF2 Position          */
#define PWM_INTSTS0_PIF2_Msk             (0x1ul << PWM_INTSTS0_PIF2_Pos)                   /*!< PWM_T::INTSTS0: PIF2 Mask              */

#define PWM_INTSTS0_PIF4_Pos             (12)                                              /*!< PWM_T::INTSTS0: PIF4 Position          */
#define PWM_INTSTS0_PIF4_Msk             (0x1ul << PWM_INTSTS0_PIF4_Pos)                   /*!< PWM_T::INTSTS0: PIF4 Mask              */

#define PWM_INTSTS0_CMPUIFn_Pos          (16)                                              /*!< PWM_T::INTSTS0: CMPUIFn Position       */
#define PWM_INTSTS0_CMPUIFn_Msk          (0x3ful << PWM_INTSTS0_CMPUIFn_Pos)               /*!< PWM_T::INTSTS0: CMPUIFn Mask           */

#define PWM_INTSTS0_CMPUIF0_Pos          (16)                                              /*!< PWM_T::INTSTS0: CMPUIF0 Position       */
#define PWM_INTSTS0_CMPUIF0_Msk          (0x1ul << PWM_INTSTS0_CMPUIF0_Pos)               /*!< PWM_T::INTSTS0: CMPUIF0 Mask           */

#define PWM_INTSTS0_CMPUIF1_Pos          (17)                                              /*!< PWM_T::INTSTS0: CMPUIF1 Position       */
#define PWM_INTSTS0_CMPUIF1_Msk          (0x1ul << PWM_INTSTS0_CMPUIF1_Pos)               /*!< PWM_T::INTSTS0: CMPUIF1 Mask           */

#define PWM_INTSTS0_CMPUIF2_Pos          (18)                                              /*!< PWM_T::INTSTS0: CMPUIF2 Position       */
#define PWM_INTSTS0_CMPUIF2_Msk          (0x1ul << PWM_INTSTS0_CMPUIF2_Pos)               /*!< PWM_T::INTSTS0: CMPUIF2 Mask           */

#define PWM_INTSTS0_CMPUIF3_Pos          (19)                                              /*!< PWM_T::INTSTS0: CMPUIF3 Position       */
#define PWM_INTSTS0_CMPUIF3_Msk          (0x1ul << PWM_INTSTS0_CMPUIF3_Pos)               /*!< PWM_T::INTSTS0: CMPUIF3 Mask           */

#define PWM_INTSTS0_CMPUIF4_Pos          (20)                                              /*!< PWM_T::INTSTS0: CMPUIF4 Position       */
#define PWM_INTSTS0_CMPUIF4_Msk          (0x1ul << PWM_INTSTS0_CMPUIF4_Pos)               /*!< PWM_T::INTSTS0: CMPUIF4 Mask           */

#define PWM_INTSTS0_CMPUIF5_Pos          (21)                                              /*!< PWM_T::INTSTS0: CMPUIF5 Position       */
#define PWM_INTSTS0_CMPUIF5_Msk          (0x1ul << PWM_INTSTS0_CMPUIF5_Pos)               /*!< PWM_T::INTSTS0: CMPUIF5 Mask           */

#define PWM_INTSTS0_CMPDIFn_Pos          (24)                                              /*!< PWM_T::INTSTS0: CMPDIFn Position       */
#define PWM_INTSTS0_CMPDIFn_Msk          (0x3ful << PWM_INTSTS0_CMPDIFn_Pos)               /*!< PWM_T::INTSTS0: CMPDIFn Mask           */

#define PWM_INTSTS0_CMPDIF0_Pos          (24)                                              /*!< PWM_T::INTSTS0: CMPDIF0 Position       */
#define PWM_INTSTS0_CMPDIF0_Msk          (0x1ul << PWM_INTSTS0_CMPDIF0_Pos)                /*!< PWM_T::INTSTS0: CMPDIF0 Mask           */

#define PWM_INTSTS0_CMPDIF1_Pos          (25)                                              /*!< PWM_T::INTSTS0: CMPDIF1 Position       */
#define PWM_INTSTS0_CMPDIF1_Msk          (0x1ul << PWM_INTSTS0_CMPDIF1_Pos)                /*!< PWM_T::INTSTS0: CMPDIF1 Mask           */

#define PWM_INTSTS0_CMPDIF2_Pos          (26)                                              /*!< PWM_T::INTSTS0: CMPDIF2 Position       */
#define PWM_INTSTS0_CMPDIF2_Msk          (0x1ul << PWM_INTSTS0_CMPDIF2_Pos)                /*!< PWM_T::INTSTS0: CMPDIF2 Mask           */

#define PWM_INTSTS0_CMPDIF3_Pos          (27)                                              /*!< PWM_T::INTSTS0: CMPDIF3 Position       */
#define PWM_INTSTS0_CMPDIF3_Msk          (0x1ul << PWM_INTSTS0_CMPDIF3_Pos)                /*!< PWM_T::INTSTS0: CMPDIF3 Mask           */

#define PWM_INTSTS0_CMPDIF4_Pos          (28)                                              /*!< PWM_T::INTSTS0: CMPDIF4 Position       */
#define PWM_INTSTS0_CMPDIF4_Msk          (0x1ul << PWM_INTSTS0_CMPDIF4_Pos)                /*!< PWM_T::INTSTS0: CMPDIF4 Mask           */

#define PWM_INTSTS0_CMPDIF5_Pos          (29)                                              /*!< PWM_T::INTSTS0: CMPDIF5 Position       */
#define PWM_INTSTS0_CMPDIF5_Msk          (0x1ul << PWM_INTSTS0_CMPDIF5_Pos)                /*!< PWM_T::INTSTS0: CMPDIF5 Mask           */

#define PWM_INTSTS1_BRKEIF0_Pos          (0)                                               /*!< PWM_T::INTSTS1: BRKEIF0 Position       */
#define PWM_INTSTS1_BRKEIF0_Msk          (0x1ul << PWM_INTSTS1_BRKEIF0_Pos)                /*!< PWM_T::INTSTS1: BRKEIF0 Mask           */

#define PWM_INTSTS1_BRKEIF1_Pos          (1)                                               /*!< PWM_T::INTSTS1: BRKEIF1 Position       */
#define PWM_INTSTS1_BRKEIF1_Msk          (0x1ul << PWM_INTSTS1_BRKEIF1_Pos)                /*!< PWM_T::INTSTS1: BRKEIF1 Mask           */

#define PWM_INTSTS1_BRKEIF2_Pos          (2)                                               /*!< PWM_T::INTSTS1: BRKEIF2 Position       */
#define PWM_INTSTS1_BRKEIF2_Msk          (0x1ul << PWM_INTSTS1_BRKEIF2_Pos)                /*!< PWM_T::INTSTS1: BRKEIF2 Mask           */

#define PWM_INTSTS1_BRKEIF3_Pos          (3)                                               /*!< PWM_T::INTSTS1: BRKEIF3 Position       */
#define PWM_INTSTS1_BRKEIF3_Msk          (0x1ul << PWM_INTSTS1_BRKEIF3_Pos)                /*!< PWM_T::INTSTS1: BRKEIF3 Mask           */

#define PWM_INTSTS1_BRKEIF4_Pos          (4)                                               /*!< PWM_T::INTSTS1: BRKEIF4 Position       */
#define PWM_INTSTS1_BRKEIF4_Msk          (0x1ul << PWM_INTSTS1_BRKEIF4_Pos)                /*!< PWM_T::INTSTS1: BRKEIF4 Mask           */

#define PWM_INTSTS1_BRKEIF5_Pos          (5)                                               /*!< PWM_T::INTSTS1: BRKEIF5 Position       */
#define PWM_INTSTS1_BRKEIF5_Msk          (0x1ul << PWM_INTSTS1_BRKEIF5_Pos)                /*!< PWM_T::INTSTS1: BRKEIF5 Mask           */

#define PWM_INTSTS1_BRKLIF0_Pos          (8)                                               /*!< PWM_T::INTSTS1: BRKLIF0 Position       */
#define PWM_INTSTS1_BRKLIF0_Msk          (0x1ul << PWM_INTSTS1_BRKLIF0_Pos)                /*!< PWM_T::INTSTS1: BRKLIF0 Mask           */

#define PWM_INTSTS1_BRKLIF1_Pos          (9)                                               /*!< PWM_T::INTSTS1: BRKLIF1 Position       */
#define PWM_INTSTS1_BRKLIF1_Msk          (0x1ul << PWM_INTSTS1_BRKLIF1_Pos)                /*!< PWM_T::INTSTS1: BRKLIF1 Mask           */

#define PWM_INTSTS1_BRKLIF2_Pos          (10)                                              /*!< PWM_T::INTSTS1: BRKLIF2 Position       */
#define PWM_INTSTS1_BRKLIF2_Msk          (0x1ul << PWM_INTSTS1_BRKLIF2_Pos)                /*!< PWM_T::INTSTS1: BRKLIF2 Mask           */

#define PWM_INTSTS1_BRKLIF3_Pos          (11)                                              /*!< PWM_T::INTSTS1: BRKLIF3 Position       */
#define PWM_INTSTS1_BRKLIF3_Msk          (0x1ul << PWM_INTSTS1_BRKLIF3_Pos)                /*!< PWM_T::INTSTS1: BRKLIF3 Mask           */

#define PWM_INTSTS1_BRKLIF4_Pos          (12)                                              /*!< PWM_T::INTSTS1: BRKLIF4 Position       */
#define PWM_INTSTS1_BRKLIF4_Msk          (0x1ul << PWM_INTSTS1_BRKLIF4_Pos)                /*!< PWM_T::INTSTS1: BRKLIF4 Mask           */

#define PWM_INTSTS1_BRKLIF5_Pos          (13)                                              /*!< PWM_T::INTSTS1: BRKLIF5 Position       */
#define PWM_INTSTS1_BRKLIF5_Msk          (0x1ul << PWM_INTSTS1_BRKLIF5_Pos)                /*!< PWM_T::INTSTS1: BRKLIF5 Mask           */

#define PWM_INTSTS1_BRKESTS0_Pos         (16)                                              /*!< PWM_T::INTSTS1: BRKESTS0 Position      */
#define PWM_INTSTS1_BRKESTS0_Msk         (0x1ul << PWM_INTSTS1_BRKESTS0_Pos)               /*!< PWM_T::INTSTS1: BRKESTS0 Mask          */

#define PWM_INTSTS1_BRKESTS1_Pos         (17)                                              /*!< PWM_T::INTSTS1: BRKESTS1 Position      */
#define PWM_INTSTS1_BRKESTS1_Msk         (0x1ul << PWM_INTSTS1_BRKESTS1_Pos)               /*!< PWM_T::INTSTS1: BRKESTS1 Mask          */

#define PWM_INTSTS1_BRKESTS2_Pos         (18)                                              /*!< PWM_T::INTSTS1: BRKESTS2 Position      */
#define PWM_INTSTS1_BRKESTS2_Msk         (0x1ul << PWM_INTSTS1_BRKESTS2_Pos)               /*!< PWM_T::INTSTS1: BRKESTS2 Mask          */

#define PWM_INTSTS1_BRKESTS3_Pos         (19)                                              /*!< PWM_T::INTSTS1: BRKESTS3 Position      */
#define PWM_INTSTS1_BRKESTS3_Msk         (0x1ul << PWM_INTSTS1_BRKESTS3_Pos)               /*!< PWM_T::INTSTS1: BRKESTS3 Mask          */

#define PWM_INTSTS1_BRKESTS4_Pos         (20)                                              /*!< PWM_T::INTSTS1: BRKESTS4 Position      */
#define PWM_INTSTS1_BRKESTS4_Msk         (0x1ul << PWM_INTSTS1_BRKESTS4_Pos)               /*!< PWM_T::INTSTS1: BRKESTS4 Mask          */

#define PWM_INTSTS1_BRKESTS5_Pos         (21)                                              /*!< PWM_T::INTSTS1: BRKESTS5 Position      */
#define PWM_INTSTS1_BRKESTS5_Msk         (0x1ul << PWM_INTSTS1_BRKESTS5_Pos)               /*!< PWM_T::INTSTS1: BRKESTS5 Mask          */

#define PWM_INTSTS1_BRKLSTS0_Pos         (24)                                              /*!< PWM_T::INTSTS1: BRKLSTS0 Position      */
#define PWM_INTSTS1_BRKLSTS0_Msk         (0x1ul << PWM_INTSTS1_BRKLSTS0_Pos)               /*!< PWM_T::INTSTS1: BRKLSTS0 Mask          */

#define PWM_INTSTS1_BRKLSTS1_Pos         (25)                                              /*!< PWM_T::INTSTS1: BRKLSTS1 Position      */
#define PWM_INTSTS1_BRKLSTS1_Msk         (0x1ul << PWM_INTSTS1_BRKLSTS1_Pos)               /*!< PWM_T::INTSTS1: BRKLSTS1 Mask          */

#define PWM_INTSTS1_BRKLSTS2_Pos         (26)                                              /*!< PWM_T::INTSTS1: BRKLSTS2 Position      */
#define PWM_INTSTS1_BRKLSTS2_Msk         (0x1ul << PWM_INTSTS1_BRKLSTS2_Pos)               /*!< PWM_T::INTSTS1: BRKLSTS2 Mask          */

#define PWM_INTSTS1_BRKLSTS3_Pos         (27)                                              /*!< PWM_T::INTSTS1: BRKLSTS3 Position      */
#define PWM_INTSTS1_BRKLSTS3_Msk         (0x1ul << PWM_INTSTS1_BRKLSTS3_Pos)               /*!< PWM_T::INTSTS1: BRKLSTS3 Mask          */

#define PWM_INTSTS1_BRKLSTS4_Pos         (28)                                              /*!< PWM_T::INTSTS1: BRKLSTS4 Position      */
#define PWM_INTSTS1_BRKLSTS4_Msk         (0x1ul << PWM_INTSTS1_BRKLSTS4_Pos)               /*!< PWM_T::INTSTS1: BRKLSTS4 Mask          */

#define PWM_INTSTS1_BRKLSTS5_Pos         (29)                                              /*!< PWM_T::INTSTS1: BRKLSTS5 Position      */
#define PWM_INTSTS1_BRKLSTS5_Msk         (0x1ul << PWM_INTSTS1_BRKLSTS5_Pos)               /*!< PWM_T::INTSTS1: BRKLSTS5 Mask          */

#define PWM_EADCTS0_TRGSEL0_Pos          (0)                                               /*!< PWM_T::EADCTS0: TRGSEL0 Position       */
#define PWM_EADCTS0_TRGSEL0_Msk          (0xful << PWM_EADCTS0_TRGSEL0_Pos)                /*!< PWM_T::EADCTS0: TRGSEL0 Mask           */

#define PWM_EADCTS0_TRGEN0_Pos           (7)                                               /*!< PWM_T::EADCTS0: TRGEN0 Position        */
#define PWM_EADCTS0_TRGEN0_Msk           (0x1ul << PWM_EADCTS0_TRGEN0_Pos)                 /*!< PWM_T::EADCTS0: TRGEN0 Mask            */

#define PWM_EADCTS0_TRGSEL1_Pos          (8)                                               /*!< PWM_T::EADCTS0: TRGSEL1 Position       */
#define PWM_EADCTS0_TRGSEL1_Msk          (0xful << PWM_EADCTS0_TRGSEL1_Pos)                /*!< PWM_T::EADCTS0: TRGSEL1 Mask           */

#define PWM_EADCTS0_TRGEN1_Pos           (15)                                              /*!< PWM_T::EADCTS0: TRGEN1 Position        */
#define PWM_EADCTS0_TRGEN1_Msk           (0x1ul << PWM_EADCTS0_TRGEN1_Pos)                 /*!< PWM_T::EADCTS0: TRGEN1 Mask            */

#define PWM_EADCTS0_TRGSEL2_Pos          (16)                                              /*!< PWM_T::EADCTS0: TRGSEL2 Position       */
#define PWM_EADCTS0_TRGSEL2_Msk          (0xful << PWM_EADCTS0_TRGSEL2_Pos)                /*!< PWM_T::EADCTS0: TRGSEL2 Mask           */

#define PWM_EADCTS0_TRGEN2_Pos           (23)                                              /*!< PWM_T::EADCTS0: TRGEN2 Position        */
#define PWM_EADCTS0_TRGEN2_Msk           (0x1ul << PWM_EADCTS0_TRGEN2_Pos)                 /*!< PWM_T::EADCTS0: TRGEN2 Mask            */

#define PWM_EADCTS0_TRGSEL3_Pos          (24)                                              /*!< PWM_T::EADCTS0: TRGSEL3 Position       */
#define PWM_EADCTS0_TRGSEL3_Msk          (0xful << PWM_EADCTS0_TRGSEL3_Pos)                /*!< PWM_T::EADCTS0: TRGSEL3 Mask           */

#define PWM_EADCTS0_TRGEN3_Pos           (31)                                              /*!< PWM_T::EADCTS0: TRGEN3 Position        */
#define PWM_EADCTS0_TRGEN3_Msk           (0x1ul << PWM_EADCTS0_TRGEN3_Pos)                 /*!< PWM_T::EADCTS0: TRGEN3 Mask            */

#define PWM_EADCTS1_TRGSEL4_Pos          (0)                                               /*!< PWM_T::EADCTS1: TRGSEL4 Position       */
#define PWM_EADCTS1_TRGSEL4_Msk          (0xful << PWM_EADCTS1_TRGSEL4_Pos)                /*!< PWM_T::EADCTS1: TRGSEL4 Mask           */

#define PWM_EADCTS1_TRGEN4_Pos           (7)                                               /*!< PWM_T::EADCTS1: TRGEN4 Position        */
#define PWM_EADCTS1_TRGEN4_Msk           (0x1ul << PWM_EADCTS1_TRGEN4_Pos)                 /*!< PWM_T::EADCTS1: TRGEN4 Mask            */

#define PWM_EADCTS1_TRGSEL5_Pos          (8)                                               /*!< PWM_T::EADCTS1: TRGSEL5 Position       */
#define PWM_EADCTS1_TRGSEL5_Msk          (0xful << PWM_EADCTS1_TRGSEL5_Pos)                /*!< PWM_T::EADCTS1: TRGSEL5 Mask           */

#define PWM_EADCTS1_TRGEN5_Pos           (15)                                              /*!< PWM_T::EADCTS1: TRGEN5 Position        */
#define PWM_EADCTS1_TRGEN5_Msk           (0x1ul << PWM_EADCTS1_TRGEN5_Pos)                 /*!< PWM_T::EADCTS1: TRGEN5 Mask            */

#define PWM_SSCTL_SSEN0_Pos              (0)                                               /*!< PWM_T::SSCTL: SSEN0 Position           */
#define PWM_SSCTL_SSEN0_Msk              (0x1ul << PWM_SSCTL_SSEN0_Pos)                    /*!< PWM_T::SSCTL: SSEN0 Mask               */

#define PWM_SSCTL_SSEN2_Pos              (2)                                               /*!< PWM_T::SSCTL: SSEN2 Position           */
#define PWM_SSCTL_SSEN2_Msk              (0x1ul << PWM_SSCTL_SSEN2_Pos)                    /*!< PWM_T::SSCTL: SSEN2 Mask               */

#define PWM_SSCTL_SSEN4_Pos              (4)                                               /*!< PWM_T::SSCTL: SSEN4 Position           */
#define PWM_SSCTL_SSEN4_Msk              (0x1ul << PWM_SSCTL_SSEN4_Pos)                    /*!< PWM_T::SSCTL: SSEN4 Mask               */

#define PWM_SSCTL_SSRC_Pos               (8)                                               /*!< PWM_T::SSCTL: SSRC Position            */
#define PWM_SSCTL_SSRC_Msk               (0x3ul << PWM_SSCTL_SSRC_Pos)                     /*!< PWM_T::SSCTL: SSRC Mask                */

#define PWM_SSTRG_CNTSEN_Pos             (0)                                               /*!< PWM_T::SSTRG: CNTSEN Position          */
#define PWM_SSTRG_CNTSEN_Msk             (0x1ul << PWM_SSTRG_CNTSEN_Pos)                   /*!< PWM_T::SSTRG: CNTSEN Mask              */

#define PWM_STATUS_CNTMAX0_Pos           (0)                                               /*!< PWM_T::STATUS: CNTMAX0 Position        */
#define PWM_STATUS_CNTMAX0_Msk           (0x1ul << PWM_STATUS_CNTMAX0_Pos)                 /*!< PWM_T::STATUS: CNTMAX0 Mask            */

#define PWM_STATUS_CNTMAX2_Pos           (2)                                               /*!< PWM_T::STATUS: CNTMAX2 Position        */
#define PWM_STATUS_CNTMAX2_Msk           (0x1ul << PWM_STATUS_CNTMAX2_Pos)                 /*!< PWM_T::STATUS: CNTMAX2 Mask            */

#define PWM_STATUS_CNTMAX4_Pos           (4)                                               /*!< PWM_T::STATUS: CNTMAX4 Position        */
#define PWM_STATUS_CNTMAX4_Msk           (0x1ul << PWM_STATUS_CNTMAX4_Pos)                 /*!< PWM_T::STATUS: CNTMAX4 Mask            */

#define PWM_STATUS_EADCTRGn_Pos          (16)                                              /*!< PWM_T::STATUS: EADCTRGn Position       */
#define PWM_STATUS_EADCTRGn_Msk          (0x3ful << PWM_STATUS_EADCTRGn_Pos)               /*!< PWM_T::STATUS: EADCTRGn Mask           */

#define PWM_STATUS_EADCTRG0_Pos          (16)                                              /*!< PWM_T::STATUS: EADCTRG0 Position       */
#define PWM_STATUS_EADCTRG0_Msk          (0x1ul << PWM_STATUS_EADCTRG0_Pos)                /*!< PWM_T::STATUS: EADCTRG0 Mask           */

#define PWM_STATUS_EADCTRG1_Pos          (17)                                              /*!< PWM_T::STATUS: EADCTRG1 Position       */
#define PWM_STATUS_EADCTRG1_Msk          (0x1ul << PWM_STATUS_EADCTRG1_Pos)                /*!< PWM_T::STATUS: EADCTRG1 Mask           */

#define PWM_STATUS_EADCTRG2_Pos          (18)                                              /*!< PWM_T::STATUS: EADCTRG2 Position       */
#define PWM_STATUS_EADCTRG2_Msk          (0x1ul << PWM_STATUS_EADCTRG2_Pos)                /*!< PWM_T::STATUS: EADCTRG2 Mask           */

#define PWM_STATUS_EADCTRG3_Pos          (19)                                              /*!< PWM_T::STATUS: EADCTRG3 Position       */
#define PWM_STATUS_EADCTRG3_Msk          (0x1ul << PWM_STATUS_EADCTRG3_Pos)                /*!< PWM_T::STATUS: EADCTRG3 Mask           */

#define PWM_STATUS_EADCTRG4_Pos          (20)                                              /*!< PWM_T::STATUS: EADCTRG4 Position       */
#define PWM_STATUS_EADCTRG4_Msk          (0x1ul << PWM_STATUS_EADCTRG4_Pos)                /*!< PWM_T::STATUS: EADCTRG4 Mask           */

#define PWM_STATUS_EADCTRG5_Pos          (21)                                              /*!< PWM_T::STATUS: EADCTRG5 Position       */
#define PWM_STATUS_EADCTRG5_Msk          (0x1ul << PWM_STATUS_EADCTRG5_Pos)                /*!< PWM_T::STATUS: EADCTRG5 Mask           */

#define PWM_CAPINEN_CAPINENn_Pos         (0)                                               /*!< PWM_T::CAPINEN: CAPINENn Position      */
#define PWM_CAPINEN_CAPINENn_Msk         (0x3ful << PWM_CAPINEN_CAPINENn_Pos)              /*!< PWM_T::CAPINEN: CAPINENn Mask          */

#define PWM_CAPINEN_CAPINEN0_Pos         (0)                                               /*!< PWM_T::CAPINEN: CAPINEN0 Position      */
#define PWM_CAPINEN_CAPINEN0_Msk         (0x1ul << PWM_CAPINEN_CAPINEN0_Pos)               /*!< PWM_T::CAPINEN: CAPINEN0 Mask          */

#define PWM_CAPINEN_CAPINEN1_Pos         (1)                                               /*!< PWM_T::CAPINEN: CAPINEN1 Position      */
#define PWM_CAPINEN_CAPINEN1_Msk         (0x1ul << PWM_CAPINEN_CAPINEN1_Pos)               /*!< PWM_T::CAPINEN: CAPINEN1 Mask          */

#define PWM_CAPINEN_CAPINEN2_Pos         (2)                                               /*!< PWM_T::CAPINEN: CAPINEN2 Position      */
#define PWM_CAPINEN_CAPINEN2_Msk         (0x1ul << PWM_CAPINEN_CAPINEN2_Pos)               /*!< PWM_T::CAPINEN: CAPINEN2 Mask          */

#define PWM_CAPINEN_CAPINEN3_Pos         (3)                                               /*!< PWM_T::CAPINEN: CAPINEN3 Position      */
#define PWM_CAPINEN_CAPINEN3_Msk         (0x1ul << PWM_CAPINEN_CAPINEN3_Pos)               /*!< PWM_T::CAPINEN: CAPINEN3 Mask          */

#define PWM_CAPINEN_CAPINEN4_Pos         (4)                                               /*!< PWM_T::CAPINEN: CAPINEN4 Position      */
#define PWM_CAPINEN_CAPINEN4_Msk         (0x1ul << PWM_CAPINEN_CAPINEN4_Pos)               /*!< PWM_T::CAPINEN: CAPINEN4 Mask          */

#define PWM_CAPINEN_CAPINEN5_Pos         (5)                                               /*!< PWM_T::CAPINEN: CAPINEN5 Position      */
#define PWM_CAPINEN_CAPINEN5_Msk         (0x1ul << PWM_CAPINEN_CAPINEN5_Pos)               /*!< PWM_T::CAPINEN: CAPINEN5 Mask          */

#define PWM_CAPCTL_CAPENn_Pos            (0)                                               /*!< PWM_T::CAPCTL: CAPENn Position         */
#define PWM_CAPCTL_CAPENn_Msk            (0x3ful << PWM_CAPCTL_CAPENn_Pos)                 /*!< PWM_T::CAPCTL: CAPENn Mask             */

#define PWM_CAPCTL_CAPEN0_Pos            (0)                                               /*!< PWM_T::CAPCTL: CAPEN0 Position         */
#define PWM_CAPCTL_CAPEN0_Msk            (0x1ul << PWM_CAPCTL_CAPEN0_Pos)                  /*!< PWM_T::CAPCTL: CAPEN0 Mask             */

#define PWM_CAPCTL_CAPEN1_Pos            (1)                                               /*!< PWM_T::CAPCTL: CAPEN1 Position         */
#define PWM_CAPCTL_CAPEN1_Msk            (0x1ul << PWM_CAPCTL_CAPEN1_Pos)                  /*!< PWM_T::CAPCTL: CAPEN1 Mask             */

#define PWM_CAPCTL_CAPEN2_Pos            (2)                                               /*!< PWM_T::CAPCTL: CAPEN2 Position         */
#define PWM_CAPCTL_CAPEN2_Msk            (0x1ul << PWM_CAPCTL_CAPEN2_Pos)                  /*!< PWM_T::CAPCTL: CAPEN2 Mask             */

#define PWM_CAPCTL_CAPEN3_Pos            (3)                                               /*!< PWM_T::CAPCTL: CAPEN3 Position         */
#define PWM_CAPCTL_CAPEN3_Msk            (0x1ul << PWM_CAPCTL_CAPEN3_Pos)                  /*!< PWM_T::CAPCTL: CAPEN3 Mask             */

#define PWM_CAPCTL_CAPEN4_Pos            (4)                                               /*!< PWM_T::CAPCTL: CAPEN4 Position         */
#define PWM_CAPCTL_CAPEN4_Msk            (0x1ul << PWM_CAPCTL_CAPEN4_Pos)                  /*!< PWM_T::CAPCTL: CAPEN4 Mask             */

#define PWM_CAPCTL_CAPEN5_Pos            (5)                                               /*!< PWM_T::CAPCTL: CAPEN5 Position         */
#define PWM_CAPCTL_CAPEN5_Msk            (0x1ul << PWM_CAPCTL_CAPEN5_Pos)                  /*!< PWM_T::CAPCTL: CAPEN5 Mask             */

#define PWM_CAPCTL_CAPINVn_Pos           (8)                                               /*!< PWM_T::CAPCTL: CAPINVn Position        */
#define PWM_CAPCTL_CAPINVn_Msk           (0x3ful << PWM_CAPCTL_CAPINVn_Pos)                /*!< PWM_T::CAPCTL: CAPINVn Mask            */

#define PWM_CAPCTL_CAPINV0_Pos           (8)                                               /*!< PWM_T::CAPCTL: CAPINV0 Position        */
#define PWM_CAPCTL_CAPINV0_Msk           (0x1ul << PWM_CAPCTL_CAPINV0_Pos)                 /*!< PWM_T::CAPCTL: CAPINV0 Mask            */

#define PWM_CAPCTL_CAPINV1_Pos           (9)                                               /*!< PWM_T::CAPCTL: CAPINV1 Position        */
#define PWM_CAPCTL_CAPINV1_Msk           (0x1ul << PWM_CAPCTL_CAPINV1_Pos)                 /*!< PWM_T::CAPCTL: CAPINV1 Mask            */

#define PWM_CAPCTL_CAPINV2_Pos           (10)                                              /*!< PWM_T::CAPCTL: CAPINV2 Position        */
#define PWM_CAPCTL_CAPINV2_Msk           (0x1ul << PWM_CAPCTL_CAPINV2_Pos)                 /*!< PWM_T::CAPCTL: CAPINV2 Mask            */

#define PWM_CAPCTL_CAPINV3_Pos           (11)                                              /*!< PWM_T::CAPCTL: CAPINV3 Position        */
#define PWM_CAPCTL_CAPINV3_Msk           (0x1ul << PWM_CAPCTL_CAPINV3_Pos)                 /*!< PWM_T::CAPCTL: CAPINV3 Mask            */

#define PWM_CAPCTL_CAPINV4_Pos           (12)                                              /*!< PWM_T::CAPCTL: CAPINV4 Position        */
#define PWM_CAPCTL_CAPINV4_Msk           (0x1ul << PWM_CAPCTL_CAPINV4_Pos)                 /*!< PWM_T::CAPCTL: CAPINV4 Mask            */

#define PWM_CAPCTL_CAPINV5_Pos           (13)                                              /*!< PWM_T::CAPCTL: CAPINV5 Position        */
#define PWM_CAPCTL_CAPINV5_Msk           (0x1ul << PWM_CAPCTL_CAPINV5_Pos)                 /*!< PWM_T::CAPCTL: CAPINV5 Mask            */

#define PWM_CAPCTL_RCRLDENn_Pos          (16)                                              /*!< PWM_T::CAPCTL: RCRLDENn Position       */
#define PWM_CAPCTL_RCRLDENn_Msk          (0x3ful << PWM_CAPCTL_RCRLDENn_Pos)               /*!< PWM_T::CAPCTL: RCRLDENn Mask           */

#define PWM_CAPCTL_RCRLDEN0_Pos          (16)                                              /*!< PWM_T::CAPCTL: RCRLDEN0 Position       */
#define PWM_CAPCTL_RCRLDEN0_Msk          (0x1ul << PWM_CAPCTL_RCRLDEN0_Pos)                /*!< PWM_T::CAPCTL: RCRLDEN0 Mask           */

#define PWM_CAPCTL_RCRLDEN1_Pos          (17)                                              /*!< PWM_T::CAPCTL: RCRLDEN1 Position       */
#define PWM_CAPCTL_RCRLDEN1_Msk          (0x1ul << PWM_CAPCTL_RCRLDEN1_Pos)                /*!< PWM_T::CAPCTL: RCRLDEN1 Mask           */

#define PWM_CAPCTL_RCRLDEN2_Pos          (18)                                              /*!< PWM_T::CAPCTL: RCRLDEN2 Position       */
#define PWM_CAPCTL_RCRLDEN2_Msk          (0x1ul << PWM_CAPCTL_RCRLDEN2_Pos)                /*!< PWM_T::CAPCTL: RCRLDEN2 Mask           */

#define PWM_CAPCTL_RCRLDEN3_Pos          (19)                                              /*!< PWM_T::CAPCTL: RCRLDEN3 Position       */
#define PWM_CAPCTL_RCRLDEN3_Msk          (0x1ul << PWM_CAPCTL_RCRLDEN3_Pos)                /*!< PWM_T::CAPCTL: RCRLDEN3 Mask           */

#define PWM_CAPCTL_RCRLDEN4_Pos          (20)                                              /*!< PWM_T::CAPCTL: RCRLDEN4 Position       */
#define PWM_CAPCTL_RCRLDEN4_Msk          (0x1ul << PWM_CAPCTL_RCRLDEN4_Pos)                /*!< PWM_T::CAPCTL: RCRLDEN4 Mask           */

#define PWM_CAPCTL_RCRLDEN5_Pos          (21)                                              /*!< PWM_T::CAPCTL: RCRLDEN5 Position       */
#define PWM_CAPCTL_RCRLDEN5_Msk          (0x1ul << PWM_CAPCTL_RCRLDEN5_Pos)                /*!< PWM_T::CAPCTL: RCRLDEN5 Mask           */

#define PWM_CAPCTL_FCRLDENn_Pos          (24)                                              /*!< PWM_T::CAPCTL: FCRLDENn Position       */
#define PWM_CAPCTL_FCRLDENn_Msk          (0x3ful << PWM_CAPCTL_FCRLDENn_Pos)               /*!< PWM_T::CAPCTL: FCRLDENn Mask           */

#define PWM_CAPCTL_FCRLDEN0_Pos          (24)                                              /*!< PWM_T::CAPCTL: FCRLDEN0 Position       */
#define PWM_CAPCTL_FCRLDEN0_Msk          (0x1ul << PWM_CAPCTL_FCRLDEN0_Pos)                /*!< PWM_T::CAPCTL: FCRLDEN0 Mask           */

#define PWM_CAPCTL_FCRLDEN1_Pos          (25)                                              /*!< PWM_T::CAPCTL: FCRLDEN1 Position       */
#define PWM_CAPCTL_FCRLDEN1_Msk          (0x1ul << PWM_CAPCTL_FCRLDEN1_Pos)                /*!< PWM_T::CAPCTL: FCRLDEN1 Mask           */

#define PWM_CAPCTL_FCRLDEN2_Pos          (26)                                              /*!< PWM_T::CAPCTL: FCRLDEN2 Position       */
#define PWM_CAPCTL_FCRLDEN2_Msk          (0x1ul << PWM_CAPCTL_FCRLDEN2_Pos)                /*!< PWM_T::CAPCTL: FCRLDEN2 Mask           */

#define PWM_CAPCTL_FCRLDEN3_Pos          (27)                                              /*!< PWM_T::CAPCTL: FCRLDEN3 Position       */
#define PWM_CAPCTL_FCRLDEN3_Msk          (0x1ul << PWM_CAPCTL_FCRLDEN3_Pos)                /*!< PWM_T::CAPCTL: FCRLDEN3 Mask           */

#define PWM_CAPCTL_FCRLDEN4_Pos          (28)                                              /*!< PWM_T::CAPCTL: FCRLDEN4 Position       */
#define PWM_CAPCTL_FCRLDEN4_Msk          (0x1ul << PWM_CAPCTL_FCRLDEN4_Pos)                /*!< PWM_T::CAPCTL: FCRLDEN4 Mask           */

#define PWM_CAPCTL_FCRLDEN5_Pos          (29)                                              /*!< PWM_T::CAPCTL: FCRLDEN5 Position       */
#define PWM_CAPCTL_FCRLDEN5_Msk          (0x1ul << PWM_CAPCTL_FCRLDEN5_Pos)                /*!< PWM_T::CAPCTL: FCRLDEN5 Mask           */

#define PWM_CAPSTS_CRLIFOVn_Pos          (0)                                               /*!< PWM_T::CAPSTS: CRLIFOVn Position       */
#define PWM_CAPSTS_CRLIFOVn_Msk          (0x3ful << PWM_CAPSTS_CRLIFOVn_Pos)               /*!< PWM_T::CAPSTS: CRLIFOVn Mask           */

#define PWM_CAPSTS_CRLIFOV0_Pos          (0)                                               /*!< PWM_T::CAPSTS: CRLIFOV0 Position       */
#define PWM_CAPSTS_CRLIFOV0_Msk          (0x1ul << PWM_CAPSTS_CRLIFOV0_Pos)                /*!< PWM_T::CAPSTS: CRLIFOV0 Mask           */

#define PWM_CAPSTS_CRLIFOV1_Pos          (1)                                               /*!< PWM_T::CAPSTS: CRLIFOV1 Position       */
#define PWM_CAPSTS_CRLIFOV1_Msk          (0x1ul << PWM_CAPSTS_CRLIFOV1_Pos)                /*!< PWM_T::CAPSTS: CRLIFOV1 Mask           */

#define PWM_CAPSTS_CRLIFOV2_Pos          (2)                                               /*!< PWM_T::CAPSTS: CRLIFOV2 Position       */
#define PWM_CAPSTS_CRLIFOV2_Msk          (0x1ul << PWM_CAPSTS_CRLIFOV2_Pos)                /*!< PWM_T::CAPSTS: CRLIFOV2 Mask           */

#define PWM_CAPSTS_CRLIFOV3_Pos          (3)                                               /*!< PWM_T::CAPSTS: CRLIFOV3 Position       */
#define PWM_CAPSTS_CRLIFOV3_Msk          (0x1ul << PWM_CAPSTS_CRLIFOV3_Pos)                /*!< PWM_T::CAPSTS: CRLIFOV3 Mask           */

#define PWM_CAPSTS_CRLIFOV4_Pos          (4)                                               /*!< PWM_T::CAPSTS: CRLIFOV4 Position       */
#define PWM_CAPSTS_CRLIFOV4_Msk          (0x1ul << PWM_CAPSTS_CRLIFOV4_Pos)                /*!< PWM_T::CAPSTS: CRLIFOV4 Mask           */

#define PWM_CAPSTS_CRLIFOV5_Pos          (5)                                               /*!< PWM_T::CAPSTS: CRLIFOV5 Position       */
#define PWM_CAPSTS_CRLIFOV5_Msk          (0x1ul << PWM_CAPSTS_CRLIFOV5_Pos)                /*!< PWM_T::CAPSTS: CRLIFOV5 Mask           */

#define PWM_CAPSTS_CFLIFOVn_Pos          (8)                                               /*!< PWM_T::CAPSTS: CFLIFOVn Position       */
#define PWM_CAPSTS_CFLIFOVn_Msk          (0x3ful << PWM_CAPSTS_CFLIFOVn_Pos)               /*!< PWM_T::CAPSTS: CFLIFOVn Mask           */

#define PWM_CAPSTS_CFLIFOV0_Pos          (8)                                               /*!< PWM_T::CAPSTS: CFLIFOV0 Position       */
#define PWM_CAPSTS_CFLIFOV0_Msk          (0x1ul << PWM_CAPSTS_CFLIFOV0_Pos)                /*!< PWM_T::CAPSTS: CFLIFOV0 Mask           */

#define PWM_CAPSTS_CFLIFOV1_Pos          (9)                                               /*!< PWM_T::CAPSTS: CFLIFOV1 Position       */
#define PWM_CAPSTS_CFLIFOV1_Msk          (0x1ul << PWM_CAPSTS_CFLIFOV1_Pos)                /*!< PWM_T::CAPSTS: CFLIFOV1 Mask           */

#define PWM_CAPSTS_CFLIFOV2_Pos          (10)                                              /*!< PWM_T::CAPSTS: CFLIFOV2 Position       */
#define PWM_CAPSTS_CFLIFOV2_Msk          (0x1ul << PWM_CAPSTS_CFLIFOV2_Pos)                /*!< PWM_T::CAPSTS: CFLIFOV2 Mask           */

#define PWM_CAPSTS_CFLIFOV3_Pos          (11)                                              /*!< PWM_T::CAPSTS: CFLIFOV3 Position       */
#define PWM_CAPSTS_CFLIFOV3_Msk          (0x1ul << PWM_CAPSTS_CFLIFOV3_Pos)                /*!< PWM_T::CAPSTS: CFLIFOV3 Mask           */

#define PWM_CAPSTS_CFLIFOV4_Pos          (12)                                              /*!< PWM_T::CAPSTS: CFLIFOV4 Position       */
#define PWM_CAPSTS_CFLIFOV4_Msk          (0x1ul << PWM_CAPSTS_CFLIFOV4_Pos)                /*!< PWM_T::CAPSTS: CFLIFOV4 Mask           */

#define PWM_CAPSTS_CFLIFOV5_Pos          (13)                                              /*!< PWM_T::CAPSTS: CFLIFOV5 Position       */
#define PWM_CAPSTS_CFLIFOV5_Msk          (0x1ul << PWM_CAPSTS_CFLIFOV5_Pos)                /*!< PWM_T::CAPSTS: CFLIFOV5 Mask           */

#define PWM_RCAPDAT_RCAPDAT_Pos          (0)                                               /*!< PWM_T::RCAPDAT0: RCAPDAT Position      */
#define PWM_RCAPDAT_RCAPDAT_Msk          (0xfffful << PWM_RCAPDAT_RCAPDAT_Pos)             /*!< PWM_T::RCAPDAT0: RCAPDAT Mask          */

#define PWM_FCAPDAT_FCAPDAT_Pos          (0)                                               /*!< PWM_T::FCAPDAT0: FCAPDAT Position      */
#define PWM_FCAPDAT_FCAPDAT_Msk          (0xfffful << PWM_FCAPDAT_FCAPDAT_Pos)             /*!< PWM_T::FCAPDAT0: FCAPDAT Mask          */

#define PWM_RCAPDAT0_RCAPDAT_Pos         (0)                                               /*!< PWM_T::RCAPDAT0: RCAPDAT Position      */
#define PWM_RCAPDAT0_RCAPDAT_Msk         (0xfffful << PWM_RCAPDAT0_RCAPDAT_Pos)            /*!< PWM_T::RCAPDAT0: RCAPDAT Mask          */

#define PWM_FCAPDAT0_FCAPDAT_Pos         (0)                                               /*!< PWM_T::FCAPDAT0: FCAPDAT Position      */
#define PWM_FCAPDAT0_FCAPDAT_Msk         (0xfffful << PWM_FCAPDAT0_FCAPDAT_Pos)            /*!< PWM_T::FCAPDAT0: FCAPDAT Mask          */

#define PWM_RCAPDAT1_RCAPDAT_Pos         (0)                                               /*!< PWM_T::RCAPDAT1: RCAPDAT Position      */
#define PWM_RCAPDAT1_RCAPDAT_Msk         (0xfffful << PWM_RCAPDAT1_RCAPDAT_Pos)            /*!< PWM_T::RCAPDAT1: RCAPDAT Mask          */

#define PWM_FCAPDAT1_FCAPDAT_Pos         (0)                                               /*!< PWM_T::FCAPDAT1: FCAPDAT Position      */
#define PWM_FCAPDAT1_FCAPDAT_Msk         (0xfffful << PWM_FCAPDAT1_FCAPDAT_Pos)            /*!< PWM_T::FCAPDAT1: FCAPDAT Mask          */

#define PWM_RCAPDAT2_RCAPDAT_Pos         (0)                                               /*!< PWM_T::RCAPDAT2: RCAPDAT Position      */
#define PWM_RCAPDAT2_RCAPDAT_Msk         (0xfffful << PWM_RCAPDAT2_RCAPDAT_Pos)            /*!< PWM_T::RCAPDAT2: RCAPDAT Mask          */

#define PWM_FCAPDAT2_FCAPDAT_Pos         (0)                                               /*!< PWM_T::FCAPDAT2: FCAPDAT Position      */
#define PWM_FCAPDAT2_FCAPDAT_Msk         (0xfffful << PWM_FCAPDAT2_FCAPDAT_Pos)            /*!< PWM_T::FCAPDAT2: FCAPDAT Mask          */

#define PWM_RCAPDAT3_RCAPDAT_Pos         (0)                                               /*!< PWM_T::RCAPDAT3: RCAPDAT Position      */
#define PWM_RCAPDAT3_RCAPDAT_Msk         (0xfffful << PWM_RCAPDAT3_RCAPDAT_Pos)            /*!< PWM_T::RCAPDAT3: RCAPDAT Mask          */

#define PWM_FCAPDAT3_FCAPDAT_Pos         (0)                                               /*!< PWM_T::FCAPDAT3: FCAPDAT Position      */
#define PWM_FCAPDAT3_FCAPDAT_Msk         (0xfffful << PWM_FCAPDAT3_FCAPDAT_Pos)            /*!< PWM_T::FCAPDAT3: FCAPDAT Mask          */

#define PWM_RCAPDAT4_RCAPDAT_Pos         (0)                                               /*!< PWM_T::RCAPDAT4: RCAPDAT Position      */
#define PWM_RCAPDAT4_RCAPDAT_Msk         (0xfffful << PWM_RCAPDAT4_RCAPDAT_Pos)            /*!< PWM_T::RCAPDAT4: RCAPDAT Mask          */

#define PWM_FCAPDAT4_FCAPDAT_Pos         (0)                                               /*!< PWM_T::FCAPDAT4: FCAPDAT Position      */
#define PWM_FCAPDAT4_FCAPDAT_Msk         (0xfffful << PWM_FCAPDAT4_FCAPDAT_Pos)            /*!< PWM_T::FCAPDAT4: FCAPDAT Mask          */

#define PWM_RCAPDAT5_RCAPDAT_Pos         (0)                                               /*!< PWM_T::RCAPDAT5: RCAPDAT Position      */
#define PWM_RCAPDAT5_RCAPDAT_Msk         (0xfffful << PWM_RCAPDAT5_RCAPDAT_Pos)            /*!< PWM_T::RCAPDAT5: RCAPDAT Mask          */

#define PWM_FCAPDAT5_FCAPDAT_Pos         (0)                                               /*!< PWM_T::FCAPDAT5: FCAPDAT Position      */
#define PWM_FCAPDAT5_FCAPDAT_Msk         (0xfffful << PWM_FCAPDAT5_FCAPDAT_Pos)            /*!< PWM_T::FCAPDAT5: FCAPDAT Mask          */

#define PWM_PDMACTL_CHEN0_1_Pos          (0)                                               /*!< PWM_T::PDMACTL: CHEN0_1 Position       */
#define PWM_PDMACTL_CHEN0_1_Msk          (0x1ul << PWM_PDMACTL_CHEN0_1_Pos)                /*!< PWM_T::PDMACTL: CHEN0_1 Mask           */

#define PWM_PDMACTL_CAPMOD0_1_Pos        (1)                                               /*!< PWM_T::PDMACTL: CAPMOD0_1 Position     */
#define PWM_PDMACTL_CAPMOD0_1_Msk        (0x3ul << PWM_PDMACTL_CAPMOD0_1_Pos)              /*!< PWM_T::PDMACTL: CAPMOD0_1 Mask         */

#define PWM_PDMACTL_CAPORD0_1_Pos        (3)                                               /*!< PWM_T::PDMACTL: CAPORD0_1 Position     */
#define PWM_PDMACTL_CAPORD0_1_Msk        (0x1ul << PWM_PDMACTL_CAPORD0_1_Pos)              /*!< PWM_T::PDMACTL: CAPORD0_1 Mask         */

#define PWM_PDMACTL_CHSEL0_1_Pos         (4)                                               /*!< PWM_T::PDMACTL: CHSEL0_1 Position      */
#define PWM_PDMACTL_CHSEL0_1_Msk         (0x1ul << PWM_PDMACTL_CHSEL0_1_Pos)               /*!< PWM_T::PDMACTL: CHSEL0_1 Mask          */

#define PWM_PDMACTL_CHEN2_3_Pos          (8)                                               /*!< PWM_T::PDMACTL: CHEN2_3 Position       */
#define PWM_PDMACTL_CHEN2_3_Msk          (0x1ul << PWM_PDMACTL_CHEN2_3_Pos)                /*!< PWM_T::PDMACTL: CHEN2_3 Mask           */

#define PWM_PDMACTL_CAPMOD2_3_Pos        (9)                                               /*!< PWM_T::PDMACTL: CAPMOD2_3 Position     */
#define PWM_PDMACTL_CAPMOD2_3_Msk        (0x3ul << PWM_PDMACTL_CAPMOD2_3_Pos)              /*!< PWM_T::PDMACTL: CAPMOD2_3 Mask         */

#define PWM_PDMACTL_CAPORD2_3_Pos        (11)                                              /*!< PWM_T::PDMACTL: CAPORD2_3 Position     */
#define PWM_PDMACTL_CAPORD2_3_Msk        (0x1ul << PWM_PDMACTL_CAPORD2_3_Pos)              /*!< PWM_T::PDMACTL: CAPORD2_3 Mask         */

#define PWM_PDMACTL_CHSEL2_3_Pos         (12)                                              /*!< PWM_T::PDMACTL: CHSEL2_3 Position      */
#define PWM_PDMACTL_CHSEL2_3_Msk         (0x1ul << PWM_PDMACTL_CHSEL2_3_Pos)               /*!< PWM_T::PDMACTL: CHSEL2_3 Mask          */

#define PWM_PDMACTL_CHEN4_5_Pos          (16)                                              /*!< PWM_T::PDMACTL: CHEN4_5 Position       */
#define PWM_PDMACTL_CHEN4_5_Msk          (0x1ul << PWM_PDMACTL_CHEN4_5_Pos)                /*!< PWM_T::PDMACTL: CHEN4_5 Mask           */

#define PWM_PDMACTL_CAPMOD4_5_Pos        (17)                                              /*!< PWM_T::PDMACTL: CAPMOD4_5 Position     */
#define PWM_PDMACTL_CAPMOD4_5_Msk        (0x3ul << PWM_PDMACTL_CAPMOD4_5_Pos)              /*!< PWM_T::PDMACTL: CAPMOD4_5 Mask         */

#define PWM_PDMACTL_CAPORD4_5_Pos        (19)                                              /*!< PWM_T::PDMACTL: CAPORD4_5 Position     */
#define PWM_PDMACTL_CAPORD4_5_Msk        (0x1ul << PWM_PDMACTL_CAPORD4_5_Pos)              /*!< PWM_T::PDMACTL: CAPORD4_5 Mask         */

#define PWM_PDMACTL_CHSEL4_5_Pos         (20)                                              /*!< PWM_T::PDMACTL: CHSEL4_5 Position      */
#define PWM_PDMACTL_CHSEL4_5_Msk         (0x1ul << PWM_PDMACTL_CHSEL4_5_Pos)               /*!< PWM_T::PDMACTL: CHSEL4_5 Mask          */

#define PWM_PDMACAP0_1_CAPBUF_Pos        (0)                                               /*!< PWM_T::PDMACAP0_1: CAPBUF Position     */
#define PWM_PDMACAP0_1_CAPBUF_Msk        (0xfffful << PWM_PDMACAP0_1_CAPBUF_Pos)           /*!< PWM_T::PDMACAP0_1: CAPBUF Mask         */

#define PWM_PDMACAP2_3_CAPBUF_Pos        (0)                                               /*!< PWM_T::PDMACAP2_3: CAPBUF Position     */
#define PWM_PDMACAP2_3_CAPBUF_Msk        (0xfffful << PWM_PDMACAP2_3_CAPBUF_Pos)           /*!< PWM_T::PDMACAP2_3: CAPBUF Mask         */

#define PWM_PDMACAP4_5_CAPBUF_Pos        (0)                                               /*!< PWM_T::PDMACAP4_5: CAPBUF Position     */
#define PWM_PDMACAP4_5_CAPBUF_Msk        (0xfffful << PWM_PDMACAP4_5_CAPBUF_Pos)           /*!< PWM_T::PDMACAP4_5: CAPBUF Mask         */

#define PWM_CAPIEN_CAPRIENn_Pos          (0)                                               /*!< PWM_T::CAPIEN: CAPRIENn Position       */
#define PWM_CAPIEN_CAPRIENn_Msk          (0x3ful << PWM_CAPIEN_CAPRIENn_Pos)               /*!< PWM_T::CAPIEN: CAPRIENn Mask           */

#define PWM_CAPIEN_CAPRIEN0_Pos          (0)                                               /*!< PWM_T::CAPIEN: CAPRIEN0 Position       */
#define PWM_CAPIEN_CAPRIEN0_Msk          (0x1ul << PWM_CAPIEN_CAPRIEN0_Pos)                /*!< PWM_T::CAPIEN: CAPRIEN0 Mask           */

#define PWM_CAPIEN_CAPRIEN1_Pos          (1)                                               /*!< PWM_T::CAPIEN: CAPRIEN1 Position       */
#define PWM_CAPIEN_CAPRIEN1_Msk          (0x1ul << PWM_CAPIEN_CAPRIEN1_Pos)                /*!< PWM_T::CAPIEN: CAPRIEN1 Mask           */

#define PWM_CAPIEN_CAPRIEN2_Pos          (2)                                               /*!< PWM_T::CAPIEN: CAPRIEN2 Position       */
#define PWM_CAPIEN_CAPRIEN2_Msk          (0x1ul << PWM_CAPIEN_CAPRIEN2_Pos)                /*!< PWM_T::CAPIEN: CAPRIEN2 Mask           */

#define PWM_CAPIEN_CAPRIEN3_Pos          (3)                                               /*!< PWM_T::CAPIEN: CAPRIEN3 Position       */
#define PWM_CAPIEN_CAPRIEN3_Msk          (0x1ul << PWM_CAPIEN_CAPRIEN3_Pos)                /*!< PWM_T::CAPIEN: CAPRIEN3 Mask           */

#define PWM_CAPIEN_CAPRIEN4_Pos          (4)                                               /*!< PWM_T::CAPIEN: CAPRIEN4 Position       */
#define PWM_CAPIEN_CAPRIEN4_Msk          (0x1ul << PWM_CAPIEN_CAPRIEN4_Pos)                /*!< PWM_T::CAPIEN: CAPRIEN4 Mask           */

#define PWM_CAPIEN_CAPRIEN5_Pos          (5)                                               /*!< PWM_T::CAPIEN: CAPRIEN5 Position       */
#define PWM_CAPIEN_CAPRIEN5_Msk          (0x1ul << PWM_CAPIEN_CAPRIEN5_Pos)                /*!< PWM_T::CAPIEN: CAPRIEN5 Mask           */

#define PWM_CAPIEN_CAPFIENn_Pos          (8)                                               /*!< PWM_T::CAPIEN: CAPFIENn Position       */
#define PWM_CAPIEN_CAPFIENn_Msk          (0x3ful << PWM_CAPIEN_CAPFIENn_Pos)               /*!< PWM_T::CAPIEN: CAPFIENn Mask           */

#define PWM_CAPIEN_CAPFIEN0_Pos          (8)                                               /*!< PWM_T::CAPIEN: CAPFIEN0 Position       */
#define PWM_CAPIEN_CAPFIEN0_Msk          (0x1ul << PWM_CAPIEN_CAPFIEN0_Pos)                /*!< PWM_T::CAPIEN: CAPFIEN0 Mask           */

#define PWM_CAPIEN_CAPFIEN1_Pos          (9)                                               /*!< PWM_T::CAPIEN: CAPFIEN1 Position       */
#define PWM_CAPIEN_CAPFIEN1_Msk          (0x1ul << PWM_CAPIEN_CAPFIEN1_Pos)                /*!< PWM_T::CAPIEN: CAPFIEN1 Mask           */

#define PWM_CAPIEN_CAPFIEN2_Pos          (10)                                              /*!< PWM_T::CAPIEN: CAPFIEN2 Position       */
#define PWM_CAPIEN_CAPFIEN2_Msk          (0x1ul << PWM_CAPIEN_CAPFIEN2_Pos)                /*!< PWM_T::CAPIEN: CAPFIEN2 Mask           */

#define PWM_CAPIEN_CAPFIEN3_Pos          (11)                                              /*!< PWM_T::CAPIEN: CAPFIEN3 Position       */
#define PWM_CAPIEN_CAPFIEN3_Msk          (0x1ul << PWM_CAPIEN_CAPFIEN3_Pos)                /*!< PWM_T::CAPIEN: CAPFIEN3 Mask           */

#define PWM_CAPIEN_CAPFIEN4_Pos          (12)                                              /*!< PWM_T::CAPIEN: CAPFIEN4 Position       */
#define PWM_CAPIEN_CAPFIEN4_Msk          (0x1ul << PWM_CAPIEN_CAPFIEN4_Pos)                /*!< PWM_T::CAPIEN: CAPFIEN4 Mask           */

#define PWM_CAPIEN_CAPFIEN5_Pos          (13)                                              /*!< PWM_T::CAPIEN: CAPFIEN5 Position       */
#define PWM_CAPIEN_CAPFIEN5_Msk          (0x1ul << PWM_CAPIEN_CAPFIEN5_Pos)                /*!< PWM_T::CAPIEN: CAPFIEN5 Mask           */

#define PWM_CAPIF_CRLIFn_Pos             (0)                                               /*!< PWM_T::CAPIF: CRLIFn Position          */
#define PWM_CAPIF_CRLIFn_Msk             (0x3ful << PWM_CAPIF_CRLIFn_Pos)                  /*!< PWM_T::CAPIF: CRLIFn Mask              */

#define PWM_CAPIF_CRLIF0_Pos             (0)                                               /*!< PWM_T::CAPIF: CRLIF0 Position          */
#define PWM_CAPIF_CRLIF0_Msk             (0x1ul << PWM_CAPIF_CRLIF0_Pos)                   /*!< PWM_T::CAPIF: CRLIF0 Mask              */

#define PWM_CAPIF_CRLIF1_Pos             (1)                                               /*!< PWM_T::CAPIF: CRLIF1 Position          */
#define PWM_CAPIF_CRLIF1_Msk             (0x1ul << PWM_CAPIF_CRLIF1_Pos)                   /*!< PWM_T::CAPIF: CRLIF1 Mask              */

#define PWM_CAPIF_CRLIF2_Pos             (2)                                               /*!< PWM_T::CAPIF: CRLIF2 Position          */
#define PWM_CAPIF_CRLIF2_Msk             (0x1ul << PWM_CAPIF_CRLIF2_Pos)                   /*!< PWM_T::CAPIF: CRLIF2 Mask              */

#define PWM_CAPIF_CRLIF3_Pos             (3)                                               /*!< PWM_T::CAPIF: CRLIF3 Position          */
#define PWM_CAPIF_CRLIF3_Msk             (0x1ul << PWM_CAPIF_CRLIF3_Pos)                   /*!< PWM_T::CAPIF: CRLIF3 Mask              */

#define PWM_CAPIF_CRLIF4_Pos             (4)                                               /*!< PWM_T::CAPIF: CRLIF4 Position          */
#define PWM_CAPIF_CRLIF4_Msk             (0x1ul << PWM_CAPIF_CRLIF4_Pos)                   /*!< PWM_T::CAPIF: CRLIF4 Mask              */

#define PWM_CAPIF_CRLIF5_Pos             (5)                                               /*!< PWM_T::CAPIF: CRLIF5 Position          */
#define PWM_CAPIF_CRLIF5_Msk             (0x1ul << PWM_CAPIF_CRLIF5_Pos)                   /*!< PWM_T::CAPIF: CRLIF5 Mask              */

#define PWM_CAPIF_CFLIFn_Pos             (8)                                               /*!< PWM_T::CAPIF: CFLIFn Position          */
#define PWM_CAPIF_CFLIFn_Msk             (0x3ful << PWM_CAPIF_CFLIFn_Pos)                  /*!< PWM_T::CAPIF: CFLIFn Mask              */

#define PWM_CAPIF_CFLIF0_Pos             (8)                                               /*!< PWM_T::CAPIF: CFLIF0 Position          */
#define PWM_CAPIF_CFLIF0_Msk             (0x1ul << PWM_CAPIF_CFLIF0_Pos)                   /*!< PWM_T::CAPIF: CFLIF0 Mask              */

#define PWM_CAPIF_CFLIF1_Pos             (9)                                               /*!< PWM_T::CAPIF: CFLIF1 Position          */
#define PWM_CAPIF_CFLIF1_Msk             (0x1ul << PWM_CAPIF_CFLIF1_Pos)                   /*!< PWM_T::CAPIF: CFLIF1 Mask              */

#define PWM_CAPIF_CFLIF2_Pos             (10)                                              /*!< PWM_T::CAPIF: CFLIF2 Position          */
#define PWM_CAPIF_CFLIF2_Msk             (0x1ul << PWM_CAPIF_CFLIF2_Pos)                   /*!< PWM_T::CAPIF: CFLIF2 Mask              */

#define PWM_CAPIF_CFLIF3_Pos             (11)                                              /*!< PWM_T::CAPIF: CFLIF3 Position          */
#define PWM_CAPIF_CFLIF3_Msk             (0x1ul << PWM_CAPIF_CFLIF3_Pos)                   /*!< PWM_T::CAPIF: CFLIF3 Mask              */

#define PWM_CAPIF_CFLIF4_Pos             (12)                                              /*!< PWM_T::CAPIF: CFLIF4 Position          */
#define PWM_CAPIF_CFLIF4_Msk             (0x1ul << PWM_CAPIF_CFLIF4_Pos)                   /*!< PWM_T::CAPIF: CFLIF4 Mask              */

#define PWM_CAPIF_CFLIF5_Pos             (13)                                              /*!< PWM_T::CAPIF: CFLIF5 Position          */
#define PWM_CAPIF_CFLIF5_Msk             (0x1ul << PWM_CAPIF_CFLIF5_Pos)                   /*!< PWM_T::CAPIF: CFLIF5 Mask              */

#define PWM_PBUF_PBUF_Pos                (0)                                               /*!< PWM_T::PBUF0: PBUF Position            */
#define PWM_PBUF_PBUF_Msk                (0xfffful << PWM_PBUF_PBUF_Pos)                   /*!< PWM_T::PBUF0: PBUF Mask                */

#define PWM_PBUF0_PBUF_Pos               (0)                                               /*!< PWM_T::PBUF0: PBUF Position            */
#define PWM_PBUF0_PBUF_Msk               (0xfffful << PWM_PBUF0_PBUF_Pos)                  /*!< PWM_T::PBUF0: PBUF Mask                */

#define PWM_PBUF2_PBUF_Pos               (0)                                               /*!< PWM_T::PBUF2: PBUF Position            */
#define PWM_PBUF2_PBUF_Msk               (0xfffful << PWM_PBUF2_PBUF_Pos)                  /*!< PWM_T::PBUF2: PBUF Mask                */

#define PWM_PBUF4_PBUF_Pos               (0)                                               /*!< PWM_T::PBUF4: PBUF Position            */
#define PWM_PBUF4_PBUF_Msk               (0xfffful << PWM_PBUF4_PBUF_Pos)                  /*!< PWM_T::PBUF4: PBUF Mask                */

#define PWM_CMPBUF_CMPBUF_Pos            (0)                                               /*!< PWM_T::CMPBUF0: CMPBUF Position        */
#define PWM_CMPBUF_CMPBUF_Msk            (0xfffful << PWM_CMPBUF_CMPBUF_Pos)               /*!< PWM_T::CMPBUF0: CMPBUF Mask            */

#define PWM_CMPBUF0_CMPBUF_Pos           (0)                                               /*!< PWM_T::CMPBUF0: CMPBUF Position        */
#define PWM_CMPBUF0_CMPBUF_Msk           (0xfffful << PWM_CMPBUF0_CMPBUF_Pos)              /*!< PWM_T::CMPBUF0: CMPBUF Mask            */

#define PWM_CMPBUF1_CMPBUF_Pos           (0)                                               /*!< PWM_T::CMPBUF1: CMPBUF Position        */
#define PWM_CMPBUF1_CMPBUF_Msk           (0xfffful << PWM_CMPBUF1_CMPBUF_Pos)              /*!< PWM_T::CMPBUF1: CMPBUF Mask            */

#define PWM_CMPBUF2_CMPBUF_Pos           (0)                                               /*!< PWM_T::CMPBUF2: CMPBUF Position        */
#define PWM_CMPBUF2_CMPBUF_Msk           (0xfffful << PWM_CMPBUF2_CMPBUF_Pos)              /*!< PWM_T::CMPBUF2: CMPBUF Mask            */

#define PWM_CMPBUF3_CMPBUF_Pos           (0)                                               /*!< PWM_T::CMPBUF3: CMPBUF Position        */
#define PWM_CMPBUF3_CMPBUF_Msk           (0xfffful << PWM_CMPBUF3_CMPBUF_Pos)              /*!< PWM_T::CMPBUF3: CMPBUF Mask            */

#define PWM_CMPBUF4_CMPBUF_Pos           (0)                                               /*!< PWM_T::CMPBUF4: CMPBUF Position        */
#define PWM_CMPBUF4_CMPBUF_Msk           (0xfffful << PWM_CMPBUF4_CMPBUF_Pos)              /*!< PWM_T::CMPBUF4: CMPBUF Mask            */

#define PWM_CMPBUF5_CMPBUF_Pos           (0)                                               /*!< PWM_T::CMPBUF5: CMPBUF Position        */
#define PWM_CMPBUF5_CMPBUF_Msk           (0xfffful << PWM_CMPBUF5_CMPBUF_Pos)              /*!< PWM_T::CMPBUF5: CMPBUF Mask            */

/**@}*/ /* PWM_CONST */
/**@}*/ /* end of PWM register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
    #pragma no_anon_unions
#endif

#endif /* __PWM_REG_H__ */
/**************************************************************************//**
 * @file     opa_reg.h
 * @version  V1.00
 * @brief    OPA register definition header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __OPA_REG_H__
#define __OPA_REG_H__

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup OPA OP Amplifier (OPA)
    Memory Mapped Structure for OPA Controller
@{ */

typedef struct
{


    /**
     * @var OPA_T::CTL
     * Offset: 0x00  OP Amplifier Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |OPEN0     |OP Amplifier 0 Enable Bit
     * |        |          |0 = Disable OP amplifier0.
     * |        |          |1 = Enabled OP amplifier0.
     * |        |          |Note: OP Amplifier 0 output needs wait stable 20us after OPEN0 is set.
     * |[4]     |OPDOEN0   |OP Amplifier 0 Schmitt Trigger Non-inverting Buffer Enable Bit
     * |        |          |0 = Disable OP amplifier0 schmitt trigger non-invert buffer.
     * |        |          |1 = Enable OP amplifier0 schmitt trigger non-invert buffer.
     * |[8]     |OPDOIEN0  |OP Amplifier 0 Schmitt Trigger Digital Output Interrupt Enable Bit
     * |        |          |0 = OP Amplifier 0 digital output interrupt function Disabled.
     * |        |          |1 = OP Amplifier 0 digital output interrupt function Enabled.
     * |        |          |The OPDOIF0 interrupt flag is set by hardware whenever the OP amplifier 0 Schmitt trigger non-inverting buffer digital
     * |        |          |output changes state, in the meanwhile, if OPDOIEN0 is set to 1, a comparator interrupt request is generated.
     * @var OPA_T::STATUS
     * Offset: 0x04  OP Amplifier Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |OPDO0     |OP Amplifier 0 Digital Output
     * |        |          |Synchronized to the APB clock to allow reading by software.
     * |        |          |Cleared when the Schmitt trigger buffer is disabled (OPDOEN0 = 0)
     * |[4]     |OPDOIF0   |OP Amplifier 0 Schmitt Trigger Digital Output Interrupt Flag
     * |        |          |OPDOIF0 interrupt flag is set by hardware whenever the OP amplifier 0 Schmitt trigger non-inverting buffer digital
     * |        |          |output changes state. This bit is cleared by writing 1 to it.
     * @var OPA_T::CALCTL
     * Offset: 0x08  OP Amplifier Calibration Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CALTRG0   |OP Amplifier 0 Calibration Trigger Bit
     * |        |          |0 = Stop, hardware auto clear.
     * |        |          |1 = Start.
     * |        |          |Note: Before enable this bit, it should set OPEN0 in advance.
     * |[16]    |CALRVS0   |OPA0 Calibration Reference Voltage Selection
     * |        |          |0 = VREF is 1/2 AVDD.
     * |        |          |1 = VREF from high vcm to low vcm.
     * @var OPA_T::CALST
     * Offset: 0x0C  OP Amplifier Calibration Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |DONE0     |OP Amplifier 0 Calibration Done Status
     * |        |          |0 = Calibrating.
     * |        |          |1 = Calibration Done.
     * |[1]     |CALNS0    |OP Amplifier 0 Calibration Result Status for NMOS
     * |        |          |0 = Pass.
     * |        |          |1 = Fail.
     * |[2]     |CALPS0    |OP Amplifier 0 Calibration Result Status for PMOS
     * |        |          |0 = Pass.
     * |        |          |1 = Fail.
     */
    __IO uint32_t CTL;                   /*!< [0x0000] OP Amplifier Control Register                                    */
    __IO uint32_t STATUS;                /*!< [0x0004] OP Amplifier Status Register                                     */
    __IO uint32_t CALCTL;                /*!< [0x0008] OP Amplifier Calibration Control Register                        */
    __I  uint32_t CALST;                 /*!< [0x000c] OP Amplifier Calibration Status Register                         */
} OPA_T;

/**
    @addtogroup OPA_CONST OPA Bit Field Definition
    Constant Definitions for OPA Controller
@{ */

#define OPA_CTL_OPEN0_Pos                (0)                                               /*!< OPA_T::CTL: OPEN0 Position             */
#define OPA_CTL_OPEN0_Msk                (0x1ul << OPA_CTL_OPEN0_Pos)                      /*!< OPA_T::CTL: OPEN0 Mask                 */

#define OPA_CTL_OPDOEN0_Pos              (4)                                               /*!< OPA_T::CTL: OPDOEN0 Position           */
#define OPA_CTL_OPDOEN0_Msk              (0x1ul << OPA_CTL_OPDOEN0_Pos)                    /*!< OPA_T::CTL: OPDOEN0 Mask               */

#define OPA_CTL_OPDOIEN0_Pos             (8)                                               /*!< OPA_T::CTL: OPDOIEN0 Position          */
#define OPA_CTL_OPDOIEN0_Msk             (0x1ul << OPA_CTL_OPDOIEN0_Pos)                   /*!< OPA_T::CTL: OPDOIEN0 Mask              */

#define OPA_STATUS_OPDO0_Pos             (0)                                               /*!< OPA_T::STATUS: OPDO0 Position          */
#define OPA_STATUS_OPDO0_Msk             (0x1ul << OPA_STATUS_OPDO0_Pos)                   /*!< OPA_T::STATUS: OPDO0 Mask              */

#define OPA_STATUS_OPDOIF0_Pos           (4)                                               /*!< OPA_T::STATUS: OPDOIF0 Position        */
#define OPA_STATUS_OPDOIF0_Msk           (0x1ul << OPA_STATUS_OPDOIF0_Pos)                 /*!< OPA_T::STATUS: OPDOIF0 Mask            */

#define OPA_CALCTL_CALTRG0_Pos           (0)                                               /*!< OPA_T::CALCTL: CALTRG0 Position        */
#define OPA_CALCTL_CALTRG0_Msk           (0x1ul << OPA_CALCTL_CALTRG0_Pos)                 /*!< OPA_T::CALCTL: CALTRG0 Mask            */

#define OPA_CALCTL_CALRVS0_Pos           (16)                                              /*!< OPA_T::CALCTL: CALRVS0 Position        */
#define OPA_CALCTL_CALRVS0_Msk           (0x1ul << OPA_CALCTL_CALRVS0_Pos)                 /*!< OPA_T::CALCTL: CALRVS0 Mask            */

#define OPA_CALST_DONE0_Pos              (0)                                               /*!< OPA_T::CALST: DONE0 Position           */
#define OPA_CALST_DONE0_Msk              (0x1ul << OPA_CALST_DONE0_Pos)                    /*!< OPA_T::CALST: DONE0 Mask               */

#define OPA_CALST_CALNS0_Pos             (1)                                               /*!< OPA_T::CALST: CALNS0 Position          */
#define OPA_CALST_CALNS0_Msk             (0x1ul << OPA_CALST_CALNS0_Pos)                   /*!< OPA_T::CALST: CALNS0 Mask              */

#define OPA_CALST_CALPS0_Pos             (2)                                               /*!< OPA_T::CALST: CALPS0 Position          */
#define OPA_CALST_CALPS0_Msk             (0x1ul << OPA_CALST_CALPS0_Pos)                   /*!< OPA_T::CALST: CALPS0 Mask              */



/**@}*/ /* OPA_CONST */
/**@}*/ /* end of OPA register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
    #pragma no_anon_unions
#endif

#endif /* __OPA_REG_H__ */

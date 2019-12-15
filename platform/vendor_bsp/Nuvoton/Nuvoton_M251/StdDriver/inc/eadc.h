/****************************************************************************//**
 * @file     eadc.h
 * @version  V0.10
 * @brief    M251 series EADC driver header file
 *
 * @note
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef __EADC_H__
#define __EADC_H__

/*---------------------------------------------------------------------------------------------------------*/
/* Include related headers                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#include "M251.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup EADC_Driver EADC Driver
  @{
*/

/** @addtogroup EADC_EXPORTED_CONSTANTS EADC Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/* EADC_SCTLn Constant Definitions                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define EADC_SCTL_CHSEL(x)                  ((x) << EADC_SCTL_CHSEL_Pos)       /*!< A/D sample module channel selection */
#define EADC_SCTL_TRGDLYDIV(x)              ((x) << EADC_SCTL_TRGDLYDIV_Pos)   /*!< A/D sample module start of conversion trigger delay clock divider selection */
#define EADC_SCTL_TRGDLYCNT(x)              ((x) << EADC_SCTL_TRGDLYCNT_Pos)   /*!< A/D sample module start of conversion trigger delay time */

#define EADC_SOFTWARE_TRIGGER               (0UL<<EADC_SCTL_TRGSEL_Pos)        /*!< Software trigger */
#define EADC_FALLING_EDGE_TRIGGER           ((1UL<<EADC_SCTL_TRGSEL_Pos) | EADC_SCTL_EXTFEN_Msk)      /*!< STADC pin falling edge trigger */
#define EADC_RISING_EDGE_TRIGGER            ((1UL<<EADC_SCTL_TRGSEL_Pos) | EADC_SCTL_EXTREN_Msk)      /*!< STADC pin rising edge trigger */
#define EADC_FALLING_RISING_EDGE_TRIGGER    ((1UL<<EADC_SCTL_TRGSEL_Pos) | EADC_SCTL_EXTFEN_Msk | EADC_SCTL_EXTREN_Msk) /*!< STADC pin both falling and rising edge trigger */
#define EADC_ADINT0_TRIGGER                 (2UL<<EADC_SCTL_TRGSEL_Pos)        /*!< EADC ADINT0 interrupt EOC pulse trigger */
#define EADC_ADINT1_TRIGGER                 (3UL<<EADC_SCTL_TRGSEL_Pos)        /*!< EADC ADINT1 interrupt EOC pulse trigger */
#define EADC_TIMER0_TRIGGER                 (4UL<<EADC_SCTL_TRGSEL_Pos)        /*!< Timer0 overflow pulse trigger */
#define EADC_TIMER1_TRIGGER                 (5UL<<EADC_SCTL_TRGSEL_Pos)        /*!< Timer1 overflow pulse trigger */
#define EADC_TIMER2_TRIGGER                 (6UL<<EADC_SCTL_TRGSEL_Pos)        /*!< Timer2 overflow pulse trigger */
#define EADC_TIMER3_TRIGGER                 (7UL<<EADC_SCTL_TRGSEL_Pos)        /*!< Timer3 overflow pulse trigger */
#define EADC_PWM0TG0_TRIGGER                (8UL<<EADC_SCTL_TRGSEL_Pos)        /*!< PWM0TG0 trigger */
#define EADC_PWM0TG1_TRIGGER                (9UL<<EADC_SCTL_TRGSEL_Pos)        /*!< PWM0TG1 trigger */
#define EADC_PWM0TG2_TRIGGER                (0xAUL<<EADC_SCTL_TRGSEL_Pos)      /*!< PWM0TG2 trigger */
#define EADC_PWM0TG3_TRIGGER                (0xBUL<<EADC_SCTL_TRGSEL_Pos)      /*!< PWM0TG3 trigger */
#define EADC_PWM0TG4_TRIGGER                (0xCUL<<EADC_SCTL_TRGSEL_Pos)      /*!< PWM0TG4 trigger */
#define EADC_PWM0TG5_TRIGGER                (0xDUL<<EADC_SCTL_TRGSEL_Pos)      /*!< PWM0TG5 trigger */
#define EADC_PWM1TG0_TRIGGER                (0xEUL<<EADC_SCTL_TRGSEL_Pos)      /*!< PWM1TG0 trigger */
#define EADC_PWM1TG1_TRIGGER                (0xFUL<<EADC_SCTL_TRGSEL_Pos)      /*!< PWM1TG1 trigger */
#define EADC_PWM1TG2_TRIGGER                (0x10UL<<EADC_SCTL_TRGSEL_Pos)     /*!< PWM1TG2 trigger */
#define EADC_PWM1TG3_TRIGGER                (0x11UL<<EADC_SCTL_TRGSEL_Pos)     /*!< PWM1TG3 trigger */
#define EADC_PWM1TG4_TRIGGER                (0x12UL<<EADC_SCTL_TRGSEL_Pos)     /*!< PWM1TG4 trigger */
#define EADC_PWM1TG5_TRIGGER                (0x13UL<<EADC_SCTL_TRGSEL_Pos)     /*!< PWM1TG5 trigger */
#define EADC_BPWM0TG_TRIGGER                (0x14UL<<EADC_SCTL_TRGSEL_Pos)     /*!< BPWM0TG trigger */
#define EADC_BPWM1TG_TRIGGER                (0x15UL<<EADC_SCTL_TRGSEL_Pos)     /*!< BPWM1TG trigger */

#define EADC_SCTL_TRGDLYDIV_DIVIDER_1       (0<<EADC_SCTL_TRGDLYDIV_Pos)       /*!< Trigger delay clock frequency is ADC_CLK/1 */
#define EADC_SCTL_TRGDLYDIV_DIVIDER_2       (0x1UL<<EADC_SCTL_TRGDLYDIV_Pos)   /*!< Trigger delay clock frequency is ADC_CLK/2 */
#define EADC_SCTL_TRGDLYDIV_DIVIDER_4       (0x2UL<<EADC_SCTL_TRGDLYDIV_Pos)   /*!< Trigger delay clock frequency is ADC_CLK/4 */
#define EADC_SCTL_TRGDLYDIV_DIVIDER_16      (0x3UL<<EADC_SCTL_TRGDLYDIV_Pos)   /*!< Trigger delay clock frequency is ADC_CLK/16 */

/*---------------------------------------------------------------------------------------------------------*/
/* EADC_CMPn Constant Definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define EADC_CMP_CMPCOND_LESS_THAN          (0UL<<EADC_CMP_CMPCOND_Pos)    /*!< The compare condition is "less than" */
#define EADC_CMP_CMPCOND_GREATER_OR_EQUAL   (1UL<<EADC_CMP_CMPCOND_Pos)    /*!< The compare condition is "greater than or equal to" */
#define EADC_CMP_CMPWEN_ENABLE              (EADC_CMP_CMPWEN_Msk)      /*!< Compare window mode enable */
#define EADC_CMP_CMPWEN_DISABLE             (~EADC_CMP_CMPWEN_Msk)     /*!< Compare window mode disable */
#define EADC_CMP_ADCMPIE_ENABLE             (EADC_CMP_ADCMPIE_Msk)     /*!< A/D result compare interrupt enable */
#define EADC_CMP_ADCMPIE_DISABLE            (~EADC_CMP_ADCMPIE_Msk)    /*!< A/D result compare interrupt disable */

/*---------------------------------------------------------------------------------------------------------*/
/* EADC_MnCTL1 Constant Definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define EADC_MCTL1_ACU_1                    (0x0UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 1 time */
#define EADC_MCTL1_ACU_2                    (0x1UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 2 times */
#define EADC_MCTL1_ACU_4                    (0x2UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 4 times */
#define EADC_MCTL1_ACU_8                    (0x3UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 8 times */
#define EADC_MCTL1_ACU_16                   (0x4UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 16 times */
#define EADC_MCTL1_ACU_32                   (0x5UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 32 times */
#define EADC_MCTL1_ACU_64                   (0x6UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 64 times */
#define EADC_MCTL1_ACU_128                  (0x7UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 128 times */
#define EADC_MCTL1_ACU_256                  (0x8UL<<EADC_MCTL1_ACU_Pos)    /*!< Accumulated 256 times */

/*---------------------------------------------------------------------------------------------------------*/
/* EADC_PWRCTL Constant Definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define EADC_PWRCTL_AUTOPDTHT_1             (0x0UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 1 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_2             (0x1UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 2 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_3             (0x2UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 3 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_4             (0x3UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 4 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_5             (0x4UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 5 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_6             (0x5UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 6 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_7             (0x6UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 7 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_8             (0x7UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 8 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_16            (0x8UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 16 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_32            (0x9UL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 32 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_64            (0xAUL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 64 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_128           (0xBUL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 128 EADC clock */
#define EADC_PWRCTL_AUTOPDTHT_256           (0xCUL<<EADC_PWRCTL_AUTOPDTHT_Pos)  /*!< Auto Power Down Threshold Time 256 EADC clock */


/*@}*/ /* end of group EADC_EXPORTED_CONSTANTS */

/** @addtogroup EADC_EXPORTED_FUNCTIONS EADC Exported Functions
  @{
*/
/*---------------------------------------------------------------------------------------------------------*/
/*  EADC Macro Definitions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

/**
  * @brief A/D Converter Control Circuits Reset.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return None
  * @details ADCRST bit (EADC_CTL[1]) remains 1 during EADC reset, when EADC reset end, the ADCRST bit is automatically cleared to 0.
  */
#define EADC_CONV_RESET(eadc) ((eadc)->CTL |= EADC_CTL_ADCRST_Msk)


/**
  * @brief Enable PDMA transfer.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details When A/D conversion is completed, the converted data is loaded into EADC_DATn (n=0~15) register,
  *         user can enable this bit to generate a PDMA data transfer request.
  */
#define EADC_ENABLE_PDMA(eadc, u32ModuleNum) (eadc)->PDMACTL |= (0x1 << u32ModuleNum)


/**
  * @brief Disable PDMA transfer.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details This macro is used to disable PDMA transfer.
  */
#define EADC_DISABLE_PDMA(eadc, u32ModuleNum) (eadc)->PDMACTL &= ~(0x1 << u32ModuleNum)


/**
  * @brief Set ADIFn at A/D start of conversion.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details The A/D converter generates ADIFn (EADC_STATUS2[n], n=0~3) at the start of conversion.
  */
#define EADC_ENABLE_INT_POSITION(eadc, u32ModuleNum) ((((eadc)->SCTL[(u32ModuleNum)])) |= EADC_SCTL_INTPOS_Msk)


/**
  * @brief Set ADIFn at A/D end of conversion.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details The A/D converter generates ADIFn (EADC_STATUS2[n], n=0~3) at the end of conversion.
  */
#define EADC_DISABLE_INT_POSITION(eadc, u32ModuleNum) ((((eadc)->SCTL[(u32ModuleNum)])) &= (~EADC_SCTL_INTPOS_Msk))


/**
  * @brief Enable the interrupt.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32Mask Decides the combination of interrupt status bits. Each bit corresponds to a interrupt status.
  *                    This parameter decides which interrupts will be enabled. Bit 0 is ADCIEN0, bit 1 is ADCIEN1..., bit 3 is ADCIEN3.
  * @return None
  * @details The A/D converter generates a conversion end ADIFn (EADC_STATUS2[n], n=0~3) upon the end of specific sample module A/D conversion.
  *         If ADCIENn bit (EADC_CTL[n+2]) is set then conversion end interrupt request ADINTn is generated (n=0~3).
  */
#define EADC_ENABLE_INT(eadc, u32Mask) ((eadc)->CTL |= ((u32Mask) << EADC_CTL_ADCIEN0_Pos))


/**
  * @brief Disable the interrupt.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32Mask Decides the combination of interrupt status bits. Each bit corresponds to a interrupt status.
  *                    This parameter decides which interrupts will be disabled. Bit 0 is ADCIEN0, bit 1 is ADCIEN1..., bit 3 is ADCIEN3.
  * @return None
  * @details Specific sample module A/D ADINT0 interrupt function disabled.
  */
#define EADC_DISABLE_INT(eadc, u32Mask) ((eadc)->CTL &= (~((u32Mask) << EADC_CTL_ADCIEN0_Pos)))


/**
  * @brief Enable the sample module interrupt.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32IntSel Decides which interrupt source will be used, valid value are from 0 to 3.
  * @param[in] u32ModuleMask the combination of sample module interrupt status bits. Each bit corresponds to a sample module interrupt status.
  *                          This parameter decides which sample module interrupts will be enabled, valid range are between 1~0x7FFFF.
  * @return None
  * @details There are 4 EADC interrupts ADINT0~3, and each of these interrupts has its own interrupt vector address.
  */
#define EADC_ENABLE_SAMPLE_MODULE_INT(eadc, u32IntSel, u32ModuleMask) ((((eadc)->INTSRC[(u32IntSel)])) |= (u32ModuleMask))


/**
  * @brief Disable the sample module interrupt.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32IntSel Decides which interrupt source will be used, valid value are from 0 to 3.
  * @param[in] u32ModuleMask the combination of sample module interrupt status bits. Each bit corresponds to a sample module interrupt status.
  *                          This parameter decides which sample module interrupts will be disabled, valid range are between 1~0x7FFFF.
  * @return None
  * @details There are 4 EADC interrupts ADINT0~3, and each of these interrupts has its own interrupt vector address.
  */
#define EADC_DISABLE_SAMPLE_MODULE_INT(eadc, u32IntSel, u32ModuleMask) ((((eadc)->INTSRC[(u32IntSel)])) &= (~(u32ModuleMask)))


/**
  * @brief Start the A/D conversion.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleMask The combination of sample module. Each bit corresponds to a sample module.
  *                         This parameter decides which sample module will be converted, valid range are between 1~0x7FFFF.
  *                         Bit 0 is sample module 0, bit 1 is sample module 1..., bit 18 is sample module 18.
  * @return None
  * @details After write EADC_SWTRG register to start EADC conversion, the EADC_PENDSTS register will show which sample module will conversion.
  */
#define EADC_START_CONV(eadc, u32ModuleMask) ((eadc)->SWTRG = (u32ModuleMask))


/**
  * @brief Cancel the conversion for sample module.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleMask The combination of sample module. Each bit corresponds to a sample module.
  *                         This parameter decides which sample module will stop the conversion, valid range are between 1~0x7FFFF.
  *                         Bit 0 is sample module 0, bit 1 is sample module 1..., bit 18 is sample module18.
  * @return None
  * @details If user want to disable the conversion of the sample module, user can write EADC_PENDSTS register to clear it.
  */
#define EADC_STOP_CONV(eadc, u32ModuleMask) ((eadc)->PENDSTS = (u32ModuleMask))


/**
  * @brief Get the conversion pending flag.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return Return the conversion pending sample module.
  * @details This STPFn (EADC_PENDSTS[n], n=0~18) bit remains 1 during pending state, when the respective EADC conversion is end,
  *         the STPFn (EADC_PENDSTS[n], n=0~18) bit is automatically cleared to 0.
  */
#define EADC_GET_PENDING_CONV(eadc) ((eadc)->PENDSTS)


/**
  * @brief Get the conversion data of the user-specified sample module.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 18.
  * @return Return the conversion data of the user-specified sample module.
  * @details This macro is used to read RESULT bit (EADC_DATn[15:0], n=0~18) field to get conversion data.
  */
#define EADC_GET_CONV_DATA(eadc, u32ModuleNum) ((((eadc)->DAT[(u32ModuleNum)])) & EADC_DAT_RESULT_Msk)


/**
  * @brief Get the data overrun flag of the user-specified sample module.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleMask The combination of data overrun status bits. Each bit corresponds to a data overrun status, valid range are between 1~0x7FFFF.
  * @return Return the data overrun flag of the user-specified sample module.
  * @details This macro is used to read OV bit (EADC_STATUS0[31:16], EADC_STATUS1[18:16]) field to get data overrun status.
  */
#define EADC_GET_DATA_OVERRUN_FLAG(eadc, u32ModuleMask) ((((eadc)->STATUS0 >> EADC_STATUS0_OV_Pos) | ((eadc)->STATUS1 & EADC_STATUS1_OV_Msk)) & (u32ModuleMask))


/**
  * @brief Get the data valid flag of the user-specified sample module.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleMask The combination of data valid status bits. Each bit corresponds to a data valid status, valid range are between 1~0x7FFFF.
  * @return Return the data valid flag of the user-specified sample module.
  * @details This macro is used to read VALID bit (EADC_STATUS0[15:0], EADC_STATUS1[2:0]) field to get data overrun status.
  */
#define EADC_GET_DATA_VALID_FLAG(eadc, u32ModuleMask) ((((eadc)->STATUS0 & EADC_STATUS0_VALID_Msk) | (((eadc)->STATUS1 & EADC_STATUS1_VALID_Msk) << 16)) & (u32ModuleMask))


/**
  * @brief Get the user-specified interrupt flags.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32Mask The combination of interrupt status bits. Each bit corresponds to a interrupt status.
  *                    Bit 0 is ADIF0, bit 1 is ADIF1..., bit 3 is ADIF3.
  *                    Bit 4 is ADCMPF0, bit 5 is ADCMPF1..., bit 7 is ADCMPF3.
  *                    Or use combination of following pre-define mask:
  *                         - \ref EADC_STATUS2_ADIF0_Msk    : ADIF0 mask
  *                         - \ref EADC_STATUS2_ADIF1_Msk    : ADIF1 mask
  *                         - \ref EADC_STATUS2_ADIF2_Msk    : ADIF2 mask
  *                         - \ref EADC_STATUS2_ADIF3_Msk    : ADIF3 mask
  *                         - \ref EADC_STATUS2_ADCMPF0_Msk  : ADCMPF0 mask
  *                         - \ref EADC_STATUS2_ADCMPF1_Msk  : ADCMPF1 mask
  *                         - \ref EADC_STATUS2_ADCMPF2_Msk  : ADCMPF2 mask
  *                         - \ref EADC_STATUS2_ADCMPF3_Msk  : ADCMPF3 mask
  * @return Return the user-specified interrupt flags.
  * @details This macro is used to get the user-specified interrupt flags.
  */
#define EADC_GET_INT_FLAG(eadc, u32Mask) ((eadc)->STATUS2 & (u32Mask))


/**
  * @brief Get the user-specified sample module overrun flags.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleMask The combination of sample module overrun status bits. Each bit corresponds to a sample module overrun status, valid range are between 1~0x7FFFF.
  * @return Return the user-specified sample module overrun flags.
  * @details This macro is used to get the user-specified sample module overrun flags.
  */
#define EADC_GET_SAMPLE_MODULE_OV_FLAG(eadc, u32ModuleMask) ((eadc)->OVSTS & (u32ModuleMask))


/**
  * @brief Clear the selected interrupt status bits.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32Mask The combination of compare interrupt status bits. Each bit corresponds to a compare interrupt status.
  *                    Bit 0 is ADIF0, bit 1 is ADIF1..., bit 3 is ADIF3.
  *                    Bit 4 is ADCMPF0, bit 5 is ADCMPF1..., bit 7 is ADCMPF3.
  *                    Or use combination of following pre-define mask:
  *                         - \ref EADC_STATUS2_ADIF0_Msk    : ADIF0 mask
  *                         - \ref EADC_STATUS2_ADIF1_Msk    : ADIF1 mask
  *                         - \ref EADC_STATUS2_ADIF2_Msk    : ADIF2 mask
  *                         - \ref EADC_STATUS2_ADIF3_Msk    : ADIF3 mask
  *                         - \ref EADC_STATUS2_ADCMPF0_Msk  : ADCMPF0 mask
  *                         - \ref EADC_STATUS2_ADCMPF1_Msk  : ADCMPF1 mask
  *                         - \ref EADC_STATUS2_ADCMPF2_Msk  : ADCMPF2 mask
  *                         - \ref EADC_STATUS2_ADCMPF3_Msk  : ADCMPF3 mask
  * @return None
  * @details This macro is used to clear clear the selected interrupt status bits.
  */
#define EADC_CLR_INT_FLAG(eadc, u32Mask) ((eadc)->STATUS2 = (u32Mask))


/**
  * @brief Clear the selected sample module overrun status bits.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleMask The combination of sample module overrun status bits. Each bit corresponds to a sample module overrun status.
  *                      Bit 0 is SPOVF0, bit 1 is SPOVF1..., bit 18 is SPOVF18.
  * @return None
  * @details This macro is used to clear the selected sample module overrun status bits.
  */
#define EADC_CLR_SAMPLE_MODULE_OV_FLAG(eadc, u32ModuleMask) ((eadc)->OVSTS = (u32ModuleMask))


/**
  * @brief Check all sample module A/D result data register overrun flags.
  * @param[in] eadc The pointer of the specified EADC module.
  * @retval 0 None of sample module data register overrun flag is set to 1.
  * @retval 1 Any one of sample module data register overrun flag is set to 1.
  * @details The AOV bit (EADC_STATUS2[27]) will keep 1 when any one of sample module data register overrun flag OVn (EADC_DATn[16], n=0~18) is set to 1.
  */
#define EADC_IS_DATA_OV(eadc) (((eadc)->STATUS2 & EADC_STATUS2_AOV_Msk) >> EADC_STATUS2_AOV_Pos)


/**
  * @brief Check all sample module A/D result data register valid flags.
  * @param[in] eadc The pointer of the specified EADC module.
  * @retval 0 None of sample module data register valid flag is set to 1.
  * @retval 1 Any one of sample module data register valid flag is set to 1.
  * @details The AVALID bit (EADC_STATUS2[26]) will keep 1 when any one of sample module data register valid flag VALIDn (EADC_DATn[17], n=0~18) is set to 1.
  */
#define EADC_IS_DATA_VALID(eadc) (((eadc)->STATUS2 & EADC_STATUS2_AVALID_Msk) >> EADC_STATUS2_AVALID_Pos)


/**
  * @brief Check all A/D sample module start of conversion overrun flags.
  * @param[in] eadc The pointer of the specified EADC module.
  * @retval 0 None of sample module event overrun flag is set to 1.
  * @retval 1 Any one of sample module event overrun flag is set to 1.
  * @details The STOVF bit (EADC_STATUS2[25]) will keep 1 when any one of sample module event overrun flag SPOVFn (EADC_OVSTS[n], n=0~18) is set to 1.
  */
#define EADC_IS_SAMPLE_MODULE_OV(eadc) (((eadc)->STATUS2 & EADC_STATUS2_STOVF_Msk) >> EADC_STATUS2_STOVF_Pos)


/**
  * @brief Check all A/D interrupt flag overrun bits.
  * @param[in] eadc The pointer of the specified EADC module.
  * @retval 0 None of ADINT interrupt flag is overwritten to 1.
  * @retval 1 Any one of ADINT interrupt flag is overwritten to 1.
  * @details The ADOVIF bit (EADC_STATUS2[24]) will keep 1 when any one of ADINT interrupt flag ADOVIFn (EADC_STATUS2[n+8], n=0~3) is overwritten to 1.
  */
#define EADC_IS_INT_FLAG_OV(eadc) (((eadc)->STATUS2 & EADC_STATUS2_ADOVIF_Msk) >> EADC_STATUS2_ADOVIF_Pos)


/**
  * @brief Get the busy state of EADC.
  * @param[in] eadc The pointer of the specified EADC module.
  * @retval 0 Idle state.
  * @retval 1 Busy state.
  * @details This macro is used to read BUSY bit (EADC_STATUS2[23]) to get busy state.
  */
#define EADC_IS_BUSY(eadc) (((eadc)->STATUS2 & EADC_STATUS2_BUSY_Msk) >> EADC_STATUS2_BUSY_Pos)


/**
  * @brief Configure the comparator 0 and enable it.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum specifies the compare sample module, valid values are from 0 to 18.
  * @param[in] u32Condition specifies the compare condition. Valid values are:
  *                        - \ref EADC_CMP_CMPCOND_LESS_THAN            : The compare condition is "less than the compare value"
  *                        - \ref EADC_CMP_CMPCOND_GREATER_OR_EQUAL     : The compare condition is "greater than or equal to the compare value"
  * @param[in] u16CMPData specifies the compare value, valid range are between 0~0xFFF.
  * @param[in] u32MatchCount specifies the match count setting, valid range are between 1~0xF.
  * @return None
  * @details For example, EADC_ENABLE_CMP0(EADC, 5, EADC_CMP_CMPCOND_GREATER_OR_EQUAL, 0x800, 10);
  *         Means EADC will assert comparator 0 flag if sample module 5 conversion result is greater or
  *         equal to 0x800 for 10 times continuously, and a compare interrupt request is generated.
  */
#define EADC_ENABLE_CMP0(eadc,\
                         u32ModuleNum,\
                         u32Condition,\
                         u16CMPData,\
                         u32MatchCount) ((eadc)->CMP[0] = (((u32ModuleNum) << EADC_CMP_CMPSPL_Pos) |\
                                 (u32Condition) |\
                                 ((u16CMPData) << EADC_CMP_CMPDAT_Pos) |\
                                 (((u32MatchCount) - 1) << EADC_CMP_CMPMCNT_Pos) |\
                                 EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Configure the comparator 1 and enable it.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum specifies the compare sample module, valid value are from 0 to 18.
  * @param[in] u32Condition specifies the compare condition. Valid values are:
  *                        - \ref EADC_CMP_CMPCOND_LESS_THAN            : The compare condition is "less than the compare value"
  *                        - \ref EADC_CMP_CMPCOND_GREATER_OR_EQUAL     : The compare condition s "greater than or equal to the compare value"
  * @param[in] u16CMPData specifies the compare value, valid range are between 0~0xFFF.
  * @param[in] u32MatchCount specifies the match count setting, valid range are between 1~0xF.
  * @return None
  * @details For example, EADC_ENABLE_CMP1(EADC, 5, EADC_CMP_CMPCOND_GREATER_OR_EQUAL, 0x800, 10);
  *         Means EADC will assert comparator 1 flag if sample module 5 conversion result is greater or
  *         equal to 0x800 for 10 times continuously, and a compare interrupt request is generated.
  */
#define EADC_ENABLE_CMP1(eadc,\
                         u32ModuleNum,\
                         u32Condition,\
                         u16CMPData,\
                         u32MatchCount) ((eadc)->CMP[1] = (((u32ModuleNum) << EADC_CMP_CMPSPL_Pos) |\
                                 (u32Condition) |\
                                 ((u16CMPData) << EADC_CMP_CMPDAT_Pos) |\
                                 (((u32MatchCount) - 1) << EADC_CMP_CMPMCNT_Pos) |\
                                 EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Configure the comparator 2 and enable it.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum specifies the compare sample module, valid value are from 0 to 18.
  * @param[in] u32Condition specifies the compare condition. Valid values are:
  *                        - \ref EADC_CMP_CMPCOND_LESS_THAN            : The compare condition is "less than the compare value"
  *                        - \ref EADC_CMP_CMPCOND_GREATER_OR_EQUAL     : The compare condition is "greater than or equal to the compare value"
  * @param[in] u16CMPData specifies the compare value, valid range are between 0~0xFFF.
  * @param[in] u32MatchCount specifies the match count setting, valid range are between 0~0xF.
  * @return None
  * @details For example, EADC_ENABLE_CMP2(EADC, 5, EADC_CMP_CMPCOND_GREATER_OR_EQUAL, 0x800, 10);
  *         Means EADC will assert comparator 2 flag if sample module 5 conversion result is greater or
  *         equal to 0x800 for 10 times continuously, and a compare interrupt request is generated.
  */
#define EADC_ENABLE_CMP2(eadc,\
                         u32ModuleNum,\
                         u32Condition,\
                         u16CMPData,\
                         u32MatchCount) ((eadc)->CMP[2] = (((u32ModuleNum) << EADC_CMP_CMPSPL_Pos) |\
                                 (u32Condition) |\
                                 ((u16CMPData) << EADC_CMP_CMPDAT_Pos) |\
                                 (((u32MatchCount) - 1) << EADC_CMP_CMPMCNT_Pos) |\
                                 EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Configure the comparator 3 and enable it.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum specifies the compare sample module, valid value are from 0 to 18.
  * @param[in] u32Condition specifies the compare condition. Valid values are:
  *                        - \ref EADC_CMP_CMPCOND_LESS_THAN            : The compare condition is "less than the compare value"
  *                        - \ref EADC_CMP_CMPCOND_GREATER_OR_EQUAL     : The compare condition is "greater than or equal to the compare value"
  * @param[in] u16CMPData specifies the compare value, valid range are between 0~0xFFF.
  * @param[in] u32MatchCount specifies the match count setting, valid range are between 1~0xF.
  * @return None
  * @details For example, EADC_ENABLE_CMP3(EADC, 5, EADC_CMP_CMPCOND_GREATER_OR_EQUAL, 0x800, 10);
  *         Means EADC will assert comparator 3 flag if sample module 5 conversion result is greater or
  *         equal to 0x800 for 10 times continuously, and a compare interrupt request is generated.
  */
#define EADC_ENABLE_CMP3(eadc,\
                         u32ModuleNum,\
                         u32Condition,\
                         u16CMPData,\
                         u32MatchCount) ((eadc)->CMP[3] = (((u32ModuleNum) << EADC_CMP_CMPSPL_Pos) |\
                                 (u32Condition) |\
                                 ((u16CMPData) << EADC_CMP_CMPDAT_Pos) |\
                                 (((u32MatchCount) - 1) << EADC_CMP_CMPMCNT_Pos) |\
                                 EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Enable the compare window mode.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32CMP Specifies the compare register, valid values are 0 and 2.
  * @return None
  * @details ADCMPF0 (EADC_STATUS2[4]) will be set when both EADC_CMP0 and EADC_CMP1 compared condition matched.
  *          ADCMPF2 (EADC_STATUS2[6]) will be set when both EADC_CMP2 and EADC_CMP3 compared condition matched.
  */
#define EADC_ENABLE_CMP_WINDOW_MODE(eadc, u32CMP) ((((eadc)->CMP[(u32CMP)])) |= EADC_CMP_CMPWEN_Msk)


/**
  * @brief Disable the compare window mode.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32CMP Specifies the compare register, valid values are 0 and 2.
  * @return None
  * @details ADCMPF0 (EADC_STATUS2[4]) will be set when EADC_CMP0 compared condition matched.
  *          ADCMPF2 (EADC_STATUS2[6]) will be set when EADC_CMP2 compared condition matched.
  */
#define EADC_DISABLE_CMP_WINDOW_MODE(eadc, u32CMP) ((((eadc)->CMP[(u32CMP)])) &= (~EADC_CMP_CMPWEN_Msk))


/**
  * @brief Enable the compare interrupt.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32CMP Specifies the compare register, valid values are from 0 to 3.
  * @return None
  * @details If the compare function is enabled and the compare condition matches the setting of CMPCOND (EADC_CMPn[2], n=0~3)
  *         and CMPMCNT (EADC_CMPn[11:8], n=0~3), ADCMPFn (EADC_STATUS2[n+4], n=0~3) will be asserted, in the meanwhile,
  *         if ADCMPIE (EADC_CMPn[1], n=0~3) is set to 1, a compare interrupt request is generated.
  */
#define EADC_ENABLE_CMP_INT(eadc, u32CMP) ((((eadc)->CMP[(u32CMP)])) |= EADC_CMP_ADCMPIE_Msk)


/**
  * @brief Disable the compare interrupt.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32CMP Specifies the compare register, valid value are from 0 to 3.
  * @return None
  * @details This macro is used to disable the compare interrupt.
  */
#define EADC_DISABLE_CMP_INT(eadc, u32CMP) ((((eadc)->CMP[(u32CMP)])) &= (~EADC_CMP_ADCMPIE_Msk))


/**
  * @brief Disable comparator 0.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return None
  * @details This macro is used to disable comparator 0.
  */
#define EADC_DISABLE_CMP0(eadc) ((eadc)->CMP[0] &= (~EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Disable comparator 1.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return None
  * @details This macro is used to disable comparator 1.
  */
#define EADC_DISABLE_CMP1(eadc) ((eadc)->CMP[1] &= (~EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Disable comparator 2.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return None
  * @details This macro is used to disable comparator 2.
  */
#define EADC_DISABLE_CMP2(eadc) ((eadc)->CMP[2] &= (~EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Disable comparator 3.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return None
  * @details This macro is used to disable comparator 3.
  */
#define EADC_DISABLE_CMP3(eadc) ((eadc)->CMP[3] &= (~EADC_CMP_ADCMPEN_Msk))


/**
  * @brief Enable conversion result left alignment.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details The 12-bit conversion result will be left aligned and stored in EADC_DATn[15:4] (n=0~15).
  */
#define EADC_ENABLE_LEFT_ALIGN(eadc, u32ModuleNum) ((((eadc)->MCTL1[(u32ModuleNum)])) |= EADC_MCTL1_ALIGN_Msk)


/**
  * @brief Disable conversion result left alignment.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details The 12-bit conversion result will be right aligned and stored in EADC_DATn[11:0] (n=0~15).
  */
#define EADC_DISABLE_LEFT_ALIGN(eadc, u32ModuleNum) ((((eadc)->MCTL1[(u32ModuleNum)])) &= (~EADC_MCTL1_ALIGN_Msk))


/**
  * @brief Enable average mode.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details Conversion results in data register will be averaged.
  * @note This average mode needs to work with accumulated mode that configured by ACU (EADC_MnCTL1[7:4], n=0~15) bit field.
  */
#define EADC_ENABLE_AVG(eadc, u32ModuleNum) ((((eadc)->MCTL1[(u32ModuleNum)])) |= EADC_MCTL1_AVG_Msk)


/**
  * @brief Disable average mode.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum Decides the sample module number, valid value are from 0 to 15.
  * @return None
  * @details Conversion results in data register will not be averaged.
  */
#define EADC_DISABLE_AVG(eadc, u32ModuleNum) ((((eadc)->MCTL1[(u32ModuleNum)])) &= (~EADC_MCTL1_AVG_Msk))


/**
  * @brief Configure the Accumulation feature and enable it.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum specifies the compare sample module, valid value are from 0 to 15.
  * @param[in] u16ACUNum specifies the number of accumulation, valid values are
  *                        - \ref EADC_MCTL1_ACU_1   : 1 conversion result will be accumulated.
  *                        - \ref EADC_MCTL1_ACU_2   : 2 conversion result will be accumulated.
  *                        - \ref EADC_MCTL1_ACU_4   : 4 conversion result will be accumulated.
  *                        - \ref EADC_MCTL1_ACU_8   : 8 conversion result will be accumulated.
  *                        - \ref EADC_MCTL1_ACU_16  : 16 conversion result will be accumulated.
  *                        - \ref EADC_MCTL1_ACU_32  : 32 conversion result will be accumulated. The result is right shift 1 bit to fit within the available 16-bit register size.
  *                        - \ref EADC_MCTL1_ACU_64  : 64 conversion result will be accumulated. The result is right shift 2 bits to fit within the available 16-bit register size.
  *                        - \ref EADC_MCTL1_ACU_128 : 128 conversion result will be accumulated. The result is right shift 3 bits to fit within the available 16-bit register size.
  *                        - \ref EADC_MCTL1_ACU_256 : 256 conversion result will be accumulated. The result is right shift 4 bits to fit within the available 16-bit register size.
  * @return None
  * @details When accumulating more than 16 samples, the result will be too large to match the
  *         16-bit RESULT register size (EADC_DATn[15:0]. To avoid overflow, the result is
  *         right shifted automatically to fit within the available 16-bit register size.
  *         The number of automatic right shifts is specified in parameter list above.
  *
  */
#define EADC_ENABLE_ACU(eadc,\
                        u32ModuleNum,\
                        u16ACUNum) ((((eadc)->MCTL1[(u32ModuleNum)])) = (((((eadc)->MCTL1[(u32ModuleNum)])) & (~EADC_MCTL1_ACU_Msk)) |\
                                    (u16ACUNum)))


/**
  * @brief Disable the Accumulation feature.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32ModuleNum specifies the compare sample module, valid value are from 0 to 15.
  * @return None
  */
#define EADC_DISABLE_ACU(eadc, u32ModuleNum) ((((eadc)->MCTL1[(u32ModuleNum)])) &= (~EADC_MCTL1_ACU_Msk))


/**
  * @brief Configure the Auto Power On/Off mode and enable it.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] u32PowerOffThreshold specifies the auto power off threshold time. Valid values are:
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_1  : Auto Power Off Threshold Time 1 EADC clock
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_2  : Auto Power Off Threshold Time 2 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_3  : Auto Power Off Threshold Time 3 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_4  : Auto Power Off Threshold Time 4 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_5  : Auto Power Off Threshold Time 5 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_6  : Auto Power Off Threshold Time 6 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_7  : Auto Power Off Threshold Time 7 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_8  : Auto Power Off Threshold Time 8 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_16 : Auto Power Off Threshold Time 16 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_32 : Auto Power Off Threshold Time 32 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_64 : Auto Power Off Threshold Time 64 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_128 : Auto Power Off Threshold Time 128 EADC clocks
  *                     - \ref EADC_PWRCTL_AUTOPDTHT_256 : Auto Power Off Threshold Time 256 EADC clocks
  * @param[in] u32PowerOnTime specifies the auto power on start-up time, valid range are between 0~0xFFF.
  * @return None
  * @details If the interval of time in idle state is longer than u32PowerOffThreshold, EADC will
  *         auto power off A/D converter analog circuit to reduce power consumption.
  *         EADC will automatically wakes-up when a conversion is started by software or hardware trigger.
  *         A start-up time is automatically inserted between the trigger event and the sampling time of the EADC.
  *         The start-up time = ((1/EADC_CLK) * u32PowerOnTime) and must be longer than 10us.
  * @note The ADCEN bit (EADC_CTL[0]) will be set to 0 when EADC power off and set to 1 when EADC be wake-up.
  */
#define EADC_ENABLE_AUTOFF(eadc,\
                           u32PowerOffThreshold,\
                           u32PowerOnTime) ((eadc)->PWRCTL = ((u32PowerOffThreshold) |\
                                   ((u32PowerOnTime) << EADC_PWRCTL_STUPT_Pos) |\
                                   EADC_PWRCTL_AUTOFF_Msk))


/**
  * @brief Disable the Auto Power On/Off mode.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return None
  */
#define EADC_DISABLE_AUTOFF(eadc) ((eadc)->PWRCTL &= (~EADC_PWRCTL_AUTOFF_Msk))


/**
  * @brief Configure the Offset Cancellation feature and enable it.
  * @param[in] eadc The pointer of the specified EADC module.
  * @param[in] i16OffsetCancel specifies the signed value of offset cancellation, valid values are -16 to 15.
  * @return None
  * @details When i16OffsetCancel is set to 0, the offset cancellation trim bits have no effect to A/D result.
  */
#define EADC_ENABLE_OFFSETCANCEL(eadc,\
                                 i16OffsetCancel) {(eadc)->CTL |= EADC_CTL_CALEN_Msk;\
    (eadc)->OFFSETCAL = (i16OffsetCancel & EADC_OFFSETCAL_OFFSETCANCEL_Msk);}


/**
  * @brief Disable the Offset Cancellation feature.
  * @param[in] eadc The pointer of the specified EADC module.
  * @return None
  */
#define EADC_DISABLE_OFFSETCANCEL(eadc) {(eadc)->OFFSETCAL = 0;\
        (eadc)->CTL &= (~EADC_CTL_CALEN_Msk);}


/*---------------------------------------------------------------------------------------------------------*/
/* Define EADC functions prototype                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void EADC_Open(EADC_T *eadc, uint32_t u32InputMode);
void EADC_Close(EADC_T *eadc);
void EADC_ConfigSampleModule(EADC_T *eadc, uint32_t u32ModuleNum, uint32_t u32TriggerSource, uint32_t u32Channel);
void EADC_SetTriggerDelayTime(EADC_T *eadc, uint32_t u32ModuleNum, uint32_t u32TriggerDelayTime, uint32_t u32DelayClockDivider);
void EADC_SetExtendSampleTime(EADC_T *eadc, uint32_t u32ModuleNum, uint32_t u32ExtendSampleTime);

/*@}*/ /* end of group EADC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group EADC_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __EADC_H__ */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/

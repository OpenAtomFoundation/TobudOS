/**************************************************************************//**
 * @file     eadc_reg.h
 * @version  V1.00
 * @brief    EADC register definition header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __EADC_REG_H__
#define __EADC_REG_H__

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup EADC Enhanced Analog to Digital Converter (EADC)
    Memory Mapped Structure for EADC Controller
@{ */

typedef struct
{


    /**
     * @var EADC_T::DAT[19]
     * Offset: 0x00~0x48  A/D Data Register 0~18 for Sample Module 0 ~ 18
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RESULT    |A/D Conversion Result
     * |        |          |This field contains 12 bits conversion result.
     * |[16]    |OV        |Overrun Flag
     * |        |          |If converted data in RESULT[11:0] has not been read before new conversion result is loaded to this register, OV is set to 1.
     * |        |          |0 = Data in RESULT[11:0] is recent conversion result.
     * |        |          |1 = Data in RESULT[11:0] is overwrite.
     * |        |          |Note: It is cleared by hardware after EADC_DAT register is read.
     * |[17]    |VALID     |Valid Flag
     * |        |          |This bit is set to 1 when corresponding sample module channel analog input conversion is completed and cleared by hardware after EADC_DAT register is read.
     * |        |          |0 = Data in RESULT[11:0] bits is not valid.
     * |        |          |1 = Data in RESULT[11:0] bits is valid.
     * @var EADC_T::CURDAT
     * Offset: 0x4C  EADC PDMA Current Transfer Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |CURDAT    |ADC PDMA Current Transfer Data (Read Only)
     * |        |          |This register is a shadow register of EADC_DATn (n=0~18) for PDMA support.
     * |        |          |This is a read only register.
     * |        |          |NOTE: After PDMA read this register, the VAILD of the shadow EADC_DAT register will be automatically cleared.
     * @var EADC_T::CTL
     * Offset: 0x50  A/D Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADCEN     |A/D Converter Enable Bit
     * |        |          |0 = ADC Disabled.
     * |        |          |1 = ADC Enabled.
     * |        |          |Note: Before starting A/D conversion function, this bit should be set to 1
     * |        |          |Clear it to 0 to disable A/D converter analog circuit power consumption.
     * |[1]     |ADCRST    |ADC A/D Converter Control Circuits Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Cause ADC control circuits reset to initial state, but not change the ADC registers value.
     * |        |          |Note: ADCRST bit remains 1 during ADC reset, when ADC reset end, the ADCRST bit is automatically cleared to 0.
     * |[2]     |ADCIEN0   |Specific Sample Module A/D ADINT0 Interrupt Enable Bit
     * |        |          |The A/D converter generates a conversion end ADIF0 (EADC_STATUS2[0]) upon the end of specific sample module A/D conversion
     * |        |          |If ADCIEN0 bit is set then conversion end interrupt request ADINT0 is generated.
     * |        |          |0 = Specific sample module A/D ADINT0 interrupt function Disabled.
     * |        |          |1 = Specific sample module A/D ADINT0 interrupt function Enabled.
     * |[3]     |ADCIEN1   |Specific Sample Module A/D ADINT1 Interrupt Enable Bit
     * |        |          |The A/D converter generates a conversion end ADIF1 (EADC_STATUS2[1]) upon the end of specific sample module A/D conversion
     * |        |          |If ADCIEN1 bit is set then conversion end interrupt request ADINT1 is generated.
     * |        |          |0 = Specific sample module A/D ADINT1 interrupt function Disabled.
     * |        |          |1 = Specific sample module A/D ADINT1 interrupt function Enabled.
     * |[4]     |ADCIEN2   |Specific Sample Module A/D ADINT2 Interrupt Enable Bit
     * |        |          |The A/D converter generates a conversion end ADIF2 (EADC_STATUS2[2]) upon the end of specific sample module A/D conversion
     * |        |          |If ADCIEN2 bit is set then conversion end interrupt request ADINT2 is generated.
     * |        |          |0 = Specific sample module A/D ADINT2 interrupt function Disabled.
     * |        |          |1 = Specific sample module A/D ADINT2 interrupt function Enabled.
     * |[5]     |ADCIEN3   |Specific Sample Module A/D ADINT3 Interrupt Enable Bit
     * |        |          |The A/D converter generates a conversion end ADIF3 (EADC_STATUS2[3]) upon the end of specific sample module A/D conversion
     * |        |          |If ADCIEN3 bit is set then conversion end interrupt request ADINT3 is generated.
     * |        |          |0 = Specific sample module A/D ADINT3 interrupt function Disabled.
     * |        |          |1 = Specific sample module A/D ADINT3 interrupt function Enabled.
     * @var EADC_T::SWTRG
     * Offset: 0x54  A/D Sample Module Software Start Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |SWTRG     |A/D Sample Module 0~18 Software Force to Start ADC Conversion
     * |        |          |0 = No effect.
     * |        |          |1 = Cause an ADC conversion when the priority is given to sample module.
     * |        |          |Note: After writing this register to start ADC conversion, the EADC_PENDSTS register will show which sample module will conversion
     * |        |          |If user want to disable the conversion of the sample module, user can write EADC_PENDSTS register to clear it.
     * @var EADC_T::PENDSTS
     * Offset: 0x58  A/D Start of Conversion Pending Flag Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |STPF      |A/D Sample Module 0~18 Start of Conversion Pending Flag
     * |        |          |Read:
     * |        |          |0 = There is no pending conversion for sample module.
     * |        |          |1 = Sample module ADC start of conversion is pending.
     * |        |          |Write:
     * |        |          |1 = Clear pending flag and stop conversion for corresponding sample module.
     * |        |          |Note1: This bit remains 1 during pending state, when the respective ADC conversion is end, the STPFn (n=0~18) bit is automatically cleared to 0.
     * |        |          |Note2: After stopping current conversion, the corresponding EADC_DATn (n=0~18) keeps its original value
     * @var EADC_T::OVSTS
     * Offset: 0x5C  A/D Sample Module Start of Conversion Overrun Flag Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |SPOVF     |A/D SAMPLE0~18 Overrun Flag
     * |        |          |0 = No sample module event overrun.
     * |        |          |1 = Indicates a new sample module event is generated while an old one event is pending.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * @var EADC_T::SCTL[16]
     * Offset: 0x80~0xBC  A/D Sample Module 0 ~ 15 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |CHSEL     |A/D Sample Module Channel Selection
     * |        |          |00H = EADC_CH0.
     * |        |          |01H = EADC_CH1.
     * |        |          |02H = EADC_CH2.
     * |        |          |03H = EADC_CH3.
     * |        |          |04H = EADC_CH4.
     * |        |          |05H = EADC_CH5.
     * |        |          |06H = EADC_CH6.
     * |        |          |07H = EADC_CH7.
     * |        |          |08H = EADC_CH8.
     * |        |          |09H = EADC_CH9.
     * |        |          |0AH = EADC_CH10.
     * |        |          |0BH = EADC_CH11.
     * |        |          |0CH = EADC_CH12.
     * |        |          |0DH = EADC_CH13.
     * |        |          |0EH = EADC_CH14.
     * |        |          |0FH = EADC_CH15.
     * |        |          |Note: when internal ADC channel16, 17 or 18 is selected, EADC_CH15 is useless.
     * |[4]     |EXTREN    |A/D External Trigger Rising Edge Enable Bit
     * |        |          |0 = Rising edge Disabled when A/D selects EADC0_ST as trigger source.
     * |        |          |1 = Rising edge Enabled when A/D selects EADC0_ST trigger source.
     * |[5]     |EXTFEN    |A/D External Trigger Falling Edge Enable Bit
     * |        |          |0 = Falling edge Disabled when A/D selects EADC0_ST as trigger source.
     * |        |          |1 = Falling edge Enabled when A/D selects EADC0_ST as trigger source.
     * |[7:6]   |TRGDLYDIV |A/D Sample Module Start of Conversion Trigger Delay Clock Divider Selection
     * |        |          |Trigger delay clock frequency:
     * |        |          |00 = ADC_CLK/1.
     * |        |          |01 = ADC_CLK/2.
     * |        |          |10 = ADC_CLK/4.
     * |        |          |11 = ADC_CLK/16.
     * |[15:8]  |TRGDLYCNT |A/D Sample Module Start of Conversion Trigger Delay Time
     * |        |          |Trigger delay time = TRGDLYCNT x ADC_CLK x n (n=1,2,4,16 from TRGDLYDIV setting).
     * |        |          |Note: If TRGDLYCNT is set to 1, Trigger delay time is actullay the same as TRGDLYCNT is set to 2 for hardware operation.
     * |[20:16] |TRGSEL    |A/D Sample Module Start of Conversion Trigger Source Selection
     * |        |          |0H = Disable trigger.
     * |        |          |1H = External trigger from EADC0_ST pin input.
     * |        |          |2H = ADC ADINT0 interrupt EOC (End of conversion) pulse trigger.
     * |        |          |3H = ADC ADINT1 interrupt EOC (End of conversion) pulse trigger.
     * |        |          |4H = Timer0 overflow pulse trigger.
     * |        |          |5H = Timer1 overflow pulse trigger.
     * |        |          |6H = Timer2 overflow pulse trigger.
     * |        |          |7H = Timer3 overflow pulse trigger.
     * |        |          |8H = PWM0TG0.
     * |        |          |9H = PWM0TG1.
     * |        |          |AH = PWM0TG2.
     * |        |          |BH = PWM0TG3.
     * |        |          |CH = PWM0TG4.
     * |        |          |DH = PWM0TG5.
     * |        |          |EH = PWM1TG0.
     * |        |          |FH = PWM1TG1.
     * |        |          |10H = PWM1TG2.
     * |        |          |11H = PWM1TG3.
     * |        |          |12H = PWM1TG4.
     * |        |          |13H = PWM1TG5.
     * |        |          |14H =BPWM0TG.
     * |        |          |15H =BPWM1TG.
     * |        |          |other = Reserved.
     * |        |          |NOTE: Refer PWM_EADCTS0, PWM_EADCTS1, BPWM_EADCTS0, BPWM_ EADCTS1 and TIMERn_CTL (n=0~3) to get more information for PWM, BPWM trigger and timer trigger.
     * |[22]    |INTPOS    |Interrupt Flag Position Select
     * |        |          |0 = Set ADIFn (EADC_STATUS2[n], n=0~3) at A/D end of conversion.
     * |        |          |1 = Set ADIFn (EADC_STATUS2[n], n=0~3) at A/D start of conversion.
     * |[31:24] |EXTSMPT   |ADC Sampling Time Extend
     * |        |          |When A/D converting at high conversion rate, the sampling time of analog input voltage may not enough if input channel loading is heavy, user can extend A/D sampling time after trigger source is coming to get enough sampling time
     * |        |          |EXTSMPT can be set from 0~8'd251.
     * @var EADC_T::SCTL0[3]
     * Offset: 0xc0~0xC8  A/D Sample Module 16 ~ 18 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PDMAEN    |PDMA Transfer Enable Bit
     * |        |          |When ADC conversion is completed, the converted data is loaded into EADC_DATn (n: 16 ~ 18) register, user can enable this bit to generate a PDMA data transfer request.
     * |        |          |0 = PDMA data transfer Disabled.
     * |        |          |1 = PDMA data transfer Enabled.
     * |        |          |Note: When set this bit field to 1, user must set ADCIENn (EADC_CTL[5:2], n=0~3) = 0 to disable interrupt.
     * |[31:24] |EXTSMPT   |ADC Sampling Time Extend
     * |        |          |When A/D converting at high conversion rate, the sampling time of analog input voltage may not enough if input channel loading is heavy, user can extend A/D sampling time after trigger source is coming to get enough sampling time
     * |        |          |EXTSMPT can be set from 0~8'd251.
     * @var EADC_T::INTSRC[4]
     * Offset: 0xD0~0xDC  ADC Interrupt 0~4 Source Enable Control Register.
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SPLIE0    |Sample Module 0 Interrupt Enable Bit
     * |        |          |0 = Sample Module 0 interrupt Disabled.
     * |        |          |1 = Sample Module 0 interrupt Enabled.
     * |[1]     |SPLIE1    |Sample Module 1 Interrupt Enable Bit
     * |        |          |0 = Sample Module 1 interrupt Disabled.
     * |        |          |1 = Sample Module 1 interrupt Enabled.
     * |[2]     |SPLIE2    |Sample Module 2 Interrupt Enable Bit
     * |        |          |0 = Sample Module 2 interrupt Disabled.
     * |        |          |1 = Sample Module 2 interrupt Enabled.
     * |[3]     |SPLIE3    |Sample Module 3 Interrupt Enable Bit
     * |        |          |0 = Sample Module 3 interrupt Disabled.
     * |        |          |1 = Sample Module 3 interrupt Enabled.
     * |[4]     |SPLIE4    |Sample Module 4 Interrupt Enable Bit
     * |        |          |0 = Sample Module 4 interrupt Disabled.
     * |        |          |1 = Sample Module 4 interrupt Enabled.
     * |[5]     |SPLIE5    |Sample Module 5 Interrupt Enable Bit
     * |        |          |0 = Sample Module 5 interrupt Disabled.
     * |        |          |1 = Sample Module 5 interrupt Enabled.
     * |[6]     |SPLIE6    |Sample Module 6 Interrupt Enable Bit
     * |        |          |0 = Sample Module 6 interrupt Disabled.
     * |        |          |1 = Sample Module 6 interrupt Enabled.
     * |[7]     |SPLIE7    |Sample Module 7 Interrupt Enable Bit
     * |        |          |0 = Sample Module 7 interrupt Disabled.
     * |        |          |1 = Sample Module 7 interrupt Enabled.
     * |[8]     |SPLIE8    |Sample Module 8 Interrupt Enable Bit
     * |        |          |0 = Sample Module 8 interrupt Disabled.
     * |        |          |1 = Sample Module 8 interrupt Enabled.
     * |[9]     |SPLIE9    |Sample Module 9 Interrupt Enable Bit
     * |        |          |0 = Sample Module 9 interrupt Disabled.
     * |        |          |1 = Sample Module 9 interrupt Enabled.
     * |[10]    |SPLIE10   |Sample Module 10 Interrupt Enable Bit
     * |        |          |0 = Sample Module 10 interrupt Disabled.
     * |        |          |1 = Sample Module 10 interrupt Enabled.
     * |[11]    |SPLIE11   |Sample Module 11 Interrupt Enable Bit
     * |        |          |0 = Sample Module 11 interrupt Disabled.
     * |        |          |1 = Sample Module 11 interrupt Enabled.
     * |[12]    |SPLIE12   |Sample Module 12 Interrupt Enable Bit
     * |        |          |0 = Sample Module 12 interrupt Disabled.
     * |        |          |1 = Sample Module 12 interrupt Enabled.
     * |[13]    |SPLIE13   |Sample Module 13 Interrupt Enable Bit
     * |        |          |0 = Sample Module 13 interrupt Disabled.
     * |        |          |1 = Sample Module 13 interrupt Enabled.
     * |[14]    |SPLIE14   |Sample Module 14 Interrupt Enable Bit
     * |        |          |0 = Sample Module 14 interrupt Disabled.
     * |        |          |1 = Sample Module 14 interrupt Enabled.
     * |[15]    |SPLIE15   |Sample Module 15 Interrupt Enable Bit
     * |        |          |0 = Sample Module 15 interrupt Disabled.
     * |        |          |1 = Sample Module 15 interrupt Enabled.
     * |[16]    |SPLIE16   |Sample Module 16 Interrupt Enable Bit
     * |        |          |0 = Sample Module 16 interrupt Disabled.
     * |        |          |1 = Sample Module 16 interrupt Enabled.
     * |[17]    |SPLIE17   |Sample Module 17 Interrupt Enable Bit
     * |        |          |0 = Sample Module 17 interrupt Disabled.
     * |        |          |1 = Sample Module 17 interrupt Enabled.
     * |[18]    |SPLIE18   |Sample Module 18 Interrupt Enable Bit
     * |        |          |0 = Sample Module 18 interrupt Disabled.
     * |        |          |1 = Sample Module 18 interrupt Enabled.
     * @var EADC_T::CMP[4]
     * Offset: 0xE0~0xEC  A/D Result Compare Register 0 ~ 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADCMPEN   |A/D Result Compare Enable Bit
     * |        |          |0 = Compare Disabled.
     * |        |          |1 = Compare Enabled.
     * |        |          |Set this bit to 1 to enable compare CMPDAT (EADC_CMPn[27:16], n=0~3) with specified sample module conversion result when converted data is loaded into EADC_DAT register.
     * |[1]     |ADCMPIE   |A/D Result Compare Interrupt Enable Bit
     * |        |          |0 = Compare function interrupt Disabled.
     * |        |          |1 = Compare function interrupt Enabled.
     * |        |          |If the compare function is enabled and the compare condition matches the setting of CMPCOND (EADC_CMPn[2], n=0~3) and CMPMCNT (EADC_CMPn[11:8], n=0~3), ADCMPFn (EADC_STATUS2[7:4], n=0~3) will be asserted, in the meanwhile, if ADCMPIE is set to 1, a compare interrupt request is generated.
     * |[2]     |CMPCOND   |Compare Condition
     * |        |          |0= Set the compare condition as that when a 12-bit A/D conversion result is less than the 12-bit CMPDAT (EADC_CMPn [27:16]), the internal match counter will increase one.
     * |        |          |1= Set the compare condition as that when a 12-bit A/D conversion result is greater or equal to the 12-bit CMPDAT (EADC_CMPn [27:16]), the internal match counter will increase one.
     * |        |          |Note: When the internal counter reaches the value to (CMPMCNT (EADC_CMPn[11:8], n=0~3) +1), the ADCMPFn bit will be set.
     * |[7:3]   |CMPSPL    |Compare Sample Module Selection
     * |        |          |00000 = Sample Module 0 conversion result EADC_DAT0 is selected to be compared.
     * |        |          |00001 = Sample Module 1 conversion result EADC_DAT1 is selected to be compared.
     * |        |          |00010 = Sample Module 2 conversion result EADC_DAT2 is selected to be compared.
     * |        |          |00011 = Sample Module 3 conversion result EADC_DAT3 is selected to be compared.
     * |        |          |00100 = Sample Module 4 conversion result EADC_DAT4 is selected to be compared.
     * |        |          |00101 = Sample Module 5 conversion result EADC_DAT5 is selected to be compared.
     * |        |          |00110 = Sample Module 6 conversion result EADC_DAT6 is selected to be compared.
     * |        |          |00111 = Sample Module 7 conversion result EADC_DAT7 is selected to be compared.
     * |        |          |01000 = Sample Module 8 conversion result EADC_DAT8 is selected to be compared.
     * |        |          |01001 = Sample Module 9 conversion result EADC_DAT9 is selected to be compared.
     * |        |          |01010 = Sample Module 10 conversion result EADC_DAT10 is selected to be compared.
     * |        |          |01011 = Sample Module 11 conversion result EADC_DAT11 is selected to be compared.
     * |        |          |01100 = Sample Module 12 conversion result EADC_DAT12 is selected to be compared.
     * |        |          |01101 = Sample Module 13 conversion result EADC_DAT13 is selected to be compared.
     * |        |          |01110 = Sample Module 14 conversion result EADC_DAT14 is selected to be compared.
     * |        |          |01111 = Sample Module 15 conversion result EADC_DAT15 is selected to be compared.
     * |        |          |10000 = Sample Module 16 conversion result EADC_DAT16 is selected to be compared.
     * |        |          |10001 = Sample Module 17 conversion result EADC_DAT17 is selected to be compared.
     * |        |          |10010 = Sample Module 18 conversion result EADC_DAT18 is selected to be compared.
     * |[11:8]  |CMPMCNT   |Compare Match Count
     * |        |          |When the specified A/D sample module analog conversion result matches the compare condition defined by CMPCOND (EADC_CMPn[2], n=0~3), the internal match counter will increase 1
     * |        |          |If the compare result does not meet the compare condition, the internal compare match counter will reset to 0
     * |        |          |When the internal counter reaches the value to (CMPMCNT +1), the ADCMPFn (EADC_STATUS2[7:4], n=0~3) will be set.
     * |[15]    |CMPWEN    |Compare Window Mode Enable Bit
     * |        |          |0 = ADCMPF0 (EADC_STATUS2[4]) will be set when EADC_CMP0 compared condition matched
     * |        |          |ADCMPF2 (EADC_STATUS2[6]) will be set when EADC_CMP2 compared condition matched
     * |        |          |1 = ADCMPF0 (EADC_STATUS2[4]) will be set when both EADC_CMP0 and EADC_CMP1 compared condition matched
     * |        |          |ADCMPF2 (EADC_STATUS2[6]) will be set when both EADC_CMP2 and EADC_CMP3 compared condition matched.
     * |        |          |Note: This bit is only present in EADC_CMP0 and EADC_CMP2 register.
     * |[27:16] |CMPDAT    |Comparison Data
     * |        |          |The 12 bits data is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage transition without imposing a load on software.
     * @var EADC_T::STATUS0
     * Offset: 0xF0  A/D Status Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |VALID     |EADC_DAT0~15 Data Valid Flag
     * |        |          |It is a mirror of VALID bit in sample module A/D result data register EADC_DATn. (n=0~15).
     * |[31:16] |OV        |EADC_DAT0~15 Overrun Flag
     * |        |          |It is a mirror to OV bit in sample module A/D result data register EADC_DATn. (n=0~15).
     * @var EADC_T::STATUS1
     * Offset: 0xF4  A/D Status Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |VALID     |EADC_DAT16~18 Data Valid Flag
     * |        |          |It is a mirror of VALID bit in sample module A/D result data register EADC_DATn. (n=16~18).
     * |[18:16] |OV        |EADC_DAT16~18 Overrun Flag
     * |        |          |It is a mirror to OV bit in sample module A/D result data register EADC_DATn. (n=16~18).
     * @var EADC_T::STATUS2
     * Offset: 0xF8  A/D Status Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADIF0     |A/D ADINT0 Interrupt Flag
     * |        |          |0 = No ADINT0 interrupt pulse received.
     * |        |          |1 = ADINT0 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an A/D conversion of specific sample module has been completed
     * |[1]     |ADIF1     |A/D ADINT1 Interrupt Flag
     * |        |          |0 = No ADINT1 interrupt pulse received.
     * |        |          |1 = ADINT1 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an A/D conversion of specific sample module has been completed
     * |[2]     |ADIF2     |A/D ADINT2 Interrupt Flag
     * |        |          |0 = No ADINT2 interrupt pulse received.
     * |        |          |1 = ADINT2 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an A/D conversion of specific sample module has been completed
     * |[3]     |ADIF3     |A/D ADINT3 Interrupt Flag
     * |        |          |0 = No ADINT3 interrupt pulse received.
     * |        |          |1 = ADINT3 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an A/D conversion of specific sample module has been completed
     * |[4]     |ADCMPF0   |ADC Compare 0 Flag
     * |        |          |When the specific sample module A/D conversion result meets setting condition in EADC_CMP0 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP0 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP0 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[5]     |ADCMPF1   |ADC Compare 1 Flag
     * |        |          |When the specific sample module A/D conversion result meets setting condition in EADC_CMP1 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP1 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP1 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[6]     |ADCMPF2   |ADC Compare 2 Flag
     * |        |          |When the specific sample module A/D conversion result meets setting condition in EADC_CMP2 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP2 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP2 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[7]     |ADCMPF3   |ADC Compare 3 Flag
     * |        |          |When the specific sample module A/D conversion result meets setting condition in EADC_CMP3 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP3 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP3 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[8]     |ADOVIF0   |A/D ADINT0 Interrupt Flag Overrun
     * |        |          |0 = ADINT0 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT0 interrupt flag is overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[9]     |ADOVIF1   |A/D ADINT1 Interrupt Flag Overrun
     * |        |          |0 = ADINT1 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT1 interrupt flag is overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[10]    |ADOVIF2   |A/D ADINT2 Interrupt Flag Overrun
     * |        |          |0 = ADINT2 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT2 interrupt flag is overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[11]    |ADOVIF3   |A/D ADINT3 Interrupt Flag Overrun
     * |        |          |0 = ADINT3 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT3 interrupt flag is overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[12]    |ADCMPO0   |ADC Compare 0 Output Status
     * |        |          |The 12 bits compare0 data CMPDAT0 (EADC_CMP0[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT0 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT0 setting.
     * |[13]    |ADCMPO1   |ADC Compare 1 Output Status
     * |        |          |The 12 bits compare1 data CMPDAT1 (EADC_CMP1[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT1 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT1 setting.
     * |[14]    |ADCMPO2   |ADC Compare 2 Output Status
     * |        |          |The 12 bits compare2 data CMPDAT2 (EADC_CMP2[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT2 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT2 setting.
     * |[15]    |ADCMPO3   |ADC Compare 3 Output Status
     * |        |          |The 12 bits compare3 data CMPDAT3 (EADC_CMP3[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT3 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT3 setting.
     * |[20:16] |CHANNEL   |Current Conversion Channel
     * |        |          |This filed reflects ADC current conversion channel when BUSY=1.
     * |        |          |It is read only.
     * |        |          |00H = EADC_CH0.
     * |        |          |01H = EADC_CH1.
     * |        |          |02H = EADC_CH2.
     * |        |          |03H = EADC_CH3.
     * |        |          |04H = EADC_CH4.
     * |        |          |05H = EADC_CH5.
     * |        |          |06H = EADC_CH6.
     * |        |          |07H = EADC_CH7.
     * |        |          |08H = EADC_CH8.
     * |        |          |09H = EADC_CH9.
     * |        |          |0AH = EADC_CH10.
     * |        |          |0BH = EADC_CH11.
     * |        |          |0CH = EADC_CH12.
     * |        |          |0DH = EADC_CH13.
     * |        |          |0EH = EADC_CH14.
     * |        |          |0FH = EADC_CH15.
     * |        |          |10H = VBG.
     * |        |          |11H = VTEMP.
     * |        |          |12H = VBAT/4.
     * |        |          |Note: These bit are read only.
     * |[23]    |BUSY      |A/D Conveter Busy/Idle Status
     * |        |          |0 = EADC is in idle state.
     * |        |          |1 = EADC is busy for sample or conversion.
     * |        |          |Note: This bit is read only
     * |        |          |Once trigger source is coming, it must wait 2 ADC_CLK synchronization then the BUSY status will be high
     * |        |          |This status will be high to low when the current conversion done.
     * |[24]    |ADOVIF    |All A/D Interrupt Flag Overrun Bits Check
     * |        |          |n=0~3.
     * |        |          |0 = None of ADINT interrupt flag ADOVIFn (EADC_STATUS2[11:8]) is overwritten to 1.
     * |        |          |1 = Any one of ADINT interrupt flag ADOVIFn (EADC_STATUS2[11:8]) is overwritten to 1.
     * |        |          |Note: This bit will keep 1 when any ADOVIFn Flag is equal to 1.
     * |[25]    |STOVF     |for All A/D Sample Module Start of Conversion Overrun Flags Check
     * |        |          |n=0~18.
     * |        |          |0 = None of sample module event overrun flag SPOVFn (EADC_OVSTS[n]) is set to 1.
     * |        |          |1 = Any one of sample module event overrun flag SPOVFn (EADC_OVSTS[n]) is set to 1.
     * |        |          |Note: This bit will keep 1 when any SPOVFn Flag is equal to 1.
     * |[26]    |AVALID    |for All Sample Module A/D Result Data Register EADC_DAT Data Valid Flag Check
     * |        |          |n=0~18.
     * |        |          |0 = None of sample module data register valid flag VALIDn (EADC_DATn[17]) is set to 1.
     * |        |          |1 = Any one of sample module data register valid flag VALIDn (EADC_DATn[17]) is set to 1.
     * |        |          |Note: This bit will keep 1 when any VALIDn Flag is equal to 1.
     * |[27]    |AOV       |for All Sample Module A/D Result Data Register Overrun Flags Check
     * |        |          |n=0~18.
     * |        |          |0 = None of sample module data register overrun flag OVn (EADC_DATn[16]) is set to 1.
     * |        |          |1 = Any one of sample module data register overrun flag OVn (EADC_DATn[16]) is set to 1.
     * |        |          |Note: This bit will keep 1 when any OVn Flag is equal to 1.
     * @var EADC_T::STATUS3
     * Offset: 0xFC  A/D Status Register 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4:0]   |CURSPL    |ADC Current Sample Module
     * |        |          |This register show the current ADC is controlled by which sample module control logic modules.
     * |        |          |If the ADC is Idle, this bit filed will set to 0x1F.
     * |        |          |Note: This is a read only register.
     * @var EADC_T::PWRCTL
     * Offset: 0x110  ADC Power Management Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |READY     |ADC Start-up Completely and Ready for Conversion (Read Only)
     * |        |          |0 = Power-on sequence is still in progress.
     * |        |          |1 = ADC is ready for conversion.
     * |[5]     |AUTOFF    |Auto Off Mode
     * |        |          |0 = Function of auto off disabled.
     * |        |          |1 = Function of auto off enabled
     * |        |          |When AUTOFF is set to 1, ADC will be power off automatically to save power
     * |[19:8]  |STUPT     |ADC Start-up Time
     * |        |          |Set this bit fields to adjust start-up time. The minimum start-up time of ADC is 10us.
     * |        |          |ADC start-up time = (1/ADC_CLK) x STUPT.
     * |[23:20] |AUTOPDTHT |Auto Power Down Threshold Time
     * |        |          |Auto Power Down Threshold Time = (1/ADC_CLK) x AUTOPDTHT.
     * |        |          |0111 = 8 ADC clock for power down threshold time.
     * |        |          |1000 = 16 ADC clock for power down threshold time.
     * |        |          |1001 = 32 ADC clock for power down threshold time.
     * |        |          |1010 = 64 ADC clock for power down threshold time.
     * |        |          |1011 = 128 ADC clock for power down threshold time.
     * |        |          |1100 = 256 ADC clock for power down threshold time.
     * |        |          |Others = 256 ADC clock for power down threshold time.
     * @var EADC_T::PDMACTL
     * Offset: 0x130  ADC PDMA Control Rgister
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |PDMATEN   |PDMA Transfer Enable Bit
     * |        |          |When ADC conversion is completed, the converted data is loaded into EADC_DATn (n: 0 ~ 18) register, user can enable this bit to generate a PDMA data transfer request.
     * |        |          |0 = PDMA data transfer Disabled.
     * |        |          |1 = PDMA data transfer Enabled.
     * |        |          |Note: When set this bit field to 1, user must set ADCIENn (EADC_CTL[5:2], n=0~3) = 0 to disable interrupt.
     * @var EADC_T::MCTL1[16]
     * Offset: 0x140~0x17C  A/D Sample Module 0~15 Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ALIGN     |Alignment Selection
     * |        |          |0 = The conversion result will be right aligned in data register.
     * |        |          |1 = The conversion result will be left aligned in data register.
     * |[1]     |AVG       |Average Mode Selection
     * |        |          |0 = Conversion results will be stored in data register without averaging.
     * |        |          |1 = Conversion results in data register will be averaged.
     * |        |          |This bit needs to work with ACU (EADC_MnCTL1[7:4], n=0~15).
     * |[7:4]   |ACU       |Number of Accumulated Conversion Results Selection
     * |        |          |0H = 1 conversion result will be accumulated.
     * |        |          |1H = 2 conversion result will be accumulated.
     * |        |          |2H = 4 conversion result will be accumulated.
     * |        |          |3H = 8 conversion result will be accumulated.
     * |        |          |4H = 16 conversion result will be accumulated.
     * |        |          |5H = 32 conversion result will be accumulated.
     * |        |          |6H = 64 conversion result will be accumulated.
     * |        |          |7H = 128 conversion result will be accumulated.
     * |        |          |8H = 256 conversion result will be accumulated.
     * @var EADC_T::OFFSETCAL
     * Offset: 0x208  A/D Result Offset Cancellation Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4:0]   |OFFSETCANCEL|A/D Offset Cancellation Trim Bits
     * |        |          |When CALEN(EADC_CTL[8]) is set to 1, the offset cancellation trim bits will compensate A/D result offset
     * |        |          |When this bit is set to 0, the offset cancellation trim bits have no effect to A/D result.
     * |        |          |Note:These 5 bits trim value wouln't latched into EADC_OFFSETCAL automatically when flash initalization
     * |        |          |User must read DCR2 by ISP command first, then write the value to OFFSETCANCEL.
     * |        |          |Note: OFFSETCANCEL is signed format
     * |        |          |OFFSETCANCEL will sign extension to 12 bit by hardware to perform signed addition with ADC conversion result if CALEN is enabled.
     */
    __I  uint32_t DAT[19];               /*!< [0x0000~0x0048] A/D Data Register n for Sample Module n,  n=0~18          */
    __I  uint32_t CURDAT;                /*!< [0x004c] EADC PDMA Current Transfer Data Register                         */
    __IO uint32_t CTL;                   /*!< [0x0050] A/D Control Register                                             */
    __O  uint32_t SWTRG;                 /*!< [0x0054] A/D Sample Module Software Start Register                        */
    __IO uint32_t PENDSTS;               /*!< [0x0058] A/D Start of Conversion Pending Flag Register                    */
    __IO uint32_t OVSTS;                 /*!< [0x005c] A/D Sample Module Start of Conversion Overrun Flag Register      */
    __I  uint32_t RESERVE0[8];
    __IO uint32_t SCTL[16];              /*!< [0x0080~0x00bc] A/D Sample Module n Control Register n=0~15               */
    __IO uint32_t SCTL0[3];              /*!< [0x00c0~0x00c8] A/D Sample Module n Control Register n=16~18              */
    __I  uint32_t RESERVE1[1];
    __IO uint32_t INTSRC[4];             /*!< [0x00d0~0x00dc] ADC Interrupt n Source Enable Control Register. n=0~3     */
    __IO uint32_t CMP[4];                /*!< [0x00e0~0x00ec] A/D Result Compare Register n, n=0~3                      */
    __I  uint32_t STATUS0;               /*!< [0x00f0] A/D Status Register 0                                            */
    __I  uint32_t STATUS1;               /*!< [0x00f4] A/D Status Register 1                                            */
    __IO uint32_t STATUS2;               /*!< [0x00f8] A/D Status Register 2                                            */
    __I  uint32_t STATUS3;               /*!< [0x00fc] A/D Status Register 3                                            */
    __I  uint32_t RESERVE2[4];
    __IO uint32_t PWRCTL;                /*!< [0x0110] ADC Power Management Control Register                            */
    __I  uint32_t RESERVE3[7];
    __IO uint32_t PDMACTL;               /*!< [0x0130] ADC PDMA Control Rgister                                         */
    __I  uint32_t RESERVE4[3];
    __IO uint32_t MCTL1[16];             /*!< [0x0140~0x017c] A/D Sample Module n Control Register 1, n=0~15            */
    __I  uint32_t RESERVE5[34];
    __IO uint32_t OFFSETCAL;             /*!< [0x0208] A/D Result Offset Cancellation Register                          */


} EADC_T;

/**
    @addtogroup EADC_CONST EADC Bit Field Definition
    Constant Definitions for EADC Controller
@{ */

#define EADC_DAT_RESULT_Pos              (0)                                               /*!< EADC_T::DATn: RESULT Position          */
#define EADC_DAT_RESULT_Msk              (0xfffful << EADC_DAT_RESULT_Pos)                 /*!< EADC_T::DATn: RESULT Mask              */

#define EADC_DAT_OV_Pos                  (16)                                              /*!< EADC_T::DATn: OV Position              */
#define EADC_DAT_OV_Msk                  (0x1ul << EADC_DAT_OV_Pos)                        /*!< EADC_T::DATn: OV Mask                  */

#define EADC_DAT_VALID_Pos               (17)                                              /*!< EADC_T::DATn: VALID Position           */
#define EADC_DAT_VALID_Msk               (0x1ul << EADC_DAT_VALID_Pos)                     /*!< EADC_T::DATn: VALID Mask               */

#define EADC_CURDAT_CURDAT_Pos           (0)                                               /*!< EADC_T::CURDAT: CURDAT Position        */
#define EADC_CURDAT_CURDAT_Msk           (0x3fffful << EADC_CURDAT_CURDAT_Pos)             /*!< EADC_T::CURDAT: CURDAT Mask            */

#define EADC_CTL_ADCEN_Pos               (0)                                               /*!< EADC_T::CTL: ADCEN Position            */
#define EADC_CTL_ADCEN_Msk               (0x1ul << EADC_CTL_ADCEN_Pos)                     /*!< EADC_T::CTL: ADCEN Mask                */

#define EADC_CTL_ADCRST_Pos              (1)                                               /*!< EADC_T::CTL: ADCRST Position           */
#define EADC_CTL_ADCRST_Msk              (0x1ul << EADC_CTL_ADCRST_Pos)                    /*!< EADC_T::CTL: ADCRST Mask               */

#define EADC_CTL_ADCIEN0_Pos             (2)                                               /*!< EADC_T::CTL: ADCIEN0 Position          */
#define EADC_CTL_ADCIEN0_Msk             (0x1ul << EADC_CTL_ADCIEN0_Pos)                   /*!< EADC_T::CTL: ADCIEN0 Mask              */

#define EADC_CTL_ADCIEN1_Pos             (3)                                               /*!< EADC_T::CTL: ADCIEN1 Position          */
#define EADC_CTL_ADCIEN1_Msk             (0x1ul << EADC_CTL_ADCIEN1_Pos)                   /*!< EADC_T::CTL: ADCIEN1 Mask              */

#define EADC_CTL_ADCIEN2_Pos             (4)                                               /*!< EADC_T::CTL: ADCIEN2 Position          */
#define EADC_CTL_ADCIEN2_Msk             (0x1ul << EADC_CTL_ADCIEN2_Pos)                   /*!< EADC_T::CTL: ADCIEN2 Mask              */

#define EADC_CTL_ADCIEN3_Pos             (5)                                               /*!< EADC_T::CTL: ADCIEN3 Position          */
#define EADC_CTL_ADCIEN3_Msk             (0x1ul << EADC_CTL_ADCIEN3_Pos)                   /*!< EADC_T::CTL: ADCIEN3 Mask              */

#define EADC_CTL_CALEN_Pos               (8)                                               /*!< EADC_T::CTL: CALEN Position            */
#define EADC_CTL_CALEN_Msk               (0x1ul << EADC_CTL_CALEN_Pos)                     /*!< EADC_T::CTL: CALEN Mask                */

#define EADC_SWTRG_SWTRG_Pos             (0)                                               /*!< EADC_T::SWTRG: SWTRG Position          */
#define EADC_SWTRG_SWTRG_Msk             (0x7fffful << EADC_SWTRG_SWTRG_Pos)               /*!< EADC_T::SWTRG: SWTRG Mask              */

#define EADC_PENDSTS_STPF_Pos            (0)                                               /*!< EADC_T::PENDSTS: STPF Position         */
#define EADC_PENDSTS_STPF_Msk            (0x7fffful << EADC_PENDSTS_STPF_Pos)              /*!< EADC_T::PENDSTS: STPF Mask             */

#define EADC_OVSTS_SPOVF_Pos             (0)                                               /*!< EADC_T::OVSTS: SPOVF Position          */
#define EADC_OVSTS_SPOVF_Msk             (0x7fffful << EADC_OVSTS_SPOVF_Pos)               /*!< EADC_T::OVSTS: SPOVF Mask              */

#define EADC_SCTL_CHSEL_Pos              (0)                                               /*!< EADC_T::SCTLn: CHSEL Position          */
#define EADC_SCTL_CHSEL_Msk              (0xful << EADC_SCTL_CHSEL_Pos)                    /*!< EADC_T::SCTLn: CHSEL Mask              */

#define EADC_SCTL_EXTREN_Pos             (4)                                               /*!< EADC_T::SCTLn: EXTREN Position         */
#define EADC_SCTL_EXTREN_Msk             (0x1ul << EADC_SCTL_EXTREN_Pos)                   /*!< EADC_T::SCTLn: EXTREN Mask             */

#define EADC_SCTL_EXTFEN_Pos             (5)                                               /*!< EADC_T::SCTLn: EXTFEN Position         */
#define EADC_SCTL_EXTFEN_Msk             (0x1ul << EADC_SCTL_EXTFEN_Pos)                   /*!< EADC_T::SCTLn: EXTFEN Mask             */

#define EADC_SCTL_TRGDLYDIV_Pos          (6)                                               /*!< EADC_T::SCTLn: TRGDLYDIV Position      */
#define EADC_SCTL_TRGDLYDIV_Msk          (0x3ul << EADC_SCTL_TRGDLYDIV_Pos)                /*!< EADC_T::SCTLn: TRGDLYDIV Mask          */

#define EADC_SCTL_TRGDLYCNT_Pos          (8)                                               /*!< EADC_T::SCTLn: TRGDLYCNT Position      */
#define EADC_SCTL_TRGDLYCNT_Msk          (0xfful << EADC_SCTL_TRGDLYCNT_Pos)               /*!< EADC_T::SCTLn: TRGDLYCNT Mask          */

#define EADC_SCTL_TRGSEL_Pos             (16)                                              /*!< EADC_T::SCTLn: TRGSEL Position         */
#define EADC_SCTL_TRGSEL_Msk             (0x1ful << EADC_SCTL_TRGSEL_Pos)                  /*!< EADC_T::SCTLn: TRGSEL Mask             */

#define EADC_SCTL_INTPOS_Pos             (22)                                              /*!< EADC_T::SCTLn: INTPOS Position         */
#define EADC_SCTL_INTPOS_Msk             (0x1ul << EADC_SCTL_INTPOS_Pos)                   /*!< EADC_T::SCTLn: INTPOS Mask             */

#define EADC_SCTL_EXTSMPT_Pos            (24)                                              /*!< EADC_T::SCTLn: EXTSMPT Position        */
#define EADC_SCTL_EXTSMPT_Msk            (0xfful << EADC_SCTL_EXTSMPT_Pos)                 /*!< EADC_T::SCTLn: EXTSMPT Mask            */

#define EADC_SCTL0_EXTSMPT_Pos           (24)                                              /*!< EADC_T::SCTL0n: EXTSMPT Position       */
#define EADC_SCTL0_EXTSMPT_Msk           (0xfful << EADC_SCTL0_EXTSMPT_Pos)                /*!< EADC_T::SCTL0n: EXTSMPT Mask           */

#define EADC_INTSRC_SPLIE0_Pos           (0)                                               /*!< EADC_T::INTSRCn: SPLIE0 Position       */
#define EADC_INTSRC_SPLIE0_Msk           (0x1ul << EADC_INTSRC_SPLIE0_Pos)                 /*!< EADC_T::INTSRCn: SPLIE0 Mask           */

#define EADC_INTSRC_SPLIE1_Pos           (1)                                               /*!< EADC_T::INTSRCn: SPLIE1 Position       */
#define EADC_INTSRC_SPLIE1_Msk           (0x1ul << EADC_INTSRC_SPLIE1_Pos)                 /*!< EADC_T::INTSRCn: SPLIE1 Mask           */

#define EADC_INTSRC_SPLIE2_Pos           (2)                                               /*!< EADC_T::INTSRCn: SPLIE2 Position       */
#define EADC_INTSRC_SPLIE2_Msk           (0x1ul << EADC_INTSRC_SPLIE2_Pos)                 /*!< EADC_T::INTSRCn: SPLIE2 Mask           */

#define EADC_INTSRC_SPLIE3_Pos           (3)                                               /*!< EADC_T::INTSRCn: SPLIE3 Position       */
#define EADC_INTSRC_SPLIE3_Msk           (0x1ul << EADC_INTSRC_SPLIE3_Pos)                 /*!< EADC_T::INTSRCn: SPLIE3 Mask           */

#define EADC_INTSRC_SPLIE4_Pos           (4)                                               /*!< EADC_T::INTSRCn: SPLIE4 Position       */
#define EADC_INTSRC_SPLIE4_Msk           (0x1ul << EADC_INTSRC_SPLIE4_Pos)                 /*!< EADC_T::INTSRCn: SPLIE4 Mask           */

#define EADC_INTSRC_SPLIE5_Pos           (5)                                               /*!< EADC_T::INTSRCn: SPLIE5 Position       */
#define EADC_INTSRC_SPLIE5_Msk           (0x1ul << EADC_INTSRC_SPLIE5_Pos)                 /*!< EADC_T::INTSRCn: SPLIE5 Mask           */

#define EADC_INTSRC_SPLIE6_Pos           (6)                                               /*!< EADC_T::INTSRCn: SPLIE6 Position       */
#define EADC_INTSRC_SPLIE6_Msk           (0x1ul << EADC_INTSRC_SPLIE6_Pos)                 /*!< EADC_T::INTSRCn: SPLIE6 Mask           */

#define EADC_INTSRC_SPLIE7_Pos           (7)                                               /*!< EADC_T::INTSRCn: SPLIE7 Position       */
#define EADC_INTSRC_SPLIE7_Msk           (0x1ul << EADC_INTSRC_SPLIE7_Pos)                 /*!< EADC_T::INTSRCn: SPLIE7 Mask           */

#define EADC_INTSRC_SPLIE8_Pos           (8)                                               /*!< EADC_T::INTSRCn: SPLIE8 Position       */
#define EADC_INTSRC_SPLIE8_Msk           (0x1ul << EADC_INTSRC_SPLIE8_Pos)                 /*!< EADC_T::INTSRCn: SPLIE8 Mask           */

#define EADC_INTSRC_SPLIE9_Pos           (9)                                               /*!< EADC_T::INTSRCn: SPLIE9 Position       */
#define EADC_INTSRC_SPLIE9_Msk           (0x1ul << EADC_INTSRC_SPLIE9_Pos)                 /*!< EADC_T::INTSRCn: SPLIE9 Mask           */

#define EADC_INTSRC_SPLIE10_Pos          (10)                                              /*!< EADC_T::INTSRCn: SPLIE10 Position      */
#define EADC_INTSRC_SPLIE10_Msk          (0x1ul << EADC_INTSRC_SPLIE10_Pos)                /*!< EADC_T::INTSRCn: SPLIE10 Mask          */

#define EADC_INTSRC_SPLIE11_Pos          (11)                                              /*!< EADC_T::INTSRCn: SPLIE11 Position      */
#define EADC_INTSRC_SPLIE11_Msk          (0x1ul << EADC_INTSRC_SPLIE11_Pos)                /*!< EADC_T::INTSRCn: SPLIE11 Mask          */

#define EADC_INTSRC_SPLIE12_Pos          (12)                                              /*!< EADC_T::INTSRCn: SPLIE12 Position      */
#define EADC_INTSRC_SPLIE12_Msk          (0x1ul << EADC_INTSRC_SPLIE12_Pos)                /*!< EADC_T::INTSRCn: SPLIE12 Mask          */

#define EADC_INTSRC_SPLIE13_Pos          (13)                                              /*!< EADC_T::INTSRCn: SPLIE13 Position      */
#define EADC_INTSRC_SPLIE13_Msk          (0x1ul << EADC_INTSRC_SPLIE13_Pos)                /*!< EADC_T::INTSRCn: SPLIE13 Mask          */

#define EADC_INTSRC_SPLIE14_Pos          (14)                                              /*!< EADC_T::INTSRCn: SPLIE14 Position      */
#define EADC_INTSRC_SPLIE14_Msk          (0x1ul << EADC_INTSRC_SPLIE14_Pos)                /*!< EADC_T::INTSRCn: SPLIE14 Mask          */

#define EADC_INTSRC_SPLIE15_Pos          (15)                                              /*!< EADC_T::INTSRCn: SPLIE15 Position      */
#define EADC_INTSRC_SPLIE15_Msk          (0x1ul << EADC_INTSRC_SPLIE15_Pos)                /*!< EADC_T::INTSRCn: SPLIE15 Mask          */

#define EADC_INTSRC_SPLIE16_Pos          (16)                                              /*!< EADC_T::INTSRCn: SPLIE16 Position      */
#define EADC_INTSRC_SPLIE16_Msk          (0x1ul << EADC_INTSRC_SPLIE16_Pos)                /*!< EADC_T::INTSRCn: SPLIE16 Mask          */

#define EADC_INTSRC_SPLIE17_Pos          (17)                                              /*!< EADC_T::INTSRCn: SPLIE17 Position      */
#define EADC_INTSRC_SPLIE17_Msk          (0x1ul << EADC_INTSRC_SPLIE17_Pos)                /*!< EADC_T::INTSRCn: SPLIE17 Mask          */

#define EADC_INTSRC_SPLIE18_Pos          (18)                                              /*!< EADC_T::INTSRCn: SPLIE18 Position      */
#define EADC_INTSRC_SPLIE18_Msk          (0x1ul << EADC_INTSRC_SPLIE18_Pos)                /*!< EADC_T::INTSRCn: SPLIE18 Mask          */

#define EADC_CMP_ADCMPEN_Pos             (0)                                               /*!< EADC_T::CMPn: ADCMPEN Position         */
#define EADC_CMP_ADCMPEN_Msk             (0x1ul << EADC_CMP_ADCMPEN_Pos)                   /*!< EADC_T::CMPn: ADCMPEN Mask             */

#define EADC_CMP_ADCMPIE_Pos             (1)                                               /*!< EADC_T::CMPn: ADCMPIE Position         */
#define EADC_CMP_ADCMPIE_Msk             (0x1ul << EADC_CMP_ADCMPIE_Pos)                   /*!< EADC_T::CMPn: ADCMPIE Mask             */

#define EADC_CMP_CMPCOND_Pos             (2)                                               /*!< EADC_T::CMPn: CMPCOND Position         */
#define EADC_CMP_CMPCOND_Msk             (0x1ul << EADC_CMP_CMPCOND_Pos)                   /*!< EADC_T::CMPn: CMPCOND Mask             */

#define EADC_CMP_CMPSPL_Pos              (3)                                               /*!< EADC_T::CMPn: CMPSPL Position          */
#define EADC_CMP_CMPSPL_Msk              (0x1ful << EADC_CMP_CMPSPL_Pos)                   /*!< EADC_T::CMPn: CMPSPL Mask              */

#define EADC_CMP_CMPMCNT_Pos             (8)                                               /*!< EADC_T::CMPn: CMPMCNT Position         */
#define EADC_CMP_CMPMCNT_Msk             (0xful << EADC_CMP_CMPMCNT_Pos)                   /*!< EADC_T::CMPn: CMPMCNT Mask             */

#define EADC_CMP_CMPWEN_Pos              (15)                                              /*!< EADC_T::CMPn: CMPWEN Position          */
#define EADC_CMP_CMPWEN_Msk              (0x1ul << EADC_CMP_CMPWEN_Pos)                    /*!< EADC_T::CMPn: CMPWEN Mask              */

#define EADC_CMP_CMPDAT_Pos              (16)                                              /*!< EADC_T::CMPn: CMPDAT Position          */
#define EADC_CMP_CMPDAT_Msk              (0xffful << EADC_CMP_CMPDAT_Pos)                  /*!< EADC_T::CMPn: CMPDAT Mask              */

#define EADC_STATUS0_VALID_Pos           (0)                                               /*!< EADC_T::STATUS0: VALID Position        */
#define EADC_STATUS0_VALID_Msk           (0xfffful << EADC_STATUS0_VALID_Pos)              /*!< EADC_T::STATUS0: VALID Mask            */

#define EADC_STATUS0_OV_Pos              (16)                                              /*!< EADC_T::STATUS0: OV Position           */
#define EADC_STATUS0_OV_Msk              (0xfffful << EADC_STATUS0_OV_Pos)                 /*!< EADC_T::STATUS0: OV Mask               */

#define EADC_STATUS1_VALID_Pos           (0)                                               /*!< EADC_T::STATUS1: VALID Position        */
#define EADC_STATUS1_VALID_Msk           (0x7ul << EADC_STATUS1_VALID_Pos)                 /*!< EADC_T::STATUS1: VALID Mask            */

#define EADC_STATUS1_OV_Pos              (16)                                              /*!< EADC_T::STATUS1: OV Position           */
#define EADC_STATUS1_OV_Msk              (0x7ul << EADC_STATUS1_OV_Pos)                    /*!< EADC_T::STATUS1: OV Mask               */

#define EADC_STATUS2_ADIF0_Pos           (0)                                               /*!< EADC_T::STATUS2: ADIF0 Position        */
#define EADC_STATUS2_ADIF0_Msk           (0x1ul << EADC_STATUS2_ADIF0_Pos)                 /*!< EADC_T::STATUS2: ADIF0 Mask            */

#define EADC_STATUS2_ADIF1_Pos           (1)                                               /*!< EADC_T::STATUS2: ADIF1 Position        */
#define EADC_STATUS2_ADIF1_Msk           (0x1ul << EADC_STATUS2_ADIF1_Pos)                 /*!< EADC_T::STATUS2: ADIF1 Mask            */

#define EADC_STATUS2_ADIF2_Pos           (2)                                               /*!< EADC_T::STATUS2: ADIF2 Position        */
#define EADC_STATUS2_ADIF2_Msk           (0x1ul << EADC_STATUS2_ADIF2_Pos)                 /*!< EADC_T::STATUS2: ADIF2 Mask            */

#define EADC_STATUS2_ADIF3_Pos           (3)                                               /*!< EADC_T::STATUS2: ADIF3 Position        */
#define EADC_STATUS2_ADIF3_Msk           (0x1ul << EADC_STATUS2_ADIF3_Pos)                 /*!< EADC_T::STATUS2: ADIF3 Mask            */

#define EADC_STATUS2_ADCMPF0_Pos         (4)                                               /*!< EADC_T::STATUS2: ADCMPF0 Position      */
#define EADC_STATUS2_ADCMPF0_Msk         (0x1ul << EADC_STATUS2_ADCMPF0_Pos)               /*!< EADC_T::STATUS2: ADCMPF0 Mask          */

#define EADC_STATUS2_ADCMPF1_Pos         (5)                                               /*!< EADC_T::STATUS2: ADCMPF1 Position      */
#define EADC_STATUS2_ADCMPF1_Msk         (0x1ul << EADC_STATUS2_ADCMPF1_Pos)               /*!< EADC_T::STATUS2: ADCMPF1 Mask          */

#define EADC_STATUS2_ADCMPF2_Pos         (6)                                               /*!< EADC_T::STATUS2: ADCMPF2 Position      */
#define EADC_STATUS2_ADCMPF2_Msk         (0x1ul << EADC_STATUS2_ADCMPF2_Pos)               /*!< EADC_T::STATUS2: ADCMPF2 Mask          */

#define EADC_STATUS2_ADCMPF3_Pos         (7)                                               /*!< EADC_T::STATUS2: ADCMPF3 Position      */
#define EADC_STATUS2_ADCMPF3_Msk         (0x1ul << EADC_STATUS2_ADCMPF3_Pos)               /*!< EADC_T::STATUS2: ADCMPF3 Mask          */

#define EADC_STATUS2_ADOVIF0_Pos         (8)                                               /*!< EADC_T::STATUS2: ADOVIF0 Position      */
#define EADC_STATUS2_ADOVIF0_Msk         (0x1ul << EADC_STATUS2_ADOVIF0_Pos)               /*!< EADC_T::STATUS2: ADOVIF0 Mask          */

#define EADC_STATUS2_ADOVIF1_Pos         (9)                                               /*!< EADC_T::STATUS2: ADOVIF1 Position      */
#define EADC_STATUS2_ADOVIF1_Msk         (0x1ul << EADC_STATUS2_ADOVIF1_Pos)               /*!< EADC_T::STATUS2: ADOVIF1 Mask          */

#define EADC_STATUS2_ADOVIF2_Pos         (10)                                              /*!< EADC_T::STATUS2: ADOVIF2 Position      */
#define EADC_STATUS2_ADOVIF2_Msk         (0x1ul << EADC_STATUS2_ADOVIF2_Pos)               /*!< EADC_T::STATUS2: ADOVIF2 Mask          */

#define EADC_STATUS2_ADOVIF3_Pos         (11)                                              /*!< EADC_T::STATUS2: ADOVIF3 Position      */
#define EADC_STATUS2_ADOVIF3_Msk         (0x1ul << EADC_STATUS2_ADOVIF3_Pos)               /*!< EADC_T::STATUS2: ADOVIF3 Mask          */

#define EADC_STATUS2_ADCMPO0_Pos         (12)                                              /*!< EADC_T::STATUS2: ADCMPO0 Position      */
#define EADC_STATUS2_ADCMPO0_Msk         (0x1ul << EADC_STATUS2_ADCMPO0_Pos)               /*!< EADC_T::STATUS2: ADCMPO0 Mask          */

#define EADC_STATUS2_ADCMPO1_Pos         (13)                                              /*!< EADC_T::STATUS2: ADCMPO1 Position      */
#define EADC_STATUS2_ADCMPO1_Msk         (0x1ul << EADC_STATUS2_ADCMPO1_Pos)               /*!< EADC_T::STATUS2: ADCMPO1 Mask          */

#define EADC_STATUS2_ADCMPO2_Pos         (14)                                              /*!< EADC_T::STATUS2: ADCMPO2 Position      */
#define EADC_STATUS2_ADCMPO2_Msk         (0x1ul << EADC_STATUS2_ADCMPO2_Pos)               /*!< EADC_T::STATUS2: ADCMPO2 Mask          */

#define EADC_STATUS2_ADCMPO3_Pos         (15)                                              /*!< EADC_T::STATUS2: ADCMPO3 Position      */
#define EADC_STATUS2_ADCMPO3_Msk         (0x1ul << EADC_STATUS2_ADCMPO3_Pos)               /*!< EADC_T::STATUS2: ADCMPO3 Mask          */

#define EADC_STATUS2_CHANNEL_Pos         (16)                                              /*!< EADC_T::STATUS2: CHANNEL Position      */
#define EADC_STATUS2_CHANNEL_Msk         (0x1ful << EADC_STATUS2_CHANNEL_Pos)              /*!< EADC_T::STATUS2: CHANNEL Mask          */

#define EADC_STATUS2_BUSY_Pos            (23)                                              /*!< EADC_T::STATUS2: BUSY Position         */
#define EADC_STATUS2_BUSY_Msk            (0x1ul << EADC_STATUS2_BUSY_Pos)                  /*!< EADC_T::STATUS2: BUSY Mask             */

#define EADC_STATUS2_ADOVIF_Pos          (24)                                              /*!< EADC_T::STATUS2: ADOVIF Position       */
#define EADC_STATUS2_ADOVIF_Msk          (0x1ul << EADC_STATUS2_ADOVIF_Pos)                /*!< EADC_T::STATUS2: ADOVIF Mask           */

#define EADC_STATUS2_STOVF_Pos           (25)                                              /*!< EADC_T::STATUS2: STOVF Position        */
#define EADC_STATUS2_STOVF_Msk           (0x1ul << EADC_STATUS2_STOVF_Pos)                 /*!< EADC_T::STATUS2: STOVF Mask            */

#define EADC_STATUS2_AVALID_Pos          (26)                                              /*!< EADC_T::STATUS2: AVALID Position       */
#define EADC_STATUS2_AVALID_Msk          (0x1ul << EADC_STATUS2_AVALID_Pos)                /*!< EADC_T::STATUS2: AVALID Mask           */

#define EADC_STATUS2_AOV_Pos             (27)                                              /*!< EADC_T::STATUS2: AOV Position          */
#define EADC_STATUS2_AOV_Msk             (0x1ul << EADC_STATUS2_AOV_Pos)                   /*!< EADC_T::STATUS2: AOV Mask              */

#define EADC_STATUS3_CURSPL_Pos          (0)                                               /*!< EADC_T::STATUS3: CURSPL Position       */
#define EADC_STATUS3_CURSPL_Msk          (0x1ful << EADC_STATUS3_CURSPL_Pos)               /*!< EADC_T::STATUS3: CURSPL Mask           */

#define EADC_MCTL1_ALIGN_Pos             (0)                                               /*!< EADC_T::MnCTL1: ALIGN Position         */
#define EADC_MCTL1_ALIGN_Msk             (0x1ul << EADC_MCTL1_ALIGN_Pos)                   /*!< EADC_T::MnCTL1: ALIGN Mask             */

#define EADC_MCTL1_AVG_Pos               (1)                                               /*!< EADC_T::MnCTL1: AVG Position           */
#define EADC_MCTL1_AVG_Msk               (0x1ul << EADC_MCTL1_AVG_Pos)                     /*!< EADC_T::MnCTL1: AVG Mask               */

#define EADC_MCTL1_ACU_Pos               (4)                                               /*!< EADC_T::MnCTL1: ACU Position           */
#define EADC_MCTL1_ACU_Msk               (0xful << EADC_MCTL1_ACU_Pos)                     /*!< EADC_T::MnCTL1: ACU Mask               */

#define EADC_PWRCTL_READY_Pos            (0)                                               /*!< EADC_T::PWRCTL: READY Position         */
#define EADC_PWRCTL_READY_Msk            (0x1ul << EADC_PWRCTL_READY_Pos)                  /*!< EADC_T::PWRCTL: READY Mask             */

#define EADC_PWRCTL_AUTOFF_Pos           (5)                                               /*!< EADC_T::PWRCTL: AUTOFF Position        */
#define EADC_PWRCTL_AUTOFF_Msk           (0x1ul << EADC_PWRCTL_AUTOFF_Pos)                 /*!< EADC_T::PWRCTL: AUTOFF Mask            */

#define EADC_PWRCTL_STUPT_Pos            (8)                                               /*!< EADC_T::PWRCTL: STUPT Position         */
#define EADC_PWRCTL_STUPT_Msk            (0xffful << EADC_PWRCTL_STUPT_Pos)                /*!< EADC_T::PWRCTL: STUPT Mask             */

#define EADC_PWRCTL_AUTOPDTHT_Pos        (20)                                              /*!< EADC_T::PWRCTL: AUTOPDTHT Position     */
#define EADC_PWRCTL_AUTOPDTHT_Msk        (0xful << EADC_PWRCTL_AUTOPDTHT_Pos)              /*!< EADC_T::PWRCTL: AUTOPDTHT Mask         */

#define EADC_PDMACTL_PDMATEN_Pos         (0)                                               /*!< EADC_T::PDMACTL: PDMATEN Position      */
#define EADC_PDMACTL_PDMATEN_Msk         (0x7fffful << EADC_PDMACTL_PDMATEN_Pos)           /*!< EADC_T::PDMACTL: PDMATEN Mask          */

#define EADC_OFFSETCAL_OFFSETCANCEL_Pos  (0)                                               /*!< EADC_T::OFFSETCAL: OFFSETCANCEL Position*/
#define EADC_OFFSETCAL_OFFSETCANCEL_Msk  (0x1ful << EADC_OFFSETCAL_OFFSETCANCEL_Pos)       /*!< EADC_T::OFFSETCAL: OFFSETCANCEL Mask   */

/**@}*/ /* EADC_CONST */
/**@}*/ /* end of EADC register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
    #pragma no_anon_unions
#endif

#endif /* __EADC_REG_H__ */
/**************************************************************************//**
 * @file     sys_reg.h
 * @version  V1.00
 * @brief    SYS register definition header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SYS_REG_H__
#define __SYS_REG_H__

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup SYS System Manger Controller (SYS)
    Memory Mapped Structure for SYS Controller
@{ */

typedef struct
{


    /**
     * @var SYS_T::PDID
     * Offset: 0x00  Part Device Identification Number Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |PDID      |Part Device Identification Number (Read Only)
     * |        |          |This register reflects device part number code
     * |        |          |Software can read this register to identify which device is used.
     * @var SYS_T::RSTSTS
     * Offset: 0x04  System Reset Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PORF      |POR Reset Flag
     * |        |          |The POR reset flag is set by the "Reset Signal" from the Power-on Reset (POR) Controller or bit CHIPRST (SYS_IPRST0[0]) to indicate the previous reset source.
     * |        |          |0 = No reset from POR or CHIPRST.
     * |        |          |1 = Power-on Reset (POR) or CHIPRST had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[1]     |PINRF     |NRESET Pin Reset Flag
     * |        |          |The nRESET pin reset flag is set by the "Reset Signal" from the nRESET Pin to indicate the previous reset source.
     * |        |          |0 = No reset from nRESET pin.
     * |        |          |1 = Pin nRESET had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[2]     |WDTRF     |WDT Reset Flag
     * |        |          |The WDT reset flag is set by the "Reset Signal" from the Watchdog Timer or Window Watchdog Timer to indicate the previous reset source.
     * |        |          |0 = No reset from watchdog timer or window watchdog timer.
     * |        |          |1 = The watchdog timer or window watchdog timer had issued the reset signal to reset the system.
     * |        |          |Note1: Write 1 to clear this bit to 0.
     * |        |          |Note2: Watchdog Timer register RSTF(WDT_CTL[2]) bit is set if the system has been reset by WDT time-out reset
     * |        |          |Window Watchdog Timer register WWDTRF(WWDT_STATUS[1]) bit is set if the system has been reset by WWDT time-out reset.
     * |[3]     |LVRF      |LVR Reset Flag
     * |        |          |The LVR reset flag is set by the "Reset Signal" from the Low Voltage Reset Controller to indicate the previous reset source.
     * |        |          |0 = No reset from LVR.
     * |        |          |1 = LVR controller had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[4]     |BODRF     |BOD Reset Flag
     * |        |          |The BOD reset flag is set by the "Reset Signal" from the Brown-Out Detector to indicate the previous reset source.
     * |        |          |0 = No reset from BOD.
     * |        |          |1 = The BOD had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[5]     |SYSRF     |System Reset Flag
     * |        |          |The system reset flag is set by the "Reset Signal" from the Cortex-M23 Core to indicate the previous reset source.
     * |        |          |0 = No reset from Cortex-M23.
     * |        |          |1 = The Cortex-M23 had issued the reset signal to reset the system by writing 1 to the bit SYSRESETREQ(AIRCR[2], Application Interrupt and Reset Control Register, address = 0xE000ED0C) in system control registers of Cortex-M23 core.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[6]     |PMURF     |PMU reset flag
     * |        |          |The PMU reset flag is set by any reset  signal when MCU is in power down state.
     * |        |          |0 = No reset in power down state.
     * |        |          |1 = Any reset signal happens in power down  state.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[7]     |CPURF     |CPU Reset Flag
     * |        |          |The CPU reset flag is set by hardware if software writes CPURST (SYS_IPRST0[1]) 1 to reset Cortex-M23 Core and Flash Memory Controller (FMC).
     * |        |          |0 = No reset from CPU.
     * |        |          |1 = The Cortex-M23 Core and FMC are reset by software setting CPURST to 1.
     * |        |          |Note: Write to clear this bit to 0.
     * |[8]     |CPULKRF   |CPU Lockup Reset Flag
     * |        |          |0 = No reset from CPU lockup happened.
     * |        |          |1 = The Cortex-M23 lockup happened and chip is reset.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |        |          |Note2: When CPU lockup happened under ICE is connected, This flag will set to 1 but chip will not reset.
     * |[9]     |VBATLVRF  |VBAT LVR Reset Flag
     * |        |          |The VBAT LVR reset flag is set by the “Reset Signal” from the VBAT Low Voltage Reset Controller to indicate the previous reset source.
     * |        |          |0 = No reset from VBAT LVR.
     * |        |          |1 = VBAT LVR controller had issued the reset signal to reset the system.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * @var SYS_T::IPRST0
     * Offset: 0x08  Peripheral  Reset Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CHIPRST   |Chip One-shot Reset (Write Protect)
     * |        |          |Setting this bit will reset the whole chip, including Processor core and all peripherals, and this bit will automatically return to 0 after the 2 clock cycles.
     * |        |          |The CHIPRST is same as the POR reset, all the chip controllers is reset and the chip setting from flash are also reload.
     * |        |          |About the difference between CHIPRST and SYSRESETREQ(AIRCR[2]), please refer to section 6.2.2
     * |        |          |0 = Chip normal operation.
     * |        |          |1 = Chip one-shot reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[1]     |CPURST    |Processor Core One-shot Reset (Write Protect)
     * |        |          |Setting this bit will only reset the processor core and Flash Memory Controller(FMC), and this bit will automatically return to 0 after the 2 clock cycles.
     * |        |          |0 = Processor core normal operation.
     * |        |          |1 = Processor core one-shot reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |PDMARST   |PDMA Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the PDMA
     * |        |          |User needs to set this bit to 0 to release from reset state.
     * |        |          |0 = PDMA controller normal operation.
     * |        |          |1 = PDMA controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |EBIRST    |EBI Controller Reset (Write Protect)
     * |        |          |Set this bit to 1 will generate a reset signal to the EBI
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = EBI controller normal operation.
     * |        |          |1 = EBI controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[7]     |CRCRST    |CRC Calculation Controller Reset (Write Protect)
     * |        |          |Set this bit to 1 will generate a reset signal to the CRC calculation controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = CRC calculation controller normal operation.
     * |        |          |1 = CRC calculation controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[12]    |CRYPTRST  |CRYPTO Controller Reset (Write Protect)
     * |        |          |Setting this bit to 1 will generate a reset signal to the CRYPTO controller
     * |        |          |User needs to set this bit to 0 to release from the reset state.
     * |        |          |0 = CRYPTO controller normal operation.
     * |        |          |1 = CRYPTO controller reset.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::IPRST1
     * Offset: 0x0C  Peripheral Reset Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |GPIORST   |GPIO Controller Reset
     * |        |          |0 = GPIO controller normal operation.
     * |        |          |1 = GPIO controller reset.
     * |[2]     |TMR0RST   |Timer0 Controller Reset
     * |        |          |0 = Timer0 controller normal operation.
     * |        |          |1 = Timer0 controller reset.
     * |[3]     |TMR1RST   |Timer1 Controller Reset
     * |        |          |0 = Timer1 controller normal operation.
     * |        |          |1 = Timer1 controller reset.
     * |[4]     |TMR2RST   |Timer2 Controller Reset
     * |        |          |0 = Timer2 controller normal operation.
     * |        |          |1 = Timer2 controller reset.
     * |[5]     |TMR3RST   |Timer3 Controller Reset
     * |        |          |0 = Timer3 controller normal operation.
     * |        |          |1 = Timer3 controller reset.
     * |[7]     |ACMP01RST |Analog Comparator 0/1 Controller Reset
     * |        |          |0 = Analog Comparator 0/1 controller normal operation.
     * |        |          |1 = Analog Comparator 0/1 controller reset.
     * |[8]     |I2C0RST   |I2C0 Controller Reset
     * |        |          |0 = I2C0 controller normal operation.
     * |        |          |1 = I2C0 controller reset.
     * |[9]     |I2C1RST   |I2C1 Controller Reset
     * |        |          |0 = I2C1 controller normal operation.
     * |        |          |1 = I2C1 controller reset.
     * |[12]    |QSPI0RST   |QSPI0 Controller Reset
     * |        |          |0 = QSPI0 controller normal operation.
     * |        |          |1 = QSPI0 controller reset.
     * |[13]    |SPI0RST   |SPI0 Controller Reset
     * |        |          |0 = SPI0 controller normal operation.
     * |        |          |1 = SPI0 controller reset.
     * |[16]    |UART0RST  |UART0 Controller Reset
     * |        |          |0 = UART0 controller normal operation.
     * |        |          |1 = UART0 controller reset.
     * |[17]    |UART1RST  |UART1 Controller Reset
     * |        |          |0 = UART1 controller normal operation.
     * |        |          |1 = UART1 controller reset.
     * |[18]    |UART2RST  |UART2 Controller Reset
     * |        |          |0 = UART2 controller normal operation.
     * |        |          |1 = UART2 controller reset.
     * |[27]    |USBDRST   |USBD Controller Reset
     * |        |          |0 = USBD controller normal operation.
     * |        |          |1 = USBD controller reset.
     * |[28]    |EADCRST   |EADC Controller Reset
     * |        |          |0 = EADC controller normal operation.
     * |        |          |1 = EADC controller reset.
     * @var SYS_T::IPRST2
     * Offset: 0x10  Peripheral Reset Control Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SC0RST    |SC0 Controller Reset
     * |        |          |0 = SC0 controller normal operation.
     * |        |          |1 = SC0 controller reset.
     * |[8]     |USCI0RST  |USCI0 Controller Reset
     * |        |          |0 = USCI0 controller normal operation.
     * |        |          |1 = USCI0 controller reset.
     * |[9]     |USCI1RST  |USCI1 Controller Reset
     * |        |          |0 = USCI1 controller normal operation.
     * |        |          |1 = USCI1 controller reset.
     * |[10]    |USCI2RST  |USCI2 Controller Reset
     * |        |          |0 = USCI2 controller normal operation.
     * |        |          |1 = USCI2 controller reset.
     * |[12]    |DACRST    |DAC Controller Reset
     * |        |          |0 = DAC controller normal operation.
     * |        |          |1 = DAC controller reset.
     * |[16]    |PWM0RST   |PWM0 Controller Reset
     * |        |          |0 = PWM0 controller normal operation.
     * |        |          |1 = PWM0 controller reset.
     * |[17]    |PWM1RST   |PWM1 Controller Reset
     * |        |          |0 = PWM1 controller normal operation.
     * |        |          |1 = PWM1 controller reset.
     * |[18]    |BPWM0RST  |BPWM0 Controller Reset
     * |        |          |0 = BPWM0 controller normal operation.
     * |        |          |1 = BPWM0 controller reset.
     * |[19]    |BPWM1RST  |BPWM1 Controller Reset
     * |        |          |0 = BPWM1 controller normal operation.
     * |        |          |1 = BPWM1 controller reset.
     * |[30]    |OPARST    |OP Amplifier (OPA) Controller Reset
     * |        |          |0 = OPA controller normal operation.
     * |        |          |1 = OPA controller reset.
     * |[31]    |PSIORST   |PSIORST
     * @var SYS_T::BODCTL
     * Offset: 0x18  Brown-out Detector Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BODEN     |Brown-out Detector Enable Bit (Write Protect)
     * |        |          |The default value is set by flash controller user configuration register CBODEN (CONFIG0 [19]).
     * |        |          |0 = Brown-out Detector function Disabled.
     * |        |          |1 = Brown-out Detector function Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |BODRSTEN  |Brown-out Reset Enable Bit (Write Protect)
     * |        |          |The default value is set by flash controller user configuration register CBORST(CONFIG0[20]) bit .
     * |        |          |0 = Brown-out "INTERRUPT" function Enabled.
     * |        |          |1 = Brown-out "RESET" function Enabled.
     * |        |          |Note1:
     * |        |          |While the Brown-out Detector function is enabled (BODEN high) and BOD reset function is enabled (BODRSTEN high), BOD will assert a signal to reset chip when the detected voltage is lower than the threshold (BODOUT high).
     * |        |          |While the BOD function is enabled (BODEN high) and BOD interrupt function is enabled (BODRSTEN low), BOD will assert an interrupt if BODOUT is high
     * |        |          |BOD interrupt will keep till to the BODEN set to 0
     * |        |          |BOD interrupt can be blocked by disabling the NVIC BOD interrupt or disabling BOD function (set BODEN low).
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[4]     |BODIF     |Brown-out Detector Interrupt Flag
     * |        |          |0 = Brown-out Detector does not detect any voltage draft at VDD down through or up through the voltage of BODVL setting.
     * |        |          |1 = When Brown-out Detector detects the VDD is dropped down through the voltage of BODVL setting or the VDD is raised up through the voltage of BODVL setting, this bit is set to 1 and the brown-out interrupt is requested if brown-out interrupt is enabled.
     * |        |          |Note: Write 1 to clear this bit to 0.
     * |[5]     |BODLPM    |Brown-out Detector Low Power Mode (Write Protect)
     * |        |          |0 = BOD operate in normal mode (default).
     * |        |          |1 = BOD Low Power mode Enabled.
     * |        |          |Note1: The BOD consumes about 100uA in normal mode, the low power mode can reduce the current to about 1/10 but slow the BOD response.
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[6]     |BODOUT    |Brown-out Detector Output Status
     * |        |          |0 = Brown-out Detector output status is 0.
     * |        |          |It means the detected voltage is higher than BODVL setting or BODEN is 0.
     * |        |          |1 = Brown-out Detector output status is 1.
     * |        |          |It means the detected voltage is lower than BODVL setting
     * |        |          |If the BODEN is 0, BOD function disabled , this bit always responds 0000.
     * |[7]     |LVREN     |Low Voltage Reset Enable Bit (Write Protect)
     * |        |          |The LVR function resets the chip when the input power voltage is lower than LVR circuit setting
     * |        |          |LVR function is enabled by default.
     * |        |          |0 = Low Voltage Reset function Disabled.
     * |        |          |1 = Low Voltage Reset function Enabled.
     * |        |          |Note1: After enabling the bit, the LVR function will be active with 100us delay for LVR output stable (default).
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[10:8]  |BODDGSEL  |Brown-out Detector Output De-glitch Time Select (Write Protect)
     * |        |          |000 = BOD output is sampled by LIRC.
     * |        |          |001 = 4 system clock (HCLK).
     * |        |          |010 = 8 system clock (HCLK).
     * |        |          |011 = 16 system clock (HCLK).
     * |        |          |100 = 32 system clock (HCLK).
     * |        |          |101 = 64 system clock (HCLK).
     * |        |          |110 = 128 system clock (HCLK).
     * |        |          |111 = 256 system clock (HCLK).
     * |        |          |Note: These bits are write protected. Refer to the SYS_REGLCTL register.
     * |[14:12] |LVRDGSEL  |LVR Output De-glitch Time Select (Write Protect)
     * |        |          |000 = Without de-glitch function.
     * |        |          |001 = 4 MIRC clock (4.032 MHz), 1 us.
     * |        |          |010 = 8 MIRC clock (4.032 MHz), 2 us.
     * |        |          |011 = 16 MIRC clock (4.032 MHz), 4 us.
     * |        |          |100 = 32 MIRC clock (4.032 MHz), 8 us.
     * |        |          |101 = 64 MIRC clock (4.032 MHz), 16 us.
     * |        |          |110 = 128 MIRC clock (4.032 MHz), 32 us.
     * |        |          |111 = 256 MIRC clock (4.032 MHz), 64 us.
     * |        |          |Note: These bits are write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: The MIRC enabled automatically when LVRDGSEL is not 000 and LVREN is 1.
     * |[18:16] |BODVL     |Brown-out Detector Threshold Voltage Selection (Write Protect)
     * |        |          |The default value is set by flash controller user configuration register CBOV (CONFIG0 [23:21]).
     * |        |          |000 = Reserved.
     * |        |          |001 = Brown-Out Detector threshold voltage is 1.8V.
     * |        |          |010 = Brown-Out Detector threshold voltage is 2.0V.
     * |        |          |011 = Brown-Out Detector threshold voltage is 2.4V.
     * |        |          |100 = Brown-Out Detector threshold voltage is 2.7V.
     * |        |          |101 = Brown-Out Detector threshold voltage is 3.0V.
     * |        |          |110 = Brown-Out Detector threshold voltage is 3.7V.
     * |        |          |111 = Brown-Out Detector threshold voltage is 4.4V.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::IVSCTL
     * Offset: 0x1C  Internal Voltage Source Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |VTEMPEN   |Temperature Sensor Enable Bit
     * |        |          |This bit is used to enable/disable temperature sensor function.
     * |        |          |0 = Temperature sensor function Disabled (default).
     * |        |          |1 = Temperature sensor function Enabled.
     * |[1]     |VBATUGEN  |VBAT Unity Gain Buffer Enable Bit
     * |        |          |This bit is used to enable/disable VBAT unity gain buffer function.
     * |        |          |0 = VBAT unity gain buffer function Disabled (default).
     * |        |          |1 = VBAT unity gain buffer function Enabled.
     * |        |          |Note: After this bit is set to 1, the value of VBAT unity gain buffer output voltage can be obtained from ADC conversion result
     * @var SYS_T::PORCTL0
     * Offset: 0x24  Power-On-reset Controller Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PORMASK   |Power-on Reset Enable Bit (Write Protect)
     * |        |          |When powered on, the POR circuit generates a reset signal to reset the whole chip function, but noise on the power may cause the POR active again
     * |        |          |User can disable internal POR circuit to avoid unpredictable noise to cause chip reset by writing 0x5AA5 to this field.
     * |        |          |The POR function will be active again when this field is set to another value or chip is reset by other reset source, including:
     * |        |          |nRESET, Watchdog, LVR reset, BOD reset, ICE reset command and the software-chip reset function.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::VREFCTL
     * Offset: 0x28  VREF Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |IVRS      |Internal Voltage Reference Scale (Write Protect)
     * |        |          |0 = Internal voltage reference(INT_VREF) set to 1.536V.
     * |        |          |1 = Internal voltage reference(INT_VREF) set to 2.048V.
     * |        |          |2 = Internal voltage reference(INT_VREF) set to 2.56V.
     * |        |          |3 = Internal voltage reference(INT_VREF) set to 3.072V.
     * |        |          |4 = Internal voltage reference(INT_VREF) set to 4.096V.
     * |        |          |Others = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |IVREN     |Internal Voltage Reference Module Enable(Write Protect)
     * |        |          |0 = Internal voltage reference module is disabled.
     * |        |          |1 = Internal voltage reference module is enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[10]    |PRELOAD   |1. Preload Active
     * |        |          |2. Preload inactive
     * @var SYS_T::GPA_MFPL
     * Offset: 0x30  GPIOA Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PA0MFP    |PA.0 Multi-function Pin Selection
     * |[7:4]   |PA1MFP    |PA.1 Multi-function Pin Selection
     * |[11:8]  |PA2MFP    |PA.2 Multi-function Pin Selection
     * |[15:12] |PA3MFP    |PA.3 Multi-function Pin Selection
     * |[19:16] |PA4MFP    |PA.4 Multi-function Pin Selection
     * |[23:20] |PA5MFP    |PA.5 Multi-function Pin Selection
     * |[27:24] |PA6MFP    |PA.6 Multi-function Pin Selection
     * |[31:28] |PA7MFP    |PA.7 Multi-function Pin Selection
     * @var SYS_T::GPA_MFPH
     * Offset: 0x34  GPIOA High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PA8MFP    |PA.8 Multi-function Pin Selection
     * |[7:4]   |PA9MFP    |PA.9 Multi-function Pin Selection
     * |[11:8]  |PA10MFP   |PA.10 Multi-function Pin Selection
     * |[15:12] |PA11MFP   |PA.11 Multi-function Pin Selection
     * |[19:16] |PA12MFP   |PA.12 Multi-function Pin Selection
     * |[23:20] |PA13MFP   |PA.13 Multi-function Pin Selection
     * |[27:24] |PA14MFP   |PA.14 Multi-function Pin Selection
     * |[31:28] |PA15MFP   |PA.15 Multi-function Pin Selection
     * @var SYS_T::GPB_MFPL
     * Offset: 0x38  GPIOB Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PB0MFP    |PB.0 Multi-function Pin Selection
     * |[7:4]   |PB1MFP    |PB.1 Multi-function Pin Selection
     * |[11:8]  |PB2MFP    |PB.2 Multi-function Pin Selection
     * |[15:12] |PB3MFP    |PB.3 Multi-function Pin Selection
     * |[19:16] |PB4MFP    |PB.4 Multi-function Pin Selection
     * |[23:20] |PB5MFP    |PB.5 Multi-function Pin Selection
     * |[27:24] |PB6MFP    |PB.6 Multi-function Pin Selection
     * |[31:28] |PB7MFP    |PB.7 Multi-function Pin Selection
     * @var SYS_T::GPB_MFPH
     * Offset: 0x3C  GPIOB High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PB8MFP    |PB.8 Multi-function Pin Selection
     * |[7:4]   |PB9MFP    |PB.9 Multi-function Pin Selection
     * |[11:8]  |PB10MFP   |PB.10 Multi-function Pin Selection
     * |[15:12] |PB11MFP   |PB.11 Multi-function Pin Selection
     * |[19:16] |PB12MFP   |PB.12 Multi-function Pin Selection
     * |[23:20] |PB13MFP   |PB.13 Multi-function Pin Selection
     * |[27:24] |PB14MFP   |PB.14 Multi-function Pin Selection
     * |[31:28] |PB15MFP   |PB.15 Multi-function Pin Selection
     * @var SYS_T::GPC_MFPL
     * Offset: 0x40  GPIOC Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PC0MFP    |PC.0 Multi-function Pin Selection
     * |[7:4]   |PC1MFP    |PC.1 Multi-function Pin Selection
     * |[11:8]  |PC2MFP    |PC.2 Multi-function Pin Selection
     * |[15:12] |PC3MFP    |PC.3 Multi-function Pin Selection
     * |[19:16] |PC4MFP    |PC.4 Multi-function Pin Selection
     * |[23:20] |PC5MFP    |PC.5 Multi-function Pin Selection
     * |[27:24] |PC6MFP    |PC.6 Multi-function Pin Selection
     * |[31:28] |PC7MFP    |PC.7 Multi-function Pin Selection
     * @var SYS_T::GPC_MFPH
     * Offset: 0x44  GPIOC High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PC8MFP    |PC.8 Multi-function Pin Selection
     * |[7:4]   |PC9MFP    |PC.9 Multi-function Pin Selection
     * |[11:8]  |PC10MFP   |PC.10 Multi-function Pin Selection
     * |[15:12] |PC11MFP   |PC.11 Multi-function Pin Selection
     * |[19:16] |PC12MFP   |PC.12 Multi-function Pin Selection
     * |[23:20] |PC13MFP   |PC.13 Multi-function Pin Selection
     * |[27:24] |PC14MFP   |PC.14 Multi-function Pin Selection
     * |[31:28] |PC15MFP   |PC.15 Multi-function Pin Selection
     * @var SYS_T::GPD_MFPL
     * Offset: 0x48  GPIOD Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PD0MFP    |PD.0 Multi-function Pin Selection
     * |[7:4]   |PD1MFP    |PD.1 Multi-function Pin Selection
     * |[11:8]  |PD2MFP    |PD.2 Multi-function Pin Selection
     * |[15:12] |PD3MFP    |PD.3 Multi-function Pin Selection
     * |[19:16] |PD4MFP    |PD.4 Multi-function Pin Selection
     * |[23:20] |PD5MFP    |PD.5 Multi-function Pin Selection
     * |[27:24] |PD6MFP    |PD.6 Multi-function Pin Selection
     * |[31:28] |PD7MFP    |PD.7 Multi-function Pin Selection
     * @var SYS_T::GPD_MFPH
     * Offset: 0x4C  GPIOD High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PD8MFP    |PD.8 Multi-function Pin Selection
     * |[7:4]   |PD9MFP    |PD.9 Multi-function Pin Selection
     * |[11:8]  |PD10MFP   |PD.10 Multi-function Pin Selection
     * |[15:12] |PD11MFP   |PD.11 Multi-function Pin Selection
     * |[19:16] |PD12MFP   |PD.12 Multi-function Pin Selection
     * |[23:20] |PD13MFP   |PD.13 Multi-function Pin Selection
     * |[27:24] |PD14MFP   |PD.14 Multi-function Pin Selection
     * |[31:28] |PD15MFP   |PD.15 Multi-function Pin Selection
     * @var SYS_T::GPE_MFPL
     * Offset: 0x50  GPIOE Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PE0MFP    |PE.0 Multi-function Pin Selection
     * |[7:4]   |PE1MFP    |PE.1 Multi-function Pin Selection
     * |[11:8]  |PE2MFP    |PE.2 Multi-function Pin Selection
     * |[15:12] |PE3MFP    |PE.3 Multi-function Pin Selection
     * |[19:16] |PE4MFP    |PE.4 Multi-function Pin Selection
     * |[23:20] |PE5MFP    |PE.5 Multi-function Pin Selection
     * |[27:24] |PE6MFP    |PE.6 Multi-function Pin Selection
     * |[31:28] |PE7MFP    |PE.7 Multi-function Pin Selection
     * @var SYS_T::GPE_MFPH
     * Offset: 0x54  GPIOE High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PE8MFP    |PE.8 Multi-function Pin Selection
     * |[7:4]   |PE9MFP    |PE.9 Multi-function Pin Selection
     * |[11:8]  |PE10MFP   |PE.10 Multi-function Pin Selection
     * |[15:12] |PE11MFP   |PE.11 Multi-function Pin Selection
     * |[19:16] |PE12MFP   |PE.12 Multi-function Pin Selection
     * |[23:20] |PE13MFP   |PE.13 Multi-function Pin Selection
     * |[27:24] |PE14_MFP  |PE.14 Multi-function Pin Selection
     * |[31:28] |PE15_MFP  |PE.15 Multi-function Pin Selection
     * @var SYS_T::GPF_MFPL
     * Offset: 0x58  GPIOF Low Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PF0MFP    |PF.0 Multi-function Pin Selection
     * |[7:4]   |PF1MFP    |PF.1 Multi-function Pin Selection
     * |[11:8]  |PF2MFP    |PF.2 Multi-function Pin Selection
     * |[15:12] |PF3MFP    |PF.3 Multi-function Pin Selection
     * |[19:16] |PF4MFP    |PF.4 Multi-function Pin Selection
     * |[23:20] |PF5MFP    |PF.5 Multi-function Pin Selection
     * |[27:24] |PF6MFP    |PF.6 Multi-function Pin Selection
     * |[31:28] |PF7MFP    |PF.7 Multi-function Pin Selection
     * @var SYS_T::GPF_MFPH
     * Offset: 0x5C  GPIOF High Byte Multiple Function Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PF8MFP    |PF.8 Multi-function Pin Selection
     * |[7:4]   |PF9MFP    |PF.9 Multi-function Pin Selection
     * |[11:8]  |PF10MFP   |PF.10 Multi-function Pin Selection
     * |[15:12] |PF11MFP   |PF.11 Multi-function Pin Selection
     * |[19:16] |PF12MFP   |PF.12 Multi-function Pin Selection
     * |[23:20] |PF13MFP   |PF.13 Multi-function Pin Selection
     * |[27:24] |PF14MFP   |PF.14 Multi-function Pin Selection
     * |[31:28] |PF15MFP   |PF.15 Multi-function Pin Selection
     * @var SYS_T::GPA_MFOS
     * Offset: 0x80  GPIOA Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[1]     |MFOS1     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[2]     |MFOS2     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[3]     |MFOS3     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[4]     |MFOS4     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[5]     |MFOS5     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[6]     |MFOS6     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[7]     |MFOS7     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[8]     |MFOS8     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[9]     |MFOS9     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[10]    |MFOS10    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[11]    |MFOS11    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[12]    |MFOS12    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[13]    |MFOS13    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[14]    |MFOS14    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[15]    |MFOS15    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * @var SYS_T::GPB_MFOS
     * Offset: 0x84  GPIOB Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[1]     |MFOS1     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[2]     |MFOS2     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[3]     |MFOS3     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[4]     |MFOS4     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[5]     |MFOS5     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[6]     |MFOS6     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[7]     |MFOS7     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[8]     |MFOS8     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[9]     |MFOS9     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[10]    |MFOS10    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[11]    |MFOS11    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[12]    |MFOS12    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[13]    |MFOS13    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[14]    |MFOS14    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[15]    |MFOS15    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * @var SYS_T::GPC_MFOS
     * Offset: 0x88  GPIOC Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[1]     |MFOS1     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[2]     |MFOS2     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[3]     |MFOS3     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[4]     |MFOS4     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[5]     |MFOS5     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[6]     |MFOS6     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[7]     |MFOS7     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[8]     |MFOS8     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[9]     |MFOS9     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[10]    |MFOS10    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[11]    |MFOS11    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[12]    |MFOS12    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[13]    |MFOS13    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[14]    |MFOS14    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[15]    |MFOS15    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * @var SYS_T::GPD_MFOS
     * Offset: 0x8C  GPIOD Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[1]     |MFOS1     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[2]     |MFOS2     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[3]     |MFOS3     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[4]     |MFOS4     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[5]     |MFOS5     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[6]     |MFOS6     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[7]     |MFOS7     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[8]     |MFOS8     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[9]     |MFOS9     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[10]    |MFOS10    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[11]    |MFOS11    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[12]    |MFOS12    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[13]    |MFOS13    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[14]    |MFOS14    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[15]    |MFOS15    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * @var SYS_T::GPE_MFOS
     * Offset: 0x90  GPIOE Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[1]     |MFOS1     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[2]     |MFOS2     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[3]     |MFOS3     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[4]     |MFOS4     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[5]     |MFOS5     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[6]     |MFOS6     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[7]     |MFOS7     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[8]     |MFOS8     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[9]     |MFOS9     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[10]    |MFOS10    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[11]    |MFOS11    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[12]    |MFOS12    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[13]    |MFOS13    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[14]    |MFOS14    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[15]    |MFOS15    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * @var SYS_T::GPF_MFOS
     * Offset: 0x94  GPIOF Multiple Function Output Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MFOS0     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[1]     |MFOS1     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[2]     |MFOS2     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[3]     |MFOS3     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[4]     |MFOS4     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[5]     |MFOS5     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[6]     |MFOS6     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[7]     |MFOS7     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[8]     |MFOS8     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[9]     |MFOS9     |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[10]    |MFOS10    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[11]    |MFOS11    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[12]    |MFOS12    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[13]    |MFOS13    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[14]    |MFOS14    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * |[15]    |MFOS15    |GPIOA-f Pin[n] Multiple Function Pin Output Mode Select
     * |        |          |This bit used to select multiple function pin output mode type for Px.n pin
     * |        |          |0 = Multiple funtion pin output mode type is Push-pull mode.
     * |        |          |1 = Multiple funtion pin output mode type is Open-drain mode.
     * |        |          |Note:
     * |        |          |Max. n=15.
     * @var SYS_T::MODCTL
     * Offset: 0xC0  Modulation Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MODEN     |Modulation Function Enable Bit
     * |        |          |This bit enables modulation funcion by modulating with PWM0 channel output and USCI0(USCI0_DAT0) or UART0(UART0_TXD) output.
     * |        |          |0 = Modulation Function Disabled.
     * |        |          |1 = Modulation Function Enabled.
     * |[1]     |MODH      |Modulation at Data High
     * |        |          |Select modulation pulse(PWM0) at high or low of UART0_TXD or USCI0_DAT0
     * |        |          |0: Modulation pulse at UART0_TXD low or USCI0_DAT0 low.
     * |        |          |1: Modulation pulse at UART0_TXD high or USCI0_DAT0 high.
     * |[7:4]   |MODPWMSEL |PWM0 Channel Select for Modulation
     * |        |          |Select the PWM0 channel to modulate with the UART0_TXD or USCI0_DAT0.
     * |        |          |0000: PWM0 Channel 0 modulate with UART0_TXD.
     * |        |          |0001: PWM0 Channel 1 modulate with UART0_TXD.
     * |        |          |0010: PWM0 Channel 2 modulate with UART0_TXD.
     * |        |          |0011: PWM0 Channel 3 modulete with UART0_TXD.
     * |        |          |0100: PWM0 Channel 4 modulete with UART0_TXD.
     * |        |          |0101: PWM0 Channel 5 modulete with UART0_TXD.
     * |        |          |0110: Reserved.
     * |        |          |0111: Reserved.
     * |        |          |1000: PWM0 Channel 0 modulate with USCI0_DAT0.
     * |        |          |1001: PWM0 Channel 1 modulate with USCI0_DAT0.
     * |        |          |1010: PWM0 Channel 2 modulate with USCI0_DAT0.
     * |        |          |1011: PWM0 Channel 3 modulete with USCI0_DAT0.
     * |        |          |1100: PWM0 Channel 4 modulete with USCI0_DAT0.
     * |        |          |1101: PWM0 Channel 5 modulete with USCI0_DAT0.
     * |        |          |1110: Reserved.
     * |        |          |1111: Reserved.
     * |        |          |Note: This bis is valid while MODEN (SYS_MODCTL[0]) is set to 1.
     * @var SYS_T::SRAM_BISTCTL
     * Offset: 0xD0  System SRAM BIST Test Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SRBIST    |SRAM Bank0 BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for SRAM bank0.
     * |        |          |0 = system SRAM BIST Disabled.
     * |        |          |1 = system SRAM BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |FMCBIST   |FMC CACHE BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for CACHE RAM
     * |        |          |0 = system CACHE BIST Disabled.
     * |        |          |1 = system CACHE BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[4]     |USBBIST   |USB BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for USB RAM
     * |        |          |0 = system USB BIST Disabled.
     * |        |          |1 = system USB BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[7]     |PDMABIST  |PDMA BIST Enable Bit (Write Protect)
     * |        |          |This bit enables BIST test for PDMA RAM
     * |        |          |0 = system PDMA BIST Disabled.
     * |        |          |1 = system PDMA BIST Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::SRAM_BISTSTS
     * Offset: 0xD4  System SRAM BIST Test Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SRBISTEF  |1st System SRAM BIST Fail Flag
     * |        |          |0 = 1st system SRAM BIST test pass.
     * |        |          |1 = 1st system SRAM BIST test fail.
     * |[1]     |CR0BISTEF |CACHE0 SRAM BIST Fail Flag
     * |        |          |0 = System CACHE RAM BIST test pass.
     * |        |          |1 = System CACHE RAM BIST test fail.
     * |[2]     |CR1BISTEF |CACHE1 SRAM BIST Fail Flag
     * |        |          |0 = System CACHE RAM BIST test pass.
     * |        |          |1 = System CACHE RAM BIST test fail.
     * |[4]     |USBBEF    |USB SRAM BIST Fail Flag
     * |        |          |0 = USB SRAM BIST test pass.
     * |        |          |1 = USB SRAM BIST test fail.
     * |[7]     |PDMABISTF |PDMA SRAM BIST Failed Flag
     * |        |          |0 = 1st PDMA SRAM BIST pass.
     * |        |          |1 =1st PDMA SRAM BIST failed.
     * |[16]    |SRBEND    |1st SRAM BIST Test Finish
     * |        |          |0 = 1st system SRAM BIST active.
     * |        |          |1 =1st system SRAM BIST finish.
     * |[17]    |CR0BEND   |CACHE 0 SRAM BIST Test Finish
     * |        |          |0 = System CACHE RAM BIST is active.
     * |        |          |1 = System CACHE RAM BIST test finish.
     * |[18]    |CR1BEND   |CACHE 1 SRAM BIST Test Finish
     * |        |          |0 = System CACHE RAM BIST is active.
     * |        |          |1 = System CACHE RAM BIST test finish.
     * |[20]    |USBBEND   |USB SRAM BIST Test Finish
     * |        |          |0 = USB SRAM BIST is active.
     * |        |          |1 = USB SRAM BIST test finish.
     * |[23]    |PDMAEND   |PDMA SRAM BIST Test Finish
     * |        |          |0 = PDMA SRAM BIST is active.
     * |        |          |1 = PDMA SRAM BIST test finish.
     * @var SYS_T::HIRCTRIMCTL
     * Offset: 0xF0  HIRC Trim Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |FREQSEL   |Trim Frequency Selection
     * |        |          |This field indicates the target frequency of 48 MHz internal high speed RC oscillator (HIRC) auto trim.
     * |        |          |During auto trim operation, if clock error detected with CESTOPEN is set to 1 or trim retry limitation count reached, this field will be cleared to 00 automatically.
     * |        |          |00 = Disable HIRC auto trim function.
     * |        |          |01 = Enable HIRC auto trim function and trim HIRC to 48 MHz.
     * |        |          |10 = Reserved..
     * |        |          |11 = Reserved.
     * |[5:4]   |LOOPSEL   |Trim Calculation Loop Selection
     * |        |          |This field defines that trim value calculation is based on how many reference clocks.
     * |        |          |00 = Trim value calculation is based on average difference in 4 clocks of reference clock.
     * |        |          |01 = Trim value calculation is based on average difference in 8 clocks of reference clock.
     * |        |          |10 = Trim value calculation is based on average difference in 16 clocks of reference clock.
     * |        |          |11 = Trim value calculation is based on average difference in 32 clocks of reference clock.
     * |        |          |Note: For example, if LOOPSEL is set as 00, auto trim circuit will calculate trim value based on the average frequency difference in 4 clocks of reference clock.
     * |[7:6]   |RETRYCNT  |Trim Value Update Limitation Count
     * |        |          |This field defines that how many times the auto trim circuit will try to update the HIRC trim value before the frequency of HIRC locked.
     * |        |          |Once the HIRC locked, the internal trim value update counter will be reset.
     * |        |          |If the trim value update counter reached this limitation value and frequency of HIRC still doesn't lock, the auto trim operation will be disabled and FREQSEL will be cleared to 00.
     * |        |          |00 = Trim retry count limitation is 64 loops.
     * |        |          |01 = Trim retry count limitation is 128 loops.
     * |        |          |10 = Trim retry count limitation is 256 loops.
     * |        |          |11 = Trim retry count limitation is 512 loops.
     * |[8]     |CESTOPEN  |Clock Error Stop Enable Bit
     * |        |          |0 = The trim operation is keep going if clock is inaccuracy.
     * |        |          |1 = The trim operation is stopped if clock is inaccuracy.
     * |[9]     |BOUNDEN   |Boundary Enable
     * |        |          |0 = Boundary function is disable.
     * |        |          |1 = Boundary function is enable.
     * |[10]    |REFCKSEL  |Reference Clock Selection
     * |        |          |0 = HIRC trim reference from external 32.768 kHz crystal oscillator.
     * |        |          |1 = HIRC trim reference from internal USB synchronous mode.
     * |[20:16] |BOUNDARY  |Boundary Selection
     * |        |          |Fill the boundary range from 0x1 to 0x1F, 0x0 is reserved.
     * |        |          |Note: This field is effective only when the BOUNDEN(SYS_HIRCTRIMCTL[9]) is enable
     * @var SYS_T::HIRCTRIMIEN
     * Offset: 0xF4  HIRC Trim Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |TFALIEN   |Trim Failure Interrupt Enable Bit
     * |        |          |This bit controls if an interrupt will be triggered while HIRC trim value update limitation count reached and HIRC frequency still not locked on target frequency set by FREQSEL(SYS_HIRCCTL[1:0]).
     * |        |          |If this bit is high and TFAILIF(SYS_HIRCSTS[1]) is set during auto trim operation, an interrupt will be triggered to notify that HIRC trim value update limitation count was reached.
     * |        |          |0 = Disable TFAILIF(SYS_HIRCSTS[1]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable TFAILIF(SYS_HIRCSTS[1]) status to trigger an interrupt to CPU.
     * |[2]     |CLKEIEN   |Clock Error Interrupt Enable Bit
     * |        |          |This bit controls if CPU would get an interrupt while clock is inaccuracy during auto trim operation.
     * |        |          |If this bit is set to1, and CLKERRIF(SYS_HIRCSTS[2]) is set during auto trim operation, an interrupt will be triggered to notify the clock frequency is inaccuracy.
     * |        |          |0 = Disable CLKERRIF(SYS_HIRCSTS[2]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable CLKERRIF(SYS_HIRCSTS[2]) status to trigger an interrupt to CPU.
     * @var SYS_T::HIRCTRIMSTS
     * Offset: 0xF8  HIRC Trim Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FREQLOCK  |HIRC Frequency Lock Status
     * |        |          |This bit indicates the HIRC frequency is locked.
     * |        |          |This is a status bit and doesn't trigger any interrupt
     * |        |          |Write 1 to clear this to 0
     * |        |          |This bit will be set automatically, if the frequecy is lock and the RC_TRIM is enabled.
     * |        |          |0 = The internal high-speed oscillator frequency doesn't lock at 48 MHz yet.
     * |        |          |1 = The internal high-speed oscillator frequency locked at 48 MHz.
     * |[1]     |TFAILIF   |Trim Failure Interrupt Status
     * |        |          |This bit indicates that HIRC trim value update limitation count reached and the HIRC clock frequency still doesn't be locked
     * |        |          |Once this bit is set, the auto trim operation stopped and FREQSEL(SYS_HIRCCTL[1:0]) will be cleared to 00 by hardware automatically.
     * |        |          |If this bit is set and TFAILIEN(SYS_HIRCIEN[1]) is high, an interrupt will be triggered to notify that HIRC trim value update limitation count was reached
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Trim value update limitation count does not reach.
     * |        |          |1 = Trim value update limitation count reached and HIRC frequency still not locked.
     * |[2]     |CLKERIF   |Clock Error Interrupt Status
     * |        |          |When the frequency of 32.768 kHz external low speed crystal oscillator (LXT) or 48MHz internal high speed RC oscillator (HIRC) is shift larger to unreasonable value, this bit will be set and to be an indicate that clock frequency is inaccuracy
     * |        |          |Once this bit is set to 1, the auto trim operation stopped and FREQSEL(SYS_HIRCCTL[1:0]) will be cleared to 00 by hardware automatically if CESTOPEN(SYS_HIRCCTL[8]) is set to 1.
     * |        |          |If this bit is set and CLKEIEN(SYS_HIRCTIEN[2]) is high, an interrupt will be triggered to notify the clock frequency is inaccuracy
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Clock frequency is accuracy.
     * |        |          |1 = Clock frequency is inaccuracy.
     * |[3]     |OVBDIF    |Over Boundary Status
     * |        |          |When the over boundary function is set, if there occurs the over boundary condition, this flag will be set.
     * |        |          |0 = Over boundary coundition did not occur.
     * |        |          |1 = Over boundary coundition occurred.
     * |        |          |Note: Write 1 to clear this flag.
     * @var SYS_T::REGLCTL
     * Offset: 0x100  Register Lock Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |REGLCTL   |Register Lock Control Code (Write Only)
     * |        |          |Some registers have write-protection function
     * |        |          |Writing these registers have to disable the protected function by writing the sequence value "59h", "16h", "88h" to this field
     * |        |          |After this sequence is completed, the REGLCTL bit will be set to 1 and write-protection registers can be normal write.
     * |        |          |Register Lock Control Disable Index (Read Only)
     * |        |          |0 = Write-protection Enabled for writing protected registers
     * |        |          |Any write to the protected register is ignored.
     * |        |          |1 = Write-protection Disabled for writing protected registers.
     * |        |          |The Protected registers are:
     * |        |          |NMIEN address 0x4000_0300
     * |        |          |FMC_ISPCTL address 0x4000_C000 (Flash ISP Control register)
     * |        |          |FMC_ISPTRG address 0x4000_C010 (ISP Trigger Control register)
     * |        |          |FMC_ISPSTS address 0x4000_C040
     * |        |          |WDT_CTL address 0x4004_0000
     * |        |          |FMC_FTCTL address 0x4000_5018
     * |        |          |FMC_ICPCMD address 0x4000_501C
     * |        |          |EADC_TEST address 0x4004_3200
     * |        |          |AHBMCTL address 0x40000400
     * |        |          |SYS_IPRST0 address 0x4000_0008
     * |        |          |SYS_BODCTL address 0x4000_0018
     * |        |          |SYS_PORCTL address  0x4000_0024
     * |        |          |SYS_SRAM_BISTCTL address 0x4000_00D0
     * |        |          |SYS_PORCTL1 address 0x4000_01EC
     * |        |          |CLK_PWRCTL address 0x4000_0200
     * |        |          |CLK_APBCLK0[0] address 0x4000_0208
     * |        |          |CLK_CLKSEL0 address 0x4000_0210
     * |        |          |CLK_CLKSEL1[3:0] address 0x4000_0214
     * |        |          |CLK_PLLCTL address 0x40000240
     * |        |          |CLK_PMUCTL address 0x4000_0290
     * |        |          |CLK_HXTFSEL address 0x4000_02B4
     * |        |          |PWM_CTL0 address 0x4005_8000
     * |        |          |PWM_CTL0 address 0x4005_9000
     * |        |          |PWM_DTCTL0_1 address 0x4005_8070
     * |        |          |PWM_DTCTL0_1 address 0x4005_9070
     * |        |          |PWM_DTCTL2_3 address 0x4005_8074
     * |        |          |PWM_DTCTL2_3 address 0x4005_9074
     * |        |          |PWM_DTCTL4_5 address 0x4005_8078
     * |        |          |PWM_DTCTL4_5 address 0x4005_9078
     * |        |          |PWM_BRKCTL0_1 address 0x4005_80C8
     * |        |          |PWM_BRKCTL0_1 address 0x4005_90C8
     * |        |          |PWM_BRKCTL2_3 address0x4005_80CC
     * |        |          |PWM_BRKCTL2_3 address0x4005_90CC
     * |        |          |PWM_BRKCTL4_5 address0x4005_80D0
     * |        |          |PWM_BRKCTL4_5 address0x4005_90D0
     * |        |          |PWM_INTEN1 address0x4005_80E4
     * |        |          |PWM_INTEN1 address0x4005_90E4
     * |        |          |PWM_INTSTS1 address0x4005_80EC
     * |        |          |PWM_INTSTS1 address0x4005_90EC
     * @var SYS_T::MIRCTRIMCTL
     * Offset: 0x104  MIRC Trim Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |FREQSEL   |Trim Frequency Selection
     * |        |          |This field indicates the target frequency of medium speed RC oscillator (MIRC) auto trim.
     * |        |          |During auto trim operation, if clock error detected with CESTOPEN is set to 1 or trim retry limitation count reached, this field will be cleared to 00 automatically.
     * |        |          |00 = Disable HIRC auto trim function.
     * |        |          |01 = Reserved.
     * |        |          |10 = Enable HIRC auto trim function and trim MIRC to 4.032 MHz.
     * |[5:4]   |LOOPSEL   |Trim Calculation Loop Selection
     * |        |          |This field defines that trim value calculation is based on how many reference clocks.
     * |        |          |00 = Reserved.
     * |        |          |01 = Trim value calculation is based on average difference in 8 clocks of reference clock.
     * |        |          |10 = Trim value calculation is based on average difference in 16 clocks of reference clock.
     * |        |          |11 = Trim value calculation is based on average difference in 32 clocks of reference clock.
     * |        |          |Note: For example, if LOOPSEL is set as 00, auto trim circuit will calculate trim value based on the average frequency difference in 4 clocks of reference clock.
     * |[7:6]   |RETRYCNT  |Trim Value Update Limitation Count
     * |        |          |This field defines that how many times the auto trim circuit will try to update the MIRC trim value before the frequency of MIRC locked.
     * |        |          |Once the MIRC locked, the internal trim value update counter will be reset.
     * |        |          |If the trim value update counter reached this limitation value and frequency of MIRC still doesn't lock, the auto trim operation will be disabled and FREQSEL will be cleared to 00.
     * |        |          |00 = Trim retry count limitation is 64 loops.
     * |        |          |01 = Trim retry count limitation is 128 loops.
     * |        |          |10 = Trim retry count limitation is 256 loops.
     * |        |          |11 = Trim retry count limitation is 512 loops.
     * |[8]     |CESTOPEN  |Clock Error Stop Enable Bit
     * |        |          |0 = The trim operation is keep going if clock is inaccuracy.
     * |        |          |1 = The trim operation is stopped if clock is inaccuracy.
     * |[9]     |BOUNDEN   |Boundary Enable
     * |        |          |0 = Boundary function is disable.
     * |        |          |1 = Boundary function is enable.
     * |[10]    |REFCKSEL  |Reference Clock Selection
     * |        |          |0 = MIRC trim reference from external 32.768 kHz crystal oscillator.
     * |        |          |1 = MIRC trim reference from internal USB synchronous mode.
     * |[20:16] |BOUNDARY  |Boundary Selection
     * |        |          |Fill the boundary range from 0x1 to 0x1F, 0x0 is reserved.
     * |        |          |Note: This field is effective only when the BOUNDEN(SYS_MIRCTRIMCTL[9]) is enable
     * @var SYS_T::MIRCTRIMIEN
     * Offset: 0x108  MIRC Trim Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |TFALIEN   |Trim Failure Interrupt Enable Bit
     * |        |          |This bit controls if an interrupt will be triggered while MIRC trim value update limitation count reached and MIRC frequency still not locked on target frequency set by FREQSEL(SYS_ MIRCCTL[1:0]).
     * |        |          |If this bit is high and TFAILIF(SYS_ MIRCSTS[1]) is set during auto trim operation, an interrupt will be triggered to notify that MIRC trim value update limitation count was reached.
     * |        |          |0 = Disable TFAILIF(SYS_MIRCSTS[1]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable TFAILIF(SYS_MIRCSTS[1]) status to trigger an interrupt to CPU.
     * |[2]     |CLKEIEN   |Clock Error Interrupt Enable Bit
     * |        |          |This bit controls if CPU would get an interrupt while clock is inaccuracy during auto trim operation.
     * |        |          |If this bit is set to1, and CLKERRIF(SYS_MIRCSTS[2]) is set during auto trim operation, an interrupt will be triggered to notify the clock frequency is inaccuracy.
     * |        |          |0 = Disable CLKERRIF(SYS_MIRCSTS[2]) status to trigger an interrupt to CPU.
     * |        |          |1 = Enable CLKERRIF(SYS_MIRCSTS[2]) status to trigger an interrupt to CPU.
     * @var SYS_T::MIRCTRIMSTS
     * Offset: 0x10C  MIRC Trim Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FREQLOCK  |MIRC Frequency Lock Status
     * |        |          |This bit indicates the MIRC frequency is locked.
     * |        |          |This is a status bit and doesn't trigger any interrupt
     * |        |          |Write 1 to clear this to 0
     * |        |          |This bit will be set automatically, if the frequecy is lock and the RC_TRIM is enabled.
     * |        |          |0 = The internal medium-speed oscillator frequency doesn't lock yet.
     * |        |          |1 = The internal medium-speed oscillator frequency locked.
     * |[1]     |TFAILIF   |Trim Failure Interrupt Status
     * |        |          |This bit indicates that MIRC trim value update limitation count reached and the MIRC clock frequency still doesn't be locked
     * |        |          |Once this bit is set, the auto trim operation stopped and FREQSEL(SYS_MIRCCTL[1:0]) will be cleared to 00 by hardware automatically.
     * |        |          |If this bit is set and TFAILIEN(SYS_MIRCIEN[1]) is high, an interrupt will be triggered to notify that MIRC trim value update limitation count was reached
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Trim value update limitation count does not reach.
     * |        |          |1 = Trim value update limitation count reached and MIRC frequency still not locked.
     * |[2]     |CLKERIF   |Clock Error Interrupt Status
     * |        |          |When the frequency of 32.768 kHz external low speed crystal oscillator (LXT) or internal medium speed RC oscillator (MIRC) is shift larger to unreasonable value, this bit will be set and to be an indicate that clock frequency is inaccuracy
     * |        |          |Once this bit is set to 1, the auto trim operation stopped and FREQSEL(SYS_MIRCCTL[1:0]) will be cleared to 00 by hardware automatically if CESTOPEN(SYS_MIRCCTL[8]) is set to 1.
     * |        |          |If this bit is set and CLKEIEN(SYS_MIRCIEN[2]) is high, an interrupt will be triggered to notify the clock frequency is inaccuracy
     * |        |          |Write 1 to clear this to 0.
     * |        |          |0 = Clock frequency is accuracy.
     * |        |          |1 = Clock frequency is inaccuracy.
     * |[3]     |OVBDIF    |Over Boundary Status
     * |        |          |When the over boundary function is set, if there occurs the over boundary condition, this flag will be set.
     * |        |          |0 = Over boundary coundition did not occur.
     * |        |          |1 = Over boundary coundition occurred.
     * |        |          |Note: Write 1 to clear this flag.
     * @var SYS_T::RTCLVRIEN
     * Offset: 0x144  RTC LVR interrupt control register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |IE        |RTC LVR interrupt enable(Write Protect)
     * |        |          |0 : disable (default)
     * |        |          |1: enable
     * |        |          |Note: These bits are write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::RTCLVRSTS
     * Offset: 0x148  RTC LVR interrupt status register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |IF        |RTC LVR interrupt flag(Write Protect)
     * |        |          |When a positive edge of RTCLVR is detected, this bit will be set to 1 and will be cleared when a 1 is set to this bit.
     * |        |          |Note: These bits are write protected. Refer to the SYS_REGLCTL register.
     * |[8]     |RTCLVR    |RTCLVR value (read only)
     * @var SYS_T::PORCTL1
     * Offset: 0x1EC  Power-On-reset Controller Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |POROFF    |Power-on Reset Enable Bit (Write Protect)
     * |        |          |After powered on, User can turn off internal analog POR circuit to save power by writing 0x5AA5 to this field.
     * |        |          |The analog POR circuit will be active again when this field is set to another value or chip is reset by other reset source, including:
     * |        |          |nRESET, Watchdog, LVR reset, BOD reset, ICE reset command and the software-chip reset function.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var SYS_T::PLCTL
     * Offset: 0x1F8  Power Level Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |PLSEL     |Power Level Select(Write Protect)
     * |        |          |00 = Set to power level 0 (PL0)
     * |        |          |01 = Reserved
     * |        |          |10 = Set to power level 2 (PL2)
     * |        |          |11= Reserved
     * |        |          |Note : When at PL2, HCLK has to be lower than 4Mhz
     * @var SYS_T::PLSTS
     * Offset: 0x1FC  Power Level Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PLCBUSY   |Power Level Change Busy Bit (Read Only)
     * |        |          |This bit is set by hardware when power level is changing . After power level change is completed, this bit will be cleared automatically by hardware.
     * |        |          |0 = Power level change is completed.
     * |        |          |1 = Power level change is ongoing.
     * |[9:8]   |CURPL     |Current Power Level (Read Only)
     * |        |          |This bit field reflect the current power level.
     * |        |          |00 = Current power level is PL0.
     * |        |          |01 = Reserved.
     * |        |          |10 = Current power level is PL2.
     * |        |          |Others = Reserved.
     */
    __I  uint32_t PDID;                  /*!< [0x0000] Part Device Identification Number Register                       */
    __IO uint32_t RSTSTS;                /*!< [0x0004] System Reset Status Register                                     */
    __IO uint32_t IPRST0;                /*!< [0x0008] Peripheral  Reset Control Register 0                             */
    __IO uint32_t IPRST1;                /*!< [0x000c] Peripheral Reset Control Register 1                              */
    __IO uint32_t IPRST2;                /*!< [0x0010] Peripheral Reset Control Register 2                         */
    __I  uint32_t RESERVE0[1];
    __IO uint32_t BODCTL;                /*!< [0x0018] Brown-out Detector Control Register                              */
    __IO uint32_t IVSCTL;                /*!< [0x001c] Internal Voltage Source Control Register                         */
    __I  uint32_t RESERVE1[1];
    __IO uint32_t PORCTL0;               /*!< [0x0024] Power-On-reset Controller Register 0                              */
    __IO uint32_t VREFCTL;               /*!< [0x0028] VREF Control Register                                            */
    __I  uint32_t RESERVE2[1];
    __IO uint32_t GPA_MFPL;              /*!< [0x0030] GPIOA Low Byte Multiple Function Control Register                */
    __IO uint32_t GPA_MFPH;              /*!< [0x0034] GPIOA High Byte Multiple Function Control Register               */
    __IO uint32_t GPB_MFPL;              /*!< [0x0038] GPIOB Low Byte Multiple Function Control Register                */
    __IO uint32_t GPB_MFPH;              /*!< [0x003c] GPIOB High Byte Multiple Function Control Register               */
    __IO uint32_t GPC_MFPL;              /*!< [0x0040] GPIOC Low Byte Multiple Function Control Register                */
    __IO uint32_t GPC_MFPH;              /*!< [0x0044] GPIOC High Byte Multiple Function Control Register               */
    __IO uint32_t GPD_MFPL;              /*!< [0x0048] GPIOD Low Byte Multiple Function Control Register                */
    __IO uint32_t GPD_MFPH;              /*!< [0x004c] GPIOD High Byte Multiple Function Control Register               */
    __IO uint32_t GPE_MFPL;              /*!< [0x0050] GPIOE Low Byte Multiple Function Control Register                */
    __IO uint32_t GPE_MFPH;              /*!< [0x0054] GPIOE High Byte Multiple Function Control Register               */
    __IO uint32_t GPF_MFPL;              /*!< [0x0058] GPIOF Low Byte Multiple Function Control Register                */
    __IO uint32_t GPF_MFPH;              /*!< [0x005c] GPIOF High Byte Multiple Function Control Register               */
    __I  uint32_t RESERVE3[8];
    __IO uint32_t GPA_MFOS;              /*!< [0x0080] GPIOA Multiple Function Output Select Register                   */
    __IO uint32_t GPB_MFOS;              /*!< [0x0084] GPIOB Multiple Function Output Select Register                   */
    __IO uint32_t GPC_MFOS;              /*!< [0x0088] GPIOC Multiple Function Output Select Register                   */
    __IO uint32_t GPD_MFOS;              /*!< [0x008c] GPIOD Multiple Function Output Select Register                   */
    __IO uint32_t GPE_MFOS;              /*!< [0x0090] GPIOE Multiple Function Output Select Register                   */
    __IO uint32_t GPF_MFOS;              /*!< [0x0094] GPIOF Multiple Function Output Select Register                   */
    __I  uint32_t RESERVE4[10];
    __IO uint32_t MODCTL;                /*!< [0x00c0] Modulation Control Register                                      */
    __I  uint32_t RESERVE5[3];
    __IO uint32_t SRAM_BISTCTL;          /*!< [0x00d0] System SRAM BIST Test Control Register                           */
    __I  uint32_t SRAM_BISTSTS;          /*!< [0x00d4] System SRAM BIST Test Status Register                            */
    __I  uint32_t RESERVE6[6];
    __IO uint32_t HIRCTRIMCTL;           /*!< [0x00f0] HIRC Trim Control Register                                       */
    __IO uint32_t HIRCTRIMIEN;           /*!< [0x00f4] HIRC Trim Interrupt Enable Register                              */
    __IO uint32_t HIRCTRIMSTS;           /*!< [0x00f8] HIRC Trim Interrupt Status Register                              */
    __I  uint32_t RESERVE7[1];
    __O  uint32_t REGLCTL;               /*!< [0x0100] Register Lock Control Register                                   */
    __IO uint32_t MIRCTRIMCTL;           /*!< [0x0104] MIRC Trim Control Register                                       */
    __IO uint32_t MIRCTRIMIEN;           /*!< [0x0108] MIRC Trim Interrupt Enable Register                              */
    __IO uint32_t MIRCTRIMSTS;           /*!< [0x010c] MIRC Trim Interrupt Status Register                             */
    __I  uint32_t RESERVE8[13];
    __IO uint32_t RTCLVRIEN;             /*!< [0x0144] RTC LVR interrupt control register                              */
    __IO uint32_t RTCLVRSTS;             /*!< [0x0148] RTC LVR interrupt status register                             */
    __I  uint32_t RESERVE9[40];
    __IO uint32_t PORCTL1;               /*!< [0x01ec] Power-On-reset Controller Register 1                              */
    __I  uint32_t RESERVE10[2];
    __IO uint32_t PLCTL;                 /*!< [0x01F8] Power Level Control Register                              */
    __I  uint32_t PLSTS;                 /*!< [0x01Fc] Power Level Status Register                                      */
} SYS_T;

/**
    @addtogroup SYS_CONST SYS Bit Field Definition
    Constant Definitions for SYS Controller
@{ */

#define SYS_PDID_PDID_Pos                (0)                                               /*!< SYS_T::PDID: PDID Position             */
#define SYS_PDID_PDID_Msk                (0xfffffffful << SYS_PDID_PDID_Pos)               /*!< SYS_T::PDID: PDID Mask                 */

#define SYS_RSTSTS_PORF_Pos              (0)                                               /*!< SYS_T::RSTSTS: PORF Position           */
#define SYS_RSTSTS_PORF_Msk              (0x1ul << SYS_RSTSTS_PORF_Pos)                    /*!< SYS_T::RSTSTS: PORF Mask               */

#define SYS_RSTSTS_PINRF_Pos             (1)                                               /*!< SYS_T::RSTSTS: PINRF Position          */
#define SYS_RSTSTS_PINRF_Msk             (0x1ul << SYS_RSTSTS_PINRF_Pos)                   /*!< SYS_T::RSTSTS: PINRF Mask              */

#define SYS_RSTSTS_WDTRF_Pos             (2)                                               /*!< SYS_T::RSTSTS: WDTRF Position          */
#define SYS_RSTSTS_WDTRF_Msk             (0x1ul << SYS_RSTSTS_WDTRF_Pos)                   /*!< SYS_T::RSTSTS: WDTRF Mask              */

#define SYS_RSTSTS_LVRF_Pos              (3)                                               /*!< SYS_T::RSTSTS: LVRF Position           */
#define SYS_RSTSTS_LVRF_Msk              (0x1ul << SYS_RSTSTS_LVRF_Pos)                    /*!< SYS_T::RSTSTS: LVRF Mask               */

#define SYS_RSTSTS_BODRF_Pos             (4)                                               /*!< SYS_T::RSTSTS: BODRF Position          */
#define SYS_RSTSTS_BODRF_Msk             (0x1ul << SYS_RSTSTS_BODRF_Pos)                   /*!< SYS_T::RSTSTS: BODRF Mask              */

#define SYS_RSTSTS_SYSRF_Pos             (5)                                               /*!< SYS_T::RSTSTS: SYSRF Position          */
#define SYS_RSTSTS_SYSRF_Msk             (0x1ul << SYS_RSTSTS_SYSRF_Pos)                   /*!< SYS_T::RSTSTS: SYSRF Mask              */

#define SYS_RSTSTS_PMURF_Pos             (6)                                               /*!< SYS_T::RSTSTS: PMURF Position          */
#define SYS_RSTSTS_PMURF_Msk             (0x1ul << SYS_RSTSTS_PMURF_Pos)                   /*!< SYS_T::RSTSTS: PMURF Mask              */

#define SYS_RSTSTS_CPURF_Pos             (7)                                               /*!< SYS_T::RSTSTS: CPURF Position          */
#define SYS_RSTSTS_CPURF_Msk             (0x1ul << SYS_RSTSTS_CPURF_Pos)                   /*!< SYS_T::RSTSTS: CPURF Mask              */

#define SYS_RSTSTS_CPULKRF_Pos           (8)                                               /*!< SYS_T::RSTSTS: CPULKRF Position        */
#define SYS_RSTSTS_CPULKRF_Msk           (0x1ul << SYS_RSTSTS_CPULKRF_Pos)                 /*!< SYS_T::RSTSTS: CPULKRF Mask            */

#define SYS_RSTSTS_VBATLVRF_Pos          (9)                                               /*!< SYS_T::RSTSTS: VBATLVRF Position        */
#define SYS_RSTSTS_VBATLVRF_Msk          (0x1ul << SYS_RSTSTS_VBATLVRF_Pos)                /*!< SYS_T::RSTSTS: VBATLVRF Mask            */

#define SYS_IPRST0_CHIPRST_Pos           (0)                                               /*!< SYS_T::IPRST0: CHIPRST Position        */
#define SYS_IPRST0_CHIPRST_Msk           (0x1ul << SYS_IPRST0_CHIPRST_Pos)                 /*!< SYS_T::IPRST0: CHIPRST Mask            */

#define SYS_IPRST0_CPURST_Pos            (1)                                               /*!< SYS_T::IPRST0: CPURST Position         */
#define SYS_IPRST0_CPURST_Msk            (0x1ul << SYS_IPRST0_CPURST_Pos)                  /*!< SYS_T::IPRST0: CPURST Mask             */

#define SYS_IPRST0_PDMARST_Pos           (2)                                               /*!< SYS_T::IPRST0: PDMARST Position        */
#define SYS_IPRST0_PDMARST_Msk           (0x1ul << SYS_IPRST0_PDMARST_Pos)                 /*!< SYS_T::IPRST0: PDMARST Mask            */

#define SYS_IPRST0_EBIRST_Pos            (3)                                               /*!< SYS_T::IPRST0: EBIRST Position         */
#define SYS_IPRST0_EBIRST_Msk            (0x1ul << SYS_IPRST0_EBIRST_Pos)                  /*!< SYS_T::IPRST0: EBIRST Mask             */

#define SYS_IPRST0_CRCRST_Pos            (7)                                               /*!< SYS_T::IPRST0: CRCRST Position         */
#define SYS_IPRST0_CRCRST_Msk            (0x1ul << SYS_IPRST0_CRCRST_Pos)                  /*!< SYS_T::IPRST0: CRCRST Mask             */

#define SYS_IPRST0_CRYPTRST_Pos          (12)                                              /*!< SYS_T::IPRST0: CRYPTRST Position       */
#define SYS_IPRST0_CRYPTRST_Msk          (0x1ul << SYS_IPRST0_CRYPTRST_Pos)                /*!< SYS_T::IPRST0: CRYPTRST Mask           */

#define SYS_IPRST1_GPIORST_Pos           (1)                                               /*!< SYS_T::IPRST1: GPIORST Position        */
#define SYS_IPRST1_GPIORST_Msk           (0x1ul << SYS_IPRST1_GPIORST_Pos)                 /*!< SYS_T::IPRST1: GPIORST Mask            */

#define SYS_IPRST1_TMR0RST_Pos           (2)                                               /*!< SYS_T::IPRST1: TMR0RST Position        */
#define SYS_IPRST1_TMR0RST_Msk           (0x1ul << SYS_IPRST1_TMR0RST_Pos)                 /*!< SYS_T::IPRST1: TMR0RST Mask            */

#define SYS_IPRST1_TMR1RST_Pos           (3)                                               /*!< SYS_T::IPRST1: TMR1RST Position        */
#define SYS_IPRST1_TMR1RST_Msk           (0x1ul << SYS_IPRST1_TMR1RST_Pos)                 /*!< SYS_T::IPRST1: TMR1RST Mask            */

#define SYS_IPRST1_TMR2RST_Pos           (4)                                               /*!< SYS_T::IPRST1: TMR2RST Position        */
#define SYS_IPRST1_TMR2RST_Msk           (0x1ul << SYS_IPRST1_TMR2RST_Pos)                 /*!< SYS_T::IPRST1: TMR2RST Mask            */

#define SYS_IPRST1_TMR3RST_Pos           (5)                                               /*!< SYS_T::IPRST1: TMR3RST Position        */
#define SYS_IPRST1_TMR3RST_Msk           (0x1ul << SYS_IPRST1_TMR3RST_Pos)                 /*!< SYS_T::IPRST1: TMR3RST Mask            */

#define SYS_IPRST1_ACMP01RST_Pos         (7)                                               /*!< SYS_T::IPRST1: ACMP01RST Position      */
#define SYS_IPRST1_ACMP01RST_Msk         (0x1ul << SYS_IPRST1_ACMP01RST_Pos)               /*!< SYS_T::IPRST1: ACMP01RST Mask          */

#define SYS_IPRST1_I2C0RST_Pos           (8)                                               /*!< SYS_T::IPRST1: I2C0RST Position        */
#define SYS_IPRST1_I2C0RST_Msk           (0x1ul << SYS_IPRST1_I2C0RST_Pos)                 /*!< SYS_T::IPRST1: I2C0RST Mask            */

#define SYS_IPRST1_I2C1RST_Pos           (9)                                               /*!< SYS_T::IPRST1: I2C1RST Position        */
#define SYS_IPRST1_I2C1RST_Msk           (0x1ul << SYS_IPRST1_I2C1RST_Pos)                 /*!< SYS_T::IPRST1: I2C1RST Mask            */

#define SYS_IPRST1_QSPI0RST_Pos          (12)                                              /*!< SYS_T::IPRST1: QSPI0RST Position       */
#define SYS_IPRST1_QSPI0RST_Msk          (0x1ul << SYS_IPRST1_QSPI0RST_Pos)                /*!< SYS_T::IPRST1: QSPI0RST Mask           */

#define SYS_IPRST1_SPI0RST_Pos           (13)                                              /*!< SYS_T::IPRST1: SPI0RST Position        */
#define SYS_IPRST1_SPI0RST_Msk           (0x1ul << SYS_IPRST1_SPI0RST_Pos)                 /*!< SYS_T::IPRST1: SPI0RST Mask            */

#define SYS_IPRST1_UART0RST_Pos          (16)                                              /*!< SYS_T::IPRST1: UART0RST Position       */
#define SYS_IPRST1_UART0RST_Msk          (0x1ul << SYS_IPRST1_UART0RST_Pos)                /*!< SYS_T::IPRST1: UART0RST Mask           */

#define SYS_IPRST1_UART1RST_Pos          (17)                                              /*!< SYS_T::IPRST1: UART1RST Position       */
#define SYS_IPRST1_UART1RST_Msk          (0x1ul << SYS_IPRST1_UART1RST_Pos)                /*!< SYS_T::IPRST1: UART1RST Mask           */

#define SYS_IPRST1_UART2RST_Pos          (18)                                              /*!< SYS_T::IPRST1: UART2RST Position       */
#define SYS_IPRST1_UART2RST_Msk          (0x1ul << SYS_IPRST1_UART2RST_Pos)                /*!< SYS_T::IPRST1: UART2RST Mask           */

#define SYS_IPRST1_USBDRST_Pos           (27)                                              /*!< SYS_T::IPRST1: USBDRST Position        */
#define SYS_IPRST1_USBDRST_Msk           (0x1ul << SYS_IPRST1_USBDRST_Pos)                 /*!< SYS_T::IPRST1: USBDRST Mask            */

#define SYS_IPRST1_EADCRST_Pos           (28)                                              /*!< SYS_T::IPRST1: EADCRST Position        */
#define SYS_IPRST1_EADCRST_Msk           (0x1ul << SYS_IPRST1_EADCRST_Pos)                 /*!< SYS_T::IPRST1: EADCRST Mask            */

#define SYS_IPRST2_SC0RST_Pos            (0)                                               /*!< SYS_T::IPRST2: SC0RST Position         */
#define SYS_IPRST2_SC0RST_Msk            (0x1ul << SYS_IPRST2_SC0RST_Pos)                  /*!< SYS_T::IPRST2: SC0RST Mask             */

#define SYS_IPRST2_USCI0RST_Pos          (8)                                               /*!< SYS_T::IPRST2: USCI0RST Position       */
#define SYS_IPRST2_USCI0RST_Msk          (0x1ul << SYS_IPRST2_USCI0RST_Pos)                /*!< SYS_T::IPRST2: USCI0RST Mask           */

#define SYS_IPRST2_USCI1RST_Pos          (9)                                               /*!< SYS_T::IPRST2: USCI1RST Position       */
#define SYS_IPRST2_USCI1RST_Msk          (0x1ul << SYS_IPRST2_USCI1RST_Pos)                /*!< SYS_T::IPRST2: USCI1RST Mask           */

#define SYS_IPRST2_USCI2RST_Pos          (10)                                              /*!< SYS_T::IPRST2: USCI2RST Position       */
#define SYS_IPRST2_USCI2RST_Msk          (0x1ul << SYS_IPRST2_USCI2RST_Pos)                /*!< SYS_T::IPRST2: USCI2RST Mask           */

#define SYS_IPRST2_DACRST_Pos            (12)                                              /*!< SYS_T::IPRST2: DACRST Position         */
#define SYS_IPRST2_DACRST_Msk            (0x1ul << SYS_IPRST2_DACRST_Pos)                  /*!< SYS_T::IPRST2: DACRST Mask             */

#define SYS_IPRST2_PWM0RST_Pos           (16)                                              /*!< SYS_T::IPRST2: PWM0RST Position        */
#define SYS_IPRST2_PWM0RST_Msk           (0x1ul << SYS_IPRST2_PWM0RST_Pos)                 /*!< SYS_T::IPRST2: PWM0RST Mask            */

#define SYS_IPRST2_PWM1RST_Pos           (17)                                              /*!< SYS_T::IPRST2: PWM1RST Position        */
#define SYS_IPRST2_PWM1RST_Msk           (0x1ul << SYS_IPRST2_PWM1RST_Pos)                 /*!< SYS_T::IPRST2: PWM1RST Mask            */

#define SYS_IPRST2_BPWM0RST_Pos          (18)                                              /*!< SYS_T::IPRST2: BPWM0RST Position       */
#define SYS_IPRST2_BPWM0RST_Msk          (0x1ul << SYS_IPRST2_BPWM0RST_Pos)                /*!< SYS_T::IPRST2: BPWM0RST Mask           */

#define SYS_IPRST2_BPWM1RST_Pos          (19)                                              /*!< SYS_T::IPRST2: BPWM1RST Position       */
#define SYS_IPRST2_BPWM1RST_Msk          (0x1ul << SYS_IPRST2_BPWM1RST_Pos)                /*!< SYS_T::IPRST2: BPWM1RST Mask           */

#define SYS_IPRST2_OPARST_Pos            (30)                                              /*!< SYS_T::IPRST2: OPARST Position         */
#define SYS_IPRST2_OPARST_Msk            (0x1ul << SYS_IPRST2_OPARST_Pos)                  /*!< SYS_T::IPRST2: OPARST Mask             */

#define SYS_IPRST2_PSIORST_Pos           (31)                                              /*!< SYS_T::IPRST2: PSIORST Position        */
#define SYS_IPRST2_PSIORST_Msk           (0x1ul << SYS_IPRST2_PSIORST_Pos)                 /*!< SYS_T::IPRST2: PSIORST Mask            */

#define SYS_BODCTL_BODEN_Pos             (0)                                               /*!< SYS_T::BODCTL: BODEN Position          */
#define SYS_BODCTL_BODEN_Msk             (0x1ul << SYS_BODCTL_BODEN_Pos)                   /*!< SYS_T::BODCTL: BODEN Mask              */

#define SYS_BODCTL_BODRSTEN_Pos          (3)                                               /*!< SYS_T::BODCTL: BODRSTEN Position       */
#define SYS_BODCTL_BODRSTEN_Msk          (0x1ul << SYS_BODCTL_BODRSTEN_Pos)                /*!< SYS_T::BODCTL: BODRSTEN Mask           */

#define SYS_BODCTL_BODIF_Pos             (4)                                               /*!< SYS_T::BODCTL: BODIF Position          */
#define SYS_BODCTL_BODIF_Msk             (0x1ul << SYS_BODCTL_BODIF_Pos)                   /*!< SYS_T::BODCTL: BODIF Mask              */

#define SYS_BODCTL_BODLPM_Pos            (5)                                               /*!< SYS_T::BODCTL: BODLPM Position         */
#define SYS_BODCTL_BODLPM_Msk            (0x1ul << SYS_BODCTL_BODLPM_Pos)                  /*!< SYS_T::BODCTL: BODLPM Mask             */

#define SYS_BODCTL_BODOUT_Pos            (6)                                               /*!< SYS_T::BODCTL: BODOUT Position         */
#define SYS_BODCTL_BODOUT_Msk            (0x1ul << SYS_BODCTL_BODOUT_Pos)                  /*!< SYS_T::BODCTL: BODOUT Mask             */

#define SYS_BODCTL_LVREN_Pos             (7)                                               /*!< SYS_T::BODCTL: LVREN Position          */
#define SYS_BODCTL_LVREN_Msk             (0x1ul << SYS_BODCTL_LVREN_Pos)                   /*!< SYS_T::BODCTL: LVREN Mask              */

#define SYS_BODCTL_BODDGSEL_Pos          (8)                                               /*!< SYS_T::BODCTL: BODDGSEL Position       */
#define SYS_BODCTL_BODDGSEL_Msk          (0x7ul << SYS_BODCTL_BODDGSEL_Pos)                /*!< SYS_T::BODCTL: BODDGSEL Mask           */

#define SYS_BODCTL_LVRDGSEL_Pos          (12)                                              /*!< SYS_T::BODCTL: LVRDGSEL Position       */
#define SYS_BODCTL_LVRDGSEL_Msk          (0x7ul << SYS_BODCTL_LVRDGSEL_Pos)                /*!< SYS_T::BODCTL: LVRDGSEL Mask           */

#define SYS_BODCTL_BODVL_Pos             (16)                                              /*!< SYS_T::BODCTL: BODVL Position          */
#define SYS_BODCTL_BODVL_Msk             (0x7ul << SYS_BODCTL_BODVL_Pos)                   /*!< SYS_T::BODCTL: BODVL Mask              */

#define SYS_IVSCTL_VTEMPEN_Pos           (0)                                               /*!< SYS_T::IVSCTL: VTEMPEN Position        */
#define SYS_IVSCTL_VTEMPEN_Msk           (0x1ul << SYS_IVSCTL_VTEMPEN_Pos)                 /*!< SYS_T::IVSCTL: VTEMPEN Mask            */

#define SYS_IVSCTL_VBATUGEN_Pos          (1)                                               /*!< SYS_T::IVSCTL: VBATUGEN Position       */
#define SYS_IVSCTL_VBATUGEN_Msk          (0x1ul << SYS_IVSCTL_VBATUGEN_Pos)                /*!< SYS_T::IVSCTL: VBATUGEN Mask           */

#define SYS_PORCTL0_PORMASK_Pos          (0)                                               /*!< SYS_T::PORCTL0: PORMASK Position         */
#define SYS_PORCTL0_PORMASK_Msk          (0xfffful << SYS_PORCTL0_PORMASK_Pos)             /*!< SYS_T::PORCTL0: PORMASK Mask             */

#define SYS_VREFCTL_IVRS_Pos             (0)                                               /*!< SYS_T::VREFCTL: IVRS Position          */
#define SYS_VREFCTL_IVRS_Msk             (0x7ul << SYS_VREFCTL_IVRS_Pos)                   /*!< SYS_T::VREFCTL: IVRS Mask              */

#define SYS_VREFCTL_IVREN_Pos            (3)                                               /*!< SYS_T::VREFCTL: IVREN Position         */
#define SYS_VREFCTL_IVREN_Msk            (0x1ul << SYS_VREFCTL_IVREN_Pos)                  /*!< SYS_T::VREFCTL: IVREN Mask             */

#define SYS_VREFCTL_PRELOAD_Pos          (10)                                              /*!< SYS_T::VREFCTL: PRELOAD Position       */
#define SYS_VREFCTL_PRELOAD_Msk          (0x1ul << SYS_VREFCTL_PRELOAD_Pos)                /*!< SYS_T::VREFCTL: PRELOAD Mask           */

#define SYS_GPA_MFPL_PA0MFP_Pos          (0)                                               /*!< SYS_T::GPA_MFPL: PA0MFP Position       */
#define SYS_GPA_MFPL_PA0MFP_Msk          (0xful << SYS_GPA_MFPL_PA0MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA0MFP Mask           */

#define SYS_GPA_MFPL_PA1MFP_Pos          (4)                                               /*!< SYS_T::GPA_MFPL: PA1MFP Position       */
#define SYS_GPA_MFPL_PA1MFP_Msk          (0xful << SYS_GPA_MFPL_PA1MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA1MFP Mask           */

#define SYS_GPA_MFPL_PA2MFP_Pos          (8)                                               /*!< SYS_T::GPA_MFPL: PA2MFP Position       */
#define SYS_GPA_MFPL_PA2MFP_Msk          (0xful << SYS_GPA_MFPL_PA2MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA2MFP Mask           */

#define SYS_GPA_MFPL_PA3MFP_Pos          (12)                                              /*!< SYS_T::GPA_MFPL: PA3MFP Position       */
#define SYS_GPA_MFPL_PA3MFP_Msk          (0xful << SYS_GPA_MFPL_PA3MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA3MFP Mask           */

#define SYS_GPA_MFPL_PA4MFP_Pos          (16)                                              /*!< SYS_T::GPA_MFPL: PA4MFP Position       */
#define SYS_GPA_MFPL_PA4MFP_Msk          (0xful << SYS_GPA_MFPL_PA4MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA4MFP Mask           */

#define SYS_GPA_MFPL_PA5MFP_Pos          (20)                                              /*!< SYS_T::GPA_MFPL: PA5MFP Position       */
#define SYS_GPA_MFPL_PA5MFP_Msk          (0xful << SYS_GPA_MFPL_PA5MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA5MFP Mask           */

#define SYS_GPA_MFPL_PA6MFP_Pos          (24)                                              /*!< SYS_T::GPA_MFPL: PA6MFP Position       */
#define SYS_GPA_MFPL_PA6MFP_Msk          (0xful << SYS_GPA_MFPL_PA6MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA6MFP Mask           */

#define SYS_GPA_MFPL_PA7MFP_Pos          (28)                                              /*!< SYS_T::GPA_MFPL: PA7MFP Position       */
#define SYS_GPA_MFPL_PA7MFP_Msk          (0xful << SYS_GPA_MFPL_PA7MFP_Pos)                /*!< SYS_T::GPA_MFPL: PA7MFP Mask           */

#define SYS_GPA_MFPH_PA8MFP_Pos          (0)                                               /*!< SYS_T::GPA_MFPH: PA8MFP Position       */
#define SYS_GPA_MFPH_PA8MFP_Msk          (0xful << SYS_GPA_MFPH_PA8MFP_Pos)                /*!< SYS_T::GPA_MFPH: PA8MFP Mask           */

#define SYS_GPA_MFPH_PA9MFP_Pos          (4)                                               /*!< SYS_T::GPA_MFPH: PA9MFP Position       */
#define SYS_GPA_MFPH_PA9MFP_Msk          (0xful << SYS_GPA_MFPH_PA9MFP_Pos)                /*!< SYS_T::GPA_MFPH: PA9MFP Mask           */

#define SYS_GPA_MFPH_PA10MFP_Pos         (8)                                               /*!< SYS_T::GPA_MFPH: PA10MFP Position      */
#define SYS_GPA_MFPH_PA10MFP_Msk         (0xful << SYS_GPA_MFPH_PA10MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA10MFP Mask          */

#define SYS_GPA_MFPH_PA11MFP_Pos         (12)                                              /*!< SYS_T::GPA_MFPH: PA11MFP Position      */
#define SYS_GPA_MFPH_PA11MFP_Msk         (0xful << SYS_GPA_MFPH_PA11MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA11MFP Mask          */

#define SYS_GPA_MFPH_PA12MFP_Pos         (16)                                              /*!< SYS_T::GPA_MFPH: PA12MFP Position      */
#define SYS_GPA_MFPH_PA12MFP_Msk         (0xful << SYS_GPA_MFPH_PA12MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA12MFP Mask          */

#define SYS_GPA_MFPH_PA13MFP_Pos         (20)                                              /*!< SYS_T::GPA_MFPH: PA13MFP Position      */
#define SYS_GPA_MFPH_PA13MFP_Msk         (0xful << SYS_GPA_MFPH_PA13MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA13MFP Mask          */

#define SYS_GPA_MFPH_PA14MFP_Pos         (24)                                              /*!< SYS_T::GPA_MFPH: PA14MFP Position      */
#define SYS_GPA_MFPH_PA14MFP_Msk         (0xful << SYS_GPA_MFPH_PA14MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA14MFP Mask          */

#define SYS_GPA_MFPH_PA15MFP_Pos         (28)                                              /*!< SYS_T::GPA_MFPH: PA15MFP Position      */
#define SYS_GPA_MFPH_PA15MFP_Msk         (0xful << SYS_GPA_MFPH_PA15MFP_Pos)               /*!< SYS_T::GPA_MFPH: PA15MFP Mask          */

#define SYS_GPB_MFPL_PB0MFP_Pos          (0)                                               /*!< SYS_T::GPB_MFPL: PB0MFP Position       */
#define SYS_GPB_MFPL_PB0MFP_Msk          (0xful << SYS_GPB_MFPL_PB0MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB0MFP Mask           */

#define SYS_GPB_MFPL_PB1MFP_Pos          (4)                                               /*!< SYS_T::GPB_MFPL: PB1MFP Position       */
#define SYS_GPB_MFPL_PB1MFP_Msk          (0xful << SYS_GPB_MFPL_PB1MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB1MFP Mask           */

#define SYS_GPB_MFPL_PB2MFP_Pos          (8)                                               /*!< SYS_T::GPB_MFPL: PB2MFP Position       */
#define SYS_GPB_MFPL_PB2MFP_Msk          (0xful << SYS_GPB_MFPL_PB2MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB2MFP Mask           */

#define SYS_GPB_MFPL_PB3MFP_Pos          (12)                                              /*!< SYS_T::GPB_MFPL: PB3MFP Position       */
#define SYS_GPB_MFPL_PB3MFP_Msk          (0xful << SYS_GPB_MFPL_PB3MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB3MFP Mask           */

#define SYS_GPB_MFPL_PB4MFP_Pos          (16)                                              /*!< SYS_T::GPB_MFPL: PB4MFP Position       */
#define SYS_GPB_MFPL_PB4MFP_Msk          (0xful << SYS_GPB_MFPL_PB4MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB4MFP Mask           */

#define SYS_GPB_MFPL_PB5MFP_Pos          (20)                                              /*!< SYS_T::GPB_MFPL: PB5MFP Position       */
#define SYS_GPB_MFPL_PB5MFP_Msk          (0xful << SYS_GPB_MFPL_PB5MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB5MFP Mask           */

#define SYS_GPB_MFPL_PB6MFP_Pos          (24)                                              /*!< SYS_T::GPB_MFPL: PB6MFP Position       */
#define SYS_GPB_MFPL_PB6MFP_Msk          (0xful << SYS_GPB_MFPL_PB6MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB6MFP Mask           */

#define SYS_GPB_MFPL_PB7MFP_Pos          (28)                                              /*!< SYS_T::GPB_MFPL: PB7MFP Position       */
#define SYS_GPB_MFPL_PB7MFP_Msk          (0xful << SYS_GPB_MFPL_PB7MFP_Pos)                /*!< SYS_T::GPB_MFPL: PB7MFP Mask           */

#define SYS_GPB_MFPH_PB8MFP_Pos          (0)                                               /*!< SYS_T::GPB_MFPH: PB8MFP Position       */
#define SYS_GPB_MFPH_PB8MFP_Msk          (0xful << SYS_GPB_MFPH_PB8MFP_Pos)                /*!< SYS_T::GPB_MFPH: PB8MFP Mask           */

#define SYS_GPB_MFPH_PB9MFP_Pos          (4)                                               /*!< SYS_T::GPB_MFPH: PB9MFP Position       */
#define SYS_GPB_MFPH_PB9MFP_Msk          (0xful << SYS_GPB_MFPH_PB9MFP_Pos)                /*!< SYS_T::GPB_MFPH: PB9MFP Mask           */

#define SYS_GPB_MFPH_PB10MFP_Pos         (8)                                               /*!< SYS_T::GPB_MFPH: PB10MFP Position      */
#define SYS_GPB_MFPH_PB10MFP_Msk         (0xful << SYS_GPB_MFPH_PB10MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB10MFP Mask          */

#define SYS_GPB_MFPH_PB11MFP_Pos         (12)                                              /*!< SYS_T::GPB_MFPH: PB11MFP Position      */
#define SYS_GPB_MFPH_PB11MFP_Msk         (0xful << SYS_GPB_MFPH_PB11MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB11MFP Mask          */

#define SYS_GPB_MFPH_PB12MFP_Pos         (16)                                              /*!< SYS_T::GPB_MFPH: PB12MFP Position      */
#define SYS_GPB_MFPH_PB12MFP_Msk         (0xful << SYS_GPB_MFPH_PB12MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB12MFP Mask          */

#define SYS_GPB_MFPH_PB13MFP_Pos         (20)                                              /*!< SYS_T::GPB_MFPH: PB13MFP Position      */
#define SYS_GPB_MFPH_PB13MFP_Msk         (0xful << SYS_GPB_MFPH_PB13MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB13MFP Mask          */

#define SYS_GPB_MFPH_PB14MFP_Pos         (24)                                              /*!< SYS_T::GPB_MFPH: PB14MFP Position      */
#define SYS_GPB_MFPH_PB14MFP_Msk         (0xful << SYS_GPB_MFPH_PB14MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB14MFP Mask          */

#define SYS_GPB_MFPH_PB15MFP_Pos         (28)                                              /*!< SYS_T::GPB_MFPH: PB15MFP Position      */
#define SYS_GPB_MFPH_PB15MFP_Msk         (0xful << SYS_GPB_MFPH_PB15MFP_Pos)               /*!< SYS_T::GPB_MFPH: PB15MFP Mask          */

#define SYS_GPC_MFPL_PC0MFP_Pos          (0)                                               /*!< SYS_T::GPC_MFPL: PC0MFP Position       */
#define SYS_GPC_MFPL_PC0MFP_Msk          (0xful << SYS_GPC_MFPL_PC0MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC0MFP Mask           */

#define SYS_GPC_MFPL_PC1MFP_Pos          (4)                                               /*!< SYS_T::GPC_MFPL: PC1MFP Position       */
#define SYS_GPC_MFPL_PC1MFP_Msk          (0xful << SYS_GPC_MFPL_PC1MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC1MFP Mask           */

#define SYS_GPC_MFPL_PC2MFP_Pos          (8)                                               /*!< SYS_T::GPC_MFPL: PC2MFP Position       */
#define SYS_GPC_MFPL_PC2MFP_Msk          (0xful << SYS_GPC_MFPL_PC2MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC2MFP Mask           */

#define SYS_GPC_MFPL_PC3MFP_Pos          (12)                                              /*!< SYS_T::GPC_MFPL: PC3MFP Position       */
#define SYS_GPC_MFPL_PC3MFP_Msk          (0xful << SYS_GPC_MFPL_PC3MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC3MFP Mask           */

#define SYS_GPC_MFPL_PC4MFP_Pos          (16)                                              /*!< SYS_T::GPC_MFPL: PC4MFP Position       */
#define SYS_GPC_MFPL_PC4MFP_Msk          (0xful << SYS_GPC_MFPL_PC4MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC4MFP Mask           */

#define SYS_GPC_MFPL_PC5MFP_Pos          (20)                                              /*!< SYS_T::GPC_MFPL: PC5MFP Position       */
#define SYS_GPC_MFPL_PC5MFP_Msk          (0xful << SYS_GPC_MFPL_PC5MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC5MFP Mask           */

#define SYS_GPC_MFPL_PC6MFP_Pos          (24)                                              /*!< SYS_T::GPC_MFPL: PC6MFP Position       */
#define SYS_GPC_MFPL_PC6MFP_Msk          (0xful << SYS_GPC_MFPL_PC6MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC6MFP Mask           */

#define SYS_GPC_MFPL_PC7MFP_Pos          (28)                                              /*!< SYS_T::GPC_MFPL: PC7MFP Position       */
#define SYS_GPC_MFPL_PC7MFP_Msk          (0xful << SYS_GPC_MFPL_PC7MFP_Pos)                /*!< SYS_T::GPC_MFPL: PC7MFP Mask           */

#define SYS_GPC_MFPH_PC8MFP_Pos          (0)                                               /*!< SYS_T::GPC_MFPH: PC8MFP Position       */
#define SYS_GPC_MFPH_PC8MFP_Msk          (0xful << SYS_GPC_MFPH_PC8MFP_Pos)                /*!< SYS_T::GPC_MFPH: PC8MFP Mask           */

#define SYS_GPC_MFPH_PC9MFP_Pos          (4)                                               /*!< SYS_T::GPC_MFPH: PC9MFP Position       */
#define SYS_GPC_MFPH_PC9MFP_Msk          (0xful << SYS_GPC_MFPH_PC9MFP_Pos)                /*!< SYS_T::GPC_MFPH: PC9MFP Mask           */

#define SYS_GPC_MFPH_PC10MFP_Pos         (8)                                               /*!< SYS_T::GPC_MFPH: PC10MFP Position      */
#define SYS_GPC_MFPH_PC10MFP_Msk         (0xful << SYS_GPC_MFPH_PC10MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC10MFP Mask          */

#define SYS_GPC_MFPH_PC11MFP_Pos         (12)                                              /*!< SYS_T::GPC_MFPH: PC11MFP Position      */
#define SYS_GPC_MFPH_PC11MFP_Msk         (0xful << SYS_GPC_MFPH_PC11MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC11MFP Mask          */

#define SYS_GPC_MFPH_PC12MFP_Pos         (16)                                              /*!< SYS_T::GPC_MFPH: PC12MFP Position      */
#define SYS_GPC_MFPH_PC12MFP_Msk         (0xful << SYS_GPC_MFPH_PC12MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC12MFP Mask          */

#define SYS_GPC_MFPH_PC13MFP_Pos         (20)                                              /*!< SYS_T::GPC_MFPH: PC13MFP Position      */
#define SYS_GPC_MFPH_PC13MFP_Msk         (0xful << SYS_GPC_MFPH_PC13MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC13MFP Mask          */

#define SYS_GPC_MFPH_PC14MFP_Pos         (24)                                              /*!< SYS_T::GPC_MFPH: PC14MFP Position      */
#define SYS_GPC_MFPH_PC14MFP_Msk         (0xful << SYS_GPC_MFPH_PC14MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC14MFP Mask          */

#define SYS_GPC_MFPH_PC15MFP_Pos         (28)                                              /*!< SYS_T::GPC_MFPH: PC15MFP Position      */
#define SYS_GPC_MFPH_PC15MFP_Msk         (0xful << SYS_GPC_MFPH_PC15MFP_Pos)               /*!< SYS_T::GPC_MFPH: PC15MFP Mask          */

#define SYS_GPD_MFPL_PD0MFP_Pos          (0)                                               /*!< SYS_T::GPD_MFPL: PD0MFP Position       */
#define SYS_GPD_MFPL_PD0MFP_Msk          (0xful << SYS_GPD_MFPL_PD0MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD0MFP Mask           */

#define SYS_GPD_MFPL_PD1MFP_Pos          (4)                                               /*!< SYS_T::GPD_MFPL: PD1MFP Position       */
#define SYS_GPD_MFPL_PD1MFP_Msk          (0xful << SYS_GPD_MFPL_PD1MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD1MFP Mask           */

#define SYS_GPD_MFPL_PD2MFP_Pos          (8)                                               /*!< SYS_T::GPD_MFPL: PD2MFP Position       */
#define SYS_GPD_MFPL_PD2MFP_Msk          (0xful << SYS_GPD_MFPL_PD2MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD2MFP Mask           */

#define SYS_GPD_MFPL_PD3MFP_Pos          (12)                                              /*!< SYS_T::GPD_MFPL: PD3MFP Position       */
#define SYS_GPD_MFPL_PD3MFP_Msk          (0xful << SYS_GPD_MFPL_PD3MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD3MFP Mask           */

#define SYS_GPD_MFPL_PD4MFP_Pos          (16)                                              /*!< SYS_T::GPD_MFPL: PD4MFP Position       */
#define SYS_GPD_MFPL_PD4MFP_Msk          (0xful << SYS_GPD_MFPL_PD4MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD4MFP Mask           */

#define SYS_GPD_MFPL_PD5MFP_Pos          (20)                                              /*!< SYS_T::GPD_MFPL: PD5MFP Position       */
#define SYS_GPD_MFPL_PD5MFP_Msk          (0xful << SYS_GPD_MFPL_PD5MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD5MFP Mask           */

#define SYS_GPD_MFPL_PD6MFP_Pos          (24)                                              /*!< SYS_T::GPD_MFPL: PD6MFP Position       */
#define SYS_GPD_MFPL_PD6MFP_Msk          (0xful << SYS_GPD_MFPL_PD6MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD6MFP Mask           */

#define SYS_GPD_MFPL_PD7MFP_Pos          (28)                                              /*!< SYS_T::GPD_MFPL: PD7MFP Position       */
#define SYS_GPD_MFPL_PD7MFP_Msk          (0xful << SYS_GPD_MFPL_PD7MFP_Pos)                /*!< SYS_T::GPD_MFPL: PD7MFP Mask           */

#define SYS_GPD_MFPH_PD8MFP_Pos          (0)                                               /*!< SYS_T::GPD_MFPH: PD8MFP Position       */
#define SYS_GPD_MFPH_PD8MFP_Msk          (0xful << SYS_GPD_MFPH_PD8MFP_Pos)                /*!< SYS_T::GPD_MFPH: PD8MFP Mask           */

#define SYS_GPD_MFPH_PD9MFP_Pos          (4)                                               /*!< SYS_T::GPD_MFPH: PD9MFP Position       */
#define SYS_GPD_MFPH_PD9MFP_Msk          (0xful << SYS_GPD_MFPH_PD9MFP_Pos)                /*!< SYS_T::GPD_MFPH: PD9MFP Mask           */

#define SYS_GPD_MFPH_PD10MFP_Pos         (8)                                               /*!< SYS_T::GPD_MFPH: PD10MFP Position      */
#define SYS_GPD_MFPH_PD10MFP_Msk         (0xful << SYS_GPD_MFPH_PD10MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD10MFP Mask          */

#define SYS_GPD_MFPH_PD11MFP_Pos         (12)                                              /*!< SYS_T::GPD_MFPH: PD11MFP Position      */
#define SYS_GPD_MFPH_PD11MFP_Msk         (0xful << SYS_GPD_MFPH_PD11MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD11MFP Mask          */

#define SYS_GPD_MFPH_PD12MFP_Pos         (16)                                              /*!< SYS_T::GPD_MFPH: PD12MFP Position      */
#define SYS_GPD_MFPH_PD12MFP_Msk         (0xful << SYS_GPD_MFPH_PD12MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD12MFP Mask          */

#define SYS_GPD_MFPH_PD13MFP_Pos         (20)                                              /*!< SYS_T::GPD_MFPH: PD13MFP Position      */
#define SYS_GPD_MFPH_PD13MFP_Msk         (0xful << SYS_GPD_MFPH_PD13MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD13MFP Mask          */

#define SYS_GPD_MFPH_PD14MFP_Pos         (24)                                              /*!< SYS_T::GPD_MFPH: PD14MFP Position      */
#define SYS_GPD_MFPH_PD14MFP_Msk         (0xful << SYS_GPD_MFPH_PD14MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD14MFP Mask          */

#define SYS_GPD_MFPH_PD15MFP_Pos         (28)                                              /*!< SYS_T::GPD_MFPH: PD15MFP Position      */
#define SYS_GPD_MFPH_PD15MFP_Msk         (0xful << SYS_GPD_MFPH_PD15MFP_Pos)               /*!< SYS_T::GPD_MFPH: PD15MFP Mask          */

#define SYS_GPE_MFPL_PE0MFP_Pos          (0)                                               /*!< SYS_T::GPE_MFPL: PE0MFP Position       */
#define SYS_GPE_MFPL_PE0MFP_Msk          (0xful << SYS_GPE_MFPL_PE0MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE0MFP Mask           */

#define SYS_GPE_MFPL_PE1MFP_Pos          (4)                                               /*!< SYS_T::GPE_MFPL: PE1MFP Position       */
#define SYS_GPE_MFPL_PE1MFP_Msk          (0xful << SYS_GPE_MFPL_PE1MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE1MFP Mask           */

#define SYS_GPE_MFPL_PE2MFP_Pos          (8)                                               /*!< SYS_T::GPE_MFPL: PE2MFP Position       */
#define SYS_GPE_MFPL_PE2MFP_Msk          (0xful << SYS_GPE_MFPL_PE2MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE2MFP Mask           */

#define SYS_GPE_MFPL_PE3MFP_Pos          (12)                                              /*!< SYS_T::GPE_MFPL: PE3MFP Position       */
#define SYS_GPE_MFPL_PE3MFP_Msk          (0xful << SYS_GPE_MFPL_PE3MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE3MFP Mask           */

#define SYS_GPE_MFPL_PE4MFP_Pos          (16)                                              /*!< SYS_T::GPE_MFPL: PE4MFP Position       */
#define SYS_GPE_MFPL_PE4MFP_Msk          (0xful << SYS_GPE_MFPL_PE4MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE4MFP Mask           */

#define SYS_GPE_MFPL_PE5MFP_Pos          (20)                                              /*!< SYS_T::GPE_MFPL: PE5MFP Position       */
#define SYS_GPE_MFPL_PE5MFP_Msk          (0xful << SYS_GPE_MFPL_PE5MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE5MFP Mask           */

#define SYS_GPE_MFPL_PE6MFP_Pos          (24)                                              /*!< SYS_T::GPE_MFPL: PE6MFP Position       */
#define SYS_GPE_MFPL_PE6MFP_Msk          (0xful << SYS_GPE_MFPL_PE6MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE6MFP Mask           */

#define SYS_GPE_MFPL_PE7MFP_Pos          (28)                                              /*!< SYS_T::GPE_MFPL: PE7MFP Position       */
#define SYS_GPE_MFPL_PE7MFP_Msk          (0xful << SYS_GPE_MFPL_PE7MFP_Pos)                /*!< SYS_T::GPE_MFPL: PE7MFP Mask           */

#define SYS_GPE_MFPH_PE8MFP_Pos          (0)                                               /*!< SYS_T::GPE_MFPH: PE8MFP Position       */
#define SYS_GPE_MFPH_PE8MFP_Msk          (0xful << SYS_GPE_MFPH_PE8MFP_Pos)                /*!< SYS_T::GPE_MFPH: PE8MFP Mask           */

#define SYS_GPE_MFPH_PE9MFP_Pos          (4)                                               /*!< SYS_T::GPE_MFPH: PE9MFP Position       */
#define SYS_GPE_MFPH_PE9MFP_Msk          (0xful << SYS_GPE_MFPH_PE9MFP_Pos)                /*!< SYS_T::GPE_MFPH: PE9MFP Mask           */

#define SYS_GPE_MFPH_PE10MFP_Pos         (8)                                               /*!< SYS_T::GPE_MFPH: PE10MFP Position      */
#define SYS_GPE_MFPH_PE10MFP_Msk         (0xful << SYS_GPE_MFPH_PE10MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE10MFP Mask          */

#define SYS_GPE_MFPH_PE11MFP_Pos         (12)                                              /*!< SYS_T::GPE_MFPH: PE11MFP Position      */
#define SYS_GPE_MFPH_PE11MFP_Msk         (0xful << SYS_GPE_MFPH_PE11MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE11MFP Mask          */

#define SYS_GPE_MFPH_PE12MFP_Pos         (16)                                              /*!< SYS_T::GPE_MFPH: PE12MFP Position      */
#define SYS_GPE_MFPH_PE12MFP_Msk         (0xful << SYS_GPE_MFPH_PE12MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE12MFP Mask          */

#define SYS_GPE_MFPH_PE13MFP_Pos         (20)                                              /*!< SYS_T::GPE_MFPH: PE13MFP Position      */
#define SYS_GPE_MFPH_PE13MFP_Msk         (0xful << SYS_GPE_MFPH_PE13MFP_Pos)               /*!< SYS_T::GPE_MFPH: PE13MFP Mask          */

#define SYS_GPE_MFPH_PE14MFP_Pos        (24)                                              /*!< SYS_T::GPE_MFPH: PE14_MFP Position     */
#define SYS_GPE_MFPH_PE14MFP_Msk        (0xful << SYS_GPE_MFPH_PE14MFP_Pos)              /*!< SYS_T::GPE_MFPH: PE14_MFP Mask         */

#define SYS_GPE_MFPH_PE15MFP_Pos        (28)                                              /*!< SYS_T::GPE_MFPH: PE15_MFP Position     */
#define SYS_GPE_MFPH_PE15MFP_Msk        (0xful << SYS_GPE_MFPH_PE15MFP_Pos)              /*!< SYS_T::GPE_MFPH: PE15_MFP Mask         */

#define SYS_GPF_MFPL_PF0MFP_Pos          (0)                                               /*!< SYS_T::GPF_MFPL: PF0MFP Position       */
#define SYS_GPF_MFPL_PF0MFP_Msk          (0xful << SYS_GPF_MFPL_PF0MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF0MFP Mask           */

#define SYS_GPF_MFPL_PF1MFP_Pos          (4)                                               /*!< SYS_T::GPF_MFPL: PF1MFP Position       */
#define SYS_GPF_MFPL_PF1MFP_Msk          (0xful << SYS_GPF_MFPL_PF1MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF1MFP Mask           */

#define SYS_GPF_MFPL_PF2MFP_Pos          (8)                                               /*!< SYS_T::GPF_MFPL: PF2MFP Position       */
#define SYS_GPF_MFPL_PF2MFP_Msk          (0xful << SYS_GPF_MFPL_PF2MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF2MFP Mask           */

#define SYS_GPF_MFPL_PF3MFP_Pos          (12)                                              /*!< SYS_T::GPF_MFPL: PF3MFP Position       */
#define SYS_GPF_MFPL_PF3MFP_Msk          (0xful << SYS_GPF_MFPL_PF3MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF3MFP Mask           */

#define SYS_GPF_MFPL_PF4MFP_Pos          (16)                                              /*!< SYS_T::GPF_MFPL: PF4MFP Position       */
#define SYS_GPF_MFPL_PF4MFP_Msk          (0xful << SYS_GPF_MFPL_PF4MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF4MFP Mask           */

#define SYS_GPF_MFPL_PF5MFP_Pos          (20)                                              /*!< SYS_T::GPF_MFPL: PF5MFP Position       */
#define SYS_GPF_MFPL_PF5MFP_Msk          (0xful << SYS_GPF_MFPL_PF5MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF5MFP Mask           */

#define SYS_GPF_MFPL_PF6MFP_Pos          (24)                                              /*!< SYS_T::GPF_MFPL: PF6MFP Position       */
#define SYS_GPF_MFPL_PF6MFP_Msk          (0xful << SYS_GPF_MFPL_PF6MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF6MFP Mask           */

#define SYS_GPF_MFPL_PF7MFP_Pos          (28)                                              /*!< SYS_T::GPF_MFPL: PF7MFP Position       */
#define SYS_GPF_MFPL_PF7MFP_Msk          (0xful << SYS_GPF_MFPL_PF7MFP_Pos)                /*!< SYS_T::GPF_MFPL: PF7MFP Mask           */

#define SYS_GPF_MFPH_PF8MFP_Pos          (0)                                               /*!< SYS_T::GPF_MFPH: PF8MFP Position       */
#define SYS_GPF_MFPH_PF8MFP_Msk          (0xful << SYS_GPF_MFPH_PF8MFP_Pos)                /*!< SYS_T::GPF_MFPH: PF8MFP Mask           */

#define SYS_GPF_MFPH_PF9MFP_Pos          (4)                                               /*!< SYS_T::GPF_MFPH: PF9MFP Position       */
#define SYS_GPF_MFPH_PF9MFP_Msk          (0xful << SYS_GPF_MFPH_PF9MFP_Pos)                /*!< SYS_T::GPF_MFPH: PF9MFP Mask           */

#define SYS_GPF_MFPH_PF10MFP_Pos         (8)                                               /*!< SYS_T::GPF_MFPH: PF10MFP Position      */
#define SYS_GPF_MFPH_PF10MFP_Msk         (0xful << SYS_GPF_MFPH_PF10MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF10MFP Mask          */

#define SYS_GPF_MFPH_PF11MFP_Pos         (12)                                              /*!< SYS_T::GPF_MFPH: PF11MFP Position      */
#define SYS_GPF_MFPH_PF11MFP_Msk         (0xful << SYS_GPF_MFPH_PF11MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF11MFP Mask          */

#define SYS_GPF_MFPH_PF12MFP_Pos         (16)                                              /*!< SYS_T::GPF_MFPH: PF12MFP Position      */
#define SYS_GPF_MFPH_PF12MFP_Msk         (0xful << SYS_GPF_MFPH_PF12MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF12MFP Mask          */

#define SYS_GPF_MFPH_PF13MFP_Pos         (20)                                              /*!< SYS_T::GPF_MFPH: PF13MFP Position      */
#define SYS_GPF_MFPH_PF13MFP_Msk         (0xful << SYS_GPF_MFPH_PF13MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF13MFP Mask          */

#define SYS_GPF_MFPH_PF14MFP_Pos         (24)                                              /*!< SYS_T::GPF_MFPH: PF14MFP Position      */
#define SYS_GPF_MFPH_PF14MFP_Msk         (0xful << SYS_GPF_MFPH_PF14MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF14MFP Mask          */

#define SYS_GPF_MFPH_PF15MFP_Pos         (28)                                              /*!< SYS_T::GPF_MFPH: PF15MFP Position      */
#define SYS_GPF_MFPH_PF15MFP_Msk         (0xful << SYS_GPF_MFPH_PF15MFP_Pos)               /*!< SYS_T::GPF_MFPH: PF15MFP Mask          */

#define SYS_GPA_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPA_MFOS: MFOS0 Position        */
#define SYS_GPA_MFOS_MFOS0_Msk           (0x1ul << SYS_GPA_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS0 Mask            */

#define SYS_GPA_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPA_MFOS: MFOS1 Position        */
#define SYS_GPA_MFOS_MFOS1_Msk           (0x1ul << SYS_GPA_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS1 Mask            */

#define SYS_GPA_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPA_MFOS: MFOS2 Position        */
#define SYS_GPA_MFOS_MFOS2_Msk           (0x1ul << SYS_GPA_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS2 Mask            */

#define SYS_GPA_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPA_MFOS: MFOS3 Position        */
#define SYS_GPA_MFOS_MFOS3_Msk           (0x1ul << SYS_GPA_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS3 Mask            */

#define SYS_GPA_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPA_MFOS: MFOS4 Position        */
#define SYS_GPA_MFOS_MFOS4_Msk           (0x1ul << SYS_GPA_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS4 Mask            */

#define SYS_GPA_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPA_MFOS: MFOS5 Position        */
#define SYS_GPA_MFOS_MFOS5_Msk           (0x1ul << SYS_GPA_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS5 Mask            */

#define SYS_GPA_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPA_MFOS: MFOS6 Position        */
#define SYS_GPA_MFOS_MFOS6_Msk           (0x1ul << SYS_GPA_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS6 Mask            */

#define SYS_GPA_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPA_MFOS: MFOS7 Position        */
#define SYS_GPA_MFOS_MFOS7_Msk           (0x1ul << SYS_GPA_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS7 Mask            */

#define SYS_GPA_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPA_MFOS: MFOS8 Position        */
#define SYS_GPA_MFOS_MFOS8_Msk           (0x1ul << SYS_GPA_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS8 Mask            */

#define SYS_GPA_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPA_MFOS: MFOS9 Position        */
#define SYS_GPA_MFOS_MFOS9_Msk           (0x1ul << SYS_GPA_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPA_MFOS: MFOS9 Mask            */

#define SYS_GPA_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPA_MFOS: MFOS10 Position       */
#define SYS_GPA_MFOS_MFOS10_Msk          (0x1ul << SYS_GPA_MFOS_MFOS10_Pos)                /*!< SYS_T::GPA_MFOS: MFOS10 Mask           */

#define SYS_GPA_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPA_MFOS: MFOS11 Position       */
#define SYS_GPA_MFOS_MFOS11_Msk          (0x1ul << SYS_GPA_MFOS_MFOS11_Pos)                /*!< SYS_T::GPA_MFOS: MFOS11 Mask           */

#define SYS_GPA_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPA_MFOS: MFOS12 Position       */
#define SYS_GPA_MFOS_MFOS12_Msk          (0x1ul << SYS_GPA_MFOS_MFOS12_Pos)                /*!< SYS_T::GPA_MFOS: MFOS12 Mask           */

#define SYS_GPA_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPA_MFOS: MFOS13 Position       */
#define SYS_GPA_MFOS_MFOS13_Msk          (0x1ul << SYS_GPA_MFOS_MFOS13_Pos)                /*!< SYS_T::GPA_MFOS: MFOS13 Mask           */

#define SYS_GPA_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPA_MFOS: MFOS14 Position       */
#define SYS_GPA_MFOS_MFOS14_Msk          (0x1ul << SYS_GPA_MFOS_MFOS14_Pos)                /*!< SYS_T::GPA_MFOS: MFOS14 Mask           */

#define SYS_GPA_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPA_MFOS: MFOS15 Position       */
#define SYS_GPA_MFOS_MFOS15_Msk          (0x1ul << SYS_GPA_MFOS_MFOS15_Pos)                /*!< SYS_T::GPA_MFOS: MFOS15 Mask           */

#define SYS_GPB_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPB_MFOS: MFOS0 Position        */
#define SYS_GPB_MFOS_MFOS0_Msk           (0x1ul << SYS_GPB_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS0 Mask            */

#define SYS_GPB_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPB_MFOS: MFOS1 Position        */
#define SYS_GPB_MFOS_MFOS1_Msk           (0x1ul << SYS_GPB_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS1 Mask            */

#define SYS_GPB_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPB_MFOS: MFOS2 Position        */
#define SYS_GPB_MFOS_MFOS2_Msk           (0x1ul << SYS_GPB_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS2 Mask            */

#define SYS_GPB_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPB_MFOS: MFOS3 Position        */
#define SYS_GPB_MFOS_MFOS3_Msk           (0x1ul << SYS_GPB_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS3 Mask            */

#define SYS_GPB_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPB_MFOS: MFOS4 Position        */
#define SYS_GPB_MFOS_MFOS4_Msk           (0x1ul << SYS_GPB_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS4 Mask            */

#define SYS_GPB_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPB_MFOS: MFOS5 Position        */
#define SYS_GPB_MFOS_MFOS5_Msk           (0x1ul << SYS_GPB_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS5 Mask            */

#define SYS_GPB_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPB_MFOS: MFOS6 Position        */
#define SYS_GPB_MFOS_MFOS6_Msk           (0x1ul << SYS_GPB_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS6 Mask            */

#define SYS_GPB_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPB_MFOS: MFOS7 Position        */
#define SYS_GPB_MFOS_MFOS7_Msk           (0x1ul << SYS_GPB_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS7 Mask            */

#define SYS_GPB_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPB_MFOS: MFOS8 Position        */
#define SYS_GPB_MFOS_MFOS8_Msk           (0x1ul << SYS_GPB_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS8 Mask            */

#define SYS_GPB_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPB_MFOS: MFOS9 Position        */
#define SYS_GPB_MFOS_MFOS9_Msk           (0x1ul << SYS_GPB_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPB_MFOS: MFOS9 Mask            */

#define SYS_GPB_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPB_MFOS: MFOS10 Position       */
#define SYS_GPB_MFOS_MFOS10_Msk          (0x1ul << SYS_GPB_MFOS_MFOS10_Pos)                /*!< SYS_T::GPB_MFOS: MFOS10 Mask           */

#define SYS_GPB_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPB_MFOS: MFOS11 Position       */
#define SYS_GPB_MFOS_MFOS11_Msk          (0x1ul << SYS_GPB_MFOS_MFOS11_Pos)                /*!< SYS_T::GPB_MFOS: MFOS11 Mask           */

#define SYS_GPB_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPB_MFOS: MFOS12 Position       */
#define SYS_GPB_MFOS_MFOS12_Msk          (0x1ul << SYS_GPB_MFOS_MFOS12_Pos)                /*!< SYS_T::GPB_MFOS: MFOS12 Mask           */

#define SYS_GPB_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPB_MFOS: MFOS13 Position       */
#define SYS_GPB_MFOS_MFOS13_Msk          (0x1ul << SYS_GPB_MFOS_MFOS13_Pos)                /*!< SYS_T::GPB_MFOS: MFOS13 Mask           */

#define SYS_GPB_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPB_MFOS: MFOS14 Position       */
#define SYS_GPB_MFOS_MFOS14_Msk          (0x1ul << SYS_GPB_MFOS_MFOS14_Pos)                /*!< SYS_T::GPB_MFOS: MFOS14 Mask           */

#define SYS_GPB_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPB_MFOS: MFOS15 Position       */
#define SYS_GPB_MFOS_MFOS15_Msk          (0x1ul << SYS_GPB_MFOS_MFOS15_Pos)                /*!< SYS_T::GPB_MFOS: MFOS15 Mask           */

#define SYS_GPC_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPC_MFOS: MFOS0 Position        */
#define SYS_GPC_MFOS_MFOS0_Msk           (0x1ul << SYS_GPC_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS0 Mask            */

#define SYS_GPC_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPC_MFOS: MFOS1 Position        */
#define SYS_GPC_MFOS_MFOS1_Msk           (0x1ul << SYS_GPC_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS1 Mask            */

#define SYS_GPC_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPC_MFOS: MFOS2 Position        */
#define SYS_GPC_MFOS_MFOS2_Msk           (0x1ul << SYS_GPC_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS2 Mask            */

#define SYS_GPC_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPC_MFOS: MFOS3 Position        */
#define SYS_GPC_MFOS_MFOS3_Msk           (0x1ul << SYS_GPC_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS3 Mask            */

#define SYS_GPC_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPC_MFOS: MFOS4 Position        */
#define SYS_GPC_MFOS_MFOS4_Msk           (0x1ul << SYS_GPC_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS4 Mask            */

#define SYS_GPC_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPC_MFOS: MFOS5 Position        */
#define SYS_GPC_MFOS_MFOS5_Msk           (0x1ul << SYS_GPC_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS5 Mask            */

#define SYS_GPC_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPC_MFOS: MFOS6 Position        */
#define SYS_GPC_MFOS_MFOS6_Msk           (0x1ul << SYS_GPC_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS6 Mask            */

#define SYS_GPC_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPC_MFOS: MFOS7 Position        */
#define SYS_GPC_MFOS_MFOS7_Msk           (0x1ul << SYS_GPC_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS7 Mask            */

#define SYS_GPC_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPC_MFOS: MFOS8 Position        */
#define SYS_GPC_MFOS_MFOS8_Msk           (0x1ul << SYS_GPC_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS8 Mask            */

#define SYS_GPC_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPC_MFOS: MFOS9 Position        */
#define SYS_GPC_MFOS_MFOS9_Msk           (0x1ul << SYS_GPC_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPC_MFOS: MFOS9 Mask            */

#define SYS_GPC_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPC_MFOS: MFOS10 Position       */
#define SYS_GPC_MFOS_MFOS10_Msk          (0x1ul << SYS_GPC_MFOS_MFOS10_Pos)                /*!< SYS_T::GPC_MFOS: MFOS10 Mask           */

#define SYS_GPC_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPC_MFOS: MFOS11 Position       */
#define SYS_GPC_MFOS_MFOS11_Msk          (0x1ul << SYS_GPC_MFOS_MFOS11_Pos)                /*!< SYS_T::GPC_MFOS: MFOS11 Mask           */

#define SYS_GPC_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPC_MFOS: MFOS12 Position       */
#define SYS_GPC_MFOS_MFOS12_Msk          (0x1ul << SYS_GPC_MFOS_MFOS12_Pos)                /*!< SYS_T::GPC_MFOS: MFOS12 Mask           */

#define SYS_GPC_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPC_MFOS: MFOS13 Position       */
#define SYS_GPC_MFOS_MFOS13_Msk          (0x1ul << SYS_GPC_MFOS_MFOS13_Pos)                /*!< SYS_T::GPC_MFOS: MFOS13 Mask           */

#define SYS_GPC_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPC_MFOS: MFOS14 Position       */
#define SYS_GPC_MFOS_MFOS14_Msk          (0x1ul << SYS_GPC_MFOS_MFOS14_Pos)                /*!< SYS_T::GPC_MFOS: MFOS14 Mask           */

#define SYS_GPC_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPC_MFOS: MFOS15 Position       */
#define SYS_GPC_MFOS_MFOS15_Msk          (0x1ul << SYS_GPC_MFOS_MFOS15_Pos)                /*!< SYS_T::GPC_MFOS: MFOS15 Mask           */

#define SYS_GPD_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPD_MFOS: MFOS0 Position        */
#define SYS_GPD_MFOS_MFOS0_Msk           (0x1ul << SYS_GPD_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS0 Mask            */

#define SYS_GPD_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPD_MFOS: MFOS1 Position        */
#define SYS_GPD_MFOS_MFOS1_Msk           (0x1ul << SYS_GPD_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS1 Mask            */

#define SYS_GPD_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPD_MFOS: MFOS2 Position        */
#define SYS_GPD_MFOS_MFOS2_Msk           (0x1ul << SYS_GPD_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS2 Mask            */

#define SYS_GPD_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPD_MFOS: MFOS3 Position        */
#define SYS_GPD_MFOS_MFOS3_Msk           (0x1ul << SYS_GPD_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS3 Mask            */

#define SYS_GPD_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPD_MFOS: MFOS4 Position        */
#define SYS_GPD_MFOS_MFOS4_Msk           (0x1ul << SYS_GPD_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS4 Mask            */

#define SYS_GPD_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPD_MFOS: MFOS5 Position        */
#define SYS_GPD_MFOS_MFOS5_Msk           (0x1ul << SYS_GPD_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS5 Mask            */

#define SYS_GPD_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPD_MFOS: MFOS6 Position        */
#define SYS_GPD_MFOS_MFOS6_Msk           (0x1ul << SYS_GPD_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS6 Mask            */

#define SYS_GPD_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPD_MFOS: MFOS7 Position        */
#define SYS_GPD_MFOS_MFOS7_Msk           (0x1ul << SYS_GPD_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS7 Mask            */

#define SYS_GPD_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPD_MFOS: MFOS8 Position        */
#define SYS_GPD_MFOS_MFOS8_Msk           (0x1ul << SYS_GPD_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS8 Mask            */

#define SYS_GPD_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPD_MFOS: MFOS9 Position        */
#define SYS_GPD_MFOS_MFOS9_Msk           (0x1ul << SYS_GPD_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPD_MFOS: MFOS9 Mask            */

#define SYS_GPD_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPD_MFOS: MFOS10 Position       */
#define SYS_GPD_MFOS_MFOS10_Msk          (0x1ul << SYS_GPD_MFOS_MFOS10_Pos)                /*!< SYS_T::GPD_MFOS: MFOS10 Mask           */

#define SYS_GPD_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPD_MFOS: MFOS11 Position       */
#define SYS_GPD_MFOS_MFOS11_Msk          (0x1ul << SYS_GPD_MFOS_MFOS11_Pos)                /*!< SYS_T::GPD_MFOS: MFOS11 Mask           */

#define SYS_GPD_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPD_MFOS: MFOS12 Position       */
#define SYS_GPD_MFOS_MFOS12_Msk          (0x1ul << SYS_GPD_MFOS_MFOS12_Pos)                /*!< SYS_T::GPD_MFOS: MFOS12 Mask           */

#define SYS_GPD_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPD_MFOS: MFOS13 Position       */
#define SYS_GPD_MFOS_MFOS13_Msk          (0x1ul << SYS_GPD_MFOS_MFOS13_Pos)                /*!< SYS_T::GPD_MFOS: MFOS13 Mask           */

#define SYS_GPD_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPD_MFOS: MFOS14 Position       */
#define SYS_GPD_MFOS_MFOS14_Msk          (0x1ul << SYS_GPD_MFOS_MFOS14_Pos)                /*!< SYS_T::GPD_MFOS: MFOS14 Mask           */

#define SYS_GPD_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPD_MFOS: MFOS15 Position       */
#define SYS_GPD_MFOS_MFOS15_Msk          (0x1ul << SYS_GPD_MFOS_MFOS15_Pos)                /*!< SYS_T::GPD_MFOS: MFOS15 Mask           */

#define SYS_GPE_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPE_MFOS: MFOS0 Position        */
#define SYS_GPE_MFOS_MFOS0_Msk           (0x1ul << SYS_GPE_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS0 Mask            */

#define SYS_GPE_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPE_MFOS: MFOS1 Position        */
#define SYS_GPE_MFOS_MFOS1_Msk           (0x1ul << SYS_GPE_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS1 Mask            */

#define SYS_GPE_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPE_MFOS: MFOS2 Position        */
#define SYS_GPE_MFOS_MFOS2_Msk           (0x1ul << SYS_GPE_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS2 Mask            */

#define SYS_GPE_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPE_MFOS: MFOS3 Position        */
#define SYS_GPE_MFOS_MFOS3_Msk           (0x1ul << SYS_GPE_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS3 Mask            */

#define SYS_GPE_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPE_MFOS: MFOS4 Position        */
#define SYS_GPE_MFOS_MFOS4_Msk           (0x1ul << SYS_GPE_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS4 Mask            */

#define SYS_GPE_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPE_MFOS: MFOS5 Position        */
#define SYS_GPE_MFOS_MFOS5_Msk           (0x1ul << SYS_GPE_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS5 Mask            */

#define SYS_GPE_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPE_MFOS: MFOS6 Position        */
#define SYS_GPE_MFOS_MFOS6_Msk           (0x1ul << SYS_GPE_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS6 Mask            */

#define SYS_GPE_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPE_MFOS: MFOS7 Position        */
#define SYS_GPE_MFOS_MFOS7_Msk           (0x1ul << SYS_GPE_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS7 Mask            */

#define SYS_GPE_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPE_MFOS: MFOS8 Position        */
#define SYS_GPE_MFOS_MFOS8_Msk           (0x1ul << SYS_GPE_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS8 Mask            */

#define SYS_GPE_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPE_MFOS: MFOS9 Position        */
#define SYS_GPE_MFOS_MFOS9_Msk           (0x1ul << SYS_GPE_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPE_MFOS: MFOS9 Mask            */

#define SYS_GPE_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPE_MFOS: MFOS10 Position       */
#define SYS_GPE_MFOS_MFOS10_Msk          (0x1ul << SYS_GPE_MFOS_MFOS10_Pos)                /*!< SYS_T::GPE_MFOS: MFOS10 Mask           */

#define SYS_GPE_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPE_MFOS: MFOS11 Position       */
#define SYS_GPE_MFOS_MFOS11_Msk          (0x1ul << SYS_GPE_MFOS_MFOS11_Pos)                /*!< SYS_T::GPE_MFOS: MFOS11 Mask           */

#define SYS_GPE_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPE_MFOS: MFOS12 Position       */
#define SYS_GPE_MFOS_MFOS12_Msk          (0x1ul << SYS_GPE_MFOS_MFOS12_Pos)                /*!< SYS_T::GPE_MFOS: MFOS12 Mask           */

#define SYS_GPE_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPE_MFOS: MFOS13 Position       */
#define SYS_GPE_MFOS_MFOS13_Msk          (0x1ul << SYS_GPE_MFOS_MFOS13_Pos)                /*!< SYS_T::GPE_MFOS: MFOS13 Mask           */

#define SYS_GPE_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPE_MFOS: MFOS14 Position       */
#define SYS_GPE_MFOS_MFOS14_Msk          (0x1ul << SYS_GPE_MFOS_MFOS14_Pos)                /*!< SYS_T::GPE_MFOS: MFOS14 Mask           */

#define SYS_GPE_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPE_MFOS: MFOS15 Position       */
#define SYS_GPE_MFOS_MFOS15_Msk          (0x1ul << SYS_GPE_MFOS_MFOS15_Pos)                /*!< SYS_T::GPE_MFOS: MFOS15 Mask           */

#define SYS_GPF_MFOS_MFOS0_Pos           (0)                                               /*!< SYS_T::GPF_MFOS: MFOS0 Position        */
#define SYS_GPF_MFOS_MFOS0_Msk           (0x1ul << SYS_GPF_MFOS_MFOS0_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS0 Mask            */

#define SYS_GPF_MFOS_MFOS1_Pos           (1)                                               /*!< SYS_T::GPF_MFOS: MFOS1 Position        */
#define SYS_GPF_MFOS_MFOS1_Msk           (0x1ul << SYS_GPF_MFOS_MFOS1_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS1 Mask            */

#define SYS_GPF_MFOS_MFOS2_Pos           (2)                                               /*!< SYS_T::GPF_MFOS: MFOS2 Position        */
#define SYS_GPF_MFOS_MFOS2_Msk           (0x1ul << SYS_GPF_MFOS_MFOS2_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS2 Mask            */

#define SYS_GPF_MFOS_MFOS3_Pos           (3)                                               /*!< SYS_T::GPF_MFOS: MFOS3 Position        */
#define SYS_GPF_MFOS_MFOS3_Msk           (0x1ul << SYS_GPF_MFOS_MFOS3_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS3 Mask            */

#define SYS_GPF_MFOS_MFOS4_Pos           (4)                                               /*!< SYS_T::GPF_MFOS: MFOS4 Position        */
#define SYS_GPF_MFOS_MFOS4_Msk           (0x1ul << SYS_GPF_MFOS_MFOS4_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS4 Mask            */

#define SYS_GPF_MFOS_MFOS5_Pos           (5)                                               /*!< SYS_T::GPF_MFOS: MFOS5 Position        */
#define SYS_GPF_MFOS_MFOS5_Msk           (0x1ul << SYS_GPF_MFOS_MFOS5_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS5 Mask            */

#define SYS_GPF_MFOS_MFOS6_Pos           (6)                                               /*!< SYS_T::GPF_MFOS: MFOS6 Position        */
#define SYS_GPF_MFOS_MFOS6_Msk           (0x1ul << SYS_GPF_MFOS_MFOS6_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS6 Mask            */

#define SYS_GPF_MFOS_MFOS7_Pos           (7)                                               /*!< SYS_T::GPF_MFOS: MFOS7 Position        */
#define SYS_GPF_MFOS_MFOS7_Msk           (0x1ul << SYS_GPF_MFOS_MFOS7_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS7 Mask            */

#define SYS_GPF_MFOS_MFOS8_Pos           (8)                                               /*!< SYS_T::GPF_MFOS: MFOS8 Position        */
#define SYS_GPF_MFOS_MFOS8_Msk           (0x1ul << SYS_GPF_MFOS_MFOS8_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS8 Mask            */

#define SYS_GPF_MFOS_MFOS9_Pos           (9)                                               /*!< SYS_T::GPF_MFOS: MFOS9 Position        */
#define SYS_GPF_MFOS_MFOS9_Msk           (0x1ul << SYS_GPF_MFOS_MFOS9_Pos)                 /*!< SYS_T::GPF_MFOS: MFOS9 Mask            */

#define SYS_GPF_MFOS_MFOS10_Pos          (10)                                              /*!< SYS_T::GPF_MFOS: MFOS10 Position       */
#define SYS_GPF_MFOS_MFOS10_Msk          (0x1ul << SYS_GPF_MFOS_MFOS10_Pos)                /*!< SYS_T::GPF_MFOS: MFOS10 Mask           */

#define SYS_GPF_MFOS_MFOS11_Pos          (11)                                              /*!< SYS_T::GPF_MFOS: MFOS11 Position       */
#define SYS_GPF_MFOS_MFOS11_Msk          (0x1ul << SYS_GPF_MFOS_MFOS11_Pos)                /*!< SYS_T::GPF_MFOS: MFOS11 Mask           */

#define SYS_GPF_MFOS_MFOS12_Pos          (12)                                              /*!< SYS_T::GPF_MFOS: MFOS12 Position       */
#define SYS_GPF_MFOS_MFOS12_Msk          (0x1ul << SYS_GPF_MFOS_MFOS12_Pos)                /*!< SYS_T::GPF_MFOS: MFOS12 Mask           */

#define SYS_GPF_MFOS_MFOS13_Pos          (13)                                              /*!< SYS_T::GPF_MFOS: MFOS13 Position       */
#define SYS_GPF_MFOS_MFOS13_Msk          (0x1ul << SYS_GPF_MFOS_MFOS13_Pos)                /*!< SYS_T::GPF_MFOS: MFOS13 Mask           */

#define SYS_GPF_MFOS_MFOS14_Pos          (14)                                              /*!< SYS_T::GPF_MFOS: MFOS14 Position       */
#define SYS_GPF_MFOS_MFOS14_Msk          (0x1ul << SYS_GPF_MFOS_MFOS14_Pos)                /*!< SYS_T::GPF_MFOS: MFOS14 Mask           */

#define SYS_GPF_MFOS_MFOS15_Pos          (15)                                              /*!< SYS_T::GPF_MFOS: MFOS15 Position       */
#define SYS_GPF_MFOS_MFOS15_Msk          (0x1ul << SYS_GPF_MFOS_MFOS15_Pos)                /*!< SYS_T::GPF_MFOS: MFOS15 Mask           */

#define SYS_MODCTL_MODEN_Pos             (0)                                               /*!< SYS_T::MODCTL: MODEN Position          */
#define SYS_MODCTL_MODEN_Msk             (0x1ul << SYS_MODCTL_MODEN_Pos)                   /*!< SYS_T::MODCTL: MODEN Mask              */

#define SYS_MODCTL_MODH_Pos              (1)                                               /*!< SYS_T::MODCTL: MODH Position           */
#define SYS_MODCTL_MODH_Msk              (0x1ul << SYS_MODCTL_MODH_Pos)                    /*!< SYS_T::MODCTL: MODH Mask               */

#define SYS_MODCTL_MODPWMSEL_Pos         (4)                                               /*!< SYS_T::MODCTL: MODPWMSEL Position      */
#define SYS_MODCTL_MODPWMSEL_Msk         (0xful << SYS_MODCTL_MODPWMSEL_Pos)               /*!< SYS_T::MODCTL: MODPWMSEL Mask          */

#define SYS_SRAM_BISTCTL_SRBIST_Pos      (0)                                               /*!< SYS_T::SRAM_BISTCTL: SRBIST Position   */
#define SYS_SRAM_BISTCTL_SRBIST_Msk      (0x1ul << SYS_SRAM_BISTCTL_SRBIST_Pos)            /*!< SYS_T::SRAM_BISTCTL: SRBIST Mask       */

#define SYS_SRAM_BISTCTL_FMCBIST_Pos     (2)                                               /*!< SYS_T::SRAM_BISTCTL: FMCBIST Position  */
#define SYS_SRAM_BISTCTL_FMCBIST_Msk     (0x1ul << SYS_SRAM_BISTCTL_FMCBIST_Pos)           /*!< SYS_T::SRAM_BISTCTL: FMCBIST Mask      */

#define SYS_SRAM_BISTCTL_USBBIST_Pos     (4)                                               /*!< SYS_T::SRAM_BISTCTL: USBBIST Position  */
#define SYS_SRAM_BISTCTL_USBBIST_Msk     (0x1ul << SYS_SRAM_BISTCTL_USBBIST_Pos)           /*!< SYS_T::SRAM_BISTCTL: USBBIST Mask      */

#define SYS_SRAM_BISTCTL_PDMABIST_Pos    (7)                                               /*!< SYS_T::SRAM_BISTCTL: PDMABIST Position */
#define SYS_SRAM_BISTCTL_PDMABIST_Msk    (0x1ul << SYS_SRAM_BISTCTL_PDMABIST_Pos)          /*!< SYS_T::SRAM_BISTCTL: PDMABIST Mask     */

#define SYS_SRAM_BISTSTS_SRBISTEF_Pos    (0)                                               /*!< SYS_T::SRAM_BISTSTS: SRBISTEF Position */
#define SYS_SRAM_BISTSTS_SRBISTEF_Msk    (0x1ul << SYS_SRAM_BISTSTS_SRBISTEF_Pos)          /*!< SYS_T::SRAM_BISTSTS: SRBISTEF Mask     */

#define SYS_SRAM_BISTSTS_CR0BISTEF_Pos   (1)                                               /*!< SYS_T::SRAM_BISTSTS: CR0BISTEF Position*/
#define SYS_SRAM_BISTSTS_CR0BISTEF_Msk   (0x1ul << SYS_SRAM_BISTSTS_CR0BISTEF_Pos)         /*!< SYS_T::SRAM_BISTSTS: CR0BISTEF Mask    */

#define SYS_SRAM_BISTSTS_CR1BISTEF_Pos   (2)                                               /*!< SYS_T::SRAM_BISTSTS: CR1BISTEF Position*/
#define SYS_SRAM_BISTSTS_CR1BISTEF_Msk   (0x1ul << SYS_SRAM_BISTSTS_CR1BISTEF_Pos)         /*!< SYS_T::SRAM_BISTSTS: CR1BISTEF Mask    */

#define SYS_SRAM_BISTSTS_USBBEF_Pos      (4)                                               /*!< SYS_T::SRAM_BISTSTS: USBBEF Position   */
#define SYS_SRAM_BISTSTS_USBBEF_Msk      (0x1ul << SYS_SRAM_BISTSTS_USBBEF_Pos)            /*!< SYS_T::SRAM_BISTSTS: USBBEF Mask       */

#define SYS_SRAM_BISTSTS_PDMABISTF_Pos   (7)                                               /*!< SYS_T::SRAM_BISTSTS: PDMABISTF Position*/
#define SYS_SRAM_BISTSTS_PDMABISTF_Msk   (0x1ul << SYS_SRAM_BISTSTS_PDMABISTF_Pos)         /*!< SYS_T::SRAM_BISTSTS: PDMABISTF Mask    */

#define SYS_SRAM_BISTSTS_SRBEND_Pos      (16)                                              /*!< SYS_T::SRAM_BISTSTS: SRBEND Position   */
#define SYS_SRAM_BISTSTS_SRBEND_Msk      (0x1ul << SYS_SRAM_BISTSTS_SRBEND_Pos)            /*!< SYS_T::SRAM_BISTSTS: SRBEND Mask       */

#define SYS_SRAM_BISTSTS_CR0BEND_Pos     (17)                                              /*!< SYS_T::SRAM_BISTSTS: CR0BEND Position  */
#define SYS_SRAM_BISTSTS_CR0BEND_Msk     (0x1ul << SYS_SRAM_BISTSTS_CR0BEND_Pos)           /*!< SYS_T::SRAM_BISTSTS: CR0BEND Mask      */

#define SYS_SRAM_BISTSTS_CR1BEND_Pos     (18)                                              /*!< SYS_T::SRAM_BISTSTS: CR1BEND Position  */
#define SYS_SRAM_BISTSTS_CR1BEND_Msk     (0x1ul << SYS_SRAM_BISTSTS_CR1BEND_Pos)           /*!< SYS_T::SRAM_BISTSTS: CR1BEND Mask      */

#define SYS_SRAM_BISTSTS_USBBEND_Pos     (20)                                              /*!< SYS_T::SRAM_BISTSTS: USBBEND Position  */
#define SYS_SRAM_BISTSTS_USBBEND_Msk     (0x1ul << SYS_SRAM_BISTSTS_USBBEND_Pos)           /*!< SYS_T::SRAM_BISTSTS: USBBEND Mask      */

#define SYS_SRAM_BISTSTS_PDMAEND_Pos     (23)                                              /*!< SYS_T::SRAM_BISTSTS: PDMAEND Position  */
#define SYS_SRAM_BISTSTS_PDMAEND_Msk     (0x1ul << SYS_SRAM_BISTSTS_PDMAEND_Pos)           /*!< SYS_T::SRAM_BISTSTS: PDMAEND Mask      */

#define SYS_HIRCTRIMCTL_FREQSEL_Pos      (0)                                               /*!< SYS_T::HIRCTRIMCTL: FREQSEL Position   */
#define SYS_HIRCTRIMCTL_FREQSEL_Msk      (0x3ul << SYS_HIRCTRIMCTL_FREQSEL_Pos)            /*!< SYS_T::HIRCTRIMCTL: FREQSEL Mask       */

#define SYS_HIRCTRIMCTL_LOOPSEL_Pos      (4)                                               /*!< SYS_T::HIRCTRIMCTL: LOOPSEL Position   */
#define SYS_HIRCTRIMCTL_LOOPSEL_Msk      (0x3ul << SYS_HIRCTRIMCTL_LOOPSEL_Pos)            /*!< SYS_T::HIRCTRIMCTL: LOOPSEL Mask       */

#define SYS_HIRCTRIMCTL_RETRYCNT_Pos     (6)                                               /*!< SYS_T::HIRCTRIMCTL: RETRYCNT Position  */
#define SYS_HIRCTRIMCTL_RETRYCNT_Msk     (0x3ul << SYS_HIRCTRIMCTL_RETRYCNT_Pos)           /*!< SYS_T::HIRCTRIMCTL: RETRYCNT Mask      */

#define SYS_HIRCTRIMCTL_CESTOPEN_Pos     (8)                                               /*!< SYS_T::HIRCTRIMCTL: CESTOPEN Position  */
#define SYS_HIRCTRIMCTL_CESTOPEN_Msk     (0x1ul << SYS_HIRCTRIMCTL_CESTOPEN_Pos)           /*!< SYS_T::HIRCTRIMCTL: CESTOPEN Mask      */

#define SYS_HIRCTRIMCTL_BOUNDEN_Pos      (9)                                               /*!< SYS_T::HIRCTRIMCTL: BOUNDEN Position   */
#define SYS_HIRCTRIMCTL_BOUNDEN_Msk      (0x1ul << SYS_HIRCTRIMCTL_BOUNDEN_Pos)            /*!< SYS_T::HIRCTRIMCTL: BOUNDEN Mask       */

#define SYS_HIRCTRIMCTL_REFCKSEL_Pos     (10)                                              /*!< SYS_T::HIRCTRIMCTL: REFCKSEL Position  */
#define SYS_HIRCTRIMCTL_REFCKSEL_Msk     (0x1ul << SYS_HIRCTRIMCTL_REFCKSEL_Pos)           /*!< SYS_T::HIRCTRIMCTL: REFCKSEL Mask      */

#define SYS_HIRCTRIMCTL_BOUNDARY_Pos     (16)                                              /*!< SYS_T::HIRCTRIMCTL: BOUNDARY Position  */
#define SYS_HIRCTRIMCTL_BOUNDARY_Msk     (0x1ful << SYS_HIRCTRIMCTL_BOUNDARY_Pos)          /*!< SYS_T::HIRCTRIMCTL: BOUNDARY Mask      */

#define SYS_HIRCTRIMIEN_TFALIEN_Pos      (1)                                               /*!< SYS_T::HIRCTRIMIEN: TFALIEN Position   */
#define SYS_HIRCTRIMIEN_TFALIEN_Msk      (0x1ul << SYS_HIRCTRIMIEN_TFALIEN_Pos)            /*!< SYS_T::HIRCTRIMIEN: TFALIEN Mask       */

#define SYS_HIRCTRIMIEN_CLKEIEN_Pos      (2)                                               /*!< SYS_T::HIRCTRIMIEN: CLKEIEN Position   */
#define SYS_HIRCTRIMIEN_CLKEIEN_Msk      (0x1ul << SYS_HIRCTRIMIEN_CLKEIEN_Pos)            /*!< SYS_T::HIRCTRIMIEN: CLKEIEN Mask       */

#define SYS_HIRCTRIMSTS_FREQLOCK_Pos     (0)                                               /*!< SYS_T::HIRCTRIMSTS: FREQLOCK Position  */
#define SYS_HIRCTRIMSTS_FREQLOCK_Msk     (0x1ul << SYS_HIRCTRIMSTS_FREQLOCK_Pos)           /*!< SYS_T::HIRCTRIMSTS: FREQLOCK Mask      */

#define SYS_HIRCTRIMSTS_TFAILIF_Pos      (1)                                               /*!< SYS_T::HIRCTRIMSTS: TFAILIF Position   */
#define SYS_HIRCTRIMSTS_TFAILIF_Msk      (0x1ul << SYS_HIRCTRIMSTS_TFAILIF_Pos)            /*!< SYS_T::HIRCTRIMSTS: TFAILIF Mask       */

#define SYS_HIRCTRIMSTS_CLKERIF_Pos      (2)                                               /*!< SYS_T::HIRCTRIMSTS: CLKERIF Position   */
#define SYS_HIRCTRIMSTS_CLKERIF_Msk      (0x1ul << SYS_HIRCTRIMSTS_CLKERIF_Pos)            /*!< SYS_T::HIRCTRIMSTS: CLKERIF Mask       */

#define SYS_HIRCTRIMSTS_OVBDIF_Pos       (3)                                               /*!< SYS_T::HIRCTRIMSTS: OVBDIF Position    */
#define SYS_HIRCTRIMSTS_OVBDIF_Msk       (0x1ul << SYS_HIRCTRIMSTS_OVBDIF_Pos)             /*!< SYS_T::HIRCTRIMSTS: OVBDIF Mask        */

#define SYS_REGLCTL_REGLCTL_Pos          (0)                                               /*!< SYS_T::REGLCTL: REGLCTL Position       */
#define SYS_REGLCTL_REGLCTL_Msk          (0xfful << SYS_REGLCTL_REGLCTL_Pos)               /*!< SYS_T::REGLCTL: REGLCTL Mask           */

#define SYS_MIRCTRIMCTL_FREQSEL_Pos      (0)                                               /*!< SYS_T::MIRCTRIMCTL: FREQSEL Position   */
#define SYS_MIRCTRIMCTL_FREQSEL_Msk      (0x3ul << SYS_MIRCTRIMCTL_FREQSEL_Pos)            /*!< SYS_T::MIRCTRIMCTL: FREQSEL Mask       */

#define SYS_MIRCTRIMCTL_LOOPSEL_Pos      (4)                                               /*!< SYS_T::MIRCTRIMCTL: LOOPSEL Position   */
#define SYS_MIRCTRIMCTL_LOOPSEL_Msk      (0x3ul << SYS_MIRCTRIMCTL_LOOPSEL_Pos)            /*!< SYS_T::MIRCTRIMCTL: LOOPSEL Mask       */

#define SYS_MIRCTRIMCTL_RETRYCNT_Pos     (6)                                               /*!< SYS_T::MIRCTRIMCTL: RETRYCNT Position  */
#define SYS_MIRCTRIMCTL_RETRYCNT_Msk     (0x3ul << SYS_MIRCTRIMCTL_RETRYCNT_Pos)           /*!< SYS_T::MIRCTRIMCTL: RETRYCNT Mask      */

#define SYS_MIRCTRIMCTL_CESTOPEN_Pos     (8)                                               /*!< SYS_T::MIRCTRIMCTL: CESTOPEN Position  */
#define SYS_MIRCTRIMCTL_CESTOPEN_Msk     (0x1ul << SYS_MIRCTRIMCTL_CESTOPEN_Pos)           /*!< SYS_T::MIRCTRIMCTL: CESTOPEN Mask      */

#define SYS_MIRCTRIMCTL_BOUNDEN_Pos      (9)                                               /*!< SYS_T::MIRCTRIMCTL: BOUNDEN Position   */
#define SYS_MIRCTRIMCTL_BOUNDEN_Msk      (0x1ul << SYS_MIRCTRIMCTL_BOUNDEN_Pos)            /*!< SYS_T::MIRCTRIMCTL: BOUNDEN Mask       */

#define SYS_MIRCTRIMCTL_REFCKSEL_Pos     (10)                                              /*!< SYS_T::MIRCTRIMCTL: REFCKSEL Position  */
#define SYS_MIRCTRIMCTL_REFCKSEL_Msk     (0x1ul << SYS_MIRCTRIMCTL_REFCKSEL_Pos)           /*!< SYS_T::MIRCTRIMCTL: REFCKSEL Mask      */

#define SYS_MIRCTRIMCTL_BOUNDARY_Pos     (16)                                              /*!< SYS_T::MIRCTRIMCTL: BOUNDARY Position  */
#define SYS_MIRCTRIMCTL_BOUNDARY_Msk     (0x1ful << SYS_MIRCTRIMCTL_BOUNDARY_Pos)          /*!< SYS_T::MIRCTRIMCTL: BOUNDARY Mask      */

#define SYS_MIRCTRIMIEN_TFALIEN_Pos      (1)                                               /*!< SYS_T::MIRCTRIMIEN: TFALIEN Position   */
#define SYS_MIRCTRIMIEN_TFALIEN_Msk      (0x1ul << SYS_MIRCTRIMIEN_TFALIEN_Pos)            /*!< SYS_T::MIRCTRIMIEN: TFALIEN Mask       */

#define SYS_MIRCTRIMIEN_CLKEIEN_Pos      (2)                                               /*!< SYS_T::MIRCTRIMIEN: CLKEIEN Position   */
#define SYS_MIRCTRIMIEN_CLKEIEN_Msk      (0x1ul << SYS_MIRCTRIMIEN_CLKEIEN_Pos)            /*!< SYS_T::MIRCTRIMIEN: CLKEIEN Mask       */

#define SYS_MIRCTRIMSTS_FREQLOCK_Pos     (0)                                               /*!< SYS_T::MIRCTRIMSTS: FREQLOCK Position  */
#define SYS_MIRCTRIMSTS_FREQLOCK_Msk     (0x1ul << SYS_MIRCTRIMSTS_FREQLOCK_Pos)           /*!< SYS_T::MIRCTRIMSTS: FREQLOCK Mask      */

#define SYS_MIRCTRIMSTS_TFAILIF_Pos      (1)                                               /*!< SYS_T::MIRCTRIMSTS: TFAILIF Position   */
#define SYS_MIRCTRIMSTS_TFAILIF_Msk      (0x1ul << SYS_MIRCTRIMSTS_TFAILIF_Pos)            /*!< SYS_T::MIRCTRIMSTS: TFAILIF Mask       */

#define SYS_MIRCTRIMSTS_CLKERIF_Pos      (2)                                               /*!< SYS_T::MIRCTRIMSTS: CLKERIF Position   */
#define SYS_MIRCTRIMSTS_CLKERIF_Msk      (0x1ul << SYS_MIRCTRIMSTS_CLKERIF_Pos)            /*!< SYS_T::MIRCTRIMSTS: CLKERIF Mask       */

#define SYS_MIRCTRIMSTS_OVBDIF_Pos       (3)                                               /*!< SYS_T::MIRCTRIMSTS: OVBDIF Position    */
#define SYS_MIRCTRIMSTS_OVBDIF_Msk       (0x1ul << SYS_MIRCTRIMSTS_OVBDIF_Pos)             /*!< SYS_T::MIRCTRIMSTS: OVBDIF Mask        */

#define SYS_RTCLVRIEN_IE_Pos             (0)                                               /*!< SYS_T::RTCLVRIEN: IE Position     */
#define SYS_RTCLVRIEN_IE_Msk             (0x1ul << SYS_RTCLVRIEN_IE_Pos)                   /*!< SYS_T::RTCLVRIEN: IE Mask         */

#define SYS_RTCLVRSTS_IF_Pos             (0)                                               /*!< SYS_T::RTCLVRSTS: IF Position     */
#define SYS_RTCLVRSTS_IF_Msk             (0x1ul << SYS_RTCLVRSTS_IF_Pos)                   /*!< SYS_T::RTCLVRSTS: IF Mask         */

#define SYS_RTCLVRSTS_RTCLVR_Pos         (8)                                               /*!< SYS_T::RTCLVRSTS: RTCLVR Position     */
#define SYS_RTCLVRSTS_RTCLVR_Msk         (0x1ul << SYS_RTCLVRSTS_RTCLVR_Pos)               /*!< SYS_T::RTCLVRSTS: RTCLVR Mask         */

#define SYS_PORCTL1_POROFF_Pos           (0)                                               /*!< SYS_T::PORCTL1: POROFF Position     */
#define SYS_PORCTL1_POROFF_Msk           (0xfffful << SYS_PORCTL1_POROFF_Pos)              /*!< SYS_T::PORCTL1: POROFF Mask         */

#define SYS_PLCTL_PLSEL_Pos              (0)                                               /*!< SYS_T::PLCTL: PLSEL Position           */
#define SYS_PLCTL_PLSEL_Msk              (0x3ul << SYS_PLCTL_PLSEL_Pos)                    /*!< SYS_T::PLCTL: PLSEL Mask               */

#define SYS_PLSTS_PLCBUSY_Pos            (0)                                               /*!< SYS_T::PLSTS: PLCBUSY Position         */
#define SYS_PLSTS_PLCBUSY_Msk            (0x1ul << SYS_PLSTS_PLCBUSY_Pos)                  /*!< SYS_T::PLSTS: PLCBUSY Mask             */

#define SYS_PLSTS_CURPL_Pos              (8)                                               /*!< SYS_T::PLSTS: CURPL Position           */
#define SYS_PLSTS_CURPL_Msk              (0x3ul << SYS_PLSTS_CURPL_Pos)                    /*!< SYS_T::PLSTS: CURPL Mask               */

/**@}*/ /* SYS_CONST */
/**@}*/ /* end of SYS register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
    #pragma no_anon_unions
#endif

#endif /* __SYS_REG_H__ */

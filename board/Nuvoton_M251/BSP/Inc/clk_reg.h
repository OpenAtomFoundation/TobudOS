/**************************************************************************//**
 * @file     clk_reg.h
 * @version  V1.00
 * @brief    CLK register definition header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __CLK_REG_H__
#define __CLK_REG_H__

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup CLK System Clock Controller (CLK)
    Memory Mapped Structure for CLK Controller
@{ */

typedef struct
{


    /**
     * @var CLK_T::PWRCTL
     * Offset: 0x00  System Power-down Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HXTEN     |HXT Enable Bit (Write Protect)
     * |        |          |The bit default value is set by flash controller user configuration register CONFIG0 [26]
     * |        |          |When the default clock source is from HXT, this bit is set to 1 automatically.
     * |        |          |0 = 4~32 MHz external high speed crystal (HXT) Disabled.
     * |        |          |1 = 4~32 MHz external high speed crystal (HXT) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[1]     |LXTEN     |LXT Enable Bit (Write Protect)
     * |        |          |0 = 32.768 kHz external low speed crystal (LXT) Disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal (LXT) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[2]     |HIRCEN    |HIRC Enable Bit (Write Protect)
     * |        |          |0 = 48MHz internal high speed RC oscillator (HIRC) Disabled.
     * |        |          |1 = 48 MHz internal high speed RC oscillator (HIRC) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[3]     |LIRCEN    |LIRC Enable Bit (Write Protect)
     * |        |          |0 = 38.4 kHz internal low speed RC oscillator (LIRC) Disabled.
     * |        |          |1 = 38.4 kHz internal low speed RC oscillator (LIRC) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note : LIRC will also be forced on when 1
     * |        |          |Power down and ~(cfg0[3] & cfg0[4] & ~cfg0[31] & cfg0[30]) 2
     * |        |          |Not power down and ~(cfg0[3] & cfg0[4] & cfg0[31])
     * |[4]     |PDWKDLY   |Enable the Wake-up Delay Counter (Write Protect)
     * |        |          |When the chip wakes up from Power-down mode, the clock control will delay certain clock cycles to wait system clock stable.
     * |        |          |The delayed clock cycle is 4096 clock cycles when chip works at 4~32 MHz external high speed crystal oscillator (HXT), HIRC and MIRC see HIRCSTBS and MIRCSTBS
     * |        |          |0 = Clock cycles delay Disabled.
     * |        |          |1 = Clock cycles delay Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[5]     |PDWKIEN   |Power-down Mode Wake-up Interrupt Enable Bit (Write Protect)
     * |        |          |0 = Power-down mode wake-up interrupt Disabled.
     * |        |          |1 = Power-down mode wake-up interrupt Enabled.
     * |        |          |Note1: The interrupt will occur when both PDWKIF and PDWKIEN are high.
     * |        |          |Note2: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[6]     |PDWKIF    |Power-down Mode Wake-up Interrupt Status
     * |        |          |Set by "Power-down wake-up event", it indicates that resume from Power-down mode.
     * |        |          |The flag is set if any wake-up source is occurred. Refer Power Modes and Wake-up Sources chapter.
     * |        |          |Note1: Write 1 to clear the bit to 0.
     * |        |          |Note2: This bit works only if PDWKIEN (CLK_PWRCTL[5]) set to 1.
     * |[7]     |PDEN      |System Power-down Enable (Write Protect)
     * |        |          |When this bit is set to 1, Power-down mode is enabled and chip Power-down behavior will depend on the PDWTCPU bit.
     * |        |          |(a) If the PDWTCPU is 0, then the chip enters Power-down mode immediately after the PDEN bit set
     * |        |          |(default)
     * |        |          |(b) if the PDWTCPU is 1, then the chip keeps active till the CPU sleep mode is also active and then the chip enters Power-down mode.
     * |        |          |When chip wakes up from Power-down mode, this bit is auto cleared
     * |        |          |Users need to set this bit again for next Power-down.
     * |        |          |In Power-down mode, HXT and the HIRC will be disabled in this mode, but LXT and LIRC are not controlled by Power-down mode.
     * |        |          |In Power-down mode, the PLL and system clock are disabled, and ignored the clock source selection
     * |        |          |The clocks of peripheral are not controlled by Power-down mode, if the peripheral clock source is from LXT or LIRC.
     * |        |          |0 = Chip operating normally or chip in idle mode because of WFI command.
     * |        |          |1 = Chip enters Power-down mode instant or wait CPU sleep command WFI.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[19]    |MIRCEN    |MIRC Enable Bit (Write Protect)
     * |        |          |0 = 4MHz internal high speed RC oscillator (MIRC) Disabled.
     * |        |          |1 = 4 MHz internal high speed RC oscillator (MIRC) Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: Will be forced on when CLKSEL0[2:0] = 0x101b.
     * |[22:20] |HXTGAIN   |HXT Gain Control Bit (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |Gain control is used to enlarge the gain of crystal to make sure crystal work normally
     * |        |          |If gain control is enabled, crystal will consume more power than gain control off.
     * |        |          |000 = HXT frequency 1~4 MHz.
     * |        |          |001 = HXT frequency  4~8 MHz.
     * |        |          |010 = HXT frequency  8~12 MHz.
     * |        |          |011 = HXT frequency 12~16 MHz.
     * |        |          |100 = HXT frequency 12~24 MHz.
     * |        |          |101 = HXT frequency 16~32 Mhz.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note : reset by powr on reset
     * @var CLK_T::AHBCLK
     * Offset: 0x04  AHB Devices Clock Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |PDMACKEN  |PDMA Controller Clock Enable Bit
     * |        |          |0 = PDMA peripheral clock Disabled.
     * |        |          |1 = PDMA peripheral clock Enabled.
     * |[2]     |ISPCKEN   |Flash ISP Controller Clock Enable Bit
     * |        |          |0 = Flash ISP peripheral clock Disabled.
     * |        |          |1 = Flash ISP peripheral clock Enabled.
     * |[3]     |EBICKEN   |EBI Controller Clock Enable Bit
     * |        |          |0 = EBI peripheral clock Disabled.
     * |        |          |1 = EBI peripheral clock Enabled.
     * |[7]     |CRCCKEN   |CRC Generator Controller Clock Enable Bit
     * |        |          |0 = CRC peripheral clock Disabled.
     * |        |          |1 = CRC peripheral clock Enabled.
     * |[12]    |CRYPTCKEN |Cryptographic Accelerator Clock Enable Bit
     * |        |          |0 = Cryptographic Accelerator clock Disabled.
     * |        |          |1 = Cryptographic Accelerator clock Enabled.
     * |[15]    |FMCIDLE   |Flash Memory Controller Clock Enable Bit in IDLE Mode
     * |        |          |0 = FMC clock Disabled when chip is under IDLE mode.
     * |        |          |1 = FMC clock Enabled when chip is under IDLE mode.
     * @var CLK_T::APBCLK0
     * Offset: 0x08  APB Devices Clock Enable Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |WDTCKEN   |Watchdog Timer Clock Enable Bit (Write Protect)
     * |        |          |0 = Watchdog timer clock Disabled.
     * |        |          |1 = Watchdog timer clock Enabled.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note : this bit is forced to 1 when cfg0[3] & cfg0[3] & cfg0[31] are all 1s
     * |[1]     |RTCCKEN   |Real-time-clock APB Interface Clock Enable Bit
     * |        |          |This bit is used to control the RTC APB clock only
     * |        |          |The RTC peripheral clock source is selected from RTCSEL(CLK_CLKSEL3[8])
     * |        |          |It can be selected to 32.768 kHz external low speed crystal or 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |0 = RTC clock Disabled.
     * |        |          |1 = RTC clock Enabled.
     * |[2]     |TMR0CKEN  |Timer0 Clock Enable Bit
     * |        |          |0 = Timer0 clock Disabled.
     * |        |          |1 = Timer0 clock Enabled.
     * |[3]     |TMR1CKEN  |Timer1 Clock Enable Bit
     * |        |          |0 = Timer1 clock Disabled.
     * |        |          |1 = Timer1 clock Enabled.
     * |[4]     |TMR2CKEN  |Timer2 Clock Enable Bit
     * |        |          |0 = Timer2 clock Disabled.
     * |        |          |1 = Timer2 clock Enabled.
     * |[5]     |TMR3CKEN  |Timer3 Clock Enable Bit
     * |        |          |0 = Timer3 clock Disabled.
     * |        |          |1 = Timer3 clock Enabled.
     * |[6]     |CLKOCKEN  |CLKO Clock Enable Bit
     * |        |          |0 = CLKO clock Disabled.
     * |        |          |1 = CLKO clock Enabled.
     * |[7]     |ACMP01CKEN|Analog Comparator 0/1 Clock Enable Bit
     * |        |          |0 = Analog comparator 0/1 clock Disabled.
     * |        |          |1 = Analog comparator 0/1 clock Enabled.
     * |[8]     |I2C0CKEN  |I2C0 Clock Enable Bit
     * |        |          |0 = I2C0 clock Disabled.
     * |        |          |1 = I2C0 clock Enabled.
     * |[9]     |I2C1CKEN  |I2C1 Clock Enable Bit
     * |        |          |0 = I2C1 clock Disabled.
     * |        |          |1 = I2C1 clock Enabled.
     * |[12]    |QSPI0CKEN |QSPI0 Clock Enable Bit
     * |        |          |0 = QSPI0 clock Disabled.
     * |        |          |1 = QSPI0 clock Enabled.
     * |[13]    |SPI0CKEN  |SPI0 Clock Enable Bit
     * |        |          |0 = SPI0 clock Disabled.
     * |        |          |1 = SPI0 clock Enabled.
     * |[16]    |UART0CKEN |UART0 Clock Enable Bit
     * |        |          |0 = UART0 clock Disabled.
     * |        |          |1 = UART0 clock Enabled.
     * |[17]    |UART1CKEN |UART1 Clock Enable Bit
     * |        |          |0 = UART1 clock Disabled.
     * |        |          |1 = UART1 clock Enabled.
     * |[18]    |UART2CKEN |UART2 Clock Enable Bit
     * |        |          |0 = UART2 clock Disabled.
     * |        |          |1 = UART2 clock Enabled.
     * |[27]    |USBDCKEN  |USB Device Clock Enable Bit
     * |        |          |0 = USB Device clock Disabled.
     * |        |          |1 = USB Device clock Enabled.
     * |[28]    |EADCCKEN  |Enhanced Analog-digital-converter (EADC) Clock Enable Bit
     * |        |          |0 = EADC clock Disabled.
     * |        |          |1 = EADC clock Enabled.
     * @var CLK_T::APBCLK1
     * Offset: 0x0C  APB Devices Clock Enable Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SC0CKEN   |SC0 Clock Enable Bit
     * |        |          |0 = SC0 clock Disabled.
     * |        |          |1 = SC0 clock Enabled.
     * |[8]     |USCI0CKEN |USCI0 Clock Enable Bit
     * |        |          |0 = USCI0 clock Disabled.
     * |        |          |1 = USCI0 clock Enabled.
     * |[9]     |USCI1CKEN |USCI1 Clock Enable Bit
     * |        |          |0 = USCI1 clock Disabled.
     * |        |          |1 = USCI1 clock Enabled.
     * |[10]    |USCI2CKEN |USCI2 Clock Enable Bit
     * |        |          |0 = USCI1 clock Disabled.
     * |        |          |1 = USCI1 clock Enabled.
     * |[12]    |DACCKEN   |DAC Clock Enable Bit
     * |        |          |0 = DAC clock Disabled.
     * |        |          |1 = DAC clock Enabled.
     * |[16]    |PWM0CKEN  |PWM0 Clock Enable Bit
     * |        |          |0 = PWM0 clock Disabled.
     * |        |          |1 = PWM0 clock Enabled.
     * |[17]    |PWM1CKEN  |PWM1 Clock Enable Bit
     * |        |          |0 = PWM1 clock Disabled.
     * |        |          |1 = PWM1 clock Enabled.
     * |[18]    |BPWM0CKEN |BPWM0 Clock Enable Bit
     * |        |          |0 = BPWM0 clock Disabled.
     * |        |          |1 = BPWM0 clock Enabled.
     * |[19]    |BPWM1CKEN |BPWM1 Clock Enable Bit
     * |        |          |0 = BPWM1 clock Disabled.
     * |        |          |1 = BPWM1 clock Enabled.
     * |[30]    |OPACKEN   |OP Amplifier (OPA) Clock Enable Bit
     * |        |          |0 = OPA clock Disabled.
     * |        |          |1 = OPA clock Enabled.
     * |[31]    |PSIOCKEN  |PSIO Clock Enable Bit
     * |        |          |0 = PSIO clock Disabled.
     * |        |          |1 = PSIO clock Enabled.
     * @var CLK_T::CLKSEL0
     * Offset: 0x10  Clock Source Select Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |HCLKSEL   |HCLK Clock Source Selection (Write Protect)
     * |        |          |Before clock switching, the related clock sources (both pre-select and new-select) must be turned on.
     * |        |          |The default value is reloaded from the value of CFOSC (CONFIG0[26]) in user configuration register of Flash controller by any reset
     * |        |          |Therefore the default value is either 000b or 101b.
     * |        |          |000 = Clock source from HXT.
     * |        |          |001 = Clock source from LXT.
     * |        |          |010 = Clock source from PLL.
     * |        |          |011 = Clock source from LIRC.
     * |        |          |101= Clock source from MIRC.
     * |        |          |111= Clock source from HIRC.
     * |        |          |Other = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[5:3]   |STCLKSEL  |Cortex-M23 SysTick Clock Source Selection (Write Protect)
     * |        |          |If SYST_CTRL[2]=0, SysTick uses listed clock source below.
     * |        |          |000 = Clock source from HXT.
     * |        |          |001 = Clock source from LXT.
     * |        |          |010 = Clock source from HXT/2.
     * |        |          |011 = Clock source from HCLK/2.
     * |        |          |111 = Clock source from HIRC/2.
     * |        |          |Note: if SysTick clock source is not from HCLK (i.e
     * |        |          |SYST_CTRL[2] = 0), SysTick clock source must less than or equal to HCLK/2.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[8]     |USBDSEL   |USB Device Clock Source Selection (Write Protect)
     * |        |          |These bits are protected bit
     * |        |          |It means programming this bit needs to write "59h", "16h", "88h" to address 0x4000_0100 to disable register protection
     * |        |          |Refer to the register REGWRPROT at address GCR_BA+0x100.
     * |        |          |0 = Clock source from HIRC.
     * |        |          |1 = Clock source from PLL. divided
     * @var CLK_T::CLKSEL1
     * Offset: 0x14  Clock Source Select Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |WDTSEL    |Watchdog Timer Clock Source Selection (Write Protect)
     * |        |          |00 = Reserved.
     * |        |          |01 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |10 = Clock source from HCLK/2048.
     * |        |          |11 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |Note: This bit is write protected.Refer to the SYS_REGLCTL register
     * |        |          |Note: This bit is forced to 11 when CONFIG0[31], CONFIG0[4], CONFIG0[3] are all ones.
     * |[3:2]   |WWDTSEL   |Window Watchdog Timer Clock Source Selection (Write Protect)
     * |        |          |10 = Clock source from HCLK/2048.
     * |        |          |11 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |Others = Reserved.
     * |[6:4]   |CLKOSEL   |Clock Divider Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from HCLK.
     * |        |          |011 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |100 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |101 = Clock source from 4 MHz internal medium speed RC oscillator (MIRC).
     * |        |          |110 = Clock source from PLL.
     * |        |          |111 = Clock source from internal USB synchronous mode.
     * |[10:8]  |TMR0SEL   |TIMER0 Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK0.
     * |        |          |011 = Clock source from external clock T0 pin.
     * |        |          |101 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[14:12] |TMR1SEL   |TIMER1 Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK0.
     * |        |          |011 = Clock source from external clock T1 pin.
     * |        |          |101 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[18:16] |TMR2SEL   |TIMER2 Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK1.
     * |        |          |011 = Clock source from external clock T2 pin.
     * |        |          |101 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[22:20] |TMR3SEL   |TIMER3 Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK1.
     * |        |          |011 = Clock source from external clock T3 pin.
     * |        |          |101 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * |[26:24] |UART0SEL  |UART0 Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from PLL.
     * |        |          |010 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |011 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |100= PCLK0-> default.
     * |        |          |101= LIRC
     * |[30:28] |UART1SEL  |UART1 Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from PLL.
     * |        |          |010 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |011 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |100= PCLK1 -> default.
     * |        |          |101= LIRC
     * @var CLK_T::CLKSEL2
     * Offset: 0x18  Clock Source Select Control Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PWM0SEL   |PWM0 Clock Source Selection
     * |        |          |The peripheral clock source of PWM0 is defined by PWM0SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK0.
     * |[1]     |PWM1SEL   |PWM1 Clock Source Selection
     * |        |          |The peripheral clock source of PWM1 is defined by PWM1SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK1.
     * |[3:2]   |QSPI0SEL  |QSPI0 Clock Source Selection
     * |        |          |00 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK0.
     * |        |          |11 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |[5:4]   |SPI0SEL   |SPI0 Clock Source Selection
     * |        |          |00 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK1.
     * |        |          |11 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |[8]     |BPWM0SEL  |BPWM0 Clock Source Selection
     * |        |          |The peripheral clock source of BPWM0 is defined by BPWM0SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK0.
     * |[9]     |BPWM1SEL  |BPWM1 Clock Source Selection
     * |        |          |The peripheral clock source of BPWM1 is defined by BPWM1SEL.
     * |        |          |0 = Clock source from PLL.
     * |        |          |1 = Clock source from PCLK1.
     * |[30:28] |PSIOSEL   |PSIO Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |010 = Clock source from PCLK1.
     * |        |          |011 = Clock source from PLL.
     * |        |          |100 = Clock source from 38.4 kHz internal low speed RC oscillator (LIRC).
     * |        |          |111 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |Others = Reserved.
     * @var CLK_T::CLKSEL3
     * Offset: 0x1C  Clock Source Select Control Register 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |SC0SEL    |SC0 Clock Source Selection
     * |        |          |00 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |01 = Clock source from PLL.
     * |        |          |10 = Clock source from PCLK0.
     * |        |          |11 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |[26:24] |UART2SEL  |UART2 Clock Source Selection
     * |        |          |000 = Clock source from 4~32 MHz external high speed crystal oscillator (HXT).
     * |        |          |001 = Clock source from PLL.
     * |        |          |010 = Reserved.
     * |        |          |010 = Clock source from 32.768 kHz external low speed crystal oscillator (LXT).
     * |        |          |011 = Clock source from 48 MHz internal high speed RC oscillator (HIRC).
     * |        |          |100= PCLK 0-> default.
     * |        |          |101= LIRC
     * @var CLK_T::CLKDIV0
     * Offset: 0x20  Clock Divider Number Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |HCLKDIV   |HCLK Clock Divide Number From HCLK Clock Source
     * |        |          |HCLK clock frequency = (HCLK clock source frequency) / (HCLKDIV + 1).
     * |[7:4]   |USBDIV    |USB Clock Divide Number From PLL Clock
     * |        |          |USB clock frequency = (PLL frequency) / (USBDIV + 1).
     * |[11:8]  |UART0DIV  |UART0 Clock Divide Number From UART0 Clock Source
     * |        |          |UART0 clock frequency = (UART0 clock source frequency) / (UART0DIV + 1).
     * |[15:12] |UART1DIV  |UART1 Clock Divide Number From UART1 Clock Source
     * |        |          |UART1 clock frequency = (UART1 clock source frequency) / (UART1DIV + 1).
     * |[23:16] |EADCDIV   |EADC Clock Divide Number From EADC Clock Source
     * |        |          |EADC clock frequency = (EADC clock source frequency) / (EADCDIV + 1).
     * @var CLK_T::CLKDIV1
     * Offset: 0x24  Clock Divider Number Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |SC0DIV    |SC0 Clock Divide Number From SC0 Clock Source
     * |        |          |SC0 clock frequency = (SC0 clock source frequency ) / (SC0DIV + 1).
     * |[31:24] |PSIODIV   |PSIO Clock Divide Number From PSIO Clock Source
     * |        |          |PSIO clock frequency = (PSIO clock source frequency ) / (PSIODIV + 1).
     * @var CLK_T::CLKDIV4
     * Offset: 0x30  Clock Divider Number Register 4
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |UART2DIV  |UART2 Clock Divide Number From UART2 Clock Source
     * |        |          |UART2 clock frequency = (UART2 clock source frequency) / (UART2DIV + 1).
     * @var CLK_T::PCLKDIV
     * Offset: 0x34  APB Clock Divider Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |APB0DIV   |APB0 Clock DIvider
     * |        |          |APB0 clock can be divided from HCLK
     * |        |          |000: PCLK0 = HCLK.
     * |        |          |001: PCLK0 = 1/2 HCLK.
     * |        |          |010: PCLK0 = 1/4 HCLK.
     * |        |          |011: PCLK0 = 1/8 HCLK.
     * |        |          |100: PCLK0 = 1/16 HCLK.
     * |        |          |101: PCLK0 = 1/32 HCLK.
     * |        |          |Others: Reserved.
     * |[6:4]   |APB1DIV   |APB1 Clock DIvider
     * |        |          |APB1 clock can be divided from HCLK
     * |        |          |000: PCLK1 = HCLK.
     * |        |          |001: PCLK1 = 1/2 HCLK.
     * |        |          |010: PCLK1 = 1/4 HCLK.
     * |        |          |011: PCLK1 = 1/8 HCLK.
     * |        |          |100: PCLK1 = 1/16 HCLK.
     * |        |          |101: PCLK1 = 1/32 HCLK.
     * |        |          |Others: Reserved.
     * @var CLK_T::PLLCTL
     * Offset: 0x40  PLL Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[5:0]   |FBDIV     |PLL Feedback Divider Control (Write Protect)
     * |        |          |Refer to the formulas below the table.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[12:9]  |INDIV     |PLL Input Divider Control (Write Protect)
     * |        |          |Refer to the formulas below the table.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[15:14] |OUTDIV    |PLL Output Divider Control (Write Protect)
     * |        |          |Refer to the formulas below the table.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[16]    |PD        |Power-down Mode (Write Protect)
     * |        |          |If set the PDEN bit to 1 in CLK_PWRCTL register, the PLL will enter Power-down mode, too.
     * |        |          |0 = PLL is in normal mode.
     * |        |          |1 = PLL is in Power-down mode (default).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[17]    |BP        |PLL Bypass Control (Write Protect)
     * |        |          |0 = PLL is in normal mode (default).
     * |        |          |1 = PLL clock output is same as PLL input clock FIN.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[18]    |OE        |PLL OE (FOUT Enable) Pin Control (Write Protect)
     * |        |          |0 = PLL FOUT Enabled.
     * |        |          |1 = PLL FOUT is fixed low.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[20:19] |PLLSRC    |PLL Source Clock Selection (Write Protect)
     * |        |          |00 = PLL source clock from 4~32 MHz external high-speed crystal oscillator (HXT).
     * |        |          |01 = PLL source clock from 12 MHz internal high-speed oscillator (HIRC/4).
     * |        |          |10 = PLL source clock from 4~32 MHz external high-speed crystal oscillator (HXT).
     * |        |          |11 = PLL source clock from 4 MHz internal high-speed oscillator (MIRC).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |        |          |Note: MIRC and HIRC have to be both on when source switch between them
     * |[23]    |STBSEL    |PLL Stable Counter Selection (Write Protect)
     * |        |          |0 = PLL stable time is 1200 PLL source clock (suitable for source clock is equal to or less than 12 MHz).
     * |        |          |1 = PLL stable time is 2400 PLL source clock (suitable for source clock is larger than 12 MHz).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[24]    |PLL_CLF_EN|1: PLL Clock Filter On
     * |        |          |0 : OFF
     * @var CLK_T::STATUS
     * Offset: 0x50  Clock Status Monitor Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HXTSTB    |HXT Clock Source Stable Flag (Read Only)
     * |        |          |0 = 4~32 MHz external high speed crystal oscillator (HXT) clock is not stable or disabled.
     * |        |          |1 = 4~32 MHz external high speed crystal oscillator (HXT) clock is stable and enabled.
     * |[1]     |LXTSTB    |LXT Clock Source Stable Flag (Read Only)
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock is not stable or disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) clock is stabled and enabled.
     * |[2]     |PLLSTB    |Internal PLL Clock Source Stable Flag (Read Only)
     * |        |          |0 = Internal PLL clock is not stable or disabled.
     * |        |          |1 = Internal PLL clock is stable and enabled.
     * |[3]     |LIRCSTB   |LIRC Clock Source Stable Flag (Read Only)
     * |        |          |0 = 38.4 kHz internal low speed RC oscillator (LIRC) clock is not stable or disabled.
     * |        |          |1 = 38.4 kHz internal low speed RC oscillator (LIRC) clock is stable and enabled.
     * |[4]     |HIRCSTB   |HIRC Clock Source Stable Flag (Read Only)
     * |        |          |0 = 48 MHz internal high speed RC oscillator (HIRC) clock is not stable or disabled.
     * |        |          |1 = 48 MHz internal high speed RC oscillator (HIRC) clock is stable and enabled.
     * |[6]     |MIRCSTB   |MIRC Clock Source Stable Flag (Read Only)
     * |        |          |0 = 4 MHz internal mid speed RC oscillator (MIRC) clock is not stable or disabled.
     * |        |          |1 = 4 MHz internal mid speed RC oscillator (MIRC) clock is stable and enabled.
     * |[7]     |CLKSFAIL  |Clock Switching Fail Flag (Read Only)
     * |        |          |This bit is updated when software switches system clock source
     * |        |          |If switch target clock is stable, this bit will be set to 0
     * |        |          |If switch target clock is not stable, this bit will be set to 1.
     * |        |          |0 = Clock switching success.
     * |        |          |1 = Clock switching failure.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * @var CLK_T::CLKOCTL
     * Offset: 0x60  Clock Output Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |FREQSEL   |Clock Output Frequency Selection
     * |        |          |The formula of output frequency is
     * |        |          |Fout = Fin/2(N+1).
     * |        |          |Fin is the input clock frequency.
     * |        |          |Fout is the frequency of divider output clock.
     * |        |          |N is the 4-bit value of FREQSEL[3:0].
     * |[4]     |CLKOEN    |Clock Output Enable Bit
     * |        |          |0 = Clock Output function Disabled.
     * |        |          |1 = Clock Output function Enabled.
     * |[5]     |DIV1EN    |Clock Output Divide One Enable Bit
     * |        |          |0 = Clock Output will output clock with source frequency divided by FREQSEL.
     * |        |          |1 = Clock Output will output clock with source frequency.
     * |[6]     |CLK1HZEN  |Clock Output 1Hz Enable Bit
     * |        |          |0 = 1 Hz clock output for 32.768 kHz frequency compensation Disabled.
     * |        |          |1 = 1 Hz clock output for 32.768 kHz frequency compensation Enabled.
     * @var CLK_T::CLKDCTL
     * Offset: 0x70  Clock Fail Detector Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4]     |HXTFDEN   |HXT Clock Fail Detector Enable Bit
     * |        |          |0 = 4~32 MHz external high speed crystal oscillator (HXT) clock fail detector Disabled.
     * |        |          |1 = 4~32 MHz external high speed crystal oscillator (HXT) clock fail detector Enabled.
     * |[5]     |HXTFIEN   |HXT Clock Fail Interrupt Enable Bit
     * |        |          |0 = 4~32 MHz external high speed crystal oscillator (HXT) clock fail interrupt Disabled.
     * |        |          |1 = 4~32 MHz external high speed crystal oscillator (HXT) clock fail interrupt Enabled.
     * |[12]    |LXTFDEN   |LXT Clock Fail Detector Enable Bit
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail detector Disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail detector Enabled.
     * |[13]    |LXTFIEN   |LXT Clock Fail Interrupt Enable Bit
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail interrupt Disabled.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) clock fail interrupt Enabled.
     * |[16]    |HXTFQDEN  |HXT Clock Frequency Range Detector Enable Bit
     * |        |          |0 = 4~32 MHz external high speed crystal oscillator (HXT) clock frequency range detector Disabled.
     * |        |          |1 = 4~32 MHz external high speed crystal oscillator (HXT) clock frequency range detector Enabled.
     * |[17]    |HXTFQIEN  |HXT Clock Frequency Range Detector Interrupt Enable Bit
     * |        |          |0 = 4~32 MHz external high speed crystal oscillator (HXT) clock frequency range detector fail interrupt Disabled.
     * |        |          |1 = 4~32 MHz external high speed crystal oscillator (HXT) clock frequency range detector fail interrupt Enabled.
     * @var CLK_T::CLKDSTS
     * Offset: 0x74  Clock Fail Detector Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HXTFIF    |HXT Clock Fail Interrupt Flag (Write Protect)
     * |        |          |0 = 4~32 MHz external high speed crystal oscillator (HXT) clock is normal.
     * |        |          |1 = 4~32 MHz external high speed crystal oscillator (HXT) clock stops.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * |[1]     |LXTFIF    |LXT Clock Fail Interrupt Flag (Write Protect)
     * |        |          |0 = 32.768 kHz external low speed crystal oscillator (LXT) clock is normal.
     * |        |          |1 = 32.768 kHz external low speed crystal oscillator (LXT) stops.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * |[8]     |HXTFQIF   |HXT Clock Frequency Range Detector Interrupt Flag (Write Protect)
     * |        |          |0 = 4~32 MHz external high speed crystal oscillator (HXT) clock frequency is normal.
     * |        |          |1 = 4~32 MHz external high speed crystal oscillator (HXT) clock frequency is abnormal.
     * |        |          |Note: Write 1 to clear the bit to 0.
     * @var CLK_T::CDUPB
     * Offset: 0x78  Clock Frequency Range Detector Upper Boundary Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |UPERBD    |HXT Clock Frequency Range Detector Upper Boundary Value
     * |        |          |The bits define the maximum value of frequency range detector window.
     * |        |          |When HXT frequency higher than this maximum frequency value, the HXT Clock Frequency Range Detector Interrupt Flag will set to 1.
     * @var CLK_T::CDLOWB
     * Offset: 0x7C  Clock Frequency Range Detector Lower Boundary Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9:0]   |LOWERBD   |HXT Clock Frequency Range Detector Lower Boundary Value
     * |        |          |The bits define the minimum value of frequency range detector window.
     * |        |          |When HXT frequency lower than this minimum frequency value, the HXT Clock Frequency Range Detector Interrupt Flag will set to 1.
     * @var CLK_T::PMUCTL
     * Offset: 0x90  Power Manager Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |PDMSEL    |Power-down Mode Selection (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |These bits control chip power-down mode grade selection when CPU execute WFI/WFE instruction.
     * |        |          |000 = Power-down mode is selected. (PD)
     * |        |          |010 = fast wake up.(FWPD)
     * |        |          |110 = Deep Power-down mode is selected (DPD).
     * |        |          |others = Reserved.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[8]     |WKTMREN   |Wake-up Timer Enable (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |0 = Wake-up timer disable at DPD mode.
     * |        |          |1 = Wake-up timer enabled at DPD mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[11:9]  |WKTMRIS   |Wake-up Timer Time-out Interval Select (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |These bits control wake-up timer time-out interval when chip at DPD mode.
     * |        |          |000 = Time-out interval is 128 OSC32K clocks (~3.368 ms).
     * |        |          |001 = Time-out interval is 256 OSC32K clocks (~6.736 ms).
     * |        |          |010 = Time-out interval is 512 OSC32K clocks (~13.47 ms).
     * |        |          |011 = Time-out interval is 1024 OSC32K clocks (~26.95 ms).
     * |        |          |100 = Time-out interval is 4096 OSC32K clocks (~107.79 ms).
     * |        |          |101 = Time-out interval is 8192 OSC32K clocks (~215.58 ms).
     * |        |          |110 = Time-out interval is 16384 OSC32K clocks (~431.16 ms).
     * |        |          |111 = Time-out interval is 32768 OSC32K clocks (~862.32 ms).
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[17:16] |WKPINEN0  |Wake-up Pin Enable 0(Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |This is control register for GPC.0 to wake-up pin.
     * |        |          |00 = Wake-up pin disable at Deep Power-down mode.
     * |        |          |01 = Wake-up pin rising edge enabled at Deep Power-down mode.
     * |        |          |10 = Wake-up pin falling edge enabled at Deep Power-down mode.
     * |        |          |11 = Wake-up pin both edge enabled at Deep Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[22]    |WKPINDBEN |Wake-up pin De-bounce Enable Bit (Write Protect)
     * |        |          |The WKPINDBEN bit is used to enable the de-bounce function for wake-up pin.
     * |        |          |If the input signal pulse width cannot be sampled by continuous eight de-bounce sample cycle,
     * |        |          |the input signal transition is seen as the signal bounce and will not trigger the wakeup.
     * |        |          |The de-bounce clock source is the 38 kHz internal low speed RC oscillator (LIRC).
     * |        |          |0 = Deep power-down wake-up pin De-bounce function disable.
     * |        |          |1 = Deep power-down wake-up pin De-bounce function enable.
     * |        |          |The de-bounce function is valid only for edge triggered
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[23]    |RTCWKEN   |RTC Wake-up Enable (Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |0 = RTC wake-up disable at Deep Power-down mode or Standby Power-down mode.
     * |        |          |1 = RTC wake-up enabled at Deep Power-down mode or Standby Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[25:24] |WKPINEN1  |Wake-up Pin Enable 1(Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |This is control register for GPB.0 to wake-up pin.
     * |        |          |00 = Wake-up pin disable at Deep Power-down mode.
     * |        |          |01 = Wake-up pin rising edge enabled at Deep Power-down mode.
     * |        |          |10 = Wake-up pin falling edge enabled at Deep Power-down mode.
     * |        |          |11 = Wake-up pin both edge enabled at Deep Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register
     * |[27:26] |WKPINEN2  |Wake-up Pin Enable 2(Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |This is control register for GPB.2 to wake-up pin.
     * |        |          |00 = Wake-up pin disable at Deep Power-down mode.
     * |        |          |01 = Wake-up pin rising edge enabled at Deep Power-down mode.
     * |        |          |10 = Wake-up pin falling edge enabled at Deep Power-down mode.
     * |        |          |11 = Wake-up pin both edge enabled at Deep Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[29:28] |WKPINEN3  |Wake-up Pin Enable 3(Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |This is control register for GPB.12 to wake-up pin.
     * |        |          |00 = Wake-up pin disable at Deep Power-down mode.
     * |        |          |01 = Wake-up pin rising edge enabled at Deep Power-down mode.
     * |        |          |10 = Wake-up pin falling edge enabled at Deep Power-down mode.
     * |        |          |11 = Wake-up pin both edge enabled at Deep Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * |[31:30] |WKPINEN4  |Wake-up Pin Enable 4(Write Protect)
     * |        |          |This is a protected register. Please refer to open lock sequence to program it.
     * |        |          |This is control register for GPF.6 to wake-up pin.
     * |        |          |00 = Wake-up pin disable at Deep Power-down mode.
     * |        |          |01 = Wake-up pin rising edge enabled at Deep Power-down mode.
     * |        |          |10 = Wake-up pin falling edge enabled at Deep Power-down mode.
     * |        |          |11 = Wake-up pin both edge enabled at Deep Power-down mode.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var CLK_T::PMUSTS
     * Offset: 0x94  Power Manager Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PINWK0    |Pin Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode was requested by a transition of the WAKEUP pin (GPC.0)
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[1]     |TMRWK     |Timer Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode (DPD) was requested by wakeup timer time-out
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[2]     |RTCWK     |RTC Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wakeup of device from Deep Power-down mode (DPD) was requested with a RTC alarm, tick time or tamper happened
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[3]     |PINWK1    |Pin Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode was requested by a transition of the WAKEUP pin (GPB.0)
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[4]     |PINWK2    |Pin Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode was requested by a transition of the WAKEUP pin (GPB.2)
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[5]     |PINWK3    |Pin Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode was requested by a transition of the WAKEUP pin (GPB.12)
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[6]     |PINWK4    |Pin Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wake-up of chip from Deep Power-down mode was requested by a transition of the WAKEUP pin (GPF.6)
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[12]    |LVRWK     |LVR Wake-up Flag (Read Only)
     * |        |          |This flag indicates that wakeup of device from Deep Power-down mode was requested with a LVR happened
     * |        |          |This flag is cleared when DPD mode is entered.
     * |[31]    |CLRWK     |Clear Wake-up Flag
     * |        |          |0 = No clear.
     * |        |          |1= Clear all wake-up flag.
     * @var CLK_T::HXTFSEL
     * Offset: 0xB4  HXT Filter Select Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |HXTFSEL   |HXT Filter Select (Write Protect)
     * |        |          |0 = HXT frequency is > 12MHz.
     * |        |          |1 = HXT frequency is <= 12MHz.
     */
    __IO uint32_t PWRCTL;                /*!< [0x0000] System Power-down Control Register                               */
    __IO uint32_t AHBCLK;                /*!< [0x0004] AHB Devices Clock Enable Control Register                        */
    __IO uint32_t APBCLK0;               /*!< [0x0008] APB Devices Clock Enable Control Register 0                      */
    __IO uint32_t APBCLK1;               /*!< [0x000c] APB Devices Clock Enable Control Register 1                      */
    __IO uint32_t CLKSEL0;               /*!< [0x0010] Clock Source Select Control Register 0                           */
    __IO uint32_t CLKSEL1;               /*!< [0x0014] Clock Source Select Control Register 1                           */
    __IO uint32_t CLKSEL2;               /*!< [0x0018] Clock Source Select Control Register 2                           */
    __IO uint32_t CLKSEL3;               /*!< [0x001c] Clock Source Select Control Register 3                           */
    __IO uint32_t CLKDIV0;               /*!< [0x0020] Clock Divider Number Register 0                                  */
    __IO uint32_t CLKDIV1;               /*!< [0x0024] Clock Divider Number Register 1                                  */
    __I  uint32_t RESERVE0[2];
    __IO uint32_t CLKDIV4;               /*!< [0x0030] Clock Divider Number Register 4                                  */
    __IO uint32_t PCLKDIV;               /*!< [0x0034] APB Clock Divider Register                                       */
    __I  uint32_t RESERVE1[2];
    __IO uint32_t PLLCTL;                /*!< [0x0040] PLL Control Register                                             */
    __I  uint32_t RESERVE2[3];
    __I  uint32_t STATUS;                /*!< [0x0050] Clock Status Monitor Register                                    */
    __I  uint32_t RESERVE3[3];
    __IO uint32_t CLKOCTL;               /*!< [0x0060] Clock Output Control Register                                    */
    __I  uint32_t RESERVE4[3];
    __IO uint32_t CLKDCTL;               /*!< [0x0070] Clock Fail Detector Control Register                             */
    __IO uint32_t CLKDSTS;               /*!< [0x0074] Clock Fail Detector Status Register                              */
    __IO uint32_t CDUPB;                 /*!< [0x0078] Clock Frequency Range Detector Upper Boundary Register           */
    __IO uint32_t CDLOWB;                /*!< [0x007c] Clock Frequency Range Detector Lower Boundary Register           */
    __I  uint32_t RESERVE5[4];
    __IO uint32_t PMUCTL;                /*!< [0x0090] Power Manager Control Register                                   */
    __IO uint32_t PMUSTS;                /*!< [0x0094] Power Manager Status Register                                    */
    __I  uint32_t RESERVE6[7];
    __IO uint32_t HXTFSEL;               /*!< [0x00b4] HXT Filter Select Control Register                               */

} CLK_T;

/**
    @addtogroup CLK_CONST CLK Bit Field Definition
    Constant Definitions for CLK Controller
@{ */

#define CLK_PWRCTL_HXTEN_Pos             (0)                                               /*!< CLK_T::PWRCTL: HXTEN Position          */
#define CLK_PWRCTL_HXTEN_Msk             (0x1ul << CLK_PWRCTL_HXTEN_Pos)                   /*!< CLK_T::PWRCTL: HXTEN Mask              */

#define CLK_PWRCTL_LXTEN_Pos             (1)                                               /*!< CLK_T::PWRCTL: LXTEN Position          */
#define CLK_PWRCTL_LXTEN_Msk             (0x1ul << CLK_PWRCTL_LXTEN_Pos)                   /*!< CLK_T::PWRCTL: LXTEN Mask              */

#define CLK_PWRCTL_HIRCEN_Pos            (2)                                               /*!< CLK_T::PWRCTL: HIRCEN Position         */
#define CLK_PWRCTL_HIRCEN_Msk            (0x1ul << CLK_PWRCTL_HIRCEN_Pos)                  /*!< CLK_T::PWRCTL: HIRCEN Mask             */

#define CLK_PWRCTL_LIRCEN_Pos            (3)                                               /*!< CLK_T::PWRCTL: LIRCEN Position         */
#define CLK_PWRCTL_LIRCEN_Msk            (0x1ul << CLK_PWRCTL_LIRCEN_Pos)                  /*!< CLK_T::PWRCTL: LIRCEN Mask             */

#define CLK_PWRCTL_PDWKDLY_Pos           (4)                                               /*!< CLK_T::PWRCTL: PDWKDLY Position        */
#define CLK_PWRCTL_PDWKDLY_Msk           (0x1ul << CLK_PWRCTL_PDWKDLY_Pos)                 /*!< CLK_T::PWRCTL: PDWKDLY Mask            */

#define CLK_PWRCTL_PDWKIEN_Pos           (5)                                               /*!< CLK_T::PWRCTL: PDWKIEN Position        */
#define CLK_PWRCTL_PDWKIEN_Msk           (0x1ul << CLK_PWRCTL_PDWKIEN_Pos)                 /*!< CLK_T::PWRCTL: PDWKIEN Mask            */

#define CLK_PWRCTL_PDWKIF_Pos            (6)                                               /*!< CLK_T::PWRCTL: PDWKIF Position         */
#define CLK_PWRCTL_PDWKIF_Msk            (0x1ul << CLK_PWRCTL_PDWKIF_Pos)                  /*!< CLK_T::PWRCTL: PDWKIF Mask             */

#define CLK_PWRCTL_PDEN_Pos              (7)                                               /*!< CLK_T::PWRCTL: PDEN Position           */
#define CLK_PWRCTL_PDEN_Msk              (0x1ul << CLK_PWRCTL_PDEN_Pos)                    /*!< CLK_T::PWRCTL: PDEN Mask               */

#define CLK_PWRCTL_MIRCEN_Pos            (19)                                              /*!< CLK_T::PWRCTL: MIRCEN Position         */
#define CLK_PWRCTL_MIRCEN_Msk            (0x1ul << CLK_PWRCTL_MIRCEN_Pos)                  /*!< CLK_T::PWRCTL: MIRCEN Mask             */

#define CLK_PWRCTL_HXTGAIN_Pos           (20)                                              /*!< CLK_T::PWRCTL: HXTGAIN Position        */
#define CLK_PWRCTL_HXTGAIN_Msk           (0x7ul << CLK_PWRCTL_HXTGAIN_Pos)                /*!< CLK_T::PWRCTL: HXTGAIN Mask            */

#define CLK_AHBCLK_PDMACKEN_Pos          (1)                                               /*!< CLK_T::AHBCLK: PDMACKEN Position       */
#define CLK_AHBCLK_PDMACKEN_Msk          (0x1ul << CLK_AHBCLK_PDMACKEN_Pos)                /*!< CLK_T::AHBCLK: PDMACKEN Mask           */

#define CLK_AHBCLK_ISPCKEN_Pos           (2)                                               /*!< CLK_T::AHBCLK: ISPCKEN Position        */
#define CLK_AHBCLK_ISPCKEN_Msk           (0x1ul << CLK_AHBCLK_ISPCKEN_Pos)                 /*!< CLK_T::AHBCLK: ISPCKEN Mask            */

#define CLK_AHBCLK_EBICKEN_Pos           (3)                                               /*!< CLK_T::AHBCLK: EBICKEN Position        */
#define CLK_AHBCLK_EBICKEN_Msk           (0x1ul << CLK_AHBCLK_EBICKEN_Pos)                 /*!< CLK_T::AHBCLK: EBICKEN Mask            */

#define CLK_AHBCLK_CRCCKEN_Pos           (7)                                               /*!< CLK_T::AHBCLK: CRCCKEN Position        */
#define CLK_AHBCLK_CRCCKEN_Msk           (0x1ul << CLK_AHBCLK_CRCCKEN_Pos)                 /*!< CLK_T::AHBCLK: CRCCKEN Mask            */

#define CLK_AHBCLK_CRYPTCKEN_Pos         (12)                                              /*!< CLK_T::AHBCLK: CRYPTCKEN Position      */
#define CLK_AHBCLK_CRYPTCKEN_Msk         (0x1ul << CLK_AHBCLK_CRYPTCKEN_Pos)               /*!< CLK_T::AHBCLK: CRYPTCKEN Mask          */

#define CLK_AHBCLK_FMCIDLE_Pos           (15)                                              /*!< CLK_T::AHBCLK: FMCIDLE Position        */
#define CLK_AHBCLK_FMCIDLE_Msk           (0x1ul << CLK_AHBCLK_FMCIDLE_Pos)                 /*!< CLK_T::AHBCLK: FMCIDLE Mask            */

#define CLK_APBCLK0_WDTCKEN_Pos          (0)                                               /*!< CLK_T::APBCLK0: WDTCKEN Position       */
#define CLK_APBCLK0_WDTCKEN_Msk          (0x1ul << CLK_APBCLK0_WDTCKEN_Pos)                /*!< CLK_T::APBCLK0: WDTCKEN Mask           */

#define CLK_APBCLK0_RTCCKEN_Pos          (1)                                               /*!< CLK_T::APBCLK0: RTCCKEN Position       */
#define CLK_APBCLK0_RTCCKEN_Msk          (0x1ul << CLK_APBCLK0_RTCCKEN_Pos)                /*!< CLK_T::APBCLK0: RTCCKEN Mask           */

#define CLK_APBCLK0_TMR0CKEN_Pos         (2)                                               /*!< CLK_T::APBCLK0: TMR0CKEN Position      */
#define CLK_APBCLK0_TMR0CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR0CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR0CKEN Mask          */

#define CLK_APBCLK0_TMR1CKEN_Pos         (3)                                               /*!< CLK_T::APBCLK0: TMR1CKEN Position      */
#define CLK_APBCLK0_TMR1CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR1CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR1CKEN Mask          */

#define CLK_APBCLK0_TMR2CKEN_Pos         (4)                                               /*!< CLK_T::APBCLK0: TMR2CKEN Position      */
#define CLK_APBCLK0_TMR2CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR2CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR2CKEN Mask          */

#define CLK_APBCLK0_TMR3CKEN_Pos         (5)                                               /*!< CLK_T::APBCLK0: TMR3CKEN Position      */
#define CLK_APBCLK0_TMR3CKEN_Msk         (0x1ul << CLK_APBCLK0_TMR3CKEN_Pos)               /*!< CLK_T::APBCLK0: TMR3CKEN Mask          */

#define CLK_APBCLK0_CLKOCKEN_Pos         (6)                                               /*!< CLK_T::APBCLK0: CLKOCKEN Position      */
#define CLK_APBCLK0_CLKOCKEN_Msk         (0x1ul << CLK_APBCLK0_CLKOCKEN_Pos)               /*!< CLK_T::APBCLK0: CLKOCKEN Mask          */

#define CLK_APBCLK0_ACMP01CKEN_Pos       (7)                                               /*!< CLK_T::APBCLK0: ACMP01CKEN Position    */
#define CLK_APBCLK0_ACMP01CKEN_Msk       (0x1ul << CLK_APBCLK0_ACMP01CKEN_Pos)             /*!< CLK_T::APBCLK0: ACMP01CKEN Mask        */

#define CLK_APBCLK0_I2C0CKEN_Pos         (8)                                               /*!< CLK_T::APBCLK0: I2C0CKEN Position      */
#define CLK_APBCLK0_I2C0CKEN_Msk         (0x1ul << CLK_APBCLK0_I2C0CKEN_Pos)               /*!< CLK_T::APBCLK0: I2C0CKEN Mask          */

#define CLK_APBCLK0_I2C1CKEN_Pos         (9)                                               /*!< CLK_T::APBCLK0: I2C1CKEN Position      */
#define CLK_APBCLK0_I2C1CKEN_Msk         (0x1ul << CLK_APBCLK0_I2C1CKEN_Pos)               /*!< CLK_T::APBCLK0: I2C1CKEN Mask          */

#define CLK_APBCLK0_QSPI0CKEN_Pos        (12)                                              /*!< CLK_T::APBCLK0: QSPI0CKEN Position     */
#define CLK_APBCLK0_QSPI0CKEN_Msk        (0x1ul << CLK_APBCLK0_QSPI0CKEN_Pos)              /*!< CLK_T::APBCLK0: QSPI0CKEN Mask         */

#define CLK_APBCLK0_SPI0CKEN_Pos         (13)                                              /*!< CLK_T::APBCLK0: SPI0CKEN Position      */
#define CLK_APBCLK0_SPI0CKEN_Msk         (0x1ul << CLK_APBCLK0_SPI0CKEN_Pos)               /*!< CLK_T::APBCLK0: SPI0CKEN Mask          */

#define CLK_APBCLK0_UART0CKEN_Pos        (16)                                              /*!< CLK_T::APBCLK0: UART0CKEN Position     */
#define CLK_APBCLK0_UART0CKEN_Msk        (0x1ul << CLK_APBCLK0_UART0CKEN_Pos)              /*!< CLK_T::APBCLK0: UART0CKEN Mask         */

#define CLK_APBCLK0_UART1CKEN_Pos        (17)                                              /*!< CLK_T::APBCLK0: UART1CKEN Position     */
#define CLK_APBCLK0_UART1CKEN_Msk        (0x1ul << CLK_APBCLK0_UART1CKEN_Pos)              /*!< CLK_T::APBCLK0: UART1CKEN Mask         */

#define CLK_APBCLK0_UART2CKEN_Pos        (18)                                              /*!< CLK_T::APBCLK0: UART2CKEN Position     */
#define CLK_APBCLK0_UART2CKEN_Msk        (0x1ul << CLK_APBCLK0_UART2CKEN_Pos)              /*!< CLK_T::APBCLK0: UART2CKEN Mask         */

#define CLK_APBCLK0_USBDCKEN_Pos         (27)                                              /*!< CLK_T::APBCLK0: USBDCKEN Position      */
#define CLK_APBCLK0_USBDCKEN_Msk         (0x1ul << CLK_APBCLK0_USBDCKEN_Pos)               /*!< CLK_T::APBCLK0: USBDCKEN Mask          */

#define CLK_APBCLK0_EADCCKEN_Pos         (28)                                              /*!< CLK_T::APBCLK0: EADCCKEN Position      */
#define CLK_APBCLK0_EADCCKEN_Msk         (0x1ul << CLK_APBCLK0_EADCCKEN_Pos)               /*!< CLK_T::APBCLK0: EADCCKEN Mask          */

#define CLK_APBCLK1_SC0CKEN_Pos          (0)                                               /*!< CLK_T::APBCLK1: SC0CKEN Position       */
#define CLK_APBCLK1_SC0CKEN_Msk          (0x1ul << CLK_APBCLK1_SC0CKEN_Pos)                /*!< CLK_T::APBCLK1: SC0CKEN Mask           */

#define CLK_APBCLK1_USCI0CKEN_Pos        (8)                                               /*!< CLK_T::APBCLK1: USCI0CKEN Position     */
#define CLK_APBCLK1_USCI0CKEN_Msk        (0x1ul << CLK_APBCLK1_USCI0CKEN_Pos)              /*!< CLK_T::APBCLK1: USCI0CKEN Mask         */

#define CLK_APBCLK1_USCI1CKEN_Pos        (9)                                               /*!< CLK_T::APBCLK1: USCI1CKEN Position     */
#define CLK_APBCLK1_USCI1CKEN_Msk        (0x1ul << CLK_APBCLK1_USCI1CKEN_Pos)              /*!< CLK_T::APBCLK1: USCI1CKEN Mask         */

#define CLK_APBCLK1_USCI2CKEN_Pos        (10)                                              /*!< CLK_T::APBCLK1: USCI2CKEN Position     */
#define CLK_APBCLK1_USCI2CKEN_Msk        (0x1ul << CLK_APBCLK1_USCI2CKEN_Pos)              /*!< CLK_T::APBCLK1: USCI2CKEN Mask         */

#define CLK_APBCLK1_DACCKEN_Pos          (12)                                              /*!< CLK_T::APBCLK1: DACCKEN Position       */
#define CLK_APBCLK1_DACCKEN_Msk          (0x1ul << CLK_APBCLK1_DACCKEN_Pos)                /*!< CLK_T::APBCLK1: DACCKEN Mask           */

#define CLK_APBCLK1_PWM0CKEN_Pos         (16)                                              /*!< CLK_T::APBCLK1: PWM0CKEN Position      */
#define CLK_APBCLK1_PWM0CKEN_Msk         (0x1ul << CLK_APBCLK1_PWM0CKEN_Pos)               /*!< CLK_T::APBCLK1: PWM0CKEN Mask          */

#define CLK_APBCLK1_PWM1CKEN_Pos         (17)                                              /*!< CLK_T::APBCLK1: PWM1CKEN Position      */
#define CLK_APBCLK1_PWM1CKEN_Msk         (0x1ul << CLK_APBCLK1_PWM1CKEN_Pos)               /*!< CLK_T::APBCLK1: PWM1CKEN Mask          */

#define CLK_APBCLK1_BPWM0CKEN_Pos        (18)                                              /*!< CLK_T::APBCLK1: BPWM0CKEN Position     */
#define CLK_APBCLK1_BPWM0CKEN_Msk        (0x1ul << CLK_APBCLK1_BPWM0CKEN_Pos)              /*!< CLK_T::APBCLK1: BPWM0CKEN Mask         */

#define CLK_APBCLK1_BPWM1CKEN_Pos        (19)                                              /*!< CLK_T::APBCLK1: BPWM1CKEN Position     */
#define CLK_APBCLK1_BPWM1CKEN_Msk        (0x1ul << CLK_APBCLK1_BPWM1CKEN_Pos)              /*!< CLK_T::APBCLK1: BPWM1CKEN Mask         */

#define CLK_APBCLK1_OPACKEN_Pos          (30)                                              /*!< CLK_T::APBCLK1: OPACKEN Position       */
#define CLK_APBCLK1_OPACKEN_Msk          (0x1ul << CLK_APBCLK1_OPACKEN_Pos)                /*!< CLK_T::APBCLK1: OPACKEN Mask           */

#define CLK_APBCLK1_PSIOCKEN_Pos         (31)                                              /*!< CLK_T::APBCLK1: PSIOCKEN Position      */
#define CLK_APBCLK1_PSIOCKEN_Msk         (0x1ul << CLK_APBCLK1_PSIOCKEN_Pos)               /*!< CLK_T::APBCLK1: PSIOCKEN Mask          */

#define CLK_CLKSEL0_HCLKSEL_Pos          (0)                                               /*!< CLK_T::CLKSEL0: HCLKSEL Position       */
#define CLK_CLKSEL0_HCLKSEL_Msk          (0x7ul << CLK_CLKSEL0_HCLKSEL_Pos)                /*!< CLK_T::CLKSEL0: HCLKSEL Mask           */

#define CLK_CLKSEL0_STCLKSEL_Pos         (3)                                               /*!< CLK_T::CLKSEL0: STCLKSEL Position      */
#define CLK_CLKSEL0_STCLKSEL_Msk         (0x7ul << CLK_CLKSEL0_STCLKSEL_Pos)               /*!< CLK_T::CLKSEL0: STCLKSEL Mask          */

#define CLK_CLKSEL0_USBDSEL_Pos          (8)                                               /*!< CLK_T::CLKSEL0: USBDSEL Position       */
#define CLK_CLKSEL0_USBDSEL_Msk          (0x1ul << CLK_CLKSEL0_USBDSEL_Pos)                /*!< CLK_T::CLKSEL0: USBDSEL Mask           */

#define CLK_CLKSEL1_WDTSEL_Pos           (0)                                               /*!< CLK_T::CLKSEL1: WDTSEL Position        */
#define CLK_CLKSEL1_WDTSEL_Msk           (0x3ul << CLK_CLKSEL1_WDTSEL_Pos)                 /*!< CLK_T::CLKSEL1: WDTSEL Mask            */

#define CLK_CLKSEL1_WWDTSEL_Pos          (2)                                               /*!< CLK_T::CLKSEL1: WWDTSEL Position       */
#define CLK_CLKSEL1_WWDTSEL_Msk          (0x3ul << CLK_CLKSEL1_WWDTSEL_Pos)                /*!< CLK_T::CLKSEL1: WWDTSEL Mask           */

#define CLK_CLKSEL1_CLKOSEL_Pos          (4)                                               /*!< CLK_T::CLKSEL1: CLKOSEL Position       */
#define CLK_CLKSEL1_CLKOSEL_Msk          (0x7ul << CLK_CLKSEL1_CLKOSEL_Pos)                /*!< CLK_T::CLKSEL1: CLKOSEL Mask           */

#define CLK_CLKSEL1_TMR0SEL_Pos          (8)                                               /*!< CLK_T::CLKSEL1: TMR0SEL Position       */
#define CLK_CLKSEL1_TMR0SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR0SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR0SEL Mask           */

#define CLK_CLKSEL1_TMR1SEL_Pos          (12)                                              /*!< CLK_T::CLKSEL1: TMR1SEL Position       */
#define CLK_CLKSEL1_TMR1SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR1SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR1SEL Mask           */

#define CLK_CLKSEL1_TMR2SEL_Pos          (16)                                              /*!< CLK_T::CLKSEL1: TMR2SEL Position       */
#define CLK_CLKSEL1_TMR2SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR2SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR2SEL Mask           */

#define CLK_CLKSEL1_TMR3SEL_Pos          (20)                                              /*!< CLK_T::CLKSEL1: TMR3SEL Position       */
#define CLK_CLKSEL1_TMR3SEL_Msk          (0x7ul << CLK_CLKSEL1_TMR3SEL_Pos)                /*!< CLK_T::CLKSEL1: TMR3SEL Mask           */

#define CLK_CLKSEL1_UART0SEL_Pos         (24)                                              /*!< CLK_T::CLKSEL1: UART0SEL Position      */
#define CLK_CLKSEL1_UART0SEL_Msk         (0x7ul << CLK_CLKSEL1_UART0SEL_Pos)               /*!< CLK_T::CLKSEL1: UART0SEL Mask          */

#define CLK_CLKSEL1_UART1SEL_Pos         (28)                                              /*!< CLK_T::CLKSEL1: UART1SEL Position      */
#define CLK_CLKSEL1_UART1SEL_Msk         (0x7ul << CLK_CLKSEL1_UART1SEL_Pos)               /*!< CLK_T::CLKSEL1: UART1SEL Mask          */

#define CLK_CLKSEL2_PWM0SEL_Pos          (0)                                               /*!< CLK_T::CLKSEL2: PWM0SEL Position       */
#define CLK_CLKSEL2_PWM0SEL_Msk          (0x1ul << CLK_CLKSEL2_PWM0SEL_Pos)                /*!< CLK_T::CLKSEL2: PWM0SEL Mask           */

#define CLK_CLKSEL2_PWM1SEL_Pos          (1)                                               /*!< CLK_T::CLKSEL2: PWM1SEL Position       */
#define CLK_CLKSEL2_PWM1SEL_Msk          (0x1ul << CLK_CLKSEL2_PWM1SEL_Pos)                /*!< CLK_T::CLKSEL2: PWM1SEL Mask           */

#define CLK_CLKSEL2_QSPI0SEL_Pos         (2)                                               /*!< CLK_T::CLKSEL2: QSPI0SEL Position      */
#define CLK_CLKSEL2_QSPI0SEL_Msk         (0x3ul << CLK_CLKSEL2_QSPI0SEL_Pos)               /*!< CLK_T::CLKSEL2: QSPI0SEL Mask          */

#define CLK_CLKSEL2_SPI0SEL_Pos          (4)                                               /*!< CLK_T::CLKSEL2: SPI0SEL Position       */
#define CLK_CLKSEL2_SPI0SEL_Msk          (0x3ul << CLK_CLKSEL2_SPI0SEL_Pos)                /*!< CLK_T::CLKSEL2: SPI0SEL Mask           */

#define CLK_CLKSEL2_BPWM0SEL_Pos         (8)                                               /*!< CLK_T::CLKSEL2: BPWM0SEL Position      */
#define CLK_CLKSEL2_BPWM0SEL_Msk         (0x1ul << CLK_CLKSEL2_BPWM0SEL_Pos)               /*!< CLK_T::CLKSEL2: BPWM0SEL Mask          */

#define CLK_CLKSEL2_BPWM1SEL_Pos         (9)                                               /*!< CLK_T::CLKSEL2: BPWM1SEL Position      */
#define CLK_CLKSEL2_BPWM1SEL_Msk         (0x1ul << CLK_CLKSEL2_BPWM1SEL_Pos)               /*!< CLK_T::CLKSEL2: BPWM1SEL Mask          */

#define CLK_CLKSEL2_PSIOSEL_Pos          (28)                                              /*!< CLK_T::CLKSEL2: PSIOSEL Position       */
#define CLK_CLKSEL2_PSIOSEL_Msk          (0x7ul << CLK_CLKSEL2_PSIOSEL_Pos)                /*!< CLK_T::CLKSEL2: PSIOSEL Mask           */

#define CLK_CLKSEL3_SC0SEL_Pos           (0)                                               /*!< CLK_T::CLKSEL3: SC0SEL Position        */
#define CLK_CLKSEL3_SC0SEL_Msk           (0x3ul << CLK_CLKSEL3_SC0SEL_Pos)                 /*!< CLK_T::CLKSEL3: SC0SEL Mask            */

#define CLK_CLKSEL3_UART2SEL_Pos         (24)                                              /*!< CLK_T::CLKSEL3: UART2SEL Position      */
#define CLK_CLKSEL3_UART2SEL_Msk         (0x7ul << CLK_CLKSEL3_UART2SEL_Pos)               /*!< CLK_T::CLKSEL3: UART2SEL Mask          */

#define CLK_CLKDIV0_HCLKDIV_Pos          (0)                                               /*!< CLK_T::CLKDIV0: HCLKDIV Position       */
#define CLK_CLKDIV0_HCLKDIV_Msk          (0xful << CLK_CLKDIV0_HCLKDIV_Pos)                /*!< CLK_T::CLKDIV0: HCLKDIV Mask           */

#define CLK_CLKDIV0_USBDIV_Pos           (4)                                               /*!< CLK_T::CLKDIV0: USBDIV Position        */
#define CLK_CLKDIV0_USBDIV_Msk           (0xful << CLK_CLKDIV0_USBDIV_Pos)                 /*!< CLK_T::CLKDIV0: USBDIV Mask            */

#define CLK_CLKDIV0_UART0DIV_Pos         (8)                                               /*!< CLK_T::CLKDIV0: UART0DIV Position      */
#define CLK_CLKDIV0_UART0DIV_Msk         (0xful << CLK_CLKDIV0_UART0DIV_Pos)               /*!< CLK_T::CLKDIV0: UART0DIV Mask          */

#define CLK_CLKDIV0_UART1DIV_Pos         (12)                                              /*!< CLK_T::CLKDIV0: UART1DIV Position      */
#define CLK_CLKDIV0_UART1DIV_Msk         (0xful << CLK_CLKDIV0_UART1DIV_Pos)               /*!< CLK_T::CLKDIV0: UART1DIV Mask          */

#define CLK_CLKDIV0_EADCDIV_Pos          (16)                                              /*!< CLK_T::CLKDIV0: EADCDIV Position       */
#define CLK_CLKDIV0_EADCDIV_Msk          (0xfful << CLK_CLKDIV0_EADCDIV_Pos)               /*!< CLK_T::CLKDIV0: EADCDIV Mask           */

#define CLK_CLKDIV1_SC0DIV_Pos           (0)                                               /*!< CLK_T::CLKDIV1: SC0DIV Position        */
#define CLK_CLKDIV1_SC0DIV_Msk           (0xfful << CLK_CLKDIV1_SC0DIV_Pos)                /*!< CLK_T::CLKDIV1: SC0DIV Mask            */

#define CLK_CLKDIV1_PSIODIV_Pos          (24)                                              /*!< CLK_T::CLKDIV1: PSIODIV Position       */
#define CLK_CLKDIV1_PSIODIV_Msk          (0xfful << CLK_CLKDIV1_PSIODIV_Pos)               /*!< CLK_T::CLKDIV1: PSIODIV Mask           */

#define CLK_CLKDIV4_UART2DIV_Pos         (0)                                               /*!< CLK_T::CLKDIV4: UART2DIV Position      */
#define CLK_CLKDIV4_UART2DIV_Msk         (0xful << CLK_CLKDIV4_UART2DIV_Pos)               /*!< CLK_T::CLKDIV4: UART2DIV Mask          */

#define CLK_PCLKDIV_APB0DIV_Pos          (0)                                               /*!< CLK_T::PCLKDIV: APB0DIV Position       */
#define CLK_PCLKDIV_APB0DIV_Msk          (0x7ul << CLK_PCLKDIV_APB0DIV_Pos)                /*!< CLK_T::PCLKDIV: APB0DIV Mask           */

#define CLK_PCLKDIV_APB1DIV_Pos          (4)                                               /*!< CLK_T::PCLKDIV: APB1DIV Position       */
#define CLK_PCLKDIV_APB1DIV_Msk          (0x7ul << CLK_PCLKDIV_APB1DIV_Pos)                /*!< CLK_T::PCLKDIV: APB1DIV Mask           */

#define CLK_PLLCTL_FBDIV_Pos             (0)                                               /*!< CLK_T::PLLCTL: FBDIV Position          */
#define CLK_PLLCTL_FBDIV_Msk             (0x3ful << CLK_PLLCTL_FBDIV_Pos)                  /*!< CLK_T::PLLCTL: FBDIV Mask              */

#define CLK_PLLCTL_INDIV_Pos             (9)                                               /*!< CLK_T::PLLCTL: INDIV Position          */
#define CLK_PLLCTL_INDIV_Msk             (0xful << CLK_PLLCTL_INDIV_Pos)                   /*!< CLK_T::PLLCTL: INDIV Mask              */

#define CLK_PLLCTL_OUTDIV_Pos            (14)                                              /*!< CLK_T::PLLCTL: OUTDIV Position         */
#define CLK_PLLCTL_OUTDIV_Msk            (0x3ul << CLK_PLLCTL_OUTDIV_Pos)                  /*!< CLK_T::PLLCTL: OUTDIV Mask             */

#define CLK_PLLCTL_PD_Pos                (16)                                              /*!< CLK_T::PLLCTL: PD Position             */
#define CLK_PLLCTL_PD_Msk                (0x1ul << CLK_PLLCTL_PD_Pos)                      /*!< CLK_T::PLLCTL: PD Mask                 */

#define CLK_PLLCTL_BP_Pos                (17)                                              /*!< CLK_T::PLLCTL: BP Position             */
#define CLK_PLLCTL_BP_Msk                (0x1ul << CLK_PLLCTL_BP_Pos)                      /*!< CLK_T::PLLCTL: BP Mask                 */

#define CLK_PLLCTL_OE_Pos                (18)                                              /*!< CLK_T::PLLCTL: OE Position             */
#define CLK_PLLCTL_OE_Msk                (0x1ul << CLK_PLLCTL_OE_Pos)                      /*!< CLK_T::PLLCTL: OE Mask                 */

#define CLK_PLLCTL_PLLSRC_Pos            (19)                                              /*!< CLK_T::PLLCTL: PLLSRC Position         */
#define CLK_PLLCTL_PLLSRC_Msk            (0x3ul << CLK_PLLCTL_PLLSRC_Pos)                  /*!< CLK_T::PLLCTL: PLLSRC Mask             */

#define CLK_PLLCTL_STBSEL_Pos            (23)                                              /*!< CLK_T::PLLCTL: STBSEL Position         */
#define CLK_PLLCTL_STBSEL_Msk            (0x1ul << CLK_PLLCTL_STBSEL_Pos)                  /*!< CLK_T::PLLCTL: STBSEL Mask             */

#define CLK_PLLCTL_PLL_CLF_EN_Pos        (24)                                              /*!< CLK_T::PLLCTL: PLL_CLF_EN Position     */
#define CLK_PLLCTL_PLL_CLF_EN_Msk        (0x1ul << CLK_PLLCTL_PLL_CLF_EN_Pos)              /*!< CLK_T::PLLCTL: PLL_CLF_EN Mask         */

#define CLK_STATUS_HXTSTB_Pos            (0)                                               /*!< CLK_T::STATUS: HXTSTB Position         */
#define CLK_STATUS_HXTSTB_Msk            (0x1ul << CLK_STATUS_HXTSTB_Pos)                  /*!< CLK_T::STATUS: HXTSTB Mask             */

#define CLK_STATUS_LXTSTB_Pos            (1)                                               /*!< CLK_T::STATUS: LXTSTB Position         */
#define CLK_STATUS_LXTSTB_Msk            (0x1ul << CLK_STATUS_LXTSTB_Pos)                  /*!< CLK_T::STATUS: LXTSTB Mask             */

#define CLK_STATUS_PLLSTB_Pos            (2)                                               /*!< CLK_T::STATUS: PLLSTB Position         */
#define CLK_STATUS_PLLSTB_Msk            (0x1ul << CLK_STATUS_PLLSTB_Pos)                  /*!< CLK_T::STATUS: PLLSTB Mask             */

#define CLK_STATUS_LIRCSTB_Pos           (3)                                               /*!< CLK_T::STATUS: LIRCSTB Position        */
#define CLK_STATUS_LIRCSTB_Msk           (0x1ul << CLK_STATUS_LIRCSTB_Pos)                 /*!< CLK_T::STATUS: LIRCSTB Mask            */

#define CLK_STATUS_HIRCSTB_Pos           (4)                                               /*!< CLK_T::STATUS: HIRCSTB Position        */
#define CLK_STATUS_HIRCSTB_Msk           (0x1ul << CLK_STATUS_HIRCSTB_Pos)                 /*!< CLK_T::STATUS: HIRCSTB Mask            */

#define CLK_STATUS_MIRCSTB_Pos           (6)                                               /*!< CLK_T::STATUS: MIRCSTB Position        */
#define CLK_STATUS_MIRCSTB_Msk           (0x1ul << CLK_STATUS_MIRCSTB_Pos)                 /*!< CLK_T::STATUS: MIRCSTB Mask            */

#define CLK_STATUS_CLKSFAIL_Pos          (7)                                               /*!< CLK_T::STATUS: CLKSFAIL Position       */
#define CLK_STATUS_CLKSFAIL_Msk          (0x1ul << CLK_STATUS_CLKSFAIL_Pos)                /*!< CLK_T::STATUS: CLKSFAIL Mask           */

#define CLK_CLKOCTL_FREQSEL_Pos          (0)                                               /*!< CLK_T::CLKOCTL: FREQSEL Position       */
#define CLK_CLKOCTL_FREQSEL_Msk          (0xful << CLK_CLKOCTL_FREQSEL_Pos)                /*!< CLK_T::CLKOCTL: FREQSEL Mask           */

#define CLK_CLKOCTL_CLKOEN_Pos           (4)                                               /*!< CLK_T::CLKOCTL: CLKOEN Position        */
#define CLK_CLKOCTL_CLKOEN_Msk           (0x1ul << CLK_CLKOCTL_CLKOEN_Pos)                 /*!< CLK_T::CLKOCTL: CLKOEN Mask            */

#define CLK_CLKOCTL_DIV1EN_Pos           (5)                                               /*!< CLK_T::CLKOCTL: DIV1EN Position        */
#define CLK_CLKOCTL_DIV1EN_Msk           (0x1ul << CLK_CLKOCTL_DIV1EN_Pos)                 /*!< CLK_T::CLKOCTL: DIV1EN Mask            */

#define CLK_CLKOCTL_CLK1HZEN_Pos         (6)                                               /*!< CLK_T::CLKOCTL: CLK1HZEN Position      */
#define CLK_CLKOCTL_CLK1HZEN_Msk         (0x1ul << CLK_CLKOCTL_CLK1HZEN_Pos)               /*!< CLK_T::CLKOCTL: CLK1HZEN Mask          */

#define CLK_CLKDCTL_HXTFDEN_Pos          (4)                                               /*!< CLK_T::CLKDCTL: HXTFDEN Position       */
#define CLK_CLKDCTL_HXTFDEN_Msk          (0x1ul << CLK_CLKDCTL_HXTFDEN_Pos)                /*!< CLK_T::CLKDCTL: HXTFDEN Mask           */

#define CLK_CLKDCTL_HXTFIEN_Pos          (5)                                               /*!< CLK_T::CLKDCTL: HXTFIEN Position       */
#define CLK_CLKDCTL_HXTFIEN_Msk          (0x1ul << CLK_CLKDCTL_HXTFIEN_Pos)                /*!< CLK_T::CLKDCTL: HXTFIEN Mask           */

#define CLK_CLKDCTL_LXTFDEN_Pos          (12)                                              /*!< CLK_T::CLKDCTL: LXTFDEN Position       */
#define CLK_CLKDCTL_LXTFDEN_Msk          (0x1ul << CLK_CLKDCTL_LXTFDEN_Pos)                /*!< CLK_T::CLKDCTL: LXTFDEN Mask           */

#define CLK_CLKDCTL_LXTFIEN_Pos          (13)                                              /*!< CLK_T::CLKDCTL: LXTFIEN Position       */
#define CLK_CLKDCTL_LXTFIEN_Msk          (0x1ul << CLK_CLKDCTL_LXTFIEN_Pos)                /*!< CLK_T::CLKDCTL: LXTFIEN Mask           */

#define CLK_CLKDCTL_HXTFQDEN_Pos         (16)                                              /*!< CLK_T::CLKDCTL: HXTFQDEN Position      */
#define CLK_CLKDCTL_HXTFQDEN_Msk         (0x1ul << CLK_CLKDCTL_HXTFQDEN_Pos)               /*!< CLK_T::CLKDCTL: HXTFQDEN Mask          */

#define CLK_CLKDCTL_HXTFQIEN_Pos         (17)                                              /*!< CLK_T::CLKDCTL: HXTFQIEN Position      */
#define CLK_CLKDCTL_HXTFQIEN_Msk         (0x1ul << CLK_CLKDCTL_HXTFQIEN_Pos)               /*!< CLK_T::CLKDCTL: HXTFQIEN Mask          */

#define CLK_CLKDSTS_HXTFIF_Pos           (0)                                               /*!< CLK_T::CLKDSTS: HXTFIF Position        */
#define CLK_CLKDSTS_HXTFIF_Msk           (0x1ul << CLK_CLKDSTS_HXTFIF_Pos)                 /*!< CLK_T::CLKDSTS: HXTFIF Mask            */

#define CLK_CLKDSTS_LXTFIF_Pos           (1)                                               /*!< CLK_T::CLKDSTS: LXTFIF Position        */
#define CLK_CLKDSTS_LXTFIF_Msk           (0x1ul << CLK_CLKDSTS_LXTFIF_Pos)                 /*!< CLK_T::CLKDSTS: LXTFIF Mask            */

#define CLK_CLKDSTS_HXTFQIF_Pos          (8)                                               /*!< CLK_T::CLKDSTS: HXTFQIF Position       */
#define CLK_CLKDSTS_HXTFQIF_Msk          (0x1ul << CLK_CLKDSTS_HXTFQIF_Pos)                /*!< CLK_T::CLKDSTS: HXTFQIF Mask           */

#define CLK_CDUPB_UPERBD_Pos             (0)                                               /*!< CLK_T::CDUPB: UPERBD Position          */
#define CLK_CDUPB_UPERBD_Msk             (0x3fful << CLK_CDUPB_UPERBD_Pos)                 /*!< CLK_T::CDUPB: UPERBD Mask              */

#define CLK_CDLOWB_LOWERBD_Pos           (0)                                               /*!< CLK_T::CDLOWB: LOWERBD Position        */
#define CLK_CDLOWB_LOWERBD_Msk           (0x3fful << CLK_CDLOWB_LOWERBD_Pos)               /*!< CLK_T::CDLOWB: LOWERBD Mask            */

#define CLK_PMUCTL_PDMSEL_Pos            (0)                                               /*!< CLK_T::PMUCTL: PDMSEL Position         */
#define CLK_PMUCTL_PDMSEL_Msk            (0x7ul << CLK_PMUCTL_PDMSEL_Pos)                  /*!< CLK_T::PMUCTL: PDMSEL Mask             */

#define CLK_PMUCTL_WKTMREN_Pos           (8)                                               /*!< CLK_T::PMUCTL: WKTMREN Position        */
#define CLK_PMUCTL_WKTMREN_Msk           (0x1ul << CLK_PMUCTL_WKTMREN_Pos)                 /*!< CLK_T::PMUCTL: WKTMREN Mask            */

#define CLK_PMUCTL_WKTMRIS_Pos           (9)                                               /*!< CLK_T::PMUCTL: WKTMRIS Position        */
#define CLK_PMUCTL_WKTMRIS_Msk           (0x7ul << CLK_PMUCTL_WKTMRIS_Pos)                 /*!< CLK_T::PMUCTL: WKTMRIS Mask            */

#define CLK_PMUCTL_WKPINEN0_Pos          (16)                                              /*!< CLK_T::PMUCTL: WKPINEN0 Position        */
#define CLK_PMUCTL_WKPINEN0_Msk          (0x3ul << CLK_PMUCTL_WKPINEN0_Pos)                /*!< CLK_T::PMUCTL: WKPINEN0 Mask            */

#define CLK_PMUCTL_WKPINDBEN_Pos         (22)                                              /*!< CLK_T::PMUCTL: WKPINDBEN Position        */
#define CLK_PMUCTL_WKPINDBEN_Msk         (0x1ul << CLK_PMUCTL_WKPINDBEN_Pos)               /*!< CLK_T::PMUCTL: WKPINDBEN Mask            */

#define CLK_PMUCTL_RTCWKEN_Pos           (23)                                              /*!< CLK_T::PMUCTL: RTCWKEN Position        */
#define CLK_PMUCTL_RTCWKEN_Msk           (0x1ul << CLK_PMUCTL_RTCWKEN_Pos)                 /*!< CLK_T::PMUCTL: RTCWKEN Mask            */

#define CLK_PMUCTL_WKPINEN1_Pos          (24)                                              /*!< CLK_T::PMUCTL: WKPINEN1 Position        */
#define CLK_PMUCTL_WKPINEN1_Msk          (0x3ul << CLK_PMUCTL_WKPINEN1_Pos)                /*!< CLK_T::PMUCTL: WKPINEN1 Mask            */

#define CLK_PMUCTL_WKPINEN2_Pos          (26)                                              /*!< CLK_T::PMUCTL: WKPINEN2 Position        */
#define CLK_PMUCTL_WKPINEN2_Msk          (0x3ul << CLK_PMUCTL_WKPINEN2_Pos)                /*!< CLK_T::PMUCTL: WKPINEN2 Mask            */

#define CLK_PMUCTL_WKPINEN3_Pos          (28)                                              /*!< CLK_T::PMUCTL: WKPINEN3 Position        */
#define CLK_PMUCTL_WKPINEN3_Msk          (0x3ul << CLK_PMUCTL_WKPINEN3_Pos)                /*!< CLK_T::PMUCTL: WKPINEN3 Mask            */

#define CLK_PMUCTL_WKPINEN4_Pos          (30)                                              /*!< CLK_T::PMUCTL: WKPINEN4 Position        */
#define CLK_PMUCTL_WKPINEN4_Msk          (0x3ul << CLK_PMUCTL_WKPINEN4_Pos)                /*!< CLK_T::PMUCTL: WKPINEN4 Mask            */

#define CLK_PMUSTS_PINWK0_Pos            (0)                                               /*!< CLK_T::PMUSTS: PINWK0 Position          */
#define CLK_PMUSTS_PINWK0_Msk            (0x1ul << CLK_PMUSTS_PINWK0_Pos)                  /*!< CLK_T::PMUSTS: PINWK0 Mask              */

#define CLK_PMUSTS_TMRWK_Pos             (1)                                               /*!< CLK_T::PMUSTS: TMRWK Position          */
#define CLK_PMUSTS_TMRWK_Msk             (0x1ul << CLK_PMUSTS_TMRWK_Pos)                   /*!< CLK_T::PMUSTS: TMRWK Mask              */

#define CLK_PMUSTS_RTCWK_Pos             (2)                                               /*!< CLK_T::PMUSTS: RTCWK Position          */
#define CLK_PMUSTS_RTCWK_Msk             (0x1ul << CLK_PMUSTS_RTCWK_Pos)                   /*!< CLK_T::PMUSTS: RTCWK Mask              */

#define CLK_PMUSTS_PINWK1_Pos            (3)                                               /*!< CLK_T::PMUSTS: PINWK1 Position          */
#define CLK_PMUSTS_PINWK1_Msk            (0x1ul << CLK_PMUSTS_PINWK1_Pos)                  /*!< CLK_T::PMUSTS: PINWK1 Mask              */

#define CLK_PMUSTS_PINWK2_Pos            (4)                                               /*!< CLK_T::PMUSTS: PINWK2 Position          */
#define CLK_PMUSTS_PINWK2_Msk            (0x1ul << CLK_PMUSTS_PINWK2_Pos)                  /*!< CLK_T::PMUSTS: PINWK2 Mask              */

#define CLK_PMUSTS_PINWK3_Pos            (5)                                               /*!< CLK_T::PMUSTS: PINWK3 Position          */
#define CLK_PMUSTS_PINWK3_Msk            (0x1ul << CLK_PMUSTS_PINWK3_Pos)                  /*!< CLK_T::PMUSTS: PINWK3 Mask              */

#define CLK_PMUSTS_PINWK4_Pos            (6)                                               /*!< CLK_T::PMUSTS: PINWK4 Position          */
#define CLK_PMUSTS_PINWK4_Msk            (0x1ul << CLK_PMUSTS_PINWK4_Pos)                  /*!< CLK_T::PMUSTS: PINWK4 Mask              */

#define CLK_PMUSTS_LVRWK_Pos             (12)                                              /*!< CLK_T::PMUSTS: LVRWK Position          */
#define CLK_PMUSTS_LVRWK_Msk             (0x1ul << CLK_PMUSTS_LVRWK_Pos)                   /*!< CLK_T::PMUSTS: LVRWK Mask              */

#define CLK_PMUSTS_CLRWK_Pos             (31)                                              /*!< CLK_T::PMUSTS: CLRWK Position          */
#define CLK_PMUSTS_CLRWK_Msk             (0x1ul << CLK_PMUSTS_CLRWK_Pos)                   /*!< CLK_T::PMUSTS: CLRWK Mask              */

#define CLK_HXTFSEL_HXTFSEL_Pos          (0)                                               /*!< CLK_T::HXTFSEL: HXTFSEL Position       */
#define CLK_HXTFSEL_HXTFSEL_Msk          (0x1ul << CLK_HXTFSEL_HXTFSEL_Pos)                /*!< CLK_T::HXTFSEL: HXTFSEL Mask           */

/**@}*/ /* CLK_CONST */
/**@}*/ /* end of CLK register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
    #pragma no_anon_unions
#endif

#endif /* __CLK_REG_H__ */

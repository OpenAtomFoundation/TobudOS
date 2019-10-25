/****************************************************************************
 * @file     clk.h
 * @version  V1.10
 * @brief    M251 series CLK driver source file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CLK_Driver CLK Driver
  @{
*/

/** @addtogroup CLK_EXPORTED_CONSTANTS CLK Exported Constants
  @{
*/

#define FREQ_4MHZ          4000000UL
#define FREQ_8MHZ          8000000UL
#define FREQ_16MHZ         16000000UL
#define FREQ_25MHZ         25000000UL
#define FREQ_32MHZ         32000000UL
#define FREQ_48MHZ         48000000UL
#define FREQ_50MHZ         50000000UL
#define FREQ_64MHZ         64000000UL
#define FREQ_72MHZ         72000000UL
#define FREQ_96MHZ         96000000UL
#define FREQ_100MHZ        100000000UL

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL0 constant definitions.  (Write-protection)                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL0_HCLKSEL_HXT           (0x00UL<<CLK_CLKSEL0_HCLKSEL_Pos)     /*!< Setting clock source as external X'tal */
#define CLK_CLKSEL0_HCLKSEL_LXT           (0x01UL<<CLK_CLKSEL0_HCLKSEL_Pos)     /*!< Setting clock source as external X'tal 32.768KHz*/
#define CLK_CLKSEL0_HCLKSEL_PLL           (0x02UL<<CLK_CLKSEL0_HCLKSEL_Pos)     /*!< Setting clock source as PLL output */
#define CLK_CLKSEL0_HCLKSEL_LIRC          (0x03UL<<CLK_CLKSEL0_HCLKSEL_Pos)     /*!< Setting clock source as internal 10KHz RC clock */
#define CLK_CLKSEL0_HCLKSEL_MIRC          (0x05UL<<CLK_CLKSEL0_HCLKSEL_Pos)     /*!< Setting clock source as USBPLL clock */
#define CLK_CLKSEL0_HCLKSEL_HIRC          (0x07UL<<CLK_CLKSEL0_HCLKSEL_Pos)     /*!< Setting clock source as internal 22.1184MHz RC clock */

#define CLK_CLKSEL0_STCLKSEL_HXT          (0x00UL<<CLK_CLKSEL0_STCLKSEL_Pos)    /*!< Setting clock source as external X'tal */
#define CLK_CLKSEL0_STCLKSEL_LXT          (0x01UL<<CLK_CLKSEL0_STCLKSEL_Pos)    /*!< Setting clock source as external X'tal 32.768KHz*/
#define CLK_CLKSEL0_STCLKSEL_HXT_DIV2     (0x02UL<<CLK_CLKSEL0_STCLKSEL_Pos)    /*!< Setting clock source as external X'tal/2 */
#define CLK_CLKSEL0_STCLKSEL_HCLK_DIV2    (0x03UL<<CLK_CLKSEL0_STCLKSEL_Pos)    /*!< Setting clock source as HCLK/2 */
#define CLK_CLKSEL0_STCLKSEL_HIRC_DIV2    (0x07UL<<CLK_CLKSEL0_STCLKSEL_Pos)    /*!< Setting clock source as internal 22.1184MHz RC clock/2 */

#define SysTick_CTRL_STCLKSEL_HCLK        (0x01UL<<SysTick_CTRL_CLKSOURCE_Pos)  /*!< Setting SysTick clock source as HCLK */
#define CLK_CLKSEL0_STCLKSEL_HCLK         SysTick_CTRL_STCLKSEL_HCLK            /*!< Setting SysTick clock source as HCLK (Backward compatible) */

#define CLK_CLKSEL0_USBDSEL_HIRC          (0x00UL<<CLK_CLKSEL0_USBDSEL_Pos)     /*!< Setting clock source as external X'tal */
#define CLK_CLKSEL0_USBDSEL_PLL           (0x01UL<<CLK_CLKSEL0_USBDSEL_Pos)     /*!< Setting clock source as external X'tal 32.768KHz*/

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL1 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL1_WDTSEL_LXT           (0x1UL<<CLK_CLKSEL1_WDTSEL_Pos)        /*!< Setting WDT clock source as external X'tal 32.768KHz*/
#define CLK_CLKSEL1_WDTSEL_HCLK_DIV2048  (0x2UL<<CLK_CLKSEL1_WDTSEL_Pos)        /*!< Setting WDT clock source as HCLK/2048 */
#define CLK_CLKSEL1_WDTSEL_LIRC          (0x3UL<<CLK_CLKSEL1_WDTSEL_Pos)        /*!< Setting WDT clock source as internal 10KHz RC clock */

#define CLK_CLKSEL1_WWDTSEL_HCLK_DIV2048 (0x2UL<<CLK_CLKSEL1_WWDTSEL_Pos)       /*!< Setting WDT clock source as HCLK/2048 */
#define CLK_CLKSEL1_WWDTSEL_LIRC         (0x3UL<<CLK_CLKSEL1_WWDTSEL_Pos)       /*!< Setting WDT clock source as internal 10KHz RC clock */

#define CLK_CLKSEL1_CLKOSEL_HXT          (0x0UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as external X'tal */
#define CLK_CLKSEL1_CLKOSEL_LXT          (0x1UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as external X'tal 32.768KHz  */
#define CLK_CLKSEL1_CLKOSEL_HCLK         (0x2UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as HCLK */
#define CLK_CLKSEL1_CLKOSEL_HIRC         (0x3UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as external internal 48MHz RC clock */
#define CLK_CLKSEL1_CLKOSEL_LIRC         (0x4UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as external internal 32.768KHz RC clock */
#define CLK_CLKSEL1_CLKOSEL_MIRC         (0x5UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as external internal 4.032MHz RC clock */
#define CLK_CLKSEL1_CLKOSEL_PLL          (0x6UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as PLL */
#define CLK_CLKSEL1_CLKOSEL_SOF          (0x7UL<<CLK_CLKSEL1_CLKOSEL_Pos)       /*!< Setting CLKO clock source as USB SOF 1kHz */

#define CLK_CLKSEL1_TMR0SEL_HXT          (0x0UL<<CLK_CLKSEL1_TMR0SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal */
#define CLK_CLKSEL1_TMR0SEL_LXT          (0x1UL<<CLK_CLKSEL1_TMR0SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal 32.768KHz */
#define CLK_CLKSEL1_TMR0SEL_PCLK0        (0x2UL<<CLK_CLKSEL1_TMR0SEL_Pos)       /*!< Setting Timer 0 clock source as PCLK */
#define CLK_CLKSEL1_TMR0SEL_EXT_TRG      (0x3UL<<CLK_CLKSEL1_TMR0SEL_Pos)       /*!< Setting Timer 0 clock source as external trigger */
#define CLK_CLKSEL1_TMR0SEL_LIRC         (0x5UL<<CLK_CLKSEL1_TMR0SEL_Pos)       /*!< Setting Timer 0 clock source as internal 10KHz RC clock */
#define CLK_CLKSEL1_TMR0SEL_HIRC         (0x7UL<<CLK_CLKSEL1_TMR0SEL_Pos)       /*!< Setting Timer 0 clock source as internal 22.1184MHz RC clock */

#define CLK_CLKSEL1_TMR1SEL_HXT          (0x0UL<<CLK_CLKSEL1_TMR1SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal */
#define CLK_CLKSEL1_TMR1SEL_LXT          (0x1UL<<CLK_CLKSEL1_TMR1SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal 32.768KHz */
#define CLK_CLKSEL1_TMR1SEL_PCLK0        (0x2UL<<CLK_CLKSEL1_TMR1SEL_Pos)       /*!< Setting Timer 0 clock source as PCLK */
#define CLK_CLKSEL1_TMR1SEL_EXT_TRG      (0x3UL<<CLK_CLKSEL1_TMR1SEL_Pos)       /*!< Setting Timer 0 clock source as external trigger */
#define CLK_CLKSEL1_TMR1SEL_LIRC         (0x5UL<<CLK_CLKSEL1_TMR1SEL_Pos)       /*!< Setting Timer 0 clock source as internal 10KHz RC clock */
#define CLK_CLKSEL1_TMR1SEL_HIRC         (0x7UL<<CLK_CLKSEL1_TMR1SEL_Pos)       /*!< Setting Timer 0 clock source as internal 22.1184MHz RC clock */

#define CLK_CLKSEL1_TMR2SEL_HXT          (0x0UL<<CLK_CLKSEL1_TMR2SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal */
#define CLK_CLKSEL1_TMR2SEL_LXT          (0x1UL<<CLK_CLKSEL1_TMR2SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal 32.768KHz */
#define CLK_CLKSEL1_TMR2SEL_PCLK1        (0x2UL<<CLK_CLKSEL1_TMR2SEL_Pos)       /*!< Setting Timer 0 clock source as PCLK */
#define CLK_CLKSEL1_TMR2SEL_EXT_TRG      (0x3UL<<CLK_CLKSEL1_TMR2SEL_Pos)       /*!< Setting Timer 0 clock source as external trigger */
#define CLK_CLKSEL1_TMR2SEL_LIRC         (0x5UL<<CLK_CLKSEL1_TMR2SEL_Pos)       /*!< Setting Timer 0 clock source as internal 10KHz RC clock */
#define CLK_CLKSEL1_TMR2SEL_HIRC         (0x7UL<<CLK_CLKSEL1_TMR2SEL_Pos)       /*!< Setting Timer 0 clock source as internal 22.1184MHz RC clock */

#define CLK_CLKSEL1_TMR3SEL_HXT          (0x0UL<<CLK_CLKSEL1_TMR3SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal */
#define CLK_CLKSEL1_TMR3SEL_LXT          (0x1UL<<CLK_CLKSEL1_TMR3SEL_Pos)       /*!< Setting Timer 0 clock source as external X'tal 32.768KHz */
#define CLK_CLKSEL1_TMR3SEL_PCLK1        (0x2UL<<CLK_CLKSEL1_TMR3SEL_Pos)       /*!< Setting Timer 0 clock source as PCLK */
#define CLK_CLKSEL1_TMR3SEL_EXT_TRG      (0x3UL<<CLK_CLKSEL1_TMR3SEL_Pos)       /*!< Setting Timer 0 clock source as external trigger */
#define CLK_CLKSEL1_TMR3SEL_LIRC         (0x5UL<<CLK_CLKSEL1_TMR3SEL_Pos)       /*!< Setting Timer 0 clock source as internal 10KHz RC clock */
#define CLK_CLKSEL1_TMR3SEL_HIRC         (0x7UL<<CLK_CLKSEL1_TMR3SEL_Pos)       /*!< Setting Timer 0 clock source as internal 22.1184MHz RC clock */

#define CLK_CLKSEL1_UART0SEL_HXT         (0x0UL<<CLK_CLKSEL1_UART0SEL_Pos)      /*!< Setting UART0 clock source as external X'tal */
#define CLK_CLKSEL1_UART0SEL_PLL         (0x1UL<<CLK_CLKSEL1_UART0SEL_Pos)      /*!< Setting UART0 clock source as external PLL */
#define CLK_CLKSEL1_UART0SEL_LXT         (0x2UL<<CLK_CLKSEL1_UART0SEL_Pos)      /*!< Setting UART0 clock source as external X'tal */
#define CLK_CLKSEL1_UART0SEL_HIRC        (0x3UL<<CLK_CLKSEL1_UART0SEL_Pos)      /*!< Setting UART0 clock source as external internal 48MHz RC clock */
#define CLK_CLKSEL1_UART0SEL_PCLK0       (0x4UL<<CLK_CLKSEL1_UART0SEL_Pos)      /*!< Setting UART0 clock source as external PCLK */
#define CLK_CLKSEL1_UART0SEL_LIRC        (0x5UL<<CLK_CLKSEL1_UART0SEL_Pos)      /*!< Setting UART0 clock source as external internal 38.4KHz RC clock */

#define CLK_CLKSEL1_UART1SEL_HXT         (0x0UL<<CLK_CLKSEL1_UART1SEL_Pos)      /*!< Setting UART1 clock source as external X'tal */
#define CLK_CLKSEL1_UART1SEL_PLL         (0x1UL<<CLK_CLKSEL1_UART1SEL_Pos)      /*!< Setting UART1 clock source as external PLL */
#define CLK_CLKSEL1_UART1SEL_LXT         (0x2UL<<CLK_CLKSEL1_UART1SEL_Pos)      /*!< Setting UART1 clock source as external X'tal */
#define CLK_CLKSEL1_UART1SEL_HIRC        (0x3UL<<CLK_CLKSEL1_UART1SEL_Pos)      /*!< Setting UART1 clock source as external internal 48MHz RC clock */
#define CLK_CLKSEL1_UART1SEL_PCLK1       (0x4UL<<CLK_CLKSEL1_UART1SEL_Pos)      /*!< Setting UART1 clock source as external PCLK */
#define CLK_CLKSEL1_UART1SEL_LIRC        (0x5UL<<CLK_CLKSEL1_UART1SEL_Pos)      /*!< Setting UART1 clock source as external internal 38.4KHz RC clock */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL2 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

#define CLK_CLKSEL2_PWM0SEL_PLL          (0x0UL<<CLK_CLKSEL2_PWM0SEL_Pos)       /*!< Setting PWM0 and PWM1 clock source as external X'tal */
#define CLK_CLKSEL2_PWM0SEL_PCLK0        (0x1UL<<CLK_CLKSEL2_PWM0SEL_Pos)       /*!< Setting PWM0 and PWM1 clock source as PCLK */

#define CLK_CLKSEL2_PWM1SEL_PLL          (0x0UL<<CLK_CLKSEL2_PWM1SEL_Pos)       /*!< Setting PWM0 and PWM1 clock source as external X'tal */
#define CLK_CLKSEL2_PWM1SEL_PCLK1        (0x1UL<<CLK_CLKSEL2_PWM1SEL_Pos)       /*!< Setting PWM0 and PWM1 clock source as PCLK */

#define CLK_CLKSEL2_QSPI0SEL_HXT         (0x0UL<<CLK_CLKSEL2_QSPI0SEL_Pos)      /*!< Setting SPI clock source as HXT */
#define CLK_CLKSEL2_QSPI0SEL_PLL         (0x1UL<<CLK_CLKSEL2_QSPI0SEL_Pos)      /*!< Setting SPI clock source as PLL */
#define CLK_CLKSEL2_QSPI0SEL_PCLK0       (0x2UL<<CLK_CLKSEL2_QSPI0SEL_Pos)      /*!< Setting SPI clock source as PCLK0 */
#define CLK_CLKSEL2_QSPI0SEL_HIRC        (0x3UL<<CLK_CLKSEL2_QSPI0SEL_Pos)      /*!< Setting SPI clock source as HIRC */

#define CLK_CLKSEL2_SPI0SEL_HXT          (0x0UL<<CLK_CLKSEL2_SPI0SEL_Pos)       /*!< Setting SPI clock source as HXT */
#define CLK_CLKSEL2_SPI0SEL_PLL          (0x1UL<<CLK_CLKSEL2_SPI0SEL_Pos)       /*!< Setting SPI clock source as PLL */
#define CLK_CLKSEL2_SPI0SEL_PCLK1        (0x2UL<<CLK_CLKSEL2_SPI0SEL_Pos)       /*!< Setting SPI clock source as PCLK0 */
#define CLK_CLKSEL2_SPI0SEL_HIRC         (0x3UL<<CLK_CLKSEL2_SPI0SEL_Pos)       /*!< Setting SPI clock source as HIRC */

#define CLK_CLKSEL2_BPWM0SEL_PLL         (0x0UL<<CLK_CLKSEL2_BPWM0SEL_Pos)      /*!< Setting PWM0 and PWM1 clock source as external X'tal */
#define CLK_CLKSEL2_BPWM0SEL_PCLK0       (0x1UL<<CLK_CLKSEL2_BPWM0SEL_Pos)      /*!< Setting PWM0 and PWM1 clock source as PCLK */

#define CLK_CLKSEL2_BPWM1SEL_PLL         (0x0UL<<CLK_CLKSEL2_BPWM1SEL_Pos)      /*!< Setting PWM0 and PWM1 clock source as external X'tal */
#define CLK_CLKSEL2_BPWM1SEL_PCLK1       (0x1UL<<CLK_CLKSEL2_BPWM1SEL_Pos)      /*!< Setting PWM0 and PWM1 clock source as PCLK */

#define CLK_CLKSEL2_PSIOSEL_HXT          (0x0UL<<CLK_CLKSEL2_PSIOSEL_Pos)
#define CLK_CLKSEL2_PSIOSEL_LXT          (0x1UL<<CLK_CLKSEL2_PSIOSEL_Pos)
#define CLK_CLKSEL2_PSIOSEL_PCLK1        (0x2UL<<CLK_CLKSEL2_PSIOSEL_Pos)
#define CLK_CLKSEL2_PSIOSEL_PLL          (0x3UL<<CLK_CLKSEL2_PSIOSEL_Pos)
#define CLK_CLKSEL2_PSIOSEL_LIRC         (0x4UL<<CLK_CLKSEL2_PSIOSEL_Pos)
#define CLK_CLKSEL2_PSIOSEL_HIRC         (0x7UL<<CLK_CLKSEL2_PSIOSEL_Pos)

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL3 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL3_SC0SEL_HXT           (0x0UL<<CLK_CLKSEL3_SC0SEL_Pos)        /*!< Setting SC0 clock source as external X'tal */
#define CLK_CLKSEL3_SC0SEL_PLL           (0x1UL<<CLK_CLKSEL3_SC0SEL_Pos)        /*!< Setting SC0 clock source as PLL */
#define CLK_CLKSEL3_SC0SEL_PCLK0         (0x2UL<<CLK_CLKSEL3_SC0SEL_Pos)        /*!< Setting SC0 clock source as PCLK */
#define CLK_CLKSEL3_SC0SEL_HIRC          (0x3UL<<CLK_CLKSEL3_SC0SEL_Pos)        /*!< Setting SC0 clock source as internal 22.1184MHz RC clock */

#define CLK_CLKSEL3_UART2SEL_HXT         (0x0UL<<CLK_CLKSEL3_UART2SEL_Pos)      /*!< Setting UART2 clock source as external X'tal */
#define CLK_CLKSEL3_UART2SEL_PLL         (0x1UL<<CLK_CLKSEL3_UART2SEL_Pos)      /*!< Setting UART2 clock source as external PLL */
#define CLK_CLKSEL3_UART2SEL_LXT         (0x2UL<<CLK_CLKSEL3_UART2SEL_Pos)      /*!< Setting UART2 clock source as external X'tal */
#define CLK_CLKSEL3_UART2SEL_HIRC        (0x3UL<<CLK_CLKSEL3_UART2SEL_Pos)      /*!< Setting UART2 clock source as external internal 48MHz RC clock */
#define CLK_CLKSEL3_UART2SEL_PCLK0       (0x4UL<<CLK_CLKSEL3_UART2SEL_Pos)      /*!< Setting UART2 clock source as external PCLK */
#define CLK_CLKSEL3_UART2SEL_LIRC        (0x5UL<<CLK_CLKSEL3_UART2SEL_Pos)      /*!< Setting UART2 clock source as external internal 38.4KHz RC clock */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV0 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV0_HCLK(x)  (((x)-1) << CLK_CLKDIV0_HCLKDIV_Pos)       /*!< CLKDIV Setting for HCLK clock divider. It could be 1~16 */
#define CLK_CLKDIV0_USB(x)   (((x)-1) << CLK_CLKDIV0_USBDIV_Pos)        /*!< CLKDIV Setting for USB clock divider. It could be 1~16, has to be 1 or even */
#define CLK_CLKDIV0_UART0(x) (((x)-1) << CLK_CLKDIV0_UART0DIV_Pos)      /*!< CLKDIV Setting for UART clock divider. It could be 1~16 */
#define CLK_CLKDIV0_UART1(x) (((x)-1) << CLK_CLKDIV0_UART1DIV_Pos)      /*!< CLKDIV Setting for UART clock divider. It could be 1~16 */
#define CLK_CLKDIV0_EADC(x)  (((x)-1) << CLK_CLKDIV0_EADCDIV_Pos)       /*!< CLKDIV Setting for EADC clock divider. It could be 1~256 */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV1 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV1_SC0(x)  (((x)-1) << CLK_CLKDIV1_SC0DIV_Pos) /*!< CLKDIV Setting for SC0 clock divider. It could be 1~256 */
#define CLK_CLKDIV1_PSIO(x) (((x)-1) << CLK_CLKDIV1_PSIODIV_Pos)/*!< CLKDIV Setting for PSIO clock divider. It could be 1~256 */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV4 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV4_UART2(x) (((x)-1) << CLK_CLKDIV4_UART2DIV_Pos)      /*!< CLKDIV Setting for UART clock divider. It could be 1~16 */

/*---------------------------------------------------------------------------------------------------------*/
/*  PCLKDIV constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_PCLKDIV_APB0DIV_DIV1       (0x0UL<<CLK_PCLKDIV_APB0DIV_Pos)
#define CLK_PCLKDIV_APB0DIV_DIV2       (0x1UL<<CLK_PCLKDIV_APB0DIV_Pos)
#define CLK_PCLKDIV_APB0DIV_DIV4       (0x2UL<<CLK_PCLKDIV_APB0DIV_Pos)
#define CLK_PCLKDIV_APB0DIV_DIV8       (0x3UL<<CLK_PCLKDIV_APB0DIV_Pos)
#define CLK_PCLKDIV_APB0DIV_DIV16      (0x4UL<<CLK_PCLKDIV_APB0DIV_Pos)
#define CLK_PCLKDIV_APB0DIV_DIV32      (0x5UL<<CLK_PCLKDIV_APB0DIV_Pos)

#define CLK_PCLKDIV_APB1DIV_DIV1       (0x0UL<<CLK_PCLKDIV_APB1DIV_Pos)
#define CLK_PCLKDIV_APB1DIV_DIV2       (0x1UL<<CLK_PCLKDIV_APB1DIV_Pos)
#define CLK_PCLKDIV_APB1DIV_DIV4       (0x2UL<<CLK_PCLKDIV_APB1DIV_Pos)
#define CLK_PCLKDIV_APB1DIV_DIV8       (0x3UL<<CLK_PCLKDIV_APB1DIV_Pos)
#define CLK_PCLKDIV_APB1DIV_DIV16      (0x4UL<<CLK_PCLKDIV_APB1DIV_Pos)
#define CLK_PCLKDIV_APB1DIV_DIV32      (0x5UL<<CLK_PCLKDIV_APB1DIV_Pos)

/*---------------------------------------------------------------------------------------------------------*/
/*  PLLCTL constant definitions. PLL = FIN * NF / NR / NO                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_PLLCTL_PLLSRC_HXT       (0x0ul << CLK_PLLCTL_PLLSRC_Pos)    /*!< For PLL clock source is HXT.  4~12MHz < FIN < 24MHz */
#define CLK_PLLCTL_PLLSRC_HIRC_DIV4 (0x1ul << CLK_PLLCTL_PLLSRC_Pos)    /*!< For PLL clock source is HIRC/4. 12 MHz< FIN < 12MHz */
#define CLK_PLLCTL_PLLSRC_MIRC      (0x3ul << CLK_PLLCTL_PLLSRC_Pos)    /*!< For PLL clock source is MIRC. 4 MHz< FIN < 4MHz */

#define CLK_PLLCTL_NF(x)            (x)                            /*!< x must be constant and 1 <= x <= 63. 64MHz < FIN*NF/NR < 100MHz. */
#define CLK_PLLCTL_NR(x)            ((x)<<CLK_PLLCTL_INDIV_Pos)    /*!< x must be constant and 1 <= x <= 15.  4MHz < FIN/NR < 8MHz */

#define CLK_PLLCTL_NO_1             0x0000UL        /*!< For output divider is 1 */
#define CLK_PLLCTL_NO_2             0x4000UL        /*!< For output divider is 2 */
#define CLK_PLLCTL_NO_4             0xC000UL        /*!< For output divider is 4 */

#define CLK_PLLCTL_16MHz_HXT        (CLK_PLLCTL_PLLSRC_HXT  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(16)     | CLK_PLLCTL_NO_4)
#define CLK_PLLCTL_48MHz_HXT        (CLK_PLLCTL_PLLSRC_HXT  | CLK_PLLCTL_NR(2)  | CLK_PLLCTL_NF(16)     | CLK_PLLCTL_NO_2) /*!< Predefined PLLCTL setting for 48MHz PLL output with HXT(12MHz X'tal) */
#define CLK_PLLCTL_50MHz_HXT        (CLK_PLLCTL_PLLSRC_HXT  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(25)     | CLK_PLLCTL_NO_2)
#define CLK_PLLCTL_72MHz_HXT        (CLK_PLLCTL_PLLSRC_HXT  | CLK_PLLCTL_NR(2)  | CLK_PLLCTL_NF(12)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_80MHz_HXT        (CLK_PLLCTL_PLLSRC_HXT  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(20)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_96MHz_HXT        (CLK_PLLCTL_PLLSRC_HXT  | CLK_PLLCTL_NR(2)  | CLK_PLLCTL_NF(16)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_100MHz_HXT       (CLK_PLLCTL_PLLSRC_HXT  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(25)     | CLK_PLLCTL_NO_1)

#define CLK_PLLCTL_16MHz_HIRC_DIV4  (CLK_PLLCTL_PLLSRC_HIRC_DIV4  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(16)     | CLK_PLLCTL_NO_4)
#define CLK_PLLCTL_48MHz_HIRC_DIV4  (CLK_PLLCTL_PLLSRC_HIRC_DIV4  | CLK_PLLCTL_NR(2)  | CLK_PLLCTL_NF(16)     | CLK_PLLCTL_NO_2) /*!< Predefined PLLCTL setting for 48MHz PLL output with HIRC_DIV4(12MHz IRC) */
#define CLK_PLLCTL_50MHz_HIRC_DIV4  (CLK_PLLCTL_PLLSRC_HIRC_DIV4  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(25)     | CLK_PLLCTL_NO_2)
#define CLK_PLLCTL_72MHz_HIRC_DIV4  (CLK_PLLCTL_PLLSRC_HIRC_DIV4  | CLK_PLLCTL_NR(2)  | CLK_PLLCTL_NF(12)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_80MHz_HIRC_DIV4  (CLK_PLLCTL_PLLSRC_HIRC_DIV4  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(20)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_96MHz_HIRC_DIV4  (CLK_PLLCTL_PLLSRC_HIRC_DIV4  | CLK_PLLCTL_NR(2)  | CLK_PLLCTL_NF(16)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_100MHz_HIRC_DIV4 (CLK_PLLCTL_PLLSRC_HIRC_DIV4  | CLK_PLLCTL_NR(3)  | CLK_PLLCTL_NF(25)     | CLK_PLLCTL_NO_1)

#define CLK_PLLCTL_16MHz_MIRC       (CLK_PLLCTL_PLLSRC_MIRC  | CLK_PLLCTL_NR(1)  | CLK_PLLCTL_NF(16)     | CLK_PLLCTL_NO_4)
#define CLK_PLLCTL_48MHz_MIRC       (CLK_PLLCTL_PLLSRC_MIRC  | CLK_PLLCTL_NR(1)  | CLK_PLLCTL_NF(24)     | CLK_PLLCTL_NO_2) /*!< Predefined PLLCTL setting for 48MHz PLL output with MIRC(4MHz IRC) */
#define CLK_PLLCTL_50MHz_MIRC       (CLK_PLLCTL_PLLSRC_MIRC  | CLK_PLLCTL_NR(1)  | CLK_PLLCTL_NF(25)     | CLK_PLLCTL_NO_2)
#define CLK_PLLCTL_72MHz_MIRC       (CLK_PLLCTL_PLLSRC_MIRC  | CLK_PLLCTL_NR(1)  | CLK_PLLCTL_NF(18)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_80MHz_MIRC       (CLK_PLLCTL_PLLSRC_MIRC  | CLK_PLLCTL_NR(1)  | CLK_PLLCTL_NF(20)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_96MHz_MIRC       (CLK_PLLCTL_PLLSRC_MIRC  | CLK_PLLCTL_NR(1)  | CLK_PLLCTL_NF(24)     | CLK_PLLCTL_NO_1)
#define CLK_PLLCTL_100MHz_MIRC      (CLK_PLLCTL_PLLSRC_MIRC  | CLK_PLLCTL_NR(1)  | CLK_PLLCTL_NF(25)     | CLK_PLLCTL_NO_1)
/*---------------------------------------------------------------------------------------------------------*/
/*  MODULE constant definitions.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
/* APBCLK(31:30)|CLKSEL(29:28)|CLKSEL_Msk(27:25) |CLKSEL_Pos(24:20)|CLKDIV(19:18)|CLKDIV_Msk(17:10)|CLKDIV_Pos(9:5)|IP_EN_Pos(4:0) */

#define MODULE_APBCLK(x)        (((x) >>30) & 0x3)    /*!< Calculate AHBCLK/APBCLK offset on MODULE index, 0x0:AHBCLK, 0x1:APBCLK0, 0x2:APBCLK1 */
#define MODULE_CLKSEL(x)        (((x) >>28) & 0x3)    /*!< Calculate CLKSEL offset on MODULE index, 0x0:CLKSEL0, 0x1:CLKSEL1, 0x2:CLKSEL2, 0x3:CLKSEL3 */
#define MODULE_CLKSEL_Msk(x)    (((x) >>25) & 0x7)    /*!< Calculate CLKSEL mask offset on MODULE index */
#define MODULE_CLKSEL_Pos(x)    (((x) >>20) & 0x1f)   /*!< Calculate CLKSEL position offset on MODULE index */
#define MODULE_CLKDIV(x)        (((x) >>18) & 0x3)    /*!< Calculate APBCLK CLKDIV on MODULE index, 0x0:CLKDIV, 0x1:CLKDIV1, 0x2:CLKDIV3, 0x3:CLKDIV4 */
#define MODULE_CLKDIV_Msk(x)    (((x) >>10) & 0xff)   /*!< Calculate CLKDIV mask offset on MODULE index */
#define MODULE_CLKDIV_Pos(x)    (((x) >>5 ) & 0x1f)   /*!< Calculate CLKDIV position offset on MODULE index */
#define MODULE_IP_EN_Pos(x)     (((x) >>0 ) & 0x1f)   /*!< Calculate APBCLK offset on MODULE index */
#define MODULE_NoMsk            0x0                   /*!< Not mask on MODULE index */
#define NA                      MODULE_NoMsk          /*!< Not Available */

#define MODULE_APBCLK_ENC(x)        (((x) & 0x03) << 30)   /*!< MODULE index, 0x0:AHBCLK, 0x1:APBCLK0, 0x2:APBCLK1 */
#define MODULE_CLKSEL_ENC(x)        (((x) & 0x03) << 28)   /*!< CLKSEL offset on MODULE index, 0x0:CLKSEL0, 0x1:CLKSEL1, 0x2:CLKSEL2, 0x3:CLKSEL3 */
#define MODULE_CLKSEL_Msk_ENC(x)    (((x) & 0x07) << 25)   /*!< CLKSEL mask offset on MODULE index */
#define MODULE_CLKSEL_Pos_ENC(x)    (((x) & 0x1f) << 20)   /*!< CLKSEL position offset on MODULE index */
#define MODULE_CLKDIV_ENC(x)        (((x) & 0x03) << 18)   /*!< APBCLK CLKDIV on MODULE index, 0x0:CLKDIV, 0x1:CLKDIV1, 0x2:CLKDIV3, 0x3:CLKDIV4*/
#define MODULE_CLKDIV_Msk_ENC(x)    (((x) & 0xff) << 10)   /*!< CLKDIV mask offset on MODULE index */
#define MODULE_CLKDIV_Pos_ENC(x)    (((x) & 0x1f) <<  5)   /*!< CLKDIV position offset on MODULE index */
#define MODULE_IP_EN_Pos_ENC(x)     (((x) & 0x1f) <<  0)   /*!< AHBCLK/APBCLK offset on MODULE index */


//AHBCLK
#define PDMA_MODULE      (MODULE_APBCLK_ENC( 0)|MODULE_IP_EN_Pos_ENC(CLK_AHBCLK_PDMACKEN_Pos)|\
                          MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                          MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))    /*!< PDMA Module */

#define ISP_MODULE       (MODULE_APBCLK_ENC( 0)|MODULE_IP_EN_Pos_ENC(CLK_AHBCLK_ISPCKEN_Pos)|\
                          MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                          MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))    /*!< ISP Module */

#define CRC_MODULE       (MODULE_APBCLK_ENC( 0)|MODULE_IP_EN_Pos_ENC(CLK_AHBCLK_CRCCKEN_Pos)|\
                          MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                          MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))    /*!< CRC Module */

#define EBI_MODULE       (MODULE_APBCLK_ENC( 0)|MODULE_IP_EN_Pos_ENC(CLK_AHBCLK_EBICKEN_Pos)|\
                          MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                          MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))    /*!< EBI Module */

#define CRPT_MODULE      (MODULE_APBCLK_ENC( 0)|MODULE_IP_EN_Pos_ENC(CLK_AHBCLK_CRYPTCKEN_Pos)|\
                          MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                          MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))    /*!< CRPT Module */

#define FMCIDLE_MODULE   (MODULE_APBCLK_ENC( 0)|MODULE_IP_EN_Pos_ENC(CLK_AHBCLK_FMCIDLE_Pos)|\
                          MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                          MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))    /*!< FMCIDLE Module */

//APBCLK0
#define WDT_MODULE     (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_WDTCKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 3)|MODULE_CLKSEL_Pos_ENC( 0)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< WDT Module */

#define WWDT_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_WDTCKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 3)|MODULE_CLKSEL_Pos_ENC( 2)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< WWDT Module */

#define CLKO_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_CLKOCKEN_Pos) |\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC( 4)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< CLKO Module */

#define TMR0_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_TMR0CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC( 8)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< TMR0 Module */

#define TMR1_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_TMR1CKEN_Pos) |\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC(12)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< TMR1 Module */

#define TMR2_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_TMR2CKEN_Pos) |\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC(16)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< TMR2 Module */

#define TMR3_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_TMR3CKEN_Pos) |\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC(20)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< TMR3 Module */

#define UART0_MODULE   (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_UART0CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC(24)|\
                        MODULE_CLKDIV_ENC( 0)|MODULE_CLKDIV_Msk_ENC(0x0F)|MODULE_CLKDIV_Pos_ENC( 8))    /*!< UART0 Module */

#define UART1_MODULE   (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_UART1CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 1)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC(28)|\
                        MODULE_CLKDIV_ENC( 0)|MODULE_CLKDIV_Msk_ENC(0x0F)|MODULE_CLKDIV_Pos_ENC( 12))   /*!< UART1 Module */

#define UART2_MODULE   (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_UART2CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 3)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC(24)|\
                        MODULE_CLKDIV_ENC( 3)|MODULE_CLKDIV_Msk_ENC(0x0F)|MODULE_CLKDIV_Pos_ENC( 0))    /*!< UART2 Module */

#define I2C0_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_I2C0CKEN_Pos) |\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< I2C0 Module */

#define I2C1_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_I2C1CKEN_Pos) |\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< I2C1 Module */

#define QSPI0_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_QSPI0CKEN_Pos) |\
                         MODULE_CLKSEL_ENC( 2)|MODULE_CLKSEL_Msk_ENC( 3)|MODULE_CLKSEL_Pos_ENC( 2)|\
                         MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))     /*!< QSPI0 Module */

#define SPI0_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_SPI0CKEN_Pos) |\
                        MODULE_CLKSEL_ENC( 2)|MODULE_CLKSEL_Msk_ENC( 3)|MODULE_CLKSEL_Pos_ENC( 4)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< SPI0 Module */

#define RTC_MODULE     (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_RTCCKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< RTC Module */

#define EADC_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_EADCCKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC( 0)|MODULE_CLKDIV_Msk_ENC(0xFF)|MODULE_CLKDIV_Pos_ENC(16))    /*!< ADC Module */

#define ACMP01_MODULE  (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_ACMP01CKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< ACMP Module */

#define USBD_MODULE    (MODULE_APBCLK_ENC( 1)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK0_USBDCKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 0)|MODULE_CLKSEL_Msk_ENC(1)|MODULE_CLKSEL_Pos_ENC(8)|\
                        MODULE_CLKDIV_ENC( 0)|MODULE_CLKDIV_Msk_ENC(0x0F)|MODULE_CLKDIV_Pos_ENC(4))     /*!< USBD Module */

//APBCLK1
#define PWM0_MODULE    (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_PWM0CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 2)|MODULE_CLKSEL_Msk_ENC( 1)|MODULE_CLKSEL_Pos_ENC( 0)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< PWM0 Module */

#define PWM1_MODULE    (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_PWM1CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 2)|MODULE_CLKSEL_Msk_ENC( 1)|MODULE_CLKSEL_Pos_ENC( 1)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< PWM1 Module */

#define DAC_MODULE     (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_DACCKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< DAC Module */

#define OPA_MODULE     (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_OPACKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< OPA Module */

#define USCI0_MODULE   (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_USCI0CKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< USCI0 Module */

#define USCI1_MODULE   (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_USCI1CKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< USCI1 Module */

#define USCI2_MODULE   (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_USCI2CKEN_Pos)|\
                        MODULE_CLKSEL_ENC(NA)|MODULE_CLKSEL_Msk_ENC(NA)|MODULE_CLKSEL_Pos_ENC(NA)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< USCI2 Module */

#define SC0_MODULE     (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_SC0CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 3)|MODULE_CLKSEL_Msk_ENC( 3)|MODULE_CLKSEL_Pos_ENC( 0)|\
                        MODULE_CLKDIV_ENC( 1)|MODULE_CLKDIV_Msk_ENC(0xFF)|MODULE_CLKDIV_Pos_ENC( 0))    /*!< SC0 Module */

#define BPWM0_MODULE   (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_BPWM0CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 2)|MODULE_CLKSEL_Msk_ENC( 1)|MODULE_CLKSEL_Pos_ENC( 8)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< BPWM0 Module */

#define BPWM1_MODULE   (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_BPWM1CKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 2)|MODULE_CLKSEL_Msk_ENC( 1)|MODULE_CLKSEL_Pos_ENC( 9)|\
                        MODULE_CLKDIV_ENC(NA)|MODULE_CLKDIV_Msk_ENC(NA)|MODULE_CLKDIV_Pos_ENC(NA))      /*!< BPWM0 Module */

#define PSIO_MODULE    (MODULE_APBCLK_ENC( 2)|MODULE_IP_EN_Pos_ENC(CLK_APBCLK1_PSIOCKEN_Pos)|\
                        MODULE_CLKSEL_ENC( 2)|MODULE_CLKSEL_Msk_ENC( 7)|MODULE_CLKSEL_Pos_ENC( 28)|\
                        MODULE_CLKDIV_ENC( 1)|MODULE_CLKDIV_Msk_ENC(0xFF)|MODULE_CLKDIV_Pos_ENC( 24))   /*!< PSIO Module */

/*---------------------------------------------------------------------------------------------------------*/
/*  PDMSEL constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_PMUCTL_PDMSEL_PD          (0x0UL<<CLK_PMUCTL_PDMSEL_Pos)
#define CLK_PMUCTL_PDMSEL_FWPD        (0x2UL<<CLK_PMUCTL_PDMSEL_Pos)
#define CLK_PMUCTL_PDMSEL_DPD         (0x6UL<<CLK_PMUCTL_PDMSEL_Pos)

/*---------------------------------------------------------------------------------------------------------*/
/*  WKTMRIS constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_PMUCTL_WKTMRIS_128        (0x0UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 128 OSC10K clocks (~3.34 ms) */
#define CLK_PMUCTL_WKTMRIS_256        (0x1UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 256 OSC10K clocks (~6.67 ms) */
#define CLK_PMUCTL_WKTMRIS_512        (0x2UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 512 OSC10K clocks (~13.34 ms) */
#define CLK_PMUCTL_WKTMRIS_1024       (0x3UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 1024 OSC10K clocks (~26.67ms) */
#define CLK_PMUCTL_WKTMRIS_4096       (0x4UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 4096 OSC10K clocks (~106.67ms) */
#define CLK_PMUCTL_WKTMRIS_8192       (0x5UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 8192 OSC10K clocks (~213.34ms) */
#define CLK_PMUCTL_WKTMRIS_16384      (0x6UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 16384 OSC10K clocks (~426.67ms) */
#define CLK_PMUCTL_WKTMRIS_32768      (0x7UL << CLK_PMUCTL_WKTMRIS_Pos)     /*!< Select Wake-up Timer Time-out Interval is 65536 OSC10K clocks (~852.34ms) */

/*---------------------------------------------------------------------------------------------------------*/
/*  DPD Pin Rising/Falling Edge Wake-up Enable constant definitions.                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_DPDWKPIN_0            (0x0UL)                                /*!< Wake-up pin0 (GPC.0) at Deep Power-down mode */
#define CLK_DPDWKPIN_1            (0x1UL)                                /*!< Wake-up pin1 (GPB.0) at Deep Power-down mode */
#define CLK_DPDWKPIN_2            (0x2UL)                                /*!< Wake-up pin2 (GPB.2) at Deep Power-down mode */
#define CLK_DPDWKPIN_3            (0x3UL)                                /*!< Wake-up pin3 (GPB.12) at Deep Power-down mode */
#define CLK_DPDWKPIN_4            (0x4UL)                                /*!< Wake-up pin4 (GPF.6) at Deep Power-down mode */

#define CLK_DPDWKPIN_RISING       (0x1UL)
#define CLK_DPDWKPIN_FALLING      (0x2UL)
#define CLK_DPDWKPIN_BOTHEDGE     (0x3UL)

#define CLK_DPDWKPIN0_DISABLE     (0x0UL << CLK_PMUCTL_WKPINEN0_Pos)     /*!< Disable Wake-up pin0 (GPC.0) at Deep Power-down mode */
#define CLK_DPDWKPIN0_RISING      (0x1UL << CLK_PMUCTL_WKPINEN0_Pos)     /*!< Enable Wake-up pin0 (GPC.0) rising edge at Deep Power-down mode */
#define CLK_DPDWKPIN0_FALLING     (0x2UL << CLK_PMUCTL_WKPINEN0_Pos)     /*!< Enable Wake-up pin0 (GPC.0) falling edge at Deep Power-down mode */
#define CLK_DPDWKPIN0_BOTHEDGE    (0x3UL << CLK_PMUCTL_WKPINEN0_Pos)     /*!< Enable Wake-up pin0 (GPC.0) both edge at Deep Power-down mode */

#define CLK_DPDWKPIN1_DISABLE     (0x0UL << CLK_PMUCTL_WKPINEN1_Pos)     /*!< Disable Wake-up pin1 (GPB.0) at Deep Power-down mode */
#define CLK_DPDWKPIN1_RISING      (0x1UL << CLK_PMUCTL_WKPINEN1_Pos)     /*!< Enable Wake-up pin1 (GPB.0) rising edge at Deep Power-down mode */
#define CLK_DPDWKPIN1_FALLING     (0x2UL << CLK_PMUCTL_WKPINEN1_Pos)     /*!< Enable Wake-up pin1 (GPB.0) falling edge at Deep Power-down mode */
#define CLK_DPDWKPIN1_BOTHEDGE    (0x3UL << CLK_PMUCTL_WKPINEN1_Pos)     /*!< Enable Wake-up pin1 (GPB.0) both edge at Deep Power-down mode */

#define CLK_DPDWKPIN2_DISABLE     (0x0UL << CLK_PMUCTL_WKPINEN2_Pos)     /*!< Disable Wake-up pin2 (GPB.2) at Deep Power-down mode */
#define CLK_DPDWKPIN2_RISING      (0x1UL << CLK_PMUCTL_WKPINEN2_Pos)     /*!< Enable Wake-up pin2 (GPB.2) rising edge at Deep Power-down mode */
#define CLK_DPDWKPIN2_FALLING     (0x2UL << CLK_PMUCTL_WKPINEN2_Pos)     /*!< Enable Wake-up pin2 (GPB.2) falling edge at Deep Power-down mode */
#define CLK_DPDWKPIN2_BOTHEDGE    (0x3UL << CLK_PMUCTL_WKPINEN2_Pos)     /*!< Enable Wake-up pin2 (GPB.2) both edge at Deep Power-down mode */

#define CLK_DPDWKPIN3_DISABLE     (0x0UL << CLK_PMUCTL_WKPINEN3_Pos)     /*!< Disable Wake-up pin3 (GPB.12) at Deep Power-down mode */
#define CLK_DPDWKPIN3_RISING      (0x1UL << CLK_PMUCTL_WKPINEN3_Pos)     /*!< Enable Wake-up pin3 (GPB.12) rising edge at Deep Power-down mode */
#define CLK_DPDWKPIN3_FALLING     (0x2UL << CLK_PMUCTL_WKPINEN3_Pos)     /*!< Enable Wake-up pin3 (GPB.12) falling edge at Deep Power-down mode */
#define CLK_DPDWKPIN3_BOTHEDGE    (0x3UL << CLK_PMUCTL_WKPINEN3_Pos)     /*!< Enable Wake-up pin3 (GPB.12) both edge at Deep Power-down mode */

#define CLK_DPDWKPIN4_DISABLE     (0x0UL << CLK_PMUCTL_WKPINEN4_Pos)     /*!< Disable Wake-up pin4 (GPF.6) at Deep Power-down mode */
#define CLK_DPDWKPIN4_RISING      (0x1UL << CLK_PMUCTL_WKPINEN4_Pos)     /*!< Enable Wake-up pin4 (GPF.6) rising edge at Deep Power-down mode */
#define CLK_DPDWKPIN4_FALLING     (0x2UL << CLK_PMUCTL_WKPINEN4_Pos)     /*!< Enable Wake-up pin4 (GPF.6) falling edge at Deep Power-down mode */
#define CLK_DPDWKPIN4_BOTHEDGE    (0x3UL << CLK_PMUCTL_WKPINEN4_Pos)     /*!< Enable Wake-up pin4 (GPF.6) both edge at Deep Power-down mode */

#define CLK_DISABLE_WKTMR(void)       (CLK->PMUCTL &= ~CLK_PMUCTL_WKTMREN_Msk)     /*!< Disable Wake-up timer at Standby or Deep Power-down mode */
#define CLK_ENABLE_WKTMR(void)        (CLK->PMUCTL |= CLK_PMUCTL_WKTMREN_Msk)      /*!< Enable Wake-up timer at Standby or Deep Power-down mode */
#define CLK_DISABLE_DPDWKPIN0(void)   (CLK->PMUCTL &= ~CLK_PMUCTL_WKPINEN0_Msk)    /*!< Disable Wake-up pin0 (GPC.0) at Deep Power-down mode */
#define CLK_DISABLE_DPDWKPIN1(void)   (CLK->PMUCTL &= ~CLK_PMUCTL_WKPINEN1_Msk)    /*!< Disable Wake-up pin1 (GPB.0) at Deep Power-down mode */
#define CLK_DISABLE_DPDWKPIN2(void)   (CLK->PMUCTL &= ~CLK_PMUCTL_WKPINEN2_Msk)    /*!< Disable Wake-up pin2 (GPB.2) at Deep Power-down mode */
#define CLK_DISABLE_DPDWKPIN3(void)   (CLK->PMUCTL &= ~CLK_PMUCTL_WKPINEN3_Msk)    /*!< Disable Wake-up pin3 (GPB.12) at Deep Power-down mode */
#define CLK_DISABLE_DPDWKPIN4(void)   (CLK->PMUCTL &= ~CLK_PMUCTL_WKPINEN4_Msk)    /*!< Disable Wake-up pin4 (GPF.6) at Deep Power-down mode */
#define CLK_DISABLE_WKPINDB(void)     (CLK->PMUCTL &= ~CLK_PMUCTL_WKPINDBEN_Msk)   /*!< Disable Wake-up pin De-bounce function  */
#define CLK_ENABLE_WKPINDB(void)      (CLK->PMUCTL |= CLK_PMUCTL_WKPINDBEN_Msk)    /*!< Enable Wake-up pin De-bounce function  */
#define CLK_DISABLE_RTCWK(void)       (CLK->PMUCTL &= ~CLK_PMUCTL_RTCWKEN_Msk)     /*!< Disable RTC Wake-up at Standby or Deep Power-down mode \hideinitializer */
#define CLK_ENABLE_RTCWK(void)        (CLK->PMUCTL |= CLK_PMUCTL_RTCWKEN_Msk)      /*!< Enable RTC Wake-up at Standby or Deep Power-down mode \hideinitializer */

/**
 * @brief       Set Wake-up Timer Time-out Interval
 *
 * @param[in]   u32Interval   The de-bounce sampling cycle selection. It could be
 *                             - \ref CLK_PMUCTL_WKTMRIS_128
 *                             - \ref CLK_PMUCTL_WKTMRIS_256
 *                             - \ref CLK_PMUCTL_WKTMRIS_512
 *                             - \ref CLK_PMUCTL_WKTMRIS_1024
 *                             - \ref CLK_PMUCTL_WKTMRIS_4096
 *                             - \ref CLK_PMUCTL_WKTMRIS_8192
 *                             - \ref CLK_PMUCTL_WKTMRIS_16384
 *                             - \ref CLK_PMUCTL_WKTMRIS_32768
 *
 * @return      None
 *
 * @details     This function set Wake-up Timer Time-out Interval.
 *
 *
 */
#define CLK_SET_WKTMR_INTERVAL(u32Interval)   (CLK->PMUCTL |= (u32Interval))


/*---------------------------------------------------------------------------------------------------------*/
/* static inline functions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/* Declare these inline functions here to avoid MISRA C 2004 rule 8.1 error */
__STATIC_INLINE void CLK_SysTickDelay(uint32_t u32USec);
__STATIC_INLINE void CLK_SysTickLongDelay(uint32_t u32USec);

/**
  * @brief      This function execute delay function.
  * @param[in]  u32USec  Delay time. The Max value is 2^24 / CPU Clock(MHz). Ex:
  *                             50MHz => 335544us, 48MHz => 349525us, 28MHz => 699050us ...
  * @return     None
  * @details    Use the SysTick to generate the delay time and the UNIT is in us.
  *             The SysTick clock source is from HCLK, i.e the same as system core clock.
  *             User can use SystemCoreClockUpdate() to calculate CyclesPerUs automatically before using this function.
  */
__STATIC_INLINE void CLK_SysTickDelay(uint32_t u32USec)
{
    SysTick->LOAD = u32USec * CyclesPerUs;
    SysTick->VAL  = (0x0u);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0u)
    {
    }

    /* Disable SysTick counter */
    SysTick->CTRL = 0u;
}

/**
  * @brief      This function execute long delay function.
  * @param[in]  u32USec  Delay time.
  * @return     None
  * @details    Use the SysTick to generate the long delay time and the UNIT is in us.
  *             The SysTick clock source is from HCLK, i.e the same as system core clock.
  *             User can use SystemCoreClockUpdate() to calculate CyclesPerUs automatically before using this function.
  */
__STATIC_INLINE void CLK_SysTickLongDelay(uint32_t u32USec)
{
    uint32_t u32Delay;

    /* It should <= 349525us for each delay loop */
    u32Delay = 349525UL;

    do
    {
        if (u32USec > u32Delay)
        {
            u32USec -= u32Delay;
        }
        else
        {
            u32Delay = u32USec;
            u32USec = 0UL;
        }

        SysTick->LOAD = u32Delay * CyclesPerUs;
        SysTick->VAL  = (0x0UL);
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

        /* Waiting for down-count to zero */
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0UL)
        {
        }

        /* Disable SysTick counter */
        SysTick->CTRL = 0UL;

    }
    while (u32USec > 0UL);

}

void CLK_DisableCKO(void);
void CLK_EnableCKO(uint32_t u32ClkSrc, uint32_t u32ClkDiv, uint32_t u32ClkDivBy1En);
void CLK_PowerDown(void);
void CLK_Idle(void);
uint32_t CLK_GetHXTFreq(void);
uint32_t CLK_GetLXTFreq(void);
uint32_t CLK_GetPCLK0Freq(void);
uint32_t CLK_GetPCLK1Freq(void);
uint32_t CLK_GetHCLKFreq(void);
uint32_t CLK_GetCPUFreq(void);
uint32_t CLK_GetPLLClockFreq(void);
void CLK_DisablePLL(void);
uint32_t CLK_EnablePLL(uint32_t u32PllClkSrc, uint32_t u32PllFreq);
void CLK_SetHCLK(uint32_t u32ClkSrc, uint32_t u32ClkDiv);
uint32_t CLK_SetCoreClock(uint32_t u32Hclk);
void CLK_SetModuleClock(uint32_t u32ModuleIdx, uint32_t u32ClkSrc, uint32_t u32ClkDiv);
void CLK_SetSysTickClockSrc(uint32_t u32ClkSrc);
void CLK_EnableXtalRC(uint32_t u32ClkMask);
void CLK_DisableXtalRC(uint32_t u32ClkMask);
void CLK_EnableModuleClock(uint32_t u32ModuleIdx);
void CLK_DisableModuleClock(uint32_t u32ModuleIdx);
uint32_t CLK_WaitClockReady(uint32_t u32ClkMask);
void CLK_EnableSysTick(uint32_t u32ClkSrc, uint32_t u32Count);
void CLK_DisableSysTick(void);
void CLK_SetPowerDownMode(uint32_t u32PDMode);
void CLK_EnableDPDWKPin(uint32_t u32Pin, uint32_t u32TriggerType);
void CLK_EnableDPDWKPin0(uint32_t u32TriggerType);
void CLK_EnableDPDWKPin1(uint32_t u32TriggerType);
void CLK_EnableDPDWKPin2(uint32_t u32TriggerType);
void CLK_EnableDPDWKPin3(uint32_t u32TriggerType);
void CLK_EnableDPDWKPin4(uint32_t u32TriggerType);
uint32_t CLK_GetPMUWKSrc(void);
uint32_t CLK_GetModuleClockSource(uint32_t u32ModuleIdx);
uint32_t CLK_GetModuleClockDivider(uint32_t u32ModuleIdx);

/*@}*/ /* end of group CLK_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group CLK_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/

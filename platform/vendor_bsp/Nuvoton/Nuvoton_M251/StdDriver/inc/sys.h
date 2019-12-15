/****************************************************************************
 * @file     sys.h
 * @version  V1.10
 * @brief    M251 series SYS driver source file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SYS_H__
#define __SYS_H__


#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SYS_Driver SYS Driver
  @{
*/

/** @addtogroup SYS_EXPORTED_CONSTANTS SYS Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/*  Module Reset Control Resister constant definitions.                                                    */
/*---------------------------------------------------------------------------------------------------------*/

#define PDMA_RST    ((0x0<<24)|SYS_IPRST0_PDMARST_Pos)      /*!< PDMA reset is one of the SYS_ResetModule parameter */
#define EBI_RST     ((0x0<<24)|SYS_IPRST0_EBIRST_Pos)       /*!< EBI reset is one of the SYS_ResetModule parameter */
#define CRC_RST     ((0x0<<24)|SYS_IPRST0_CRCRST_Pos)       /*!< CRC reset is one of the SYS_ResetModule parameter */
#define CRPT_RST    ((0x0<<24)|SYS_IPRST0_CRYPTRST_Pos)    /*!< CRYPTO reset is one of the SYS_ResetModule parameter */

#define GPIO_RST    ((0x4<<24)|SYS_IPRST1_GPIORST_Pos)      /*!< GPIO reset is one of the SYS_ResetModule parameter */
#define TMR0_RST    ((0x4<<24)|SYS_IPRST1_TMR0RST_Pos)      /*!< TMR0 reset is one of the SYS_ResetModule parameter */
#define TMR1_RST    ((0x4<<24)|SYS_IPRST1_TMR1RST_Pos)      /*!< TMR1 reset is one of the SYS_ResetModule parameter */
#define TMR2_RST    ((0x4<<24)|SYS_IPRST1_TMR2RST_Pos)      /*!< TMR2 reset is one of the SYS_ResetModule parameter */
#define TMR3_RST    ((0x4<<24)|SYS_IPRST1_TMR3RST_Pos)      /*!< TMR3 reset is one of the SYS_ResetModule parameter */
#define ACMP01_RST  ((0x4<<24)|SYS_IPRST1_ACMP01RST_Pos)    /*!< ACMP reset is one of the SYS_ResetModule parameter */
#define I2C0_RST    ((0x4<<24)|SYS_IPRST1_I2C0RST_Pos)      /*!< I2C0 reset is one of the SYS_ResetModule parameter */
#define I2C1_RST    ((0x4<<24)|SYS_IPRST1_I2C1RST_Pos)      /*!< I2C1 reset is one of the SYS_ResetModule parameter */
#define QSPI0_RST   ((0x4<<24)|SYS_IPRST1_QSPI0RST_Pos)     /*!< QSPI0 reset is one of the SYS_ResetModule parameter */
#define SPI0_RST    ((0x4<<24)|SYS_IPRST1_SPI0RST_Pos)      /*!< SPI0 reset is one of the SYS_ResetModule parameter */
#define UART0_RST   ((0x4<<24)|SYS_IPRST1_UART0RST_Pos)     /*!< UART0 reset is one of the SYS_ResetModule parameter */
#define UART1_RST   ((0x4<<24)|SYS_IPRST1_UART1RST_Pos)     /*!< UART1 reset is one of the SYS_ResetModule parameter */
#define UART2_RST   ((0x4<<24)|SYS_IPRST1_UART2RST_Pos)     /*!< UART2 reset is one of the SYS_ResetModule parameter */
#define USBD_RST    ((0x4<<24)|SYS_IPRST1_USBDRST_Pos)      /*!< USBD reset is one of the SYS_ResetModule parameter */
#define EADC_RST    ((0x4<<24)|SYS_IPRST1_EADCRST_Pos)       /*!< ADC reset is one of the SYS_ResetModule parameter */

#define SC0_RST     ((0x8<<24)|SYS_IPRST2_SC0RST_Pos)       /*!< SC0 reset is one of the SYS_ResetModule parameter */
#define USCI0_RST   ((0x8<<24)|SYS_IPRST2_USCI0RST_Pos)     /*!< USCI0 reset is one of the SYS_ResetModule parameter */
#define USCI1_RST   ((0x8<<24)|SYS_IPRST2_USCI1RST_Pos)     /*!< USCI1 reset is one of the SYS_ResetModule parameter */
#define USCI2_RST   ((0x8<<24)|SYS_IPRST2_USCI2RST_Pos)     /*!< USCI2 reset is one of the SYS_ResetModule parameter */
#define DAC_RST     ((0x8<<24)|SYS_IPRST2_DACRST_Pos)       /*!< DAC reset is one of the SYS_ResetModule parameter */
#define PWM0_RST    ((0x8<<24)|SYS_IPRST2_PWM0RST_Pos)      /*!< PWM0 reset is one of the SYS_ResetModule parameter */
#define PWM1_RST    ((0x8<<24)|SYS_IPRST2_PWM1RST_Pos)      /*!< PWM1 reset is one of the SYS_ResetModule parameter */
#define BPWM0_RST   ((0x8<<24)|SYS_IPRST2_BPWM0RST_Pos)     /*!< BPWM0 reset is one of the SYS_ResetModule parameter */
#define BPWM1_RST   ((0x8<<24)|SYS_IPRST2_BPWM1RST_Pos)     /*!< BPWM0 reset is one of the SYS_ResetModule parameter */
#define OPA_RST     ((0x8<<24)|SYS_IPRST2_OPARST_Pos)       /*!< OPA reset is one of the SYS_ResetModule parameter */
#define PSIO_RST    ((0x8<<24)|SYS_IPRST2_PSIORST_Pos)      /*!< PSIO reset is one of the SYS_ResetModule parameter */

/*---------------------------------------------------------------------------------------------------------*/
/*  Brown Out Detector Threshold Voltage Selection constant definitions.                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define SYS_BODCTL_BOD_RST_EN           (1UL<<SYS_BODCTL_BODRSTEN_Pos)    /*!< Brown-out Reset Enable */
#define SYS_BODCTL_BOD_INTERRUPT_EN     (0UL<<SYS_BODCTL_BODRSTEN_Pos)    /*!< Brown-out Interrupt Enable */
#define SYS_BODCTL_BODVL_4_4V           (0x7UL<<SYS_BODCTL_BODVL_Pos)       /*!< Setting Brown Out Detector Threshold Voltage as 4.4V */
#define SYS_BODCTL_BODVL_3_7V           (0x6UL<<SYS_BODCTL_BODVL_Pos)       /*!< Setting Brown Out Detector Threshold Voltage as 3.7V */
#define SYS_BODCTL_BODVL_3_0V           (0x5UL<<SYS_BODCTL_BODVL_Pos)       /*!< Setting Brown Out Detector Threshold Voltage as 3.0V */
#define SYS_BODCTL_BODVL_2_7V           (0x4UL<<SYS_BODCTL_BODVL_Pos)       /*!< Setting Brown Out Detector Threshold Voltage as 2.7V */
#define SYS_BODCTL_BODVL_2_4V           (0x3UL<<SYS_BODCTL_BODVL_Pos)       /*!< Setting Brown Out Detector Threshold Voltage as 2.4V */
#define SYS_BODCTL_BODVL_2_0V           (0x2UL<<SYS_BODCTL_BODVL_Pos)       /*!< Setting Brown Out Detector Threshold Voltage as 2.0V */
#define SYS_BODCTL_BODVL_1_8V           (0x1UL<<SYS_BODCTL_BODVL_Pos)       /*!< Setting Brown Out Detector Threshold Voltage as 1.8V */

#define SYS_BODCTL_LVRDGSEL_0CLK       (0x0UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time Without de-glitch function. */
#define SYS_BODCTL_LVRDGSEL_4CLK       (0x1UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time is selected 4MIRC clock*/
#define SYS_BODCTL_LVRDGSEL_8CLK       (0x2UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time is selected 8MIRC clock*/
#define SYS_BODCTL_LVRDGSEL_16CLK      (0x3UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time is selected 16MIRC clock*/
#define SYS_BODCTL_LVRDGSEL_32CLK      (0x4UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time is selected 32MIRC clock*/
#define SYS_BODCTL_LVRDGSEL_64CLK      (0x5UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time is selected 64MIRC clock*/
#define SYS_BODCTL_LVRDGSEL_128CLK     (0x6UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time is selected 128MIRC clock*/
#define SYS_BODCTL_LVRDGSEL_256CLK     (0x7UL<<SYS_BODCTL_LVRDGSEL_Pos)        /*!LVR Output De-glitch Time is selected 256MIRC clock*/

#define SYS_BODCTL_BODDGSEL_0CLK       (0x0UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is sampled by RC10K clock. */
#define SYS_BODCTL_BODDGSEL_4CLK       (0x1UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is selected 4HCLK clock*/
#define SYS_BODCTL_BODDGSEL_8CLK       (0x2UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is selected 8HCLK clock*/
#define SYS_BODCTL_BODDGSEL_16CLK      (0x3UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is selected 16HCLK clock*/
#define SYS_BODCTL_BODDGSEL_32CLK      (0x4UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is selected 32HCLK clock*/
#define SYS_BODCTL_BODDGSEL_64CLK      (0x5UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is selected 64HCLK clock*/
#define SYS_BODCTL_BODDGSEL_128CLK     (0x6UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is selected 128HCLK clock*/
#define SYS_BODCTL_BODDGSEL_256CLK     (0x7UL<<SYS_BODCTL_BODDGSEL_Pos)        /*!BOD Output De-glitch Time is selected 256HCLK clock*/

/*---------------------------------------------------------------------------------------------------------*/
/*  PLCTL constant definitions. (Write-Protection Register)                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define SYS_PLCTL_PLSEL_PL0     (0x0UL<<SYS_PLCTL_PLSEL_Pos)   /*!< Set power level to power level 0 */
#define SYS_PLCTL_PLSEL_PL2     (0x2UL<<SYS_PLCTL_PLSEL_Pos)   /*!< Set power level to power level 2 */

/*---------------------------------------------------------------------------------------------------------*/
/*  PLSTS constant definitions. (Write-Protection Register)                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define SYS_PLSTS_CURPL_PL0     (0x0UL<<SYS_PLSTS_CURPL_Pos) /*!< Current Power Level is power level 0 */
#define SYS_PLSTS_CURPL_PL2     (0x2UL<<SYS_PLSTS_CURPL_Pos) /*!< Current Power Level is power level 2 */

/*---------------------------------------------------------------------------------------------------------*/
/*  Multi-Function constant definitions.                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/* How to use below #define?

Example: If user want to set PD.3 as UART0_TXD and PD.2 as UART0_RXD in initial function,
         user can issue following command to achieve it.

         SYS->GPD_MFPL = (SYS->GPD_MFPL & ~SYS_GPD_MFPL_PD2MFP_Msk) | SYS_GPD_MFPL_PD2MFP_UART0_RXD;
         SYS->GPD_MFPL = (SYS->GPD_MFPL & ~SYS_GPD_MFPL_PD3MFP_Msk) | SYS_GPD_MFPL_PD3MFP_UART0_TXD;
*/
/* PA.0 MFP */
#define SYS_GPA_MFPL_PA0MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for GPIO        */
#define SYS_GPA_MFPL_PA0MFP_QSPI0_MOSI0  (0x03UL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for QSPI0_MOSI0  */
#define SYS_GPA_MFPL_PA0MFP_SPI0_MOSI    (0x04UL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for SPI0_MOSI   */
#define SYS_GPA_MFPL_PA0MFP_SC0_CLK      (0x06UL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for SC0_CLK     */
#define SYS_GPA_MFPL_PA0MFP_UART0_RXD    (0x07UL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for UART0_RXD   */
#define SYS_GPA_MFPL_PA0MFP_UART1_nRTS   (0x08UL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for UART1_nRTS  */
#define SYS_GPA_MFPL_PA0MFP_PSIO0_CH7    (0x0aUL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for PSIO0_CH7   */
#define SYS_GPA_MFPL_PA0MFP_USCI2_DAT1   (0x0bUL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for USCI2_DAT1  */
#define SYS_GPA_MFPL_PA0MFP_BPWM0_CH0    (0x0cUL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for BPWM0_CH0   */
#define SYS_GPA_MFPL_PA0MFP_PWM0_CH5     (0x0dUL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for PWM0_CH5    */
#define SYS_GPA_MFPL_PA0MFP_DAC0_ST      (0x0fUL<<SYS_GPA_MFPL_PA0MFP_Pos) /*!< GPA_MFPL PA0 setting for DAC0_ST     */

/* PA.1 MFP */
#define SYS_GPA_MFPL_PA1MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for GPIO        */
#define SYS_GPA_MFPL_PA1MFP_QSPI0_MISO0  (0x03UL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for QSPI0_MISO0  */
#define SYS_GPA_MFPL_PA1MFP_SPI0_MISO    (0x04UL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for SPI0_MISO   */
#define SYS_GPA_MFPL_PA1MFP_SC0_DAT      (0x06UL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for SC0_DAT     */
#define SYS_GPA_MFPL_PA1MFP_UART0_TXD    (0x07UL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for UART0_TXD   */
#define SYS_GPA_MFPL_PA1MFP_UART1_nCTS   (0x08UL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for UART1_nCTS  */
#define SYS_GPA_MFPL_PA1MFP_PSIO0_CH6    (0x0aUL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for PSIO0_CH6   */
#define SYS_GPA_MFPL_PA1MFP_USCI2_DAT0   (0x0bUL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for USCI2_DAT0  */
#define SYS_GPA_MFPL_PA1MFP_BPWM0_CH1    (0x0cUL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for BPWM0_CH1   */
#define SYS_GPA_MFPL_PA1MFP_PWM0_CH4     (0x0dUL<<SYS_GPA_MFPL_PA1MFP_Pos) /*!< GPA_MFPL PA1 setting for PWM0_CH4    */

/* PA.2 MFP */
#define SYS_GPA_MFPL_PA2MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for GPIO        */
#define SYS_GPA_MFPL_PA2MFP_QSPI0_CLK    (0x03UL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for QSPI0_CLK    */
#define SYS_GPA_MFPL_PA2MFP_SPI0_CLK     (0x04UL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for SPI0_CLK    */
#define SYS_GPA_MFPL_PA2MFP_SC0_RST      (0x06UL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for SC0_RST     */
#define SYS_GPA_MFPL_PA2MFP_I2C0_SMBSUS  (0x07UL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for I2C0_SMBSUS */
#define SYS_GPA_MFPL_PA2MFP_UART1_RXD    (0x08UL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for UART1_RXD   */
#define SYS_GPA_MFPL_PA2MFP_I2C1_SDA     (0x09UL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for I2C1_SDA    */
#define SYS_GPA_MFPL_PA2MFP_PSIO0_CH5    (0x0aUL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for PSIO0_CH5   */
#define SYS_GPA_MFPL_PA2MFP_USCI2_CLK    (0x0bUL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for USCI2_CLK   */
#define SYS_GPA_MFPL_PA2MFP_BPWM0_CH2    (0x0cUL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for BPWM0_CH2   */
#define SYS_GPA_MFPL_PA2MFP_PWM0_CH3     (0x0dUL<<SYS_GPA_MFPL_PA2MFP_Pos) /*!< GPA_MFPL PA2 setting for PWM0_CH3    */

/* PA.3 MFP */
#define SYS_GPA_MFPL_PA3MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for GPIO        */
#define SYS_GPA_MFPL_PA3MFP_QSPI0_SS     (0x03UL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for QSPI0_SS     */
#define SYS_GPA_MFPL_PA3MFP_SPI0_SS      (0x04UL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for SPI0_SS     */
#define SYS_GPA_MFPL_PA3MFP_SC0_PWR      (0x06UL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for SC0_PWR     */
#define SYS_GPA_MFPL_PA3MFP_I2C0_SMBAL   (0x07UL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for I2C0_SMBAL  */
#define SYS_GPA_MFPL_PA3MFP_UART1_TXD    (0x08UL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for UART1_TXD   */
#define SYS_GPA_MFPL_PA3MFP_I2C1_SCL     (0x09UL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for I2C1_SCL    */
#define SYS_GPA_MFPL_PA3MFP_PSIO0_CH4    (0x0aUL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for PSIO0_CH4   */
#define SYS_GPA_MFPL_PA3MFP_USCI2_CTL0   (0x0bUL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for USCI2_CTL0  */
#define SYS_GPA_MFPL_PA3MFP_BPWM0_CH3    (0x0cUL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for BPWM0_CH3   */
#define SYS_GPA_MFPL_PA3MFP_PWM0_CH2     (0x0dUL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for PWM0_CH2    */
#define SYS_GPA_MFPL_PA3MFP_PWM1_BRAKE1  (0x0fUL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for PWM1_BRAKE1    */
#define SYS_GPA_MFPL_PA3MFP_CLKO         (0x0eUL<<SYS_GPA_MFPL_PA3MFP_Pos) /*!< GPA_MFPL PA3 setting for CLKO        */

/* PA.4 MFP */
#define SYS_GPA_MFPL_PA4MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for GPIO        */
#define SYS_GPA_MFPL_PA4MFP_QSPI0_MOSI1  (0x03UL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for QSPI0_MOSI1  */
#define SYS_GPA_MFPL_PA4MFP_SPI0_I2SMCLK (0x04UL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for SPI0_I2SMCLK*/
#define SYS_GPA_MFPL_PA4MFP_SC0_nCD      (0x06UL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for SC0_nCD     */
#define SYS_GPA_MFPL_PA4MFP_UART0_nRTS   (0x07UL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for UART0_nRTS  */
#define SYS_GPA_MFPL_PA4MFP_UART0_RXD    (0x08UL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for UART0_RXD   */
#define SYS_GPA_MFPL_PA4MFP_I2C0_SDA     (0x09UL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for I2C0_SDA    */
#define SYS_GPA_MFPL_PA4MFP_USCI2_CTL1   (0x0bUL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for USCI2_CTL1  */
#define SYS_GPA_MFPL_PA4MFP_BPWM0_CH4    (0x0cUL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for BPWM0_CH4   */
#define SYS_GPA_MFPL_PA4MFP_PWM0_CH1     (0x0dUL<<SYS_GPA_MFPL_PA4MFP_Pos) /*!< GPA_MFPL PA4 setting for PWM0_CH1    */

/* PA.5 MFP */
#define SYS_GPA_MFPL_PA5MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA5MFP_Pos) /*!< GPA_MFPL PA5 setting for GPIO        */
#define SYS_GPA_MFPL_PA5MFP_QSPI0_MISO1  (0x03UL<<SYS_GPA_MFPL_PA5MFP_Pos) /*!< GPA_MFPL PA5 setting for QSPI0_MISO1  */
#define SYS_GPA_MFPL_PA5MFP_UART0_nCTS   (0x07UL<<SYS_GPA_MFPL_PA5MFP_Pos) /*!< GPA_MFPL PA5 setting for UART0_nCTS  */
#define SYS_GPA_MFPL_PA5MFP_UART0_TXD    (0x08UL<<SYS_GPA_MFPL_PA5MFP_Pos) /*!< GPA_MFPL PA5 setting for UART0_TXD   */
#define SYS_GPA_MFPL_PA5MFP_I2C0_SCL     (0x09UL<<SYS_GPA_MFPL_PA5MFP_Pos) /*!< GPA_MFPL PA5 setting for I2C0_SCL    */
#define SYS_GPA_MFPL_PA5MFP_BPWM0_CH5    (0x0cUL<<SYS_GPA_MFPL_PA5MFP_Pos) /*!< GPA_MFPL PA5 setting for BPWM0_CH5   */
#define SYS_GPA_MFPL_PA5MFP_PWM0_CH0     (0x0dUL<<SYS_GPA_MFPL_PA5MFP_Pos) /*!< GPA_MFPL PA5 setting for PWM0_CH0    */

/* PA.6 MFP */
#define SYS_GPA_MFPL_PA6MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for GPIO        */
#define SYS_GPA_MFPL_PA6MFP_EBI_AD6      (0x02UL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for EBI_AD6     */
#define SYS_GPA_MFPL_PA6MFP_UART0_RXD    (0x07UL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for UART0_RXD   */
#define SYS_GPA_MFPL_PA6MFP_I2C1_SDA     (0x08UL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for I2C1_SDA    */
#define SYS_GPA_MFPL_PA6MFP_PWM1_CH5     (0x0bUL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for PWM1_CH5    */
#define SYS_GPA_MFPL_PA6MFP_BPWM1_CH3    (0x0cUL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for BPWM1_CH3   */
#define SYS_GPA_MFPL_PA6MFP_ACMP1_WLAT   (0x0dUL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for ACMP1_WLAT  */
#define SYS_GPA_MFPL_PA6MFP_TM3          (0x0eUL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for TM3         */
#define SYS_GPA_MFPL_PA6MFP_INT0         (0x0fUL<<SYS_GPA_MFPL_PA6MFP_Pos) /*!< GPA_MFPL PA6 setting for INT0        */

/* PA.7 MFP */
#define SYS_GPA_MFPL_PA7MFP_GPIO         (0x00UL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for GPIO        */
#define SYS_GPA_MFPL_PA7MFP_EBI_AD7      (0x02UL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for EBI_AD7     */
#define SYS_GPA_MFPL_PA7MFP_UART0_TXD    (0x07UL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for UART0_TXD   */
#define SYS_GPA_MFPL_PA7MFP_I2C1_SCL     (0x08UL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for I2C1_SCL    */
#define SYS_GPA_MFPL_PA7MFP_PWM1_CH4     (0x0bUL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for PWM1_CH4    */
#define SYS_GPA_MFPL_PA7MFP_BPWM1_CH2    (0x0cUL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for BPWM1_CH2   */
#define SYS_GPA_MFPL_PA7MFP_ACMP0_WLAT   (0x0dUL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for ACMP0_WLAT  */
#define SYS_GPA_MFPL_PA7MFP_TM2          (0x0eUL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for TM2         */
#define SYS_GPA_MFPL_PA7MFP_INT1         (0x0fUL<<SYS_GPA_MFPL_PA7MFP_Pos) /*!< GPA_MFPL PA7 setting for INT1        */

/* PA.8 MFP */
#define SYS_GPA_MFPH_PA8MFP_GPIO         (0x00UL<<SYS_GPA_MFPH_PA8MFP_Pos) /*!< GPA_MFPH PA8 setting for GPIO        */
#define SYS_GPA_MFPH_PA8MFP_EBI_ALE      (0x02UL<<SYS_GPA_MFPH_PA8MFP_Pos) /*!< GPA_MFPH PA8 setting for EBI_ALE     */
#define SYS_GPA_MFPH_PA8MFP_USCI0_CTL1   (0x06UL<<SYS_GPA_MFPH_PA8MFP_Pos) /*!< GPA_MFPH PA8 setting for USCI0_CTL1  */
#define SYS_GPA_MFPH_PA8MFP_UART1_RXD    (0x07UL<<SYS_GPA_MFPH_PA8MFP_Pos) /*!< GPA_MFPH PA8 setting for UART1_RXD   */
#define SYS_GPA_MFPH_PA8MFP_BPWM0_CH3    (0x09UL<<SYS_GPA_MFPH_PA8MFP_Pos) /*!< GPA_MFPH PA8 setting for BPWM0_CH3   */
#define SYS_GPA_MFPH_PA8MFP_TM3_EXT      (0x0dUL<<SYS_GPA_MFPH_PA8MFP_Pos) /*!< GPA_MFPH PA8 setting for TM3_EXT     */
#define SYS_GPA_MFPH_PA8MFP_INT4         (0x0fUL<<SYS_GPA_MFPH_PA8MFP_Pos) /*!< GPA_MFPH PA8 setting for INT4        */

/* PA.9 MFP */
#define SYS_GPA_MFPH_PA9MFP_GPIO         (0x00UL<<SYS_GPA_MFPH_PA9MFP_Pos) /*!< GPA_MFPH PA9 setting for GPIO        */
#define SYS_GPA_MFPH_PA9MFP_EBI_MCLK     (0x02UL<<SYS_GPA_MFPH_PA9MFP_Pos) /*!< GPA_MFPH PA9 setting for EBI_MCLK    */
#define SYS_GPA_MFPH_PA9MFP_USCI0_DAT1   (0x06UL<<SYS_GPA_MFPH_PA9MFP_Pos) /*!< GPA_MFPH PA9 setting for USCI0_DAT1  */
#define SYS_GPA_MFPH_PA9MFP_UART1_TXD    (0x07UL<<SYS_GPA_MFPH_PA9MFP_Pos) /*!< GPA_MFPH PA9 setting for UART1_TXD   */
#define SYS_GPA_MFPH_PA9MFP_BPWM0_CH2    (0x09UL<<SYS_GPA_MFPH_PA9MFP_Pos) /*!< GPA_MFPH PA9 setting for BPWM0_CH2   */
#define SYS_GPA_MFPH_PA9MFP_TM2_EXT      (0x0dUL<<SYS_GPA_MFPH_PA9MFP_Pos) /*!< GPA_MFPH PA9 setting for TM2_EXT     */
#define SYS_GPA_MFPH_PA9MFP_SWDH_DAT     (0x0fUL<<SYS_GPA_MFPH_PA9MFP_Pos) /*!< GPA_MFPH PA9 setting for SWDH_DAT    */

/* PA.10 MFP */
#define SYS_GPA_MFPH_PA10MFP_GPIO        (0x00UL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for GPIO       */
#define SYS_GPA_MFPH_PA10MFP_ACMP1_P0    (0x01UL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for ACMP1_P0   */
#define SYS_GPA_MFPH_PA10MFP_EBI_nWR     (0x02UL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for EBI_nWR    */
#define SYS_GPA_MFPH_PA10MFP_USCI0_DAT0  (0x06UL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for USCI0_DAT0 */
#define SYS_GPA_MFPH_PA10MFP_BPWM0_CH1   (0x09UL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for BPWM0_CH1  */
#define SYS_GPA_MFPH_PA10MFP_TM1_EXT     (0x0dUL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for TM1_EXT    */
#define SYS_GPA_MFPH_PA10MFP_DAC0_ST     (0x0eUL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for DAC0_ST    */
#define SYS_GPA_MFPH_PA10MFP_SWDH_CLK    (0x0fUL<<SYS_GPA_MFPH_PA10MFP_Pos)/*!< GPA_MFPH PA10 setting for SWDH_CLK   */

/* PA.11 MFP */
#define SYS_GPA_MFPH_PA11MFP_GPIO        (0x00UL<<SYS_GPA_MFPH_PA11MFP_Pos)/*!< GPA_MFPH PA11 setting for GPIO       */
#define SYS_GPA_MFPH_PA11MFP_ACMP0_P0    (0x01UL<<SYS_GPA_MFPH_PA11MFP_Pos)/*!< GPA_MFPH PA11 setting for ACMP0_P0   */
#define SYS_GPA_MFPH_PA11MFP_EBI_nRD     (0x02UL<<SYS_GPA_MFPH_PA11MFP_Pos)/*!< GPA_MFPH PA11 setting for EBI_nRD    */
#define SYS_GPA_MFPH_PA11MFP_USCI0_CLK   (0x06UL<<SYS_GPA_MFPH_PA11MFP_Pos)/*!< GPA_MFPH PA11 setting for USCI0_CLK  */
#define SYS_GPA_MFPH_PA11MFP_BPWM0_CH0   (0x09UL<<SYS_GPA_MFPH_PA11MFP_Pos)/*!< GPA_MFPH PA11 setting for BPWM0_CH0  */
#define SYS_GPA_MFPH_PA11MFP_TM0_EXT     (0x0dUL<<SYS_GPA_MFPH_PA11MFP_Pos)/*!< GPA_MFPH PA11 setting for TM0_EXT    */

/* PA.12 MFP */
#define SYS_GPA_MFPH_PA12MFP_GPIO        (0x00UL<<SYS_GPA_MFPH_PA12MFP_Pos)/*!< GPA_MFPH PA12 setting for GPIO       */
#define SYS_GPA_MFPH_PA12MFP_I2C1_SCL    (0x04UL<<SYS_GPA_MFPH_PA12MFP_Pos)/*!< GPA_MFPH PA12 setting for I2C1_SCL   */
#define SYS_GPA_MFPH_PA12MFP_USCI2_DAT1  (0x08UL<<SYS_GPA_MFPH_PA12MFP_Pos)/*!< GPA_MFPH PA12 setting for USCI2_DAT1 */
#define SYS_GPA_MFPH_PA12MFP_PSIO0_CH4   (0x0aUL<<SYS_GPA_MFPH_PA12MFP_Pos)/*!< GPA_MFPH PA12 setting for PSIO0_CH4  */
#define SYS_GPA_MFPH_PA12MFP_BPWM1_CH2   (0x0bUL<<SYS_GPA_MFPH_PA12MFP_Pos)/*!< GPA_MFPH PA12 setting for BPWM1_CH2  */

/* PA.13 MFP */
#define SYS_GPA_MFPH_PA13MFP_GPIO        (0x00UL<<SYS_GPA_MFPH_PA13MFP_Pos)/*!< GPA_MFPH PA13 setting for GPIO       */
#define SYS_GPA_MFPH_PA13MFP_I2C1_SDA    (0x04UL<<SYS_GPA_MFPH_PA13MFP_Pos)/*!< GPA_MFPH PA13 setting for I2C1_SDA   */
#define SYS_GPA_MFPH_PA13MFP_USCI2_DAT0  (0x08UL<<SYS_GPA_MFPH_PA13MFP_Pos)/*!< GPA_MFPH PA13 setting for USCI2_DAT0 */
#define SYS_GPA_MFPH_PA13MFP_PSIO0_CH5   (0x0aUL<<SYS_GPA_MFPH_PA13MFP_Pos)/*!< GPA_MFPH PA13 setting for PSIO0_CH5  */
#define SYS_GPA_MFPH_PA13MFP_BPWM1_CH3   (0x0bUL<<SYS_GPA_MFPH_PA13MFP_Pos)/*!< GPA_MFPH PA13 setting for BPWM1_CH3  */

/* PA.14 MFP */
#define SYS_GPA_MFPH_PA14MFP_GPIO        (0x00UL<<SYS_GPA_MFPH_PA14MFP_Pos)/*!< GPA_MFPH PA14 setting for GPIO       */
#define SYS_GPA_MFPH_PA14MFP_UART0_TXD   (0x03UL<<SYS_GPA_MFPH_PA14MFP_Pos)/*!< GPA_MFPH PA14 setting for UART0_TXD  */
#define SYS_GPA_MFPH_PA14MFP_USCI2_CLK   (0x08UL<<SYS_GPA_MFPH_PA14MFP_Pos)/*!< GPA_MFPH PA14 setting for USCI2_CLK  */
#define SYS_GPA_MFPH_PA14MFP_PSIO0_CH6   (0x0aUL<<SYS_GPA_MFPH_PA14MFP_Pos)/*!< GPA_MFPH PA14 setting for PSIO0_CH6  */
#define SYS_GPA_MFPH_PA14MFP_BPWM1_CH4   (0x0bUL<<SYS_GPA_MFPH_PA14MFP_Pos)/*!< GPA_MFPH PA14 setting for BPWM1_CH4  */

/* PA.15 MFP */
#define SYS_GPA_MFPH_PA15MFP_GPIO        (0x00UL<<SYS_GPA_MFPH_PA15MFP_Pos)/*!< GPA_MFPH PA15 setting for GPIO       */
#define SYS_GPA_MFPH_PA15MFP_UART0_RXD   (0x03UL<<SYS_GPA_MFPH_PA15MFP_Pos)/*!< GPA_MFPH PA15 setting for UART0_RXD  */
#define SYS_GPA_MFPH_PA15MFP_USCI2_CTL1  (0x08UL<<SYS_GPA_MFPH_PA15MFP_Pos)/*!< GPA_MFPH PA15 setting for USCI2_CTL1 */
#define SYS_GPA_MFPH_PA15MFP_PSIO0_CH7   (0x0aUL<<SYS_GPA_MFPH_PA15MFP_Pos)/*!< GPA_MFPH PA15 setting for PSIO0_CH7  */
#define SYS_GPA_MFPH_PA15MFP_BPWM1_CH5   (0x0bUL<<SYS_GPA_MFPH_PA15MFP_Pos)/*!< GPA_MFPH PA15 setting for BPWM1_CH5  */

/* PB.0 MFP */
#define SYS_GPB_MFPL_PB0MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for GPIO        */
#define SYS_GPB_MFPL_PB0MFP_EADC0_CH0    (0x01UL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for EADC0_CH0   */
#define SYS_GPB_MFPL_PB0MFP_OPA0_P       (0x01UL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for OPA0_P      */
#define SYS_GPB_MFPL_PB0MFP_EBI_ADR9     (0x02UL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for EBI_ADR9    */
#define SYS_GPB_MFPL_PB0MFP_UART2_RXD    (0x07UL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for UART2_RXD   */
#define SYS_GPB_MFPL_PB0MFP_SPI0_I2SMCLK (0x08UL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for SPI0_I2SMCLK*/
#define SYS_GPB_MFPL_PB0MFP_I2C1_SDA     (0x09UL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for I2C1_SDA    */
#define SYS_GPB_MFPL_PB0MFP_QSPI0_MOSI1  (0x0aUL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for QSPI0_MOSI1  */
#define SYS_GPB_MFPL_PB0MFP_PWM0_CH5     (0x0bUL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for PWM0_CH5    */
#define SYS_GPB_MFPL_PB0MFP_PWM1_CH5     (0x0cUL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for PWM1_CH5    */
#define SYS_GPB_MFPL_PB0MFP_PWM0_BRAKE1  (0x0dUL<<SYS_GPB_MFPL_PB0MFP_Pos) /*!< GPB_MFPL PB0 setting for PWM0_BRAKE1 */

/* PB.1 MFP */
#define SYS_GPB_MFPL_PB1MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for GPIO        */
#define SYS_GPB_MFPL_PB1MFP_EADC0_CH1    (0x01UL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for EADC0_CH1   */
#define SYS_GPB_MFPL_PB1MFP_OPA0_N       (0x01UL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for OPA0_N      */
#define SYS_GPB_MFPL_PB1MFP_EBI_ADR8     (0x02UL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for EBI_ADR8    */
#define SYS_GPB_MFPL_PB1MFP_UART2_TXD    (0x07UL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for UART2_TXD   */
#define SYS_GPB_MFPL_PB1MFP_USCI1_CLK    (0x08UL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for USCI1_CLK   */
#define SYS_GPB_MFPL_PB1MFP_I2C1_SCL     (0x09UL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for I2C1_SCL    */
#define SYS_GPB_MFPL_PB1MFP_QSPI0_MISO1  (0x0aUL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for QSPI0_MISO1  */
#define SYS_GPB_MFPL_PB1MFP_PWM0_CH4     (0x0bUL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for PWM0_CH4    */
#define SYS_GPB_MFPL_PB1MFP_PWM1_CH4     (0x0cUL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for PWM1_CH4    */
#define SYS_GPB_MFPL_PB1MFP_PWM0_BRAKE0  (0x0dUL<<SYS_GPB_MFPL_PB1MFP_Pos) /*!< GPB_MFPL PB1 setting for PWM0_BRAKE0 */

/* PB.2 MFP */
#define SYS_GPB_MFPL_PB2MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for GPIO        */
#define SYS_GPB_MFPL_PB2MFP_EADC0_CH2    (0x01UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for EADC0_CH2   */
#define SYS_GPB_MFPL_PB2MFP_ACMP0_P1     (0x01UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for ACMP0_P1    */
#define SYS_GPB_MFPL_PB2MFP_OPA0_O       (0x01UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for OPA0_O      */
#define SYS_GPB_MFPL_PB2MFP_EBI_ADR3     (0x02UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for EBI_ADR3    */
#define SYS_GPB_MFPL_PB2MFP_UART1_RXD    (0x06UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for UART1_RXD   */
#define SYS_GPB_MFPL_PB2MFP_USCI1_DAT0   (0x08UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for USCI1_DAT0  */
#define SYS_GPB_MFPL_PB2MFP_SC0_PWR      (0x09UL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for SC0_PWR     */
#define SYS_GPB_MFPL_PB2MFP_PWM0_CH3     (0x0bUL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for PWM0_CH3    */
#define SYS_GPB_MFPL_PB2MFP_PSIO0_CH7    (0x0cUL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for PSIO0_CH7   */
#define SYS_GPB_MFPL_PB2MFP_TM3          (0x0eUL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for TM3         */
#define SYS_GPB_MFPL_PB2MFP_INT3         (0x0fUL<<SYS_GPB_MFPL_PB2MFP_Pos) /*!< GPB_MFPL PB2 setting for INT3        */
#define SYS_GPB_MFPL_PB2MFP_I2C1_SDA     (0x04UL<<SYS_GPB_MFPL_PB2MFP_Pos)/*!< GPB_MFPL PB2 setting for I2C1_SDA   */

/* PB.3 MFP */
#define SYS_GPB_MFPL_PB3MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for GPIO        */
#define SYS_GPB_MFPL_PB3MFP_EADC0_CH3    (0x01UL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for EADC0_CH3   */
#define SYS_GPB_MFPL_PB3MFP_ACMP0_N      (0x01UL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for ACMP0_N     */
#define SYS_GPB_MFPL_PB3MFP_EBI_ADR2     (0x02UL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for EBI_ADR2    */
#define SYS_GPB_MFPL_PB3MFP_UART1_TXD    (0x06UL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for UART1_TXD   */
#define SYS_GPB_MFPL_PB3MFP_USCI1_DAT1   (0x08UL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for USCI1_DAT1  */
#define SYS_GPB_MFPL_PB3MFP_SC0_RST      (0x09UL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for SC0_RST     */
#define SYS_GPB_MFPL_PB3MFP_PWM0_CH2     (0x0bUL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for PWM0_CH2    */
#define SYS_GPB_MFPL_PB3MFP_PSIO0_CH6    (0x0cUL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for PSIO0_CH6   */
#define SYS_GPB_MFPL_PB3MFP_TM2          (0x0eUL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for TM2         */
#define SYS_GPB_MFPL_PB3MFP_INT2         (0x0fUL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB3 setting for INT2        */
#define SYS_GPB_MFPL_PB3MFP_I2C1_SCL     (0x04UL<<SYS_GPB_MFPL_PB3MFP_Pos)/*!< GPB_MFPL PB2 setting for I2C1_SCL   */
#define SYS_GPB_MFPL_PB3MFP_PWM0_BRAKE0  (0x0dUL<<SYS_GPB_MFPL_PB3MFP_Pos) /*!< GPB_MFPL PB2 setting for PWM0_BRAKE0    */

/* PB.4 MFP */
#define SYS_GPB_MFPL_PB4MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for GPIO        */
#define SYS_GPB_MFPL_PB4MFP_EADC0_CH4    (0x01UL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for EADC0_CH4   */
#define SYS_GPB_MFPL_PB4MFP_ACMP1_P1     (0x01UL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for ACMP1_P1    */
#define SYS_GPB_MFPL_PB4MFP_EBI_ADR1     (0x02UL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for EBI_ADR1    */
#define SYS_GPB_MFPL_PB4MFP_I2C0_SDA     (0x06UL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for I2C0_SDA    */
#define SYS_GPB_MFPL_PB4MFP_USCI1_CTL1   (0x08UL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for USCI1_CTL1  */
#define SYS_GPB_MFPL_PB4MFP_SC0_DAT      (0x09UL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for SC0_DAT     */
#define SYS_GPB_MFPL_PB4MFP_PWM0_CH1     (0x0bUL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for PWM0_CH1    */
#define SYS_GPB_MFPL_PB4MFP_PSIO0_CH5    (0x0cUL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for PSIO0_CH5   */
#define SYS_GPB_MFPL_PB4MFP_TM1          (0x0eUL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for TM1         */
#define SYS_GPB_MFPL_PB4MFP_INT1         (0x0fUL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for INT1        */
#define SYS_GPB_MFPL_PB4MFP_UART2_RXD    (0x0dUL<<SYS_GPB_MFPL_PB4MFP_Pos) /*!< GPB_MFPL PB4 setting for UART2_RXD   */

/* PB.5 MFP */
#define SYS_GPB_MFPL_PB5MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for GPIO        */
#define SYS_GPB_MFPL_PB5MFP_EADC0_CH5    (0x01UL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for EADC0_CH5   */
#define SYS_GPB_MFPL_PB5MFP_ACMP1_N      (0x01UL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for ACMP1_N     */
#define SYS_GPB_MFPL_PB5MFP_EBI_ADR0     (0x02UL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for EBI_ADR0    */
#define SYS_GPB_MFPL_PB5MFP_I2C0_SCL     (0x06UL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for I2C0_SCL    */
#define SYS_GPB_MFPL_PB5MFP_USCI1_CTL0   (0x08UL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for USCI1_CTL0  */
#define SYS_GPB_MFPL_PB5MFP_SC0_CLK      (0x09UL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for SC0_CLK     */
#define SYS_GPB_MFPL_PB5MFP_PWM0_CH0     (0x0bUL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for PWM0_CH0    */
#define SYS_GPB_MFPL_PB5MFP_PSIO0_CH4    (0x0cUL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for PSIO0_CH4   */
#define SYS_GPB_MFPL_PB5MFP_TM0          (0x0eUL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for TM0         */
#define SYS_GPB_MFPL_PB5MFP_INT0         (0x0fUL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for INT0        */
#define SYS_GPB_MFPL_PB5MFP_UART2_TXD    (0x0dUL<<SYS_GPB_MFPL_PB5MFP_Pos) /*!< GPB_MFPL PB5 setting for UART2_TXD   */

/* PB.6 MFP */
#define SYS_GPB_MFPL_PB6MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for GPIO        */
#define SYS_GPB_MFPL_PB6MFP_EADC0_CH6    (0x01UL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for EADC0_CH6   */
#define SYS_GPB_MFPL_PB6MFP_EBI_nWRH     (0x02UL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for EBI_nWRH    */
#define SYS_GPB_MFPL_PB6MFP_USCI1_DAT1   (0x04UL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for USCI1_DAT1  */
#define SYS_GPB_MFPL_PB6MFP_UART1_RXD    (0x06UL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for UART1_RXD   */
#define SYS_GPB_MFPL_PB6MFP_EBI_nCS1     (0x08UL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for EBI_nCS1    */
#define SYS_GPB_MFPL_PB6MFP_BPWM1_CH5    (0x0aUL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for BPWM1_CH5   */
#define SYS_GPB_MFPL_PB6MFP_PWM1_BRAKE1  (0x0bUL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for PWM1_BRAKE1 */
#define SYS_GPB_MFPL_PB6MFP_PWM1_CH5     (0x0cUL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for PWM1_CH5    */
#define SYS_GPB_MFPL_PB6MFP_INT4         (0x0dUL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for INT4        */
#define SYS_GPB_MFPL_PB6MFP_ACMP1_O      (0x0fUL<<SYS_GPB_MFPL_PB6MFP_Pos) /*!< GPB_MFPL PB6 setting for ACMP1_O     */

/* PB.7 MFP */
#define SYS_GPB_MFPL_PB7MFP_GPIO         (0x00UL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for GPIO        */
#define SYS_GPB_MFPL_PB7MFP_EADC0_CH7    (0x01UL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for EADC0_CH7   */
#define SYS_GPB_MFPL_PB7MFP_EBI_nWRL     (0x02UL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for EBI_nWRL    */
#define SYS_GPB_MFPL_PB7MFP_USCI1_DAT0   (0x04UL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for USCI1_DAT0  */
#define SYS_GPB_MFPL_PB7MFP_UART1_TXD    (0x06UL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for UART1_TXD   */
#define SYS_GPB_MFPL_PB7MFP_EBI_nCS0     (0x08UL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for EBI_nCS0    */
#define SYS_GPB_MFPL_PB7MFP_BPWM1_CH4    (0x0aUL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for BPWM1_CH4   */
#define SYS_GPB_MFPL_PB7MFP_PWM1_BRAKE0  (0x0bUL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for PWM1_BRAKE0 */
#define SYS_GPB_MFPL_PB7MFP_PWM1_CH4     (0x0cUL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for PWM1_CH4    */
#define SYS_GPB_MFPL_PB7MFP_INT5         (0x0dUL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for INT5        */
#define SYS_GPB_MFPL_PB7MFP_ACMP0_O      (0x0fUL<<SYS_GPB_MFPL_PB7MFP_Pos) /*!< GPB_MFPL PB7 setting for ACMP0_O     */

/* PB.8 MFP */
#define SYS_GPB_MFPH_PB8MFP_GPIO         (0x00UL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for GPIO        */
#define SYS_GPB_MFPH_PB8MFP_EADC0_CH8    (0x01UL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for EADC0_CH8   */
#define SYS_GPB_MFPH_PB8MFP_EBI_ADR19    (0x02UL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for EBI_ADR19   */
#define SYS_GPB_MFPH_PB8MFP_USCI1_CLK    (0x04UL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for USCI1_CLK   */
#define SYS_GPB_MFPH_PB8MFP_UART0_RXD    (0x05UL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for UART0_RXD   */
#define SYS_GPB_MFPH_PB8MFP_UART1_nRTS   (0x06UL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for UART1_nRTS  */
#define SYS_GPB_MFPH_PB8MFP_I2C1_SMBSUS  (0x07UL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for I2C1_SMBSUS */
#define SYS_GPB_MFPH_PB8MFP_BPWM1_CH3    (0x0aUL<<SYS_GPB_MFPH_PB8MFP_Pos) /*!< GPB_MFPH PB8 setting for BPWM1_CH3   */

/* PB.9 MFP */
#define SYS_GPB_MFPH_PB9MFP_GPIO         (0x00UL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for GPIO        */
#define SYS_GPB_MFPH_PB9MFP_EADC0_CH9    (0x01UL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for EADC0_CH9   */
#define SYS_GPB_MFPH_PB9MFP_EBI_ADR18    (0x02UL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for EBI_ADR18   */
#define SYS_GPB_MFPH_PB9MFP_USCI1_CTL1   (0x04UL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for USCI1_CTL1  */
#define SYS_GPB_MFPH_PB9MFP_UART0_TXD    (0x05UL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for UART0_TXD   */
#define SYS_GPB_MFPH_PB9MFP_UART1_nCTS   (0x06UL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for UART1_nCTS  */
#define SYS_GPB_MFPH_PB9MFP_I2C1_SMBAL   (0x07UL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for I2C1_SMBAL  */
#define SYS_GPB_MFPH_PB9MFP_BPWM1_CH2    (0x0aUL<<SYS_GPB_MFPH_PB9MFP_Pos) /*!< GPB_MFPH PB9 setting for BPWM1_CH2   */

/* PB.10 MFP */
#define SYS_GPB_MFPH_PB10MFP_GPIO        (0x00UL<<SYS_GPB_MFPH_PB10MFP_Pos)/*!< GPB_MFPH PB10 setting for GPIO       */
#define SYS_GPB_MFPH_PB10MFP_EADC0_CH10  (0x01UL<<SYS_GPB_MFPH_PB10MFP_Pos)/*!< GPB_MFPH PB10 setting for EADC0_CH10 */
#define SYS_GPB_MFPH_PB10MFP_EBI_ADR17   (0x02UL<<SYS_GPB_MFPH_PB10MFP_Pos)/*!< GPB_MFPH PB10 setting for EBI_ADR17  */
#define SYS_GPB_MFPH_PB10MFP_USCI1_CTL0  (0x04UL<<SYS_GPB_MFPH_PB10MFP_Pos)/*!< GPB_MFPH PB10 setting for USCI1_CTL0 */
#define SYS_GPB_MFPH_PB10MFP_UART0_nRTS  (0x05UL<<SYS_GPB_MFPH_PB10MFP_Pos)/*!< GPB_MFPH PB10 setting for UART0_nRTS */
#define SYS_GPB_MFPH_PB10MFP_I2C1_SDA    (0x07UL<<SYS_GPB_MFPH_PB10MFP_Pos)/*!< GPB_MFPH PB10 setting for I2C1_SDA   */
#define SYS_GPB_MFPH_PB10MFP_BPWM1_CH1   (0x0aUL<<SYS_GPB_MFPH_PB10MFP_Pos)/*!< GPB_MFPH PB10 setting for BPWM1_CH1  */

/* PB.11 MFP */
#define SYS_GPB_MFPH_PB11MFP_GPIO        (0x00UL<<SYS_GPB_MFPH_PB11MFP_Pos)/*!< GPB_MFPH PB11 setting for GPIO       */
#define SYS_GPB_MFPH_PB11MFP_EADC0_CH11  (0x01UL<<SYS_GPB_MFPH_PB11MFP_Pos)/*!< GPB_MFPH PB11 setting for EADC0_CH11 */
#define SYS_GPB_MFPH_PB11MFP_EBI_ADR16   (0x02UL<<SYS_GPB_MFPH_PB11MFP_Pos)/*!< GPB_MFPH PB11 setting for EBI_ADR16  */
#define SYS_GPB_MFPH_PB11MFP_UART0_nCTS  (0x05UL<<SYS_GPB_MFPH_PB11MFP_Pos)/*!< GPB_MFPH PB11 setting for UART0_nCTS */
#define SYS_GPB_MFPH_PB11MFP_I2C1_SCL    (0x07UL<<SYS_GPB_MFPH_PB11MFP_Pos)/*!< GPB_MFPH PB11 setting for I2C1_SCL   */
#define SYS_GPB_MFPH_PB11MFP_SPI0_I2SMCLK (0x09UL<<SYS_GPB_MFPH_PB11MFP_Pos)/*!< GPB_MFPH PB11 setting for SPI0_I2SMCLK*/
#define SYS_GPB_MFPH_PB11MFP_BPWM1_CH0   (0x0aUL<<SYS_GPB_MFPH_PB11MFP_Pos)/*!< GPB_MFPH PB11 setting for BPWM1_CH0  */

/* PB.12 MFP */
#define SYS_GPB_MFPH_PB12MFP_GPIO        (0x00UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for GPIO       */
#define SYS_GPB_MFPH_PB12MFP_EADC0_CH12  (0x01UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for EADC0_CH12 */
#define SYS_GPB_MFPH_PB12MFP_DAC0_OUT    (0x01UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for DAC0_OUT   */
#define SYS_GPB_MFPH_PB12MFP_ACMP0_P2    (0x01UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for ACMP0_P2   */
#define SYS_GPB_MFPH_PB12MFP_ACMP1_P2    (0x01UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for ACMP1_P2   */
#define SYS_GPB_MFPH_PB12MFP_EBI_AD15    (0x02UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for EBI_AD15   */
#define SYS_GPB_MFPH_PB12MFP_SPI0_MOSI   (0x04UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for SPI0_MOSI  */
#define SYS_GPB_MFPH_PB12MFP_USCI0_CLK   (0x05UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for USCI0_CLK  */
#define SYS_GPB_MFPH_PB12MFP_UART0_RXD   (0x06UL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for UART0_RXD  */
#define SYS_GPB_MFPH_PB12MFP_PSIO0_CH3   (0x0aUL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for PSIO0_CH3  */
#define SYS_GPB_MFPH_PB12MFP_PWM1_CH3    (0x0bUL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for PWM1_CH3   */
#define SYS_GPB_MFPH_PB12MFP_TM3_EXT     (0x0dUL<<SYS_GPB_MFPH_PB12MFP_Pos)/*!< GPB_MFPH PB12 setting for TM3_EXT    */

/* PB.13 MFP */
#define SYS_GPB_MFPH_PB13MFP_GPIO        (0x00UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for GPIO       */
#define SYS_GPB_MFPH_PB13MFP_EADC0_CH13  (0x01UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for EADC0_CH13 */
#define SYS_GPB_MFPH_PB13MFP_ACMP0_P3    (0x01UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for ACMP0_P3   */
#define SYS_GPB_MFPH_PB13MFP_ACMP1_P3    (0x01UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for ACMP1_P3   */
#define SYS_GPB_MFPH_PB13MFP_EBI_AD14    (0x02UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for EBI_AD14   */
#define SYS_GPB_MFPH_PB13MFP_SPI0_MISO   (0x04UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for SPI0_MISO  */
#define SYS_GPB_MFPH_PB13MFP_USCI0_DAT0  (0x05UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for USCI0_DAT0 */
#define SYS_GPB_MFPH_PB13MFP_UART0_TXD   (0x06UL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for UART0_TXD  */
#define SYS_GPB_MFPH_PB13MFP_PSIO0_CH2   (0x0aUL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for PSIO0_CH2  */
#define SYS_GPB_MFPH_PB13MFP_PWM1_CH2    (0x0bUL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for PWM1_CH2   */
#define SYS_GPB_MFPH_PB13MFP_TM2_EXT     (0x0dUL<<SYS_GPB_MFPH_PB13MFP_Pos)/*!< GPB_MFPH PB13 setting for TM2_EXT    */

/* PB.14 MFP */
#define SYS_GPB_MFPH_PB14MFP_GPIO        (0x00UL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for GPIO       */
#define SYS_GPB_MFPH_PB14MFP_EADC0_CH14  (0x01UL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for EADC0_CH14 */
#define SYS_GPB_MFPH_PB14MFP_EBI_AD13    (0x02UL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for EBI_AD13   */
#define SYS_GPB_MFPH_PB14MFP_SPI0_CLK    (0x04UL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for SPI0_CLK   */
#define SYS_GPB_MFPH_PB14MFP_USCI0_DAT1  (0x05UL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for USCI0_DAT1 */
#define SYS_GPB_MFPH_PB14MFP_UART0_nRTS  (0x06UL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for UART0_nRTS */
#define SYS_GPB_MFPH_PB14MFP_PSIO0_CH1   (0x0aUL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for PSIO0_CH1  */
#define SYS_GPB_MFPH_PB14MFP_PWM1_CH1    (0x0bUL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for PWM1_CH1   */
#define SYS_GPB_MFPH_PB14MFP_TM1_EXT     (0x0dUL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for TM1_EXT    */
#define SYS_GPB_MFPH_PB14MFP_CLKO        (0x0eUL<<SYS_GPB_MFPH_PB14MFP_Pos)/*!< GPB_MFPH PB14 setting for CLKO       */

/* PB.15 MFP */
#define SYS_GPB_MFPH_PB15MFP_GPIO        (0x00UL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for GPIO       */
#define SYS_GPB_MFPH_PB15MFP_EADC0_CH15  (0x01UL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for EADC0_CH15 */
#define SYS_GPB_MFPH_PB15MFP_EBI_AD12    (0x02UL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for EBI_AD12   */
#define SYS_GPB_MFPH_PB15MFP_SPI0_SS     (0x04UL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for SPI0_SS    */
#define SYS_GPB_MFPH_PB15MFP_USCI0_CTL1  (0x05UL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for USCI0_CTL1 */
#define SYS_GPB_MFPH_PB15MFP_UART0_nCTS  (0x06UL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for UART0_nCTS */
#define SYS_GPB_MFPH_PB15MFP_PSIO0_CH0   (0x0aUL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for PSIO0_CH0  */
#define SYS_GPB_MFPH_PB15MFP_PWM1_CH0    (0x0bUL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for PWM1_CH0   */
#define SYS_GPB_MFPH_PB15MFP_TM0_EXT     (0x0dUL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for TM0_EXT    */
#define SYS_GPB_MFPH_PB15MFP_PWM0_BRAKE1 (0x0fUL<<SYS_GPB_MFPH_PB15MFP_Pos)/*!< GPB_MFPH PB15 setting for PWM0_BRAKE1    */

/* PC.0 MFP */
#define SYS_GPC_MFPL_PC0MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for GPIO        */
#define SYS_GPC_MFPL_PC0MFP_EBI_AD0      (0x02UL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for EBI_AD0     */
#define SYS_GPC_MFPL_PC0MFP_QSPI0_MOSI0  (0x04UL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for QSPI0_MOSI0  */
#define SYS_GPC_MFPL_PC0MFP_UART2_RXD    (0x08UL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for UART2_RXD   */
#define SYS_GPC_MFPL_PC0MFP_I2C0_SDA     (0x09UL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for I2C0_SDA    */
#define SYS_GPC_MFPL_PC0MFP_PWM1_CH5     (0x0cUL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for PWM1_CH5    */
#define SYS_GPC_MFPL_PC0MFP_USCI2_DAT1   (0x0dUL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for USCI2_DAT1  */
#define SYS_GPC_MFPL_PC0MFP_ACMP1_O      (0x0eUL<<SYS_GPC_MFPL_PC0MFP_Pos) /*!< GPC_MFPL PC0 setting for ACMP1_O     */

/* PC.1 MFP */
#define SYS_GPC_MFPL_PC1MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for GPIO        */
#define SYS_GPC_MFPL_PC1MFP_EBI_AD1      (0x02UL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for EBI_AD1     */
#define SYS_GPC_MFPL_PC1MFP_QSPI0_MISO0  (0x04UL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for QSPI0_MISO0  */
#define SYS_GPC_MFPL_PC1MFP_UART2_TXD    (0x08UL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for UART2_TXD   */
#define SYS_GPC_MFPL_PC1MFP_I2C0_SCL     (0x09UL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for I2C0_SCL    */
#define SYS_GPC_MFPL_PC1MFP_PWM1_CH4     (0x0cUL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for PWM1_CH4    */
#define SYS_GPC_MFPL_PC1MFP_USCI2_DAT0   (0x0dUL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for USCI2_DAT0  */
#define SYS_GPC_MFPL_PC1MFP_ACMP0_O      (0x0eUL<<SYS_GPC_MFPL_PC1MFP_Pos) /*!< GPC_MFPL PC1 setting for ACMP0_O     */

/* PC.2 MFP */
#define SYS_GPC_MFPL_PC2MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for GPIO        */
#define SYS_GPC_MFPL_PC2MFP_EBI_AD2      (0x02UL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for EBI_AD2     */
#define SYS_GPC_MFPL_PC2MFP_QSPI0_CLK    (0x04UL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for QSPI0_CLK    */
#define SYS_GPC_MFPL_PC2MFP_UART2_nCTS   (0x08UL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for UART2_nCTS  */
#define SYS_GPC_MFPL_PC2MFP_I2C0_SMBSUS  (0x09UL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for I2C0_SMBSUS */
#define SYS_GPC_MFPL_PC2MFP_PWM1_CH3     (0x0cUL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for PWM1_CH3    */
#define SYS_GPC_MFPL_PC2MFP_USCI2_CLK    (0x0dUL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for USCI2_CLK   */
#define SYS_GPC_MFPL_PC2MFP_PSIO0_CH3    (0x0fUL<<SYS_GPC_MFPL_PC2MFP_Pos) /*!< GPC_MFPL PC2 setting for PSIO0_CH3   */

/* PC.3 MFP */
#define SYS_GPC_MFPL_PC3MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for GPIO        */
#define SYS_GPC_MFPL_PC3MFP_EBI_AD3      (0x02UL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for EBI_AD3     */
#define SYS_GPC_MFPL_PC3MFP_QSPI0_SS     (0x04UL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for QSPI0_SS     */
#define SYS_GPC_MFPL_PC3MFP_UART2_nRTS   (0x08UL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for UART2_nRTS  */
#define SYS_GPC_MFPL_PC3MFP_I2C0_SMBAL   (0x09UL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for I2C0_SMBAL  */
#define SYS_GPC_MFPL_PC3MFP_PWM1_CH2     (0x0cUL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for PWM1_CH2    */
#define SYS_GPC_MFPL_PC3MFP_USCI2_CTL0   (0x0dUL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for USCI2_CTL0  */
#define SYS_GPC_MFPL_PC3MFP_PSIO0_CH2    (0x0fUL<<SYS_GPC_MFPL_PC3MFP_Pos) /*!< GPC_MFPL PC3 setting for PSIO0_CH2   */

/* PC.4 MFP */
#define SYS_GPC_MFPL_PC4MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for GPIO        */
#define SYS_GPC_MFPL_PC4MFP_EBI_AD4      (0x02UL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for EBI_AD4     */
#define SYS_GPC_MFPL_PC4MFP_QSPI0_MOSI1  (0x04UL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for QSPI0_MOSI1  */
#define SYS_GPC_MFPL_PC4MFP_UART2_RXD    (0x08UL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for UART2_RXD   */
#define SYS_GPC_MFPL_PC4MFP_I2C1_SDA     (0x09UL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for I2C1_SDA    */
#define SYS_GPC_MFPL_PC4MFP_PWM1_CH1     (0x0cUL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for PWM1_CH1    */
#define SYS_GPC_MFPL_PC4MFP_USCI2_CTL1   (0x0dUL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for USCI2_CTL1  */
#define SYS_GPC_MFPL_PC4MFP_PSIO0_CH1    (0x0fUL<<SYS_GPC_MFPL_PC4MFP_Pos) /*!< GPC_MFPL PC4 setting for PSIO0_CH1   */

/* PC.5 MFP */
#define SYS_GPC_MFPL_PC5MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC5MFP_Pos) /*!< GPC_MFPL PC5 setting for GPIO        */
#define SYS_GPC_MFPL_PC5MFP_EBI_AD5      (0x02UL<<SYS_GPC_MFPL_PC5MFP_Pos) /*!< GPC_MFPL PC5 setting for EBI_AD5     */
#define SYS_GPC_MFPL_PC5MFP_QSPI0_MISO1  (0x04UL<<SYS_GPC_MFPL_PC5MFP_Pos) /*!< GPC_MFPL PC5 setting for QSPI0_MISO1  */
#define SYS_GPC_MFPL_PC5MFP_UART2_TXD    (0x08UL<<SYS_GPC_MFPL_PC5MFP_Pos) /*!< GPC_MFPL PC5 setting for UART2_TXD   */
#define SYS_GPC_MFPL_PC5MFP_I2C1_SCL     (0x09UL<<SYS_GPC_MFPL_PC5MFP_Pos) /*!< GPC_MFPL PC5 setting for I2C1_SCL    */
#define SYS_GPC_MFPL_PC5MFP_PWM1_CH0     (0x0cUL<<SYS_GPC_MFPL_PC5MFP_Pos) /*!< GPC_MFPL PC5 setting for PWM1_CH0    */
#define SYS_GPC_MFPL_PC5MFP_PSIO0_CH0    (0x0fUL<<SYS_GPC_MFPL_PC5MFP_Pos) /*!< GPC_MFPL PC5 setting for PSIO0_CH0   */

/* PC.6 MFP */
#define SYS_GPC_MFPL_PC6MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for GPIO        */
#define SYS_GPC_MFPL_PC6MFP_EBI_AD8      (0x02UL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for EBI_AD8     */
#define SYS_GPC_MFPL_PC6MFP_UART0_nRTS   (0x07UL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for UART0_nRTS  */
#define SYS_GPC_MFPL_PC6MFP_I2C1_SMBSUS  (0x08UL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for I2C1_SMBSUS */
#define SYS_GPC_MFPL_PC6MFP_PWM1_CH3     (0x0bUL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for PWM1_CH3    */
#define SYS_GPC_MFPL_PC6MFP_BPWM1_CH1    (0x0cUL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for BPWM1_CH1   */
#define SYS_GPC_MFPL_PC6MFP_TM1          (0x0eUL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for TM1         */
#define SYS_GPC_MFPL_PC6MFP_INT2         (0x0fUL<<SYS_GPC_MFPL_PC6MFP_Pos) /*!< GPC_MFPL PC6 setting for INT2        */

/* PC.7 MFP */
#define SYS_GPC_MFPL_PC7MFP_GPIO         (0x00UL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for GPIO        */
#define SYS_GPC_MFPL_PC7MFP_EBI_AD9      (0x02UL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for EBI_AD9     */
#define SYS_GPC_MFPL_PC7MFP_UART0_nCTS   (0x07UL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for UART0_nCTS  */
#define SYS_GPC_MFPL_PC7MFP_I2C1_SMBAL   (0x08UL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for I2C1_SMBAL  */
#define SYS_GPC_MFPL_PC7MFP_PWM1_CH2     (0x0bUL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for PWM1_CH2    */
#define SYS_GPC_MFPL_PC7MFP_BPWM1_CH0    (0x0cUL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for BPWM1_CH0   */
#define SYS_GPC_MFPL_PC7MFP_TM0          (0x0eUL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for TM0         */
#define SYS_GPC_MFPL_PC7MFP_INT3         (0x0fUL<<SYS_GPC_MFPL_PC7MFP_Pos) /*!< GPC_MFPL PC7 setting for INT3        */

/* PC.8 MFP */
#define SYS_GPC_MFPH_PC8MFP_GPIO         (0x00UL<<SYS_GPC_MFPH_PC8MFP_Pos) /*!< GPC_MFPH PC8 setting for GPIO        */
#define SYS_GPC_MFPH_PC8MFP_EBI_ADR16    (0x02UL<<SYS_GPC_MFPH_PC8MFP_Pos) /*!< GPC_MFPH PC8 setting for EBI_ADR16   */
#define SYS_GPC_MFPH_PC8MFP_I2C0_SDA     (0x04UL<<SYS_GPC_MFPH_PC8MFP_Pos) /*!< GPC_MFPH PC8 setting for I2C0_SDA    */
#define SYS_GPC_MFPH_PC8MFP_UART1_RXD    (0x08UL<<SYS_GPC_MFPH_PC8MFP_Pos) /*!< GPC_MFPH PC8 setting for UART1_RXD   */
#define SYS_GPC_MFPH_PC8MFP_PWM1_CH1     (0x0bUL<<SYS_GPC_MFPH_PC8MFP_Pos) /*!< GPC_MFPH PC8 setting for PWM1_CH1    */
#define SYS_GPC_MFPH_PC8MFP_BPWM1_CH4    (0x0cUL<<SYS_GPC_MFPH_PC8MFP_Pos) /*!< GPC_MFPH PC8 setting for BPWM1_CH4   */

/* PC.9 MFP */
#define SYS_GPC_MFPH_PC9MFP_GPIO         (0x00UL<<SYS_GPC_MFPH_PC9MFP_Pos) /*!< GPC_MFPH PC9 setting for GPIO        */
#define SYS_GPC_MFPH_PC9MFP_EBI_ADR7     (0x02UL<<SYS_GPC_MFPH_PC9MFP_Pos) /*!< GPC_MFPH PC9 setting for EBI_ADR7    */
#define SYS_GPC_MFPH_PC9MFP_PWM1_CH3     (0x0cUL<<SYS_GPC_MFPH_PC9MFP_Pos) /*!< GPC_MFPH PC9 setting for PWM1_CH3    */

/* PC.10 MFP */
#define SYS_GPC_MFPH_PC10MFP_GPIO        (0x00UL<<SYS_GPC_MFPH_PC10MFP_Pos)/*!< GPC_MFPH PC10 setting for GPIO       */
#define SYS_GPC_MFPH_PC10MFP_EBI_ADR6    (0x02UL<<SYS_GPC_MFPH_PC10MFP_Pos)/*!< GPC_MFPH PC10 setting for EBI_ADR6   */
#define SYS_GPC_MFPH_PC10MFP_PWM1_CH2    (0x0cUL<<SYS_GPC_MFPH_PC10MFP_Pos)/*!< GPC_MFPH PC10 setting for PWM1_CH2   */

/* PC.11 MFP */
#define SYS_GPC_MFPH_PC11MFP_GPIO        (0x00UL<<SYS_GPC_MFPH_PC11MFP_Pos)/*!< GPC_MFPH PC11 setting for GPIO       */
#define SYS_GPC_MFPH_PC11MFP_EBI_ADR5    (0x02UL<<SYS_GPC_MFPH_PC11MFP_Pos)/*!< GPC_MFPH PC11 setting for EBI_ADR5   */
#define SYS_GPC_MFPH_PC11MFP_UART0_RXD   (0x03UL<<SYS_GPC_MFPH_PC11MFP_Pos)/*!< GPC_MFPH PC11 setting for UART0_RXD  */
#define SYS_GPC_MFPH_PC11MFP_I2C0_SDA    (0x04UL<<SYS_GPC_MFPH_PC11MFP_Pos)/*!< GPC_MFPH PC11 setting for I2C0_SDA   */
#define SYS_GPC_MFPH_PC11MFP_PWM1_CH1    (0x0cUL<<SYS_GPC_MFPH_PC11MFP_Pos)/*!< GPC_MFPH PC11 setting for PWM1_CH1   */
#define SYS_GPC_MFPH_PC11MFP_ACMP1_O     (0x0eUL<<SYS_GPC_MFPH_PC11MFP_Pos)/*!< GPC_MFPH PC11 setting for ACMP1_O    */

/* PC.12 MFP */
#define SYS_GPC_MFPH_PC12MFP_GPIO        (0x00UL<<SYS_GPC_MFPH_PC12MFP_Pos)/*!< GPC_MFPH PC12 setting for GPIO       */
#define SYS_GPC_MFPH_PC12MFP_EBI_ADR4    (0x02UL<<SYS_GPC_MFPH_PC12MFP_Pos)/*!< GPC_MFPH PC12 setting for EBI_ADR4   */
#define SYS_GPC_MFPH_PC12MFP_UART0_TXD   (0x03UL<<SYS_GPC_MFPH_PC12MFP_Pos)/*!< GPC_MFPH PC12 setting for UART0_TXD  */
#define SYS_GPC_MFPH_PC12MFP_I2C0_SCL    (0x04UL<<SYS_GPC_MFPH_PC12MFP_Pos)/*!< GPC_MFPH PC12 setting for I2C0_SCL   */
#define SYS_GPC_MFPH_PC12MFP_SC0_nCD     (0x09UL<<SYS_GPC_MFPH_PC12MFP_Pos)/*!< GPC_MFPH PC12 setting for SC0_nCD    */
#define SYS_GPC_MFPH_PC12MFP_PWM1_CH0    (0x0cUL<<SYS_GPC_MFPH_PC12MFP_Pos)/*!< GPC_MFPH PC12 setting for PWM1_CH0   */
#define SYS_GPC_MFPH_PC12MFP_ACMP0_O     (0x0eUL<<SYS_GPC_MFPH_PC12MFP_Pos)/*!< GPC_MFPH PC12 setting for ACMP0_O    */

/* PC.13 MFP */
#define SYS_GPC_MFPH_PC13MFP_GPIO        (0x00UL<<SYS_GPC_MFPH_PC13MFP_Pos)/*!< GPC_MFPH PC13 setting for GPIO       */

/* PC.14 MFP */
#define SYS_GPC_MFPH_PC14MFP_GPIO         (0x00UL<<SYS_GPC_MFPH_PC14MFP_Pos)/*!< GPC_MFPH PC14 setting for GPIO       */
#define SYS_GPC_MFPH_PC14MFP_EBI_AD11     (0x02UL<<SYS_GPC_MFPH_PC14MFP_Pos)/*!< GPC_MFPH PC14 setting for EBI_AD11   */
#define SYS_GPC_MFPH_PC14MFP_SPI0_I2SMCLK (0x04UL<<SYS_GPC_MFPH_PC14MFP_Pos)/*!< GPC_MFPH PC14 setting for SPI0_I2SMCLK*/
#define SYS_GPC_MFPH_PC14MFP_USCI0_CTL0   (0x05UL<<SYS_GPC_MFPH_PC14MFP_Pos)/*!< GPC_MFPH PC14 setting for USCI0_CTL0 */
#define SYS_GPC_MFPH_PC14MFP_QSPI0_CLK    (0x06UL<<SYS_GPC_MFPH_PC14MFP_Pos)/*!< GPC_MFPH PC14 setting for QSPI0_CLK   */
#define SYS_GPC_MFPH_PC14MFP_USCI2_CLK    (0x07UL<<SYS_GPC_MFPH_PC14MFP_Pos)/*!< GPC_MFPH PC14 setting for USCI2_CLK  */
#define SYS_GPC_MFPH_PC14MFP_TM1          (0x0dUL<<SYS_GPC_MFPH_PC14MFP_Pos)/*!< GPC_MFPH PC14 setting for TM1        */

/* PD.0 MFP */
#define SYS_GPD_MFPL_PD0MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD0MFP_Pos) /*!< GPD_MFPL PD0 setting for GPIO        */
#define SYS_GPD_MFPL_PD0MFP_EBI_AD13     (0x02UL<<SYS_GPD_MFPL_PD0MFP_Pos) /*!< GPD_MFPL PD0 setting for EBI_AD13    */
#define SYS_GPD_MFPL_PD0MFP_USCI0_CLK    (0x03UL<<SYS_GPD_MFPL_PD0MFP_Pos) /*!< GPD_MFPL PD0 setting for USCI0_CLK   */
#define SYS_GPD_MFPL_PD0MFP_SPI0_MOSI    (0x04UL<<SYS_GPD_MFPL_PD0MFP_Pos) /*!< GPD_MFPL PD0 setting for SPI0_MOSI   */
#define SYS_GPD_MFPL_PD0MFP_TM2          (0x0eUL<<SYS_GPD_MFPL_PD0MFP_Pos) /*!< GPD_MFPL PD0 setting for TM2         */

/* PD.1 MFP */
#define SYS_GPD_MFPL_PD1MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD1MFP_Pos) /*!< GPD_MFPL PD1 setting for GPIO        */
#define SYS_GPD_MFPL_PD1MFP_EBI_AD12     (0x02UL<<SYS_GPD_MFPL_PD1MFP_Pos) /*!< GPD_MFPL PD1 setting for EBI_AD12    */
#define SYS_GPD_MFPL_PD1MFP_USCI0_DAT0   (0x03UL<<SYS_GPD_MFPL_PD1MFP_Pos) /*!< GPD_MFPL PD1 setting for USCI0_DAT0  */
#define SYS_GPD_MFPL_PD1MFP_SPI0_MISO    (0x04UL<<SYS_GPD_MFPL_PD1MFP_Pos) /*!< GPD_MFPL PD1 setting for SPI0_MISO   */

/* PD.2 MFP */
#define SYS_GPD_MFPL_PD2MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD2MFP_Pos) /*!< GPD_MFPL PD2 setting for GPIO        */
#define SYS_GPD_MFPL_PD2MFP_EBI_AD11     (0x02UL<<SYS_GPD_MFPL_PD2MFP_Pos) /*!< GPD_MFPL PD2 setting for EBI_AD11    */
#define SYS_GPD_MFPL_PD2MFP_USCI0_DAT1   (0x03UL<<SYS_GPD_MFPL_PD2MFP_Pos) /*!< GPD_MFPL PD2 setting for USCI0_DAT1  */
#define SYS_GPD_MFPL_PD2MFP_SPI0_CLK     (0x04UL<<SYS_GPD_MFPL_PD2MFP_Pos) /*!< GPD_MFPL PD2 setting for SPI0_CLK    */
#define SYS_GPD_MFPL_PD2MFP_UART0_RXD    (0x09UL<<SYS_GPD_MFPL_PD2MFP_Pos) /*!< GPD_MFPL PD2 setting for UART0_RXD   */

/* PD.3 MFP */
#define SYS_GPD_MFPL_PD3MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD3MFP_Pos) /*!< GPD_MFPL PD3 setting for GPIO        */
#define SYS_GPD_MFPL_PD3MFP_EBI_AD10     (0x02UL<<SYS_GPD_MFPL_PD3MFP_Pos) /*!< GPD_MFPL PD3 setting for EBI_AD10    */
#define SYS_GPD_MFPL_PD3MFP_USCI0_CTL1   (0x03UL<<SYS_GPD_MFPL_PD3MFP_Pos) /*!< GPD_MFPL PD3 setting for USCI0_CTL1  */
#define SYS_GPD_MFPL_PD3MFP_SPI0_SS      (0x04UL<<SYS_GPD_MFPL_PD3MFP_Pos) /*!< GPD_MFPL PD3 setting for SPI0_SS     */
#define SYS_GPD_MFPL_PD3MFP_USCI1_CTL0   (0x06UL<<SYS_GPD_MFPL_PD3MFP_Pos) /*!< GPD_MFPL PD3 setting for USCI1_CTL0  */
#define SYS_GPD_MFPL_PD3MFP_UART0_TXD    (0x09UL<<SYS_GPD_MFPL_PD3MFP_Pos) /*!< GPD_MFPL PD3 setting for UART0_TXD   */

/* PD.4 MFP */
#define SYS_GPD_MFPL_PD4MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD4MFP_Pos) /*!< GPD_MFPL PD4 setting for GPIO        */
#define SYS_GPD_MFPL_PD4MFP_USCI0_CTL0   (0x03UL<<SYS_GPD_MFPL_PD4MFP_Pos) /*!< GPD_MFPL PD4 setting for USCI0_CTL0  */
#define SYS_GPD_MFPL_PD4MFP_I2C1_SDA     (0x04UL<<SYS_GPD_MFPL_PD4MFP_Pos) /*!< GPD_MFPL PD4 setting for I2C1_SDA    */
#define SYS_GPD_MFPL_PD4MFP_USCI1_CTL1   (0x06UL<<SYS_GPD_MFPL_PD4MFP_Pos) /*!< GPD_MFPL PD4 setting for USCI1_CTL1  */
#define SYS_GPD_MFPL_PD4MFP_PSIO0_CH7    (0x0aUL<<SYS_GPD_MFPL_PD4MFP_Pos) /*!< GPD_MFPL PD4 setting for PSIO0_CH7   */

/* PD.5 MFP */
#define SYS_GPD_MFPL_PD5MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD5MFP_Pos) /*!< GPD_MFPL PD5 setting for GPIO        */
#define SYS_GPD_MFPL_PD5MFP_I2C1_SCL     (0x04UL<<SYS_GPD_MFPL_PD5MFP_Pos) /*!< GPD_MFPL PD5 setting for I2C1_SCL    */
#define SYS_GPD_MFPL_PD5MFP_USCI1_DAT0   (0x06UL<<SYS_GPD_MFPL_PD5MFP_Pos) /*!< GPD_MFPL PD5 setting for USCI1_DAT0  */
#define SYS_GPD_MFPL_PD5MFP_PSIO0_CH6    (0x0aUL<<SYS_GPD_MFPL_PD5MFP_Pos) /*!< GPD_MFPL PD5 setting for PSIO0_CH6   */

/* PD.6 MFP */
#define SYS_GPD_MFPL_PD6MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD6MFP_Pos) /*!< GPD_MFPL PD6 setting for GPIO        */
#define SYS_GPD_MFPL_PD6MFP_UART1_RXD    (0x03UL<<SYS_GPD_MFPL_PD6MFP_Pos) /*!< GPD_MFPL PD6 setting for UART1_RXD   */
#define SYS_GPD_MFPL_PD6MFP_I2C0_SDA     (0x04UL<<SYS_GPD_MFPL_PD6MFP_Pos) /*!< GPD_MFPL PD6 setting for I2C0_SDA    */
#define SYS_GPD_MFPL_PD6MFP_USCI1_DAT1   (0x06UL<<SYS_GPD_MFPL_PD6MFP_Pos) /*!< GPD_MFPL PD6 setting for USCI1_DAT1  */
#define SYS_GPD_MFPL_PD6MFP_PSIO0_CH5    (0x0aUL<<SYS_GPD_MFPL_PD6MFP_Pos) /*!< GPD_MFPL PD6 setting for PSIO0_CH5   */

/* PD.7 MFP */
#define SYS_GPD_MFPL_PD7MFP_GPIO         (0x00UL<<SYS_GPD_MFPL_PD7MFP_Pos) /*!< GPD_MFPL PD7 setting for GPIO        */
#define SYS_GPD_MFPL_PD7MFP_UART1_TXD    (0x03UL<<SYS_GPD_MFPL_PD7MFP_Pos) /*!< GPD_MFPL PD7 setting for UART1_TXD   */
#define SYS_GPD_MFPL_PD7MFP_I2C0_SCL     (0x04UL<<SYS_GPD_MFPL_PD7MFP_Pos) /*!< GPD_MFPL PD7 setting for I2C0_SCL    */
#define SYS_GPD_MFPL_PD7MFP_USCI1_CLK    (0x06UL<<SYS_GPD_MFPL_PD7MFP_Pos) /*!< GPD_MFPL PD7 setting for USCI1_CLK   */
#define SYS_GPD_MFPL_PD7MFP_PSIO0_CH4    (0x0aUL<<SYS_GPD_MFPL_PD7MFP_Pos) /*!< GPD_MFPL PD7 setting for PSIO0_CH4   */

/* PD.8 MFP */
#define SYS_GPD_MFPH_PD8MFP_GPIO         (0x00UL<<SYS_GPD_MFPH_PD8MFP_Pos) /*!< GPD_MFPH PD8 setting for GPIO        */
#define SYS_GPD_MFPH_PD8MFP_EBI_AD6      (0x02UL<<SYS_GPD_MFPH_PD8MFP_Pos) /*!< GPD_MFPH PD8 setting for EBI_AD6     */
#define SYS_GPD_MFPH_PD8MFP_UART2_nRTS   (0x04UL<<SYS_GPD_MFPH_PD8MFP_Pos) /*!< GPD_MFPH PD8 setting for UART2_nRTS  */
#define SYS_GPD_MFPH_PD8MFP_PSIO0_CH3    (0x0aUL<<SYS_GPD_MFPH_PD8MFP_Pos) /*!< GPD_MFPH PD8 setting for PSIO0_CH3   */

/* PD.9 MFP */
#define SYS_GPD_MFPH_PD9MFP_GPIO         (0x00UL<<SYS_GPD_MFPH_PD9MFP_Pos) /*!< GPD_MFPH PD9 setting for GPIO        */
#define SYS_GPD_MFPH_PD9MFP_EBI_AD7      (0x02UL<<SYS_GPD_MFPH_PD9MFP_Pos) /*!< GPD_MFPH PD9 setting for EBI_AD7     */
#define SYS_GPD_MFPH_PD9MFP_UART2_nCTS   (0x04UL<<SYS_GPD_MFPH_PD9MFP_Pos) /*!< GPD_MFPH PD9 setting for UART2_nCTS  */
#define SYS_GPD_MFPH_PD9MFP_PSIO0_CH2    (0x0aUL<<SYS_GPD_MFPH_PD9MFP_Pos) /*!< GPD_MFPH PD9 setting for PSIO0_CH2   */

/* PD.10 MFP */
#define SYS_GPD_MFPH_PD10MFP_GPIO        (0x00UL<<SYS_GPD_MFPH_PD10MFP_Pos)/*!< GPD_MFPH PD10 setting for GPIO       */
#define SYS_GPD_MFPH_PD10MFP_EBI_nCS2    (0x02UL<<SYS_GPD_MFPH_PD10MFP_Pos)/*!< GPD_MFPH PD10 setting for EBI_nCS2   */
#define SYS_GPD_MFPH_PD10MFP_UART1_RXD   (0x03UL<<SYS_GPD_MFPH_PD10MFP_Pos)/*!< GPD_MFPH PD10 setting for UART1_RXD  */

/* PD.11 MFP */
#define SYS_GPD_MFPH_PD11MFP_GPIO        (0x00UL<<SYS_GPD_MFPH_PD11MFP_Pos)/*!< GPD_MFPH PD11 setting for GPIO       */
#define SYS_GPD_MFPH_PD11MFP_EBI_nCS1    (0x02UL<<SYS_GPD_MFPH_PD11MFP_Pos)/*!< GPD_MFPH PD11 setting for EBI_nCS1   */
#define SYS_GPD_MFPH_PD11MFP_UART1_TXD   (0x03UL<<SYS_GPD_MFPH_PD11MFP_Pos)/*!< GPD_MFPH PD11 setting for UART1_TXD  */

/* PD.12 MFP */
#define SYS_GPD_MFPH_PD12MFP_GPIO        (0x00UL<<SYS_GPD_MFPH_PD12MFP_Pos)/*!< GPD_MFPH PD12 setting for GPIO       */
#define SYS_GPD_MFPH_PD12MFP_EBI_nCS0    (0x02UL<<SYS_GPD_MFPH_PD12MFP_Pos)/*!< GPD_MFPH PD12 setting for EBI_nCS0   */
#define SYS_GPD_MFPH_PD12MFP_UART2_RXD   (0x07UL<<SYS_GPD_MFPH_PD12MFP_Pos)/*!< GPD_MFPH PD12 setting for UART2_RXD  */
#define SYS_GPD_MFPH_PD12MFP_BPWM0_CH5   (0x09UL<<SYS_GPD_MFPH_PD12MFP_Pos)/*!< GPD_MFPH PD12 setting for BPWM0_CH5  */
#define SYS_GPD_MFPH_PD12MFP_CLKO        (0x0dUL<<SYS_GPD_MFPH_PD12MFP_Pos)/*!< GPD_MFPH PD12 setting for CLKO       */
#define SYS_GPD_MFPH_PD12MFP_EADC0_ST    (0x0eUL<<SYS_GPD_MFPH_PD12MFP_Pos)/*!< GPD_MFPH PD12 setting for EADC0_ST   */
#define SYS_GPD_MFPH_PD12MFP_INT5        (0x0fUL<<SYS_GPD_MFPH_PD12MFP_Pos)/*!< GPD_MFPH PD12 setting for INT5       */

/* PD.13 MFP */
#define SYS_GPD_MFPH_PD13MFP_GPIO         (0x00UL<<SYS_GPD_MFPH_PD13MFP_Pos)/*!< GPD_MFPH PD13 setting for GPIO       */
#define SYS_GPD_MFPH_PD13MFP_EBI_AD10     (0x02UL<<SYS_GPD_MFPH_PD13MFP_Pos)/*!< GPD_MFPH PD13 setting for EBI_AD10   */
#define SYS_GPD_MFPH_PD13MFP_SPI0_I2SMCLK (0x04UL<<SYS_GPD_MFPH_PD13MFP_Pos)/*!< GPD_MFPH PD13 setting for SPI0_I2SMCLK*/
#define SYS_GPD_MFPH_PD13MFP_USCI2_CTL0   (0x08UL<<SYS_GPD_MFPH_PD13MFP_Pos)/*!< GPD_MFPH PD13 setting for USCI2_CTL0 */

/* PD.14 MFP */
#define SYS_GPD_MFPH_PD14MFP_GPIO        (0x00UL<<SYS_GPD_MFPH_PD14MFP_Pos)/*!< GPD_MFPH PD14 setting for GPIO       */

/* PD.15 MFP */
#define SYS_GPD_MFPH_PD15MFP_GPIO        (0x00UL<<SYS_GPD_MFPH_PD15MFP_Pos)/*!< GPD_MFPH PD15 setting for GPIO       */
#define SYS_GPD_MFPH_PD15MFP_PSIO0_CH7   (0x0bUL<<SYS_GPD_MFPH_PD15MFP_Pos)/*!< GPD_MFPH PD15 setting for PSIO0_CH7  */
#define SYS_GPD_MFPH_PD15MFP_PWM0_CH5    (0x0cUL<<SYS_GPD_MFPH_PD15MFP_Pos)/*!< GPD_MFPH PD15 setting for PWM0_CH5   */
#define SYS_GPD_MFPH_PD15MFP_TM3         (0x0eUL<<SYS_GPD_MFPH_PD15MFP_Pos)/*!< GPD_MFPH PD15 setting for TM3        */
#define SYS_GPD_MFPH_PD15MFP_INT1        (0x0fUL<<SYS_GPD_MFPH_PD15MFP_Pos)/*!< GPD_MFPH PD15 setting for INT1       */

/* PE.0 MFP */
#define SYS_GPE_MFPL_PE0MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE0MFP_Pos) /*!< GPE_MFPL PE0 setting for GPIO        */
#define SYS_GPE_MFPL_PE0MFP_EBI_AD11     (0x02UL<<SYS_GPE_MFPL_PE0MFP_Pos) /*!< GPE_MFPL PE0 setting for EBI_AD11    */
#define SYS_GPE_MFPL_PE0MFP_QSPI0_MOSI0  (0x03UL<<SYS_GPE_MFPL_PE0MFP_Pos) /*!< GPE_MFPL PE0 setting for QSPI0_MOSI0  */
#define SYS_GPE_MFPL_PE0MFP_I2C1_SDA     (0x08UL<<SYS_GPE_MFPL_PE0MFP_Pos) /*!< GPE_MFPL PE0 setting for I2C1_SDA    */
#define SYS_GPE_MFPL_PE0MFP_USCI2_DAT0   (0x0aUL<<SYS_GPE_MFPL_PE0MFP_Pos) /*!< GPE_MFPL PE0 setting for USCI2_DAT0  */

/* PE.1 MFP */
#define SYS_GPE_MFPL_PE1MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE1MFP_Pos) /*!< GPE_MFPL PE1 setting for GPIO        */
#define SYS_GPE_MFPL_PE1MFP_EBI_AD10     (0x02UL<<SYS_GPE_MFPL_PE1MFP_Pos) /*!< GPE_MFPL PE1 setting for EBI_AD10    */
#define SYS_GPE_MFPL_PE1MFP_QSPI0_MISO0  (0x03UL<<SYS_GPE_MFPL_PE1MFP_Pos) /*!< GPE_MFPL PE1 setting for QSPI0_MISO0  */
#define SYS_GPE_MFPL_PE1MFP_I2C1_SCL     (0x08UL<<SYS_GPE_MFPL_PE1MFP_Pos) /*!< GPE_MFPL PE1 setting for I2C1_SCL    */
#define SYS_GPE_MFPL_PE1MFP_USCI2_DAT1   (0x0aUL<<SYS_GPE_MFPL_PE1MFP_Pos) /*!< GPE_MFPL PE1 setting for USCI2_DAT1  */

/* PE.2 MFP */
#define SYS_GPE_MFPL_PE2MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE2MFP_Pos) /*!< GPE_MFPL PE2 setting for GPIO        */
#define SYS_GPE_MFPL_PE2MFP_EBI_ALE      (0x02UL<<SYS_GPE_MFPL_PE2MFP_Pos) /*!< GPE_MFPL PE2 setting for EBI_ALE     */
#define SYS_GPE_MFPL_PE2MFP_SC0_CLK      (0x06UL<<SYS_GPE_MFPL_PE2MFP_Pos) /*!< GPE_MFPL PE2 setting for SC0_CLK     */
#define SYS_GPE_MFPL_PE2MFP_USCI0_CLK    (0x07UL<<SYS_GPE_MFPL_PE2MFP_Pos) /*!< GPE_MFPL PE2 setting for USCI0_CLK   */
#define SYS_GPE_MFPL_PE2MFP_USCI2_CTL0   (0x0aUL<<SYS_GPE_MFPL_PE2MFP_Pos) /*!< GPE_MFPL PE2 setting for USCI2_CTL0  */
#define SYS_GPE_MFPL_PE2MFP_PWM0_CH5     (0x0cUL<<SYS_GPE_MFPL_PE2MFP_Pos) /*!< GPE_MFPL PE2 setting for PWM0_CH5    */
#define SYS_GPE_MFPL_PE2MFP_BPWM0_CH0    (0x0dUL<<SYS_GPE_MFPL_PE2MFP_Pos) /*!< GPE_MFPL PE2 setting for BPWM0_CH0   */

/* PE.3 MFP */
#define SYS_GPE_MFPL_PE3MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE3MFP_Pos) /*!< GPE_MFPL PE3 setting for GPIO        */
#define SYS_GPE_MFPL_PE3MFP_EBI_MCLK     (0x02UL<<SYS_GPE_MFPL_PE3MFP_Pos) /*!< GPE_MFPL PE3 setting for EBI_MCLK    */
#define SYS_GPE_MFPL_PE3MFP_SC0_DAT      (0x06UL<<SYS_GPE_MFPL_PE3MFP_Pos) /*!< GPE_MFPL PE3 setting for SC0_DAT     */
#define SYS_GPE_MFPL_PE3MFP_USCI0_DAT0   (0x07UL<<SYS_GPE_MFPL_PE3MFP_Pos) /*!< GPE_MFPL PE3 setting for USCI0_DAT0  */
#define SYS_GPE_MFPL_PE3MFP_PWM0_CH4     (0x0cUL<<SYS_GPE_MFPL_PE3MFP_Pos) /*!< GPE_MFPL PE3 setting for PWM0_CH4    */
#define SYS_GPE_MFPL_PE3MFP_BPWM0_CH1    (0x0dUL<<SYS_GPE_MFPL_PE3MFP_Pos) /*!< GPE_MFPL PE3 setting for BPWM0_CH1   */

/* PE.4 MFP */
#define SYS_GPE_MFPL_PE4MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE4MFP_Pos) /*!< GPE_MFPL PE4 setting for GPIO        */
#define SYS_GPE_MFPL_PE4MFP_EBI_nWR      (0x02UL<<SYS_GPE_MFPL_PE4MFP_Pos) /*!< GPE_MFPL PE4 setting for EBI_nWR     */
#define SYS_GPE_MFPL_PE4MFP_SC0_RST      (0x06UL<<SYS_GPE_MFPL_PE4MFP_Pos) /*!< GPE_MFPL PE4 setting for SC0_RST     */
#define SYS_GPE_MFPL_PE4MFP_USCI0_DAT1   (0x07UL<<SYS_GPE_MFPL_PE4MFP_Pos) /*!< GPE_MFPL PE4 setting for USCI0_DAT1  */
#define SYS_GPE_MFPL_PE4MFP_PSIO0_CH3    (0x0aUL<<SYS_GPE_MFPL_PE4MFP_Pos) /*!< GPE_MFPL PE4 setting for PSIO0_CH3   */
#define SYS_GPE_MFPL_PE4MFP_PWM0_CH3     (0x0cUL<<SYS_GPE_MFPL_PE4MFP_Pos) /*!< GPE_MFPL PE4 setting for PWM0_CH3    */
#define SYS_GPE_MFPL_PE4MFP_BPWM0_CH2    (0x0dUL<<SYS_GPE_MFPL_PE4MFP_Pos) /*!< GPE_MFPL PE4 setting for BPWM0_CH2   */

/* PE.5 MFP */
#define SYS_GPE_MFPL_PE5MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE5MFP_Pos) /*!< GPE_MFPL PE5 setting for GPIO        */
#define SYS_GPE_MFPL_PE5MFP_EBI_nRD      (0x02UL<<SYS_GPE_MFPL_PE5MFP_Pos) /*!< GPE_MFPL PE5 setting for EBI_nRD     */
#define SYS_GPE_MFPL_PE5MFP_SC0_PWR      (0x06UL<<SYS_GPE_MFPL_PE5MFP_Pos) /*!< GPE_MFPL PE5 setting for SC0_PWR     */
#define SYS_GPE_MFPL_PE5MFP_USCI0_CTL1   (0x07UL<<SYS_GPE_MFPL_PE5MFP_Pos) /*!< GPE_MFPL PE5 setting for USCI0_CTL1  */
#define SYS_GPE_MFPL_PE5MFP_PSIO0_CH2    (0x0aUL<<SYS_GPE_MFPL_PE5MFP_Pos) /*!< GPE_MFPL PE5 setting for PSIO0_CH2   */
#define SYS_GPE_MFPL_PE5MFP_PWM0_CH2     (0x0cUL<<SYS_GPE_MFPL_PE5MFP_Pos) /*!< GPE_MFPL PE5 setting for PWM0_CH2    */
#define SYS_GPE_MFPL_PE5MFP_BPWM0_CH3    (0x0dUL<<SYS_GPE_MFPL_PE5MFP_Pos) /*!< GPE_MFPL PE5 setting for BPWM0_CH3   */

/* PE.6 MFP */
#define SYS_GPE_MFPL_PE6MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE6MFP_Pos) /*!< GPE_MFPL PE6 setting for GPIO        */
#define SYS_GPE_MFPL_PE6MFP_SC0_nCD      (0x06UL<<SYS_GPE_MFPL_PE6MFP_Pos) /*!< GPE_MFPL PE6 setting for SC0_nCD     */
#define SYS_GPE_MFPL_PE6MFP_USCI0_CTL0   (0x07UL<<SYS_GPE_MFPL_PE6MFP_Pos) /*!< GPE_MFPL PE6 setting for USCI0_CTL0  */
#define SYS_GPE_MFPL_PE6MFP_PSIO0_CH1    (0x0aUL<<SYS_GPE_MFPL_PE6MFP_Pos) /*!< GPE_MFPL PE6 setting for PSIO0_CH1   */
#define SYS_GPE_MFPL_PE6MFP_PWM0_CH1     (0x0cUL<<SYS_GPE_MFPL_PE6MFP_Pos) /*!< GPE_MFPL PE6 setting for PWM0_CH1    */
#define SYS_GPE_MFPL_PE6MFP_BPWM0_CH4    (0x0dUL<<SYS_GPE_MFPL_PE6MFP_Pos) /*!< GPE_MFPL PE6 setting for BPWM0_CH4   */

/* PE.7 MFP */
#define SYS_GPE_MFPL_PE7MFP_GPIO         (0x00UL<<SYS_GPE_MFPL_PE7MFP_Pos) /*!< GPE_MFPL PE7 setting for GPIO        */
#define SYS_GPE_MFPL_PE7MFP_PSIO0_CH0    (0x0aUL<<SYS_GPE_MFPL_PE7MFP_Pos) /*!< GPE_MFPL PE7 setting for PSIO0_CH0   */
#define SYS_GPE_MFPL_PE7MFP_PWM0_CH0     (0x0cUL<<SYS_GPE_MFPL_PE7MFP_Pos) /*!< GPE_MFPL PE7 setting for PWM0_CH0    */
#define SYS_GPE_MFPL_PE7MFP_BPWM0_CH5    (0x0dUL<<SYS_GPE_MFPL_PE7MFP_Pos) /*!< GPE_MFPL PE7 setting for BPWM0_CH5   */

/* PE.8 MFP */
#define SYS_GPE_MFPH_PE8MFP_GPIO         (0x00UL<<SYS_GPE_MFPH_PE8MFP_Pos) /*!< GPE_MFPH PE8 setting for GPIO        */
#define SYS_GPE_MFPH_PE8MFP_EBI_ADR10    (0x02UL<<SYS_GPE_MFPH_PE8MFP_Pos) /*!< GPE_MFPH PE8 setting for EBI_ADR10   */
#define SYS_GPE_MFPH_PE8MFP_USCI1_CTL1   (0x06UL<<SYS_GPE_MFPH_PE8MFP_Pos) /*!< GPE_MFPH PE8 setting for USCI1_CTL1  */
#define SYS_GPE_MFPH_PE8MFP_UART2_TXD    (0x07UL<<SYS_GPE_MFPH_PE8MFP_Pos) /*!< GPE_MFPH PE8 setting for UART2_TXD   */
#define SYS_GPE_MFPH_PE8MFP_PWM0_CH0     (0x0aUL<<SYS_GPE_MFPH_PE8MFP_Pos) /*!< GPE_MFPH PE8 setting for PWM0_CH0    */
#define SYS_GPE_MFPH_PE8MFP_PWM0_BRAKE0  (0x0bUL<<SYS_GPE_MFPH_PE8MFP_Pos) /*!< GPE_MFPH PE8 setting for PWM0_BRAKE0 */

/* PE.9 MFP */
#define SYS_GPE_MFPH_PE9MFP_GPIO         (0x00UL<<SYS_GPE_MFPH_PE9MFP_Pos) /*!< GPE_MFPH PE9 setting for GPIO        */
#define SYS_GPE_MFPH_PE9MFP_EBI_ADR11    (0x02UL<<SYS_GPE_MFPH_PE9MFP_Pos) /*!< GPE_MFPH PE9 setting for EBI_ADR11   */
#define SYS_GPE_MFPH_PE9MFP_USCI1_CTL0   (0x06UL<<SYS_GPE_MFPH_PE9MFP_Pos) /*!< GPE_MFPH PE9 setting for USCI1_CTL0  */
#define SYS_GPE_MFPH_PE9MFP_UART2_RXD    (0x07UL<<SYS_GPE_MFPH_PE9MFP_Pos) /*!< GPE_MFPH PE9 setting for UART2_RXD   */
#define SYS_GPE_MFPH_PE9MFP_PWM0_CH1     (0x0aUL<<SYS_GPE_MFPH_PE9MFP_Pos) /*!< GPE_MFPH PE9 setting for PWM0_CH1    */
#define SYS_GPE_MFPH_PE9MFP_PWM0_BRAKE1  (0x0bUL<<SYS_GPE_MFPH_PE9MFP_Pos) /*!< GPE_MFPH PE9 setting for PWM0_BRAKE1 */

/* PE.10 MFP */
#define SYS_GPE_MFPH_PE10MFP_GPIO        (0x00UL<<SYS_GPE_MFPH_PE10MFP_Pos)/*!< GPE_MFPH PE10 setting for GPIO       */
#define SYS_GPE_MFPH_PE10MFP_EBI_ADR12   (0x02UL<<SYS_GPE_MFPH_PE10MFP_Pos)/*!< GPE_MFPH PE10 setting for EBI_ADR12  */
#define SYS_GPE_MFPH_PE10MFP_USCI1_DAT0  (0x06UL<<SYS_GPE_MFPH_PE10MFP_Pos)/*!< GPE_MFPH PE10 setting for USCI1_DAT0 */
#define SYS_GPE_MFPH_PE10MFP_PWM0_CH2    (0x0aUL<<SYS_GPE_MFPH_PE10MFP_Pos)/*!< GPE_MFPH PE10 setting for PWM0_CH2   */
#define SYS_GPE_MFPH_PE10MFP_PWM1_BRAKE0 (0x0bUL<<SYS_GPE_MFPH_PE10MFP_Pos)/*!< GPE_MFPH PE10 setting for PWM1_BRAKE0*/

/* PE.11 MFP */
#define SYS_GPE_MFPH_PE11MFP_GPIO        (0x00UL<<SYS_GPE_MFPH_PE11MFP_Pos)/*!< GPE_MFPH PE11 setting for GPIO       */
#define SYS_GPE_MFPH_PE11MFP_EBI_ADR13   (0x02UL<<SYS_GPE_MFPH_PE11MFP_Pos)/*!< GPE_MFPH PE11 setting for EBI_ADR13  */
#define SYS_GPE_MFPH_PE11MFP_USCI1_DAT1  (0x06UL<<SYS_GPE_MFPH_PE11MFP_Pos)/*!< GPE_MFPH PE11 setting for USCI1_DAT1 */
#define SYS_GPE_MFPH_PE11MFP_UART1_nCTS  (0x08UL<<SYS_GPE_MFPH_PE11MFP_Pos)/*!< GPE_MFPH PE11 setting for UART1_nCTS */
#define SYS_GPE_MFPH_PE11MFP_PWM0_CH3    (0x0aUL<<SYS_GPE_MFPH_PE11MFP_Pos)/*!< GPE_MFPH PE11 setting for PWM0_CH3   */
#define SYS_GPE_MFPH_PE11MFP_PWM1_BRAKE1 (0x0bUL<<SYS_GPE_MFPH_PE11MFP_Pos)/*!< GPE_MFPH PE11 setting for PWM1_BRAKE1*/

/* PE.12 MFP */
#define SYS_GPE_MFPH_PE12MFP_GPIO        (0x00UL<<SYS_GPE_MFPH_PE12MFP_Pos)/*!< GPE_MFPH PE12 setting for GPIO       */
#define SYS_GPE_MFPH_PE12MFP_EBI_ADR14   (0x02UL<<SYS_GPE_MFPH_PE12MFP_Pos)/*!< GPE_MFPH PE12 setting for EBI_ADR14  */
#define SYS_GPE_MFPH_PE12MFP_USCI1_CLK   (0x06UL<<SYS_GPE_MFPH_PE12MFP_Pos)/*!< GPE_MFPH PE12 setting for USCI1_CLK  */
#define SYS_GPE_MFPH_PE12MFP_UART1_nRTS  (0x08UL<<SYS_GPE_MFPH_PE12MFP_Pos)/*!< GPE_MFPH PE12 setting for UART1_nRTS */
#define SYS_GPE_MFPH_PE12MFP_PWM0_CH4    (0x0aUL<<SYS_GPE_MFPH_PE12MFP_Pos)/*!< GPE_MFPH PE12 setting for PWM0_CH4   */

/* PE.13 MFP */
#define SYS_GPE_MFPH_PE13MFP_GPIO        (0x00UL<<SYS_GPE_MFPH_PE13MFP_Pos)/*!< GPE_MFPH PE13 setting for GPIO       */
#define SYS_GPE_MFPH_PE13MFP_EBI_ADR15   (0x02UL<<SYS_GPE_MFPH_PE13MFP_Pos)/*!< GPE_MFPH PE13 setting for EBI_ADR15  */
#define SYS_GPE_MFPH_PE13MFP_I2C0_SCL    (0x04UL<<SYS_GPE_MFPH_PE13MFP_Pos)/*!< GPE_MFPH PE13 setting for I2C0_SCL   */
#define SYS_GPE_MFPH_PE13MFP_UART1_TXD   (0x08UL<<SYS_GPE_MFPH_PE13MFP_Pos)/*!< GPE_MFPH PE13 setting for UART1_TXD  */
#define SYS_GPE_MFPH_PE13MFP_PWM0_CH5    (0x0aUL<<SYS_GPE_MFPH_PE13MFP_Pos)/*!< GPE_MFPH PE13 setting for PWM0_CH5   */
#define SYS_GPE_MFPH_PE13MFP_PWM1_CH0    (0x0bUL<<SYS_GPE_MFPH_PE13MFP_Pos)/*!< GPE_MFPH PE13 setting for PWM1_CH0   */
#define SYS_GPE_MFPH_PE13MFP_BPWM1_CH5   (0x0cUL<<SYS_GPE_MFPH_PE13MFP_Pos)/*!< GPE_MFPH PE13 setting for BPWM1_CH5  */

/* PE.14 MFP */
#define SYS_GPE_MFPH_PE14MFP_GPIO        (0x00UL<<SYS_GPE_MFPH_PE14MFP_Pos)/*!< GPE_MFPH PE14 setting for GPIO       */
#define SYS_GPE_MFPH_PE14MFP_EBI_AD8     (0x02UL<<SYS_GPE_MFPH_PE14MFP_Pos)/*!< GPE_MFPH PE14 setting for EBI_AD8    */
#define SYS_GPE_MFPH_PE14MFP_UART2_TXD   (0x03UL<<SYS_GPE_MFPH_PE14MFP_Pos)/*!< GPE_MFPH PE14 setting for UART2_TXD  */
#define SYS_GPE_MFPH_PE14MFP_PSIO0_CH0   (0x0aUL<<SYS_GPE_MFPH_PE14MFP_Pos)/*!< GPE_MFPH PE14 setting for PSIO0_CH0  */

/* PE.15 MFP */
#define SYS_GPE_MFPH_PE15MFP_GPIO        (0x00UL<<SYS_GPE_MFPH_PE15MFP_Pos)/*!< GPE_MFPH PE15 setting for GPIO       */
#define SYS_GPE_MFPH_PE15MFP_EBI_AD9     (0x02UL<<SYS_GPE_MFPH_PE15MFP_Pos)/*!< GPE_MFPH PE15 setting for EBI_AD9    */
#define SYS_GPE_MFPH_PE15MFP_UART2_RXD   (0x03UL<<SYS_GPE_MFPH_PE15MFP_Pos)/*!< GPE_MFPH PE15 setting for UART2_RXD  */
#define SYS_GPE_MFPH_PE15MFP_PSIO0_CH1   (0x0aUL<<SYS_GPE_MFPH_PE15MFP_Pos)/*!< GPE_MFPH PE15 setting for PSIO0_CH1  */

/* PF.0 MFP */
#define SYS_GPF_MFPL_PF0MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF0MFP_Pos) /*!< GPF_MFPL PF0 setting for GPIO        */
#define SYS_GPF_MFPL_PF0MFP_UART1_TXD    (0x02UL<<SYS_GPF_MFPL_PF0MFP_Pos) /*!< GPF_MFPL PF0 setting for UART1_TXD   */
#define SYS_GPF_MFPL_PF0MFP_I2C1_SCL     (0x03UL<<SYS_GPF_MFPL_PF0MFP_Pos) /*!< GPF_MFPL PF0 setting for I2C1_SCL    */
#define SYS_GPF_MFPL_PF0MFP_BPWM1_CH0    (0x0cUL<<SYS_GPF_MFPL_PF0MFP_Pos) /*!< GPF_MFPL PF0 setting for BPWM1_CH0   */
#define SYS_GPF_MFPL_PF0MFP_ICE_DAT      (0x0eUL<<SYS_GPF_MFPL_PF0MFP_Pos) /*!< GPF_MFPL PF0 setting for ICE_DAT     */
#define SYS_GPF_MFPL_PF0MFP_UART0_TXD    (0x04UL<<SYS_GPF_MFPL_PF0MFP_Pos) /*!< GPF_MFPL PF0 setting for UART0_TXD   */

/* PF.1 MFP */
#define SYS_GPF_MFPL_PF1MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF1MFP_Pos) /*!< GPF_MFPL PF1 setting for GPIO        */
#define SYS_GPF_MFPL_PF1MFP_UART1_RXD    (0x02UL<<SYS_GPF_MFPL_PF1MFP_Pos) /*!< GPF_MFPL PF1 setting for UART1_RXD   */
#define SYS_GPF_MFPL_PF1MFP_I2C1_SDA     (0x03UL<<SYS_GPF_MFPL_PF1MFP_Pos) /*!< GPF_MFPL PF1 setting for I2C1_SDA    */
#define SYS_GPF_MFPL_PF1MFP_BPWM1_CH1    (0x0cUL<<SYS_GPF_MFPL_PF1MFP_Pos) /*!< GPF_MFPL PF1 setting for BPWM1_CH1   */
#define SYS_GPF_MFPL_PF1MFP_ICE_CLK      (0x0eUL<<SYS_GPF_MFPL_PF1MFP_Pos) /*!< GPF_MFPL PF1 setting for ICE_CLK     */
#define SYS_GPF_MFPL_PF1MFP_UART0_RXD    (0x04UL<<SYS_GPF_MFPL_PF1MFP_Pos) /*!< GPF_MFPL PF1 setting for UART0_RXD   */

/* PF.2 MFP */
#define SYS_GPF_MFPL_PF2MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF2MFP_Pos) /*!< GPF_MFPL PF2 setting for GPIO        */
#define SYS_GPF_MFPL_PF2MFP_EBI_nCS1     (0x02UL<<SYS_GPF_MFPL_PF2MFP_Pos) /*!< GPF_MFPL PF2 setting for EBI_nCS1    */
#define SYS_GPF_MFPL_PF2MFP_UART0_RXD    (0x03UL<<SYS_GPF_MFPL_PF2MFP_Pos) /*!< GPF_MFPL PF2 setting for UART0_RXD   */
#define SYS_GPF_MFPL_PF2MFP_I2C0_SDA     (0x04UL<<SYS_GPF_MFPL_PF2MFP_Pos) /*!< GPF_MFPL PF2 setting for I2C0_SDA    */
#define SYS_GPF_MFPL_PF2MFP_QSPI0_CLK    (0x05UL<<SYS_GPF_MFPL_PF2MFP_Pos) /*!< GPF_MFPL PF2 setting for QSPI0_CLK    */
#define SYS_GPF_MFPL_PF2MFP_XT1_OUT      (0x0aUL<<SYS_GPF_MFPL_PF2MFP_Pos) /*!< GPF_MFPL PF2 setting for XT1_OUT     */
#define SYS_GPF_MFPL_PF2MFP_BPWM1_CH1    (0x0bUL<<SYS_GPF_MFPL_PF2MFP_Pos) /*!< GPF_MFPL PF2 setting for BPWM1_CH1   */

/* PF.3 MFP */
#define SYS_GPF_MFPL_PF3MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF3MFP_Pos) /*!< GPF_MFPL PF3 setting for GPIO        */
#define SYS_GPF_MFPL_PF3MFP_EBI_nCS0     (0x02UL<<SYS_GPF_MFPL_PF3MFP_Pos) /*!< GPF_MFPL PF3 setting for EBI_nCS0    */
#define SYS_GPF_MFPL_PF3MFP_UART0_TXD    (0x03UL<<SYS_GPF_MFPL_PF3MFP_Pos) /*!< GPF_MFPL PF3 setting for UART0_TXD   */
#define SYS_GPF_MFPL_PF3MFP_I2C0_SCL     (0x04UL<<SYS_GPF_MFPL_PF3MFP_Pos) /*!< GPF_MFPL PF3 setting for I2C0_SCL    */
#define SYS_GPF_MFPL_PF3MFP_XT1_IN       (0x0aUL<<SYS_GPF_MFPL_PF3MFP_Pos) /*!< GPF_MFPL PF3 setting for XT1_IN      */
#define SYS_GPF_MFPL_PF3MFP_BPWM1_CH0    (0x0bUL<<SYS_GPF_MFPL_PF3MFP_Pos) /*!< GPF_MFPL PF3 setting for BPWM1_CH0   */

/* PF.4 MFP */
#define SYS_GPF_MFPL_PF4MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF4MFP_Pos) /*!< GPF_MFPL PF4 setting for GPIO        */
#define SYS_GPF_MFPL_PF4MFP_UART2_TXD    (0x02UL<<SYS_GPF_MFPL_PF4MFP_Pos) /*!< GPF_MFPL PF4 setting for UART2_TXD   */
#define SYS_GPF_MFPL_PF4MFP_UART2_nRTS   (0x04UL<<SYS_GPF_MFPL_PF4MFP_Pos) /*!< GPF_MFPL PF4 setting for UART2_nRTS  */
#define SYS_GPF_MFPL_PF4MFP_BPWM0_CH5    (0x08UL<<SYS_GPF_MFPL_PF4MFP_Pos) /*!< GPF_MFPL PF4 setting for BPWM0_CH5   */
#define SYS_GPF_MFPL_PF4MFP_X32_OUT      (0x0aUL<<SYS_GPF_MFPL_PF4MFP_Pos) /*!< GPF_MFPL PF4 setting for X32_OUT     */
#define SYS_GPF_MFPL_PF4MFP_PWM0_CH1     (0x07UL<<SYS_GPF_MFPL_PF4MFP_Pos) /*!< GPF_MFPL PF4 setting for PWM0_CH1   */

/* PF.5 MFP */
#define SYS_GPF_MFPL_PF5MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF5MFP_Pos) /*!< GPF_MFPL PF5 setting for GPIO        */
#define SYS_GPF_MFPL_PF5MFP_UART2_RXD    (0x02UL<<SYS_GPF_MFPL_PF5MFP_Pos) /*!< GPF_MFPL PF5 setting for UART2_RXD   */
#define SYS_GPF_MFPL_PF5MFP_UART2_nCTS   (0x04UL<<SYS_GPF_MFPL_PF5MFP_Pos) /*!< GPF_MFPL PF5 setting for UART2_nCTS  */
#define SYS_GPF_MFPL_PF5MFP_BPWM0_CH4    (0x08UL<<SYS_GPF_MFPL_PF5MFP_Pos) /*!< GPF_MFPL PF5 setting for BPWM0_CH4   */
#define SYS_GPF_MFPL_PF5MFP_X32_IN       (0x0aUL<<SYS_GPF_MFPL_PF5MFP_Pos) /*!< GPF_MFPL PF5 setting for X32_IN      */
#define SYS_GPF_MFPL_PF5MFP_EADC0_ST     (0x0bUL<<SYS_GPF_MFPL_PF5MFP_Pos) /*!< GPF_MFPL PF5 setting for EADC0_ST    */
#define SYS_GPF_MFPL_PF5MFP_PWM0_CH0     (0x07UL<<SYS_GPF_MFPL_PF5MFP_Pos) /*!< GPF_MFPL PF5 setting for PWM0_CH0   */

/* PF.6 MFP */
#define SYS_GPF_MFPL_PF6MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF6MFP_Pos) /*!< GPF_MFPL PF6 setting for GPIO        */
#define SYS_GPF_MFPL_PF6MFP_EBI_ADR19    (0x02UL<<SYS_GPF_MFPL_PF6MFP_Pos) /*!< GPF_MFPL PF6 setting for EBI_ADR19   */
#define SYS_GPF_MFPL_PF6MFP_SC0_CLK      (0x03UL<<SYS_GPF_MFPL_PF6MFP_Pos) /*!< GPF_MFPL PF6 setting for SC0_CLK     */
#define SYS_GPF_MFPL_PF6MFP_SPI0_MOSI    (0x05UL<<SYS_GPF_MFPL_PF6MFP_Pos) /*!< GPF_MFPL PF6 setting for SPI0_MOSI   */
#define SYS_GPF_MFPL_PF6MFP_EBI_nCS0     (0x07UL<<SYS_GPF_MFPL_PF6MFP_Pos) /*!< GPF_MFPL PF6 setting for EBI_nCS0    */
#define SYS_GPF_MFPL_PF6MFP_TAMPER0      (0x0aUL<<SYS_GPF_MFPL_PF6MFP_Pos) /*!< GPF_MFPL PF6 setting for TAMPER0     */

/* PF.7 MFP */
#define SYS_GPF_MFPL_PF7MFP_GPIO         (0x00UL<<SYS_GPF_MFPL_PF7MFP_Pos) /*!< GPF_MFPL PF7 setting for GPIO        */
#define SYS_GPF_MFPL_PF7MFP_EBI_ADR18    (0x02UL<<SYS_GPF_MFPL_PF7MFP_Pos) /*!< GPF_MFPL PF7 setting for EBI_ADR18   */
#define SYS_GPF_MFPL_PF7MFP_SC0_DAT      (0x03UL<<SYS_GPF_MFPL_PF7MFP_Pos) /*!< GPF_MFPL PF7 setting for SC0_DAT     */
#define SYS_GPF_MFPL_PF7MFP_SPI0_MISO    (0x05UL<<SYS_GPF_MFPL_PF7MFP_Pos) /*!< GPF_MFPL PF7 setting for SPI0_MISO   */

/* PF.14 MFP */
#define SYS_GPF_MFPH_PF14MFP_GPIO        (0x00UL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for GPIO       */
#define SYS_GPF_MFPH_PF14MFP_PWM1_BRAKE0 (0x09UL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for PWM1_BRAKE0    */
#define SYS_GPF_MFPH_PF14MFP_PWM0_BRAKE0 (0x0aUL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for PWM0_BRAKE0  */
#define SYS_GPF_MFPH_PF14MFP_PSIO0_CH3   (0x0bUL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for PSIO0_CH3  */
#define SYS_GPF_MFPH_PF14MFP_PWM0_CH4    (0x0cUL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for PWM0_CH4    */
#define SYS_GPF_MFPH_PF14MFP_CLKO        (0x0dUL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for CLKO  */
#define SYS_GPF_MFPH_PF14MFP_TM3         (0x0eUL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for TM3  */
#define SYS_GPF_MFPH_PF14MFP_INT5        (0x0fUL<<SYS_GPF_MFPH_PF14MFP_Pos)/*!< GPF_MFPH PF14 setting for INT5  */

/*@}*/ /* end of group SYS_EXPORTED_CONSTANTS */

/** @addtogroup SYS_EXPORTED_FUNCTIONS SYS Exported Functions
  @{
*/


/**
  * @brief      Clear Brown-out detector interrupt flag
  * @param      None
  * @return     None
  * @details    This macro clear Brown-out detector interrupt flag.
  * \hideinitializer
  */
#define SYS_CLEAR_BOD_INT_FLAG()        (SYS->BODCTL |= SYS_BODCTL_BODIF_Msk)

/**
  * @brief      Set Brown-out detector function to normal mode
  * @param      None
  * @return     None
  * @details    This macro set Brown-out detector to normal mode.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_CLEAR_BOD_LPM()             (SYS->BODCTL &= ~SYS_BODCTL_BODLPM_Msk)

/**
  * @brief      Disable Brown-out detector function
  * @param      None
  * @return     None
  * @details    This macro disable Brown-out detector function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_DISABLE_BOD()               (SYS->BODCTL &= ~SYS_BODCTL_BODEN_Msk)

/**
  * @brief      Enable Brown-out detector function
  * @param      None
  * @return     None
  * @details    This macro enable Brown-out detector function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_ENABLE_BOD()                (SYS->BODCTL |= SYS_BODCTL_BODEN_Msk)

/**
  * @brief      Get Brown-out detector interrupt flag
  * @param      None
  * @retval     0   Brown-out detect interrupt flag is not set.
  * @retval     >=1 Brown-out detect interrupt flag is set.
  * @details    This macro get Brown-out detector interrupt flag.
  * \hideinitializer
  */
#define SYS_GET_BOD_INT_FLAG()          (SYS->BODCTL & SYS_BODCTL_BODIF_Msk)

/**
  * @brief      Get Brown-out detector status
  * @param      None
  * @retval     0   System voltage is higher than BOD threshold voltage setting or BOD function is disabled.
  * @retval     >=1 System voltage is lower than BOD threshold voltage setting.
  * @details    This macro get Brown-out detector output status.
  *             If the BOD function is disabled, this function always return 0.
  * \hideinitializer
  */
#define SYS_GET_BOD_OUTPUT()            (SYS->BODCTL & SYS_BODCTL_BODOUT_Msk)

/**
  * @brief      Disable Brown-out detector interrupt function
  * @param      None
  * @return     None
  * @details    This macro disable Brown-out detector interrupt function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_DISABLE_BOD_RST()           (SYS->BODCTL &= ~SYS_BODCTL_BODRSTEN_Msk)

/**
  * @brief      Enable Brown-out detector reset function
  * @param      None
  * @return     None
  * @details    This macro enable Brown-out detect reset function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_ENABLE_BOD_RST()            (SYS->BODCTL |= SYS_BODCTL_BODRSTEN_Msk)

/**
  * @brief      Set Brown-out detector function low power mode
  * @param      None
  * @return     None
  * @details    This macro set Brown-out detector to low power mode.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_SET_BOD_LPM()               (SYS->BODCTL |= SYS_BODCTL_BODLPM_Msk)

/**
  * @brief      Set Brown-out detector voltage level
  * @param[in]  u32Level is Brown-out voltage level. Including :
  *             - \ref SYS_BODCTL_BODVL_4_4V
  *             - \ref SYS_BODCTL_BODVL_3_7V
  *             - \ref SYS_BODCTL_BODVL_3_0V
  *             - \ref SYS_BODCTL_BODVL_2_7V
  *             - \ref SYS_BODCTL_BODVL_2_4V
  *             - \ref SYS_BODCTL_BODVL_2_0V
  *             - \ref SYS_BODCTL_BODVL_1_8V
  * @return     None
  * @details    This macro set Brown-out detector voltage level.
  *             The write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_SET_BOD_LEVEL(u32Level)     (SYS->BODCTL = (SYS->BODCTL & ~SYS_BODCTL_BODVL_Msk) | (u32Level))

/**
  * @brief      Get reset source is from Brown-out detector reset
  * @param      None
  * @retval     0   Previous reset source is not from Brown-out detector reset
  * @retval     >=1 Previous reset source is from Brown-out detector reset
  * @details    This macro get previous reset source is from Brown-out detect reset or not.
  * \hideinitializer
  */
#define SYS_IS_BOD_RST()                (SYS->RSTSTS & SYS_RSTSTS_BODRF_Msk)

/**
  * @brief      Get reset source is from VBAT LVR
  * @param      None
  * @retval     0   Previous reset source is not from VBAT LVR
  * @retval     >=1 Previous reset source is from VBAT LVR
  * @details    This macro get previous reset source is from VBAT LVR or not.
  * \hideinitializer
  */
#define SYS_IS_VBATLVR_RST()                (SYS->RSTSTS & SYS_RSTSTS_VBATLVRF_Msk)

/**
  * @brief      Get reset source is from CPU Lockup reset
  * @param      None
  * @retval     0   Previous reset source is not from CPU Lockup reset
  * @retval     >=1 Previous reset source is from CPU Lockup reset
  * @details    This macro get previous reset source is from CPU Lockup reset or not.
  * \hideinitializer
  */
#define SYS_IS_CPULK_RST()                (SYS->RSTSTS & SYS_RSTSTS_CPULKRF_Msk)

/**
  * @brief      Get reset source is from CPU reset
  * @param      None
  * @retval     0   Previous reset source is not from CPU reset
  * @retval     >=1 Previous reset source is from CPU reset
  * @details    This macro get previous reset source is from CPU reset.
  * \hideinitializer
  */
#define SYS_IS_CPU_RST()                (SYS->RSTSTS & SYS_RSTSTS_CPURF_Msk)

/**
  * @brief      Get reset source is from LVR Reset
  * @param      None
  * @retval     0   Previous reset source is not from Low-Voltage-Reset
  * @retval     >=1 Previous reset source is from Low-Voltage-Reset
  * @details    This macro get previous reset source is from Low-Voltage-Reset.
  * \hideinitializer
  */
#define SYS_IS_LVR_RST()                (SYS->RSTSTS & SYS_RSTSTS_LVRF_Msk)

/**
  * @brief      Get reset source is from Power-on Reset
  * @param      None
  * @retval     0   Previous reset source is not from Power-on Reset
  * @retval     >=1 Previous reset source is from Power-on Reset
  * @details    This macro get previous reset source is from Power-on Reset.
  * \hideinitializer
  */
#define SYS_IS_POR_RST()                (SYS->RSTSTS & SYS_RSTSTS_PORF_Msk)

/**
  * @brief      Get reset source is from reset pin reset
  * @param      None
  * @retval     0   Previous reset source is not from reset pin reset
  * @retval     >=1 Previous reset source is from reset pin reset
  * @details    This macro get previous reset source is from reset pin reset.
  * \hideinitializer
  */
#define SYS_IS_RSTPIN_RST()             (SYS->RSTSTS & SYS_RSTSTS_PINRF_Msk)

/**
  * @brief      Get reset source is from system reset
  * @param      None
  * @retval     0   Previous reset source is not from system reset
  * @retval     >=1 Previous reset source is from system reset
  * @details    This macro get previous reset source is from system reset.
  * \hideinitializer
  */
#define SYS_IS_SYSTEM_RST()             (SYS->RSTSTS & SYS_RSTSTS_SYSRF_Msk)

/**
  * @brief      Get reset source is from window watch dog reset
  * @param      None
  * @retval     0   Previous reset source is not from window watch dog reset
  * @retval     >=1 Previous reset source is from window watch dog reset
  * @details    This macro get previous reset source is from window watch dog reset.
  * \hideinitializer
  */
#define SYS_IS_WDT_RST()                (SYS->RSTSTS & SYS_RSTSTS_WDTRF_Msk)

/**
  * @brief      Disable Low-Voltage-Reset function
  * @param      None
  * @return     None
  * @details    This macro disable Low-Voltage-Reset function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_DISABLE_LVR()               (SYS->BODCTL &= ~SYS_BODCTL_LVREN_Msk)

/**
  * @brief      Enable Low-Voltage-Reset function
  * @param      None
  * @return     None
  * @details    This macro enable Low-Voltage-Reset function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_ENABLE_LVR()                (SYS->BODCTL |= SYS_BODCTL_LVREN_Msk)

/**
  * @brief      Disable Power-on Reset function
  * @param      None
  * @return     None
  * @details    This macro disable Power-on Reset function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_DISABLE_POR()               (SYS->PORCTL0 = 0x5AA5)

/**
  * @brief      Enable Power-on Reset function
  * @param      None
  * @return     None
  * @details    This macro enable Power-on Reset function.
  *             The register write-protection function should be disabled before using this macro.
  * \hideinitializer
  */
#define SYS_ENABLE_POR()                (SYS->PORCTL0 = 0)

/**
  * @brief      Clear reset source flag
  * @param[in]  u32RstSrc is reset source. Including :
  *             - \ref SYS_RSTSTS_PORF_Msk
  *             - \ref SYS_RSTSTS_PINRF_Msk
  *             - \ref SYS_RSTSTS_WDTRF_Msk
  *             - \ref SYS_RSTSTS_LVRF_Msk
  *             - \ref SYS_RSTSTS_BODRF_Msk
  *             - \ref SYS_RSTSTS_SYSRF_Msk
  *             - \ref SYS_RSTSTS_CPURF_Msk
  *             - \ref SYS_RSTSTS_CPULKRF_Msk
  *             - \ref SYS_RSTSTS_VBATLVRF_Msk
  * @return     None
  * @details    This macro clear reset source flag.
  * \hideinitializer
  */
#define SYS_CLEAR_RST_SOURCE(u32RstSrc) ((SYS->RSTSTS) = (u32RstSrc) )


/*---------------------------------------------------------------------------------------------------------*/
/* static inline functions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/* Declare these inline functions here to avoid MISRA C 2004 rule 8.1 error */
__STATIC_INLINE void SYS_UnlockReg(void);
__STATIC_INLINE void SYS_LockReg(void);

/**
  * @brief      Disable register write-protection function
  * @param      None
  * @return     None
  * @details    This function disable register write-protection function.
  *             To unlock the protected register to allow write access.
  */
__STATIC_INLINE void SYS_UnlockReg(void)
{
    do
    {
        SYS->REGLCTL = 0x59UL;
        SYS->REGLCTL = 0x16UL;
        SYS->REGLCTL = 0x88UL;
    }
    while (SYS->REGLCTL == 0UL);
}

/**
  * @brief      Enable register write-protection function
  * @param      None
  * @return     None
  * @details    This function is used to enable register write-protection function.
  *             To lock the protected register to forbid write access.
  */
__STATIC_INLINE void SYS_LockReg(void)
{
    SYS->REGLCTL = 0x0UL;
}


void SYS_ClearResetSrc(uint32_t u32Src);
uint32_t SYS_GetBODStatus(void);
uint32_t SYS_GetResetSrc(void);
uint32_t SYS_IsRegLocked(void);
uint32_t SYS_ReadPDID(void);
void SYS_ResetChip(void);
void SYS_ResetCPU(void);
void SYS_ResetModule(uint32_t u32ModuleIndex);
void SYS_EnableBOD(int32_t i32Mode, uint32_t u32BODLevel);
void SYS_DisableBOD(void);
void SYS_SetPowerLevel(uint32_t u32PowerLevel);


/*@}*/ /* end of group SYS_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SYS_Driver */

/*@}*/ /* end of group Standard_Driver */


#ifdef __cplusplus
}
#endif

#endif /* __SYS_H__ */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/

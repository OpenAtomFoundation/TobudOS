/**
 * @file    wm_regs.h
 *
 * @brief   register & operation file
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_REGS_H
#define WM_REGS_H

#define __MPU_PRESENT 						1

#define __CM3_REV                 0x0201  /*!< Core Revision r2p1                           */
#define __NVIC_PRIO_BITS          4       /*!< W600 uses 4 Bits for the Priority Levels    */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used */

typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

/******  Venus specific Interrupt Numbers *********************************************************/
  SDIO_RX_IRQn                = 0,      /*!< SDIO Receiver Interrupt                              */
  SDIO_TX_IRQn                = 1,      /*!< SDIO Transmit Interrupt                              */
  SDIO_RX_CMD_IRQn            = 2,      /*!< SDIO Command Receiver Interrupt                      */
  SDIO_TX_CMD_IRQn            = 3,      /*!< SDIO Command Transmit Interrupt                      */
  MAC_IRQn                 	  = 4,      /*!< MAC Interrupt                                        */

  SEC_RX_IRQn                 = 6,      /*!< EXTI Line0 Interrupt                                 */
  SEC_TX_MNGT_IRQn            = 7,      /*!< EXTI Line1 Interrupt                                 */
  SEC_TX_DAT_IRQn             = 8,      /*!< EXTI Line2 Interrupt                                 */
  PMU_RTC_IRQn                = 9,      /*!< PMU RTC Interrupt                                    */
  PMU_SLP_INTVL_IRQn          = 10,     /*!< PMU Sleep Interval Interrupt                         */
  PMU_GPIO_WAKE_IRQn          = 11,     /*!< PMU GPIO Wake Interrupt                              */
  PMU_SDIO_WAKE_IRQn          = 12,     /*!< PMU SDIO Wake Interrupt                              */
  DMA_Channel0_IRQn           = 13,     /*!< DMA  Channel 0 global Interrupt                      */
  DMA_Channel1_IRQn           = 14,     /*!< DMA  Channel 1 global Interrupt                      */
  DMA_Channel2_IRQn           = 15,     /*!< DMA  Channel 2 global Interrupt                      */
  DMA_Channel3_IRQn           = 16,     /*!< DMA  Channel 3 global Interrupt                      */
  DMA_Channel4_7_IRQn         = 17,     /*!< DMA  Channel 4~7 global Interrupt                    */
  DMA_BRUST_IRQn              = 18,     /*!< DMA Burst global Interrupt                           */
  I2C_IRQn         			  		= 19,     /*!< I2C Interrupts                                       */
  ADC_IRQn        			  		= 20,     /*!< ADC Convert Interrupts                               */
  SPI_LS_IRQn                 = 21,     /*!< Low Speed SPI Interrupt                              */
  SPI_HS_IRQn                 = 22,     /*!< High Speed SPI Interrupt                             */
  UART0_IRQn                  = 23,     /*!< UART0 Interrupts                                     */
  UART1_IRQn               	  = 24,     /*!< UART1 Interrupt                                      */
  GPIO_IRQn                	  = 25,     /*!< GPIO Interrupt                                       */
  TIM1_IRQn           		  	= 26,     /*!< TIM1 Trigger and Commutation Interrupt               */
  TIM2_IRQn                	  = 27,     /*!< TIM1 Capture Compare Interrupt                       */
  TIM3_IRQn                   = 28,     /*!< TIM2 global Interrupt                                */
  TIM4_IRQn                   = 29,     /*!< TIM3 global Interrupt                                */
  TIM5_IRQn                	  = 31,     /*!< I2C1 Event Interrupt                                 */
  WDG_IRQn                	  = 32,     /*!< Watch Dog Interrupt                                  */
  PMU_IRQn                    = 33,     /*!< PMU Interrupt                                        */
  FLASH_IRQn                  = 34,     /*!< Flash Interrupt                                      */
  PWM_IRQn                 	  = 35,     /*!< PWM Interrupt                                        */
  I2S_IRQn              	  	= 36,     /*!< I2S  Interrupts                                      */
  RSA_IRQn            		  	=	38,   	
  CRYPTION_IRQn       		  	=	39, 
  GPIOB_IRQn									= 40,  
  UART2_IRQn				  				= 41,	

} IRQn_Type;

#include "core_cm3.h"

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

typedef volatile unsigned int TLS_REG;    /* Hardware register definition */

#define APB_CLK      (40000000) /* 40MHz */

#define DEVICE_BASE_ADDR    0x40000000


/***************************************************************
 * SDIO模块寄存器定义
 ***************************************************************/
#define HR_SDIO_BASE_ADDR     DEVICE_BASE_ADDR
#define HR_SDIO_CIS0                (HR_SDIO_BASE_ADDR + 0x008)
#define HR_SDIO_CIS1                (HR_SDIO_BASE_ADDR + 0x00C)
#define HR_SDIO_CSA                 (HR_SDIO_BASE_ADDR + 0x010)
#define HR_SDIO_READ                (HR_SDIO_BASE_ADDR + 0x014)
#define HR_SDIO_WRITE               (HR_SDIO_BASE_ADDR + 0x018)
#define HR_SDIO_INTEN               (HR_SDIO_BASE_ADDR + 0x030)
#define HR_SDIO_OCR                 (HR_SDIO_BASE_ADDR + 0x034)
#define HR_SDIO_CIA                 (HR_SDIO_BASE_ADDR + 0x024)
#define HR_SDIO_PROG                (HR_SDIO_BASE_ADDR + 0x028)

/***************************************************************
 * SPI模块寄存器定义
 ***************************************************************/
#define HR_HSPI_BASE_ADDR       (DEVICE_BASE_ADDR + 0x200)
#define HR_HSPI_CLEAR_FIFO          (HR_HSPI_BASE_ADDR)
#define HR_HSPI_SPI_CFG             (HR_HSPI_BASE_ADDR + 0x04)
#define HR_HSPI_MODE_CFG            (HR_HSPI_BASE_ADDR + 0x08)
#define HR_HSPI_INT_MASK            (HR_HSPI_BASE_ADDR + 0x0C)
#define HR_HSPI_INT_STTS            (HR_HSPI_BASE_ADDR + 0x10)
#define HR_HSPI_RXDAT_LEN           (HR_HSPI_BASE_ADDR + 0x18)

/***************************************************************
 * SDIO WRAPPER寄存器定义
 ***************************************************************/
#define HR_SDIO_WRAPPER_BASE_ADDR        (DEVICE_BASE_ADDR + 0x300)
#define HR_SDIO_INT_SRC             (HR_SDIO_WRAPPER_BASE_ADDR + 0x000)
#define HR_SDIO_INT_MASK            (HR_SDIO_WRAPPER_BASE_ADDR + 0x004)
#define HR_SDIO_UPCMDVALID          (HR_SDIO_WRAPPER_BASE_ADDR + 0x008)
#define HR_SDIO_DOWNCMDVALID        (HR_SDIO_WRAPPER_BASE_ADDR + 0x00C)
#define HR_SDIO_TXBD_LINKEN         (HR_SDIO_WRAPPER_BASE_ADDR + 0x010)
#define HR_SDIO_TXBD_ADDR           (HR_SDIO_WRAPPER_BASE_ADDR + 0x014)
#define HR_SDIO_TXEN                (HR_SDIO_WRAPPER_BASE_ADDR + 0x018)
#define HR_SDIO_TX_STTS             (HR_SDIO_WRAPPER_BASE_ADDR + 0x01C)
#define HR_SDIO_RXBD_LINKEN         (HR_SDIO_WRAPPER_BASE_ADDR + 0x020)
#define HR_SDIO_RXBD_ADDR           (HR_SDIO_WRAPPER_BASE_ADDR + 0x024)
#define HR_SDIO_RXEN                (HR_SDIO_WRAPPER_BASE_ADDR + 0x028)
#define HR_SDIO_RX_STTS             (HR_SDIO_WRAPPER_BASE_ADDR + 0x02C)
#define HR_SDIO_CMD_ADDR            (HR_SDIO_WRAPPER_BASE_ADDR + 0x030)
#define HR_SDIO_CMD_SIZE            (HR_SDIO_WRAPPER_BASE_ADDR + 0x034)

/* SDIO interrupt bit definition */
#define SDIO_WP_INT_SRC_CMD_DOWN         (1UL<<3)
#define SDIO_WP_INT_SRC_CMD_UP           (1UL<<2)
#define SDIO_WP_INT_SRC_DATA_DOWN        (1UL<<1)
#define SDIO_WP_INT_SRC_DATA_UP          (1UL<<0)


/***************************************************************
 * DMA模块寄存器定义
 ***************************************************************/
#define HR_DMA_BASE_ADDR                (DEVICE_BASE_ADDR + 0x400)
#define HR_DMA_INT_MASK                 (HR_DMA_BASE_ADDR + 0x0)
#define HR_DMA_INT_SRC                  (HR_DMA_BASE_ADDR + 0x4)
#define HR_DMA_CHNL_SEL					(HR_DMA_BASE_ADDR + 0x08)
#define HR_DMA_CHNL0_SRC_ADDR           (HR_DMA_BASE_ADDR + 0x10)
#define HR_DMA_CHNL0_DEST_ADDR          (HR_DMA_BASE_ADDR + 0x14)
#define HR_DMA_CHNL0_SRC_WRAP_ADDR      (HR_DMA_BASE_ADDR + 0x18)
#define HR_DMA_CHNL0_DEST_WRAP_ADDR     (HR_DMA_BASE_ADDR + 0x1C)
#define HR_DMA_CHNL0_WRAP_SIZE          (HR_DMA_BASE_ADDR + 0x20)
#define HR_DMA_CHNL0_CHNL_CTRL          (HR_DMA_BASE_ADDR + 0x24)
#define HR_DMA_CHNL0_DMA_MODE           (HR_DMA_BASE_ADDR + 0x28)
#define HR_DMA_CHNL0_DMA_CTRL           (HR_DMA_BASE_ADDR + 0x2C)
#define HR_DMA_CHNL0_DMA_STATUS         (HR_DMA_BASE_ADDR + 0x30)
#define HR_DMA_CHNL0_LINK_DEST_ADDR     (HR_DMA_BASE_ADDR + 0x34)
#define HR_DMA_CHNL0_CURRENT_DEST_ADDR  (HR_DMA_BASE_ADDR + 0x38)

#define DMA_STARTADDRESS        			(0x40000400)
#define DMA_INTMASK_REG      	  			(*(volatile unsigned int*)(DMA_STARTADDRESS+0x000))
#define DMA_INTSRC_REG      	  			(*(volatile unsigned int*)(DMA_STARTADDRESS+0x004))
#define DMA_CHNL_REG_BASE             		(DMA_STARTADDRESS+0x010)
#define DMA_SRCADDR_REG(ch)      	  		(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x00))
#define DMA_DESTADDR_REG(ch)      	  		(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x04))
#define DMA_SRCWRAPADDR_REG(ch)				(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x08))
#define DMA_DESTWRAPADDR_REG(ch)			(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x0C))
#define DMA_WRAPSIZE_REG(ch)				(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x10))
#define DMA_CHNLCTRL_REG(ch)      	  		(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x14))
#define DMA_MODE_REG(ch)      	  			(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x18))
#define DMA_CTRL_REG(ch)      	  			(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x1C))
#define DMA_STATUS_REG(ch)      	  		(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x20))
#define DMA_CURRSRCADDR_REG(ch)      		(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x24))
#define DMA_CURRDESTADDR_REG(ch)      		(*(volatile unsigned int*)(DMA_CHNL_REG_BASE + 0x30 * (ch /*- 1*/) +0x28))

#define DMA_CHNL_CTRL_CHNL_ON           (1<<0)
#define DMA_CHNL_CTRL_CHNL_OFF          (1<<1)

#define DMA_MODE_HARD_MODE                  (1<<0)
#define DMA_MODE_CHAIN_MODE                 (1<<1)

/***************************************************************
 * 节能模块寄存器定义
 ***************************************************************/
#define HR_PMU_BASE_ADDR                (DEVICE_BASE_ADDR + 0x600)
#define HR_PMU_PS_CR                    (HR_PMU_BASE_ADDR + 0x00)
#define HR_PMU_TIMER0                   (HR_PMU_BASE_ADDR + 0x04)
#define HR_PMU_TIMER1                   (HR_PMU_BASE_ADDR + 0x08)
#define HR_PMU_RTC_CTRL1               	(HR_PMU_BASE_ADDR + 0x0C)
#define HR_PMU_RTC_CTRL2                (HR_PMU_BASE_ADDR + 0x10)
#define HR_PMU_INTERRUPT_SRC            (HR_PMU_BASE_ADDR + 0x14)
#define HR_PMU_INTERRUPT_MASK          	(HR_PMU_BASE_ADDR + 0x18)
#define HR_PMU_WLAN_STTS                (HR_PMU_BASE_ADDR + 0x1C)

/* 节能寄存器位定义 */
#define PMU_WLAN_SLEEP_EN_BIT           (1UL << 0)

#define PMU_PRETBTT_PERIOD_RSV          (0xE0000000)
#define PMU_TBTT_TIMER_MASK             (0x3FFFF)
#define PMU_TBTT_TIMER_SHIFT            (11)
#define PMU_SLEEP_INTERVAL_MASK         (0x7FF)
#define PMU_SLEEP_INTERVAL_SHIFT        (0)

#define PMU_TBTT_OFFSET_MASK            (0x7FFF)
#define PMU_TBTT_OFFSET_SHIFT           (17)
#define PMU_PRETBTT_TIMER_MASK          (0x1FFFF)
#define PMU_PRETBTT_TIMER_SHIFT         (0)
#define PMU_PRETBTT_OFFSET_DEFAULT      (((30UL) << 17) | (300UL))

#define PMU_TIMER0_WAKEUP_EN            (1UL << 3)
#define PMU_TIMER1_WAKEUP_EN            (1UL << 2)
#define PMU_GPIO_WAKEUP_EN              (1UL << 1)
#define PMU_SDIO_WAKEUP_EN              (1UL << 0)

#define PMU_TIMER0_NTERRUPT_SRC         (1UL << 0)
#define PMU_TIMER1_NTERRUPT_SRC         (1UL << 1)
#define PMU_GPIO_NTERRUPT_SRC           (1UL << 2)
#define PMU_SDIO_NTERRUPT_SRC           (1UL << 3)

#define PMU_TIMER0_NTERRUPT_MASK        (1UL << 0)
#define PMU_TIMER1_NTERRUPT_MASK        (1UL << 1)
#define PMU_GPIO_NTERRUPT_MASK          (1UL << 2)
#define PMU_SDIO_NTERRUPT_MASK          (1UL << 3)

#define PMU_WLAN_RX_IDLE_BIT            (1UL << 1)
#define PMU_WLAN_TX_IDLE_BIT            (1UL << 0)

#define PMU_LDO_DAC28_BYPASS      (1UL<<0)
#define PMU_LDO_SW_BYPASS      (1UL<<1)
#define PMU_DLDO_BYPASS      (1UL<<2)
#define PMU_LDO_PRE_BYPASS      (1UL<<3)
#define PMU_LDO_ADDA_D_BYPASS      (1UL<<4)
#define PMU_LDO_ADDA_A_BYPASS      (1UL<<5)
#define PMU_LDO_UM_PA_BYPASS      (1UL<<6)
#define PMU_LDO_CHP_BYPASS      (1UL<<7)
#define PMU_LDO_DM_IF_BYPASS      (1UL<<8)
#define PMU_LDO_LNA_BYPASS      (1UL<<9)
#define PMU_LDO_VCO_BYPASS      (1UL<<10)
#define PMU_LDO_LO_BYPASS      (1UL<<11)
#define PMU_LDO_PLL_BYPASS      (1UL<<15)
#define PMU_LDO_WLAN_BYPASS      (1UL<<19)
#define PMU_LDO_BGR_BYPASS      (1UL<<20)
#define PMU_LDO_BB_BYPASS      (1UL<<21)

/***************************************************************
 * system clock 和bus clock寄存器定义
 ***************************************************************/
#define HR_CLK_BASE_ADDR        (DEVICE_BASE_ADDR + 0x700)
#define HR_CLK_SOFT_CLK_MASK    (HR_CLK_BASE_ADDR + 0x04) 
#define HR_CLK_BBP_CLT_CTRL     (HR_CLK_BASE_ADDR + 0x08)
#define HR_CLK_RST_CTL         		 (HR_CLK_BASE_ADDR + 0x0c)
#define HR_CLK_DIV_CTL			(HR_CLK_BASE_ADDR + 0x10)

#define HR_CLK_I2S_CTL		(HR_CLK_BASE_ADDR + 0x18)

#define I2S_CLK_CTL_EXT_Pos			(0)
#define I2S_CLK_CTL_MCLK_Pos		(1)
#define I2S_CLK_CTL_MCLKDIV_Pos	(2)
#define I2S_CLK_CTL_BCLKDIV_Pos	(8)

/***************************************************************
 * 内存模块寄存器定义
 ***************************************************************/
#define HR_MEM_BASE_ADDR        (DEVICE_BASE_ADDR + 0x800)
#define HR_MEM_TXBUF_BASE_ADDR      (HR_MEM_BASE_ADDR + 0x00)
#define HR_MEM_RXBUF_BASE_ADDR      (HR_MEM_BASE_ADDR + 0x04)
#define HR_MEM_BUF_NUM_CFG          (HR_MEM_BASE_ADDR + 0x08)
#define HR_MEM_BUF_SIZE_CFG         (HR_MEM_BASE_ADDR + 0x0C)
#define HR_MEM_AGGR_CFG             (HR_MEM_BASE_ADDR + 0x10)
#define HR_MEM_BUF_EN               (HR_MEM_BASE_ADDR + 0x14)


/***************************************************************
 * 中断模块寄存器定义
 ***************************************************************/
#define HR_VIC_BASE_ADDR   0xFFFFF000
#define HR_VIC_IRQ_STATUS           (HR_VIC_BASE_ADDR + 0x0)
#define HR_VIC_FIQ_STATUS           (HR_VIC_BASE_ADDR + 0x4)
#define HR_VIC_RAW_INTR             (HR_VIC_BASE_ADDR + 0x8)
#define HR_VIC_INT_SELECT           (HR_VIC_BASE_ADDR + 0xc)
#define HR_VIC_INT_ENABLE           (HR_VIC_BASE_ADDR + 0x10)
#define HR_VIC_INT_EN_CLR           (HR_VIC_BASE_ADDR + 0x14)
#define HR_VIC_SOFT_INT             (HR_VIC_BASE_ADDR + 0x18)
#define HR_VIC_SOFT_INT_CLR         (HR_VIC_BASE_ADDR + 0x1c)
#define HR_VIC_PROTECT              (HR_VIC_BASE_ADDR + 0x20)
#define HR_VIC_VECT_ADDR            (HR_VIC_BASE_ADDR + 0x30)
#define HR_VIC_DEF_VECT_ADDR        (HR_VIC_BASE_ADDR + 0x34)
#define HR_VIC_VECT_ENABLE          (HR_VIC_BASE_ADDR + 0x100)
#define HR_VIC_VECT_BASE_ADDR       (HR_VIC_BASE_ADDR + 0x104)
#define HR_VIC_INT_NUM              (HR_VIC_BASE_ADDR + 0x108)
#define HR_VIC_INT_PRIO0            (HR_VIC_BASE_ADDR + 0x10C)
#define HR_VIC_INT_PRIO1            (HR_VIC_BASE_ADDR + 0x110)
#define HR_VIC_INT_PRIO2            (HR_VIC_BASE_ADDR + 0x114)
#define HR_VIC_INT_PRIO3            (HR_VIC_BASE_ADDR + 0x118)


/* APB基地址*/
#define HR_APB_BASE_ADDR 0x40010000

/***************************************************************
 * I2C模块寄存器定义
 ***************************************************************/
#define HR_I2C_BASE_ADDR    (HR_APB_BASE_ADDR)
#define HR_I2C_PRER_LO              (HR_I2C_BASE_ADDR + 0x0)
#define HR_I2C_PRER_HI              (HR_I2C_BASE_ADDR + 0x04)
#define HR_I2C_CTRL                 (HR_I2C_BASE_ADDR + 0x08)
#define HR_I2C_TX_RX                   (HR_I2C_BASE_ADDR + 0x0C)
#define HR_I2C_CR_SR                   (HR_I2C_BASE_ADDR + 0x10)
#define HR_I2C_TXR               (HR_I2C_BASE_ADDR + 0x14)
#define HR_I2C_CR               (HR_I2C_BASE_ADDR + 0x18)


#define I2C_CTRL_INT_DISABLE	(0x1UL<<6)
#define I2C_CTRL_ENABLE			(0x1UL<<7)

#define I2C_CR_STA				(0x1UL<<7)
#define I2C_CR_STO				(0x1UL<<6)
#define I2C_CR_RD				(0x1UL<<5)
#define I2C_CR_WR				(0x1UL<<4)
#define I2C_CR_NAK				(0x1UL<<3)
#define I2C_CR_IACK				(0x1UL)

#define I2C_SR_NAK				(0x1UL<<7)
#define I2C_SR_BUSY					(0x1UL<<6)
#define I2C_SR_TIP				(0x1UL<<1)
#define I2C_SR_IF				(0x1UL)


/***************************************************************
 * SD ADC模块寄存器定义
 ***************************************************************/
#define HR_SD_ADC_BASE_ADDR         (HR_APB_BASE_ADDR + 0x200)
#define HR_SD_ADC_RESULT_REG     	((HR_SD_ADC_BASE_ADDR+0x000))
#define HR_SD_ADC_CONFIG_REG     	((HR_SD_ADC_BASE_ADDR+0x004))
#define HR_SD_ADC_CTRL_REG	       ((HR_SD_ADC_BASE_ADDR+0x008))
#define HR_SD_ADC_INT_MASK_REG	((HR_SD_ADC_BASE_ADDR+0x00C))
#define HR_SD_ADC_INT_SOURCE_REG	((HR_SD_ADC_BASE_ADDR+0x010))


/***************************************************************
 * SPI模块寄存器定义
 ***************************************************************/
#define HR_SPI_BASE_ADDR            (HR_APB_BASE_ADDR + 0x400)
#define HR_SPI_CHCFG_REG		   		(HR_SPI_BASE_ADDR + 0x000)
#define HR_SPI_SPICFG_REG		   		(HR_SPI_BASE_ADDR + 0x004)
#define HR_SPI_CLKCFG_REG		   		(HR_SPI_BASE_ADDR + 0x008)
#define HR_SPI_MODECFG_REG		   	(HR_SPI_BASE_ADDR + 0x00C)
#define HR_SPI_INT_MASK_REG		   		(HR_SPI_BASE_ADDR + 0x010)
#define HR_SPI_INT_STATUS_REG		   		(HR_SPI_BASE_ADDR + 0x014)
#define HR_SPI_STATUS_REG		   	(HR_SPI_BASE_ADDR + 0x018)
#define HR_SPI_TIMEOUT_REG		   	(HR_SPI_BASE_ADDR + 0x01C)
#define HR_SPI_TXDATA_REG		   		(HR_SPI_BASE_ADDR + 0x020)
#define HR_SPI_TRANSLEN_REG		   	(HR_SPI_BASE_ADDR + 0x024)
#define HR_SPI_SLV_XMIT_LEN_REG		   	(HR_SPI_BASE_ADDR + 0x028)
#define HR_SPI_RXDATA_REG		   		(HR_SPI_BASE_ADDR + 0x030)


#define SPIM_STARTADDRESS 		  		(HR_APB_BASE_ADDR + 0x400)
#define SPIM_CHCFG_REG		   			(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x000))
#define SPIM_SPICFG_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x004))
#define SPIM_CLKCFG_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x008))
#define SPIM_MODECFG_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x00C))
#define SPIM_INTEN_REG		   			(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x010))
#define SPIM_INTSRC_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x014))
#define SPIM_SPISTATUS_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x018))
#define SPIM_SPITIMEOUT_REG		   	(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x01C))
#define SPIM_TXDATA_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x020))
#define SPIM_TRANSLEN_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x024))
#define SPIM_RXDATA_REG		   		(*(volatile unsigned int*)(SPIM_STARTADDRESS+0x030))


/*RSA寄存器*/
#define RSA_BASE_ADDRESS       0x40002200
#define  RSAXBUF               (*((volatile unsigned long *) (RSA_BASE_ADDRESS + 0x0 )))
#define  RSAYBUF               (*((volatile unsigned long *) (RSA_BASE_ADDRESS + 0x100 )))
#define  RSAMBUF               (*((volatile unsigned long *) (RSA_BASE_ADDRESS + 0x200 )))
#define  RSADBUF               (*((volatile unsigned long *) (RSA_BASE_ADDRESS + 0x300 )))
#define  RSACON                (*((volatile unsigned long *) (RSA_BASE_ADDRESS + 0x400 )))
#define  RSAMC                 (*((volatile unsigned long *) (RSA_BASE_ADDRESS + 0x404 )))
#define  RSAN                  (*((volatile unsigned long *) (RSA_BASE_ADDRESS + 0x408 )))



/* Bits Definitions of "CH_CFG" */
#define SPI_RX_INVALID_BITS(n)      ((n) << 23)
#define SPI_CLEAR_FIFOS      (1U << 22)
#define SPI_CONTINUE_MODE      (1U << 21)
#define SPI_RX_CHANNEL_ON      (1U << 20)
#define SPI_RX_CHANNEL_OFF      (0U << 20)
#define SPI_TX_CHANNEL_ON      (1U << 19)
#define SPI_TX_CHANNEL_OFF      (0U << 19)
#define SPI_VALID_CLKS_NUM(n)      ((n) << 3)
#define SPI_CS_LOW      (0U << 2)
#define SPI_CS_HIGH      (1U << 2)
#define SPI_FORCE_SPI_CS_OUT      (1U << 1)
#define SPI_START      (1U << 0)

/* Bits Definitions of "SPI_CFG" */
#define SPI_FRAME_FORMAT_MOTO      (0U << 17)
#define SPI_FRAME_FORMAT_TI      (1U << 17)
#define SPI_FRAME_FORMAT_MICROWAIRE      (2U << 17)
#define SPI_TX_PIN_ALWS_DRIVER      (1U << 16)
#define SPI_CS_HOLD(n)      ((n) << 12)
#define CS_HOLD_1_CLKS     0
#define CS_HOLD_2_CLKS     1
#define CS_HOLD_4_CLKS     2
#define CS_HOLD_8_CLKS     3
#define CS_HOLD_16_CLKS     4
#define CS_HOLD_32_CLKS     5
#define CS_HOLD_64_CLKS     6
#define CS_HOLD_127_CLKS     7
#define SPI_CS_SETUP(n)      ((n) << 9)
#define CS_SETUP_1_CLKS     0
#define CS_SETUP_2_CLKS     1
#define CS_SETUP_4_CLKS     2
#define CS_SETUP_8_CLKS     3
#define CS_SETUP_16_CLKS     4
#define CS_SETUP_32_CLKS     5
#define CS_SETUP_64_CLKS     6
#define CS_SETUP_127_CLKS     7
#define SPI_DATA_OUT_DELAY(n)      ((n) << 7)
#define DATA_OUT_DELAY_1_CLKS      0
#define DATA_OUT_DELAY_2_CLKS      1
#define DATA_OUT_DELAY_3_CLKS      2
#define DATA_OUT_DELAY_0_CLKS      3
#define SPI_FRAME_DELAY(n)      ((n) << 4)
#define FRAME_DELAY_1_CLKS     0
#define FRAME_DELAY_2_CLKS     1
#define FRAME_DELAY_4_CLKS     2
#define FRAME_DELAY_8_CLKS     3
#define FRAME_DELAY_16_CLKS     4
#define FRAME_DELAY_32_CLKS     5
#define FRAME_DELAY_64_CLKS     6
#define FRAME_DELAY_127_CLKS     7
#define SPI_LITTLE_ENDIAN      (0 << 3)
#define SPI_BIG_ENDIAN      (1 << 3)
#define SPI_SET_MASTER_SLAVE(mode)      ((mode) << 2)
#define SPI_MASTER      1
#define SPI_SLAVE      0
#define SPI_SET_CPHA(n)      ((n) << 1)
#define SPI_SET_CPOL(n)      ((n) << 0)

/* Bits Definitions of "CLK_CFG" */
#define SPI_GET_SCLK_DIVIDER(clk)      ((APB_CLK)/((clk) * 2) - 1)/*In HZ*/
#define SPI_SCLK_DIVIDER(n)      ((n) << 0)

/* Bits Definitions of "MODE_CFG" */
#define SPI_RX_TRIGGER_LEVEL(n)      ((n) << 6)
#define SPI_TX_TRIGGER_LEVEL(n)      ((n) << 2)
#define SPI_RX_DMA_ON      (1 << 1)
#define SPI_RX_DMA_OFF      (0 << 1)
#define SPI_TX_DMA_ON      (1 << 0)
#define SPI_TX_DMA_OFF      (0 << 0)

/* Bits Definitions of SPI interrupt register(SPI_INT_MASK and SPI_INT_SOURCE) */
#define SPI_INT_MASK_ALL      0xffU
#define SPI_INT_CLEAR_ALL      0xffU
#define SPI_INT_TIMEOUT      (1U << 7)
#define SPI_INT_TRANSFER_DONE      (1U << 6)
#define SPI_INT_RX_OVERRUN      (1U << 5)
#define SPI_INT_RX_UNDERRUN      (1U << 4)
#define SPI_INT_TX_OVERRUN      (1U << 3)
#define SPI_INT_TX_UNDERRUN      (1U << 2)
#define SPI_INT_RX_FIFO_RDY      (1U << 1)
#define SPI_INT_TX_FIFO_RDY      (1U << 0)

/* Bits Definitions of "SPI_STATUS" */
#define SPI_IS_BUSY(value)      ((((value) & (1 << 12)) != 0) ? 1 : 0)
#define SPI_GET_RX_FIFO_CNT(value)      (((value) & 0xFC0) >> 6)
#define SPI_GET_TX_FIFO_CNT(value)      ((value) & 0x3F)

/* Bits Definitions of "SPI_TIMEOUT" */
#define SPI_TIMER_EN      (1U << 31)
#define SPI_TIME_OUT(n)      (((((n) * (APB_CLK)) / 1000) & ~(0x01U << 31)) << 0)

/***************************************************************
 * UART寄存器定义
 ***************************************************************/
#define HR_UART0_BASE_ADDR          (HR_APB_BASE_ADDR + 0x800)
#define HR_UART1_BASE_ADDR          (HR_APB_BASE_ADDR + 0xA00)
#define HR_UART2_BASE_ADDR			(HR_APB_BASE_ADDR + 0x1C00)

#define HR_UART0_LINE_CTRL          (HR_UART0_BASE_ADDR + 0x0)
#define HR_UART0_FLOW_CTRL          (HR_UART0_BASE_ADDR + 0x04)
#define HR_UART0_DMA_CTRL           (HR_UART0_BASE_ADDR + 0x08)
#define HR_UART0_FIFO_CTRL          (HR_UART0_BASE_ADDR + 0x0C)
#define HR_UART0_BAUD_RATE_CTRL     (HR_UART0_BASE_ADDR + 0x10)
#define HR_UART0_INT_MASK           (HR_UART0_BASE_ADDR + 0x14)
#define HR_UART0_INT_SRC            (HR_UART0_BASE_ADDR + 0x18)
#define HR_UART0_FIFO_STATUS        (HR_UART0_BASE_ADDR + 0x1C)
#define HR_UART0_TX_WIN             (HR_UART0_BASE_ADDR + 0x20)
#define HR_UART0_RX_WIN             (HR_UART0_BASE_ADDR + 0x30)

#define HR_UART1_LINE_CTRL          (HR_UART1_BASE_ADDR + 0x0)
#define HR_UART1_FLOW_CTRL          (HR_UART1_BASE_ADDR + 0x04)
#define HR_UART1_DMA_CTRL           (HR_UART1_BASE_ADDR + 0x08)
#define HR_UART1_FIFO_CTRL          (HR_UART1_BASE_ADDR + 0x0C)
#define HR_UART1_BAUD_RATE_CTRL     (HR_UART1_BASE_ADDR + 0x10)
#define HR_UART1_INT_MASK           (HR_UART1_BASE_ADDR + 0x14)
#define HR_UART1_INT_SRC            (HR_UART1_BASE_ADDR + 0x18)
#define HR_UART1_FIFO_STATUS        (HR_UART1_BASE_ADDR + 0x1C)
#define HR_UART1_TX_WIN             (HR_UART1_BASE_ADDR + 0x20)
#define HR_UART1_RX_WIN             (HR_UART1_BASE_ADDR + 0x30)

#define HR_UART2_LINE_CTRL          (HR_UART2_BASE_ADDR + 0x0)
#define HR_UART2_FLOW_CTRL          (HR_UART2_BASE_ADDR + 0x04)
#define HR_UART2_DMA_CTRL           (HR_UART2_BASE_ADDR + 0x08)
#define HR_UART2_FIFO_CTRL          (HR_UART2_BASE_ADDR + 0x0C)
#define HR_UART2_BAUD_RATE_CTRL     (HR_UART2_BASE_ADDR + 0x10)
#define HR_UART2_INT_MASK           (HR_UART2_BASE_ADDR + 0x14)
#define HR_UART2_INT_SRC            (HR_UART2_BASE_ADDR + 0x18)
#define HR_UART2_FIFO_STATUS        (HR_UART2_BASE_ADDR + 0x1C)
#define HR_UART2_TX_WIN             (HR_UART2_BASE_ADDR + 0x20)
#define HR_UART2_RX_WIN             (HR_UART2_BASE_ADDR + 0x30)
#define HR_UART2_GUARD_TIME			(HR_UART2_BASE_ADDR + 0x40)
#define HR_UART2_WAIT_TIME			(HR_UART2_BASE_ADDR + 0x44)
/*
 * Line Control Register bits definition
 * ULCON0 ULCON1 Register
 */
/* bit 0-1 : Word length */
#define ULCON_WL5               0x00
#define ULCON_WL6               0x01
#define ULCON_WL7               0x02
#define ULCON_WL8               0x03
#define ULCON_WL_MASK           0x03
/* bit 2 : Number of stop bits */
#define ULCON_STOP_2            0x04  /* 2 stop bit */
/* bit 3-4 : Parity mode */
#define ULCON_PMD_MASK          0x18
#define ULCON_PMD_EN            0x08  /* no parity */
#define ULCON_PMD_ODD           0x18  /* odd parity */
#define ULCON_PMD_EVEN          0x08  /* even parity */
/* bit 6 uart tx enable */
#define ULCON_TX_EN             0x40
#define ULCON_RX_EN             0x80
/* bit 6 : Infra-red mode */
#define ULCON_INFRA_RED         0x80

/*
 * auto flow control register bits definition
 */
#define UFC_ENABLE                0x01
#define UFC_SW_RTS_SET            0x02
#define UFC_RTS_TRIGGER_LVL_4_BYTE  (0<<2)
#define UFC_RTS_TRIGGER_LVL_8_BYTE  (1<<2)
#define UFC_RTS_TRIGGER_LVL_12_BYTE  (2<<2)
#define UFC_RTS_TRIGGER_LVL_16_BYTE  (3<<2)
#define UFC_RTS_TRIGGER_LVL_20_BYTE  (4<<2)
#define UFC_RTS_TRIGGER_LVL_24_BYTE  (5<<2)
#define UFC_RTS_TRIGGER_LVL_28_BYTE  (6<<2)
#define UFC_RTS_TRIGGER_LVL_31_BYTE  (7<<2)

#define UFC_TX_FIFO_RESET       0x01
#define UFC_RX_FIFO_RESET       0x02
#define UFC_TX_FIFO_LVL_ZERO    (0<<2)
#define UFC_TX_FIFO_LVL_4_BYTE  (1<<2)
#define UFC_TX_FIFO_LVL_8_BYTE  (2<<2)
#define UFC_TX_FIFO_LVL_16_BYTE (3<<2)
#define UFC_RX_FIFO_LVL_ZERO    (0<<4)
#define UFC_RX_FIFO_LVL_4_BYTE  (1<<4)
#define UFC_RX_FIFO_LVL_8_BYTE  (2<<4)
#define UFC_RX_FIFO_LVL_16_BYTE (3<<4)

/* dma control */
#define UDMA_RX_FIFO_TIMEOUT    (1<<2)
#define UDMA_RX_FIFO_TIMEOUT_SHIFT  (3)

/*
 * uart interrupt source register bits definition
 */
#define UIS_TX_FIFO_EMPTY       (1UL<<0)
#define UIS_TX_FIFO             (1UL<<1)
#define UIS_RX_FIFO             (1UL<<2)
#define UIS_RX_FIFO_TIMEOUT     (1UL<<3)
#define UIS_CTS_CHNG            (1UL<<4)
#define UIS_BREAK               (1UL<<5)
#define UIS_FRM_ERR             (1UL<<6)
#define UIS_PARITY_ERR          (1UL<<7)
#define UIS_OVERRUN             (1UL<<8)

/*
 * fifo status register bits definition
 */
/* bit 0-5 : tx fifo count */
#define UFS_TX_FIFO_CNT_MASK      0x3F
/* bit 6-11 : rx fifo count */
#define UFS_RX_FIFO_CNT_MASK      0x3C0
#define UFS_CST_STS               (1UL<<12)

/***************************************************************
 * GPIO寄存器定义
 ***************************************************************/
#define HR_GPIO_BASE_ADDR           (HR_APB_BASE_ADDR + 0x0C00)
#define HR_GPIO_DATA                (HR_GPIO_BASE_ADDR + 0x0)
#define HR_GPIO_DATA_EN             (HR_GPIO_BASE_ADDR + 0x04)
#define HR_GPIO_DIR                 (HR_GPIO_BASE_ADDR + 0x08)
#define HR_GPIO_PULL_EN             (HR_GPIO_BASE_ADDR + 0x0C)
#define HR_GPIO_AF_SEL              (HR_GPIO_BASE_ADDR + 0x10)
#define HR_GPIO_AF_S1               (HR_GPIO_BASE_ADDR + 0x14)
#define HR_GPIO_AF_S0               (HR_GPIO_BASE_ADDR + 0x18)
#define HR_GPIO_IS                  (HR_GPIO_BASE_ADDR + 0x20)
#define HR_GPIO_IBE                 (HR_GPIO_BASE_ADDR + 0x24)
#define HR_GPIO_IEV                 (HR_GPIO_BASE_ADDR + 0x28)
#define HR_GPIO_IE                  (HR_GPIO_BASE_ADDR + 0x2c)
#define HR_GPIO_RIS                 (HR_GPIO_BASE_ADDR + 0x30)
#define HR_GPIO_MIS                 (HR_GPIO_BASE_ADDR + 0x34)
#define HR_GPIO_IC                  (HR_GPIO_BASE_ADDR + 0x38)

#define HR_GPIOA_BASE_ADDR           (HR_APB_BASE_ADDR + 0x0C00)
#define HR_GPIOA_DATA                (HR_GPIOA_BASE_ADDR + 0x0)
#define HR_GPIOA_DATA_EN             (HR_GPIOA_BASE_ADDR + 0x04)
#define HR_GPIOA_DATA_DIR			 (HR_GPIOA_BASE_ADDR + 0x08)
#define HR_GPIOA_DATA_PULLEN		 (HR_GPIOA_BASE_ADDR + 0x0c)
#define HR_GPIOA_AFSEL				 (HR_GPIOA_BASE_ADDR + 0x10)
#define HR_GPIOA_AFS1				 (HR_GPIOA_BASE_ADDR + 0x14)
#define HR_GPIOA_AFS0				 (HR_GPIOA_BASE_ADDR + 0x18)
#define HR_GPIOA_IS					 (HR_GPIOA_BASE_ADDR + 0x20)
#define HR_GPIOA_IBE				 (HR_GPIOA_BASE_ADDR + 0x24)
#define HR_GPIOA_IEV				 (HR_GPIOA_BASE_ADDR + 0x28)
#define HR_GPIOA_IE					 (HR_GPIOA_BASE_ADDR + 0x2C)
#define HR_GPIOA_RIS				 (HR_GPIOA_BASE_ADDR + 0x30)
#define HR_GPIOA_MIS				 (HR_GPIOA_BASE_ADDR + 0x34)
#define HR_GPIOA_IC					 (HR_GPIOA_BASE_ADDR + 0x38)

#define HR_GPIOB_BASE_ADDR           (HR_APB_BASE_ADDR + 0x1200)
#define HR_GPIOB_DATA                (HR_GPIOB_BASE_ADDR + 0x0)
#define HR_GPIOB_DATA_EN             (HR_GPIOB_BASE_ADDR + 0x04)
#define HR_GPIOB_DATA_DIR			 (HR_GPIOB_BASE_ADDR + 0x08)
#define HR_GPIOB_DATA_PULLEN		 (HR_GPIOB_BASE_ADDR + 0x0c)
#define HR_GPIOB_AFSEL				 (HR_GPIOB_BASE_ADDR + 0x10)
#define HR_GPIOB_AFS1				 (HR_GPIOB_BASE_ADDR + 0x14)
#define HR_GPIOB_AFS0				 (HR_GPIOB_BASE_ADDR + 0x18)
#define HR_GPIOB_IS					 (HR_GPIOB_BASE_ADDR + 0x20)
#define HR_GPIOB_IBE				 (HR_GPIOB_BASE_ADDR + 0x24)
#define HR_GPIOB_IEV				 (HR_GPIOB_BASE_ADDR + 0x28)
#define HR_GPIOB_IE					 (HR_GPIOB_BASE_ADDR + 0x2C)
#define HR_GPIOB_RIS				 (HR_GPIOB_BASE_ADDR + 0x30)
#define HR_GPIOB_MIS				 (HR_GPIOB_BASE_ADDR + 0x34)
#define HR_GPIOB_IC					 (HR_GPIOB_BASE_ADDR + 0x38)


#define IO_PA_BASE_ADDR			(HR_APB_BASE_ADDR + 0x0C00)
#define IO_PA_DATA				(*(volatile unsigned int*)(IO_PA_BASE_ADDR + 0x0))
#define IO_PA_EN				(*(volatile unsigned int*)(IO_PA_BASE_ADDR + 0x4))
#define IO_PA_DIR				(*(volatile unsigned int*)(IO_PA_BASE_ADDR + 0x8))
#define IO_PA_REN				(*(volatile unsigned int*)(IO_PA_BASE_ADDR + 0xC))
#define IO_PA_AFSEL				(*(volatile unsigned int*)(IO_PA_BASE_ADDR + 0x10))
#define IO_PA_AFS1				(*(volatile unsigned int*)(IO_PA_BASE_ADDR + 0x14))
#define IO_PA_AFS0				(*(volatile unsigned int*)(IO_PA_BASE_ADDR + 0x18))

/***************************************************************
 * TIMER寄存器定义
 ***************************************************************/
#define HR_TIMER_BASE_ADDR          (HR_APB_BASE_ADDR + 0x0E00)
#define HR_TIMER_CFG                (HR_TIMER_BASE_ADDR + 0x0)
#define HR_TIMER0_5_CSR				(HR_TIMER_BASE_ADDR + 0x04)
#define HR_TIMER0_PRD				(HR_TIMER_BASE_ADDR + 0x08)
#define HR_TIMER1_PRD				(HR_TIMER_BASE_ADDR + 0x0C)
#define HR_TIMER2_PRD				(HR_TIMER_BASE_ADDR + 0x10)
#define HR_TIMER3_PRD				(HR_TIMER_BASE_ADDR + 0x14)
#define HR_TIMER4_PRD				(HR_TIMER_BASE_ADDR + 0x18)
#define HR_TIMER5_PRD				(HR_TIMER_BASE_ADDR + 0x1C)
/* timer csr bits defintion */
#define TLS_TIMER_MS_UNIT(n)               (1UL<<(0+5*n))
#define TLS_TIMER_ONE_TIME(n)              (1UL<<(1+5*n))
#define TLS_TIMER_EN(n)                    (1UL<<(2+5*n))
#define TLS_TIMER_INT_EN(n)                (1UL<<(3+5*n))
#define TLS_TIMER_INT_CLR(n)            (1UL<<(4+5*n))

/***************************************************************
 * WATCH DOG寄存器定义
 ***************************************************************/
#define HR_WDG_BASE_ADDR            (HR_APB_BASE_ADDR + 0x1000)
#define HR_WDG_LOAD_VALUE           (HR_WDG_BASE_ADDR + 0x00)
#define HR_WDG_CUR_VALUE            (HR_WDG_BASE_ADDR + 0x04)
#define HR_WDG_CTRL                 (HR_WDG_BASE_ADDR + 0x08)
#define HR_WDG_INT_CLR             	(HR_WDG_BASE_ADDR + 0x0C)
#define HR_WDG_INT_SRC              (HR_WDG_BASE_ADDR + 0x10)
#define HR_WDG_INT_MIS              (HR_WDG_BASE_ADDR + 0x14)
#define HR_WDG_LOCK                 (HR_WDG_BASE_ADDR + 0x40)

/***************************************************************
 * IOCTL寄存器定义
 ***************************************************************/
#define HR_IOCTL_BASE_ADDR          (HR_APB_BASE_ADDR + 0x1200)
#define HR_IOCTL_DL_MODE            (HR_IOCTL_BASE_ADDR + 0x0)
#define HR_IOCTL_GP_SDIO_I2C        (HR_IOCTL_BASE_ADDR + 0x04)
#define HR_IOCTL_GP_JTAG            (HR_IOCTL_BASE_ADDR + 0x08)
#define HR_IOCTL_GP_SPI             (HR_IOCTL_BASE_ADDR + 0x0C)
#define HR_IOCTL_PE                 (HR_IOCTL_BASE_ADDR + 0x10)


#define IOCTRL_STARTADDRESS	(HR_APB_BASE_ADDR + 0x1200)
#define IOCTRL_DOWNLOAD_MODE_REG	 (*(volatile unsigned *)(IOCTRL_STARTADDRESS+0x000))
#define IOCTRL_PIN_CONFIG_REG1      (*(volatile unsigned *)(IOCTRL_STARTADDRESS+0x004))
#define IOCTRL_PIN_CONFIG_REG2      (*(volatile unsigned *)(IOCTRL_STARTADDRESS+0x008))
#define IOCTRL_PIN_CONFIG_REG3      (*(volatile unsigned *)(IOCTRL_STARTADDRESS+0x00C))


/** bit field of the lcd gate control in CLK gating register */
#define HR_CLK_LCD_GATE_Pos						(10)

/***********************************************************//**
 * LCD寄存器定义
 ***************************************************************/

#define HR_LCD_REG_BASE     		 (HR_APB_BASE_ADDR + 0x1600) //(0x4001 1600)
#define HR_LCD_CR				      	 (HR_LCD_REG_BASE+0x000)
#define HR_LCD_FRAME_CNT     		 (HR_LCD_REG_BASE+0x004)
#define HR_LCD_COM0_1_SEG     	 (HR_LCD_REG_BASE+0x008)
#define HR_LCD_COM2_3_SEG   		 (HR_LCD_REG_BASE+0x00C)
#define HR_LCD_COM4_5_SEG   		 (HR_LCD_REG_BASE+0x010)
#define HR_LCD_COM6_7_SEG   		 (HR_LCD_REG_BASE+0x014)
#define HR_LCD_IO_EN						 (HR_LCD_REG_BASE+0x018)

#define LCD_CR_EN_Pos							(8)
#define LCD_CR_PD_Pos							(9)

#define LCD_CR_MODE_Pos						(11)
#define LCD_CR_MODE_MASK					(1UL<<11)
#define LCD_CR_MODE_4x20					(1UL<<11)
#define LCD_CR_MODE_8X16					(0UL<<11)

#define LCD_EN								(1UL<<8)

#define LCD_BIAS_MASK						(3UL<<6)
#define LCD_BIAS_MASK_Pos					(6)
#define LCD_BIAS_ONEFOURTH					(0UL<<6)
#define LCD_BIAS_ONEHALF					(1UL<<6)
#define LCD_BIAS_ONETHIRD					(2UL<<6)
#define LCD_BIAS_STATIC						(3UL<<6)

#define LCD_VLCD_MASK						(7UL<<3)
#define LCD_VLCD_MASK_Pos					(3)
#define LCD_VLCD_27							(0UL<<3)
#define LCD_VLCD_29							(1UL<<3)
#define LCD_VLCD_31							(2UL<<3)
#define LCD_VLCD_33							(3UL<<3)


#define LCD_DUTY_MASK						(7UL<<0)
#define LCD_DUTY_MASK_Pos					(0)
#define LCD_DUTY_STATIC						(0UL<<0)
#define LCD_DUTY_ONEHALF					(1UL<<0)
#define LCD_DUTY_ONETHIRD					(2UL<<0)
#define LCD_DUTY_ONEFOURTH					(3UL<<0)
#define LCD_DUTY_ONEFIFTH					(4UL<<0)
#define LCD_DUTY_ONESIXTH					(5UL<<0)
#define LCD_DUTY_ONESEVENTH					(6UL<<0)
#define LCD_DUTY_ONEEIGHTH					(7UL<<0)


/***********************************************************//**
 * I2S寄存器定义
 ***************************************************************/
#define HR_CLK_I2S_GATE_Pos					(10)


#define HR_I2S_REG_BASE					 (HR_APB_BASE_ADDR+0x1A00)
#define HR_I2S_CTRL			      	 (HR_I2S_REG_BASE+0x000)
#define HR_I2S_INT_MASK     		 (HR_I2S_REG_BASE+0x004)
#define HR_I2S_INT_SRC		     	 (HR_I2S_REG_BASE+0x008)
#define HR_I2S_STATUS			   		 (HR_I2S_REG_BASE+0x00C)
#define HR_I2S_TX					  		 (HR_I2S_REG_BASE+0x010)
#define HR_I2S_RX					  		 (HR_I2S_REG_BASE+0x014)

/******************************************************************************/
/*                                                                            */
/*                        PWM                                                 */
/*                                                                            */
/******************************************************************************/
#define HR_PWM_REG_BASE                 (HR_APB_BASE_ADDR+0x1800)
#define HR_PWM_CLKDIV01                 (HR_PWM_REG_BASE+0x0000)
#define HR_PWM_CLKDIV23                 (HR_PWM_REG_BASE+0x0004)
#define HR_PWM_CTL                      (HR_PWM_REG_BASE+0x0008)
#define HR_PWM_PERIOD                   (HR_PWM_REG_BASE+0x000C)
#define HR_PWM_PNUM                     (HR_PWM_REG_BASE+0x0010)
#define HR_PWM_CMPDAT                   (HR_PWM_REG_BASE+0x0014)
#define HR_PWM_DTCTL                    (HR_PWM_REG_BASE+0x0018)
#define HR_PWM_INTEN                    (HR_PWM_REG_BASE+0x001C)
#define HR_PWM_INTSTS                   (HR_PWM_REG_BASE+0x0020)
#define HR_PWM_CAPDAT                   (HR_PWM_REG_BASE+0x0024)
#define HR_PWM_BRKCTL                   (HR_PWM_REG_BASE+0x0028)
#define HR_PWM_CH4_REG1                 (HR_PWM_REG_BASE+0x002C)
#define HR_PWM_CH4_REG2                 (HR_PWM_REG_BASE+0x0030)
#define HR_PWM_CAP2DAT                  (HR_PWM_REG_BASE+0x0034)
#define HR_PWM_CAP2CTL                  (HR_PWM_REG_BASE+0x0038)

/*  EFUSE Register */
#define HR_EFUSE_REG_BASE      (0x0f001600)
#define HR_EFUSE_WDATA_PORT      (HR_EFUSE_REG_BASE+0x000)
#define HR_EFUSE_PGM_CMD      (HR_EFUSE_REG_BASE+0x004)
#define HR_EFUSE_PGM_STTS      (HR_EFUSE_REG_BASE+0x008)
#define HR_EFUSE_CLEAR      (HR_EFUSE_REG_BASE+0x00C)
#define HR_EFUSE_VDDQ_SETUP      (HR_EFUSE_REG_BASE+0x010)

/* PGM_CMD define */
#define EFUSE_PGM_START      (1UL)
#define EFUSE1_SELECT      (1UL << 1)
#define EFUSE2_SELECT      (1UL << 2)

/* PGM STTS define */
#define EFUSE_PGM_DONE	      (1UL)
#define EFUSE_FIFO_FREE      (1UL << 1)

/* CLEAR_EFUSE */
#define EFUSE_RESET      (1UL)

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup REG_APIs REG APIs
 * @brief Register operate APIs
 */

/**
 * @addtogroup REG_APIs
 * @{
 */

/**
 * @brief          This function is used to write value to register or memory
 *
 * @param[in]      reg    address to be written
 * @param[in]      val    value to write
 *
 * @return         None
 *
 * @note           None
 */
static __inline void tls_reg_write32(unsigned int reg, unsigned int val)
{
    *(TLS_REG *)reg = val;
}


/**
 * @brief          This function is used to read value from register or memory
 *
 * @param[in]      reg    address to read
 *
 * @retval         val    read from register or memory
 *
 * @note           None
 */
static __inline unsigned int tls_reg_read32(unsigned int reg)
{
    unsigned int val = *(TLS_REG *)reg;
    return val;
}


/**
 * @brief          This function is used to write specified bit through
                   bit-band alias region
 *
 * @param[in]      addr    32-bit aligned byte address where the bit exists.
 * @param[in]      bit     Bit position.
 * @param[in]      val     The value that the bit is set to, 0 or 1.
 *
 * @return         None
 *
 * @note           Only when bit-band mechnism is supported, you can use it
 */
static __inline void tls_bitband_write(volatile unsigned int addr, unsigned int bit, unsigned int val)
{
	unsigned int temp;

	temp = (((unsigned int)(addr) & 0xF0000000) + 0x02000000) \
                          +((((unsigned int)(addr)&0xFFFFF)<<5)\
                          +(  (unsigned int)(bit)<<2));

	*((volatile unsigned int * )temp) = val;
}

/**
 * @brief          This function is used to read a single bit from address
                   by bit-band alias region
 *
 * @param[in]      addr    32-bit aligned byte address where the bit exists
 * @param[in]      bit     Bit position
 *
 * @retval         The value of the requested bit
 *
 * @note           Only when bit-band mechnism is supported, you can use it
 */
static __inline unsigned int tls_bitband_read(volatile unsigned int addr, unsigned int bit)
{
	unsigned int temp;

	temp = (((unsigned int)(addr) & 0xF0000000) + 0x02000000) \
                          +((((unsigned int)(addr)&0xFFFFF)<<5)\
                          +(  (unsigned int)(bit)<<2));

	return *((volatile unsigned int *)temp);
}


/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_REGS_H */


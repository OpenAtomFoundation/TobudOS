/*!
    \file  gd32f1x0_rcu.h
    \brief definitions for the RCU
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_RCU_H
#define GD32F1X0_RCU_H

#include "gd32f1x0.h"

/* RCU definitions */
#define RCU                         RCU_BASE

/* registers definitions */
#define RCU_CTL0                    REG32(RCU + 0x00U)        /*!< control register 0 */
#define RCU_CFG0                    REG32(RCU + 0x04U)        /*!< configuration register 0 */
#define RCU_INT                     REG32(RCU + 0x08U)        /*!< interrupt register */
#define RCU_APB2RST                 REG32(RCU + 0x0CU)        /*!< APB2 reset register */
#define RCU_APB1RST                 REG32(RCU + 0x10U)        /*!< APB1 reset register */
#define RCU_AHBEN                   REG32(RCU + 0x14U)        /*!< AHB enable register */
#define RCU_APB2EN                  REG32(RCU + 0x18U)        /*!< APB2 enable register */
#define RCU_APB1EN                  REG32(RCU + 0x1CU)        /*!< APB1 enable register  */
#define RCU_BDCTL                   REG32(RCU + 0x20U)        /*!< backup domain control register */
#define RCU_RSTSCK                  REG32(RCU + 0x24U)        /*!< reset source /clock register */
#define RCU_AHBRST                  REG32(RCU + 0x28U)        /*!< AHB reset register */
#define RCU_CFG1                    REG32(RCU + 0x2CU)        /*!< configuration register 1 */
#define RCU_CFG2                    REG32(RCU + 0x30U)        /*!< configuration register 2 */
#define RCU_CTL1                    REG32(RCU + 0x34U)        /*!< control register 1 */
#ifdef GD32F170_190
#define RCU_CFG3                    REG32(RCU + 0x80U)        /*!< configuration register 3 */
#endif /* GD32F170_190 */
#define RCU_ADDAPB1EN               REG32(RCU + 0xF8U)        /*!< APB1 additional enable register */
#define RCU_ADDAPB1RST              REG32(RCU + 0xFCU)        /*!< APB1 additional reset register */
#define RCU_VKEY                    REG32(RCU + 0x100U)       /*!< voltage key register */
#define RCU_DSV                     REG32(RCU + 0x134U)       /*!< deep-sleep mode voltage register */
#ifdef GD32F130_150
#define RCU_PDVSEL                  REG32(RCU + 0x138U)       /*!< power down voltage select register */
#endif /* GD32F130_150 */

/* bits definitions */
/* RCU_CTL0 */
#define RCU_CTL0_IRC8MEN            BIT(0)                    /*!< internal high speed oscillator enable */
#define RCU_CTL0_IRC8MSTB           BIT(1)                    /*!< IRC8M high speed internal oscillator stabilization flag */
#define RCU_CTL0_IRC8MADJ           BITS(3,7)                 /*!< high speed internal oscillator clock trim adjust value */
#define RCU_CTL0_IRC8MCALIB         BITS(8,15)                /*!< high speed internal oscillator calibration value register */
#define RCU_CTL0_HXTALEN            BIT(16)                   /*!< external high speed oscillator enable */
#define RCU_CTL0_HXTALSTB           BIT(17)                   /*!< external crystal oscillator clock stabilization flag */
#define RCU_CTL0_HXTALBPS           BIT(18)                   /*!< external crystal oscillator clock bypass mode enable */
#define RCU_CTL0_CKMEN              BIT(19)                   /*!< HXTAL clock monitor enable */
#define RCU_CTL0_PLLEN              BIT(24)                   /*!< PLL enable */
#define RCU_CTL0_PLLSTB             BIT(25)                   /*!< PLL clock stabilization flag */

/* RCU_CFG0 */
#define RCU_CFG0_SCS                BITS(0,1)                 /*!< system clock switch */
#define RCU_CFG0_SCSS               BITS(2,3)                 /*!< system clock switch status */
#define RCU_CFG0_AHBPSC             BITS(4,7)                 /*!< AHB prescaler selection */
#define RCU_CFG0_APB1PSC            BITS(8,10)                /*!< APB1 prescaler selection */
#define RCU_CFG0_APB2PSC            BITS(11,13)               /*!< APB2 prescaler selection */
#define RCU_CFG0_ADCPSC             BITS(14,15)               /*!< ADC clock prescaler selection */
#define RCU_CFG0_PLLSEL             BIT(16)                   /*!< PLL clock source selection */
#define RCU_CFG0_PLLPREDV           BIT(17)                   /*!< HXTAL divider for PLL source clock selection */
#define RCU_CFG0_PLLMF              (BIT(27) | BITS(18,21))   /*!< PLL multiply factor */
#ifdef GD32F130_150
#define RCU_CFG0_USBDPSC            BITS(22,23)               /*!< USBD clock prescaler selection */
#define RCU_CFG0_CKOUTSEL           BITS(24,26)               /*!< CK_OUT clock source selection */
#elif defined (GD32F170_190)
#define RCU_CFG0_CKOUT0SEL          BITS(24,26)               /*!< CK_OUT0 clock source selection */
#endif /* GD32F130_150 */
#define RCU_CFG0_PLLMF4             BIT(27)                   /*!< bit 4 of PLLMF */
#ifdef GD32F130_150
#define RCU_CFG0_CKOUTDIV           BITS(28,30)               /*!< CK_OUT divider which the CK_OUT frequency can be reduced */
#elif defined (GD32F170_190)
#define RCU_CFG0_CKOUT0DIV          BITS(28,30)               /*!< CK_OUT0 divider which the CK_OUT0 frequency can be reduced */
#endif /* GD32F130_150 */
#define RCU_CFG0_PLLDV              BIT(31)                   /*!< CK_PLL divide by 1 or 2 for CK_OUT(GD32F130_150) or CK_OUT0(GD32F170_190) */

/* RCU_INT */
#define RCU_INT_IRC40KSTBIF         BIT(0)                    /*!< IRC40K stabilization interrupt flag */
#define RCU_INT_LXTALSTBIF          BIT(1)                    /*!< LXTAL stabilization interrupt flag */
#define RCU_INT_IRC8MSTBIF          BIT(2)                    /*!< IRC8M stabilization interrupt flag */
#define RCU_INT_HXTALSTBIF          BIT(3)                    /*!< HXTAL stabilization interrupt flag */
#define RCU_INT_PLLSTBIF            BIT(4)                    /*!< PLL stabilization interrupt flag */
#ifdef GD32F130_150
#define RCU_INT_IRC14MSTBIF         BIT(5)                    /*!< IRC14M stabilization interrupt flag */
#elif defined (GD32F170_190)
#define RCU_INT_IRC28MSTBIF         BIT(5)                    /*!< IRC28M stabilization interrupt flag */
#endif /* GD32F130_150 */
#define RCU_INT_CKMIF               BIT(7)                    /*!< HXTAL clock stuck interrupt flag */
#define RCU_INT_IRC40KSTBIE         BIT(8)                    /*!< IRC40K stabilization interrupt enable */
#define RCU_INT_LXTALSTBIE          BIT(9)                    /*!< LXTAL stabilization interrupt enable */
#define RCU_INT_IRC8MSTBIE          BIT(10)                   /*!< IRC8M stabilization interrupt enable */
#define RCU_INT_HXTALSTBIE          BIT(11)                   /*!< HXTAL stabilization interrupt enable */
#define RCU_INT_PLLSTBIE            BIT(12)                   /*!< PLL stabilization interrupt enable */
#ifdef GD32F130_150
#define RCU_INT_IRC14MSTBIE         BIT(13)                   /*!< IRC14M stabilization interrupt enable */
#elif defined (GD32F170_190)
#define RCU_INT_IRC28MSTBIE         BIT(13)                   /*!< IRC28M stabilization interrupt enable */
#endif /* GD32F130_150 */
#define RCU_INT_IRC40KSTBIC         BIT(16)                   /*!< IRC40K stabilization interrupt clear */
#define RCU_INT_LXTALSTBIC          BIT(17)                   /*!< LXTAL stabilization interrupt clear */
#define RCU_INT_IRC8MSTBIC          BIT(18)                   /*!< IRC8M stabilization interrupt clear */
#define RCU_INT_HXTALSTBIC          BIT(19)                   /*!< HXTAL stabilization interrupt clear */
#define RCU_INT_PLLSTBIC            BIT(20)                   /*!< PLL stabilization interrupt clear */
#ifdef GD32F130_150
#define RCU_INT_IRC14MSTBIC         BIT(21)                   /*!< IRC14M stabilization interrupt clear */
#elif defined (GD32F170_190)
#define RCU_INT_IRC28MSTBIC         BIT(21)                   /*!< IRC28M stabilization interrupt clear */
#endif /* GD32F130_150 */
#define RCU_INT_CKMIC               BIT(23)                   /*!< HXTAL clock stuck interrupt clear */

/* RCU_APB2RST */
#define RCU_APB2RST_CFGRST          BIT(0)                    /*!< system configuration reset */
#define RCU_APB2RST_ADCRST          BIT(9)                    /*!< ADC reset */
#define RCU_APB2RST_TIMER0RST       BIT(11)                   /*!< TIMER0 reset */
#define RCU_APB2RST_SPI0RST         BIT(12)                   /*!< SPI0 reset */
#define RCU_APB2RST_USART0RST       BIT(14)                   /*!< USART0 reset */
#define RCU_APB2RST_TIMER14RST      BIT(16)                   /*!< TIMER14 reset */
#define RCU_APB2RST_TIMER15RST      BIT(17)                   /*!< TIMER15 reset */
#define RCU_APB2RST_TIMER16RST      BIT(18)                   /*!< TIMER16 reset */

/* RCU_APB1RST */
#define RCU_APB1RST_TIMER1RST       BIT(0)                    /*!< TIMER1 timer reset */
#define RCU_APB1RST_TIMER2RST       BIT(1)                    /*!< TIMER2 timer reset */
#define RCU_APB1RST_TIMER5RST       BIT(4)                    /*!< TIMER5 timer reset */
#define RCU_APB1RST_TIMER13RST      BIT(8)                    /*!< TIMER13 timer reset */
#ifdef GD32F170_190
#define RCU_APB1RST_SLCDRST         BIT(9)                    /*!< SLCD reset */
#endif /* GD32F170_190 */
#define RCU_APB1RST_WWDGTRST        BIT(11)                   /*!< window watchdog timer reset */
#define RCU_APB1RST_SPI1RST         BIT(14)                   /*!< SPI1 reset */
#define RCU_APB1RST_SPI2RST         BIT(15)                   /*!< SPI2 reset */
#define RCU_APB1RST_USART1RST       BIT(17)                   /*!< USART1 reset */
#define RCU_APB1RST_I2C0RST         BIT(21)                   /*!< I2C0 reset */
#define RCU_APB1RST_I2C1RST         BIT(22)                   /*!< I2C1 reset */
#ifdef GD32F130_150
#define RCU_APB1RST_USBDRST         BIT(23)                   /*!< USBD reset */
#endif /* GD32F130_150 */
#ifdef GD32F170_190
#define RCU_APB1RST_CAN0RST         BIT(25)                   /*!< CAN0 reset */
#define RCU_APB1RST_CAN1RST         BIT(26)                   /*!< CAN1 reset */
#endif /* GD32F170_190 */
#define RCU_APB1RST_PMURST          BIT(28)                   /*!< power control reset */
#define RCU_APB1RST_DACRST          BIT(29)                   /*!< DAC reset */
#define RCU_APB1RST_CECRST          BIT(30)                   /*!< HDMI CEC reset */
#ifdef GD32F170_190
#define RCU_APB1RST_OPAIVREFRST     BIT(31)                   /*!< OPA and IVREF reset */
#endif /* GD32F170_190 */

/* RCU_AHBEN */
#define RCU_AHBEN_DMAEN             BIT(0)                    /*!< DMA clock enable */
#define RCU_AHBEN_SRAMSPEN          BIT(2)                    /*!< SRAM interface clock enable */
#define RCU_AHBEN_FMCSPEN           BIT(4)                    /*!< FMC clock enable */
#define RCU_AHBEN_CRCEN             BIT(6)                    /*!< CRC clock enable */
#define RCU_AHBEN_PAEN              BIT(17)                   /*!< GPIO port A clock enable */
#define RCU_AHBEN_PBEN              BIT(18)                   /*!< GPIO port B clock enable */
#define RCU_AHBEN_PCEN              BIT(19)                   /*!< GPIO port C clock enable */
#define RCU_AHBEN_PDEN              BIT(20)                   /*!< GPIO port D clock enable */
#define RCU_AHBEN_PFEN              BIT(22)                   /*!< GPIO port F clock enable */
#define RCU_AHBEN_TSIEN             BIT(24)                   /*!< TSI clock enable */

/* RCU_APB2EN */
#define RCU_APB2EN_CFGCMPEN         BIT(0)                    /*!< system configuration and comparator clock enable */
#define RCU_APB2EN_ADCEN            BIT(9)                    /*!< ADC interface clock enable */
#define RCU_APB2EN_TIMER0EN         BIT(11)                   /*!< TIMER0 timer clock enable */
#define RCU_APB2EN_SPI0EN           BIT(12)                   /*!< SPI0 clock enable */
#define RCU_APB2EN_USART0EN         BIT(14)                   /*!< USART0 clock enable */
#define RCU_APB2EN_TIMER14EN        BIT(16)                   /*!< TIMER14 timer clock enable */
#define RCU_APB2EN_TIMER15EN        BIT(17)                   /*!< TIMER15 timer clock enable */
#define RCU_APB2EN_TIMER16EN        BIT(18)                   /*!< TIMER16 timer clock enable */

/* RCU_APB1EN */
#define RCU_APB1EN_TIMER1EN         BIT(0)                    /*!< TIMER1 timer clock enable */
#define RCU_APB1EN_TIMER2EN         BIT(1)                    /*!< TIMER2 timer clock enable */
#define RCU_APB1EN_TIMER5EN         BIT(4)                    /*!< TIMER5 timer clock enable */
#define RCU_APB1EN_TIMER13EN        BIT(8)                    /*!< TIMER13 timer clock enable */
#ifdef GD32F170_190
#define RCU_APB1EN_SLCDEN           BIT(9)                    /*!< SLCD clock enable */
#endif /* GD32F170_190 */
#define RCU_APB1EN_WWDGTEN          BIT(11)                   /*!< window watchdog timer clock enable */
#define RCU_APB1EN_SPI1EN           BIT(14)                   /*!< SPI1 clock enable */
#define RCU_APB1EN_SPI2EN           BIT(15)                   /*!< SPI2 clock enable */
#define RCU_APB1EN_USART1EN         BIT(17)                   /*!< USART1 clock enable */
#define RCU_APB1EN_I2C0EN           BIT(21)                   /*!< I2C0 clock enable */
#define RCU_APB1EN_I2C1EN           BIT(22)                   /*!< I2C1 clock enable */
#ifdef GD32F130_150
#define RCU_APB1EN_USBDEN           BIT(23)                   /*!< USBD clock enable */
#endif /* GD32F130_150 */
#ifdef GD32F170_190
#define RCU_APB1EN_CAN0EN           BIT(25)                   /*!< CAN0 clock enable */
#define RCU_APB1EN_CAN1EN           BIT(26)                   /*!< CAN1 clock enable */
#endif /* GD32F170_190 */
#define RCU_APB1EN_PMUEN            BIT(28)                   /*!< power interface clock enable */
#define RCU_APB1EN_DACEN            BIT(29)                   /*!< DAC interface clock enable */
#define RCU_APB1EN_CECEN            BIT(30)                   /*!< HDMI CEC interface clock enable */
#ifdef GD32F170_190
#define RCU_APB1EN_OPAIVREFEN       BIT(31)                   /*!< OPA and IVREF clock enable */
#endif /* GD32F170_190 */

/* RCU_BDCTL */
#define RCU_BDCTL_LXTALEN           BIT(0)                    /*!< LXTAL enable */
#define RCU_BDCTL_LXTALSTB          BIT(1)                    /*!< external low-speed oscillator stabilization */
#define RCU_BDCTL_LXTALBPS          BIT(2)                    /*!< LXTAL bypass mode enable */
#define RCU_BDCTL_LXTALDRI          BITS(3,4)                 /*!< LXTAL drive capability */
#define RCU_BDCTL_RTCSRC            BITS(8,9)                 /*!< RTC clock entry selection */
#define RCU_BDCTL_RTCEN             BIT(15)                   /*!< RTC clock enable */
#define RCU_BDCTL_BKPRST            BIT(16)                   /*!< Backup domain reset */

/* RCU_RSTSCK */
#define RCU_RSTSCK_IRC40KEN         BIT(0)                    /*!< IRC40K enable */
#define RCU_RSTSCK_IRC40KSTB        BIT(1)                    /*!< IRC40K stabilization */
#define RCU_RSTSCK_V12RSTF          BIT(23)                   /*!< V12 domain Power reset flag */
#define RCU_RSTSCK_RSTFC            BIT(24)                   /*!< reset flag clear */
#define RCU_RSTSCK_OBLRSTF          BIT(25)                   /*!< option byte loader reset flag */
#define RCU_RSTSCK_EPRSTF           BIT(26)                   /*!< external pin reset flag */
#define RCU_RSTSCK_PORRSTF          BIT(27)                   /*!< power reset flag */
#define RCU_RSTSCK_SWRSTF           BIT(28)                   /*!< software reset flag */
#define RCU_RSTSCK_FWDGTRSTF        BIT(29)                   /*!< free watchdog timer reset flag */
#define RCU_RSTSCK_WWDGTRSTF        BIT(30)                   /*!< window watchdog timer reset flag */
#define RCU_RSTSCK_LPRSTF           BIT(31)                   /*!< low-power reset flag */

/* RCU_AHBRST */
#define RCU_AHBRST_PARST            BIT(17)                   /*!< GPIO port A reset */
#define RCU_AHBRST_PBRST            BIT(18)                   /*!< GPIO port B reset */
#define RCU_AHBRST_PCRST            BIT(19)                   /*!< GPIO port C reset */
#define RCU_AHBRST_PDRST            BIT(20)                   /*!< GPIO port D reset */
#define RCU_AHBRST_PFRST            BIT(22)                   /*!< GPIO port F reset */
#define RCU_AHBRST_TSIRST           BIT(24)                   /*!< TSI unit reset */

/* RCU_CFG1 */
#define RCU_CFG1_HXTALPREDV         BITS(0,3)                 /*!< CK_HXTAL divider previous PLL */

/* RCU_CFG2 */
#define RCU_CFG2_USART0SEL          BITS(0,1)                 /*!< CK_USART0 clock source selection */
#define RCU_CFG2_CECSEL             BIT(6)                    /*!< CK_CEC clock source selection */
#define RCU_CFG2_ADCSEL             BIT(8)                    /*!< CK_ADC clock source selection */
#ifdef GD32F170_190
#define RCU_CFG2_IRC28MDIV          BIT(16)                   /*!< CK_IRC28M divider 2 or not */
#endif /* GD32F170_190 */

/* RCU_CTL1 */
#ifdef GD32F130_150
#define RCU_CTL1_IRC14MEN           BIT(0)                    /*!< IRC14M internal 14M RC oscillator enable */
#define RCU_CTL1_IRC14MSTB          BIT(1)                    /*!< IRC14M internal 14M RC oscillator stabilization flag */
#define RCU_CTL1_IRC14MADJ          BITS(3,7)                 /*!< internal 14M RC oscillator clock trim adjust value */
#define RCU_CTL1_IRC14MCALIB        BITS(8,15)                /*!< internal 14M RC oscillator calibration value register */
#elif defined (GD32F170_190)
#define RCU_CTL1_IRC28MEN           BIT(0)                    /*!< IRC28M internal 28M RC oscillator enable */
#define RCU_CTL1_IRC28MSTB          BIT(1)                    /*!< IRC28M internal 28M RC oscillator stabilization flag */
#define RCU_CTL1_IRC28MADJ          BITS(3,7)                 /*!< internal 28M RC oscillator clock trim adjust value */
#define RCU_CTL1_IRC28MCALIB        BITS(8,15)                /*!< internal 28M RC oscillator calibration value register */
#endif /* GD32F130_150 */

#ifdef GD32F170_190
/* RCU_CFG3 */
#define RCU_CFG3_CKOUT1SRC          BITS(0,2)                 /*!< CKOUT1 clock source selection */
#define RCU_CFG3_CKOUT1DIV          BITS(8,13)                /*!< CK_OUT1 divider which the CK_OUT1 frequency can be reduced */
#endif /* GD32F170_190 */

/* RCU_ADDAPB1EN */
#define RCU_ADDAPB1EN_I2C2EN        BIT(0)                    /*!< I2C2 unit clock enable */

/* RCU_ADDAPB1RST */
#define RCU_ADDAPB1RST_I2C2RST      BIT(0)                    /*!< I2C2 unit reset */

/* RCU_VKEY */
#define RCU_VKEY_KEY                BITS(0,31)                /*!< key of RCU_PDVSEL and RCU_DSV register */

/* RCU_DSV */
#define RCU_DSV_DSLPVS              BITS(0,2)                 /*!< deep-sleep mode voltage select */

#ifdef GD32F130_150
/* RCU_PDVSEL */
#define RCU_PDVSEL_PDRVS            BIT(0)                    /*!< power down voltage select */
#endif /* GD32F130_150 */

/* constants definitions */
/* define the peripheral clock enable bit position and its register index offset */
#define RCU_REGIDX_BIT(regidx, bitpos)      (((uint32_t)(regidx) << 6) | (bitpos))
#define RCU_REG_VAL(periph)                 (REG32(RCU + ((uint32_t)(periph) >> 6)))
#define RCU_BIT_POS(val)                    ((uint32_t)(val) & 0x1FU)
/* define the voltage key unlock value */
#define RCU_VKEY_UNLOCK                 ((uint32_t)0x1A2B3C4DU)

/* register index */
enum reg_idx
{
    /* peripherals enable */
    IDX_AHBEN   = 0x14U, 
    IDX_APB2EN  = 0x18U, 
    IDX_APB1EN  = 0x1CU, 
    IDX_ADDAPB1EN = 0xF8U,
    /* peripherals reset */
    IDX_AHBRST  = 0x28U, 
    IDX_APB2RST = 0x0CU, 
    IDX_APB1RST = 0x10U, 
    IDX_ADDAPB1RST = 0xFCU,
    /* clock stabilization */
    IDX_CTL0    = 0x00U,
    IDX_BDCTL   = 0x20U,
    IDX_STB     = 0x24U,
    IDX_CTL1    = 0x34U,
    /* peripheral reset */
    IDX_RSTSCK  = 0x24U,
    /* clock stabilization and stuck interrupt */
    IDX_INT     = 0x08U,
    /* configuration register */
    IDX_CFG0    = 0x04U,
    IDX_CFG2    = 0x30U
};

/* peripheral clock enable */
typedef enum
{
    /* AHB peripherals */
    RCU_DMA     = RCU_REGIDX_BIT(IDX_AHBEN, 0U),              /*!< DMA clock */
    RCU_CRC     = RCU_REGIDX_BIT(IDX_AHBEN, 6U),              /*!< CRC clock */
    RCU_GPIOA   = RCU_REGIDX_BIT(IDX_AHBEN, 17U),             /*!< GPIOA clock */
    RCU_GPIOB   = RCU_REGIDX_BIT(IDX_AHBEN, 18U),             /*!< GPIOB clock */
    RCU_GPIOC   = RCU_REGIDX_BIT(IDX_AHBEN, 19U),             /*!< GPIOC clock */
    RCU_GPIOD   = RCU_REGIDX_BIT(IDX_AHBEN, 20U),             /*!< GPIOD clock */
    RCU_GPIOF   = RCU_REGIDX_BIT(IDX_AHBEN, 22U),             /*!< GPIOF clock */
    RCU_TSI     = RCU_REGIDX_BIT(IDX_AHBEN, 24U),             /*!< TSI clock */
    
    /* APB2 peripherals */
    RCU_CFGCMP  = RCU_REGIDX_BIT(IDX_APB2EN, 0U),             /*!< CFGCMP clock */
    RCU_ADC     = RCU_REGIDX_BIT(IDX_APB2EN, 9U),             /*!< ADC clock */
    RCU_TIMER0  = RCU_REGIDX_BIT(IDX_APB2EN, 11U),            /*!< TIMER0 clock */
    RCU_SPI0    = RCU_REGIDX_BIT(IDX_APB2EN, 12U),            /*!< SPI0 clock */
    RCU_USART0  = RCU_REGIDX_BIT(IDX_APB2EN, 14U),            /*!< USART0 clock */
    RCU_TIMER14 = RCU_REGIDX_BIT(IDX_APB2EN, 16U),            /*!< TIMER14 clock */
    RCU_TIMER15 = RCU_REGIDX_BIT(IDX_APB2EN, 17U),            /*!< TIMER15 clock */
    RCU_TIMER16 = RCU_REGIDX_BIT(IDX_APB2EN, 18U),            /*!< TIMER16 clock */
    
    /* APB1 peripherals */
    RCU_TIMER1  = RCU_REGIDX_BIT(IDX_APB1EN, 0U),             /*!< TIMER1 clock */
    RCU_TIMER2  = RCU_REGIDX_BIT(IDX_APB1EN, 1U),             /*!< TIMER2 clock */
    RCU_TIMER5  = RCU_REGIDX_BIT(IDX_APB1EN, 4U),             /*!< TIMER5 clock */
    RCU_TIMER13 = RCU_REGIDX_BIT(IDX_APB1EN, 8U),             /*!< TIMER13 clock */
#ifdef GD32F170_190
    RCU_SLCD    = RCU_REGIDX_BIT(IDX_APB1EN, 9U),             /*!< SLCD clock */
#endif /* GD32F170_190 */
    RCU_WWDGT   = RCU_REGIDX_BIT(IDX_APB1EN, 11U),            /*!< WWDGT clock */
    RCU_SPI1    = RCU_REGIDX_BIT(IDX_APB1EN, 14U),            /*!< SPI1 clock */
    RCU_SPI2    = RCU_REGIDX_BIT(IDX_APB1EN, 15U),            /*!< SPI2 clock */
    RCU_USART1  = RCU_REGIDX_BIT(IDX_APB1EN, 17U),            /*!< USART1 clock */
    RCU_I2C0    = RCU_REGIDX_BIT(IDX_APB1EN, 21U),            /*!< I2C0 clock */
    RCU_I2C1    = RCU_REGIDX_BIT(IDX_APB1EN, 22U),            /*!< I2C1 clock */
#ifdef GD32F130_150
    RCU_USBD    = RCU_REGIDX_BIT(IDX_APB1EN, 23U),            /*!< USBD clock */
#endif /* GD32F130_150 */
#ifdef GD32F170_190
    RCU_CAN0    = RCU_REGIDX_BIT(IDX_APB1EN, 25U),            /*!< CAN0 clock */
    RCU_CAN1    = RCU_REGIDX_BIT(IDX_APB1EN, 26U),            /*!< CAN1 clock */
#endif /* GD32F170_190 */
    RCU_PMU     = RCU_REGIDX_BIT(IDX_APB1EN, 28U),            /*!< PMU clock */
    RCU_DAC     = RCU_REGIDX_BIT(IDX_APB1EN, 29U),            /*!< DAC clock */
    RCU_CEC     = RCU_REGIDX_BIT(IDX_APB1EN, 30U),            /*!< CEC clock */
#ifdef GD32F170_190
    RCU_OPAIVREF = RCU_REGIDX_BIT(IDX_APB1EN, 31U),           /*!< OPAIVREF clock */
#endif /* GD32F170_190 */
    RCU_RTC     = RCU_REGIDX_BIT(IDX_BDCTL, 15U),             /*!< RTC clock */
    
    /* RCU_ADDAPB1EN */
    RCU_I2C2    = RCU_REGIDX_BIT(IDX_ADDAPB1EN, 0U)           /*!< I2C2 clock */
}rcu_periph_enum;

/* peripheral clock enable when sleep mode*/
typedef enum
{
    /* AHB peripherals */
    RCU_SRAM_SLP   = RCU_REGIDX_BIT(IDX_AHBEN, 2U),           /*!< SRAM clock when sleep mode */
    RCU_FMC_SLP    = RCU_REGIDX_BIT(IDX_AHBEN, 4U)            /*!< FMC clock when sleep mode */
}rcu_periph_sleep_enum;

/* peripherals reset */
typedef enum
{
    /* AHB peripherals reset */
    RCU_GPIOARST   = RCU_REGIDX_BIT(IDX_AHBRST, 17U),         /*!< GPIOA reset */
    RCU_GPIOBRST   = RCU_REGIDX_BIT(IDX_AHBRST, 18U),         /*!< GPIOB reset */
    RCU_GPIOCRST   = RCU_REGIDX_BIT(IDX_AHBRST, 19U),         /*!< GPIOC reset */
    RCU_GPIODRST   = RCU_REGIDX_BIT(IDX_AHBRST, 20U),         /*!< GPIOD reset */
    RCU_GPIOFRST   = RCU_REGIDX_BIT(IDX_AHBRST, 22U),         /*!< GPIOF reset */
    RCU_TSIRST     = RCU_REGIDX_BIT(IDX_AHBRST, 24U),         /*!< TSI reset */
    
    /* APB2 peripherals reset */
    RCU_CFGCMPRST  = RCU_REGIDX_BIT(IDX_APB2RST, 0U),         /*!< CFGCMP reset */
    RCU_ADCRST     = RCU_REGIDX_BIT(IDX_APB2RST, 9U),         /*!< ADC reset */
    RCU_TIMER0RST  = RCU_REGIDX_BIT(IDX_APB2RST, 11U),        /*!< TIMER0 reset */
    RCU_SPI0RST    = RCU_REGIDX_BIT(IDX_APB2RST, 12U),        /*!< SPI0 reset */
    RCU_USART0RST  = RCU_REGIDX_BIT(IDX_APB2RST, 14U),        /*!< USART0 reset */
    RCU_TIMER14RST = RCU_REGIDX_BIT(IDX_APB2RST, 16U),        /*!< TIMER14 reset */
    RCU_TIMER15RST = RCU_REGIDX_BIT(IDX_APB2RST, 17U),        /*!< TIMER15 reset */
    RCU_TIMER16RST = RCU_REGIDX_BIT(IDX_APB2RST, 18U),        /*!< TIMER16 reset */
    
    /* APB1 peripherals reset */
    RCU_TIMER1RST  = RCU_REGIDX_BIT(IDX_APB1RST, 0U),         /*!< TIMER1 reset */
    RCU_TIMER2RST  = RCU_REGIDX_BIT(IDX_APB1RST, 1U),         /*!< TIMER2 reset */
    RCU_TIMER5RST  = RCU_REGIDX_BIT(IDX_APB1RST, 4U),         /*!< TIMER5 reset */
    RCU_TIMER13RST = RCU_REGIDX_BIT(IDX_APB1RST, 8U),         /*!< TIMER13 reset */
#ifdef GD32F170_190
    RCU_SLCDRST    = RCU_REGIDX_BIT(IDX_APB1RST, 9U),         /*!< SLCD reset */
#endif /* GD32F170_190 */
    RCU_WWDGTRST   = RCU_REGIDX_BIT(IDX_APB1RST, 11U),        /*!< WWDGT reset */
    RCU_SPI1RST    = RCU_REGIDX_BIT(IDX_APB1RST, 14U),        /*!< SPI1 reset */
    RCU_SPI2RST    = RCU_REGIDX_BIT(IDX_APB1RST, 15U),        /*!< SPI2 reset */
    RCU_USART1RST  = RCU_REGIDX_BIT(IDX_APB1RST, 17U),        /*!< USART1 reset */
    RCU_I2C0RST    = RCU_REGIDX_BIT(IDX_APB1RST, 21U),        /*!< I2C0 reset */
    RCU_I2C1RST    = RCU_REGIDX_BIT(IDX_APB1RST, 22U),        /*!< I2C1 reset */
#ifdef GD32F130_150
    RCU_USBDRST    = RCU_REGIDX_BIT(IDX_APB1RST, 23U),        /*!< USBD reset */
#endif /* GD32F130_150 */
#ifdef GD32F170_190
    RCU_CAN0RST    = RCU_REGIDX_BIT(IDX_APB1RST, 25U),        /*!< CAN0 reset */
    RCU_CAN1RST    = RCU_REGIDX_BIT(IDX_APB1RST, 26U),        /*!< CAN1 reset */
#endif /* GD32F170_190 */
    RCU_PMURST     = RCU_REGIDX_BIT(IDX_APB1RST, 28U),        /*!< PMU reset */
    RCU_DACRST     = RCU_REGIDX_BIT(IDX_APB1RST, 29U),        /*!< DAC reset */
    RCU_CECRST     = RCU_REGIDX_BIT(IDX_APB1RST, 30U),        /*!< CEC reset */
#ifdef GD32F170_190
    RCU_OPAIVREFRST = RCU_REGIDX_BIT(IDX_APB1RST, 31U),       /*!< OPAIVREF reset */
#endif /* GD32F170_190 */

    /* RCU_ADDAPB1RST */
    RCU_I2C2RST    = RCU_REGIDX_BIT(IDX_ADDAPB1RST, 0U),      /*!< I2C2 reset */
}rcu_periph_reset_enum;

/* clock stabilization and peripheral reset flags */
typedef enum
{
    RCU_FLAG_IRC40KSTB    = RCU_REGIDX_BIT(IDX_STB, 1U),      /*!< IRC40K stabilization flags */
    RCU_FLAG_LXTALSTB     = RCU_REGIDX_BIT(IDX_BDCTL, 1U),    /*!< LXTAL stabilization flags */
    RCU_FLAG_IRC8MSTB     = RCU_REGIDX_BIT(IDX_CTL0, 1U),     /*!< IRC8M stabilization flags */
    RCU_FLAG_HXTALSTB     = RCU_REGIDX_BIT(IDX_CTL0, 17U),    /*!< HXTAL stabilization flags */
    RCU_FLAG_PLLSTB       = RCU_REGIDX_BIT(IDX_CTL0, 25U),    /*!< PLL stabilization flags */
#ifdef GD32F130_150
    RCU_FLAG_IRC14MSTB    = RCU_REGIDX_BIT(IDX_CTL1, 1U),     /*!< IRC14M stabilization flags */
#elif defined (GD32F170_190)
    RCU_FLAG_IRC28MSTB    = RCU_REGIDX_BIT(IDX_CTL1, 1U),     /*!< IRC28M stabilization flags */
#endif /* GD32F130_150 */
    RCU_FLAG_V12RST       = RCU_REGIDX_BIT(IDX_RSTSCK, 23U),  /*!< V12 reset flags */
    RCU_FLAG_OBLRST       = RCU_REGIDX_BIT(IDX_RSTSCK, 25U),  /*!< OBL reset flags */
    RCU_FLAG_EPRST        = RCU_REGIDX_BIT(IDX_RSTSCK, 26U),  /*!< EPR reset flags */
    RCU_FLAG_PORRST       = RCU_REGIDX_BIT(IDX_RSTSCK, 27U),  /*!< Power reset flags */
    RCU_FLAG_SWRST        = RCU_REGIDX_BIT(IDX_RSTSCK, 28U),  /*!< SW reset flags */
    RCU_FLAG_FWDGTRST     = RCU_REGIDX_BIT(IDX_RSTSCK, 29U),  /*!< FWDGT reset flags */
    RCU_FLAG_WWDGTRST     = RCU_REGIDX_BIT(IDX_RSTSCK, 30U),  /*!< WWDGT reset flags */
    RCU_FLAG_LPRST        = RCU_REGIDX_BIT(IDX_RSTSCK, 31U)   /*!< LP reset flags */
}rcu_flag_enum;

/* clock stabilization and ckm interrupt flags */
typedef enum
{
    RCU_INT_FLAG_IRC40KSTB = RCU_INT_IRC40KSTBIF,             /*!< IRC40K stabilization interrupt flag */
    RCU_INT_FLAG_LXTALSTB  = RCU_INT_LXTALSTBIF,              /*!< LXTAL stabilization interrupt flag */
    RCU_INT_FLAG_IRC8MSTB  = RCU_INT_IRC8MSTBIF,              /*!< IRC8M stabilization interrupt flag */
    RCU_INT_FLAG_HXTALSTB  = RCU_INT_HXTALSTBIF,              /*!< HXTAL stabilization interrupt flag */
    RCU_INT_FLAG_PLLSTB    = RCU_INT_PLLSTBIF,                /*!< PLL stabilization interrupt flag */
#ifdef GD32F130_150
    RCU_INT_FLAG_IRC14MSTB = RCU_INT_IRC14MSTBIF,             /*!< IRC14M stabilization interrupt flag */
#elif defined (GD32F170_190)
    RCU_INT_FLAG_IRC28MSTB = RCU_INT_IRC28MSTBIF,             /*!< IRC28M stabilization interrupt flag */
#endif /* GD32F130_150 */
    RCU_INT_FLAG_CKM       = RCU_INT_CKMIF                    /*!< CKM interrupt flag */
}rcu_int_flag_enum;

/* clock stabilization and stuck interrupt flags clear */
typedef enum
{
    RCU_INT_FLAG_IRC40KSTB_CLR = RCU_INT_IRC40KSTBIC,        /*!< IRC40K stabilization interrupt flags clear */
    RCU_INT_FLAG_LXTALSTB_CLR  = RCU_INT_LXTALSTBIC,         /*!< LXTAL stabilization interrupt flags clear */
    RCU_INT_FLAG_IRC8MSTB_CLR  = RCU_INT_IRC8MSTBIC,         /*!< IRC8M stabilization interrupt flags clear */
    RCU_INT_FLAG_HXTALSTB_CLR  = RCU_INT_HXTALSTBIC,         /*!< HXTAL stabilization interrupt flags clear */
    RCU_INT_FLAG_PLLSTB_CLR    = RCU_INT_PLLSTBIC,           /*!< PLL stabilization interrupt flags clear */
#ifdef GD32F130_150
    RCU_INT_FLAG_IRC14MSTB_CLR = RCU_INT_IRC14MSTBIC,        /*!< IRC14M stabilization interrupt flags clear */
#elif defined (GD32F170_190)
    RCU_INT_FLAG_IRC28MSTB_CLR = RCU_INT_IRC28MSTBIC,        /*!< IRC28M stabilization interrupt flags clear */
#endif /* GD32F130_150 */
    RCU_INT_FLAG_CKM_CLR       = RCU_INT_CKMIC               /*!< CKM interrupt flags clear */
}rcu_int_flag_clear_enum;

/* clock stabilization interrupt enable or disable */
typedef enum
{
    RCU_INT_IRC40KSTB       = RCU_INT_IRC40KSTBIE,           /*!< IRC40K stabilization interrupt */
    RCU_INT_LXTALSTB        = RCU_INT_LXTALSTBIE,            /*!< LXTAL stabilization interrupt */
    RCU_INT_IRC8MSTB        = RCU_INT_IRC8MSTBIE,            /*!< IRC8M stabilization interrupt */
    RCU_INT_HXTALSTB        = RCU_INT_HXTALSTBIE,            /*!< HXTAL stabilization interrupt */
    RCU_INT_PLLSTB          = RCU_INT_PLLSTBIE,              /*!< PLL stabilization interrupt */
#ifdef GD32F130_150
    RCU_INT_IRC14MSTB       = RCU_INT_IRC14MSTBIE            /*!< IRC14M stabilization interrupt */
#elif defined (GD32F170_190)
    RCU_INT_IRC28MSTB       = RCU_INT_IRC28MSTBIE            /*!< IRC28M stabilization interrupt */
#endif /* GD32F130_150 */
}rcu_int_enum;

/* ADC clock source */
typedef enum
{
#ifdef GD32F130_150
    RCU_ADCCK_IRC14M        = 0,                             /*!< ADC clock source select IRC14M */
#elif defined (GD32F170_190)
    RCU_ADCCK_IRC28M_DIV2   = 0,                             /*!< ADC clock source select IRC28M/2 */
    RCU_ADCCK_IRC28M,                                        /*!< ADC clock source select IRC28M */
#endif /* GD32F130_150 */
    RCU_ADCCK_APB2_DIV2,                                     /*!< ADC clock source select APB2/2 */
    RCU_ADCCK_APB2_DIV4,                                     /*!< ADC clock source select APB2/4 */
    RCU_ADCCK_APB2_DIV6,                                     /*!< ADC clock source select APB2/6 */
    RCU_ADCCK_APB2_DIV8                                      /*!< ADC clock source select APB2/8 */
}rcu_adc_clock_enum;

/* oscillator types */
typedef enum
{
    RCU_HXTAL   = RCU_REGIDX_BIT(IDX_CTL0, 16U),             /*!< HXTAL */
    RCU_LXTAL   = RCU_REGIDX_BIT(IDX_BDCTL, 0U),             /*!< LXTAL */
    RCU_IRC8M   = RCU_REGIDX_BIT(IDX_CTL0, 0U),              /*!< IRC8M */
#ifdef GD32F130_150
    RCU_IRC14M  = RCU_REGIDX_BIT(IDX_CTL1, 0U),              /*!< IRC14M */
#elif defined (GD32F170_190)
    RCU_IRC28M  = RCU_REGIDX_BIT(IDX_CTL1, 0U),              /*!< IRC28M */
#endif /* GD32F130_150 */
    RCU_IRC40K  = RCU_REGIDX_BIT(IDX_RSTSCK, 0U),            /*!< IRC40K */
    RCU_PLL_CK  = RCU_REGIDX_BIT(IDX_CTL0, 24U)              /*!< PLL */
}rcu_osci_type_enum;

/* rcu clock frequency */
typedef enum
{
    CK_SYS      = 0,                                         /*!< system clock */
    CK_AHB,                                                  /*!< AHB clock */
    CK_APB1,                                                 /*!< APB1 clock */
    CK_APB2,                                                 /*!< APB2 clock */
    CK_ADC,                                                  /*!< ADC clock */
    CK_CEC,                                                  /*!< CEC clock */
    CK_USART                                                 /*!< USART clock */
}rcu_clock_freq_enum;

/* system clock source select */
#define CFG0_SCS(regval)            (BITS(0,1) & ((uint32_t)(regval) << 0))
#define RCU_CKSYSSRC_IRC8M          CFG0_SCS(0)              /*!< system clock source select IRC8M */
#define RCU_CKSYSSRC_HXTAL          CFG0_SCS(1)              /*!< system clock source select HXTAL */
#define RCU_CKSYSSRC_PLL            CFG0_SCS(2)              /*!< system clock source select PLL */

/* system clock source select status */
#define CFG0_SCSS(regval)           (BITS(2,3) & ((uint32_t)(regval) << 2))
#define RCU_SCSS_IRC8M              CFG0_SCSS(0)             /*!< system clock source select IRC8M */
#define RCU_SCSS_HXTAL              CFG0_SCSS(1)             /*!< system clock source select HXTAL */
#define RCU_SCSS_PLL                CFG0_SCSS(2)             /*!< system clock source select PLL */

/* AHB prescaler selection */
#define CFG0_AHBPSC(regval)         (BITS(4,7) & ((uint32_t)(regval) << 4))
#define RCU_AHB_CKSYS_DIV1          CFG0_AHBPSC(0)           /*!< AHB prescaler select CK_SYS */
#define RCU_AHB_CKSYS_DIV2          CFG0_AHBPSC(8)           /*!< AHB prescaler select CK_SYS/2 */
#define RCU_AHB_CKSYS_DIV4          CFG0_AHBPSC(9)           /*!< AHB prescaler select CK_SYS/4 */
#define RCU_AHB_CKSYS_DIV8          CFG0_AHBPSC(10)          /*!< AHB prescaler select CK_SYS/8 */
#define RCU_AHB_CKSYS_DIV16         CFG0_AHBPSC(11)          /*!< AHB prescaler select CK_SYS/16 */
#define RCU_AHB_CKSYS_DIV64         CFG0_AHBPSC(12)          /*!< AHB prescaler select CK_SYS/64 */
#define RCU_AHB_CKSYS_DIV128        CFG0_AHBPSC(13)          /*!< AHB prescaler select CK_SYS/128 */
#define RCU_AHB_CKSYS_DIV256        CFG0_AHBPSC(14)          /*!< AHB prescaler select CK_SYS/256 */
#define RCU_AHB_CKSYS_DIV512        CFG0_AHBPSC(15)          /*!< AHB prescaler select CK_SYS/512 */

/* APB1 prescaler selection */
#define CFG0_APB1PSC(regval)        (BITS(8,10) & ((uint32_t)(regval) << 8))
#define RCU_APB1_CKAHB_DIV1         CFG0_APB1PSC(0)          /*!< APB1 prescaler select CK_AHB */
#define RCU_APB1_CKAHB_DIV2         CFG0_APB1PSC(4)          /*!< APB1 prescaler select CK_AHB/2 */
#define RCU_APB1_CKAHB_DIV4         CFG0_APB1PSC(5)          /*!< APB1 prescaler select CK_AHB/4 */
#define RCU_APB1_CKAHB_DIV8         CFG0_APB1PSC(6)          /*!< APB1 prescaler select CK_AHB/8 */
#define RCU_APB1_CKAHB_DIV16        CFG0_APB1PSC(7)          /*!< APB1 prescaler select CK_AHB/16 */

/* APB2 prescaler selection */
#define CFG0_APB2PSC(regval)        (BITS(11,13) & ((uint32_t)(regval) << 11))
#define RCU_APB2_CKAHB_DIV1         CFG0_APB2PSC(0)          /*!< APB2 prescaler select CK_AHB */
#define RCU_APB2_CKAHB_DIV2         CFG0_APB2PSC(4)          /*!< APB2 prescaler select CK_AHB/2 */
#define RCU_APB2_CKAHB_DIV4         CFG0_APB2PSC(5)          /*!< APB2 prescaler select CK_AHB/4 */
#define RCU_APB2_CKAHB_DIV8         CFG0_APB2PSC(6)          /*!< APB2 prescaler select CK_AHB/8 */
#define RCU_APB2_CKAHB_DIV16        CFG0_APB2PSC(7)          /*!< APB2 prescaler select CK_AHB/16 */

/* ADC clock prescaler selection */
#define CFG0_ADCPSC(regval)         (BITS(14,15) & ((uint32_t)(regval) << 14))
#define RCU_ADC_CKAPB2_DIV2         CFG0_ADCPSC(0)           /*!< ADC clock prescaler select CK_APB2/2 */
#define RCU_ADC_CKAPB2_DIV4         CFG0_ADCPSC(1)           /*!< ADC clock prescaler select CK_APB2/4 */
#define RCU_ADC_CKAPB2_DIV6         CFG0_ADCPSC(2)           /*!< ADC clock prescaler select CK_APB2/6 */
#define RCU_ADC_CKAPB2_DIV8         CFG0_ADCPSC(3)           /*!< ADC clock prescaler select CK_APB2/8 */

/* PLL clock source selection */
#define RCU_PLLSRC_IRC8M_DIV2       (uint32_t)0x00000000     /*!< PLL clock source select IRC8M/2 */
#define RCU_PLLSRC_HXTAL            RCU_CFG0_PLLSEL          /*!< PLL clock source select HXTAL */

/* HXTAL divider for PLL source clock selection */
#define RCU_PLLPREDV_HXTAL          (uint32_t)0x00000000     /*!< HXTAL clock selected */
#define RCU_PLLPREDV_HXTAL_DIV2     RCU_CFG0_PLLPREDV        /*!< HXTAL/2 clock selected */

/* PLL multiply factor */
#define CFG0_PLLMF(regval)          (BITS(18,21) & ((uint32_t)(regval) << 18))
#define RCU_PLL_MUL2                CFG0_PLLMF(0)                       /*!< PLL source clock multiply by 2 */
#define RCU_PLL_MUL3                CFG0_PLLMF(1)                       /*!< PLL source clock multiply by 3 */
#define RCU_PLL_MUL4                CFG0_PLLMF(2)                       /*!< PLL source clock multiply by 4 */
#define RCU_PLL_MUL5                CFG0_PLLMF(3)                       /*!< PLL source clock multiply by 5 */
#define RCU_PLL_MUL6                CFG0_PLLMF(4)                       /*!< PLL source clock multiply by 6 */
#define RCU_PLL_MUL7                CFG0_PLLMF(5)                       /*!< PLL source clock multiply by 7 */
#define RCU_PLL_MUL8                CFG0_PLLMF(6)                       /*!< PLL source clock multiply by 8 */
#define RCU_PLL_MUL9                CFG0_PLLMF(7)                       /*!< PLL source clock multiply by 9 */
#define RCU_PLL_MUL10               CFG0_PLLMF(8)                       /*!< PLL source clock multiply by 10 */
#define RCU_PLL_MUL11               CFG0_PLLMF(9)                       /*!< PLL source clock multiply by 11 */
#define RCU_PLL_MUL12               CFG0_PLLMF(10)                      /*!< PLL source clock multiply by 12 */
#define RCU_PLL_MUL13               CFG0_PLLMF(11)                      /*!< PLL source clock multiply by 13 */
#define RCU_PLL_MUL14               CFG0_PLLMF(12)                      /*!< PLL source clock multiply by 14 */
#define RCU_PLL_MUL15               CFG0_PLLMF(13)                      /*!< PLL source clock multiply by 15 */
#define RCU_PLL_MUL16               CFG0_PLLMF(14)                      /*!< PLL source clock multiply by 16 */
#define RCU_PLL_MUL17               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(0))   /*!< PLL source clock multiply by 17 */
#define RCU_PLL_MUL18               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(1))   /*!< PLL source clock multiply by 18 */
#define RCU_PLL_MUL19               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(2))   /*!< PLL source clock multiply by 19 */
#define RCU_PLL_MUL20               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(3))   /*!< PLL source clock multiply by 20 */
#define RCU_PLL_MUL21               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(4))   /*!< PLL source clock multiply by 21 */
#define RCU_PLL_MUL22               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(5))   /*!< PLL source clock multiply by 22 */
#define RCU_PLL_MUL23               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(6))   /*!< PLL source clock multiply by 23 */
#define RCU_PLL_MUL24               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(7))   /*!< PLL source clock multiply by 24 */
#define RCU_PLL_MUL25               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(8))   /*!< PLL source clock multiply by 25 */
#define RCU_PLL_MUL26               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(9))   /*!< PLL source clock multiply by 26 */
#define RCU_PLL_MUL27               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(10))  /*!< PLL source clock multiply by 27 */
#define RCU_PLL_MUL28               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(11))  /*!< PLL source clock multiply by 28 */
#define RCU_PLL_MUL29               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(12))  /*!< PLL source clock multiply by 29 */
#define RCU_PLL_MUL30               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(13))  /*!< PLL source clock multiply by 30 */
#define RCU_PLL_MUL31               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(14))  /*!< PLL source clock multiply by 31 */
#define RCU_PLL_MUL32               (RCU_CFG0_PLLMF4 | CFG0_PLLMF(15))  /*!< PLL source clock multiply by 32 */

#ifdef GD32F130_150
/* USBD clock prescaler selection */
#define CFG0_USBDPSC(regval)        (BITS(22,23) & ((uint32_t)(regval) << 22))
#define RCU_USBD_CKPLL_DIV1_5       CFG0_USBDPSC(0)                     /*!< USBD clock prescaler select CK_PLL/1.5 */
#define RCU_USBD_CKPLL_DIV1         CFG0_USBDPSC(1)                     /*!< USBD clock prescaler select CK_PLL */
#define RCU_USBD_CKPLL_DIV2_5       CFG0_USBDPSC(2)                     /*!< USBD clock prescaler select CK_PLL/2.5 */
#define RCU_USBD_CKPLL_DIV2         CFG0_USBDPSC(3)                     /*!< USBD clock prescaler select CK_PLL/2 */

/* CK_OUT clock source selection */
#define CFG0_CKOUTSEL(regval)       (BITS(24,26) & ((uint32_t)(regval) << 24))
#define RCU_CKOUTSRC_NONE           CFG0_CKOUTSEL(0)                    /*!< no clock selected */
#define RCU_CKOUTSRC_IRC14M         CFG0_CKOUTSEL(1)                    /*!< CK_OUT clock source select IRC14M */
#define RCU_CKOUTSRC_IRC40K         CFG0_CKOUTSEL(2)                    /*!< CK_OUT clock source select IRC40K */
#define RCU_CKOUTSRC_LXTAL          CFG0_CKOUTSEL(3)                    /*!< CK_OUT clock source select LXTAL */
#define RCU_CKOUTSRC_CKSYS          CFG0_CKOUTSEL(4)                    /*!< CK_OUT clock source select CKSYS */
#define RCU_CKOUTSRC_IRC8M          CFG0_CKOUTSEL(5)                    /*!< CK_OUT clock source select IRC8M */
#define RCU_CKOUTSRC_HXTAL          CFG0_CKOUTSEL(6)                    /*!< CK_OUT clock source select HXTAL */
#define RCU_CKOUTSRC_CKPLL_DIV1     (RCU_CFG0_PLLDV | CFG0_CKOUTSEL(7)) /*!< CK_OUT clock source select CK_PLL */
#define RCU_CKOUTSRC_CKPLL_DIV2     CFG0_CKOUTSEL(7)                    /*!< CK_OUT clock source select CK_PLL/2 */

/* CK_OUT divider */
#define CFG0_CKOUTDIV(regval)       (BITS(28,30) & ((uint32_t)(regval) << 28))
#define RCU_CKOUT_DIV1              CFG0_CKOUTDIV(0)                    /*!< CK_OUT is divided by 1 */
#define RCU_CKOUT_DIV2              CFG0_CKOUTDIV(1)                    /*!< CK_OUT is divided by 2 */
#define RCU_CKOUT_DIV4              CFG0_CKOUTDIV(2)                    /*!< CK_OUT is divided by 4 */
#define RCU_CKOUT_DIV8              CFG0_CKOUTDIV(3)                    /*!< CK_OUT is divided by 8 */
#define RCU_CKOUT_DIV16             CFG0_CKOUTDIV(4)                    /*!< CK_OUT is divided by 16 */
#define RCU_CKOUT_DIV32             CFG0_CKOUTDIV(5)                    /*!< CK_OUT is divided by 32 */
#define RCU_CKOUT_DIV64             CFG0_CKOUTDIV(6)                    /*!< CK_OUT is divided by 64 */
#define RCU_CKOUT_DIV128            CFG0_CKOUTDIV(7)                    /*!< CK_OUT is divided by 128 */
#elif defined (GD32F170_190)
/* CK_OUT0 clock source selection */
#define CFG0_CKOUT0SEL(regval)      (BITS(24,26) & ((uint32_t)(regval) << 24))
#define RCU_CKOUT0SRC_NONE          CFG0_CKOUT0SEL(0)                    /*!< no clock selected */
#define RCU_CKOUT0SRC_IRC28M        CFG0_CKOUT0SEL(1)                    /*!< CK_OUT0 clock source select IRC28M */
#define RCU_CKOUT0SRC_IRC40K        CFG0_CKOUT0SEL(2)                    /*!< CK_OUT0 clock source select IRC40K */
#define RCU_CKOUT0SRC_LXTAL         CFG0_CKOUT0SEL(3)                    /*!< CK_OUT0 clock source select LXTAL */
#define RCU_CKOUT0SRC_CKSYS         CFG0_CKOUT0SEL(4)                    /*!< CK_OUT0 clock source select CKSYS */
#define RCU_CKOUT0SRC_IRC8M         CFG0_CKOUT0SEL(5)                    /*!< CK_OUT0 clock source select IRC8M */
#define RCU_CKOUT0SRC_HXTAL         CFG0_CKOUT0SEL(6)                    /*!< CK_OUT0 clock source select HXTAL */
#define RCU_CKOUT0SRC_CKPLL_DIV1    (RCU_CFG0_PLLDV | CFG0_CKOUT0SEL(7)) /*!< CK_OUT0 clock source select CK_PLL */
#define RCU_CKOUT0SRC_CKPLL_DIV2    CFG0_CKOUT0SEL(7)                    /*!< CK_OUT0 clock source select CK_PLL/2 */

/* CK_OUT0 divider */
#define CFG0_CKOUT0DIV(regval)      (BITS(28,30) & ((uint32_t)(regval) << 28))
#define RCU_CKOUT0_DIV1             CFG0_CKOUT0DIV(0)                   /*!< CK_OUT0 is divided by 1 */
#define RCU_CKOUT0_DIV2             CFG0_CKOUT0DIV(1)                   /*!< CK_OUT0 is divided by 2 */
#define RCU_CKOUT0_DIV4             CFG0_CKOUT0DIV(2)                   /*!< CK_OUT0 is divided by 4 */
#define RCU_CKOUT0_DIV8             CFG0_CKOUT0DIV(3)                   /*!< CK_OUT0 is divided by 8 */
#define RCU_CKOUT0_DIV16            CFG0_CKOUT0DIV(4)                   /*!< CK_OUT0 is divided by 16 */
#define RCU_CKOUT0_DIV32            CFG0_CKOUT0DIV(5)                   /*!< CK_OUT0 is divided by 32 */
#define RCU_CKOUT0_DIV64            CFG0_CKOUT0DIV(6)                   /*!< CK_OUT0 is divided by 64 */
#define RCU_CKOUT0_DIV128           CFG0_CKOUT0DIV(7)                   /*!< CK_OUT0 is divided by 128 */
#endif /* GD32F130_150 */

/* CK_PLL divide by 1 or 2 for CK_OUT */
#define RCU_PLLDV_CKPLL_DIV2        (uint32_t)0x00000000U               /*!< CK_PLL divide by 2 for CK_OUT */
#define RCU_PLLDV_CKPLL             RCU_CFG0_PLLDV                      /*!< CK_PLL divide by 1 for CK_OUT */

/* LXTAL drive capability */
#define BDCTL_LXTALDRI(regval)      (BITS(3,4) & ((uint32_t)(regval) << 3))
#define RCU_LXTAL_LOWDRI            BDCTL_LXTALDRI(0)                   /*!< lower driving capability */
#define RCU_LXTAL_MED_LOWDRI        BDCTL_LXTALDRI(1)                   /*!< medium low driving capability */
#define RCU_LXTAL_MED_HIGHDRI       BDCTL_LXTALDRI(2)                   /*!< medium high driving capability */
#define RCU_LXTAL_HIGHDRI           BDCTL_LXTALDRI(3)                   /*!< higher driving capability */

/* RTC clock entry selection */
#define BDCTL_RTCSRC(regval)        (BITS(8,9) & ((uint32_t)(regval) << 8))
#define RCU_RTCSRC_NONE             BDCTL_RTCSRC(0)                     /*!< no clock selected */
#define RCU_RTCSRC_LXTAL            BDCTL_RTCSRC(1)                     /*!< LXTAL selected as RTC source clock */
#define RCU_RTCSRC_IRC40K           BDCTL_RTCSRC(2)                     /*!< IRC40K selected as RTC source clock */
#define RCU_RTCSRC_HXTAL_DIV32      BDCTL_RTCSRC(3)                     /*!< HXTAL/32 selected as RTC source clock */

#ifdef GD32F170_190
/* SLCD clock entry selection */
#define BDCTL_RTCSRC(regval)        (BITS(8,9) & ((uint32_t)(regval) << 8))
#define RCU_SLCDSRC_NONE            BDCTL_RTCSRC(0)                     /*!< no clock selected */
#define RCU_SLCDSRC_LXTAL           BDCTL_RTCSRC(1)                     /*!< LXTAL selected as SLCD source clock */
#define RCU_SLCDSRC_IRC40K          BDCTL_RTCSRC(2)                     /*!< IRC40K selected as SLCD source clock */
#define RCU_SLCDSRC_HXTAL_DIV32     BDCTL_RTCSRC(3)                     /*!< HXTAL/32 selected as SLCD source clock */
#endif /* GD32F170_190 */

/* CK_HXTAL divider previous PLL */
#define CFG1_HXTALPREDV(regval)     (BITS(0,3) & ((uint32_t)(regval) << 0))
#define RCU_PLL_HXTAL_DIV1          CFG1_HXTALPREDV(0)                  /*!< HXTAL input to PLL not divided */
#define RCU_PLL_HXTAL_DIV2          CFG1_HXTALPREDV(1)                  /*!< HXTAL input to PLL divided by 2 */
#define RCU_PLL_HXTAL_DIV3          CFG1_HXTALPREDV(2)                  /*!< HXTAL input to PLL divided by 3 */
#define RCU_PLL_HXTAL_DIV4          CFG1_HXTALPREDV(3)                  /*!< HXTAL input to PLL divided by 4 */
#define RCU_PLL_HXTAL_DIV5          CFG1_HXTALPREDV(4)                  /*!< HXTAL input to PLL divided by 5 */
#define RCU_PLL_HXTAL_DIV6          CFG1_HXTALPREDV(5)                  /*!< HXTAL input to PLL divided by 6 */
#define RCU_PLL_HXTAL_DIV7          CFG1_HXTALPREDV(6)                  /*!< HXTAL input to PLL divided by 7 */
#define RCU_PLL_HXTAL_DIV8          CFG1_HXTALPREDV(7)                  /*!< HXTAL input to PLL divided by 8 */
#define RCU_PLL_HXTAL_DIV9          CFG1_HXTALPREDV(8)                  /*!< HXTAL input to PLL divided by 9 */
#define RCU_PLL_HXTAL_DIV10         CFG1_HXTALPREDV(9)                  /*!< HXTAL input to PLL divided by 10 */
#define RCU_PLL_HXTAL_DIV11         CFG1_HXTALPREDV(10)                 /*!< HXTAL input to PLL divided by 11 */
#define RCU_PLL_HXTAL_DIV12         CFG1_HXTALPREDV(11)                 /*!< HXTAL input to PLL divided by 12 */
#define RCU_PLL_HXTAL_DIV13         CFG1_HXTALPREDV(12)                 /*!< HXTAL input to PLL divided by 13 */
#define RCU_PLL_HXTAL_DIV14         CFG1_HXTALPREDV(13)                 /*!< HXTAL input to PLL divided by 14 */
#define RCU_PLL_HXTAL_DIV15         CFG1_HXTALPREDV(14)                 /*!< HXTAL input to PLL divided by 15 */
#define RCU_PLL_HXTAL_DIV16         CFG1_HXTALPREDV(15)                 /*!< HXTAL input to PLL divided by 16 */

/* USART0 clock source selection */
#define CFG2_USART0SEL(regval)      (BITS(0,1) & ((uint32_t)(regval) << 0))
#define RCU_USART0SRC_CKAPB2        CFG2_USART0SEL(0)                   /*!< CK_USART0 select CK_APB2 */
#define RCU_USART0SRC_CKSYS         CFG2_USART0SEL(1)                   /*!< CK_USART0 select CK_SYS */
#define RCU_USART0SRC_LXTAL         CFG2_USART0SEL(2)                   /*!< CK_USART0 select LXTAL */
#define RCU_USART0SRC_IRC8M         CFG2_USART0SEL(3)                   /*!< CK_USART0 select IRC8M */

/* CEC clock source selection */
#define RCU_CECSRC_IRC8M_DIV244     (uint32_t)0x00000000U               /*!< CK_CEC clock source select IRC8M/244 */
#define RCU_CECSRC_LXTAL            RCU_CFG2_CECSEL                     /*!< CK_CEC clock source select LXTAL */

#ifdef GD32F130_150
/* ADC clock source selection */
#define RCU_ADCSRC_IRC14M           (uint32_t)0x00000000U               /*!< ADC clock source select */
#define RCU_ADCSRC_APB2DIV          RCU_CFG2_ADCSEL                     /*!< ADC clock source select */
#elif defined (GD32F170_190)
/* ADC clock source selection */
#define RCU_ADCSRC_IRC28M           (uint32_t)0x00000000U               /*!< ADC clock source select */
#define RCU_ADCSRC_APB2DIV          RCU_CFG2_ADCSEL                     /*!< ADC clock source select */

/* IRC28M clock divider for ADC */
#define RCU_ADC_IRC28M_DIV2         (uint32_t)0x00000000U               /*!< IRC28M/2 select to ADC clock */
#define RCU_ADC_IRC28M_DIV1         RCU_CFG2_IRC28MDIV                  /*!< IRC28M select to ADC clock */

/* CK_OUT1 clock source selection */
#define CFG3_CKOUT1SRC(regval)      (BITS(0,2) & ((uint32_t)(regval) << 0))
#define RCU_CKOUT1SRC_NONE          CFG3_CKOUT1SRC(0)                   /*!< no clock selected */
#define RCU_CKOUT1SRC_IRC28M        CFG3_CKOUT1SRC(1)                   /*!< CK_OUT1 clock source select IRC28M */
#define RCU_CKOUT1SRC_IRC40K        CFG3_CKOUT1SRC(2)                   /*!< CK_OUT1 clock source select IRC40K */
#define RCU_CKOUT1SRC_LXTAL         CFG3_CKOUT1SRC(3)                   /*!< CK_OUT1 clock source select LXTAL */
#define RCU_CKOUT1SRC_CKSYS         CFG3_CKOUT1SRC(4)                   /*!< CK_OUT1 clock source select CKSYS */
#define RCU_CKOUT1SRC_IRC8M         CFG3_CKOUT1SRC(5)                   /*!< CK_OUT1 clock source select IRC8M */
#define RCU_CKOUT1SRC_HXTAL         CFG3_CKOUT1SRC(6)                   /*!< CK_OUT1 clock source select HXTAL */
#define RCU_CKOUT1SRC_CKPLL_DIV1    0x00000007U                         /*!< CK_OUT1 clock source select CK_PLL */
#define RCU_CKOUT1SRC_CKPLL_DIV2    0x00000008U                         /*!< CK_OUT1 clock source select CK_PLL/2 */

/* CK_OUT1 divider */
#define CFG3_CKOUT1DIV(regval)      (BITS(8,13) & ((uint32_t)(regval) << 8))
#define RCU_CKOUT1_DIV1             CFG3_CKOUT1DIV(0)                   /*!< CK_OUT1 is divided by 1 */
#define RCU_CKOUT1_DIV2             CFG3_CKOUT1DIV(1)                   /*!< CK_OUT1 is divided by 2 */
#define RCU_CKOUT1_DIV3             CFG3_CKOUT1DIV(2)                   /*!< CK_OUT1 is divided by 3 */
#define RCU_CKOUT1_DIV4             CFG3_CKOUT1DIV(3)                   /*!< CK_OUT1 is divided by 4 */
#define RCU_CKOUT1_DIV5             CFG3_CKOUT1DIV(4)                   /*!< CK_OUT1 is divided by 5 */
#define RCU_CKOUT1_DIV6             CFG3_CKOUT1DIV(5)                   /*!< CK_OUT1 is divided by 6 */
#define RCU_CKOUT1_DIV7             CFG3_CKOUT1DIV(6)                   /*!< CK_OUT1 is divided by 7 */
#define RCU_CKOUT1_DIV8             CFG3_CKOUT1DIV(7)                   /*!< CK_OUT1 is divided by 8 */
#define RCU_CKOUT1_DIV9             CFG3_CKOUT1DIV(8)                   /*!< CK_OUT1 is divided by 9 */
#define RCU_CKOUT1_DIV10            CFG3_CKOUT1DIV(9)                   /*!< CK_OUT1 is divided by 10 */
#define RCU_CKOUT1_DIV11            CFG3_CKOUT1DIV(10)                  /*!< CK_OUT1 is divided by 11 */
#define RCU_CKOUT1_DIV12            CFG3_CKOUT1DIV(11)                  /*!< CK_OUT1 is divided by 12 */
#define RCU_CKOUT1_DIV13            CFG3_CKOUT1DIV(12)                  /*!< CK_OUT1 is divided by 13 */
#define RCU_CKOUT1_DIV14            CFG3_CKOUT1DIV(13)                  /*!< CK_OUT1 is divided by 14 */
#define RCU_CKOUT1_DIV15            CFG3_CKOUT1DIV(14)                  /*!< CK_OUT1 is divided by 15 */
#define RCU_CKOUT1_DIV16            CFG3_CKOUT1DIV(15)                  /*!< CK_OUT1 is divided by 16 */
#define RCU_CKOUT1_DIV17            CFG3_CKOUT1DIV(16)                  /*!< CK_OUT1 is divided by 17 */
#define RCU_CKOUT1_DIV18            CFG3_CKOUT1DIV(17)                  /*!< CK_OUT1 is divided by 18 */
#define RCU_CKOUT1_DIV19            CFG3_CKOUT1DIV(18)                  /*!< CK_OUT1 is divided by 19 */
#define RCU_CKOUT1_DIV20            CFG3_CKOUT1DIV(19)                  /*!< CK_OUT1 is divided by 20 */
#define RCU_CKOUT1_DIV21            CFG3_CKOUT1DIV(20)                  /*!< CK_OUT1 is divided by 21 */
#define RCU_CKOUT1_DIV22            CFG3_CKOUT1DIV(21)                  /*!< CK_OUT1 is divided by 22 */
#define RCU_CKOUT1_DIV23            CFG3_CKOUT1DIV(22)                  /*!< CK_OUT1 is divided by 23 */
#define RCU_CKOUT1_DIV24            CFG3_CKOUT1DIV(23)                  /*!< CK_OUT1 is divided by 24 */
#define RCU_CKOUT1_DIV25            CFG3_CKOUT1DIV(24)                  /*!< CK_OUT1 is divided by 25 */
#define RCU_CKOUT1_DIV26            CFG3_CKOUT1DIV(25)                  /*!< CK_OUT1 is divided by 26 */
#define RCU_CKOUT1_DIV27            CFG3_CKOUT1DIV(26)                  /*!< CK_OUT1 is divided by 27 */
#define RCU_CKOUT1_DIV28            CFG3_CKOUT1DIV(27)                  /*!< CK_OUT1 is divided by 28 */
#define RCU_CKOUT1_DIV29            CFG3_CKOUT1DIV(28)                  /*!< CK_OUT1 is divided by 29 */
#define RCU_CKOUT1_DIV30            CFG3_CKOUT1DIV(29)                  /*!< CK_OUT1 is divided by 30 */
#define RCU_CKOUT1_DIV31            CFG3_CKOUT1DIV(30)                  /*!< CK_OUT1 is divided by 31 */
#define RCU_CKOUT1_DIV32            CFG3_CKOUT1DIV(31)                  /*!< CK_OUT1 is divided by 32 */
#define RCU_CKOUT1_DIV33            CFG3_CKOUT1DIV(32)                  /*!< CK_OUT1 is divided by 33 */
#define RCU_CKOUT1_DIV34            CFG3_CKOUT1DIV(33)                  /*!< CK_OUT1 is divided by 34 */
#define RCU_CKOUT1_DIV35            CFG3_CKOUT1DIV(34)                  /*!< CK_OUT1 is divided by 35 */
#define RCU_CKOUT1_DIV36            CFG3_CKOUT1DIV(35)                  /*!< CK_OUT1 is divided by 36 */
#define RCU_CKOUT1_DIV37            CFG3_CKOUT1DIV(36)                  /*!< CK_OUT1 is divided by 37 */
#define RCU_CKOUT1_DIV38            CFG3_CKOUT1DIV(37)                  /*!< CK_OUT1 is divided by 38 */
#define RCU_CKOUT1_DIV39            CFG3_CKOUT1DIV(38)                  /*!< CK_OUT1 is divided by 39 */
#define RCU_CKOUT1_DIV40            CFG3_CKOUT1DIV(39)                  /*!< CK_OUT1 is divided by 40 */
#define RCU_CKOUT1_DIV41            CFG3_CKOUT1DIV(40)                  /*!< CK_OUT1 is divided by 41 */
#define RCU_CKOUT1_DIV42            CFG3_CKOUT1DIV(41)                  /*!< CK_OUT1 is divided by 42 */
#define RCU_CKOUT1_DIV43            CFG3_CKOUT1DIV(42)                  /*!< CK_OUT1 is divided by 43 */
#define RCU_CKOUT1_DIV44            CFG3_CKOUT1DIV(43)                  /*!< CK_OUT1 is divided by 44 */
#define RCU_CKOUT1_DIV45            CFG3_CKOUT1DIV(44)                  /*!< CK_OUT1 is divided by 45 */
#define RCU_CKOUT1_DIV46            CFG3_CKOUT1DIV(45)                  /*!< CK_OUT1 is divided by 46 */
#define RCU_CKOUT1_DIV47            CFG3_CKOUT1DIV(46)                  /*!< CK_OUT1 is divided by 47 */
#define RCU_CKOUT1_DIV48            CFG3_CKOUT1DIV(47)                  /*!< CK_OUT1 is divided by 48 */
#define RCU_CKOUT1_DIV49            CFG3_CKOUT1DIV(48)                  /*!< CK_OUT1 is divided by 49 */
#define RCU_CKOUT1_DIV50            CFG3_CKOUT1DIV(49)                  /*!< CK_OUT1 is divided by 50 */
#define RCU_CKOUT1_DIV51            CFG3_CKOUT1DIV(50)                  /*!< CK_OUT1 is divided by 51 */
#define RCU_CKOUT1_DIV52            CFG3_CKOUT1DIV(51)                  /*!< CK_OUT1 is divided by 52 */
#define RCU_CKOUT1_DIV53            CFG3_CKOUT1DIV(52)                  /*!< CK_OUT1 is divided by 53 */
#define RCU_CKOUT1_DIV54            CFG3_CKOUT1DIV(53)                  /*!< CK_OUT1 is divided by 54 */
#define RCU_CKOUT1_DIV55            CFG3_CKOUT1DIV(54)                  /*!< CK_OUT1 is divided by 55 */
#define RCU_CKOUT1_DIV56            CFG3_CKOUT1DIV(55)                  /*!< CK_OUT1 is divided by 56 */
#define RCU_CKOUT1_DIV57            CFG3_CKOUT1DIV(56)                  /*!< CK_OUT1 is divided by 57 */
#define RCU_CKOUT1_DIV58            CFG3_CKOUT1DIV(57)                  /*!< CK_OUT1 is divided by 58 */
#define RCU_CKOUT1_DIV59            CFG3_CKOUT1DIV(58)                  /*!< CK_OUT1 is divided by 59 */
#define RCU_CKOUT1_DIV60            CFG3_CKOUT1DIV(59)                  /*!< CK_OUT1 is divided by 60 */
#define RCU_CKOUT1_DIV61            CFG3_CKOUT1DIV(60)                  /*!< CK_OUT1 is divided by 61 */
#define RCU_CKOUT1_DIV62            CFG3_CKOUT1DIV(61)                  /*!< CK_OUT1 is divided by 62 */
#define RCU_CKOUT1_DIV63            CFG3_CKOUT1DIV(62)                  /*!< CK_OUT1 is divided by 63 */
#define RCU_CKOUT1_DIV64            CFG3_CKOUT1DIV(63)                  /*!< CK_OUT1 is divided by 64 */
#endif /* GD32F130_150 */

#ifdef GD32F130_150
/* Deep-sleep mode voltage */
#define DSV_DSLPVS(regval)          (BITS(0,2) & ((uint32_t)(regval) << 0))
#define RCU_DEEPSLEEP_V_1_2         DSV_DSLPVS(0)                       /*!< core voltage is 1.2V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_1_1         DSV_DSLPVS(1)                       /*!< core voltage is 1.1V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_1_0         DSV_DSLPVS(2)                       /*!< core voltage is 1.0V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_0_9         DSV_DSLPVS(3)                       /*!< core voltage is 0.9V in deep-sleep mode */

/*Power down voltage select*/
#define RCU_PDR_V_2_6               (uint32_t)0x00000000U               /*!< power down voltage is 2.6V */
#define RCU_PDR_V_1_8               RCU_PDVSEL_PDRVS                    /*!< power down voltage is 1.8V */

#elif defined (GD32F170_190)
/* Deep-sleep mode voltage */
#define DSV_DSLPVS(regval)          (BITS(0,2) & ((uint32_t)(regval) << 0))
#define RCU_DEEPSLEEP_V_1_8         DSV_DSLPVS(0)                       /*!< core voltage is 1.8V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_1_6         DSV_DSLPVS(1)                       /*!< core voltage is 1.6V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_1_4         DSV_DSLPVS(2)                       /*!< core voltage is 1.4V in deep-sleep mode */
#define RCU_DEEPSLEEP_V_1_2         DSV_DSLPVS(3)                       /*!< core voltage is 1.2V in deep-sleep mode */
#endif /* GD32F130_150 */

/* function declarations */
/* deinitialize the RCU */
void rcu_deinit(void);
/* enable the peripherals clock */
void rcu_periph_clock_enable(rcu_periph_enum periph);
/* disable the peripherals clock */
void rcu_periph_clock_disable(rcu_periph_enum periph);
/* enable the peripherals clock when sleep mode */
void rcu_periph_clock_sleep_enable(rcu_periph_sleep_enum periph);
/* disable the peripherals clock when sleep mode */
void rcu_periph_clock_sleep_disable(rcu_periph_sleep_enum periph);
/* reset the peripherals */
void rcu_periph_reset_enable(rcu_periph_reset_enum periph_reset);
/* disable reset the peripheral */
void rcu_periph_reset_disable(rcu_periph_reset_enum periph_reset);
/* reset the BKP domain */
void rcu_bkp_reset_enable(void);
/* disable the BKP domain reset */
void rcu_bkp_reset_disable(void);

/* configure the system clock source */
void rcu_system_clock_source_config(uint32_t ck_sys);
/* get the system clock source */
uint32_t rcu_system_clock_source_get(void);
/* configure the AHB prescaler selection */
void rcu_ahb_clock_config(uint32_t ck_ahb);
/* configure the APB1 prescaler selection */
void rcu_apb1_clock_config(uint32_t ck_apb1);
/* configure the APB2 prescaler selection */
void rcu_apb2_clock_config(uint32_t ck_apb2);
/* configure the ADC clock source and prescaler selection */
void rcu_adc_clock_config(rcu_adc_clock_enum ck_adc);
#ifdef GD32F130_150
/* configure the USBD prescaler selection */
void rcu_usbd_clock_config(uint32_t ck_usbd);
/* configure the CK_OUT clock source and divider */
void rcu_ckout_config(uint32_t ckout_src, uint32_t ckout_div);
#elif defined (GD32F170_190)
/* configure the CK_OUT0 clock source and divider */
void rcu_ckout0_config(uint32_t ckout0_src, uint32_t ckout0_div);
/* configure the CK_OUT1 clock source and divider */
void rcu_ckout1_config(uint32_t ckout1_src, uint32_t ckout1_div);
#endif /* GD32F130_150 */
/* configure the PLL clock source selection and PLL multiply factor */
void rcu_pll_config(uint32_t pll_src, uint32_t pll_mul);
/* configure the USART clock source selection */
void rcu_usart_clock_config(uint32_t ck_usart);
/* configure the CEC clock source selection */
void rcu_cec_clock_config(uint32_t ck_cec);
/* configure the RTC clock source selection */
void rcu_rtc_clock_config(uint32_t rtc_clock_source);
#ifdef GD32F170_190
void rcu_slcd_clock_config(uint32_t slcd_clock_source);
#endif /* GD32F170_190 */
/* configure the HXTAL divider used as input of PLL */
void rcu_hxtal_prediv_config(uint32_t hxtal_prediv);
/* configure the LXTAL drive capability */
void rcu_lxtal_drive_capability_config(uint32_t lxtal_dricap);

/* get the clock stabilization and periphral reset flags */
FlagStatus rcu_flag_get(rcu_flag_enum flag);
/* clear the reset flag */
void rcu_all_reset_flag_clear(void);
/* get the clock stabilization interrupt and ckm flags */
FlagStatus rcu_interrupt_flag_get(rcu_int_flag_enum int_flag);
/* clear the interrupt flags */
void rcu_interrupt_flag_clear(rcu_int_flag_clear_enum int_flag_clear);
/* enable the stabilization interrupt */
void rcu_interrupt_enable(rcu_int_enum stab_int);
/* disable the stabilization interrupt */
void rcu_interrupt_disable(rcu_int_enum stab_int);

/* wait until oscillator stabilization flags is SET or oscillator startup is timeout */
ErrStatus rcu_osci_stab_wait(rcu_osci_type_enum osci);
/* turn on the oscillator */
void rcu_osci_on(rcu_osci_type_enum osci);
/* turn off the oscillator */
void rcu_osci_off(rcu_osci_type_enum osci);
/* enable the oscillator bypass mode, HXTALEN or LXTALEN must be reset before it  */
void rcu_osci_bypass_mode_enable(rcu_osci_type_enum osci);
/* disable the oscillator bypass mode, HXTALEN or LXTALEN must be reset before it */
void rcu_osci_bypass_mode_disable(rcu_osci_type_enum osci);
/* enable the HXTAL clock monitor */
void rcu_hxtal_clock_monitor_enable(void);
/* disable the HXTAL clock monitor */
void rcu_hxtal_clock_monitor_disable(void);

/* set the IRC8M adjust value */
void rcu_irc8m_adjust_value_set(uint8_t irc8m_adjval);
#ifdef GD32F130_150
/* set the IRC14M adjust value */
void rcu_irc14m_adjust_value_set(uint8_t irc14m_adjval);
#elif defined (GD32F170_190)
/* set the IRC28M adjust value */
void rcu_irc28m_adjust_value_set(uint8_t irc28m_adjval);
#endif /* GD32F130_150 */
/* unlock the voltage key */
void rcu_voltage_key_unlock(void);
/* set the deep sleep mode voltage */
void rcu_deepsleep_voltage_set(uint32_t dsvol);
#ifdef GD32F130_150
/* set the power down voltage */
void rcu_power_down_voltage_set(uint32_t pdvol);
#endif /* GD32F130_150 */

/* get the system clock, bus and peripheral clock frequency */
uint32_t rcu_clock_freq_get(rcu_clock_freq_enum clock);

#endif /* GD32F1X0_RCU_H */

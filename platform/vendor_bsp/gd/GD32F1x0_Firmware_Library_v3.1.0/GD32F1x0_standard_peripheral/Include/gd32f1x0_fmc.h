/*!
    \file  gd32f1x0_fmc.h
    \brief definitions for the FMC
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_FMC_H
#define GD32F1X0_FMC_H

#include "gd32f1x0.h"

/* FMC and option byte definition */
#define FMC                     FMC_BASE                    /*!< FMC register base address */
#define OB                      OB_BASE                     /*!< option byte base address */

/* registers definitions */
#define FMC_WS                  REG32((FMC) + 0x00U)        /*!< FMC wait state register */
#define FMC_KEY                 REG32((FMC) + 0x04U)        /*!< FMC unlock key register */
#define FMC_OBKEY               REG32((FMC) + 0x08U)        /*!< FMC option bytes unlock key register */
#define FMC_STAT                REG32((FMC) + 0x0CU)        /*!< FMC status register */
#define FMC_CTL                 REG32((FMC) + 0x10U)        /*!< FMC control register */
#define FMC_ADDR                REG32((FMC) + 0x14U)        /*!< FMC address register */
#define FMC_OBSTAT              REG32((FMC) + 0x1CU)        /*!< FMC option bytes status register */
#define FMC_WP                  REG32((FMC) + 0x20U)        /*!< FMC write protection register */
#define FMC_WSEN                REG32((FMC) + 0xFCU)        /*!< FMC wait state enable register  */
#define FMC_PID                 REG32((FMC) + 0x100U)       /*!< FMC product ID register */

#define OB_SPC                  REG16((OB) + 0x00U)         /*!< option byte security protection value */
#define OB_USER                 REG16((OB) + 0x02U)         /*!< option byte user value*/
#define OB_WP0                  REG16((OB) + 0x08U)         /*!< option byte write protection 0 */
#define OB_WP1                  REG16((OB) + 0x0AU)         /*!< option byte write protection 1 */

/* bits definitions */
/* FMC_WS */
#define FMC_WS_WSCNT            BITS(0,2)                   /*!< wait state counter */

/* FMC_KEY */
#define FMC_KEY_KEY             BITS(0,31)                  /*!< FMC main flash unlock key bits */

/* FMC_OBKEY */
#define FMC_OBKEY_OBKEY         BITS(0,31)                  /*!< option bytes unlock key bits */

/* FMC_STAT */
#define FMC_STAT_BUSY           BIT(0)                      /*!< flash busy flag bit */
#define FMC_STAT_PGERR          BIT(2)                      /*!< flash program error flag bit */
#define FMC_STAT_WPERR          BIT(4)                      /*!< flash write protection error flag bit */
#define FMC_STAT_ENDF           BIT(5)                      /*!< flash end of operation flag bit */

/* FMC_CTL */
#define FMC_CTL_PG              BIT(0)                      /*!< main flash program command bit */
#define FMC_CTL_PER             BIT(1)                      /*!< main flash page erase bit */
#define FMC_CTL_MER             BIT(2)                      /*!< main flash mass erase bit */
#define FMC_CTL_OBPG            BIT(4)                      /*!< option bytes program command bit */
#define FMC_CTL_OBER            BIT(5)                      /*!< option bytes erase command bit */
#define FMC_CTL_START           BIT(6)                      /*!< send erase command to FMC bit */
#define FMC_CTL_LK              BIT(7)                      /*!< flash lock bit */
#define FMC_CTL_OBWEN           BIT(9)                      /*!< option bytes erase/program enable bit */
#define FMC_CTL_ERRIE           BIT(10)                     /*!< error interrupt enable bit */
#define FMC_CTL_ENDIE           BIT(12)                     /*!< end of operation interrupt enable bit */
#define FMC_CTL_OBRLD           BIT(13)                     /*!< option bytes reload bit */

/* FMC_ADDR */
#define FMC_ADDR_ADDR           BITS(0,31)                  /*!< flash command address bits */

/* FMC_OBSTAT */
#define FMC_OBSTAT_OBERR        BIT(0)                      /*!< option bytes read error bit */
#define FMC_OBSTAT_PLVL_BIT0    BIT(1)                      /*!< protection level bit 0 */
#define FMC_OBSTAT_PLVL_BIT1    BIT(2)                      /*!< protection level bit 1 */
#define FMC_OBSTAT_USER         BITS(8,15)                  /*!< option bytes user bits */
#define FMC_OBSTAT_DATA         BITS(16,31)                 /*!< option byte data bits */

/* FMC_WSEN */
#define FMC_WSEN_WSEN           BIT(0)                      /*!< FMC wait state enable bit */
#ifdef GD32F170_190
#define FMC_WSEN_BPEN           BIT(1)                      /*!< FMC bit program enable bit */
#endif /* GD32F170_190 */

/* FMC_PID */
#define FMC_PID_PID             BITS(0,31)                  /*!< product ID bits */

/* constants definitions */
/* fmc state */
typedef enum
{
    FMC_READY,                                              /*!< the operation has been completed */
    FMC_BUSY,                                               /*!< the operation is in progress */
    FMC_PGERR,                                              /*!< program error */
    FMC_WPERR,                                              /*!< erase/program protection error */
    FMC_TOERR,                                              /*!< timeout error */
    FMC_OB_HSPC                                             /*!< option byte security protection code high */
}fmc_state_enum;

/* unlock key */
#define UNLOCK_KEY0                ((uint32_t)0x45670123U)  /*!< unlock key 0 */
#define UNLOCK_KEY1                ((uint32_t)0xCDEF89ABU)  /*!< unlock key 1 */

/* wait state counter value */
#define WS_WSCNT_0                 ((uint8_t)0x00U)         /*!< 0 wait state added */
#define WS_WSCNT_1                 ((uint8_t)0x01U)         /*!< 1 wait state added */
#define WS_WSCNT_2                 ((uint8_t)0x02U)         /*!< 2 wait state added */

/* read protect configure */
#define FMC_NSPC                   ((uint8_t)0xA5U)         /*!< no security protection */
#define FMC_LSPC                   ((uint8_t)0xBBU)         /*!< low security protection, any value except 0xA5 or 0xCC */
#define FMC_HSPC                   ((uint8_t)0xCCU)         /*!< high security protection */

/* option byte write protection */
#define OB_LWP                     ((uint32_t)0x000000FFU)  /*!< write protection low bits */
#define OB_HWP                     ((uint32_t)0x0000FF00U)  /*!< write protection high bits */

/* option byte software/hardware free watchdog timer */  
#define OBUSER_NWDG_HW(regval)     (BIT(0) & ((uint32_t)(regval) << 0))
#define OB_FWDGT_HW                OBUSER_NWDG_HW(0)        /*!< hardware free watchdog timer */
#define OB_FWDGT_SW                OBUSER_NWDG_HW(1)        /*!< software free watchdog timer */

/* option byte reset or not entering deep sleep mode */
#define OBUSER_NRST_DPSLP(regval)  (BIT(1) & ((uint32_t)(regval) << 1))
#define OB_DEEPSLEEP_RST           OBUSER_NRST_DPSLP(0)     /*!< generate a reset instead of entering deepsleep mode */
#define OB_DEEPSLEEP_NRST          OBUSER_NRST_DPSLP(1)     /*!< no reset when entering deepsleep mode */

/* option byte reset or not entering standby mode */
#define OBUSER_NRST_STDBY(regval)  (BIT(2) & ((uint32_t)(regval) << 2))
#define OB_STDBY_RST               OBUSER_NRST_STDBY(0)     /*!< generate a reset instead of entering standby mode */
#define OB_STDBY_NRST              OBUSER_NRST_STDBY(1)     /*!< no reset when entering deepsleep mode */

/* option byte OB_BOOT1_n set */
#define OBUSER_BOOT1_N(regval)     (BIT(4) & ((uint32_t)(regval) << 4))
#define OB_BOOT1_SET_1             OBUSER_BOOT1_N(0)        /*!< BOOT1 bit is 1 */
#define OB_BOOT1_SET_0             OBUSER_BOOT1_N(1)        /*!< BOOT1 bit is 0 */

/* option byte VDDA monitor enable/disable */
#define OBUSER_VDDA_VISOR(regval)  (BIT(5) & ((uint32_t)(regval) << 5))
#define OB_VDDA_DISABLE            OBUSER_VDDA_VISOR(0)     /*!< disable VDDA monitor */
#define OB_VDDA_ENABLE             OBUSER_VDDA_VISOR(1)     /*!< enable VDDA monitor */

/* option byte SRAM parity enable/disable */
#define OBUSER_SRAM_PARITY(regval) (BIT(6) & ((uint32_t)(regval) << 6))
#define OB_SRAM_PARITY_ENABLE      OBUSER_SRAM_PARITY(0)    /*!< enable SRAM parity check */
#define OB_SRAM_PARITY_DISABLE     OBUSER_SRAM_PARITY(1)    /*!< disable SRAM parity check */

/* option byte security protection level in FMC_OBSTAT register */
#define OB_OBSTAT_PLEVEL_NO        ((uint32_t)0x00000000U)  /*!< no security protection */
#define OB_OBSTAT_PLEVEL_LOW       ((uint32_t)0x00000002U)  /*!< low security protection */
#define OB_OBSTAT_PLEVEL_HIGH      ((uint32_t)0x00000006U)  /*!< high security protection */

/* option byte user mask */
#define OB_USER_MASK               ((uint8_t)0x88U)         /*!< OB_USER reserved bit mask */

/* option byte data address */
#define OB_DATA_ADDR0              ((uint32_t)0x1FFFF804U)  /*!< option byte data address 0 */
#define OB_DATA_ADDR1              ((uint32_t)0x1FFFF806U)  /*!< option byte data address 1 */

/* FMC flags */
#define FMC_FLAG_BUSY              FMC_STAT_BUSY            /*!< FMC busy flag */
#define FMC_FLAG_PGERR             FMC_STAT_PGERR           /*!< FMC programming error flag */
#define FMC_FLAG_WPERR             FMC_STAT_WPERR           /*!< FMC write protection error flag */
#define FMC_FLAG_END               FMC_STAT_ENDF            /*!< FMC end of programming flag */

/* FMC interrupt enable */
#define FMC_INTEN_END              FMC_CTL_ENDIE            /*!< enable FMC end of operation interrupt */
#define FMC_INTEN_ERR              FMC_CTL_ERRIE            /*!< enable FMC error interrupt */

/* FMC time out */
#define FMC_TIMEOUT_COUNT          ((uint32_t)0x000F0000U)  /*!< count to judge of FMC timeout */

/* function declarations */
/* FMC main memory programming functions */
/* unlock the main FMC operation */
void fmc_unlock(void);
/* lock the main FMC operation */
void fmc_lock(void);
/* set the wait state counter value */
void fmc_wscnt_set(uint8_t wscnt);
/* fmc wait state enable */
void fmc_wait_state_enable(void);
/* fmc wait state disable */
void fmc_wait_state_disable(void);
/* FMC erase page */
fmc_state_enum fmc_page_erase(uint32_t page_address);
/* FMC erase whole chip */
fmc_state_enum fmc_mass_erase(void);
/* FMC program a word at the corresponding address */
fmc_state_enum fmc_word_program(uint32_t address, uint32_t data);
/* FMC program a half word at the corresponding address */
fmc_state_enum fmc_halfword_program(uint32_t address, uint16_t data);
#ifdef GD32F170_190
/* FMC program a word at the corresponding address without erasing */
fmc_state_enum fmc_word_reprogram(uint32_t address, uint32_t data);
#endif /* GD32F170_190 */

/* FMC option bytes programming functions */
/* unlock the option byte operation */
void ob_unlock(void);
/* lock the option byte operation */
void ob_lock(void);
/* reload the option byte and generate a system reset */
void ob_reset(void);
/* erase option byte */
fmc_state_enum ob_erase(void);
/* enable option byte write protection (OB_WP) */
fmc_state_enum ob_write_protection_enable(uint32_t ob_wp);
/* configure read out protect */
fmc_state_enum ob_security_protection_config(uint8_t ob_spc);
/* write the FMC option byte user */
fmc_state_enum ob_user_write(uint8_t ob_user);
/* write the FMC option byte data */
fmc_state_enum ob_data_program(uint32_t address, uint8_t data);
/* get the FMC option byte OB_USER */
uint8_t ob_user_get(void);
/* get the FMC option byte OB_DATA */
uint16_t ob_data_get(void);
/* get the FMC option byte write protection */
uint16_t ob_write_protection_get(void);
/* get the value of FMC option byte security protection level (PLEVEL) in FMC_OBSTAT register */
uint32_t ob_obstat_plevel_get(void);

/* FMC interrupts and flags management functions */
/* enable FMC interrupt */
void fmc_interrupt_enable(uint32_t interrupt);
/* disable FMC interrupt */
void fmc_interrupt_disable(uint32_t interrupt);
/* get flag set or reset */
FlagStatus fmc_flag_get(uint32_t flag);
/* clear the FMC pending flag */
void fmc_flag_clear(uint32_t flag);
/* return the FMC state */
fmc_state_enum fmc_state_get(void);
/* check FMC ready or not */
fmc_state_enum fmc_ready_wait(uint32_t timeout);

#endif /* GD32F1X0_FMC_H */

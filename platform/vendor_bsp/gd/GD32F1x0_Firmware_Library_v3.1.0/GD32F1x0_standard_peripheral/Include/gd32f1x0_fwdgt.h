/*!
    \file  gd32f1x0_fwdgt.h
    \brief definitions for the FWDGT 
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_FWDGT_H
#define GD32F1X0_FWDGT_H

#include "gd32f1x0.h"

/* FWDGT definitions */
#define FWDGT                       FWDGT_BASE

/* registers definitions */
#define FWDGT_CTL                   REG32((FWDGT) + 0x00U)           /*!< FWDGT control register */
#define FWDGT_PSC                   REG32((FWDGT) + 0x04U)           /*!< FWDGT prescaler register */
#define FWDGT_RLD                   REG32((FWDGT) + 0x08U)           /*!< FWDGT reload register */
#define FWDGT_STAT                  REG32((FWDGT) + 0x0CU)           /*!< FWDGT status register */
#define FWDGT_WND                   REG32((FWDGT) + 0x10U)           /*!< FWDGT window register */

/* bits definitions */
/* FWDGT_CTL */
#define FWDGT_CTL_CMD               BITS(0,15)                      /*!< FWDGT command value */

/* FWDGT_PSC */
#define FWDGT_PSC_PSC               BITS(0,2)                       /*!< FWDGT prescaler divider value */

/* FWDGT_RLD */
#define FWDGT_RLD_RLD               BITS(0,11)                      /*!< FWDGT counter reload value */

/* FWDGT_STAT */
#define FWDGT_STAT_PUD              BIT(0)                          /*!< FWDGT prescaler divider value update */
#define FWDGT_STAT_RUD              BIT(1)                          /*!< FWDGT counter reload value update */
#define FWDGT_STAT_WUD              BIT(2)                          /*!< FWDGT counter window value update */

/* FWDGT_WND */
#define FWDGT_WND_WND               BITS(0,11)                      /*!< FWDGT counter window value */

/* constants definitions */
/* ctl register value */
#define CTL_CMD(regval)             (BITS(0,15) & ((uint32_t)(regval) << 0U))  /*!< write value to FWDGT_CTL_CMD bit field */

/* psc register value */
#define PSC_PSC(regval)             (BITS(0,2) & ((uint32_t)(regval) << 0U))
#define FWDGT_PSC_DIV4              ((uint8_t)PSC_PSC(0))           /*!< FWDGT prescaler set to 4 */
#define FWDGT_PSC_DIV8              ((uint8_t)PSC_PSC(1))           /*!< FWDGT prescaler set to 8 */
#define FWDGT_PSC_DIV16             ((uint8_t)PSC_PSC(2))           /*!< FWDGT prescaler set to 16 */
#define FWDGT_PSC_DIV32             ((uint8_t)PSC_PSC(3))           /*!< FWDGT prescaler set to 32 */
#define FWDGT_PSC_DIV64             ((uint8_t)PSC_PSC(4))           /*!< FWDGT prescaler set to 64 */
#define FWDGT_PSC_DIV128            ((uint8_t)PSC_PSC(5))           /*!< FWDGT prescaler set to 128 */
#define FWDGT_PSC_DIV256            ((uint8_t)PSC_PSC(6))           /*!< FWDGT prescaler set to 256 */

/* rld register value */
#define RLD_RLD(regval)             (BITS(0,11) & ((uint32_t)(regval) << 0U))  /*!< write value to FWDGT_RLD_RLD bit field */

/* wnd register value */
#define WND_WND(regval)             (BITS(0,11) & ((uint32_t)(regval) << 0U))  /*!< write value to FWDGT_WND_WND bit field */

/* control value */
#define FWDGT_WRITEACCESS_ENABLE    ((uint16_t)0x5555U)              /*!< FWDGT_CTL bits write access enable value */
#define FWDGT_WRITEACCESS_DISABLE   ((uint16_t)0x0000U)              /*!< FWDGT_CTL bits write access disable value */
#define FWDGT_KEY_RELOAD            ((uint16_t)0xAAAAU)              /*!< FWDGT_CTL bits fwdgt counter reload value */
#define FWDGT_KEY_ENABLE            ((uint16_t)0xCCCCU)              /*!< FWDGT_CTL bits fwdgt counter enable value */

/* FWDGT timeout value */
#define FWDGT_WND_TIMEOUT           ((uint32_t)0x000FFFFFU)          /*!< FWDGT_WND register write operation state flag timeout */
#define FWDGT_PSC_TIMEOUT           ((uint32_t)0x000FFFFFU)          /*!< FWDGT_PSC register write operation state flag timeout */
#define FWDGT_RLD_TIMEOUT           ((uint32_t)0x000FFFFFU)          /*!< FWDGT_RLD register write operation state flag timeout */

/* FWDGT flag definitions */
#define FWDGT_FLAG_PUD              FWDGT_STAT_PUD                   /*!< a write operation to FWDGT_PSC register is on going */
#define FWDGT_FLAG_RUD              FWDGT_STAT_RUD                   /*!< a write operation to FWDGT_RLD register is on going */
#define FWDGT_FLAG_WUD              FWDGT_STAT_WUD                   /*!< a write operation to FWDGT_WND register is on going */

/* function declarations */
/* disable write access to FWDGT_PSC,FWDGT_RLD and FWDGT_WND */
void fwdgt_write_disable(void);
/* start the free watchdog timer counter */
void fwdgt_enable(void);

/* configure the free watchdog timer counter window value */
ErrStatus fwdgt_window_value_config(uint16_t window_value);
/* reload the counter of FWDGT */
void fwdgt_counter_reload(void);
/* configure counter reload value, and prescaler divider value */
ErrStatus fwdgt_config(uint16_t reload_value, uint8_t prescaler_div);

/* get flag state of FWDGT */
FlagStatus fwdgt_flag_get(uint16_t flag);

#endif /* GD32F1X0_FWDGT_H */

/*!
    \file  gd32f1x0_pmu.h
    \brief definitions for the PMU
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_PMU_H
#define GD32F1X0_PMU_H

#include "gd32f1x0.h"

/* PMU definitions */
#define PMU                           PMU_BASE

/* registers definitions */
#define PMU_CTL                       REG32((PMU) + 0x00U)      /*!< PMU control register */
#define PMU_CS                        REG32((PMU) + 0x04U)      /*!< PMU control and status register */
                                                               
/* bits definitions */
/* PMU_CTL */
#define PMU_CTL_LDOLP                 BIT(0)                   /*!< ldo low power mode */
#define PMU_CTL_STBMOD                BIT(1)                   /*!< standby mode */
#define PMU_CTL_WURST                 BIT(2)                   /*!< wakeup flag reset */
#define PMU_CTL_STBRST                BIT(3)                   /*!< standby flag reset */
#define PMU_CTL_LVDEN                 BIT(4)                   /*!< low voltage detector enable */
#define PMU_CTL_LVDT                  BITS(5,7)                /*!< low voltage detector threshold */
#define PMU_CTL_BKPWEN                BIT(8)                   /*!< backup domain write enable */

/* PMU_CS */
#define PMU_CS_WUF                    BIT(0)                   /*!< wakeup flag */
#define PMU_CS_STBF                   BIT(1)                   /*!< standby flag */
#define PMU_CS_LVDF                   BIT(2)                   /*!< low voltage detector status flag */
#define PMU_CS_WUPEN0                 BIT(8)                   /*!< wakeup pin 0 enable */
#define PMU_CS_WUPEN1                 BIT(9)                   /*!< wakeup pin 1 enable */

/* constants definitions */
/* PMU low voltage detector threshold definitions */
#define CTL_LVDT(regval)              (BITS(5,7)&((uint32_t)(regval) << 5))
#define PMU_LVDT_0                    CTL_LVDT(0)              /*!< voltage threshold is 2.2V */
#define PMU_LVDT_1                    CTL_LVDT(1)              /*!< voltage threshold is 2.3V */
#define PMU_LVDT_2                    CTL_LVDT(2)              /*!< voltage threshold is 2.4V */
#define PMU_LVDT_3                    CTL_LVDT(3)              /*!< voltage threshold is 2.5V */
#define PMU_LVDT_4                    CTL_LVDT(4)              /*!< voltage threshold is 2.6V */
#define PMU_LVDT_5                    CTL_LVDT(5)              /*!< voltage threshold is 2.7V */
#define PMU_LVDT_6                    CTL_LVDT(6)              /*!< voltage threshold is 2.8V */
#define PMU_LVDT_7                    CTL_LVDT(7)              /*!< voltage threshold is 2.9V */

/* PMU flag definitions */
#define PMU_FLAG_WAKEUP               PMU_CS_WUF               /*!< wakeup flag status */
#define PMU_FLAG_STANDBY              PMU_CS_STBF              /*!< standby flag status */
#define PMU_FLAG_LVD                  PMU_CS_LVDF              /*!< lvd flag status */

/* PMU ldo definitions */
#define PMU_LDO_NORMAL                ((uint32_t)0x00000000)   /*!< LDO normal work when pmu enter deepsleep mode */
#define PMU_LDO_LOWPOWER              PMU_CTL_LDOLP            /*!< LDO work at low power status when pmu enter deepsleep mode */

/* PMU flag reset definitions */
#define PMU_FLAG_RESET_WAKEUP         ((uint8_t)0x00)          /*!< wakeup flag reset */
#define PMU_FLAG_RESET_STANDBY        ((uint8_t)0x01)          /*!< standby flag reset */

/* PMU command constants definitions */
#define WFI_CMD                       ((uint8_t)0x00)          /*!< use WFI command */
#define WFE_CMD                       ((uint8_t)0x01)          /*!< use WFE command */

/* PMU wakeup pin definitions */
#define PMU_WAKEUP_PIN0               PMU_CS_WUPEN0            /*!< wakeup pin 0 */
#define PMU_WAKEUP_PIN1               PMU_CS_WUPEN1            /*!< wakeup pin 1 */

/* function declarations */

/* PMU reset */
void pmu_deinit(void);
/* select low voltage detector threshold */
void pmu_lvd_select(uint32_t lvdt_n);
/* PMU lvd disable */
void pmu_lvd_disable(void);

/* PMU work at sleep mode */
void pmu_to_sleepmode(uint8_t sleepmodecmd);
/* PMU work at deepsleep mode */
void pmu_to_deepsleepmode(uint32_t ldo,uint8_t deepsleepmodecmd);
/* PMU work at standby mode */
void pmu_to_standbymode(uint8_t standbymodecmd);

/* reset flag bit */
void pmu_flag_clear(uint32_t flag_reset);
/* get flag state */
FlagStatus pmu_flag_get(uint32_t flag);

/* PMU backup domain write enable */
void pmu_backup_write_enable(void);
/* PMU backup domain write disable */
void pmu_backup_write_disable(void);

/* wakeup pin enable */
void pmu_wakeup_pin_enable(uint32_t wakeup_pin);
/* wakeup pin disable */
void pmu_wakeup_pin_disable(uint32_t wakeup_pin);

#endif /* GD32F1X0_PMU_H */

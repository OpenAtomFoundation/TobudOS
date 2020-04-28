/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include "tos_k.h"

__STATIC__ gic_data_t gic_data[GIC_NRS];

__STATIC_INLINE__ cpu_addr_t gic_base_get(void)
{
    /* read CBAR */
    return (ARM_MRC(15, 4, 15, 0, 0) & 0xFFFF0000UL);
}

__STATIC__ void gic_data_init(gic_data_t *gic)
{
    cpu_addr_t gic_base;

    gic_base = gic_base_get();

    gic->dist   = (gic_dist_t *)(gic_base + GICD_OFFSET);   /* distributor */
    gic->cpu    = (gic_cpu_t *)(gic_base + GICC_OFFSET);    /* cpu interface */
}

__STATIC__ void gic_dist_init(gic_dist_t *gic_dist)
{
    uint32_t i = 0, int_id_max = 0;

    // let's make it simple here, maybe SMP someday, but not now
#define IT_LINES_NUMBER(d_type) ((d_type) & 0x1FUL)
    int_id_max = IT_LINES_NUMBER(gic_dist->D_TYPER) + 1;

    /* Disable all PPI, SGI and SPI */
    for (i = 0; i < int_id_max; ++i) {
        gic_dist->D_ICENABLER[i] = 0xFFFFFFFFUL;
    }

    /* Enable group0 distribution */
    gic_dist->D_CTLR = 1UL;
}

__STATIC__ void gic_cpu_init(gic_cpu_t *gic_cpu)
{
    /* Make all interrupts have higher priority */
    gic_cpu->C_PMR = (0xFFUL << (8 - GIC_PRIO_BITS)) & 0xFFUL;

    /* No subpriority, all priority level allows preemption */
    gic_cpu->C_BPR = 7 - GIC_PRIO_BITS;

    /* Enable group0 signaling */
    gic_cpu->C_CTLR = 1UL;
}

__STATIC_INLINE__ gic_data_t *gic_get(uint32_t gic_nr)
{
    gic_data_t *gic;

    if (gic_nr >= GIC_NRS) {
        return K_NULL;
    }

    gic = &gic_data[gic_nr];

    if (gic->cpu && gic->dist) {
        return gic;
    }

    return K_NULL;
}

__KNL__ uint32_t gic_interrupt_id_get(uint32_t gic_nr)
{
    gic_data_t *gic = gic_get(gic_nr);

    if (gic) {
#define INTERRUPT_ID(iar)   ((iar) & 0x3FFUL)
        return INTERRUPT_ID(gic->cpu->C_IAR);
    }

    return (uint32_t)-1;
}

__KNL__ void gic_interrupt_end(uint32_t gic_nr, uint32_t vector)
{
    gic_data_t *gic = gic_get(gic_nr);

    if (gic) {
        gic->cpu->C_EOIR = vector;
    }
}

__KNL__ int gic_init(uint32_t gic_nr)
{
    gic_data_t *gic;

    if (gic_nr >= GIC_NRS) {
        return -1;
    }

    gic = &gic_data[gic_nr];

    gic_data_init(gic);
    gic_dist_init(gic->dist);
    gic_cpu_init(gic->cpu);

    return 0;
}

__KNL__ void gic_interrupt_enable(uint32_t gic_nr, uint32_t vector)
{
    int reg;
    uint32_t mask;
    gic_data_t *gic;

    gic = gic_get(gic_nr);
    if (gic) {
        reg = vector / 32;
        mask = 1ULL << (vector % 32);

        gic->dist->D_ISENABLER[reg] = mask;
    }
}

__KNL__ void gic_interrupt_disable(uint32_t gic_nr, uint32_t vector)
{
    int reg;
    uint32_t mask;
    gic_data_t *gic;

    gic = gic_get(gic_nr);
    if (gic) {
        reg = vector / 32;
        mask = 1ULL << (vector % 32);
        gic->dist->D_ICENABLER[reg] = mask;
    }
}


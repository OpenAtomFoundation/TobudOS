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

#include <tos_compiler.h>
#include <tos_ktypes.h>

#define ECLIC_ADDR_BASE         0xD2000000
#define CLIC_INT_TMR            0x07

#define ECLIC_CFG_OFFSET        0x00
#define ECLIC_INFO_OFFSET       0x04
#define ECLIC_MTH_OFFSET        0x0B


#define ECLIC_INT_IP_OFFSET     0x1000
#define ECLIC_INT_IE_OFFSET     0x1001
#define ECLIC_INT_ATTR_OFFSET   0x1002
#define ECLIC_INT_CTRL_OFFSET   0x1003


#define ECLIC_CFG_NLBITS_MASK   0x1E
#define ECLIC_CFG_NLBITS_LSB    1


static uint8_t eclic_get_clic_int_ctl_bits() {
    uint32_t bits = *(volatile uint32_t*)(ECLIC_ADDR_BASE+ECLIC_INFO_OFFSET);
    bits >>= 21;
    return (uint8_t) bits;
}


static uint8_t eclic_get_nlbits() {
    uint8_t nlbits = *(volatile uint8_t*)(ECLIC_ADDR_BASE+ECLIC_CFG_OFFSET);
    nlbits = (nlbits & ECLIC_CFG_NLBITS_MASK) >> ECLIC_CFG_NLBITS_LSB;

    uint8_t cicbits = eclic_get_clic_int_ctl_bits();
    if(nlbits > cicbits) {
        nlbits = cicbits;
    }

    return nlbits;
}

static uint8_t eclic_get_intctrl(uint32_t source) {
    return *(volatile uint8_t*)(ECLIC_ADDR_BASE+ECLIC_INT_CTRL_OFFSET+source*4);
}

static void    eclic_set_intctrl(uint32_t source, uint8_t v) {
    *(volatile uint8_t*)(ECLIC_ADDR_BASE+ECLIC_INT_CTRL_OFFSET+source*4) = v;
}


static void eclic_enable_interrupt(uint32_t source) {
    *(volatile uint8_t*)(ECLIC_ADDR_BASE+ECLIC_INT_IE_OFFSET+source*4) = 1;
}

static void eclic_set_irq_level(uint32_t source, uint8_t level) {
    uint8_t nlbits = eclic_get_nlbits();

    if (nlbits == 0) {
        return ;
    }

    uint8_t intctrl_val = eclic_get_intctrl(CLIC_INT_TMR);

    intctrl_val <<= nlbits;
    intctrl_val >>= nlbits;
    intctrl_val  |= (level << (8- nlbits));

    eclic_set_intctrl(CLIC_INT_TMR, intctrl_val);
}

static void eclic_set_irq_priority(uint32_t source, uint8_t priority) {
    uint8_t nlbits = eclic_get_nlbits();
    uint8_t cicbits= eclic_get_clic_int_ctl_bits();

    if (nlbits >= cicbits) {
        return ;
    }


    uint8_t pad   = ~0;
    pad         <<= cicbits;
    pad         >>= cicbits;


    uint8_t intctrl_val = eclic_get_intctrl(CLIC_INT_TMR);

    intctrl_val >>= (8 - nlbits);
    intctrl_val <<= (8 - nlbits);
    intctrl_val  |= (priority << (8 - cicbits));
    intctrl_val  |= pad;

    eclic_set_intctrl(CLIC_INT_TMR, intctrl_val);
}

void rv32_exception_entry();
__PORT__ void port_cpu_init() {

    __ASM__ __VOLATILE__("csrw mtvec, %0"::"r"(rv32_exception_entry));

    // MTVT2: 0x7EC
    // use mtvec as entry of irq and other trap
    __ASM__ __VOLATILE__("csrc 0x7EC, 0x1");

    eclic_enable_interrupt(CLIC_INT_TMR);

    eclic_set_irq_level(CLIC_INT_TMR, 0);

}

__PORT__ void port_systick_priority_set(uint32_t priority) {
    eclic_set_irq_priority(CLIC_INT_TMR, priority);
}


__PORT__ void *port_get_irq_vector_table() {
    void *base = 0;

    // MTVT:  0x307
    __ASM__ __VOLATILE__("csrr %0, 0x307":"=r"(base));

    return base;
}

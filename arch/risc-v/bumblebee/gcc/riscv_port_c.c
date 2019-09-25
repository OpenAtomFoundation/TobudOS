#include "gd32vf103.h"

void riscv_cpu_init() {
    eclic_irq_enable(CLIC_INT_TMR, 0, 0);
}

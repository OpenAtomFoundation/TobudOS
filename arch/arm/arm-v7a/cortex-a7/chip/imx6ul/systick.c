#include "tos_k.h"
#include "imx6ul.h"

static void epit1_init(unsigned int frac, unsigned int value)
{
    if (frac > 0XFFF) {
        frac = 0XFFF;
    }

    EPIT1->CR = 0;

    EPIT1->CR = (1 << 24 | frac << 4 | 1 << 3 | 1 << 2 | 1 << 1);

    EPIT1->LR = value;
    EPIT1->CMPR = 0;

    GIC_EnableIRQ(EPIT1_IRQn);

    EPIT1->CR |= 1 << 0;
}

void systick_handler(void *arg)
{
    if (EPIT1->SR & (1 << 0)) {
        tos_tick_handler();
    }

    EPIT1->SR |= 1 << 0;
}

void systick_init(k_tick_t tick_per_second)
{
    epit1_init(0, 66000000 / tick_per_second);

    tos_interrupt_handler_register(EPIT1_IRQn, (int_handler_t)systick_handler, NULL);
}


#include "tos_k.h"
#include "imx6ul.h"

/* set the core clock to 528MHz */
void clock_init(void)
{
    unsigned int reg = 0;

    if ((((CCM->CCSR) >> 2) & 0x1) == 0) {
        CCM->CCSR &= ~(1 << 8);
        CCM->CCSR |= (1 << 2);
    }

    CCM_ANALOG->PLL_ARM = (1 << 13) | ((88 << 0) & 0X7F);
    CCM->CCSR &= ~(1 << 2);
    CCM->CACRR = 1;

    reg = CCM_ANALOG->PFD_528;
    reg &= ~(0X3F3F3F3F);
    reg |= 32 << 24;
    reg |= 24 << 16;
    reg |= 16 << 8;
    reg |= 27 << 0;
    CCM_ANALOG->PFD_528 = reg;

    reg = 0;
    reg = CCM_ANALOG->PFD_480;
    reg &= ~(0X3F3F3F3F);
    reg |= 19 << 24;
    reg |= 17 << 16;
    reg |= 16 << 8;
    reg |= 12 << 0;
    CCM_ANALOG->PFD_480 = reg;

    CCM->CBCMR &= ~(3 << 18);
    CCM->CBCMR |= (1 << 18);
    CCM->CBCDR &= ~(1 << 25);
    while(CCM->CDHIPR & (1 << 5)) {
        ;
    }

    CCM->CBCDR &= ~(3 << 8);
    CCM->CBCDR |= 1 << 8;

    CCM->CSCMR1 &= ~(1 << 6);
    CCM->CSCMR1 &= ~(7 << 0);

    CCM->CSCDR1 &= ~(1 << 6);
    CCM->CSCDR1 &= ~0X3F;
}

void clock_enable(void)
{
    CCM->CCGR0 = 0XFFFFFFFF;
    CCM->CCGR1 = 0XFFFFFFFF;
    CCM->CCGR2 = 0XFFFFFFFF;
    CCM->CCGR3 = 0XFFFFFFFF;
    CCM->CCGR4 = 0XFFFFFFFF;
    CCM->CCGR5 = 0XFFFFFFFF;
    CCM->CCGR6 = 0XFFFFFFFF;
}


/**************************************************************************//**
 * @file     sc.c
 * @version  V3.00
 * @brief    Smart Card(SC) driver source file
 *
 * @note
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"

/* Below are variables used locally by SC driver and does not want to parse by doxygen unless HIDDEN_SYMBOLS is defined */
/** @cond HIDDEN_SYMBOLS */
static uint32_t u32CardStateIgnore[SC_INTERFACE_NUM] = {0UL};

/** @endcond HIDDEN_SYMBOLS */

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SC_Driver SC Driver
  @{
*/

/** @addtogroup SC_EXPORTED_FUNCTIONS SC Exported Functions
  @{
*/

/**
  * @brief      Check Smartcard Slot Status
  *
  * @param[in]  psSC    The pointer of smartcard module.
  *
  * @retval     TRUE    Card insert
  * @retval     FALSE   Card remove
  *
  * @details    This function is used to check if specified smartcard slot is presented.
  */
uint32_t SC_IsCardInserted(SC_T *psSC)
{
    uint32_t ret;

    /* put conditions into two variable to remove IAR compilation warning */
    uint32_t cond1 = ((psSC->STATUS & SC_STATUS_CDPINSTS_Msk) >> SC_STATUS_CDPINSTS_Pos);
    uint32_t cond2 = ((psSC->CTL & SC_CTL_CDLV_Msk) >> SC_CTL_CDLV_Pos);

    if ((psSC == SC0) && (u32CardStateIgnore[0] == 1UL))
    {
        ret = (uint32_t)TRUE;
    }
    else if (cond1 != cond2)
    {
        ret = (uint32_t)FALSE;
    }
    else
    {
        ret = (uint32_t)TRUE;
    }

    return ret;
}

/*
  * @brief      Reset the Tx/Rx FIFO
  *
  * @param[in]  psSC    The pointer of smartcard module.
  *
  * @return     None
  *
  * @details    This function reset both transmit and receive FIFO of specified smartcard module.
  */
void SC_ClearFIFO(SC_T *psSC)
{
    while (psSC->ALTCTL & SC_ALTCTL_SYNC_Msk)
    {
        ;
    }

    psSC->ALTCTL |= (SC_ALTCTL_TXRST_Msk | SC_ALTCTL_RXRST_Msk);
}

/**
  * @brief      Disable specified Smartcard
  *
  * @param[in]  psSC    The pointer of smartcard module.
  *
  * @return     None
  *
  * @details    SC will force all transition to IDLE state.
  */
void SC_Close(SC_T *psSC)
{
    psSC->INTEN = 0UL;

    while (psSC->PINCTL & SC_PINCTL_SYNC_Msk)
    {
        ;
    }

    psSC->PINCTL = 0UL;
    psSC->ALTCTL = 0UL;

    while (psSC->CTL & SC_CTL_SYNC_Msk)
    {
        ;
    }

    psSC->CTL = 0UL;
}

/**
  * @brief      Initialized Smartcard
  *
  * @param[in]  psSC    The pointer of smartcard module.
  * @param[in]  u32CardDet   Card detect polarity, select the SC_CD pin state which indicates card insert. Could be:
  *                         -\ref SC_PIN_STATE_HIGH
  *                         -\ref SC_PIN_STATE_LOW
  *                         -\ref SC_PIN_STATE_IGNORE, no card detect pin, always assumes card present.
  * @param[in]  u32PWR  Power on polarity, select the SC_PWR pin state which could set smartcard VCC to high level. Could be:
  *                         -\ref SC_PIN_STATE_HIGH
  *                         -\ref SC_PIN_STATE_LOW
  *
  * @return     None
  *
  * @details    Initialization process configures smartcard and enables engine clock.
  */
void SC_Open(SC_T *psSC, uint32_t u32CardDet, uint32_t u32PWR)
{
    uint32_t u32Reg = 0UL, u32Intf;

    if (psSC == SC0)
    {
        u32Intf = 0UL;
    }
    else
    {
        u32Intf = 2UL;
    }

    if (u32CardDet != SC_PIN_STATE_IGNORE)
    {
        u32Reg = u32CardDet ? 0UL : SC_CTL_CDLV_Msk;
        u32CardStateIgnore[u32Intf] = 0UL;
    }
    else
    {
        u32CardStateIgnore[u32Intf] = 1UL;
    }

    psSC->PINCTL = u32PWR ? 0UL : SC_PINCTL_PWRINV_Msk;

    while (psSC->CTL & SC_CTL_SYNC_Msk)
    {
        ;
    }

    psSC->CTL = SC_CTL_SCEN_Msk | u32Reg;
}

/**
  * @brief      Reset specified Smartcard
  *
  * @param[in]  psSC    The pointer of smartcard module.
  *
  * @return     None
  *
  * @details    Reset the Tx/Rx FIFO, clock and initial default parameter.
  */
void SC_ResetReader(SC_T *psSC)
{
    uint32_t u32Intf;

    if (psSC == SC0)
    {
        u32Intf = 0UL;
    }
    else
    {
        u32Intf = 2UL;
    }

    /* Reset FIFO, enable auto de-activation while card removal */
    psSC->ALTCTL |= (SC_ALTCTL_TXRST_Msk | SC_ALTCTL_RXRST_Msk | SC_ALTCTL_ADACEN_Msk);

    /* Set Rx trigger level to 1 character, longest card detect debounce period, disable error retry (EMV ATR does not use error retry) */
    while (psSC->CTL & SC_CTL_SYNC_Msk)
    {
        ;
    }

    psSC->CTL &= ~(SC_CTL_RXTRGLV_Msk |
                   SC_CTL_CDDBSEL_Msk |
                   SC_CTL_TXRTY_Msk |
                   SC_CTL_TXRTYEN_Msk |
                   SC_CTL_RXRTY_Msk |
                   SC_CTL_RXRTYEN_Msk);

    while (psSC->CTL & SC_CTL_SYNC_Msk)
    {
        ;
    }

    /* Enable auto convention, and all three smartcard internal timers */
    psSC->CTL |= SC_CTL_AUTOCEN_Msk | SC_CTL_TMRSEL_Msk;
    /* Disable Rx timeout */
    psSC->RXTOUT = 0UL;
    /* 372 clocks per ETU by default */
    psSC->ETUCTL = 371UL;

    /* Enable necessary interrupt for smartcard operation */
    if (u32CardStateIgnore[u32Intf])   /* Do not enable card detect interrupt if card present state ignore */
    {
        psSC->INTEN = (SC_INTEN_RDAIEN_Msk |
                       SC_INTEN_TERRIEN_Msk |
                       SC_INTEN_TMR0IEN_Msk |
                       SC_INTEN_TMR1IEN_Msk |
                       SC_INTEN_TMR2IEN_Msk |
                       SC_INTEN_BGTIEN_Msk |
                       SC_INTEN_ACERRIEN_Msk);
    }
    else
    {
        psSC->INTEN = (SC_INTEN_RDAIEN_Msk |
                       SC_INTEN_TERRIEN_Msk |
                       SC_INTEN_TMR0IEN_Msk |
                       SC_INTEN_TMR1IEN_Msk |
                       SC_INTEN_TMR2IEN_Msk |
                       SC_INTEN_BGTIEN_Msk |
                       SC_INTEN_ACERRIEN_Msk |
                       SC_INTEN_CDIEN_Msk);
    }

    return;
}

/**
  * @brief      Set Block Guard Time
  *
  * @param[in]  psSC      The pointer of smartcard module.
  * @param[in]  u32BGT  Block guard time using ETU as unit, valid range are between 1 ~ 32.
  *
  * @return     None
  *
  * @details    This function is used to configure block guard time (BGT) of specified smartcard module.
  */
void SC_SetBlockGuardTime(SC_T *psSC, uint32_t u32BGT)
{
    psSC->CTL = (psSC->CTL & ~SC_CTL_BGT_Msk) | ((u32BGT - 1UL) << SC_CTL_BGT_Pos);
}

/**
  * @brief      Set Character Guard Time
  *
  * @param[in]  psSC    The pointer of smartcard module.
  * @param[in]  u32CGT  Character guard time using ETU as unit, valid range are between 11 ~ 267.
  *
  * @return     None
  *
  * @details    This function is used to configure character guard time (CGT) of specified smartcard module.
  * @note       Before using this API, user should set the correct stop bit length first.
  */
void SC_SetCharGuardTime(SC_T *psSC, uint32_t u32CGT)
{
    /* CGT is "START bit" + "8-bits" + "Parity bit" + "STOP bit(s)" + "EGT counts" */
    u32CGT -= psSC->CTL & SC_CTL_NSB_Msk ? 11UL : 12UL;
    psSC->EGT = u32CGT;
}

/**
  * @brief      Stop all Timer Counting
  *
  * @param[in]  psSC    The pointer of smartcard module.
  *
  * @return     None
  *
  * @details    This function stop all smartcard timer of specified smartcard module.
  * @note       This function stop the timers within smartcard module, \b not timer module.
  */
void SC_StopAllTimer(SC_T *psSC)
{
    while (psSC->ALTCTL & SC_ALTCTL_SYNC_Msk)
    {
        ;
    }

    psSC->ALTCTL &= ~(SC_ALTCTL_CNTEN0_Msk | SC_ALTCTL_CNTEN1_Msk | SC_ALTCTL_CNTEN2_Msk);
}

/**
  * @brief      Configure and Start specified Timer
  *
  * @param[in]  psSC        The pointer of smartcard module.
  * @param[in]  u32TimerNum Specify time channel to start. Valid values are 0, 1, 2.
  * @param[in]  u32Mode     Timer operating mode, valid values are:
  *                             - \ref SC_TMR_MODE_0
  *                             - \ref SC_TMR_MODE_1
  *                             - \ref SC_TMR_MODE_2
  *                             - \ref SC_TMR_MODE_3
  *                             - \ref SC_TMR_MODE_4
  *                             - \ref SC_TMR_MODE_5
  *                             - \ref SC_TMR_MODE_6
  *                             - \ref SC_TMR_MODE_7
  *                             - \ref SC_TMR_MODE_8
  *                             - \ref SC_TMR_MODE_F
  * @param[in]  u32ETUCount Timer timeout duration, ETU based. For timer 0, valid  range are between 1 ~ 0x1000000 ETUs.
  *                         For timer 1 and timer 2, valid range are between 1 ~ 0x100 ETUs.
  *
  * @return     None
  *
  * @details    Enable Timer starting, counter will count when condition match.
  * @note       This function start the timer within smartcard module, \b not timer module.
  * @note       Depend on the timer operating mode, timer may not start counting immediately.
  */
void SC_StartTimer(SC_T *psSC, uint32_t u32TimerNum, uint32_t u32Mode, uint32_t u32ETUCount)
{
    uint32_t reg = u32Mode | (SC_TMRCTL0_CNT_Msk & (u32ETUCount - 1UL));

    /* before to set CNTEN0/1/2 of reg. ALTCTL, SCEN bit must be enabled */
    if (u32TimerNum <= 2)
    {
        while (psSC->CTL & SC_CTL_SYNC_Msk) {};

        psSC->CTL |= SC_CTL_TMRSEL_Msk | SC_CTL_SCEN_Msk;

        while (psSC->CTL & SC_CTL_SYNC_Msk) {};

        while (psSC->ALTCTL & SC_ALTCTL_SYNC_Msk) {};
    }

    switch (u32TimerNum)
    {
    case 0UL:
        while (psSC->TMRCTL0 & SC_TMRCTL0_SYNC_Msk) {};

        psSC->TMRCTL0 = reg;

        psSC->ALTCTL |= SC_ALTCTL_CNTEN0_Msk;

        break;

    case 1UL:
        while (psSC->TMRCTL1 & SC_TMRCTL1_SYNC_Msk) {};

        psSC->TMRCTL1 = reg;

        psSC->ALTCTL |= SC_ALTCTL_CNTEN1_Msk;

        break;

    case 2UL:
        while (psSC->TMRCTL2 & SC_TMRCTL2_SYNC_Msk) {};

        psSC->TMRCTL2 = reg;

        psSC->ALTCTL |= SC_ALTCTL_CNTEN2_Msk;

        break;

    default:
        break;
    }
}

/**
  * @brief      Stop specified Timer Counting
  *
  * @param[in]  psSC        The pointer of smartcard module.
  * @param[in]  u32TimerNum Specify timer channel to stop. Valid values are 0, 1, 2.
  *
  * @return     None
  *
  * @details    This function stop a smartcard timer of specified smartcard module.
  * @note       This function stop the timer within smartcard module, \b not timer module.
  */
void SC_StopTimer(SC_T *psSC, uint32_t u32TimerNum)
{
    while (psSC->ALTCTL & SC_ALTCTL_SYNC_Msk) {}

    if (u32TimerNum == 0UL)     /* timer 0 */
    {
        psSC->ALTCTL &= ~SC_ALTCTL_CNTEN0_Msk;
    }
    else if (u32TimerNum == 1UL)     /* timer 1 */
    {
        psSC->ALTCTL &= ~SC_ALTCTL_CNTEN1_Msk;
    }
    else                        /* timer 2 */
    {
        psSC->ALTCTL &= ~SC_ALTCTL_CNTEN2_Msk;
    }
}

/**
  * @brief      Get specified Smartcard Clock Frequency
  *
  * @param[in]  psSC  The pointer of smartcard module.
  *
  * @return     Smartcard frequency in kHZ
  *
  * @details    This function is used to get specified smartcard module clock frequency in kHz.
  */
uint32_t SC_GetInterfaceClock(SC_T *psSC)
{
    uint32_t u32ClkSrc, u32Num, u32Clk = __HIRC, u32Div;

    /* Get smartcard module clock source and divider */
    if (psSC == SC0)
    {
        u32Num = 0UL;
        u32ClkSrc = CLK_GetModuleClockSource(SC0_MODULE);
        u32Div = CLK_GetModuleClockDivider(SC0_MODULE);
        //  u32ClkSrc = __HXT;
        //  u32Div = SC0->ETUCTL&SC_ETUCTL_ETURDIV_Msk;
    }
    else
    {
        u32Clk = 0UL;
    }

    if (u32Clk == 0UL)
    {
        ; /* Invalid sc port */
    }
    else
    {
        /* Get smartcard module clock */
        if (u32ClkSrc == 0UL)
        {
            u32Clk = __HXT;
        }
        else if (u32ClkSrc == 1UL)
        {
            u32Clk = CLK_GetPLLClockFreq();
        }
        else if (u32ClkSrc == 2UL)
        {
            if (u32Num == 1UL)
            {
                u32Clk = CLK_GetPCLK1Freq();
            }
            else
            {
                u32Clk = CLK_GetPCLK0Freq();
            }
        }
        else
        {
            u32Clk = __HIRC;
        }

        u32Clk /= (u32Div + 1UL) * 1000UL;
    }

    return u32Clk;
}

/*@}*/ /* end of group SC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SC_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/

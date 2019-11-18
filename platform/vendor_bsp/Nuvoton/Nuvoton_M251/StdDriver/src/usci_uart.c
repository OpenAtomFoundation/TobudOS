/**************************************************************************//**
 * @file     usci_uart.c
 * @version  V0.10
 * @brief    M251 series USCI UART (UUART) driver source file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup USCI_UART_Driver USCI_UART Driver
  @{
*/

/** @addtogroup USCI_UART_EXPORTED_FUNCTIONS USCI_UART Exported Functions
  @{
*/

/**
 *    @brief        Clear USCI_UART specified interrupt flag
 *
 *    @param[in]    psUUART The pointer of the specified USCI_UART module.
 *    @param[in]    u32Mask The combination of all related interrupt sources.
 *                          Each bit corresponds to a interrupt source.
 *                          This parameter decides which interrupt flags will be cleared. It could be the combination of:
 *                          - \ref UUART_ABR_INT_MASK
 *                          - \ref UUART_RLS_INT_MASK
 *                          - \ref UUART_BUF_RXOV_INT_MASK
 *                          - \ref UUART_TXST_INT_MASK
 *                          - \ref UUART_TXEND_INT_MASK
 *                          - \ref UUART_RXST_INT_MASK
 *                          - \ref UUART_RXEND_INT_MASK
 *
 *    @return       None
 *
 *    @details      The function is used to clear USCI_UART related interrupt flags specified by u32Mask parameter.
 */

void UUART_ClearIntFlag(UUART_T *psUUART, uint32_t u32Mask)
{

    if (u32Mask & UUART_ABR_INT_MASK)   /* Clear Auto-baud Rate Interrupt */
    {
        psUUART->PROTSTS = UUART_PROTSTS_ABRDETIF_Msk;
    }

    if (u32Mask & UUART_RLS_INT_MASK)   /* Clear Receive Line Status Interrupt */
    {
        psUUART->PROTSTS = (UUART_PROTSTS_BREAK_Msk | UUART_PROTSTS_FRMERR_Msk | UUART_PROTSTS_PARITYERR_Msk);
    }

    if (u32Mask & UUART_BUF_RXOV_INT_MASK)   /* Clear Receive Buffer Over-run Error Interrupt */
    {
        psUUART->BUFSTS = UUART_BUFSTS_RXOVIF_Msk;
    }

    if (u32Mask & UUART_TXST_INT_MASK)   /* Clear Transmit Start Interrupt */
    {
        psUUART->PROTSTS = UUART_PROTSTS_TXSTIF_Msk;
    }

    if (u32Mask & UUART_TXEND_INT_MASK)   /* Clear Transmit End Interrupt */
    {
        psUUART->PROTSTS = UUART_PROTSTS_TXENDIF_Msk;
    }

    if (u32Mask & UUART_RXST_INT_MASK)   /* Clear Receive Start Interrupt */
    {
        psUUART->PROTSTS = UUART_PROTSTS_RXSTIF_Msk;
    }

    if (u32Mask & UUART_RXEND_INT_MASK)   /* Clear Receive End Interrupt */
    {
        psUUART->PROTSTS = UUART_PROTSTS_RXENDIF_Msk;
    }

}

/**
 *    @brief        Get USCI_UART specified interrupt flag
 *
 *    @param[in]    psUUART The pointer of the specified USCI_UART module.
 *    @param[in]    u32Mask The combination of all related interrupt sources.
 *                          Each bit corresponds to a interrupt source.
 *                          This parameter decides which interrupt flags will be read. It is combination of:
 *                          - \ref UUART_ABR_INT_MASK
 *                          - \ref UUART_RLS_INT_MASK
 *                          - \ref UUART_BUF_RXOV_INT_MASK
 *                          - \ref UUART_TXST_INT_MASK
 *                          - \ref UUART_TXEND_INT_MASK
 *                          - \ref UUART_RXST_INT_MASK
 *                          - \ref UUART_RXEND_INT_MASK
 *
 *    @return       Interrupt flags of selected sources.
 *
 *    @details      The function is used to get USCI_UART related interrupt flags specified by u32Mask parameter.
 */

uint32_t UUART_GetIntFlag(UUART_T *psUUART, uint32_t u32Mask)
{
    uint32_t u32IntFlag = 0ul;
    uint32_t u32Tmp1, u32Tmp2;

    /* Check Auto-baud Rate Interrupt Flag */
    u32Tmp1 = (u32Mask & UUART_ABR_INT_MASK);
    u32Tmp2 = (psUUART->PROTSTS & UUART_PROTSTS_ABRDETIF_Msk);

    if (u32Tmp1 && u32Tmp2)
    {
        u32IntFlag |= UUART_ABR_INT_MASK;
    }

    /* Check Receive Line Status Interrupt Flag */
    u32Tmp1 = (u32Mask & UUART_RLS_INT_MASK);
    u32Tmp2 = (psUUART->PROTSTS & (UUART_PROTSTS_BREAK_Msk | UUART_PROTSTS_FRMERR_Msk | UUART_PROTSTS_PARITYERR_Msk));

    if (u32Tmp1 && u32Tmp2)
    {
        u32IntFlag |= UUART_RLS_INT_MASK;
    }

    /* Check Receive Buffer Over-run Error Interrupt Flag */
    u32Tmp1 = (u32Mask & UUART_BUF_RXOV_INT_MASK);
    u32Tmp2 = (psUUART->BUFSTS & UUART_BUFSTS_RXOVIF_Msk);

    if (u32Tmp1 && u32Tmp2)
    {
        u32IntFlag |= UUART_BUF_RXOV_INT_MASK;
    }

    /* Check Transmit Start Interrupt Flag */
    u32Tmp1 = (u32Mask & UUART_TXST_INT_MASK);
    u32Tmp2 = (psUUART->PROTSTS & UUART_PROTSTS_TXSTIF_Msk);

    if (u32Tmp1 && u32Tmp2)
    {
        u32IntFlag |= UUART_TXST_INT_MASK;
    }

    /* Check Transmit End Interrupt Flag */
    u32Tmp1 = (u32Mask & UUART_TXEND_INT_MASK);
    u32Tmp2 = (psUUART->PROTSTS & UUART_PROTSTS_TXENDIF_Msk);

    if (u32Tmp1 && u32Tmp2)
    {
        u32IntFlag |= UUART_TXEND_INT_MASK;
    }

    /* Check Receive Start Interrupt Flag */
    u32Tmp1 = (u32Mask & UUART_RXST_INT_MASK);
    u32Tmp2 = (psUUART->PROTSTS & UUART_PROTSTS_RXSTIF_Msk);

    if (u32Tmp1 && u32Tmp2)
    {
        u32IntFlag |= UUART_RXST_INT_MASK;
    }

    /* Check Receive End Interrupt Flag */
    u32Tmp1 = (u32Mask & UUART_RXEND_INT_MASK);
    u32Tmp2 = (psUUART->PROTSTS & UUART_PROTSTS_RXENDIF_Msk);

    if (u32Tmp1 && u32Tmp2)
    {
        u32IntFlag |= UUART_RXEND_INT_MASK;
    }

    return u32IntFlag;

}


/**
 *  @brief      Disable USCI_UART function mode
 *
 *  @param[in]  psUUART The pointer of the specified USCI_UART module.
 *
 *  @return     None
 *
 *  @details    The function is used to disable USCI_UART function mode.
 */
void UUART_Close(UUART_T *psUUART)
{
    psUUART->CTL = 0ul;
}


/**
 *    @brief        Disable interrupt function.
 *
 *    @param[in]    psUUART The pointer of the specified USCI_UART module.
 *    @param[in]    u32Mask The combination of all related interrupt enable bits.
 *                          Each bit corresponds to a interrupt enable bit.
 *                          This parameter decides which interrupts will be disabled. It is combination of:
 *                          - \ref UUART_ABR_INT_MASK
 *                          - \ref UUART_RLS_INT_MASK
 *                          - \ref UUART_BUF_RXOV_INT_MASK
 *                          - \ref UUART_TXST_INT_MASK
 *                          - \ref UUART_TXEND_INT_MASK
 *                          - \ref UUART_RXST_INT_MASK
 *                          - \ref UUART_RXEND_INT_MASK
 *
 *    @return       None
 *
 *    @details      The function is used to disabled USCI_UART related interrupts specified by u32Mask parameter.
 */
void UUART_DisableInt(UUART_T *psUUART, uint32_t u32Mask)
{

    /* Disable Auto-baud rate interrupt flag */
    if ((u32Mask & UUART_ABR_INT_MASK) == UUART_ABR_INT_MASK)
    {
        psUUART->PROTIEN &= ~UUART_PROTIEN_ABRIEN_Msk;
    }

    /* Disable receive line status interrupt flag */
    if ((u32Mask & UUART_RLS_INT_MASK) == UUART_RLS_INT_MASK)
    {
        psUUART->PROTIEN &= ~UUART_PROTIEN_RLSIEN_Msk;
    }

    /* Disable RX overrun interrupt flag */
    if ((u32Mask & UUART_BUF_RXOV_INT_MASK) == UUART_BUF_RXOV_INT_MASK)
    {
        psUUART->BUFCTL &= ~UUART_BUFCTL_RXOVIEN_Msk;
    }

    /* Disable TX start interrupt flag */
    if ((u32Mask & UUART_TXST_INT_MASK) == UUART_TXST_INT_MASK)
    {
        psUUART->INTEN &= ~UUART_INTEN_TXSTIEN_Msk;
    }

    /* Disable TX end interrupt flag */
    if ((u32Mask & UUART_TXEND_INT_MASK) == UUART_TXEND_INT_MASK)
    {
        psUUART->INTEN &= ~UUART_INTEN_TXENDIEN_Msk;
    }

    /* Disable RX start interrupt flag */
    if ((u32Mask & UUART_RXST_INT_MASK) == UUART_RXST_INT_MASK)
    {
        psUUART->INTEN &= ~UUART_INTEN_RXSTIEN_Msk;
    }

    /* Disable RX end interrupt flag */
    if ((u32Mask & UUART_RXEND_INT_MASK) == UUART_RXEND_INT_MASK)
    {
        psUUART->INTEN &= ~UUART_INTEN_RXENDIEN_Msk;
    }
}


/**
 *    @brief        Enable interrupt function.
 *
 *    @param[in]    psUUART     The pointer of the specified USCI_UART module.
 *    @param[in]    u32Mask     The combination of all related interrupt enable bits.
 *                              Each bit corresponds to a interrupt enable bit.
 *                              This parameter decides which interrupts will be enabled. It is combination of:
 *                              - \ref UUART_ABR_INT_MASK
 *                              - \ref UUART_RLS_INT_MASK
 *                              - \ref UUART_BUF_RXOV_INT_MASK
 *                              - \ref UUART_TXST_INT_MASK
 *                              - \ref UUART_TXEND_INT_MASK
 *                              - \ref UUART_RXST_INT_MASK
 *                              - \ref UUART_RXEND_INT_MASK
 *
 *    @return       None
 *
 *    @details      The function is used to enable USCI_UART related interrupts specified by u32Mask parameter.
 */
void UUART_EnableInt(UUART_T  *psUUART, uint32_t u32Mask)
{
    /* Enable Auto-baud rate interrupt flag */
    if ((u32Mask & UUART_ABR_INT_MASK) == UUART_ABR_INT_MASK)
    {
        psUUART->PROTIEN |= UUART_PROTIEN_ABRIEN_Msk;
    }

    /* Enable receive line status interrupt flag */
    if ((u32Mask & UUART_RLS_INT_MASK) == UUART_RLS_INT_MASK)
    {
        psUUART->PROTIEN |= UUART_PROTIEN_RLSIEN_Msk;
    }

    /* Enable RX overrun interrupt flag */
    if ((u32Mask & UUART_BUF_RXOV_INT_MASK) == UUART_BUF_RXOV_INT_MASK)
    {
        psUUART->BUFCTL |= UUART_BUFCTL_RXOVIEN_Msk;
    }

    /* Enable TX start interrupt flag */
    if ((u32Mask & UUART_TXST_INT_MASK) == UUART_TXST_INT_MASK)
    {
        psUUART->INTEN |= UUART_INTEN_TXSTIEN_Msk;
    }

    /* Enable TX end interrupt flag */
    if ((u32Mask & UUART_TXEND_INT_MASK) == UUART_TXEND_INT_MASK)
    {
        psUUART->INTEN |= UUART_INTEN_TXENDIEN_Msk;
    }

    /* Enable RX start interrupt flag */
    if ((u32Mask & UUART_RXST_INT_MASK) == UUART_RXST_INT_MASK)
    {
        psUUART->INTEN |= UUART_INTEN_RXSTIEN_Msk;
    }

    /* Enable RX end interrupt flag */
    if ((u32Mask & UUART_RXEND_INT_MASK) == UUART_RXEND_INT_MASK)
    {
        psUUART->INTEN |= UUART_INTEN_RXENDIEN_Msk;
    }
}


/**
 *    @brief        Open and set USCI_UART function
 *
 *    @param[in]    psUUART         The pointer of the specified USCI_UART module.
 *    @param[in]    u32Baudrate     The baud rate of USCI_UART module.
 *
 *    @return       Real baud rate of USCI_UART module.
 *
 *    @details      This function use to enable USCI_UART function and set baud-rate.
 */
uint32_t UUART_Open(UUART_T *psUUART, uint32_t u32Baudrate)
{
    uint32_t u32PCLKFreq, u32PDSCnt, u32DSCnt, u32ClkDiv;
    uint32_t u32Tmp, u32Tmp2, u32Tmp3, u32Min, u32MinClkDiv, u32MinDSCnt;

    uint32_t u32Div;

    /* Get PCLK frequency */
    if ((psUUART == UUART0) || (psUUART == UUART2))
    {
        u32PCLKFreq = CLK_GetPCLK0Freq();
    }
    else
    {
        u32PCLKFreq = CLK_GetPCLK1Freq();
    }

    /* Find out divide value */
    u32Div = u32PCLKFreq / u32Baudrate;

    /* Avoid decimal of divider being discarded and find divider that are closest to the baud rate */
    u32Tmp = (u32PCLKFreq / u32Div) - u32Baudrate;
    u32Tmp2 = u32Baudrate - (u32PCLKFreq / (u32Div + 1));

    if (u32Tmp >= u32Tmp2)
    {
        u32Div = u32Div + 1;
    }

    /* Divide value = CLKDIV(MAX. = 0x400) * DSCNT(MAX. = 0x10) * PDSCNT */
    u32Tmp = 0x400 * 0x10;

    /* Find out PDSCNT value */
    for (u32PDSCnt = 1; u32PDSCnt <= 0x04; u32PDSCnt++)
    {
        if (u32Div <= (u32Tmp * u32PDSCnt)) break;
    }

    /* PDSCNT MAX.= 0x4 */
    if (u32PDSCnt > 0x4) u32PDSCnt = 0x4;

    /* Avoid decimal of PDSCNT being discarded */
    u32Tmp = u32Div / u32PDSCnt;
    u32Tmp2 = (u32Div / u32Tmp) - u32PDSCnt;
    u32Tmp3 = u32PDSCnt - (u32Div / (u32Tmp + 1));

    if (u32Tmp2 >= u32Tmp3)
    {
        u32Div = u32Tmp + 1;
    }
    else u32Div = u32Tmp;

    /* Find best solution of DSCNT and CLKDIV*/
    u32Min = (uint32_t) - 1;
    u32MinDSCnt = 0;
    u32MinClkDiv = 0;

    u32Tmp = 0;

    for (u32DSCnt = 6; u32DSCnt <= 0x10; u32DSCnt++)   /* DSCNT could be 0x5~0xF */
    {

        u32ClkDiv = u32Div / u32DSCnt;

        if (u32ClkDiv >= 0x400)
        {
            u32ClkDiv = 0x400;
            u32Tmp = u32Tmp2 = abs((int)(u32PCLKFreq / (u32ClkDiv * u32DSCnt * u32PDSCnt)) - (int)u32Baudrate);
        }
        else    /* Avoid decimal of PDSCNT being discarded */
        {
            u32Tmp = abs((int)(u32PCLKFreq / (u32ClkDiv * u32DSCnt * u32PDSCnt)) - (int)u32Baudrate);
            u32Tmp2 = abs((int)(u32PCLKFreq / ((u32ClkDiv + 1) * u32DSCnt * u32PDSCnt)) - (int)u32Baudrate);
        }

        if (u32Tmp > u32Tmp2)
        {
            u32ClkDiv = u32ClkDiv + 1;
        }
        else u32Tmp2 = u32Tmp;

        /* Record the closest solution */
        if (u32Tmp2 < u32Min)
        {
            u32Min = u32Tmp2;
            u32MinDSCnt = u32DSCnt;
            u32MinClkDiv = u32ClkDiv;

            /* Break when get good results */
            if (u32Min == 0)
            {
                break;
            }
        }
    }

    /* Enable USCI_UART protocol */
    psUUART->CTL &= ~UUART_CTL_FUNMODE_Msk;
    psUUART->CTL = 2 << UUART_CTL_FUNMODE_Pos;

    /* Set USCI_UART line configuration */
    psUUART->LINECTL = UUART_WORD_LEN_8 | UUART_LINECTL_LSB_Msk;
    psUUART->DATIN0 = (2 << UUART_DATIN0_EDGEDET_Pos);  /* Set falling edge detection */

    /* Set USCI_UART baud rate */
    psUUART->BRGEN = ((u32MinClkDiv - 1) << UUART_BRGEN_CLKDIV_Pos) |
                     ((u32MinDSCnt - 1) << UUART_BRGEN_DSCNT_Pos) |
                     ((u32PDSCnt - 1) << UUART_BRGEN_PDSCNT_Pos);

    psUUART->PROTCTL |= UUART_PROTCTL_PROTEN_Msk;

    return (u32PCLKFreq / u32PDSCnt / u32MinDSCnt / u32MinClkDiv);
}


/**
 *    @brief        Read USCI_UART data
 *
 *    @param[in]    psUUART         The pointer of the specified USCI_UART module.
 *    @param[in]    pu8RxBuf        The buffer to receive the data of receive buffer.
 *    @param[in]    u32ReadBytes    The read bytes number of data.
 *
 *    @return       Receive byte count
 *
 *    @details      The function is used to read Rx data from RX buffer and the data will be stored in pu8RxBuf.
 */
uint32_t UUART_Read(UUART_T *psUUART, uint8_t pu8RxBuf[], uint32_t u32ReadBytes)
{
    uint32_t  u32Count, u32Delayno;

    for (u32Count = 0ul; u32Count < u32ReadBytes; u32Count++)
    {
        u32Delayno = 0ul;

        while (psUUART->BUFSTS & UUART_BUFSTS_RXEMPTY_Msk)   /* Check RX empty => failed */
        {
            u32Delayno++;

            if (u32Delayno >= 0x40000000ul)
            {
                break;
            }
        }

        if (u32Delayno >= 0x40000000ul)
        {
            break;
        }

        pu8RxBuf[u32Count] = (uint8_t)psUUART->RXDAT;    /* Get Data from USCI RX  */
    }

    return u32Count;

}


/**
 *    @brief        Set USCI_UART line configuration
 *
 *    @param[in]    psUUART         The pointer of the specified USCI_UART module.
 *    @param[in]    u32Baudrate     The register value of baud rate of USCI_UART module.
 *                                  If u32Baudrate = 0, USCI_UART baud rate will not change.
 *    @param[in]    u32DataWidth    The data length of USCI_UART module.
 *                                  - \ref UUART_WORD_LEN_6
 *                                  - \ref UUART_WORD_LEN_7
 *                                  - \ref UUART_WORD_LEN_8
 *                                  - \ref UUART_WORD_LEN_9
 *    @param[in]    u32Parity       The parity setting (none/odd/even) of USCI_UART module.
 *                                  - \ref UUART_PARITY_NONE
 *                                  - \ref UUART_PARITY_ODD
 *                                  - \ref UUART_PARITY_EVEN
 *    @param[in]    u32StopBits     The stop bit length (1/2 bit) of USCI_UART module.
 *                                  - \ref UUART_STOP_BIT_1
 *                                  - \ref UUART_STOP_BIT_2
 *
 *    @return       Real baud rate of USCI_UART module.
 *
 *    @details      This function use to config USCI_UART line setting.
 */
uint32_t UUART_SetLine_Config(UUART_T *psUUART, uint32_t u32Baudrate, uint32_t u32DataWidth, uint32_t u32Parity, uint32_t u32StopBits)
{
    uint32_t u32PCLKFreq, u32PDSCnt, u32DSCnt, u32ClkDiv;
    uint32_t u32Tmp, u32Tmp2, u32Tmp3, u32Min, u32MinClkDiv, u32MinDSCnt;

    uint32_t u32Div;

    /* Get PCLK frequency */
    if ((psUUART == UUART0) || (psUUART == UUART2))
    {
        u32PCLKFreq = CLK_GetPCLK0Freq();
    }
    else
    {
        u32PCLKFreq = CLK_GetPCLK1Freq();
    }

    if (u32Baudrate != 0)
    {
        u32Div = u32PCLKFreq / u32Baudrate;
        u32Tmp = (u32PCLKFreq / u32Div) - u32Baudrate;
        u32Tmp2 = u32Baudrate - (u32PCLKFreq / (u32Div + 1));

        if (u32Tmp >= u32Tmp2)
        {
            u32Div = u32Div + 1;
        }

        u32Tmp = 0x400 * 0x10;

        for (u32PDSCnt = 1; u32PDSCnt <= 0x04; u32PDSCnt++)
        {
            if (u32Div <= (u32Tmp * u32PDSCnt)) break;
        }

        if (u32PDSCnt > 0x4) u32PDSCnt = 0x4;

        u32Tmp = u32Div / u32PDSCnt;
        u32Tmp2 = (u32Div / u32Tmp) - u32PDSCnt;
        u32Tmp3 = u32PDSCnt - (u32Div / (u32Tmp + 1));

        if (u32Tmp2 >= u32Tmp3)
        {
            u32Div = u32Tmp + 1;
        }
        else u32Div = u32Tmp;

        /* Find best solution */
        u32Min = (uint32_t) - 1;
        u32MinDSCnt = 0;
        u32MinClkDiv = 0;

        u32Tmp = 0;

        for (u32DSCnt = 6; u32DSCnt <= 0x10; u32DSCnt++)   /* DSCNT could be 0x5~0xF */
        {

            u32ClkDiv = u32Div / u32DSCnt;

            if (u32ClkDiv >= 0x400)
            {
                u32ClkDiv = 0x400;
                u32Tmp = u32Tmp2 = abs((int)(u32PCLKFreq / (u32ClkDiv * u32DSCnt * u32PDSCnt)) - (int)u32Baudrate);
            }
            else
            {
                u32Tmp = abs((int)(u32PCLKFreq / (u32ClkDiv * u32DSCnt * u32PDSCnt)) - (int)u32Baudrate);
                u32Tmp2 = abs((int)(u32PCLKFreq / ((u32ClkDiv + 1) * u32DSCnt * u32PDSCnt)) - (int)u32Baudrate);
            }

            if (u32Tmp > u32Tmp2)
            {
                u32ClkDiv = u32ClkDiv + 1;
            }
            else u32Tmp2 = u32Tmp;

            if (u32Tmp2 < u32Min)
            {
                u32Min = u32Tmp2;
                u32MinDSCnt = u32DSCnt;
                u32MinClkDiv = u32ClkDiv;

                /* Break when get good results */
                if (u32Min == 0)
                {
                    break;
                }
            }
        }

        /* Set USCI_UART baud rate */
        psUUART->BRGEN = ((u32MinClkDiv - 1) << UUART_BRGEN_CLKDIV_Pos) |
                         ((u32MinDSCnt - 1) << UUART_BRGEN_DSCNT_Pos) |
                         ((u32PDSCnt - 1) << UUART_BRGEN_PDSCNT_Pos);
    }
    else
    {
        u32PDSCnt = ((psUUART->BRGEN & UUART_BRGEN_PDSCNT_Msk) >> UUART_BRGEN_PDSCNT_Pos) + 1;
        u32MinDSCnt = ((psUUART->BRGEN & UUART_BRGEN_DSCNT_Msk) >> UUART_BRGEN_DSCNT_Pos) + 1;
        u32MinClkDiv = ((psUUART->BRGEN & UUART_BRGEN_CLKDIV_Msk) >> UUART_BRGEN_CLKDIV_Pos) + 1;
    }

    /* Set USCI_UART line configuration */
    psUUART->LINECTL = (psUUART->LINECTL & ~UUART_LINECTL_DWIDTH_Msk) | u32DataWidth;
    psUUART->PROTCTL = (psUUART->PROTCTL & ~(UUART_PROTCTL_STICKEN_Msk | UUART_PROTCTL_EVENPARITY_Msk |
                        UUART_PROTCTL_PARITYEN_Msk)) | u32Parity;
    psUUART->PROTCTL = (psUUART->PROTCTL & ~UUART_PROTCTL_STOPB_Msk) | u32StopBits;

    return (u32PCLKFreq / u32PDSCnt / u32MinDSCnt / u32MinClkDiv);
}


/**
 *    @brief        Write USCI_UART data
 *
 *    @param[in]    psUUART         The pointer of the specified USCI_UART module.
 *    @param[in]    pu8TxBuf        The buffer to send the data to USCI transmission buffer.
 *    @param[out]   u32WriteBytes   The byte number of data.
 *
 *    @return       Transfer byte count
 *
 *    @details      The function is to write data into TX buffer to transmit data by USCI_UART.
 */
uint32_t UUART_Write(UUART_T *psUUART, uint8_t pu8TxBuf[], uint32_t u32WriteBytes)
{
    uint32_t  u32Count, u32Delayno;

    for (u32Count = 0ul; u32Count != u32WriteBytes; u32Count++)
    {
        u32Delayno = 0ul;

        while ((psUUART->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0ul)   /* Wait Tx empty */
        {
            u32Delayno++;

            if (u32Delayno >= 0x40000000ul)
            {
                break;
            }
        }

        if (u32Delayno >= 0x40000000ul)
        {
            break;
        }

        psUUART->TXDAT = (uint8_t)pu8TxBuf[u32Count];    /* Send USCI_UART Data to buffer */
    }

    return u32Count;

}


/**
 *    @brief        Enable USCI_UART Wake-up Function
 *
 *    @param[in]    psUUART         The pointer of the specified USCI_UART module.
 *    @param[in]    u32WakeupMode   The wakeup mode of USCI_UART module.
 *                                   - \ref UUART_PROTCTL_DATWKEN_Msk    : Data wake-up Mode
 *                                   - \ref UUART_PROTCTL_CTSWKEN_Msk    : nCTS wake-up Mode
 *
 *    @return       None
 *
 *    @details      The function is used to enable Wake-up function of USCI_UART.
 */
void UUART_EnableWakeup(UUART_T *psUUART, uint32_t u32WakeupMode)
{
    psUUART->PROTCTL |= u32WakeupMode;
    psUUART->WKCTL |= UUART_WKCTL_WKEN_Msk;
}


/**
 *    @brief        Disable USCI_UART Wake-up Function
 *
 *    @param[in]    psUUART   The pointer of the specified USCI_UART module.
 *
 *    @return       None
 *
 *    @details      The function is used to disable Wake-up function of USCI_UART.
 */
void UUART_DisableWakeup(UUART_T *psUUART)
{
    psUUART->PROTCTL &= ~(UUART_PROTCTL_DATWKEN_Msk | UUART_PROTCTL_CTSWKEN_Msk);
    psUUART->WKCTL &= ~UUART_WKCTL_WKEN_Msk;
}

/**
 *    @brief        Enable USCI_UART auto flow control
 *
 *    @param[in]    psUUART   The pointer of the specified USCI_UART module.
 *
 *    @return       None
 *
 *    @details      The function is used to enable USCI_UART auto flow control.
 */
void UUART_EnableFlowCtrl(UUART_T *psUUART)
{
    /* Set RTS signal is low level active */
    psUUART->LINECTL &= ~UUART_LINECTL_CTLOINV_Msk;

    /* Set CTS signal is low level active */
    psUUART->CTLIN0 &= ~UUART_CTLIN0_ININV_Msk;

    /* Enable CTS and RTS auto flow control function */
    psUUART->PROTCTL |= UUART_PROTCTL_RTSAUTOEN_Msk | UUART_PROTCTL_CTSAUTOEN_Msk;
}

/**
 *    @brief        Disable USCI_UART auto flow control
 *
 *    @param[in]    psUUART    The pointer of the specified USCI_UART module.
 *
 *    @return       None
 *
 *    @details      The function is used to disable USCI_UART auto flow control.
 */
void UUART_DisableFlowCtrl(UUART_T *psUUART)
{
    /* Disable CTS and RTS auto flow control function */
    psUUART->PROTCTL &= ~(UUART_PROTCTL_RTSAUTOEN_Msk | UUART_PROTCTL_CTSAUTOEN_Msk);
}




/*@}*/ /* end of group USCI_UART_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group USCI_UART_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/


/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_lpuart.h"

void LPUART_SetBaudrate2(LPUART_Type * LPUARTx, uint32_t freq, uint32_t baudrate)
{
    LPUARTx->LPUBAUD |= LPUART_LPUBAUD_BREN_MASK;
    uint32_t base_cycles = freq / baudrate;
    uint32_t current_cycles = 0;
    uint32_t ideal_K;
    uint32_t K;
    K = 0xFFFFFFFF / freq / 4 * 2;
    LPUARTx->LPUBAUD |= LPUART_LPUBAUD_BR(base_cycles);
    LPUARTx->MODU = 0;
    for (uint8_t bit_idx = 0; bit_idx < 12; bit_idx++)
    {
        ideal_K = (uint32_t)( (base_cycles * K + 0.5) * (bit_idx + 1) );
        current_cycles += base_cycles;
        if (current_cycles * K >= ideal_K)
        {
            continue;
        }
        else if ( (ideal_K - current_cycles * K) >= (K / 2) )
        {
            LPUARTx->MODU |= 1 << bit_idx;
            current_cycles += 1;
        }
    }
}

/* LPUART initialization use LSE(32.768KHz) clock source. */
void LPUART_Init(LPUART_Type * LPUARTx, LPUART_Init_Type * init)
{
    uint32_t lpuart_mctl[6] = {0x952, 0xefb, 0x6db, 0x482,0x6d6, 0x842};
    if (init->ClockSource == LPUART_ClockSource_LSE)
    {
        LPUARTx->LPUBAUD &= ~LPUART_LPUBAUD_BREN_MASK;  /* BREN = 0, only 32.768KHz clock is supported as clock source. */
        LPUARTx->LPUBAUD = LPUART_LPUBAUD_BAUD(init->BaudRate);
        LPUARTx->MODU = lpuart_mctl[init->BaudRate];
    }

    /* WordLength. */
    LPUARTx->LPUCON |= LPUART_LPUCON_DL(init->WordLength);

    /* StopBits. */
    LPUARTx->LPUCON |= LPUART_LPUCON_SL(init->StopBits);

    /* Parity. */
    if (init->Parity == LPUART_Parity_Even)
    {
        LPUARTx->LPUCON |= LPUART_LPUCON_PAREN_MASK;
        LPUARTx->LPUCON &= ~LPUART_LPUCON_PTYP_MASK;
    }
    if (init->Parity == LPUART_Parity_Odd)
    {
        LPUARTx->LPUCON |= (LPUART_LPUCON_PAREN_MASK | LPUART_LPUCON_PTYP_MASK);
    }
}

/* LPUART enable tx. */
void LPUART_EnableTx(LPUART_Type * LPUARTx, bool enable)
{
    if (enable)
    {
        LPUARTx->LPUEN |= LPUART_LPUEN_TXEN_MASK;
    }
    else
    {
        LPUARTx->LPUEN &= ~LPUART_LPUEN_TXEN_MASK;
    }
}

/* LPUART enable rx. */
void LPUART_EnableRx(LPUART_Type * LPUARTx, bool enable)
{
    if (enable)
    {
        LPUARTx->LPUEN |= LPUART_LPUEN_RXEN_MASK;
    }
    else
    {
        LPUARTx->LPUEN &= ~LPUART_LPUEN_RXEN_MASK;
    }
}

/* LPUART get status. */
uint32_t LPUART_GetStatus(LPUART_Type * LPUARTx)
{
    return LPUARTx->LPUSTA;
}

/* LPUART put data. */
void LPUART_PutData(LPUART_Type * LPUARTx, uint8_t value)
{
    LPUARTx->LPUTXD = value;
}

/* LPAURT put data. */
uint8_t LPUART_GetData(LPUART_Type * LPUARTx)
{
    return (uint8_t)(LPUARTx->LPURXD & 0xff);
}

/* LPUART enable interrupt. */
void LPUART_EnableInterrupts(LPUART_Type * LPUARTx, uint32_t interrupts, bool enable)
{
    switch (interrupts)
    {
        case LPUART_INT_RX_FULL:
            if (enable)
            {
                LPUARTx->LPUCON |= LPUART_LPUCON_RXIE_MASK;
            }
            else
            {
                LPUARTx->LPUCON &= ~LPUART_LPUCON_TCIE_MASK;
            }
            break;
        case LPUART_INT_TX_EMPTY:
            if (enable)
            {
                LPUARTx->LPUCON |= LPUART_LPUCON_TXIE_MASK;
            }
            else
            {
                LPUARTx->LPUCON &= ~LPUART_LPUCON_TXIE_MASK;
            }
            break;
        case LPUART_INT_TX_DONE:
            if (enable)
            {
                LPUARTx->LPUCON |= LPUART_LPUCON_TCIE_MASK;
            }
            else
            {
                LPUARTx->LPUCON &= ~LPUART_LPUCON_TCIE_MASK;
            }
            break;
        default:
            break;
    }
}

/* Get LPUART interrupt status. */
uint32_t LPUART_GetInterruptStatus(LPUART_Type * LPUARTx)
{
    return LPUARTx->LPUIF;
}

/* Get LPUART interrupt enable status. */
uint32_t LPUART_GetEnabledInterrupts(LPUART_Type * LPUARTx)
{
    uint32_t flags = 0u;
    if ( 0u != (LPUART_LPUCON_RXIE_MASK & LPUARTx->LPUCON) )
    {
        flags |= LPUART_INT_RX_FULL;
    }
    if ( 0u != (LPUART_LPUCON_TXIE_MASK & LPUARTx->LPUCON) )
    {
        flags |= LPUART_INT_TX_EMPTY;
    }
    if ( 0u != (LPUART_LPUCON_TCIE_MASK & LPUARTx->LPUCON) )
    {
        flags |= LPUART_INT_TX_DONE;
    }
    return flags;
}

/* Clear LPUART interrupt status. */
void LPUART_ClearInterruptStatus(LPUART_Type * LPUARTx, uint32_t interrupts)
{
    LPUARTx->LPUIF |= interrupts;
}

/* LPUART enable DMA. */
void LPUART_EnableDMA(LPUART_Type * LPUARTx, uint32_t dma, bool enable)
{
    if (enable)
    {
        LPUARTx->LPUEN |= dma;
    }
    else
    {
        LPUARTx->LPUEN &= ~dma;
    }
}

/* LPUART get tx data register address. */
uint32_t   LPUART_GetTxDataRegAddr(LPUART_Type * LPUARTx)
{
    return (uint32_t)(&(LPUARTx->LPUTXD));
}

/* LPUART get rx data register address. */
uint32_t   LPUART_GetRxDataRegAddr(LPUART_Type * LPUARTx)
{
    return (uint32_t)(&(LPUARTx->LPURXD));
}

/* EOF. */

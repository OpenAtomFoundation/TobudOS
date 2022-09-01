/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_i2s.h"

/* init i2s in master mode. */
void I2S_InitMaster(I2S_Type * I2Sx, I2S_Master_Init_Type * init)
{
    if (NULL == init ||
        0u == init->SampleRate)
    {
        return;
    }

    /* get I2Sx->GCTL value & clear the field to be configured. */
    uint32_t gctl = I2Sx->GCTL & ~(SPI_I2S_GCTL_RXEN_MASK
                                 | SPI_I2S_GCTL_TXEN_MASK
                                 | SPI_I2S_GCTL_MODE_MASK
                                 | SPI_I2S_GCTL_INTEN_MASK);

    /* get I2Sx->CCTL value & clear the field to be configured. */
    uint32_t cctl = I2Sx->CCTL & ~(SPI_I2S_CCTL_RXEDGE_MASK
                                 | SPI_I2S_CCTL_CPOL_MASK);

    /* get I2Sx->I2SCFGR value & clear the field to be configured. */
    uint32_t cfgr = I2Sx->I2SCFGR & ~(SPI_I2S_I2SCFGR_I2SDIV_MASK
                                    | SPI_I2S_I2SCFGR_HFDSEL_MASK
                                    | SPI_I2S_I2SCFGR_MCKOE_MASK
                                    | SPI_I2S_I2SCFGR_SPII2S_MASK
                                    | SPI_I2S_I2SCFGR_I2SSTD_MASK
                                    | SPI_I2S_I2SCFGR_DATLEN_MASK
                                    | SPI_I2S_I2SCFGR_CHLEN_MASK);

    /* set the xfer mode. */
    switch (init->XferMode)
    {
        case I2S_XferMode_TxOnly:
            gctl |=  SPI_I2S_GCTL_TXEN_MASK;
            gctl &= ~SPI_I2S_GCTL_RXEN_MASK;
            cfgr |=  SPI_I2S_I2SCFGR_HFDSEL_MASK;
            break;
        case I2S_XferMode_RxOnly:
            gctl |=  SPI_I2S_GCTL_RXEN_MASK;
            gctl &= ~SPI_I2S_GCTL_TXEN_MASK;
            cfgr |= SPI_I2S_I2SCFGR_HFDSEL_MASK;
            break;
        case I2S_XferMode_TxRx:
            gctl |= SPI_I2S_GCTL_TXEN_MASK | SPI_I2S_GCTL_RXEN_MASK;
            cfgr &= ~SPI_I2S_I2SCFGR_HFDSEL_MASK;
            break;
        default:
            break;
    }

    /* master mode. */
    gctl |= SPI_I2S_GCTL_MODE_MASK /* master mode, disable i2s, and reset the other bits in CCTL. */
         |  SPI_I2S_GCTL_INTEN_MASK; /* always enable the global interrupt. the specific events are controlled by each bits in INTEN register. */

    /* set CPOL. */
    cctl |= SPI_I2S_CCTL_RXEDGE_MASK
         |  SPI_I2S_CCTL_CPOL(init->Polarity);

    uint32_t div = 0u;

    /* calc the i2s_div value. */
    if (!(init->EnableMCLK) )
    {
        if (I2S_DataWidth_16b == init->DataWidth)
        {
            div = (init->ClockFreqHz) / (32u * (init->SampleRate) );
        }
        else
        {
            div = (init->ClockFreqHz) / (64u * (init->SampleRate) );
            cfgr |= SPI_I2S_I2SCFGR_CHLEN_MASK;
        }
    }
    else
    {
        div = (init->ClockFreqHz) / (256u * (init->SampleRate) );
        if (I2S_DataWidth_16b != init->DataWidth)
        {
            cfgr |= SPI_I2S_I2SCFGR_CHLEN_MASK;
        }
    }

    cfgr |= SPI_I2S_I2SCFGR_I2SDIV(div)
         |  SPI_I2S_I2SCFGR_MCKOE(1u)
         |  SPI_I2S_I2SCFGR_SPII2S_MASK
         |  SPI_I2S_I2SCFGR_I2SSTD(init->Protocol)
         |  SPI_I2S_I2SCFGR_DATLEN(init->DataWidth);

    I2Sx->GCTL = gctl; /* set I2Sx->GCTL. */
    I2Sx->CCTL = cctl; /* set I2Sx->CCTL. */
    I2Sx->I2SCFGR = cfgr; /* set I2Sx->I2SCFGR. */
}

/* enable i2s. */
void I2S_Enable(I2S_Type * I2Sx, bool enable)
{
    if (enable)
    {
        I2Sx->GCTL |= SPI_I2S_GCTL_SPIEN_MASK;
    }
    else
    {
        I2Sx->GCTL &= ~SPI_I2S_GCTL_SPIEN_MASK;
    }
}

/* get i2s status. */
uint32_t I2S_GetStatus(I2S_Type * I2Sx)
{
    return I2Sx->CSTAT;
}

/* enable interrupt. */
void I2S_EnableInterrupts(I2S_Type * I2Sx, uint32_t interrupts, bool enable)
{
    if (enable)
    {
        I2Sx->INTEN |= interrupts;
    }
    else
    {
        I2Sx->INTEN &= ~interrupts;
    }
}

/* get interrupt status. */
uint32_t I2S_GetInterruptStatus(I2S_Type * I2Sx)
{
    return I2Sx->INTSTAT;
}

/* clear interrupt status. */
void I2S_ClearInterruptStatus(I2S_Type * I2Sx, uint32_t interrupts)
{
    I2Sx->INTCLR = interrupts;
}

/* put data to tx buffer. */
void I2S_PutData(I2S_Type * I2Sx, uint32_t data)
{
    I2Sx->TXREG = data;
}

/* get data in rx buffer. */
uint32_t I2S_GetData(I2S_Type * I2Sx)
{
    return I2Sx->RXREG;
}

I2S_Channel_Type I2S_GetXferChannel(I2S_Type * I2Sx)
{
    if (0u != (I2Sx->CSTAT & SPI_I2S_CSTAT_CHSIDE_MASK) )
    {
        return I2S_Channel_Right;
    }
    else
    {
        return I2S_Channel_Left;
    }
}

/* enable dma. */
void I2S_EnableDMA(I2S_Type * I2Sx, bool enable)
{
    if (enable)
    {
        I2Sx->GCTL |= SPI_I2S_GCTL_DMAMODE_MASK;
    }
    else
    {
        I2Sx->GCTL &= ~SPI_I2S_GCTL_DMAMODE_MASK;
    }
}

/* get rx buffer reg addr. */
uint32_t I2S_GetRxDataRegAddr(I2S_Type * I2Sx)
{
    return (uint32_t)(&(I2Sx->RXREG) );
}

/* get tx buffer reg addr. */
uint32_t I2S_GetTxDataRegAddr(I2S_Type * I2Sx)
{
    return (uint32_t)(&(I2Sx->TXREG) );
}

/* EOF. */

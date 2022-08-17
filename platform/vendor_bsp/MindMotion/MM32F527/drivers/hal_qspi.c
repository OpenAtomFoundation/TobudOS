/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_qspi.h"

void QSPI_Init(QSPI_Type * QSPIx, QSPI_Init_Type * init)
{
    if (NULL == init /* if the init pointer is null, quit init. */
     || init->SckDiv < 2u || init->SckDiv > 9u || init->SckDiv % 2u != 0u /* The available value must be even & the range would be 2 ~ 128. */
     || init->CsHighLevelCycles < 2u || init->CsHighLevelCycles > 9u) /* available range would be 2 ~ 9. */
    {
        return;
    }

    QSPIx->CR = (QSPIx->CR & ~(QSPI_CR_SCKDIV_MASK | QSPI_CR_CSRHT_MASK | QSPI_CR_SCKMODE_MASK | QSPI_CR_OPMODE_MASK) )
              | QSPI_CR_SCKDIV(init->SckDiv / 2u - 1u) /* set the baudrate. */
              | QSPI_CR_CSRHT(init->CsHighLevelCycles - 2u) /* set the CS high level cycle. */
              | QSPI_CR_SCKMODE(init->SpiMode) /* set the SPI mode. */
              | QSPI_CR_OPMODE(QSPI_OpMode_Idle); /* set the operation mode, idle by default. */
}

void QSPI_EnableDirectRead(QSPI_Type * QSPIx, QSPI_DirectXferConf_Type * conf)
{
    if (NULL == conf) /* disabled. */
    {
        QSPIx->CR = (QSPIx->CR & ~QSPI_CR_OPMODE_MASK) | QSPI_CR_OPMODE(QSPI_OpMode_Idle);
        return;
    }

    /* calculate the optimal value of rxdly, the value is SCK / 4. */
    uint32_t rxdly = ( ( (QSPIx->CR & QSPI_CR_SCKDIV_MASK) >> QSPI_CR_SCKDIV_SHIFT) + 1u ) >> 1u;
    if (rxdly > 3)
    {
        rxdly = 3; /* available range would be 0 ~ 3. */
    }

    QSPIx->DMCR = (QSPIx->DMCR & ~(QSPI_DMCR_RXDLY_MASK | QSPI_DMCR_NUMDC_MASK | QSPI_DMCR_DMODE_MASK
                                | QSPI_DMCR_ABMODE_MASK | QSPI_DMCR_ABSIZE_MASK | QSPI_DMCR_ADSIZE_MASK
                                | QSPI_DMCR_ADMODE_MASK | QSPI_DMCR_IMODE_MASK  | QSPI_DMCR_INST_MASK) )
                | QSPI_DMCR_RXDLY(rxdly)
                | QSPI_DMCR_NUMDC(conf->DummyCycles)
                | QSPI_DMCR_DMODE(conf->DataBusWidth)
                | QSPI_DMCR_ABSIZE(conf->AltWordWidth)
                | QSPI_DMCR_ABMODE(conf->AltBusWidth)
                | QSPI_DMCR_ADSIZE(conf->AddrWordWidth)
                | QSPI_DMCR_ADMODE(conf->AddrBusWidth)
                | QSPI_DMCR_IMODE(conf->CmdBusWidth)
                | QSPI_DMCR_INST(conf->CmdValue);

    QSPIx->DABR = QSPI_DABR_DALT(conf->AltValue);

    QSPIx->CR = ( QSPIx->CR & ~( QSPI_CR_OPMODE_MASK) ) | QSPI_CR_OPMODE(QSPI_OpMode_Direct);
}

uint8_t QSPI_GetDirectData8b(QSPI_Type * QSPIx, uint32_t offset)
{
    (void)QSPIx;
    return (*( (uint8_t *)(QSPI_M_BASE + offset) ) );
}

uint16_t QSPI_GetDirectData16b(QSPI_Type * QSPIx, uint32_t offset)
{
    (void)QSPIx;
    return (*( (uint16_t *)(QSPI_M_BASE + offset) ) );
}

uint32_t QSPI_GetDirectData32b(QSPI_Type * QSPIx, uint32_t offset)
{
    (void)QSPIx;
    return (*( (uint32_t *)(QSPI_M_BASE + offset) ) );
}

void QSPI_SetIndirectReadConf(QSPI_Type * QSPIx, QSPI_IndirectXferConf_Type * conf)
{
    if(NULL == conf)
    {
        return;
    }

    /* calculate the optimal value of rxdly, the value is SCK / 4. */
    uint32_t rxdly = ( ( (QSPIx->CR & QSPI_CR_SCKDIV_MASK) >> QSPI_CR_SCKDIV_SHIFT) + 1u ) >> 1u;
    if (rxdly > 3)
    {
        rxdly = 3; /* available range would be 0 ~ 3. */
    }

    QSPIx->IMCR = ( QSPIx->IMCR & ~(QSPI_IMCR_IDMODE_MASK | QSPI_IMCR_NUMDC_MASK  | QSPI_IMCR_DSIZE_MASK
                                  | QSPI_IMCR_DMODE_MASK  | QSPI_IMCR_ABSIZE_MASK | QSPI_IMCR_ABMODE_MASK
                                  | QSPI_IMCR_ADSIZE_MASK | QSPI_IMCR_ADMODE_MASK | QSPI_IMCR_IMODE_MASK
                                  | QSPI_IMCR_INST_MASK   | QSPI_IMCR_RXDLY_MASK) )
                | QSPI_IMCR_RXDLY(rxdly)
                | QSPI_IMCR_IDMODE(0u) /* read. */
                | QSPI_IMCR_NUMDC(conf->DummyCycles)
                | QSPI_IMCR_DSIZE(conf->DataWordWidth)
                | QSPI_IMCR_DMODE(conf->DataBusWidth)
                | QSPI_IMCR_ABSIZE(conf->AltWordWidth)
                | QSPI_IMCR_ABMODE(conf->AltBusWidth)
                | QSPI_IMCR_ADSIZE(conf->AddrWordWidth)
                | QSPI_IMCR_ADMODE(conf->AddrBusWidth)
                | QSPI_IMCR_IMODE(conf->CmdBusWidth)
                | QSPI_IMCR_INST(conf->CmdValue);
    QSPIx->IABR = QSPI_IABR_IALT(conf->AltValue);
    QSPIx->IADR = QSPI_IADR_IADDR(conf->AddrValue);
    QSPIx->IWCR = QSPI_IWCR_IWCNT(0u); /* wait 7 AHB_CLK to quit inrect mode. */
    QSPIx->IDLR = QSPI_IDLR_IDLEN(conf->DataLen - 1u);
    QSPIx->CR = (QSPIx->CR & ~QSPI_CR_OPMODE_MASK) | QSPI_CR_OPMODE(QSPI_OpMode_Indirect);
}

uint32_t QSPI_GetIndirectData(QSPI_Type * QSPIx)
{
    return QSPIx->IDFR;
}

void QSPI_SetIndirectWriteConf(QSPI_Type * QSPIx, QSPI_IndirectXferConf_Type * conf)
{
    if(NULL == conf)
    {
        return;
    }

    /* calculate the optimal value of rxdly, the value is SCK / 4. */
    uint32_t rxdly = ( ( (QSPIx->CR & QSPI_CR_SCKDIV_MASK) >> QSPI_CR_SCKDIV_SHIFT) + 1u ) >> 1u;
    if (rxdly > 3)
    {
        rxdly = 3; /* available range would be 0 ~ 3. */
    }

    QSPIx->IMCR = ( QSPIx->IMCR & ~(QSPI_IMCR_IDMODE_MASK | QSPI_IMCR_NUMDC_MASK  | QSPI_IMCR_DSIZE_MASK
                                  | QSPI_IMCR_DMODE_MASK  | QSPI_IMCR_ABSIZE_MASK | QSPI_IMCR_ABMODE_MASK
                                  | QSPI_IMCR_ADSIZE_MASK | QSPI_IMCR_ADMODE_MASK | QSPI_IMCR_IMODE_MASK
                                  | QSPI_IMCR_INST_MASK   | QSPI_IMCR_RXDLY_MASK) )
                | QSPI_IMCR_RXDLY(rxdly)
                | QSPI_IMCR_IDMODE(1u) /* write. */
                | QSPI_IMCR_NUMDC(conf->DummyCycles)
                | QSPI_IMCR_DSIZE(conf->DataWordWidth)
                | QSPI_IMCR_DMODE(conf->DataBusWidth)
                | QSPI_IMCR_ABSIZE(conf->AltWordWidth)
                | QSPI_IMCR_ABMODE(conf->AltBusWidth)
                | QSPI_IMCR_ADSIZE(conf->AddrWordWidth)
                | QSPI_IMCR_ADMODE(conf->AddrBusWidth)
                | QSPI_IMCR_IMODE(conf->CmdBusWidth)
                | QSPI_IMCR_INST(conf->CmdValue);
    QSPIx->IABR = QSPI_IABR_IALT(conf->AltValue);
    QSPIx->IADR = QSPI_IADR_IADDR(conf->AddrValue);
    QSPIx->IWCR = QSPI_IWCR_IWCNT(0u); /* wait 7 AHB_CLK to quit inrect mode. */
    QSPIx->IDLR = QSPI_IDLR_IDLEN(conf->DataLen - 1u);
    QSPIx->CR = (QSPIx->CR & ~QSPI_CR_OPMODE_MASK) | QSPI_CR_OPMODE(QSPI_OpMode_Indirect);
}

void QSPI_PutIndirectData(QSPI_Type * QSPIx, uint32_t value)
{
    QSPIx->IDFR = value;
}

void QSPI_EnableXIP(QSPI_Type * QSPIx, bool enable)
{
    if (true == enable)
    {
        QSPIx->CR = QSPIx->CR | QSPI_CR_XIPMODE_MASK;
    }
    else
    {
        QSPIx->CR = QSPIx->CR & ~QSPI_CR_XIPMODE_MASK;
    }
}

void QSPI_SetIndirectAutoXIP(QSPI_Type * QSPIx, bool enable)
{
    if (true == enable)
    {
        QSPIx->IMCR |=  QSPI_IMCR_XIPIM_MASK;
    }
    else
    {
        QSPIx->IMCR &= ~QSPI_IMCR_XIPIM_MASK;
    }
}

uint32_t QSPI_GetStatus(QSPI_Type * QSPIx)
{
    return QSPIx->SR;
}

void QSPI_ClearStatus(QSPI_Type * QSPIx, uint32_t status)
{
    QSPIx->SR &= status; /* write 1 to clear status. */
}

uint32_t QSPI_GetOpMode(QSPI_Type * QSPIx)
{
    return (QSPIx->SR & QSPI_SR_OPCRCF_MASK) >> QSPI_SR_OPCRCF_SHIFT;
}

void QSPI_EnableInterrupts(QSPI_Type * QSPIx, uint32_t interrupts, bool enable)
{
    if(true == enable)
    {
        QSPIx->IDER |=  interrupts;
    }
    else
    {
        QSPIx->IDER &= ~interrupts;
    }
}

void QSPI_EnableDMARequest(QSPI_Type * QSPIx, uint32_t request, bool enable)
{
    if(true == enable)
    {
        QSPIx->IDER |=  request;
    }
    else
    {
        QSPIx->IDER &= ~request;
    }
}

/* EOF. */

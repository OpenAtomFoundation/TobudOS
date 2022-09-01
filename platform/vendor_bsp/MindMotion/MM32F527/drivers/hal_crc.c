/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_crc.h"

/* CRC initialization. */
void CRC_Init(CRC_Type * CRCx, CRC_Init_Type * init)
{
    CRCx->CR &= ~(CRC_PR_PR_MASK | CRC_CR_RO_MASK | CRC_CR_RI_MASK | CRC_CR_BEO_MASK | CRC_CR_BEI_MASK);
    CRCx->PR &= ~CRC_PR_PR_MASK;
    CRCx->CR |= CRC_CR_PWIDTH(init->PolynomialWidth) | CRC_CR_RO(init->OutRev)
             | CRC_CR_RI(init->InRev) | CRC_CR_BEO(init->OutEndian) | CRC_CR_BEI(init->InEndian);
    CRCx->PR = CRC_PR_PR(init->Polynomial);
}

/* Set the value in CRC calculation unit as the initial value. */
void CRC_SetSeed(CRC_Type * CRCx, uint32_t val)
{
    CRCx->IVR = CRC_IVR_IVR(val);
    CRCx->CR |= CRC_CR_RST_MASK;
}

/* Set data for CRC calculation. */
void CRC_SetData(CRC_Type * CRCx, uint32_t val)
{
    CRCx->DR = val;
}

/* Get CRC calculation result. */
uint32_t CRC_GetResult(CRC_Type * CRCx)
{
    return CRCx->DR;
}

/* EOF. */

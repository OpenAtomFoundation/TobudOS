/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_mds.h"

/* Configure the trigger mode, not using clu out as the trigger in. */
void MDS_EnableTrigger(MDS_Type * MDSx, uint32_t out, uint32_t in, MDS_TriggerInMode_Type trigger_mode)
{
    MDSx->TRIGXCR[out] =( MDSx->TRIGXCR[out] & ~(MDS_TRIGXCR_CLUEN_MASK | MDS_TRIGXCR_TRGSEL_MASK | MDS_TRIGXCR_EDGESEL_MASK) )
                                     | MDS_TRIGXCR_TRGSEL(trigger_mode)
                                     | MDS_TRIGXCR_TRGSEL(in)
                                     ;
}

/* enable the software trigger as the trigger source. */
void MDS_DoSwTrigger(MDS_Type * MDSx)
{
    MDSx->SWTRIG = MDS_SWTRIG_SWTRIG_MASK;
}

/* Configure the connection mode, not using clu out as the connection in. */
void MDS_EnableConn(MDS_Type * MDSx, uint32_t out, uint32_t in)
{
    MDSx->CONNXCR[out] =( MDSx->CONNXCR[out] & ~(MDS_CONNXCR_CLUEN_MASK | MDS_CONNXCR_TRGSEL_MASK ) )
                                         | MDS_CONNXCR_TRGSEL(in)
                                         ;
}

/* Configure the trigger mode, using clu out as the trigger in. */
void MDS_EnableTriggerClu(MDS_Type * MDSx, uint32_t out, uint32_t in, MDS_CluConf_Type * conf)
{
    if (!conf)
    {
        return;
    }

    MDSx->TRIGXCR[out] =( MDSx->TRIGXCR[out] & ~(MDS_TRIGXCR_CLUEN_MASK | MDS_TRIGXCR_TRGSEL_MASK | MDS_TRIGXCR_EDGESEL_MASK) )
                          | MDS_TRIGXCR_CLUSEL(in)
                          ;
    /* configure the trigger in for one clu in. */
    for (uint32_t i = 0; i<MDS_CLU_IN_LEN; i++)
    {
        MDSx->TRIGCLUXSEL[in] |= conf->CluInSource[i] << (i * 8u);
    }
    /* configure the each trigger detection edge mode. */
    MDSx->TRIGCLUXCFG[in] = conf->CluInSourceTriggerEdgeMask;
    /* configure the each trigger in inverter and the clu operation logic. */
    MDSx->CONNCLUXCFG[in] = conf->CluInSourceInvertMask | conf->CluLogic;
    /* enable the clu mode. */
    MDSx->TRIGXCR[out] |= MDS_TRIGXCR_CLUEN_MASK;
}

/* Configure the clu out as the trigger source mode . */
void MDS_EnableConnClu(MDS_Type * MDSx, uint32_t out, uint32_t in, MDS_CluConf_Type * conf)
{
    if (!conf)
    {
        return;
    }

    MDSx->CONNXCR[out] = ( MDSx->CONNXCR[out] & ~ (MDS_CONNXCR_CLUEN_MASK | MDS_CONNXCR_CLUSEL_MASK))
                               | MDS_CONNXCR_CLUSEL(in)
                               ;

    /* configure the connection in for one clu in. */
    for (uint32_t i = 0; i<MDS_CLU_IN_LEN; i++)
    {
        MDSx->CONNCLUXSEL[in] |= conf->CluInSource[i] << (i * 8u);
    }
    /* configure the each trigger in inverter and the clu operation logic. */
    MDSx->CONNCLUXCFG[in] = conf->CluInSourceInvertMask | conf->CluLogic;
    /* enable the clu mode. */
    MDSx->CONNXCR[out] |= MDS_CONNXCR_CLUEN_MASK;

}


/* EOF. */

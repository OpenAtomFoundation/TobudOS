/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "hal_flexcan.h"

void FLEXCAN_Enable(FLEXCAN_Type * FLEXCANx, bool enable)
{
    if (enable)
    {
        FLEXCANx->MCR &= ~ FLEXCAN_MCR_MDIS_MASK; /* Unmask the module disable. */
    }
    else
    {
        FLEXCANx->MCR |= FLEXCAN_MCR_MDIS_MASK;
    }
}

void FLEXCAN_DoSoftReset(FLEXCAN_Type * FLEXCANx)
{
    /* Wait for exiting from low power mode. */
    while ( 0u != (FLEXCANx->MCR & FLEXCAN_MCR_LPMACK_MASK) )
    {
    }
    FLEXCANx->MCR |= FLEXCAN_MCR_SOFTRST_MASK;
    while ( 0u != (FLEXCANx->MCR & FLEXCAN_MCR_SOFTRST_MASK) )
    {
    }

    /* Enable overall warning interrupt. Detailed tx warning and rx warning will be setup by detailed func.*/
    FLEXCANx->MCR |= (FLEXCAN_MCR_WRNEN_MASK | FLEXCAN_MCR_LPRIOEN_MASK);
    /* Set sampling mode, using three sampling points. */
    FLEXCANx->CTRL1 |= FLEXCAN_CTRL1_SMP_MASK;
    /* No response automatically generated after receiving remote frame. */
    FLEXCANx->CTRL2 |= ( FLEXCAN_CTRL2_RRS_MASK
                       | FLEXCAN_CTRL2_EACEN_MASK
                       );

    /* Get the defaulted max message buffer number. */
    uint32_t mb_num = ( (FLEXCANx->MCR & FLEXCAN_MCR_MAXMB_MASK) >> FLEXCAN_MCR_MAXMB_SHIFT );

    /* Clear the individual mask and CS for all message buffers. */
    for (uint32_t i = 0u; i <= mb_num; i++)
    {
        FLEXCANx->RXIMRN[i] = 0x3FFFFFFFu; /* Defaultedly, this mask value is for standard data frame. */
        FLEXCAN_ResetMb(FLEXCANx, i);
    }

    /* Clear the global mask. */
    FLEXCANx->RXMGMASK = 0x3FFFFFFFu;
    FLEXCANx->RX14MASK = 0x3FFFFFFFu;
    FLEXCANx->RX15MASK = 0x3FFFFFFFu;
    FLEXCANx->RXFGMASK = 0x3FFFFFFFu;
}

bool FLEXCAN_Init(FLEXCAN_Type * FLEXCANx, FLEXCAN_Init_Type * init)
{
    /* Just enable the module shortly for soft reset. */
    FLEXCAN_Enable(FLEXCANx, true);

    /* softreset the module to enter freezemode and do some default setup. */
    FLEXCAN_DoSoftReset(FLEXCANx);

    /* Clear register setting bit and save other settings. */
    uint32_t mcr = FLEXCANx->MCR & ~ ( FLEXCAN_MCR_SLFWAK_MASK
                                     | FLEXCAN_MCR_WAKSRC_MASK
                                     | FLEXCAN_MCR_MAXMB_MASK
                                     | FLEXCAN_MCR_SRXDIS_MASK
                                     | FLEXCAN_MCR_IRMQ_MASK
                                     );
    uint32_t ctrl1 = FLEXCANx->CTRL1 & ~ ( FLEXCAN_CTRL1_LOM_MASK
                                         | FLEXCAN_CTRL1_LPB_MASK
                                         | FLEXCAN_CTRL1_CLKSRC_MASK
                                         | FLEXCAN_CTRL1_BOFFREC_MASK
                                         | FLEXCAN_CTRL1_TSYN_MASK
                                         );

    uint32_t ctrl2 = FLEXCANx->CTRL2 & ~ ( FLEXCAN_CTRL2_RRS_MASK
                                         | FLEXCAN_CTRL2_EACEN_MASK
                                         );

    /* Select the work mode. */
    switch (init->WorkMode)
    {
        case FLEXCAN_WorkMode_LoopBack:
            ctrl1 |= FLEXCAN_CTRL1_LPB_MASK;
            break;
        case FLEXCAN_WorkMode_ListenOnly:
            ctrl1 |= FLEXCAN_CTRL1_LOM_MASK;
            break;
        case FLEXCAN_WorkMode_Normal:
            break;
    }

    /* Set the self wake up mechanism. */
    switch (init->SelfWakeUp)
    {
        case FLEXCAN_SelfWakeUp_BypassFilter:
            mcr |= FLEXCAN_MCR_SLFWAK_MASK;
            break;
        case FLEXCAN_SelfWakeUp_WithFilter:
            mcr |= ( FLEXCAN_MCR_SLFWAK_MASK
                   | FLEXCAN_MCR_WAKSRC_MASK
                   );
            break;
        case FLEXCAN_SelfWakeUp_Disabled:
            break;
        default:
            break;
    }

    /* Set the max number of message buffer to be used during xferring. */
    mcr |= FLEXCAN_MCR_MAXMB(init->MaxXferNum);

    /* If enable timer synchronization. */
    if ( init->EnableTimerSync )
    {
        ctrl1 |= FLEXCAN_CTRL1_TSYN_MASK;
    }

    /* Set the global mask or individual mask. */
    if (init->Mask == FLEXCAN_Mask_Individual)
    {
        mcr |= FLEXCAN_MCR_IRMQ_MASK;
    }

    /* Enable the self reception for self rx frame. */
    if (!init->EnableSelfReception)
    {
        mcr |= FLEXCAN_MCR_SRXDIS_MASK;
    }

    /* Set the clock source of protocol engine. */
    ctrl1 |= FLEXCAN_CTRL1_CLKSRC(init->ClockSource);
    /* Store the remote request but not automatically generate a response. */
    ctrl2 |= ( FLEXCAN_CTRL2_RRS_MASK
             | FLEXCAN_CTRL2_EACEN_MASK /* Enable comparison of IDE and RTR. */
             );

    /* Write back to the register. */
    FLEXCANx->MCR = mcr;
    FLEXCANx->CTRL1 = ctrl1;
    FLEXCANx->CTRL2 = ctrl2;

    /* Calculate divider and set timing. */
    /* Calculate the time quantum for resynchronization. */
    uint32_t quantum = ( 1u
                         + ( init->TimConf->PropSegLen + 1u )
                         + ( init->TimConf->PhaSegLen1 + 1u )
                         + ( init->TimConf->PhaSegLen2 + 1u )
                         );
    /* Check validity. */
    if ( (quantum * init->BaudRate > init->ClockFreqHz)  || (init->BaudRate == 0u) )
    {
        return false;
    }
    /* Calculate the prescaler value. */
    init->TimConf->PreDiv = init->ClockFreqHz / ( quantum * init->BaudRate ) - 1u;

    FLEXCAN_SetTimingConf(FLEXCANx, init->TimConf);

    return true;
}

void FLEXCAN_SetTimingConf(FLEXCAN_Type * FLEXCANx, FLEXCAN_TimConf_Type * conf)
{
    /* Enter freeze mode before setting timing parameters. */
    FLEXCAN_EnableFreezeMode(FLEXCANx, true);

    /* If use extended then set up the CBT reg. */
    if ( conf->EnableExtendedTime )
    {
        /* clear extended timing conf. */
        uint32_t cbt = FLEXCANx->CBT & ~ ( FLEXCAN_CBT_BTF_MASK
                                         | FLEXCAN_CBT_EPRESDIV_MASK
                                         | FLEXCAN_CBT_ERJW_MASK
                                         | FLEXCAN_CBT_EPROPSEG_MASK
                                         | FLEXCAN_CBT_EPSEG1_MASK
                                         | FLEXCAN_CBT_EPSEG2_MASK
                                         );
        cbt = FLEXCAN_CBT_BTF_MASK
            | FLEXCAN_CBT_EPRESDIV(conf->PreDiv)
            | FLEXCAN_CBT_EPSEG1(conf->PhaSegLen1)
            | FLEXCAN_CBT_EPSEG2(conf->PhaSegLen2)
            | FLEXCAN_CBT_EPROPSEG(conf->PropSegLen)
            | FLEXCAN_CBT_ERJW(conf->JumpWidth);
        FLEXCANx->CBT = cbt;
    }
    else
    {
        /* Clear registers for writing. */
        uint32_t ctrl1 = FLEXCANx->CTRL1 & ~ ( FLEXCAN_CTRL1_PRESDIV_MASK
                                             | FLEXCAN_CTRL1_PSEG1_MASK
                                             | FLEXCAN_CTRL1_PSEG2_MASK
                                             | FLEXCAN_CTRL1_PROPSEG_MASK
                                             | FLEXCAN_CTRL1_RJW_MASK
                                             );
        ctrl1 |= FLEXCAN_CTRL1_PRESDIV(conf->PreDiv)
               | FLEXCAN_CTRL1_PSEG1(conf->PhaSegLen1)
               | FLEXCAN_CTRL1_PSEG2(conf->PhaSegLen2)
               | FLEXCAN_CTRL1_PROPSEG(conf->PropSegLen)
               | FLEXCAN_CTRL1_RJW(conf->JumpWidth);
        FLEXCANx->CTRL1 = ctrl1;
    }

    /* Exit the freeze mode after setting timing parameters. */
    FLEXCAN_EnableFreezeMode(FLEXCANx, false);
}

void FLEXCAN_EnableFreezeMode(FLEXCAN_Type * FLEXCANx, bool enable)
{
    if (enable)
    {
        FLEXCANx->MCR |= ( FLEXCAN_MCR_FRZ_MASK | FLEXCAN_MCR_HALT_MASK );

        /* Wait a period of time for entering. */
        while (0u == (FLEXCANx->MCR & FLEXCAN_MCR_FRZACK_MASK))
        {
        }
    }
    else
    {
        FLEXCANx->MCR &= ~( FLEXCAN_MCR_FRZ_MASK | FLEXCAN_MCR_HALT_MASK );

        /* Wait a period of time for entirely exiting. */
        while (0u != (FLEXCANx->MCR & FLEXCAN_MCR_FRZACK_MASK))
        {
        }
    }
}

/* RXMGMASK register macro. */
#define FLEXCAN_RXMGMASK_RTR_SHIFT   (31u)
#define FLEXCAN_RXMGMASK_RTR_MASK    (0x1u << FLEXCAN_RXMGMASK_RTR_SHIFT)
#define FLEXCAN_RXMGMASK_RTR(x)      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXMGMASK_RTR_SHIFT)) & FLEXCAN_RXMGMASK_RTR_MASK)
#define FLEXCAN_RXMGMASK_IDE_SHIFT   (30u)
#define FLEXCAN_RXMGMASK_IDE_MASK    (0x1u << FLEXCAN_RXMGMASK_IDE_SHIFT)
#define FLEXCAN_RXMGMASK_IDE(x)      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXMGMASK_IDE_SHIFT)) & FLEXCAN_RXMGMASK_IDE_MASK)
#define FLEXCAN_RXMGMASK_STD_SHIFT   (18u)
#define FLEXCAN_RXMGMASK_STD_MASK    (0x7FFU << FLEXCAN_RXMGMASK_STD_SHIFT)
#define FLEXCAN_RXMGMASK_STD(x)      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXMGMASK_STD_SHIFT)) & FLEXCAN_RXMGMASK_STD_MASK)
#define FLEXCAN_RXMGMASK_EXT_SHIFT   (0u)
#define FLEXCAN_RXMGMASK_EXT_MASK    (0x1FFFFFFFu << FLEXCAN_RXMGMASK_EXT_SHIFT)
#define FLEXCAN_RXMGMASK_EXT(x)      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXMGMASK_EXT_SHIFT)) & FLEXCAN_RXMGMASK_EXT_MASK)

void FLEXCAN_SetGlobalMbMaskConf(FLEXCAN_Type * FLEXCANx, FLEXCAN_RxMbMaskConf_Type * conf)
{
    /* Calculate the mask value. */
    uint32_t mask = ( FLEXCAN_RXMGMASK_RTR(conf->MbType)
                    | FLEXCAN_RXMGMASK_IDE(conf->MbFormat)
                    | ( (conf->MbFormat == FLEXCAN_MbFormat_Extended) ? (FLEXCAN_RXMGMASK_EXT(conf->IdMask)) : (FLEXCAN_RXMGMASK_STD(conf->IdMask)) )
                    );

    FLEXCANx->RXMGMASK = mask;
    FLEXCANx->RX14MASK = mask;
    FLEXCANx->RX15MASK = mask;
}

/* RXFGMASK register macro. */
#define FLEXCAN_RXFGMASK_RXIDA_RTR_SHIFT                    (31u)
#define FLEXCAN_RXFGMASK_RXIDA_RTR_MASK                     (0x1u << FLEXCAN_RXFGMASK_RXIDA_RTR_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDA_RTR(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDA_RTR_SHIFT)) & FLEXCAN_RXFGMASK_RXIDA_RTR_MASK)
#define FLEXCAN_RXFGMASK_RXIDA_IDE_SHIFT                    (30u)
#define FLEXCAN_RXFGMASK_RXIDA_IDE_MASK                     (0x1u << FLEXCAN_RXFGMASK_RXIDA_IDE_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDA_IDE(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDA_IDE_SHIFT)) & FLEXCAN_RXFGMASK_RXIDA_IDE_MASK)
#define FLEXCAN_RXFGMASK_RXIDA_STD_SHIFT                    (19u)
#define FLEXCAN_RXFGMASK_RXIDA_STD_MASK                     (0x7FFu << FLEXCAN_RXFGMASK_RXIDA_STD_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDA_STD(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDA_STD_SHIFT)) & FLEXCAN_RXFGMASK_RXIDA_STD_MASK)
#define FLEXCAN_RXFGMASK_RXIDA_EXT_SHIFT                    (1u)
#define FLEXCAN_RXFGMASK_RXIDA_EXT_MASK                     (0x1FFFFFFFu << FLEXCAN_RXFGMASK_RXIDA_EXT_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDA_EXT(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDA_EXT_SHIFT)) & FLEXCAN_RXFGMASK_RXIDA_EXT_MASK)

#define FLEXCAN_RXFGMASK_RXIDB_RTR_HIGH_SHIFT               (31u)
#define FLEXCAN_RXFGMASK_RXIDB_RTR_HIGH_MASK                (0x1u << FLEXCAN_RXFGMASK_RXIDB_RTR_HIGH_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_RTR_HIGH(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_RTR_HIGH_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_RTR_HIGH_MASK)
#define FLEXCAN_RXFGMASK_RXIDB_IDE_HIGH_SHIFT               (30u)
#define FLEXCAN_RXFGMASK_RXIDB_IDE_HIGH_MASK                (0x1u << FLEXCAN_RXFGMASK_RXIDB_IDE_HIGH_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_IDE_HIGH(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_IDE_HIGH_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_IDE_HIGH_MASK)
#define FLEXCAN_RXFGMASK_RXIDB_STD_HIGH_SHIFT               (19u)
#define FLEXCAN_RXFGMASK_RXIDB_STD_HIGH_MASK                (0x7FFu << FLEXCAN_RXFGMASK_RXIDB_STD_HIGH_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_STD_HIGH(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_STD_HIGH_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_STD_HIGH_MASK)
#define FLEXCAN_RXFGMASK_RXIDB_EXT_HIGH_SHIFT               (16u)
#define FLEXCAN_RXFGMASK_RXIDB_EXT_HIGH_MASK                (0x3FFFu << FLEXCAN_RXFGMASK_RXIDB_EXT_HIGH_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_EXT_HIGH(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_EXT_HIGH_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_EXT_HIGH_MASK)
#define FLEXCAN_RXFGMASK_RXIDB_RTR_LOW_SHIFT                (15u)
#define FLEXCAN_RXFGMASK_RXIDB_RTR_LOW_MASK                 (0x1u << FLEXCAN_RXFGMASK_RXIDB_RTR_LOW_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_RTR_LOW(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_RTR_LOW_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_RTR_LOW_MASK)
#define FLEXCAN_RXFGMASK_RXIDB_IDE_LOW_SHIFT                (14u)
#define FLEXCAN_RXFGMASK_RXIDB_IDE_LOW_MASK                 (0x1u << FLEXCAN_RXFGMASK_RXIDB_IDE_LOW_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_IDE_LOW(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_IDE_LOW_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_IDE_LOW_MASK)
#define FLEXCAN_RXFGMASK_RXIDB_STD_LOW_SHIFT                (3u)
#define FLEXCAN_RXFGMASK_RXIDB_STD_LOW_MASK                 (0x7FFu << FLEXCAN_RXFGMASK_RXIDB_STD_LOW_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_STD_LOW(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_STD_LOW_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_STD_LOW_MASK)
#define FLEXCAN_RXFGMASK_RXIDB_EXT_LOW_SHIFT                (0u)
#define FLEXCAN_RXFGMASK_RXIDB_EXT_LOW_MASK                 (0x3FFFu << FLEXCAN_RXFGMASK_RXIDB_EXT_LOW_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDB_EXT_LOW(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDB_EXT_LOW_SHIFT)) & FLEXCAN_RXFGMASK_RXIDB_EXT_LOW_MASK)

#define FLEXCAN_RXFGMASK_RXIDC_HIGH_SHIFT                   (24u)
#define FLEXCAN_RXFGMASK_RXIDC_HIGH_MASK                    (0xFFu << FLEXCAN_RXFGMASK_RXIDC_HIGH_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDC_HIGH(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDC_HIGH_SHIFT)) & FLEXCAN_RXFGMASK_RXIDC_HIGH_MASK)
#define FLEXCAN_RXFGMASK_RXIDC_MIDHIGH_SHIFT                (16u)
#define FLEXCAN_RXFGMASK_RXIDC_MIDHIGH_MASK                 (0xFFu << FLEXCAN_RXFGMASK_RXIDC_MIDHIGH_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDC_MIDHIGH(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDC_MIDHIGH_SHIFT)) & FLEXCAN_RXFGMASK_RXIDC_MIDHIGH_MASK)
#define FLEXCAN_RXFGMASK_RXIDC_MIDLOW_SHIFT                 (8u)
#define FLEXCAN_RXFGMASK_RXIDC_MIDLOW_MASK                  (0xFFu << FLEXCAN_RXFGMASK_RXIDC_MIDLOW_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDC_MIDLOW(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDC_MIDLOW_SHIFT)) & FLEXCAN_RXFGMASK_RXIDC_MIDLOW_MASK)
#define FLEXCAN_RXFGMASK_RXIDC_LOW_SHIFT                    (0u)
#define FLEXCAN_RXFGMASK_RXIDC_LOW_MASK                     (0xFFu << FLEXCAN_RXFGMASK_RXIDC_LOW_SHIFT)
#define FLEXCAN_RXFGMASK_RXIDC_LOW(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_RXIDC_LOW_SHIFT)) & FLEXCAN_RXFGMASK_RXIDC_LOW_MASK)

void FLEXCAN_SetRxFifoGlobalMaskConf(FLEXCAN_Type *FLEXCANx, FLEXCAN_RxFifoMaskConf_Type * conf)
{
    uint32_t mask = 0u;
    /* Select filter format. */
    switch(conf->FilterFormat)
    {
        case FLEXCAN_FifoIdFilterFormat_A:
            mask = ( FLEXCAN_RXFGMASK_RXIDA_RTR(conf->MbType)
                   | FLEXCAN_RXFGMASK_RXIDA_IDE(conf->MbFormat)
                   | ( (conf->MbFormat == FLEXCAN_MbFormat_Extended) ?  FLEXCAN_RXFGMASK_RXIDA_EXT(conf->RxIdA) : FLEXCAN_RXFGMASK_RXIDA_STD(conf->RxIdA) )
                   );
            break;
        case FLEXCAN_FifoIdFilterFormat_B:
             mask = ( FLEXCAN_RXFGMASK_RXIDB_RTR_HIGH(conf->MbType)
                    | FLEXCAN_RXFGMASK_RXIDB_IDE_HIGH(conf->MbFormat)
                    | ( (conf->MbFormat == FLEXCAN_MbFormat_Extended) ?  FLEXCAN_RXFGMASK_RXIDB_EXT_HIGH(conf->RxIdB[0]) : FLEXCAN_RXFGMASK_RXIDB_STD_HIGH(conf->RxIdB[0]) )
                    | FLEXCAN_RXFGMASK_RXIDB_RTR_LOW(conf->MbType)
                    | FLEXCAN_RXFGMASK_RXIDB_IDE_LOW(conf->MbFormat)
                    | ( (conf->MbFormat == FLEXCAN_MbFormat_Extended) ?  FLEXCAN_RXFGMASK_RXIDB_EXT_LOW(conf->RxIdB[1]) : FLEXCAN_RXFGMASK_RXIDB_STD_LOW(conf->RxIdB[1]) )
                    );
            break;
        case FLEXCAN_FifoIdFilterFormat_C:
            mask = ( FLEXCAN_RXFGMASK_RXIDC_HIGH(conf->RxIdC[0])
                   | FLEXCAN_RXFGMASK_RXIDC_MIDHIGH(conf->RxIdC[1])
                   | FLEXCAN_RXFGMASK_RXIDC_MIDLOW(conf->RxIdC[2])
                   | FLEXCAN_RXFGMASK_RXIDC_LOW(conf->RxIdC[3])
                   );
            break;
        default:
            break;
    }
    FLEXCANx->RXFGMASK = mask;
}

void FLEXCAN_SetRxMbIndividualMaskConf(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_RxMbMaskConf_Type * conf)
{
    /* Calculate the mask value. */
    uint32_t mask = ( FLEXCAN_RXMGMASK_RTR(conf->MbType)
                    | FLEXCAN_RXMGMASK_IDE(conf->MbFormat)
                    | ( (conf->MbFormat == FLEXCAN_MbFormat_Extended) ? (FLEXCAN_RXMGMASK_EXT(conf->IdMask)) : (FLEXCAN_RXMGMASK_STD(conf->IdMask)) )
                    );
    FLEXCANx->RXIMRN[channel] = mask;
}

void FLEXCAN_ResetMb(FLEXCAN_Type * FLEXCANx, uint32_t channel)
{
    FLEXCANx->MB[channel].CS = 0u;
    FLEXCANx->MB[channel].ID = 0u;
    FLEXCANx->MB[channel].WORD0 = 0u;
    FLEXCANx->MB[channel].WORD1 = 0u;
}

void FLEXCAN_SetMbCode(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_MbCode_Type code)
{
    FLEXCANx->MB[channel].CS = (FLEXCANx->MB[channel].CS & ~FLEXCAN_CS_CODE_MASK) | FLEXCAN_CS_CODE(code);
}

void FLEXCAN_SetRxMb(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_RxMbConf_Type * conf)
{
    if (NULL != conf)
    {
        uint32_t cs = 0u;

        /* Select the mb format type for seting ID area with different format. */
        if (conf->MbFormat == FLEXCAN_MbFormat_Extended)
        {
            cs |= FLEXCAN_CS_IDE_MASK; /* Set extended frame flag bit. */
            FLEXCANx->MB[channel].ID = ( (conf->Id) << FLEXCAN_ID_EXT_SHIFT ) & ( FLEXCAN_ID_EXT_MASK | FLEXCAN_ID_STD_MASK);
        }
        else
        {
            FLEXCANx->MB[channel].ID = FLEXCAN_ID_STD(conf->Id);
        }

        /* Set RTR area for remote frame. */
        if (conf->MbType == FLEXCAN_MbType_Remote)
        {
            cs |= FLEXCAN_CS_RTR_MASK;
        }
        FLEXCANx->MB[channel].CS = cs;
        /* FLEXCAN_SetMbCode(FLEXCANx, channel, FLEXCAN_MbCode_RxEmpty); */
    }
    else
    {
        /* If not use Rx Mb anymore, just reset for neither Tx nor Rx. */
        FLEXCAN_ResetMb(FLEXCANx, channel);
    }
}

bool FLEXCAN_EnableRxFifo(FLEXCAN_Type * FLEXCANx, FLEXCAN_RxFifoConf_Type * conf)
{
    if ( NULL == conf )
    {
        /* Disable the fifo. */
        FLEXCANx->MCR &= ~ FLEXCAN_MCR_RFEN_MASK;
        /* Clear rffn value. */
        uint32_t rffn = ( (FLEXCANx->CTRL2 & FLEXCAN_CTRL2_RFFN_MASK) >> FLEXCAN_CTRL2_RFFN_SHIFT );
        uint32_t max_channel = 8u + rffn * 2u;

        /* Clear the mb used for fifo and fifo filters. */
        for (uint32_t i = 0u; i < max_channel; i++)
        {
            FLEXCAN_ResetMb(FLEXCANx, i);
        }
    }
    else
    {
        /* Clear registers for setting. */
        uint32_t mcr = FLEXCANx->MCR & ~ ( FLEXCAN_MCR_RFEN_MASK
                                         | FLEXCAN_MCR_IDAM_MASK
                                         );
        uint32_t ctrl2 = FLEXCANx->CTRL2 & ~ ( FLEXCAN_CTRL2_RFFN_MASK
                                             | FLEXCAN_CTRL2_MRP_MASK
                                             );

        /* Set FLEXCAN fifo filter format. */
        mcr |= FLEXCAN_MCR_IDAM(conf->FilterFormat);

        /* Calculate the mb to be used for fifo. */
        uint32_t max_mb = ( (FLEXCANx->MCR & FLEXCAN_MCR_MAXMB_MASK) >> FLEXCAN_MCR_MAXMB_SHIFT );
        max_mb = max_mb < 0xFu ? 0xFu : max_mb;
        /* Calculate the rffn setting value from the Id filter number to be used. */
        uint32_t rffn = 0u;
        for (uint32_t i = 0u; i < 0xFu; i++)
        {
            if ( ( ( i + 1u ) * 8u) >= (conf->IdFilterNum) )
            {
                if ( max_mb < (6u + ( i + 1u ) * 2u) )
                {
                    return false;
                }
                rffn = i;
                break;
            }
        }
        ctrl2 |= FLEXCAN_CTRL2_RFFN(rffn);

        /* Set the filter value, for left Mb space, just set it all 1. */
        uint32_t idx = 0u;
        for (uint32_t i = 6u; i < 6u + ( rffn + 1u ) * 2u; i++)
        {
            volatile uint32_t * mb = &(FLEXCANx->MB[i].CS);
            for (uint32_t k = 0u; k < 4u; k++)
            {
                /* Fill all valid filter in the mail box occupied by filter.
                 * Disable unused Rx FIFO Filter, the other rest of register in the last Mail box occupied by fiter set
                 * as 0xffffffff.
                 */
                mb[k] = (idx < conf->IdFilterNum) ? (conf->IdFilterTable[idx]) : 0xFFFFFFFFU;

                /* Try to fill next filter in current Mail Box.  */
                idx++;
            }
        }

        /* Set Rx priority between Mb and fifo. */
        if (FLEXCAN_FifoPriority_FifoFirst == conf->priority)
        {
            ctrl2 |= FLEXCAN_CTRL2_MRP_MASK;
        }

        /* Enable the fifo. */
        mcr |= FLEXCAN_MCR_RFEN_MASK;
        FLEXCANx->MCR = mcr;
        FLEXCANx->CTRL2 = ctrl2;

        return true;
    }
    return false;
}

void FLEXCAN_EnableInterrupts(FLEXCAN_Type * FLEXCANx, uint32_t interrupts, bool enable)
{
    uint32_t ctrl1 = FLEXCANx->CTRL1;

    /* Enable interrupts. */
    if (enable)
    {
        ctrl1 |= interrupts;
    }
    else
    {
        ctrl1 &= ~interrupts;
    }

    FLEXCANx->CTRL1 = ctrl1;
}

uint32_t FLEXCAN_GetStatus(FLEXCAN_Type * FLEXCANx)
{
    return FLEXCANx->ESR1;
}

void FLEXCAN_ClearStatus(FLEXCAN_Type * FLEXCANx, uint32_t flags)
{
    FLEXCANx->ESR1 = flags;
}

void FLEXCAN_EnableMbInterrupts(FLEXCAN_Type * FLEXCANx, uint32_t interrupts, bool enable)
{
    if (enable)
    {
        FLEXCANx->IMASK1 |= interrupts;
    }
    else
    {
        FLEXCANx->IMASK1 &= ~ interrupts;
    }
}

uint32_t FLEXCAN_GetMbStatus(FLEXCAN_Type * FLEXCANx)
{
    return FLEXCANx->IFLAG1;
}

void FLEXCAN_ClearMbStatus(FLEXCAN_Type * FLEXCANx, uint32_t interrupts)
{
    FLEXCANx->IFLAG1 = interrupts;
}

void FLEXCAN_EnableFifoDMA(FLEXCAN_Type * FLEXCANx, bool enable)
{
    if (enable)
    {
        FLEXCANx->MCR |= FLEXCAN_MCR_DMA_MASK;
    }
    else
    {
        FLEXCANx->MCR &= ~ FLEXCAN_MCR_DMA_MASK;
    }
}

uint32_t FLEXCAN_GetTxError(FLEXCAN_Type * FLEXCANx)
{
    return ( (FLEXCANx->ECR & FLEXCAN_ECR_TXERRCNT_MASK) >> FLEXCAN_ECR_TXERRCNT_SHIFT );
}

uint32_t FLEXCAN_GetRxError(FLEXCAN_Type * FLEXCANx)
{
    return ( (FLEXCANx->ECR & FLEXCAN_ECR_RXERRCNT_MASK) >> FLEXCAN_ECR_RXERRCNT_SHIFT );
}

uint32_t FLEXCAN_GetFifoAddr(FLEXCAN_Type * FLEXCANx)
{
    return (uint32_t)(&(FLEXCANx->MB[0].CS));
}

bool FLEXCAN_WriteTxMb(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_Mb_Type * mb)
{
    if ( FLEXCAN_MbCode_TxDataOrRemote != ( (FLEXCANx->MB[channel].CS & FLEXCAN_CS_CODE_MASK) >> FLEXCAN_CS_CODE_SHIFT ) )
    {
        /* Reset the CODE for transfer. */
        uint32_t cs = (FLEXCANx->MB[channel].CS & ~ FLEXCAN_CS_CODE_MASK) | FLEXCAN_CS_CODE(FLEXCAN_MbCode_TxInactive);

        /* Switch mb format. */
        if (mb->FORMAT == FLEXCAN_MbFormat_Extended)
        {
            /* Extended Mb frame with longer ID. */
            FLEXCANx->MB[channel].ID = ( ( ( mb->ID ) & ( FLEXCAN_ID_STD_MASK | FLEXCAN_ID_EXT_MASK ) )
                                     | FLEXCAN_ID_PRIO(mb->PRIORITY)
                                     );
            cs |= FLEXCAN_CS_SRR_MASK | FLEXCAN_CS_IDE_MASK;
        }
        else
        {
            FLEXCANx->MB[channel].ID = ( FLEXCAN_ID_STD(mb->ID)
                                     | FLEXCAN_ID_PRIO(mb->PRIORITY)
                                     );
            cs &= ~ (FLEXCAN_CS_SRR_MASK | FLEXCAN_CS_IDE_MASK);
        }

        /* Switch Mb type. */
        if (mb->TYPE == FLEXCAN_MbType_Remote)
        {
            cs |= FLEXCAN_CS_RTR_MASK;
        }
        else
        {
            cs &= ~ FLEXCAN_CS_RTR_MASK;
        }

        /* Set Mb length for Tx. */
        cs |= FLEXCAN_CS_DLC(mb->LENGTH);
        FLEXCANx->MB[channel].WORD0 = mb->WORD0;
        FLEXCANx->MB[channel].WORD1 = mb->WORD1;
        FLEXCANx->MB[channel].CS = cs; /* Always set the CS area at last. */
        /* FLEXCAN_SetMbCode(FLEXCANx, channel, FLEXCAN_MbCode_TxDataOrRemote); */
        return true;
    }

    return false;
}

bool FLEXCAN_ReadRxMb(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_Mb_Type * mb)
{
    uint32_t cs = FLEXCANx->MB[channel].CS;

    /* Switch the Mb format to get the ID. */
    if ( 0u != (cs & FLEXCAN_CS_IDE_MASK) )
    {
        mb->ID = FLEXCANx->MB[channel].ID;
        mb->FORMAT = FLEXCAN_MbFormat_Extended;
    }
    else
    {
        mb->ID = ( (FLEXCANx->MB[channel].ID & FLEXCAN_ID_STD_MASK) >> FLEXCAN_ID_STD_SHIFT);
        mb->FORMAT = FLEXCAN_MbFormat_Standard;
    }

    /* Swith the Mb type. */
    if ( 0u != (cs & FLEXCAN_CS_RTR_MASK) )
    {
        mb->TYPE = FLEXCAN_MbType_Remote;
    }
    else
    {
        mb->TYPE = FLEXCAN_MbType_Data;
    }

    /* Get the valid data length. */
    mb->LENGTH = ( (cs & FLEXCAN_CS_DLC_MASK) >> FLEXCAN_CS_DLC_SHIFT );
    /* Get the Rx Mb frame timestamp. */
    mb->TIMESTAMP = ( (cs & FLEXCAN_CS_TIMESTAMP_MASK) >> FLEXCAN_CS_TIMESTAMP_SHIFT);
    /* Read the data. */
    mb->WORD0 = FLEXCANx->MB[channel].WORD0;
    mb->WORD1 = FLEXCANx->MB[channel].WORD1;
    /* Unlock the Mb. */
    (void)FLEXCANx->TIMER;
    return true;
}

bool FLEXCAN_ReadRxFifo(FLEXCAN_Type * FLEXCANx, FLEXCAN_Mb_Type * mb)
{
    if ( 0u != ( FLEXCANx->MCR & FLEXCAN_MCR_RFEN_MASK ) ) /* If fifo is enabled, MB0 will be the only output of fifo. */
    {
        /* Mb 0 is the fifo output. */
        FLEXCAN_ReadRxMb(FLEXCANx, 0u, mb);
        /* Get the id filter hit number. */
        mb->IDHIT = (FLEXCANx->RXFIR & FLEXCAN_RXFIR_IDHIT_MASK); /* Get the fifo output idhit number. */
        return true;
    }
    else
    {
        (void)FLEXCANx->TIMER; /* Unlock the Mb. */
        return false;
    }
}

uint32_t FLEXCAN_GetMatchCrcValue(FLEXCAN_Type * FLEXCANx, uint32_t * channel)
{
    if (channel)
    {
        *channel = ( FLEXCANx->CRCR & FLEXCAN_CRCR_MBCRC_MASK) >> FLEXCAN_CRCR_MBCRC_SHIFT; /* Get the number of the Mb who has succeeded in arbitration for Tx. */
    }

    return FLEXCANx->CRCR & FLEXCAN_CRCR_TXCRC_MASK;/* Get the CRC value of the last Tx frame. */
}

/* EOF. */

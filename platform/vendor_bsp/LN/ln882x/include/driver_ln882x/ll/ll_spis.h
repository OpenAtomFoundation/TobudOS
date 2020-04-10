#ifndef __LL_SPIS_H__
#define __LL_SPIS_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

void LL_SPIS_CtrlR0(uint8_t dfs, uint8_t cfs, uint8_t srl, uint8_t slv_oe, uint8_t tmod, uint8_t scpol, uint8_t scph, uint8_t frf);

void LL_SPIS_Enable(uint8_t en);

void LL_SPIS_MW_HandShake_Enable(uint8_t en);

void LL_SPIS_MW_Mode_Set(uint8_t mode);

void LL_SPIS_MW_TxMode_Set(uint8_t transfer_mode);

void LL_SPIS_TxFIFO_Threshold_Set(uint8_t tx_thd);

void LL_SPIS_RxFIFO_Threshold_Set(uint8_t rx_thd);

uint8_t LL_SPIS_Current_TxFIFO_Level(void);

uint8_t LL_SPIS_Current_RxFIFO_Level(void);

uint8_t LL_SPIS_Normal_Status(void);

uint8_t LL_SPIS_Is_DataCollision(void);

uint8_t LL_SPIS_Is_TxError(void);

uint8_t LL_SPIS_Is_RxFIFO_Full(void);

uint8_t LL_SPIS_Is_RxFIFO_NotEmpty(void);

uint8_t LL_SPIS_Is_TxFIFO_Empty(void);

uint8_t LL_SPIS_Is_TxFIFO_NotFull(void);

uint8_t LL_SPIS_Is_Busy(void);

void LL_SPIS_IntMask_Set(uint8_t mask_map);

uint8_t LL_SPIS_IntMask_Get(void);

uint8_t LL_SPIS_Int_Status(void);

uint8_t LL_SPIS_RawInt_Status(void);

uint8_t LL_SPIS_IntClr_TxOverflow(void);

uint8_t LL_SPIS_IntClr_RxOverflow(void);

uint8_t LL_SPIS_IntClr_RxUnderflow(void);

uint8_t LL_SPIS_IntClr_MultiMaster(void);

uint8_t LL_SPIS_IntClr_All(void);

void LL_SPIS_DMA_Ctrl(uint8_t tx_dma_en, uint8_t rx_dma_en);

void LL_SPIS_DMA_TxDataLevel(uint8_t tx_lvl);

void LL_SPIS_DMA_RXDataLevel(uint8_t rx_lvl);

void LL_SPIS_DataReg_Set(uint32_t data);

uint32_t LL_SPIS_DataReg_Get(void);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __LL_SPIS_H__

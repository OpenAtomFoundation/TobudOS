#ifndef __LL_SPIM2_H__
#define __LL_SPIM2_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

void LL_SPIM2_MiscCfg(uint16_t val);

void LL_SPIM2_TxInt_Enable(uint8_t en);

void LL_SPIM2_RxInt_Enable(uint8_t en);

void LL_SPIM2_OverflowInt_Enable(uint8_t en);

void LL_SPIM2_ModefaultInt_Enable(uint8_t en);

void LL_SPIM2_Enable(uint8_t en);

void LL_SPIM2_BaudRate_Set(uint8_t baud_rate);
        
void LL_SPIM2_Master_Enable(uint8_t en);
void LL_SPIM2_Clk_Polarity(uint8_t clk_polarity);
void LL_SPIM2_Clk_Phase(uint8_t clk_phase);
uint8_t LL_SPIM2_IsTxEmpty(void);


/**
 * @brief Mode Fault Enable bit.
 *
 * @param en
 */
void LL_SPIM2_ModeFault_Enable(uint8_t en);

void LL_SPIM2_StartTransfer(void);

/**
 * @brief Set transfer direction. 1 for read, 0 for write.
 *
 * @param rw
 */
void LL_SPIM2_Set_ReadWriteDir(uint8_t rw);

void LL_SPIM2_Set_AddrLength(uint8_t length);

void LL_SPIM2_Set_DataLength(uint8_t length);

void LL_SPIM2_TxData0_Set(uint32_t data0);

void LL_SPIM2_TxData1_Set(uint32_t data1);

uint32_t LL_SPIM2_RxData0_Get(void);

uint32_t LL_SPIM2_RxData1_Get(void);

uint8_t LL_SPIM2_RawInt_Status(void);

uint8_t LL_SPIM2_Int_Status(void);

void LL_SPIM2_IntClr(uint8_t int_map);

void LL_SPIM2_IntClr_All(void);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __LL_SPIM2_H__

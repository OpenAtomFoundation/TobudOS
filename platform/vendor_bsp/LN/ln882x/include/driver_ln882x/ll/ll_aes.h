#ifndef __LL_AES_H__
#define __LL_AES_H__

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ln88xx.h"
typedef void* AesInstance;
/*----- AES Control Codes: Crypto Mode -----*/


/*----- AES Control Codes: Mode Parameters: Key length -----*/


/**
 * @brief Set Opcode：00：encrypt 01:decrypt 11:key expand
 *
 * @param AES BASE ADDRSS
 * @param opcode
 * @return  NULL
 */

void LL_AES_Set_Opcode(AesInstance AESx, uint8_t opcode);
/**
 * @brief Set Endian 0:little Endian 1:big Endian
 *
 * @param AES BASE ADDRSS
 * @param endian
 * @return  NULL
 */
void LL_AES_Set_Endian(AesInstance AESx, bool endian);

/**
 * @brief Set Key length 00:128  01:192  10:256 11:reserved
 *
 * @param AES BASE ADDRSS
 * @param keylength
 * @return  NULL
 */
uint32_t LL_AES_Set_Keylen(AesInstance AESx, uint8_t keylength);

/**
 * @brief Set mode  0:ECB  1:CBC
 *
 * @param AES BASE ADDRSS
 * @param mode
 * @return  NULL
 */
void LL_AES_Set_mode(AesInstance AESx, bool mode);

/**
 * @brief Set start  0: disable AES  1:enable AES
 *
 * @param AES BASE ADDRSS
 * @param start
 * @return  NULL
 */
void LL_Aes_Enable(AesInstance AESx, bool start);

/**
 * @brief Set encrypt or decrypt finish Interrupt Enable  0: disable  1:enable
 *
 * @param AES BASE ADDRSS
 * @param data_int_en
 * @return  NULL
 */
void LL_Aes_Enable_Interrupt(AesInstance AESx, bool data_int_en);

/**
 * @brief Set key expand Interrupt Enable  0: disable  1:enable
 *
 * @param AES BASE ADDRSS
 * @param key_int_en
 * @return  NULL
 */
void LL_Aes_Key_Enable_Interrupt(AesInstance AESx, bool key_int_en);

/**
 * @brief write 0 clear key expand Interrupt status   
 *
 * @param AES BASE ADDRSS
 * @param key_int_en is  0
 * @return  NULL
 */
void LL_Aes_Clear_Key_Interrupt(AesInstance AESx, uint8_t key_int_flag);

/**
 * @brief write 0 clear encrypt or decrypt finish Interrupt status   
 *
 * @param AES BASE ADDRSS
 * @param data_int_flag 
 * @return  NULL
 */
void LL_Aes_Clear_Data_Interrupt_Key(AesInstance AESx, uint8_t data_int_flag);

/**
 * @brief get Interrupt status  AES_IT :  0x40: encrypt or decrypt Interrupt， 0x20：key expand Interrupt      
 *
 * @param AES BASE ADDRSS
 * @param AES_IT   
 * @return  1: Interrupt finish  0: no interrupt
 */
uint32_t LL_Aes_Get_Intstatus(AesInstance AESx, uint32_t AES_IT);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __LL_AES_H__ */

/**************************************************************************//**
 * @file     crypto.h
 * @version  V1.10
 * @brief    Cryptographic Accelerator driver header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CRYPTO_Driver CRYPTO Driver
  @{
*/


/** @addtogroup CRYPTO_EXPORTED_CONSTANTS CRYPTO Exported Constants
  @{
*/

#define PRNG_KEY_SIZE_64        0UL     /*!< Select to generate 64-bit random key    \hideinitializer */
#define PRNG_KEY_SIZE_128       1UL     /*!< Select to generate 128-bit random key   \hideinitializer */
#define PRNG_KEY_SIZE_192       2UL     /*!< Select to generate 192-bit random key   \hideinitializer */
#define PRNG_KEY_SIZE_256       3UL     /*!< Select to generate 256-bit random key   \hideinitializer */

#define PRNG_SEED_CONT          0UL     /*!< PRNG using current seed                 \hideinitializer */
#define PRNG_SEED_RELOAD        1UL     /*!< PRNG reload new seed                    \hideinitializer */

#define AES_KEY_SIZE_128        0UL     /*!< AES select 128-bit key length           \hideinitializer */
#define AES_KEY_SIZE_192        1UL     /*!< AES select 192-bit key length           \hideinitializer */
#define AES_KEY_SIZE_256        2UL     /*!< AES select 256-bit key length           \hideinitializer */

#define AES_MODE_ECB            0UL     /*!< AES select ECB mode                     \hideinitializer */
#define AES_MODE_CBC            1UL     /*!< AES select CBC mode                     \hideinitializer */
#define AES_MODE_CFB            2UL     /*!< AES select CFB mode                     \hideinitializer */
#define AES_MODE_OFB            3UL     /*!< AES select OFB mode                     \hideinitializer */
#define AES_MODE_CTR            4UL     /*!< AES select CTR mode                     \hideinitializer */
#define AES_MODE_CBC_CS1        0x10UL  /*!< AES select CBC CS1 mode                 \hideinitializer */
#define AES_MODE_CBC_CS2        0x11UL  /*!< AES select CBC CS2 mode                 \hideinitializer */
#define AES_MODE_CBC_CS3        0x12UL  /*!< AES select CBC CS3 mode                 \hideinitializer */

#define AES_NO_SWAP             0UL     /*!< AES do not swap input and output data   \hideinitializer */
#define AES_OUT_SWAP            1UL     /*!< AES swap output data                    \hideinitializer */
#define AES_IN_SWAP             2UL     /*!< AES swap input data                     \hideinitializer */
#define AES_IN_OUT_SWAP         3UL     /*!< AES swap both input and output data     \hideinitializer */

#define CRYPTO_DMA_FIRST        0x4UL   /*!< Do first encrypt/decrypt in DMA cascade \hideinitializer */
#define CRYPTO_DMA_ONE_SHOT     0x5UL   /*!< Do one shot encrypt/decrypt with DMA      \hideinitializer */
#define CRYPTO_DMA_CONTINUE     0x6UL   /*!< Do continuous encrypt/decrypt in DMA cascade \hideinitializer */
#define CRYPTO_DMA_LAST         0x7UL   /*!< Do last encrypt/decrypt in DMA cascade          \hideinitializer */


/*@}*/ /* end of group CRYPTO_EXPORTED_CONSTANTS */


/** @addtogroup CRYPTO_EXPORTED_MACROS CRYPTO Exported Macros
  @{
*/

/*----------------------------------------------------------------------------------------------*/
/*  Macros                                                                                      */
/*----------------------------------------------------------------------------------------------*/

/**
  * @brief This macro enables PRNG interrupt.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define PRNG_ENABLE_INT(crpt)       ((crpt)->INTEN |= CRPT_INTEN_PRNGIEN_Msk)

/**
  * @brief This macro disables PRNG interrupt.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define PRNG_DISABLE_INT(crpt)      ((crpt)->INTEN &= ~CRPT_INTEN_PRNGIEN_Msk)

/**
  * @brief This macro gets PRNG interrupt flag.
  * @param crpt     Specified cripto module
  * @return PRNG interrupt flag.
  * \hideinitializer
  */
#define PRNG_GET_INT_FLAG(crpt)     ((crpt)->INTSTS & CRPT_INTSTS_PRNGIF_Msk)

/**
  * @brief This macro clears PRNG interrupt flag.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define PRNG_CLR_INT_FLAG(crpt)     ((crpt)->INTSTS = CRPT_INTSTS_PRNGIF_Msk)

/**
  * @brief This macro enables AES interrupt.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define AES_ENABLE_INT(crpt)        ((crpt)->INTEN |= (CRPT_INTEN_AESIEN_Msk|CRPT_INTEN_AESEIEN_Msk))

/**
  * @brief This macro disables AES interrupt.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define AES_DISABLE_INT(crpt)       ((crpt)->INTEN &= ~(CRPT_INTEN_AESIEN_Msk|CRPT_INTEN_AESEIEN_Msk))

/**
  * @brief This macro gets AES interrupt flag.
  * @param crpt     Specified cripto module
  * @return AES interrupt flag.
  * \hideinitializer
  */
#define AES_GET_INT_FLAG(crpt)      ((crpt)->INTSTS & (CRPT_INTSTS_AESIF_Msk|CRPT_INTSTS_AESEIF_Msk))

/**
  * @brief This macro clears AES interrupt flag.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define AES_CLR_INT_FLAG(crpt)      ((crpt)->INTSTS = (CRPT_INTSTS_AESIF_Msk|CRPT_INTSTS_AESEIF_Msk))

/**
  * @brief This macro enables AES key protection.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define AES_ENABLE_KEY_PROTECT(crpt)  ((crpt)->AES_CTL |= CRPT_AES_CTL_KEYPRT_Msk)

/**
  * @brief This macro disables AES key protection.
  * @param crpt     Specified cripto module
  * @return None
  * \hideinitializer
  */
#define AES_DISABLE_KEY_PROTECT(crpt) ((crpt)->AES_CTL = ((crpt)->AES_CTL & ~CRPT_AES_CTL_KEYPRT_Msk) | (0x16UL<<CRPT_AES_CTL_KEYUNPRT_Pos)); \
    ((crpt)->AES_CTL &= ~CRPT_AES_CTL_KEYPRT_Msk)

/*@}*/ /* end of group CRYPTO_EXPORTED_MACROS */



/** @addtogroup CRYPTO_EXPORTED_FUNCTIONS CRYPTO Exported Functions
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/*  Functions                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

void PRNG_Open(CRPT_T *crpt, uint32_t u32KeySize, uint32_t u32SeedReload, uint32_t u32Seed);
void PRNG_Start(CRPT_T *crpt);
void PRNG_Read(CRPT_T *crpt, uint32_t au32RandKey[]);
void AES_Open(CRPT_T *crpt, uint32_t u32Channel, uint32_t u32EncDec, uint32_t u32OpMode, uint32_t u32KeySize, uint32_t u32SwapType);
void AES_Start(CRPT_T *crpt, uint32_t u32Channel, uint32_t u32DMAMode);
void AES_SetKey(CRPT_T *crpt, uint32_t u32Channel, uint32_t au32Keys[], uint32_t u32KeySize);
void AES_SetInitVect(CRPT_T *crpt, uint32_t u32Channel, uint32_t au32IV[]);
void AES_SetDMATransfer(CRPT_T *crpt, uint32_t u32Channel, uint32_t u32SrcAddr, uint32_t u32DstAddr, uint32_t u32TransCnt);


/*@}*/ /* end of group CRYPTO_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group CRYPTO_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __CRYPTO_H__ */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/


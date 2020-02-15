/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_OTFAD_H_
#define _FSL_OTFAD_H_

#include "fsl_common.h"

/*!
 * @addtogroup otfad
 * @{
 */

/******************************************************************************
 * Definitions.
 *****************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.0.0. */
#define FSL_OTFAD_DRIVER_VERSION (MAKE_VERSION(2U, 0U, 0U))
/*@}*/

/*! @brief Status codes for the OTFAD driver. */
enum
{
    kStatus_OTFAD_ResRegAccessMode = MAKE_STATUS(kStatusGroup_OTFAD, 0), /*!< Restricted register mode */
    kStatus_OTFAD_AddressError     = MAKE_STATUS(kStatusGroup_OTFAD, 1), /*!< End address less than start address */
    kStatus_OTFAD_RegionOverlap =
        MAKE_STATUS(kStatusGroup_OTFAD, 2), /*!< the OTFAD does not support any form of memory region overlap,
                                                 for system accesses that hit in multiple contexts or no contexts,
                                                 the fetched data is simply bypassed */
    kStatus_OTFAD_RegionMiss =
        MAKE_STATUS(kStatusGroup_OTFAD, 3), /*!< For accesses that hit in a single context, but not the selected one */
};

/*! @brief OTFAD context type */
enum
{
    kOTFAD_Context_0 = 0U, /*!< context 0 */
    kOTFAD_Context_1 = 1U, /*!< context 1 */
    kOTFAD_Context_2 = 2U, /*!< context 2 */
    kOTFAD_Context_3 = 3U, /*!< context 3 */
};

/*! @brief OTFAD operate mode */
enum
{
    kOTFAD_NRM = 0x00U, /*!< Normal Mode */
    kOTFAD_SVM = 0x02U, /*!< Security Violation Mode */
    kOTFAD_LDM = 0x03U, /*!< Logically Disabled Mode */
};

/*! @brief OTFAD encryption configuration structure */
typedef struct _otfad_encryption_config
{
    bool valid;           /* The context is valid or not */
    bool AESdecryption;   /* AES decryption enable */
    uint8_t readOnly;     /* read write attribute for the entire set of context registers */
    uint8_t contextIndex; /* OTFAD context index */
    uint32_t startAddr;   /* Start address*/
    uint32_t endAddr;     /* End address */
    uint32_t key[4];      /* Encryption key */
    uint32_t counter[2];  /* Encryption counter */
} otfad_encryption_config_t;

/*! @brief OTFAD configuration structure */
typedef struct _otfad_config
{
    bool enableIntRequest;          /* Interrupt request enable */
    uint8_t forceError;             /* Forces the OTFAD¡¯s key blob error flag (SR[KBERR]) to be asserted */
    uint8_t forceSVM;               /* Force entry into SVM after a write */
    uint8_t forceLDM;               /* Force entry into LDM after a write  */
    uint8_t keyBlobScramble;        /* Key blob KEK scrambling */
    uint8_t keyBlobProcess;         /* Key blob processing */
    uint8_t restrictedRegAccess;    /* Restricted register access enable */
    uint8_t CRCIndex;               /* Select which context has its CRC enabled */
    uint8_t startKeyBlobProcessing; /* key blob processing is initiated */
    bool enableOTFAD;               /* OTFAD has decryption enabled */
    void *flexspiBaseAddr;          /*! Driver Base address. */
} otfad_config_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief OTFAD module initialization function.
 *
 * @param config OTFAD configuration.
 */
void OTFAD_GetDefaultConfig(otfad_config_t *config);

/*!
 * @brief OTFAD module initialization function.
 *
 * @param base OTFAD base address.
 * @param config OTFAD configuration.
 */
AT_QUICKACCESS_SECTION_CODE(void OTFAD_Init(OTFAD_Type *base, const otfad_config_t *config));

/*!
 * @brief Deinitializes the OTFAD.
 *
 */
AT_QUICKACCESS_SECTION_CODE(void OTFAD_Deinit(OTFAD_Type *base));

/* @} */

/*!
 * @name Status
 * @{
 */
/*!
 * @brief OTFAD module get operate mode.
 *
 * @param base OTFAD base address.
 */
static inline uint32_t OTFAD_GetOperateMode(OTFAD_Type *base)
{
    return (base->SR & OTFAD_SR_MODE_MASK) >> OTFAD_SR_MODE_SHIFT;
}

/*!
 * @brief OTFAD module get status.
 *
 * @param base OTFAD base address.
 */
static inline uint32_t OTFAD_GetStatus(OTFAD_Type *base)
{
    return base->SR;
}

/*!
 * @name functional
 * @{
 */

/*!
 * @brief OTFAD module set encryption configuration
 *
 * @param base  OTFAD base address.
 * @param config  encryption configuration.
 *
 * Note: if enable keyblob process, the first 256 bytes external memory is use for keyblob data,
 *       so this region shouldn't be in OTFAD region.
 */
status_t OTFAD_SetEncryptionConfig(OTFAD_Type *base, const otfad_encryption_config_t *config);

/*!
 * @brief OTFAD module get encryption configuration
 *
 * @param base  OTFAD base address.
 * @param config  encryption configuration.
 *
 * Note: if enable keyblob process, the first 256 bytes external memory is use for keyblob data,
 *       so this region shouldn't be in OTFAD region.
 */
status_t OTFAD_GetEncryptionConfig(OTFAD_Type *base, otfad_encryption_config_t *config);

/*!
 * @brief OTFAD module do hit determination.
 *
 * @param base  OTFAD base address.
 * @param address  the physical address space assigned to the QuadSPI(FlexSPI) module.
 * @param contextIndex  hitted context region index.
 * @return  status, such as kStatus_Success or kStatus_OTFAD_ResRegAccessMode.
 */
status_t OTFAD_HitDetermination(OTFAD_Type *base, uint32_t address, uint8_t *contextIndex);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif

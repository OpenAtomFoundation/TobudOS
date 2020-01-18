/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "rng.h"
#include "fsl_trng.h"

hal_rng_status_t HAL_RngInit(void)
{
    trng_config_t config;
    hal_rng_status_t status = kStatus_HAL_RngSuccess;
    TRNG_Type *trngArr[]    = TRNG_BASE_PTRS;
    uint32_t randomSeed     = 0;

    (void)TRNG_GetDefaultConfig(&config);
    config.sampleMode = kTRNG_SampleModeVonNeumann;
    /* Init TRNG */
    if ((int32_t)kStatus_InvalidArgument == TRNG_Init(trngArr[0], (void *)&config))
    {
        return kStatus_HAL_RngInvalidArgumen;
    }

    status = HAL_RngHwGetData(&randomSeed, sizeof(randomSeed));
    if (kStatus_HAL_RngSuccess != status)
    {
        return status;
    }
    srand(randomSeed);
    return kStatus_HAL_RngSuccess;
}

void HAL_RngDeinit(void)
{
    TRNG_Type *trngArr[] = TRNG_BASE_PTRS;
    TRNG_Deinit(trngArr[0]);
    return;
}

hal_rng_status_t HAL_RngHwGetData(void *pRandomNo, uint32_t dataSize)
{
    TRNG_Type *trngArr[]    = TRNG_BASE_PTRS;
    hal_rng_status_t status = kStatus_HAL_RngSuccess;

    if (NULL == pRandomNo)
    {
        return kStatus_HAL_RngNullPointer;
    }
    if ((int32_t)kStatus_Success != TRNG_GetRandomData(trngArr[0], pRandomNo, dataSize))
    {
        status = kStatus_HAL_RngInternalError;
    }
    return status;
}
hal_rng_status_t HAL_RngGetData(void *pRandomNo, uint32_t dataSize)
{
    if (NULL == pRandomNo)
    {
        return kStatus_HAL_RngNullPointer;
    }

    for (uint32_t i = 0; i < dataSize; i++)
    {
        ((uint8_t *)pRandomNo)[i] = rand() & 0xFF;
    }
    return kStatus_HAL_RngSuccess;
}

hal_rng_status_t HAL_RngSetSeed(uint32_t seed)
{
    return KStatus_HAL_RngNotSupport;
}

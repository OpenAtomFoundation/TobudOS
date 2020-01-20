/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "crc.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t HAL_CrcCompute(hal_crc_config_t *crcConfig, uint8_t *dataIn, uint32_t length)
{
    uint32_t shiftReg    = crcConfig->crcSeed << ((4U - crcConfig->crcSize) << 3U);
    uint32_t crcPoly     = crcConfig->crcPoly << ((4U - crcConfig->crcSize) << 3U);
    uint32_t crcXorOut   = crcConfig->crcXorOut << ((4U - crcConfig->crcSize) << 3U);
    uint16_t startOffset = crcConfig->crcStartByte;
    uint8_t crcBits      = 8U * crcConfig->crcSize;
    uint32_t computedCRC = 0;
    uint32_t i, j;
    uint8_t data = 0;
    uint8_t bit;

    /* Size 0 will bypass CRC calculation. */
    if (crcConfig->crcSize != 0U)
    {
        for (i = 0UL + startOffset; i < length; i++)
        {
            data = dataIn[i];

            if (crcConfig->crcRefIn == KHAL_CrcRefInput)
            {
                bit = 0U;
                for (j = 0U; j < 8U; j++)
                {
                    bit = (bit << 1);
                    bit |= ((data & 1U) != 0U) ? 1U : 0U;
                    data = (data >> 1);
                }
                data = bit;
            }

            for (j = 0; j < 8U; j++)
            {
                bit  = ((data & 0x80U) != 0U) ? 1U : 0U;
                data = (data << 1);

                if ((shiftReg & 1UL << 31) != 0U)
                {
                    bit = (bit != 0U) ? 0U : 1U;
                }

                shiftReg = (shiftReg << 1);

                if (bit != 0U)
                {
                    shiftReg ^= crcPoly;
                }

                if ((bool)bit && ((crcPoly & (1UL << (32U - crcBits))) != 0U))
                {
                    shiftReg |= (1UL << (32U - crcBits));
                }
                else
                {
                    shiftReg &= ~(1UL << (32U - crcBits));
                }
            }
        }

        shiftReg ^= crcXorOut;

        if (crcConfig->crcByteOrder == KHAL_CrcMSByteFirst)
        {
            computedCRC = (shiftReg >> (32U - crcBits));
        }
        else
        {
            computedCRC = 0;
            j           = 1U;
            for (i = 0; i < 32U; i++)
            {
                computedCRC = (computedCRC << 1);
                computedCRC |= ((shiftReg & j) != 0U) ? 1U : 0U;
                j = (j << 1);
            }
        }
    }

    return computedCRC;
}

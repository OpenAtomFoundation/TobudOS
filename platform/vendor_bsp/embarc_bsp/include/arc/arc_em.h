/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
   --------------------------------------------- */

/**
 * @file
 * @ingroup ARC_HAL_CORE_EM
 * @brief Header file of EM series
 */

/**
 * @addtogroup ARC_HAL_CORE_EM
 * @{
 */

#ifndef H_ARC_EM
#define H_ARC_EM

#include "arc/arc.h"

/**
 * @name Address Generation Unit (AGU) related auxiliary register
 * @{
 */
#define AUX_AGU_AP0     (0x5c0)
#define AUX_AGU_AP1     (0x5c1)
#define AUX_AGU_AP2     (0x5c2)
#define AUX_AGU_AP3     (0x5c3)
#define AUX_AGU_AP4     (0x5c4)
#define AUX_AGU_AP5     (0x5c5)
#define AUX_AGU_AP6     (0x5c6)
#define AUX_AGU_AP7     (0x5c7)
#define AUX_AGU_AP8     (0x5c8)
#define AUX_AGU_AP9     (0x5c9)
#define AUX_AGU_AP10    (0x5ca)
#define AUX_AGU_AP11    (0x5cb)
#define AUX_AGU_AP12    (0x5cc)
#define AUX_AGU_AP13    (0x5cd)
#define AUX_AGU_AP14    (0x5ce)
#define AUX_AGU_AP15    (0x5cf)

#define AUX_AGU_OS0     (0x5d0)
#define AUX_AGU_OS1     (0x5d1)
#define AUX_AGU_OS2     (0x5d2)
#define AUX_AGU_OS3     (0x5d3)
#define AUX_AGU_OS4     (0x5d4)
#define AUX_AGU_OS5     (0x5d5)
#define AUX_AGU_OS6     (0x5d6)
#define AUX_AGU_OS7     (0x5d7)
#define AUX_AGU_OS8     (0x5d8)
#define AUX_AGU_OS9     (0x5d9)
#define AUX_AGU_OS10    (0x5da)
#define AUX_AGU_OS11    (0x5db)
#define AUX_AGU_OS12    (0x5dc)
#define AUX_AGU_OS13    (0x5dd)
#define AUX_AGU_OS14    (0x5de)
#define AUX_AGU_OS15    (0x5df)

#define AUX_AGU_MOD0    (0x5e0)
#define AUX_AGU_MOD1    (0x5e1)
#define AUX_AGU_MOD2    (0x5e2)
#define AUX_AGU_MOD3    (0x5e3)
#define AUX_AGU_MOD4    (0x5e4)
#define AUX_AGU_MOD5    (0x5e5)
#define AUX_AGU_MOD6    (0x5e6)
#define AUX_AGU_MOD7    (0x5e7)
#define AUX_AGU_MOD8    (0x5e8)
#define AUX_AGU_MOD9    (0x5e9)
#define AUX_AGU_MOD10   (0x5ea)
#define AUX_AGU_MOD11   (0x5eb)
#define AUX_AGU_MOD12   (0x5ec)
#define AUX_AGU_MOD13   (0x5ed)
#define AUX_AGU_MOD14   (0x5ee)
#define AUX_AGU_MOD15   (0x5ef)
#define AUX_AGU_MOD16   (0x5f0)
#define AUX_AGU_MOD17   (0x5f1)
#define AUX_AGU_MOD18   (0x5f2)
#define AUX_AGU_MOD19   (0x5f3)
#define AUX_AGU_MOD20   (0x5f4)
#define AUX_AGU_MOD21   (0x5f5)
#define AUX_AGU_MOD22   (0x5f6)
#define AUX_AGU_MOD23   (0x5f7)
/** @} */

#define AUX_XCCM_BASE   (0x5f8)
#define AUX_YCCM_BASE   (0x5f9)

/** @todo Add em series specific definitions here */
#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif  /* H_ARC_EM */

/** @} */
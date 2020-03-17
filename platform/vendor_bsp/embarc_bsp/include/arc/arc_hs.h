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
 * @ingroup ARC_HAL_CORE_HS
 * @brief header file of HS series
 */

/**
 * @addtogroup ARC_HAL_CORE_HS
 * @{
 */

#ifndef H_ARC_HS
#define H_ARC_HS

#include "arc/arc.h"

#define AUX_TLBPD0                  (0x460)
#define AUX_TLBPD1                  (0x461)
#define AUX_TLBPD1_HL               (0x463)
#define AUX_TLBINDEX                (0x464)
#define AUX_TLBCOMMAND              (0x465)
#define AUX_PID                     (0x468)
#define AUX_SASID0                  (0x46a)
#define AUX_SASID1                  (0x46b)
#define AUX_SCRATCH_DATA0           (0x46c)
#define AUX_BPU_FLUSH               (0x480)
#define AUX_BPU_CTRL                (0x481)
#define AUX_IO_COH_ENABLE           (0x500)
#define AUX_IO_COH_PARTIAL          (0x501)
#define AUX_IO_COH_AP0_BASE         (0x508)
#define AUX_IO_COH_AP0_SIZE         (0x509)
#define AUX_IO_CACHE_ENABLE         (0x520)
#define AUX_IO_CACHE_AP0_BASE       (0x528)
#define AUX_IO_CACHE_AP0_SIZE       (0x529)

#define AUX_SLC_AUX_CACHE_CONFIG    (0x901)
#define AUX_SLC_AUX_CACHE_CTRL      (0x903)
#define AUX_SLC_AUX_CACHE_FLUSH     (0x904)
#define AUX_SLC_AUX_CACHE_INV       (0x905)

#define AUX_SLC_AUX_CACHE_LOCK0     (0x906)
#define AUX_SLC_AUX_CACHE_LOCK1     (0x907)
#define AUX_SLC_AUX_CACHE_LOCK2     (0x908)
#define AUX_SLC_AUX_CACHE_LOCK3     (0x909)
#define AUX_SLC_AUX_CACHE_LOCK4     (0x90a)
#define AUX_SLC_AUX_CACHE_LOCK5     (0x90b)
#define AUX_SLC_AUX_CACHE_LOCK6     (0x90c)
#define AUX_SLC_AUX_CACHE_LOCK7     (0x90d)

#define AUX_SLC_AUX_LINE_LOCK       (0x90e)
#define AUX_SLC_AUX_LINE_LOCK1      (0x90f)
#define AUX_SLC_AUX_LINE_INV        (0x910)
#define AUX_SLC_AUX_LINE_INV1       (0x911)
#define AUX_SLC_AUX_LINE_FLUSH      (0x912)
#define AUX_SLC_AUX_LINE_FLUSH1     (0x913)
#define AUX_SLC_AUX_RGN_START       (0x914)
#define AUX_SLC_AUX_RGN_START1      (0x915)
#define AUX_SLC_AUX_RGN_END         (0x916)
#define AUX_SLC_AUX_RGN_END1        (0x917)
#define AUX_SLC_AUX_LINE_ADDR       (0x918)
#define AUX_SLC_AUX_LINE_ADDR1      (0x919)
#define AUX_SLC_AUX_DIRECT_IDX      (0x91a)
#define AUX_SLC_AUX_TAG_DATA        (0x91b)
#define AUX_SLC_AUX_TAG_DATA1       (0x91c)
#define AUX_SLC_AUX_STATUS_DATA     (0x91d)
#define AUX_SLC_AUX_DATA0           (0x91f)
#define AUX_SLC_AUX_DATA1           (0x920)
#define AUX_SLC_AUX_DATA2           (0x921)
#define AUX_SLC_AUX_DATA3           (0x922)
#define AUX_SLC_AUX_FAULT_ADDR      (0x923)
#define AUX_SLC_AUX_FAULT_ADDR1     (0x924)
#define AUX_SLC_AUX_FAULT_STAT      (0x925)
#define AUX_SLC_AUX_PM_CMD          (0x926)
#define AUX_SLC_AUX_PM_EVENT        (0x927)
#define AUX_SLC_AUX_PM_OVF          (0x928)
#define AUX_SLC_AUX_PM_CNT0         (0x929)
#define AUX_SLC_AUX_PM_CNT1         (0x92a)

/** @todo add hs series specific definitions here */
#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif /* H_ARC_HS */
/** @}*/

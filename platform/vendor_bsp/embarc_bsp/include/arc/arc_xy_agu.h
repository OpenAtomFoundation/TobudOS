/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#ifndef H_ARC_XY_AGU
#define H_ARC_XY_AGU

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

#define U0      % r32
#define U1      % r33
#define U2      % r34

#ifdef ARC_FEATURE_XCCM_PRESENT
#define ARC_X_CCM __attribute__ ((section(".x_ccm")))
#else
#define ARC_X_CCM
#endif

#ifdef ARC_FEATURE_YCCM_PRESENT
#define ARC_Y_CCM __attribute__ ((section(".y_ccm")))
#else
#define ARC_Y_CCM
#endif

#define US_EXPAND_MOD_NAME(reg_id) AUX_AGU_MOD ## reg_id
#define US_EXPAND_OS_NAME(reg_id)  AUX_AGU_OS  ## reg_id
#define US_EXPAND_AP_NAME(reg_id)  AUX_AGU_AP  ## reg_id

#define ARC_AGU_MOD_SET(agu_mod, ptr_id, data_type, addressing)	\
	arc_aux_write(US_EXPAND_MOD_NAME(agu_mod), (uint32_t)(ptr_id | data_type | addressing))
#define ARC_AGU_OS_SET(os_id, offset) \
	arc_aux_write(US_EXPAND_OS_NAME(os_id), (uint32_t)offset)
#define ARC_AGU_AP_SET(ap_id, addr) \
	arc_aux_write(US_EXPAND_AP_NAME(ap_id), (uint32_t)addr)
#define ARC_AGU_MOD_GET(mod_id) arc_aux_read(US_EXPAND_MOD_NAME(mod_id))
#define ARC_AGU_OS_GET(os_id) arc_aux_read(US_EXPAND_MOD_NAME(os_id))
#define ARC_AGU_AP_GET(ap_id) arc_aux_read(US_EXPAND_AP_NAME(ap_id))

#define ARC_AGU_MOD_VW(x) ((x) << 4)
#define ARC_AGU_MOD_FX(x) ((x) << 6)
#define ARC_AGU_MOD_DIR(x) ((x) << 11)
#define ARC_AGU_MOD_REV (1 << 12)
#define ARC_AGU_MOD_REP (1 << 13)

#define ARC_AGU_MOD_SC(x) ((x) << 14)
#define ARC_AGU_MOD_OFFSET_REG(x) ((x) << 25)
#define ARC_AGU_MOD_OFFSET_IMM(x) ((x) << 18)

#define ARC_AGU_MOD_WRAP_REG(x) ((x) << 14)
#define ARC_AGU_MOD_WRAP_IMM(x) ((x) << 14)

#define ARC_AGU_MOD_OPC(x) ((x) << 29)

#define ARC_AGU_DT_I32 (ARC_AGU_MOD_FX(11) | ARC_AGU_MOD_VW(2))
#define ARC_AGU_DT_I32_CAST_I16 (ARC_AGU_MOD_FX(8) | ARC_AGU_MOD_VW(2))
#define ARC_AGU_DT_I32_CAST_I8 (ARC_AGU_MOD_FX(4) | ARC_AGU_MOD_VW(2))

#define ARC_AGU_DT_I16 (ARC_AGU_MOD_FX(3) | ARC_AGU_MOD_VW(1))
#define ARC_AGU_DT_I16_CAST_I8 (ARC_AGU_MOD_FX(0) | ARC_AGU_MOD_VW(1))

#define ARC_AGU_DT_I8 (ARC_AGU_MOD_FX(7) | ARC_AGU_MOD_VW(0))

#define ARC_AGU_DT_V2I16 (ARC_AGU_MOD_FX(3) | ARC_AGU_MOD_VW(2))
#define ARC_AGU_DT_V2I16_REPLIC_I16 (ARC_AGU_MOD_FX(3) | ARC_AGU_MOD_VW(2) | ARC_AGU_MOD_REP)
#define ARC_AGU_DT_V2I16_CAST_V2I8 (ARC_AGU_MOD_FX(0) | ARC_AGU_MOD_VW(2))

#define ARC_AGU_DT_V2I8 (ARC_AGU_MOD_FX(7) | ARC_AGU_MOD_VW(1))
#define ARC_AGU_DT_V2I8_REPLIC_I8 (ARC_AGU_MOD_FX(7) | ARC_AGU_MOD_VW(1) | ARC_AGU_MOD_REP)

#define ARC_AGU_DT_V4I8 (ARC_AGU_MOD_FX(7) | ARC_AGU_MOD_VW(2))
#define ARC_AGU_DT_V4I8_REPLIC_I8 (ARC_AGU_MOD_FX(7) | ARC_AGU_MOD_VW(2) | ARC_AGU_MOD_REP)

/* AGU addressing modes with scling factor macroses */

#define ARC_AGU_AD_INC_I32(offset) (ARC_AGU_MOD_OPC(1) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(2))
#define ARC_AGU_AD_DEC_I32(offset) (ARC_AGU_MOD_OPC(1) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(2) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_INC_I32_REG(reg_id) (ARC_AGU_MOD_OPC(0) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(2))
#define ARC_AGU_AD_DEC_I32_REG(reg_id) (ARC_AGU_MOD_OPC(0) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(2) | ARC_AGU_MOD_DIR(1))

#define ARC_AGU_AD_BITREV_INC_I32(offset) (ARC_AGU_MOD_OPC(3) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(2))
#define ARC_AGU_AD_BITREV_DEC_I32(offset) (ARC_AGU_MOD_OPC(3) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(2) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_BITREV_INC_I32_REG(reg_id) (ARC_AGU_MOD_OPC(2) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(2))
#define ARC_AGU_AD_BITREV_DEC_I32_REG(reg_id) (ARC_AGU_MOD_OPC(2) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(2) | ARC_AGU_MOD_DIR(1))

#define ARC_AGU_AD_INC_I16(offset) (ARC_AGU_MOD_OPC(1) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(1))
#define ARC_AGU_AD_DEC_I16(offset) (ARC_AGU_MOD_OPC(1) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(1) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_INC_I16_REG(reg_id) (ARC_AGU_MOD_OPC(0) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(1))
#define ARC_AGU_AD_DEC_I16_REG(reg_id) (ARC_AGU_MOD_OPC(0) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(1) | ARC_AGU_MOD_DIR(1))

#define ARC_AGU_AD_BITREV_INC_I16(offset) (ARC_AGU_MOD_OPC(3) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(1))
#define ARC_AGU_AD_BITREV_DEC_I16(offset) (ARC_AGU_MOD_OPC(3) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(1) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_BITREV_INC_I16_REG(reg_id) (ARC_AGU_MOD_OPC(2) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(1))
#define ARC_AGU_AD_BITREV_DEC_I16_REG(reg_id) (ARC_AGU_MOD_OPC(2) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(1) | ARC_AGU_MOD_DIR(1))

#define ARC_AGU_AD_INC_I8(offset) (ARC_AGU_MOD_OPC(1) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(0))
#define ARC_AGU_AD_DEC_I8(offset) (ARC_AGU_MOD_OPC(1) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(0) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_INC_I8_REG(reg_id) (ARC_AGU_MOD_OPC(0) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(0))
#define ARC_AGU_AD_DEC_I8_REG(reg_id) (ARC_AGU_MOD_OPC(0) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(0) | ARC_AGU_MOD_DIR(1))

#define ARC_AGU_AD_BITREV_INC_I8(offset) (ARC_AGU_MOD_OPC(3) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(0))
#define ARC_AGU_AD_BITREV_DEC_I8(offset) (ARC_AGU_MOD_OPC(3) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_SC(0) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_BITREV_INC_I8_REG(reg_id) (ARC_AGU_MOD_OPC(2) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(0))
#define ARC_AGU_AD_BITREV_DEC_I8_REG(reg_id) (ARC_AGU_MOD_OPC(2) | ARC_AGU_MOD_OFFSET_REG(reg_id) | ARC_AGU_MOD_SC(0) | ARC_AGU_MOD_DIR(1))

/******************************************************************************
*
*  AGU data types with type casting macroses for unsigned int data types
*  unsigned int data type assumes lsb aligned data conversion without sign extension
*
******************************************************************************/

#define ARC_AGU_DT_UI32 ARC_AGU_DT_I32
#define ARC_AGU_DT_UI32_CAST_UI16 (ARC_AGU_MOD_FX(9) | ARC_AGU_MOD_VW(2))
#define ARC_AGU_DT_UI32_CAST_UI8 (ARC_AGU_MOD_FX(5) | ARC_AGU_MOD_VW(2))

#define ARC_AGU_DT_UI16 ARC_AGU_DT_I16
#define ARC_AGU_DT_UI16_CAST_UI8 (ARC_AGU_MOD_FX(1) | ARC_AGU_MOD_VW(1))

#define ARC_AGU_DT_UI8 ARC_AGU_DT_I8

#define ARC_AGU_DT_V2UI16 ARC_AGU_DT_V2I16
#define ARC_AGU_DT_V2UI16_REPLIC_UI16 ARC_AGU_DT_V2I16_REPLIC_I16
#define ARC_AGU_DT_V2UI16_CAST_V2UI8 (ARC_AGU_MOD_FX(1) | ARC_AGU_MOD_VW(2))

#define ARC_AGU_DT_V2UI8 ARC_AGU_DT_V2I8
#define ARC_AGU_DT_V2UI8_REPLIC_UI8 ARC_AGU_DT_V2I8_REPLIC_I8

#define ARC_AGU_DT_V4UI8 ARC_AGU_DT_V4I8
#define ARC_AGU_DT_V4UI8_REPLIC_UI8 ARC_AGU_DT_V4I8_REPLIC_I8

/* AGU addressing modes with scling factor macroses */

#define ARC_AGU_AD_INC_UI32(offset) ARC_AGU_AD_INC_I32(offset)
#define ARC_AGU_AD_DEC_UI32(offset) ARC_AGU_AD_DEC_I32(offset)
#define ARC_AGU_AD_INC_UI32_REG(reg_id) ARC_AGU_AD_INC_I32_REG(reg_id)
#define ARC_AGU_AD_DEC_UI32_REG(reg_id) ARC_AGU_AD_DEC_I32_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_UI32(offset) ARC_AGU_AD_BITREV_INC_I32(offset)
#define ARC_AGU_AD_BITREV_DEC_UI32(offset) ARC_AGU_AD_BITREV_DEC_I32(offset)
#define ARC_AGU_AD_BITREV_INC_UI32_REG(reg_id) ARC_AGU_AD_BITREV_INC_I32_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_UI32_REG(reg_id) ARC_AGU_AD_BITREV_DEC_I32_REG(reg_id)

#define ARC_AGU_AD_INC_UI16(offset) ARC_AGU_AD_INC_I16(offset)
#define ARC_AGU_AD_DEC_UI16(offset) ARC_AGU_AD_DEC_I16(offset)
#define ARC_AGU_AD_INC_UI16_REG(reg_id) ARC_AGU_AD_INC_I16_REG(reg_id)
#define ARC_AGU_AD_DEC_UI16_REG(reg_id) ARC_AGU_AD_DEC_I16_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_UI16(offset) ARC_AGU_AD_BITREV_INC_I16(offset)
#define ARC_AGU_AD_BITREV_DEC_UI16(offset) ARC_AGU_AD_BITREV_DEC_I16(offset)
#define ARC_AGU_AD_BITREV_INC_UI16_REG(reg_id) ARC_AGU_AD_BITREV_INC_I16_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_UI16_REG(reg_id) ARC_AGU_AD_BITREV_DEC_I16_REG(reg_id)

#define ARC_AGU_AD_INC_UI8(offset) ARC_AGU_AD_INC_I8(offset)
#define ARC_AGU_AD_DEC_UI8(offset) ARC_AGU_AD_DEC_I8(offset)
#define ARC_AGU_AD_INC_UI8_REG(reg_id) ARC_AGU_AD_INC_I8_REG(reg_id)
#define ARC_AGU_AD_DEC_UI8_REG(reg_id) ARC_AGU_AD_DEC_I8_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_UI8(offset) ARC_AGU_AD_BITREV_INC_I8(offset)
#define ARC_AGU_AD_BITREV_DEC_UI8(offset) ARC_AGU_AD_BITREV_DEC_I8(offset)
#define ARC_AGU_AD_BITREV_INC_UI8_REG(reg_id) ARC_AGU_AD_BITREV_INC_I8_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_UI8_REG(reg_id) ARC_AGU_AD_BITREV_DEC_I8_REG(reg_id)

/******************************************************************************
*
*  AGU data types with type casting macroses for q.xx data types
*  q.xx data type assumes msb aligned data conversion
*
******************************************************************************/

#define ARC_AGU_DT_Q31 ARC_AGU_DT_I32
#define ARC_AGU_DT_Q31_CAST_Q15 (ARC_AGU_MOD_FX(10) | ARC_AGU_MOD_VW(2))
#define ARC_AGU_DT_Q31_CAST_Q7 (ARC_AGU_MOD_FX(6) | ARC_AGU_MOD_VW(2))

#define ARC_AGU_DT_Q15 ARC_AGU_DT_I16
#define ARC_AGU_DT_Q15_CAST_Q7 (ARC_AGU_MOD_FX(2) | ARC_AGU_MOD_VW(1))

#define ARC_AGU_DT_Q7 ARC_AGU_DT_I8
#define ARC_AGU_DT_Q7_CAST_Q15 (ARC_AGU_MOD_FX(2) | ARC_AGU_MOD_VW(1)) // packing

#define ARC_AGU_DT_V2Q15 ARC_AGU_DT_V2I16
#define ARC_AGU_DT_V2Q15_REPLIC_Q15 ARC_AGU_DT_V2I16_REPLIC_I16
#define ARC_AGU_DT_V2Q15_CAST_V2Q7 (ARC_AGU_MOD_FX(2) | ARC_AGU_MOD_VW(2))

#define ARC_AGU_DT_V2Q7 ARC_AGU_DT_V2I8
#define ARC_AGU_DT_V2Q7_REPLIC_Q7 ARC_AGU_DT_V2I8_REPLIC_I8
#define ARC_AGU_DT_V2Q7_CAST_V2Q15 (ARC_AGU_MOD_FX(2) | ARC_AGU_MOD_VW(2)) // packing

#define ARC_AGU_DT_V4Q7 ARC_AGU_DT_V4I8
#define ARC_AGU_DT_V4Q7_REPLIC_Q7 ARC_AGU_DT_V4I8_REPLIC_I8

/* AGU addressing modes with scling factor macroses for q.xx data types */

#define ARC_AGU_AD_INC_Q31(offset) ARC_AGU_AD_INC_I32(offset)
#define ARC_AGU_AD_DEC_Q31(offset) ARC_AGU_AD_DEC_I32(offset)
#define ARC_AGU_AD_INC_Q31_REG(reg_id) ARC_AGU_AD_INC_I32_REG(reg_id)
#define ARC_AGU_AD_DEC_Q31_REG(reg_id) ARC_AGU_AD_DEC_I32_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_Q31(offset) ARC_AGU_AD_BITREV_INC_I32(offset)
#define ARC_AGU_AD_BITREV_DEC_Q31(offset) ARC_AGU_AD_BITREV_DEC_I32(offset)
#define ARC_AGU_AD_BITREV_INC_Q31_REG(reg_id) ARC_AGU_AD_BITREV_INC_I32_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_Q31_REG(reg_id) ARC_AGU_AD_BITREV_DEC_I32_REG(reg_id)

#define ARC_AGU_AD_INC_Q15(offset) ARC_AGU_AD_INC_I16(offset)
#define ARC_AGU_AD_DEC_Q15(offset) ARC_AGU_AD_DEC_I16(offset)
#define ARC_AGU_AD_INC_Q15_REG(reg_id) ARC_AGU_AD_INC_I16_REG(reg_id)
#define ARC_AGU_AD_DEC_Q15_REG(reg_id) ARC_AGU_AD_DEC_I16_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_Q15(offset) ARC_AGU_AD_BITREV_INC_I16(offset)
#define ARC_AGU_AD_BITREV_DEC_Q15(offset) ARC_AGU_AD_BITREV_DEC_I16(offset)
#define ARC_AGU_AD_BITREV_INC_Q15_REG(reg_id) ARC_AGU_AD_BITREV_INC_I16_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_Q15_REG(reg_id) ARC_AGU_AD_BITREV_DEC_I16_REG(reg_id)

#define ARC_AGU_AD_INC_Q7(offset) ARC_AGU_AD_INC_I8(offset)
#define ARC_AGU_AD_DEC_Q7(offset) ARC_AGU_AD_DEC_I8(offset)
#define ARC_AGU_AD_INC_Q7_REG(reg_id) ARC_AGU_AD_INC_I8_REG(reg_id)
#define ARC_AGU_AD_DEC_Q7_REG(reg_id) ARC_AGU_AD_DEC_I8_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_Q7(offset) ARC_AGU_AD_BITREV_INC_I8(offset)
#define ARC_AGU_AD_BITREV_DEC_Q7(offset) ARC_AGU_AD_BITREV_DEC_I8(offset)
#define ARC_AGU_AD_BITREV_INC_Q7_REG(reg_id) ARC_AGU_AD_BITREV_INC_I8_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_Q7_REG(reg_id) ARC_AGU_AD_BITREV_DEC_I8_REG(reg_id)

/* AGU modificator for reverse vector elements (can be useful for Endian conversions) */

#define ARC_AGU_DT_VECTOR_REVERSE ARC_AGU_MOD_REV

/* AGU wrapping modifiers */

#define ARC_AGU_AD_WRAP_INC_RR(inc_os_id, modulo_os_id) (ARC_AGU_MOD_OPC(4) | ARC_AGU_MOD_OFFSET_REG(inc_os_id) | ARC_AGU_MOD_WRAP_REG(modulo_os_id))
#define ARC_AGU_AD_WRAP_DEC_RR(dec_os_id, modulo_os_id) (ARC_AGU_MOD_OPC(4) | ARC_AGU_MOD_OFFSET_REG(dec_os_id) | ARC_AGU_MOD_WRAP_REG(modulo_os_id) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_WRAP_INC_II(offset, modulo) (ARC_AGU_MOD_OPC(5) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_WRAP_IMM(modulo))
#define ARC_AGU_AD_WRAP_DEC_II(offset, modulo) (ARC_AGU_MOD_OPC(5) | ARC_AGU_MOD_OFFSET_IMM(offset) | ARC_AGU_MOD_WRAP_IMM(modulo) | ARC_AGU_MOD_DIR(1))
#define ARC_AGU_AD_WRAP_INC_RI(dec_os_id, modulo) (ARC_AGU_MOD_OPC(6) | ARC_AGU_MOD_OFFSET_REG(dec_os_id) | ARC_AGU_MOD_WRAP_IMM(modulo))
#define ARC_AGU_AD_WRAP_DEC_RI(dec_os_id, modulo) (ARC_AGU_MOD_OPC(6) | ARC_AGU_MOD_OFFSET_REG(dec_os_id) | ARC_AGU_MOD_WRAP_IMM(modulo) | ARC_AGU_MOD_DIR(1))

/* Immediate Wrapping constants */

#define AGU_WRAP_IMM_2 0
#define AGU_WRAP_IMM_4 1
#define AGU_WRAP_IMM_8 2
#define AGU_WRAP_IMM_16 3
#define AGU_WRAP_IMM_32 4
#define AGU_WRAP_IMM_64 5
#define AGU_WRAP_IMM_128 6
#define AGU_WRAP_IMM_256 7
#define AGU_WRAP_IMM_512 8
#define AGU_WRAP_IMM_1024 9
#define AGU_WRAP_IMM_2048 10
#define AGU_WRAP_IMM_4096 11
#define AGU_WRAP_IMM_8192 12
#define AGU_WRAP_IMM_16384 13
#define AGU_WRAP_IMM_32768 14
#define AGU_WRAP_IMM_65536 15

/* push pop macros to follow calling conventions */

#define ARC_AGU_STORE_REGS(ap_num, mod_num, os_num)										      \
	int32_t agu_temp_idx;													      \
	uint32_t agu_temp_ap[ap_num];												      \
	uint32_t agu_temp_mod[mod_num];												      \
	uint32_t agu_temp_os[os_num];												      \
	int32_t agu_temp_ap_num = ap_num;											      \
	int32_t agu_temp_mod_num = mod_num;											      \
	int32_t agu_temp_os_num = os_num;											      \
	for (agu_temp_idx = 0; agu_temp_idx < ap_num; agu_temp_idx++) agu_temp_ap[agu_temp_idx] = _lr(AGU_AUX_AP0 + agu_temp_idx);    \
	for (agu_temp_idx = 0; agu_temp_idx < mod_num; agu_temp_idx++) agu_temp_mod[agu_temp_idx] = _lr(AGU_AUX_MOD0 + agu_temp_idx); \
	for (agu_temp_idx = 0; agu_temp_idx < os_num; agu_temp_idx++) agu_temp_os[agu_temp_idx] = _lr(AGU_AUX_OS0 + agu_temp_idx); {}

#define ARC_AGU_RESTORE_REGS()														       \
	for (agu_temp_idx = 0; agu_temp_idx < agu_temp_ap_num; agu_temp_idx++) _sr (agu_temp_ap[agu_temp_idx], AGU_AUX_AP0 + agu_temp_idx);    \
	for (agu_temp_idx = 0; agu_temp_idx < agu_temp_mod_num; agu_temp_idx++) _sr (agu_temp_mod[agu_temp_idx], AGU_AUX_MOD0 + agu_temp_idx); \
	for (agu_temp_idx = 0; agu_temp_idx < agu_temp_os_num; agu_temp_idx++) _sr (agu_temp_os[agu_temp_idx], AGU_AUX_OS0 + agu_temp_idx); {}

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif  /* H_ARC_XY_AGU */

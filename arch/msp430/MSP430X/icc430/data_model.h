/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#if __DATA_MODEL__ == __DATA_MODEL_SMALL__
	#define pushm_x pushm.w
	#define popm_x  popm.w
	#define push_x  push.w
	#define pop_x   pop.w
	#define mov_x   mov.w
	#define cmp_x   cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_MEDIUM__
	#define pushm_x pushm.a
	#define popm_x  popm.a
	#define push_x  pushx.a
	#define pop_x   popx.a
	#define mov_x   mov.w
	#define cmp_x   cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_LARGE__
	#define pushm_x pushm.a
	#define popm_x  popm.a
	#define push_x  pushx.a
	#define pop_x   popx.a
	#define mov_x   movx.a
	#define cmp_x   cmpx.a
#endif

#ifndef pushm_x
	#error The assembler options must define one of the following symbols: __DATA_MODEL_SMALL__, __DATA_MODEL_MEDIUM__, or __DATA_MODEL_LARGE__
#endif

#endif /* DATA_MODEL_H */


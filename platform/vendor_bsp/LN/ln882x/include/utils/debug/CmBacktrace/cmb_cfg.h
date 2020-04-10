/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: It is the configure head file for this library.
 * Created on: 2016-12-15
 */

#ifndef _CMB_CFG_H_
#define _CMB_CFG_H_
#include "utils/debug/log.h"
#include "proj_config.h"


/* print line, must config by user */
#define cmb_println(...)               log_printf(##__VA_ARGS__);LOG(LOG_LVL_ERROR, "\r\n")

/* enable bare metal(no OS) platform */
#define CMB_USING_BARE_METAL_PLATFORM 

///* enable OS platform */
//#ifdef __CONFIG_OS_KERNEL
//    #define CMB_USING_OS_PLATFORM 
//#endif

///* OS platform type, must config when CMB_USING_OS_PLATFORM is enable */
//#define CMB_OS_PLATFORM_TYPE          CMB_OS_PLATFORM_FREERTOS 

/* cpu platform type, must config by user */
#define CMB_CPU_PLATFORM_TYPE         CMB_CPU_ARM_CORTEX_M4
    
/* enable dump stack information */
#define CMB_USING_DUMP_STACK_INFO

/* language of print information */
#define CMB_PRINT_LANGUAGE            CMB_PRINT_LANGUAGE_ENGLISH
    
#define CMB_CODE_SECTION_NAME         ER_FLASH

#endif /* _CMB_CFG_H_ */



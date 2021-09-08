/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-02-05     Jianjia Ma   The first version
 * 2021-09-08     derekduke    add tos support
 */

#ifndef __NNOM_PORT_H__
#define __NNOM_PORT_H__

#include <stdlib.h>
#include <stdio.h>

/* use static memory */
#define NNOM_USING_STATIC_MEMORY    // enable to use built in memory allocation on a large static memory block
                                     // must set buf using "nnom_set_static_buf()" before creating a model. 

/* dynamic memory interfaces */
/* when libc is not available, you shall implement the below memory interfaces (libc equivalents). */
#ifndef NNOM_USING_STATIC_MEMORY    
    //#define nnom_malloc(n)      malloc(n)       
    //#define nnom_free(p)        free(p)
		#define nnom_malloc(n)       tos_mmheap_alloc(n)     
		#define nnom_free(n)         tos_mmheap_free(n)
#endif

/* memory interface */
/* when libc is not available, you shall implement your equivalent functions here */
#define nnom_memset(p,v,s)        memset(p,v,s)        
#define nnom_memcpy(dst,src,len)  memcpy(dst,src,len)  

/* runtime & debug */
#define nnom_us_get()       0       // return a microsecond timestamp
#define nnom_ms_get()       0       // return a millisecond timestamp
#define NNOM_LOG(...)       printf(__VA_ARGS__)

/* NNoM configuration */
#define NNOM_BLOCK_NUM  	(16)		// maximum number of memory blocks, increase it when log request.   
#define DENSE_WEIGHT_OPT 	(1)		// if used fully connected layer optimized weights. 

//#define NNOM_TRUNCATE             // disable: backend ops use round to the nearest int (default). enable: floor 

/* Backend format configuration */
//#define NNOM_USING_CHW            // uncomment if using CHW format. otherwise using default HWC format.
                                    // Notes, CHW is incompatible with CMSIS-NN. 
                                    // CHW must be used when using hardware accelerator such as KPU in K210 chip

/* Backend selection */
//#define NNOM_USING_CMSIS_NN       // uncomment if use CMSIS-NN for optimation 


#endif




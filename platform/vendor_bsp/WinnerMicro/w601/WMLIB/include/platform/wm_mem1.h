/***************************************************************************** 
* 
* File Name : wm_mem.h
* 
* Description: memory manager Module 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : dave
* 
* Date : 2014-6-12 
*****************************************************************************/ 


#ifndef WM_MEM_H
#define WM_MEM_H


#include "wm_type_def.h"
//#define WM_MEM_DEBUG 1
#if WM_MEM_DEBUG

#include "list.h"

#define  MEM_HEADER_PATTERN          0x76028412
#define  MEM_TAILER_PATTERN          0x83395627
#define  MEM_FREED_PATTERN           0x82962503

extern u32 alloc_heap_mem_bytes; 
extern u32 alloc_heap_mem_blk_cnt;
extern u32 alloc_heap_mem_max_size;

//
// Note: it's important that the size of MP_MEMORY_BLOCK structure
//       be multiple of 16 bytes.
//
typedef struct _MEMORY_BLOCK {

    struct dl_list  list;    /**< Pointer to next and previous blocks */
    char  *file;             /**< name of the file which is doing the allocation */
    u32    pad;              /**< pad to make the size of whole structure multiple of 16 bytes */
    u32    line;             /**< line number where allocated */
    u32    length;           /**< ulong index of trailer (=(length/4)-1 relative to data start */
    u32    header_pattern;   /**< To help detect underflows. A trailer is also added to find overflows */
} MEMORY_BLOCK, *PMEMORY_BLOCK;
typedef struct _MEMORY_PATTERN{
    u32 pattern0;
    //u32 pattern1;
    //u32 pattern2;
    //u32 pattern3;
}MEMORY_PATTERN, *PMEMORY_PATTERN;
void mem_free_debug(void *p, char* file, int line);
#define tls_mem_free(p)   mem_free_debug( p, __FILE__, __LINE__)
void *mem_alloc_debug(u32 size, char* file, int line);
void mem_free_allocated_blocks(void);
#define tls_mem_alloc(size)   mem_alloc_debug(size, __FILE__, __LINE__)
void * mem_realloc_debug(void *mem_address, u32 size, char* file, int line);
#define tls_mem_realloc(mem_address, size)   mem_realloc_debug(mem_address, size, __FILE__, __LINE__)

void *mem_calloc_debug(u32 n,u32 size,char * file,int line);
#define tls_mem_calloc(n, size) mem_calloc_debug(n, size, __FILE__, __LINE__)
void tls_mem_alloc_info(void);
int  is_safe_addr_debug(void* p, u32 len, char* file, int line);
#define tls_is_safe_addr(p, len)           is_safe_addr_debug(p, len, __FILE__, __LINE__)
#if 1
#define MEMCPY memcpy
#define SMEMCPY MEMCPY
#else
#define MEMCPY(dst,src,len)             do { \
	if(tls_is_safe_addr(dst, len)){ \
	memcpy(dst,src,len);}}while(0)

#define SMEMCPY(dst,src,len)            do { \
	if(tls_is_safe_addr(dst, len)){ \
	memcpy(dst,src,len);}}while(0)
#endif
#else /* WM_MEM_DEBUG */

void * mem_alloc_debug(u32 size);
void mem_free_debug(void *p);
void * mem_realloc_debug(void *mem_address, u32 size);
void *mem_calloc_debug(u32 length, u32 size);

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup MEM_APIs MEM APIs
 * @brief memory manager APIs
 */

/**
 * @addtogroup MEM_APIs
 * @{
 */

/**
 * @brief          This function is called by memory allocation
 *
 * @param[in]      size
 *
 * @retval         NULL    malloc failed
 * @retval         pointer pointer to the address of the allocated memory
 *
 * @note           None
 */
#define tls_mem_alloc(size)   mem_alloc_debug(size)

/**
 * @brief      This function is used to free memory
 *
 * @param      None
 *
 * @return     None
 *
 * @note       None
 */
#define tls_mem_free       mem_free_debug

/**
 * @brief          This function is used to realloc memory
 *
 * @param      	   None
 *
 * @retval         NULL    realloc failed
 * @retval         Pointer pointer to the address of the allocated memory
 *
 * @note           None
 */
#define tls_mem_realloc   mem_realloc_debug

/**
 * @brief          This function is used to calloc memory
 *
 * @param      	   None
 *
 * @retval         NULL    realloc failed
 * @retval         Pointer pointer to the address of the allocated memory
 *
 * @note           None
 */
#define tls_mem_calloc    mem_calloc_debug

/**
 * @brief          This function is used to copy memory content from one address to another address
 *
 * @param[in]      dst   pointer to destination address
 * @param[in]      src   pointer to source address
 * @param[in]      len   length to copy
 *
 * @retval         dst
 *
 * @note           None
 */
#define MEMCPY(dst,src,len)      memcpy(dst,src,len)

/**
 * @brief          This function is used to copy memory content from one address to another address
 *
 * @param[in]      dst   pointer to destination address
 * @param[in]      src   pointer to source address
 * @param[in]      len   length to copy
 *
 * @retval         dst
 *
 * @note           None
 */
#define SMEMCPY(dst,src,len)    memcpy(dst,src,len)

/**
 * @brief          This function is used to get available memory
 *
 * @param[in]    None  
 *
 * @return         None
 *
 * @note          This api just can get coarse memory that is remained
 */
unsigned int tls_mem_get_avail_heapsize(void);


/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_MEM_DEBUG */

#endif /* TLS_MEM_H */


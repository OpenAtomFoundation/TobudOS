/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "generic_list.h"
#include "mem_manager.h"
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
#include "fsl_debug_console.h"
#endif

/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#define __mem_get_LR() __get_LR()
#elif defined(__GNUC__)
#define __mem_get_LR() __builtin_return_address(0)
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define __mem_get_LR() __return_address()
#endif
/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/
/*! @brief Buffer pools structure*/
typedef struct _mem_pool_structure
{
    list_label_t freeAnchor;
    struct _mem_pool_structure *nextPool;
    uint8_t *pHeap;
    uint32_t heapSize;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    list_label_t allocatedAnchor;
    uint16_t allocatedBlocksPeak;
    uint16_t poolFragmentWaste;
    uint16_t poolTotalFragmentWaste;
    uint16_t poolFragmentWastePeak;
    uint16_t poolFragmentMinWaste;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    uint16_t poolId;
    uint16_t blockSize;
    uint16_t numBlocks;
    uint16_t allocatedBlocks;
} mem_pool_structure_t;

/*! @brief Header description for buffers.*/
typedef struct _block_list_header
{
    list_element_t link;
    mem_pool_structure_t *pParentPool;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    uint32_t caller;
    uint16_t allocatedBytes;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
} block_list_header_t;

/*! @brief State structure for memory manager. */
typedef struct _mem_manager_info
{
    mem_pool_structure_t *pPool;
    uint16_t poolNum;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    uint16_t allocationFailures;
    uint16_t freeFailures;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
} mem_manager_info_t;

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/
/*****************************************************************************
 *****************************************************************************
 * Private prototypes
 *****************************************************************************
 *****************************************************************************/
/*****************************************************************************
 *****************************************************************************
 * Private memory definitions
 *****************************************************************************
 *****************************************************************************/
static mem_manager_info_t s_memmanager = {0};
/*****************************************************************************
******************************************************************************
* Private API macro define
******************************************************************************
*****************************************************************************/

/*****************************************************************************
******************************************************************************
* Private functions
******************************************************************************
*****************************************************************************/
static mem_status_t MEM_Init(void)
{
    static bool initialized = false;
    assert(sizeof(mem_pool_structure_t) == MEM_POOL_SIZE);
    assert(sizeof(block_list_header_t) == MEM_BLOCK_SIZE);
    if (!initialized)
    {
        s_memmanager.pPool   = NULL;
        s_memmanager.poolNum = 0;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
        s_memmanager.allocationFailures = 0;
        s_memmanager.freeFailures       = 0;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
        initialized = true;
    }
    return kStatus_MemSuccess;
}

/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/
/*!
 * @brief Add memory buffer to memory manager buffer list.
 *
 * @note This API should be called when need add memory buffer to memory manager buffer list. First use
 * MEM_BLOCK_BUFFER_DEFINE to
 *        define memory buffer, then call MEM_AddBuffer function with MEM_BLOCK_BUFFER Macro.
 *  @code
 * MEM_BLOCK_BUFFER_DEFINE(app64, 5, 64,0);
 * MEM_BLOCK_BUFFER_DEFINE(app128, 6, 128,0);
 * MEM_BLOCK_BUFFER_DEFINE(app256, 7, 256,0);
 *
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app64));
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app128));
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app256));
 *  @endcode
 *
 * @param buffer                     Pointer the memory pool buffer, use MEM_BLOCK_BUFFER Macro as the input parameter.
 *
 * @retval kStatus_MemSuccess        Memory manager add Buffer succeed.
 * @retval kStatus_MemInitError      Memory manager add Buffer error occurred.
 */
mem_status_t MEM_AddBuffer(uint8_t *buffer)
{
    mem_config_t *memConfig     = (mem_config_t *)(void *)buffer;
    mem_pool_structure_t *pPool = (mem_pool_structure_t *)memConfig->pbuffer;
    uint8_t *pHeap              = memConfig->pbuffer + sizeof(mem_pool_structure_t);
    mem_pool_structure_t *pPrevPool, *pTempPool;
    uint16_t blockNum;

    assert(buffer);
    assert(memConfig->numberOfBlocks);
    assert(memConfig->blockSize);

    uint32_t regPrimask = DisableGlobalIRQ();
    blockNum            = memConfig->numberOfBlocks;
    (void)MEM_Init();
    pPool->pHeap = pHeap;
    LIST_Init((list_handle_t)(void *)&pPool->freeAnchor, blockNum);
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    LIST_Init((list_handle_t)(void *)&pPool->allocatedAnchor, blockNum);
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    while (0U != blockNum)
    {
        /* Add block to list of free memory. */
        (void)LIST_AddTail((list_handle_t)(void *)&pPool->freeAnchor,
                           (list_element_handle_t)(void *)&((block_list_header_t *)(void *)pHeap)->link);
        ((block_list_header_t *)(void *)pHeap)->pParentPool = pPool;
        /* Add block size (without list header)*/
        pHeap = (uint8_t *)&pHeap[memConfig->blockSize + sizeof(block_list_header_t)];
        blockNum--;
    }
    pPool->numBlocks = memConfig->numberOfBlocks;
    pPool->blockSize = memConfig->blockSize;
    pPool->poolId    = *(uint16_t *)(void *)(&buffer[4]);
    pPool->heapSize =
        (MEM_POOL_SIZE + (uint32_t)memConfig->numberOfBlocks * (MEM_BLOCK_SIZE + (uint32_t)memConfig->blockSize));
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    pPool->allocatedBlocksPeak    = 0;
    pPool->poolTotalFragmentWaste = 0;
    pPool->poolFragmentWaste      = 0;
    pPool->poolFragmentWastePeak  = 0;
    pPool->poolFragmentMinWaste   = 0xffff;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    if (s_memmanager.pPool == NULL)
    {
        s_memmanager.pPool = pPool;
    }
    else
    {
        pTempPool = s_memmanager.pPool;
        pPrevPool = pTempPool;
        while (NULL != pTempPool)
        {
            if (((pPool->blockSize >= pPrevPool->blockSize) && (pPool->blockSize <= pTempPool->blockSize)) ||
                (pPool->blockSize <= pPrevPool->blockSize))
            {
                if (pTempPool == s_memmanager.pPool)
                {
                    s_memmanager.pPool = pPool;
                }
                else
                {
                    pPrevPool->nextPool = pPool;
                }
                pPool->nextPool = pTempPool;
                break;
            }
            pPrevPool = pTempPool;
            pTempPool = pTempPool->nextPool;
        }
        if (pPool->blockSize > pPrevPool->blockSize)
        {
            pPrevPool->nextPool = pPool;
        }
    }

    s_memmanager.poolNum++;
    EnableGlobalIRQ(regPrimask);
    return kStatus_MemSuccess;
}

/*!
 * @brief Remove memory buffer from memory manager buffer list.
 *
 * @note This API should be called when need remove memory buffer to memory manager buffer list. Use with
 * MEM_BLOCK_BUFFER Macro as input parameter.
 *
 * @param buffer                     Pointer the memory pool buffer, use MEM_BLOCK_BUFFER Macro as the input parameter.
 *
 * @retval kStatus_MemSuccess        Memory manager remove buffer succeed.
 * @retval kStatus_MemUnknownError      Memory manager remove buffer error occurred.
 */
#if (defined(MEM_MANAGER_BUFFER_REMOVE) && (MEM_MANAGER_BUFFER_REMOVE > 0U))
mem_status_t MEM_RemoveBuffer(uint8_t *buffer)
{
    mem_config_t *memConfig     = (mem_config_t *)(void *)buffer;
    mem_pool_structure_t *pPool = (mem_pool_structure_t *)memConfig->pbuffer;
    uint8_t *pHeap = (uint8_t *)((mem_pool_structure_t *)memConfig->pbuffer + sizeof(mem_pool_structure_t));
    mem_pool_structure_t *pPrevPool, *pTempPool;

    assert(buffer);
    assert(((mem_config_t *)buffer)->numberOfBlocks);
    assert(((mem_config_t *)buffer)->blockSize);

    uint32_t regPrimask = DisableGlobalIRQ();
    pTempPool           = s_memmanager.pPool;
    pPrevPool           = pTempPool;
    while (NULL != pTempPool)
    {
        if (0U != pPool->allocatedBlocks)
        {
            break;
        }
        if (pTempPool->pHeap == pHeap)
        {
            if (pPool == s_memmanager.pPool)
            {
                s_memmanager.pPool = pPool->nextPool;
            }
            else
            {
                pPrevPool->nextPool = pPool->nextPool;
            }
            s_memmanager.poolNum--;
            EnableGlobalIRQ(regPrimask);
            return kStatus_MemSuccess;
        }
        pPrevPool = pTempPool;
        pTempPool = pTempPool->nextPool;
    }
    EnableGlobalIRQ(regPrimask);
    return kStatus_MemUnknownError;
}
#endif // MEM_MANAGER_BUFFER_REMOVE

/*!
 * @brief Memory buffer allocate.
 *
 * @param numBytes           The number of bytes will be allocated.
 * @retval Memory buffer address when allocate success, NULL when allocate fail.
 */
void *MEM_BufferAlloc(uint32_t numBytes)
{
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    uint32_t fragmentWaste = 0;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    mem_pool_structure_t *pPool = s_memmanager.pPool;
    block_list_header_t *pBlock;

    uint32_t regPrimask = DisableGlobalIRQ();

    while (0U != numBytes)
    {
        if ((numBytes <= pPool->blockSize))
        {
            pBlock = (block_list_header_t *)(void *)LIST_RemoveHead((list_handle_t)(void *)&pPool->freeAnchor);

            if (NULL != pBlock)
            {
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
                pBlock->allocatedBytes = (uint16_t)numBytes;
                pBlock->caller         = (uint32_t)((uint32_t *)__mem_get_LR);
                (void)LIST_AddTail((list_handle_t)(void *)&pPool->allocatedAnchor,
                                   (list_element_handle_t)(void *)&((block_list_header_t *)pBlock)->link);
#endif /*MEM_MANAGER_ENABLE_TRACE*/
                pBlock++;
                pPool->allocatedBlocks++;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
                if (pPool->allocatedBlocks > pPool->allocatedBlocksPeak)
                {
                    pPool->allocatedBlocksPeak = pPool->allocatedBlocks;
                }
                fragmentWaste = pPool->blockSize - numBytes;
                if (fragmentWaste > pPool->poolFragmentWastePeak)
                {
                    pPool->poolFragmentWastePeak = (uint16_t)fragmentWaste;
                }
                pPool->poolFragmentWaste = (uint16_t)fragmentWaste;
                pPool->poolTotalFragmentWaste += (uint16_t)fragmentWaste;
                if (fragmentWaste < pPool->poolFragmentMinWaste)
                {
                    pPool->poolFragmentMinWaste = (uint16_t)fragmentWaste;
                }
                pBlock->allocatedBytes = (uint16_t)numBytes;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
                EnableGlobalIRQ(regPrimask);
                return pBlock;
            }
        }
        /* Try next pool*/
        pPool = pPool->nextPool;
        if (NULL == pPool)
        {
            break;
        }
    }
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    s_memmanager.allocationFailures++;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    EnableGlobalIRQ(regPrimask);
    return NULL;
}

/*!
 * @brief Memory buffer free.
 *
 * @param buffer                     The memory buffer address will be free.
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
mem_status_t MEM_BufferFree(void *buffer /* IN: Block of memory to free*/
)
{
    block_list_header_t *pBlock;
    mem_pool_structure_t *pParentPool;
    mem_pool_structure_t *pool;

    uint32_t regPrimask = DisableGlobalIRQ();
    do
    {
        if (NULL == buffer)
        {
            break;
        }
        pBlock      = (block_list_header_t *)buffer - 1;
        pParentPool = (mem_pool_structure_t *)pBlock->pParentPool;

        assert(((uint8_t *)pBlock >= (uint8_t *)pParentPool->pHeap) &&
               ((uint8_t *)pBlock <= ((uint8_t *)pParentPool->pHeap + pParentPool->heapSize)));

        pool = s_memmanager.pPool;
        while (NULL != pool)
        {
            if (pParentPool == pool)
            {
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
                (void)LIST_RemoveElement((list_element_handle_t)(void *)&((block_list_header_t *)pBlock)->link);
#endif /*MEM_MANAGER_ENABLE_TRACE*/
                (void)LIST_AddTail((list_handle_t)(void *)&pParentPool->freeAnchor,
                                   (list_element_handle_t)(void *)&pBlock->link);
                pParentPool->allocatedBlocks--;
                EnableGlobalIRQ(regPrimask);
                return kStatus_MemSuccess;
            }
            pool = pool->nextPool;
        }
        break;
    } while (true);

#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    s_memmanager.freeFailures++;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    EnableGlobalIRQ(regPrimask);
    return kStatus_MemFreeError;
}

/*!
 * @brief Returns the size of a given buffer.
 *
 * @param buffer                     The memory buffer address will be free.
 * @retval The size of a given buffer.
 */
uint16_t MEM_BufferGetSize(void *buffer) /* IN: Block of memory to get size*/
{
    block_list_header_t *pBlock;
    mem_pool_structure_t *pParentPool;

    uint32_t regPrimask = DisableGlobalIRQ();

    pBlock      = (block_list_header_t *)buffer - 1;
    pParentPool = (mem_pool_structure_t *)pBlock->pParentPool;

    if (buffer != NULL)
    {
        EnableGlobalIRQ(regPrimask);
        return pParentPool->blockSize;
    }
    EnableGlobalIRQ(regPrimask);
    return 0;
}

/*!
 * @brief Trace memory manager all information to use debug.
 *
 */
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
void MEM_Trace(void)
{
    uint32_t i;
    mem_pool_structure_t *pPool = s_memmanager.pPool;
    block_list_header_t *pBlock;
    (void)PRINTF("MEM_Trace debug information, Pools Number:%d   allocationFailures: %d  freeFailures:%d \r\n",
                 s_memmanager.poolNum, s_memmanager.allocationFailures, s_memmanager.allocationFailures,
                 s_memmanager.freeFailures);
    while (NULL != pPool)
    {
        (void)PRINTF("POOL: ID %d  blockSize:%d   status:\r\n", pPool->poolId, pPool->blockSize);
        (void)PRINTF(
            "numBlocks allocatedBlocks  allocatedBlocksPeak  poolFragmentWaste poolFragmentWastePeak "
            "poolFragmentMinWaste poolTotalFragmentWaste\r\n");
        (void)PRINTF(
            "    %d          %d                %d                %d                  %d                       %d       "
            "              %d           \r\n",
            pPool->numBlocks, pPool->allocatedBlocks, pPool->allocatedBlocksPeak, pPool->poolFragmentWaste,
            pPool->poolFragmentWastePeak, pPool->poolFragmentMinWaste, pPool->poolTotalFragmentWaste);
        (void)PRINTF("Currently pool meory block allocate status: \r\n");
        pBlock = (block_list_header_t *)(void *)LIST_GetHead((list_handle_t)(void *)&pPool->allocatedAnchor);
        i      = 0;
        while (NULL != pBlock)
        {
            (void)PRINTF("Block %d caller : 0x%x Allocated  bytes: %d  \r\n", i++, pBlock->caller,
                         pBlock->allocatedBytes);
            pBlock = (block_list_header_t *)(void *)LIST_GetNext((list_element_handle_t)(void *)pBlock);
        }
        /* Try next pool*/
        pPool = pPool->nextPool;
    }
}
#endif /*MEM_MANAGER_ENABLE_TRACE*/

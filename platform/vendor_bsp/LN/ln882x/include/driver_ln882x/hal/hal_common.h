#ifndef __HAL_COMMON_H_
#define __HAL_COMMON_H_

#ifdef __cpluscplus
    extern "C" {
#endif // __cpluscplus

#include "proj_config.h"
#include "stdint.h"
        
//---------------------HAL define------------------------------
typedef enum 
{
	HAL_OK       = 0x00,
	HAL_ERROR    = 0x01,
	HAL_BUSY     = 0x02,
	HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

typedef enum 
{
	HAL_UNLOCKED = 0x00,
	HAL_LOCKED   = 0x01  
} HAL_LockTypeDef;

        
#if (FULL_ASSERT == ENABLE)
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed. 
  *         If expr is true, it returns no value.
  * @retval None
  */
    void assert_failed(uint8_t* file, uint32_t line);
    #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
#else
    #define assert_param(expr) ((void)0U)
#endif /* FULL_ASSERT */  


#ifdef __cpluscplus
    }
#endif // __cpluscplus
#endif // __HAL_COMMON_H_

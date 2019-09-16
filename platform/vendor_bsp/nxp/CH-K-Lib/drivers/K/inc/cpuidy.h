/**
  ******************************************************************************
  * @file    cpuidy.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为内部文件，用于获取芯片的出厂信息，少部分用户使用
  ******************************************************************************
  */
#ifndef __CH_LIB_CPUIDY_H__
#define __CH_LIB_CPUIDY_H__

#include <stdint.h>

/**
 * \enum  CPUIDY_MemSize_Type
 * \brief 芯片信息
 */
typedef enum
{
    kPFlashSizeInKB,   /**< 芯片内部的PFlash大小 */              
    kDFlashSizeInKB,   /**< 芯片内部的DFlash大小 */ 
    kFlexNVMSizeInKB,  /**< 芯片内部的FlexNVMFlash大小 */ 
    kEEPROMSizeInByte, /**< 芯片内部的EEPROM大小 */ 
    kRAMSizeInKB,      /**< 芯片内部的RAM大小 */ 
    kMemNameCount,     /**< 暂未定义 */
}CPUIDY_MemSize_Type;


/* API functions */
char *CPUIDY_GetFamID(void);
uint32_t CPUIDY_GetPinCount(void);
uint32_t CPUIDY_GetMemSize(CPUIDY_MemSize_Type memSizeName);
void CPUIDY_GetUID(uint32_t * UIDArray);


#endif


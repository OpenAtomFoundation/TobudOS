/**
  ******************************************************************************
  * @file    cpuidy.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @date    2015.10.04 FreeXc 完善了cpuidy.c & cpuidy.h文件的注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为内部文件，用于获取芯片的出厂信息，少部分用户使用
  ******************************************************************************
  */
#include "cpuidy.h"
#include "common.h"

/* memory size filed */
#define PFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#define DFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#if defined(SIM_FCFG1_NVMSIZE_MASK)
#define FLEXNVM_SIZE_FIELD          ((SIM->FCFG1 & SIM_FCFG1_NVMSIZE_MASK)>>SIM_FCFG1_NVMSIZE_SHIFT)
#endif

#if defined(SIM_FCFG1_EESIZE_MASK)
#define EEPROM_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_EESIZE_MASK)>>SIM_FCFG1_EESIZE_SHIFT) 
#endif
#define RAM_SIZE_FIELD              ((SIM->SOPT1 & SIM_SOPT1_RAMSIZE_MASK)>>SIM_SOPT1_RAMSIZE_SHIFT)

/* memory size table */
static const uint32_t CPUIDY_PFlashSizeTable[16] = {8, 16, 24, 32, 48, 64, 96 ,128, 192, 256, 0, 512, 0, 0, 0, 0};
static const uint32_t CPUIDY_RAMSizeTable[16] = {4, 8, 12, 16, 24, 32, 48, 64, 96, 128, 0, 0, 0, 0, 0, 0};
static const uint32_t CPUIDY_EEPROMSizeTable[16] = {16*1024, 8*1024, 4*1024, 2*1024, 1*1024, 512, 256, 128, 64, 32, 0, 0, 0, 0, 0};
static const uint32_t CPUIDY_NVMSizeTable[16] = {8, 16, 24, 32, 48, 64, 96, 128, 192, 256 ,0, 512, 0, 0, 0, 0};

/* pin count field */
#define PINCOUNT_FIELD              ((SIM->SDID & SIM_SDID_PINID_MASK)>>SIM_SDID_PINID_SHIFT)
static const uint32_t CPUIDY_PinCountTable[16] = {16, 24, 32, 46, 48, 64, 80, 81, 100, 121, 144, 168, 196, 200, 256};

/* family type field */
#define FAMID_FIELD                 ((SIM->SDID & SIM_SDID_FAMID_MASK)>>SIM_SDID_FAMID_SHIFT) 

/* family type string */
static const char *CPUIDY_FamIDTable[8] = 
{
    "K10",
    "K20",
    "K61 or K30",
    "K40",
    "K60",
    "K70",
    "K50 or K52",
    "K51 or K53",
};


/**
 * @brief  获得芯片的系列ID
 * @code
 *      // 打印芯片型号
 *      printf("Family Type:%s\r\n", CPUIDY_GetFamID());
 * @endcode
 * @retval ID字符串指针
 */
char *CPUIDY_GetFamID(void)
{
    return (char*)CPUIDY_FamIDTable[FAMID_FIELD];
}

/**
 * @brief  获得芯片的引脚数
 * @code
 *      // 获取芯片的引脚数
 *      uint32_t PinCnt;
 *      PinCnt = CPUIDY_GetPinCount();
 *      //将芯片的引脚数显示出来
 *      printf("Pin Cnt:%d\r\n", PinCnt);
 * @endcode
 * @retval 引脚数量
 */
uint32_t CPUIDY_GetPinCount(void)
{
    return CPUIDY_PinCountTable[PINCOUNT_FIELD];
}

/**
 * @brief  获得芯片ROM/RAM 大小
 * @code
 *      // 获取芯片Pflash的大小并显示出来
 *      uint32_t PFlashSize;
 *      PFlashSize = CPUIDY_GetMemSize(kPFlashSizeInKB);
 *      printf("PFlash Size:%dKB\r\n", PFlashSize);
 * @endcode
 * @param[in]  memSizeName 存储器类型
 *              @arg kPFlashSizeInKB   编程Flash的尺寸
 *              @arg kDFlashSizeInKB   数据Flash的尺寸
 *              @arg kFlexNVMSizeInKB  FlexNVM的尺寸
 *              @arg kEEPROMSizeInByte EEPOROM的尺寸
 *              @arg kRAMSizeInKB      芯片RAM的尺寸
 * @retval None
 */
uint32_t CPUIDY_GetMemSize(CPUIDY_MemSize_Type memSizeName)
{
    uint32_t ret_value;
    switch (memSizeName)
		{
        case kPFlashSizeInKB:
            ret_value = CPUIDY_PFlashSizeTable[PFLASH_SIZE_FIELD];
            break;
        case kDFlashSizeInKB:
            ret_value = 0;
            break;
        case kFlexNVMSizeInKB:
            #if defined(SIM_FCFG1_NVMSIZE_MASK)
            ret_value = CPUIDY_NVMSizeTable[FLEXNVM_SIZE_FIELD];
            #endif
            break;
        case kEEPROMSizeInByte:
            #if defined(SIM_FCFG1_EESIZE_MASK)
            ret_value = CPUIDY_EEPROMSizeTable[EEPROM_SIZE_FIELD];
            #endif
            break;
        case kRAMSizeInKB:
            ret_value = CPUIDY_RAMSizeTable[RAM_SIZE_FIELD];
            break;
        default:
            ret_value = 0;
            break;
		}
    return ret_value;
}

/**
 * @brief  获得芯片UID信息(全球唯一识别码)
 * @code
 *      // 获取芯片的UID并显示出来
 *      uint32_t UID[4],i;
 *      CPUIDY_GetUID(UID);
 *      for(i = 0; i < 4; i++)
 *      {
 *          printf("UID[%d]:0x%x\n", i, UID[i]);
 *      }
 * @endcode
 * @param[in]  UIDArray UID数据的首地址
 * @retval None
 */
void CPUIDY_GetUID(uint32_t * UIDArray)
{
    UIDArray[0] = SIM->UIDL;
    UIDArray[1] = SIM->UIDML;
    UIDArray[2] = SIM->UIDMH;
    UIDArray[3] = SIM->UIDH;
}


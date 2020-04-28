/**
  ******************************************************************************
  * @file    stm8l15x_flash.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the FLASH firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************  
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_FLASH_H
#define __STM8L15x_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup FLASH
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @addtogroup FLASH_Exported_Constants
  * @{
  */
#define FLASH_PROGRAM_START_PHYSICAL_ADDRESS       ((uint32_t)0x00008000) /*!< Flash: start address */  
#define FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS   ((uint32_t)0x00001000) /*!< Data Eeprom: start address */

/* STM8L15x High density devices */
#if defined (STM8L15X_HD) || defined (STM8L05X_HD_VL) 
 #define FLASH_PROGRAM_END_PHYSICAL_ADDRESS         ((uint32_t)0x00017FFF) /*!< Flash: end address */
 #define FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS     ((uint32_t)0x000017FF) /*!< Data Eeprom: end address */
 #define FLASH_PROGRAM_BLOCKS_NUMBER                ((uint16_t)0x200)      /*!< Flash memory: total number of Block */
 #define FLASH_DATA_EEPROM_BLOCKS_NUMBER            ((uint8_t)0x10)        /*!< Data EEprom: total number of Block */
 #define FLASH_BLOCK_SIZE                           ((uint8_t)0x80)        /*!< Number of bytes in a Block 
                                                                              (common for Program and Data EEprom memories) */
 
/* STM8L15x Medium density and Medium density plus devices */ 
#elif defined (STM8L15X_MD) || defined (STM8L15X_MDP) || defined (STM8AL31_L_MD) || defined (STM8L05X_MD_VL)
 #define FLASH_PROGRAM_END_PHYSICAL_ADDRESS         ((uint32_t)0x0000FFFF) /*!< Flash: end address */
 #define FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS     ((uint32_t)0x000013FF) /*!< Data Eeprom: end address */
 #define FLASH_PROGRAM_BLOCKS_NUMBER                ((uint16_t)0x100)      /*!< Flash memory: total number of Block */
 #define FLASH_DATA_EEPROM_BLOCKS_NUMBER            ((uint8_t)0x8)         /*!< Data EEprom: total number of Block */
 #define FLASH_BLOCK_SIZE                           ((uint8_t)0x80)        /*!< Number of bytes in a Block 
                                                                              (common for Program and Data EEprom memories) */

/* STM8L15x Low density devices */ 
#elif defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
 #define FLASH_PROGRAM_END_PHYSICAL_ADDRESS         ((uint32_t)0x00009FFF) /*!< Flash: end address */
 #define FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS     ((uint32_t)0x000010FF) /*!< Data Eeprom: end address */
 #define FLASH_PROGRAM_BLOCKS_NUMBER                ((uint16_t)0x80)       /*!< Flash memory: total number of Block */
 #define FLASH_DATA_EEPROM_BLOCKS_NUMBER            ((uint8_t)0x4)         /*!< Data EEprom: total number of Block */
 #define FLASH_BLOCK_SIZE                           ((uint8_t)0x40)        /*!< Number of bytes in a Block 
                                                                              (common for Program and Data EEprom memories) */
#endif /* STM8L15X_HD or STM8L05X_HD_VL*/

/*Common defines for all STM8L15x devices */
#define FLASH_OPTION_BYTES_START_PHYSICAL_ADDRESS  ((uint32_t)0x00004800) /*!< Option bytes: start address */
#define FLASH_OPTION_BYTES_END_PHYSICAL_ADDRESS    ((uint32_t)0x0000480A) /*!< Option bytes: end address */
#define FLASH_RASS_KEY1                            ((uint8_t)0x56)        /*!< First RASS key */
#define FLASH_RASS_KEY2                            ((uint8_t)0xAE)        /*!< Second RASS key */
#define FLASH_READOUTPROTECTION_KEY                ((uint8_t)0xAA)        /*!< Read out protection key */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup FLASH_Exported_Types
  * @{
  */

/** @defgroup FLASH_Memory_Type
  * @{
  */
  typedef enum
  {
    FLASH_MemType_Program      = (uint8_t)0xFD, /*!< Program memory */
    FLASH_MemType_Data         = (uint8_t)0xF7  /*!< Data EEPROM memory */
  } FLASH_MemType_TypeDef;

/**
  * @}
  */
  
/** @defgroup FLASH_Programming_Mode
  * @{
  */
typedef enum
{
  FLASH_ProgramMode_Standard = (uint8_t)0x00, /*!< Standard programming mode */
  FLASH_ProgramMode_Fast     = (uint8_t)0x10  /*!< Fast programming mode */
} FLASH_ProgramMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup FLASH_Programming_Time
  * @{
  */
typedef enum
{
  FLASH_ProgramTime_Standard = (uint8_t)0x00, /*!< Standard programming time fixed at 1/2 tprog */
  FLASH_ProgramTime_TProg    = (uint8_t)0x01  /*!< Programming time fixed at tprog */
} FLASH_ProgramTime_TypeDef;

/**
  * @}
  */
  
/** @defgroup FLASH_Power_Mode
  * @{
  */
typedef enum
{
  FLASH_Power_IDDQ = (uint8_t)0x00,  /*!< Flash program and data EEPROM in IDDQ */
  FLASH_Power_On    = (uint8_t)0x01  /*!< Flash program and data EEPROM not in IDDQ */
} FLASH_Power_TypeDef;

/**
  * @}
  */
  
/** @defgroup FLASH_Status
  * @{
  */
typedef enum
{
  FLASH_Status_Write_Protection_Error = (uint8_t)0x01, /*!< Write attempted to protected Block */
  FLASH_Status_TimeOut                = (uint8_t)0x02, /*!< Time out error */
  FLASH_Status_Successful_Operation   = (uint8_t)0x04  /*!< End of operation flag */
} FLASH_Status_TypeDef;

/**
  * @}
  */
  
/** @defgroup FLASH_Power_Status
  * @{
  */
typedef enum
{
  FLASH_PowerStatus_IDDQDuringWaitMode          = (uint8_t)0x04, /*!< Flash program and data EEPROM
                                                                            in IDDQ during Wait mode*/
  FLASH_PowerStatus_IDDQDuringRunMode           = (uint8_t)0x08, /*!< Flash program and data EEPROM
                                                                            in IDDQ mode during Run mode*/
  FLASH_PowerStatus_IDDQDuringWaitAndRunModes   = (uint8_t)0x0C,  /*!<Flash program and data EEPROM
                                                                            in IDDQ during Wait and run modes*/
  FLASH_PowerStatus_On                          = (uint8_t)0x00  /*!< Flash program and data EEPROM
                                                                                    is powered on during Wait and Run modes */
} FLASH_PowerStatus_TypeDef;

/**
  * @}
  */

/** @defgroup FLASH_Flags
  * @{
  */
typedef enum {
  FLASH_FLAG_HVOFF     = (uint8_t)0x40,     /*!< End of high voltage flag */
  FLASH_FLAG_DUL       = (uint8_t)0x08,     /*!< Data EEPROM unlocked flag */
  FLASH_FLAG_EOP       = (uint8_t)0x04,     /*!< End of programming (write or erase operation) flag */
  FLASH_FLAG_PUL       = (uint8_t)0x02,     /*!< Flash Program memory unlocked flag */
  FLASH_FLAG_WR_PG_DIS = (uint8_t)0x01      /*!< Write attempted to protected page flag */
} FLASH_FLAG_TypeDef;

/**
  * @}
  */
  
/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @addtogroup FLASH_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function in order to check the different
  *   sensitivity values for the flash Address
  */

#define IS_FLASH_PROGRAM_ADDRESS(Address) (((Address) >= FLASH_PROGRAM_START_PHYSICAL_ADDRESS) && \
    ((Address) <= FLASH_PROGRAM_END_PHYSICAL_ADDRESS))


/**
  * @brief Macro used by the assert function in order to check the different
  *   sensitivity values for the data Eeprom Address
  */

#define IS_FLASH_DATA_EEPROM_ADDRESS(Address) (((Address) >= FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS) && \
    ((Address) <= FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS))

/**
  * @brief Macro used by the assert function in order to check the different
  *  sensitivity values for the data eeprom and flash program Address
  */
#define IS_FLASH_ADDRESS(Address)((((Address) >= FLASH_PROGRAM_START_PHYSICAL_ADDRESS) && ((Address) <= FLASH_PROGRAM_END_PHYSICAL_ADDRESS)) || \
                                  (((Address) >= FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS) && ((Address) <= FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS)))

/**
  * @brief Macro used by the assert function in order to check the different
  *   sensitivity values for the option bytes Address
  */
#define IS_OPTION_BYTE_ADDRESS(ADDRESS) (((ADDRESS) >= FLASH_OPTION_BYTES_START_PHYSICAL_ADDRESS) && \
    ((ADDRESS) <= FLASH_OPTION_BYTES_END_PHYSICAL_ADDRESS))

/**
  * @brief Macro used by the assert function in order to check the different
  *   sensitivity values for the flash Block number
  */
#define IS_FLASH_PROGRAM_BLOCK_NUMBER(BlockNum) ((BlockNum) < FLASH_PROGRAM_BLOCKS_NUMBER)

/**
  * @brief Macro used by the assert function in order to check the different
  *   sensitivity values for the data eeprom Block number
  */
#define IS_FLASH_DATA_EEPROM_BLOCK_NUMBER(BlockNum) ((BlockNum) < FLASH_DATA_EEPROM_BLOCKS_NUMBER)

/**
  * @brief Macro used by the assert function in order to check the different
  *   sensitivity values for the flash memory type
  */
#define IS_FLASH_MEMORY_TYPE(MemType) (((MemType) == FLASH_MemType_Program) || \
                                       ((MemType) == FLASH_MemType_Data))

/**
  * @brief Macro used by the assert function in order to check the different
  *   sensitivity values for the flash program block mode
  */
#define IS_FLASH_PROGRAM_MODE(Mode) (((Mode) == FLASH_ProgramMode_Standard) || \
                                     ((Mode) == FLASH_ProgramMode_Fast))

/**
  * @brief Macro used by the assert function in order to check the program time mode
  */
#define IS_FLASH_PROGRAM_TIME(Time) (((Time) == FLASH_ProgramTime_Standard) || \
                                     ((Time) == FLASH_ProgramTime_TProg))

/**
  * @brief Macro used by the assert function in order to check the power mode
  */
#define IS_FLASH_POWER(Power) (((Power) == FLASH_Power_IDDQ) || \
                               ((Power) == FLASH_Power_On))

/**
  * @brief Macro used by the assert function in order to check the power status during wait and run modes
  */
#define IS_FLASH_POWERSTATUS(PowerStatus) (((PowerStatus) == FLASH_PowerStatus_IDDQDuringWaitMode) || \
    ((PowerStatus) == FLASH_PowerStatus_IDDQDuringRunMode ) || \
    ((PowerStatus) == FLASH_PowerStatus_IDDQDuringWaitAndRunModes) || \
    ((PowerStatus) == FLASH_Power_On))

/**
  * @brief Macro used by the assert function in order to check the different flags values
  */
#define IS_FLASH_FLAGS(FLAG) (((FLAG) == FLASH_FLAG_HVOFF) || \
                              ((FLAG) == FLASH_FLAG_DUL) || \
                              ((FLAG) == FLASH_FLAG_EOP) || \
                              ((FLAG) == FLASH_FLAG_PUL) || \
                              ((FLAG) == FLASH_FLAG_WR_PG_DIS))

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/* FLASH program and Data EEPROM memories interface configuration functions ***/
FLASH_ProgramTime_TypeDef FLASH_GetProgrammingTime(void);
void FLASH_SetProgrammingTime(FLASH_ProgramTime_TypeDef FLASH_ProgTime);
void FLASH_PowerWaitModeConfig(FLASH_Power_TypeDef FLASH_Power);

/* FLASH program and Data EEPROM memories Programming functions ***************/
void FLASH_DeInit(void);
void FLASH_Unlock(FLASH_MemType_TypeDef FLASH_MemType);
void FLASH_Lock(FLASH_MemType_TypeDef FLASH_MemType);
void FLASH_ProgramByte(uint32_t Address, uint8_t Data);
void FLASH_EraseByte(uint32_t Address);
void FLASH_ProgramWord(uint32_t Address, uint32_t Data);
uint8_t FLASH_ReadByte(uint32_t Address);

/* Option Bytes Programming functions *****************************************/
uint16_t FLASH_GetBootSize(void);
uint16_t FLASH_GetCodeSize(void);
FunctionalState FLASH_GetReadOutProtectionStatus(void);
void FLASH_ProgramOptionByte(uint16_t Address, uint8_t Data);
void FLASH_EraseOptionByte(uint16_t Address);

/* Interrupts and flags management functions **********************************/
void FLASH_ITConfig(FunctionalState NewState);
FlagStatus FLASH_GetFlagStatus(FLASH_FLAG_TypeDef FLASH_FLAG);

/* Functions to be executed from RAM ******************************************/

/**
@code
 All the functions declared below must be executed from RAM exclusively, except 
 for the FLASH_WaitForLastOperation function which can be executed from Flash.
 
 Steps of the execution from RAM differs from one toolchain to another.
 for more details refer to stm8l15x_flash.c file.
 
 To enable execution from RAM you can either uncomment the following define 
 in the stm8s.h file or define it in your toolchain compiler preprocessor
 - #define RAM_EXECUTION  (1) 

@endcode
*/

IN_RAM(void FLASH_PowerRunModeConfig(FLASH_Power_TypeDef FLASH_Power));
IN_RAM(FLASH_PowerStatus_TypeDef FLASH_GetPowerStatus(void));

IN_RAM(void FLASH_ProgramBlock(uint16_t BlockNum, FLASH_MemType_TypeDef FLASH_MemType,
                        FLASH_ProgramMode_TypeDef FLASH_ProgMode, uint8_t *Buffer));
IN_RAM(void FLASH_EraseBlock(uint16_t BlockNum, FLASH_MemType_TypeDef FLASH_MemType));

IN_RAM(FLASH_Status_TypeDef FLASH_WaitForLastOperation(FLASH_MemType_TypeDef FLASH_MemType));

#endif /*__STM8L15x_FLASH_H*/

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

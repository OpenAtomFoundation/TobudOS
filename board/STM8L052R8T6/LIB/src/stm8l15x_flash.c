/**
  ******************************************************************************
  * @file    stm8l15x_flash.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides all the Flash firmware functions. These functions 
  *          can be executed from Internal FLASH or Internal RAM memories.
  *            - FLASH program and Data EEPROM memories interface configuration
  *            - FLASH program and Data EEPROM memories Programming
  *            - Option Bytes Programming
  *            - Interrupts and flags management
  *            - Functions to be executed from RAM
  *               
  *  @verbatim
  *  
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *                           
  *          This driver provides functions to configure and program the Flash 
  *          memory of all STM8L15x devices
  *          These functions are split in 4 groups
  * 
  *           1. FLASH program and Data EEPROM memories interface configuration 
  *              functions: this group includes the management of the following 
  *              features:
  *                    - Set the fixed programming time
  *                    - Configure the power state during WFI mode
  *                    - Configure the power state during run, low power run and
  *                      WFE modes (should be executed from internal RAM) 
  *  
  *           2. FLASH program and Data EEPROM memories Programming functions: 
  *              this group includes all needed functions to erase and program 
  *              the FLASH program or the Data EEPROM memory.
  *                    - Lock and Unlock the Flash program interface.
  *                    - Lock and Unlock the Data EEPROM interface.  
  *                    - Erase function: Erase Byte, Erase Word and Erase Block 
  *                      (should be executed from internal RAM). 
  *                    - Program functions: Program Byte, Program Word, 
  *                      Program Block (should be executed from internal RAM) 
  *                      and Fast Program Block (should be executed from internal
  *                      RAM).
  *    
  *           3. FLASH Option Bytes Programming functions: this group includes 
  *              all needed functions to:
  *                    - Program/erase the user option Bytes 
  *                    - Get the Read Out Protection status (ROP option byte)
  *                    - Get the User Boot Code size (UBC option byte)
  *                    - Get the Protected Code size (PCODE option byte) 
  *                    
  *            Note: Option byte programming is very similar to data EEPROM byte
  *                  programming.          
  *  
  *           4. FLASH Interrupts and flag management functions: this group 
  *              includes all needed functions to:
  *                    - Enable/Disable the flash interrupt sources
  *                    - Get flags status
  *                    - Wait for last flash operation(can be executed from 
  *                      internal RAM)
  *                        
  *           5. Functions to be executed from RAM: this group includes the functions
  *              that should be executed from RAM and provides description on how 
  *              to handle this with the different supported toolchains
  *   
  *  The table below lists the functions that can be executed from RAM.
  *  
  *   +--------------------------------------------------------------------------------|
  *   |   Functions prototypes      |    RAM execution            |     Comments       |
  *   ---------------------------------------------------------------------------------|
  *   |                             | Mandatory in case of block  | Can be executed    |
  *   | FLASH_WaitForLastOperation  | Operation:                  | from Flash in case |
  *   |                             | - Block programming         | of byte and word   |
  *   |                             | - Block erase               | Operations         |
  *   |--------------------------------------------------------------------------------|
  *   | FLASH_PowerRunModeConfig    |       Exclusively           | useless from Flash |
  *   |--------------------------------------------------------------------------------|
  *   | FLASH_GetPowerStatus        |       Exclusively           | useless from Flash |
  *   |--------------------------------------------------------------------------------|
  *   | FLASH_ProgramBlock          |       Exclusively           | useless from Flash |
  *   |--------------------------------------------------------------------------------|
  *   | FLASH_EraseBlock            |       Exclusively           | useless from Flash |
  *   |--------------------------------------------------------------------------------|
  *  
  *  To be able to execute functions from RAM several steps have to be followed.
  *   These steps may differ from one toolchain to another.
  *   A detailed description is available below within this driver.
  *   You can also refer to the Flash_DataProgram example provided within the
  *   STM8L15x_StdPeriph_Lib package.
  * 
  *  @endverbatim
  *                      
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
/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_flash.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup FLASH 
  * @brief FLASH driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup FLASH_Private_Define
  * @{
  */ 
#define FLASH_CLEAR_BYTE   ((uint8_t)0x00)
#define FLASH_SET_BYTE     ((uint8_t)0xFF)
#define OPERATION_TIMEOUT  ((uint16_t)0xFFFF)
/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
 
/** @defgroup FLASH_Private_Functions
  * @{
  */ 

/** @defgroup FLASH_Group1 FLASH program and Data EEPROM memories Interface 
  *                        configuration functions
  *  @brief   FLASH Interface configuration functions 
 *
@verbatim   
 ===============================================================================
      FLASH program and Data EEPROM memories interface configuration functions
 ===============================================================================  

   The FLASH program and Data EEPROM memories interface configuration functions,
    includes the following functions:
		
   - FLASH_ProgramTime_TypeDef FLASH_GetProgrammingTime(void);
   - void FLASH_SetProgrammingTime(FLASH_ProgramTime_TypeDef FLASH_ProgTime);
	 - void FLASH_PowerWaitModeConfig(FLASH_Power_TypeDef FLASH_Power);

   These functions don't need the unlock sequence.

@endverbatim
  * @{
  */
	
/**
  * @brief  Sets the fixed programming time
  * @param  FLASH_ProgTime : Indicates the programming time to be fixed
  *          This parameter can be one of the following values:
  *            @arg FLASH_ProgramTime_Standard: Standard programming time fixed at 1/2 tprog
  *            @arg FLASH_ProgramTime_TProg: Programming time fixed at tprog 
  * @retval None
  */
void FLASH_SetProgrammingTime(FLASH_ProgramTime_TypeDef FLASH_ProgTime)
{
  /* Check parameter */
  assert_param(IS_FLASH_PROGRAM_TIME(FLASH_ProgTime));
  
  FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);
  FLASH->CR1 |= (uint8_t)FLASH_ProgTime;
}

/**
  * @brief  Returns the fixed programming time
  * @param  None
  * @retval Fixed programming time value
  */
FLASH_ProgramTime_TypeDef FLASH_GetProgrammingTime(void)
{
  return((FLASH_ProgramTime_TypeDef)(FLASH->CR1 & FLASH_CR1_FIX));
}

/**
  * @brief  Configures the power state for Flash program and data EEPROM during
  *          wait for interrupt mode
  * @param  FLASH_Power: The power state for Flash program and data EEPROM during
  *         wait for interrupt mode
  *          This parameter can be one of the following values:
  *            @arg FLASH_Power_IDDQ: Flash program and data EEPROM in IDDQ
  *            @arg FLASH_Power_On: Flash program and data EEPROM not in IDDQ 
  * @retval None
  */
void FLASH_PowerWaitModeConfig(FLASH_Power_TypeDef FLASH_Power)
{
  /* Check parameter */
  assert_param(IS_FLASH_POWER(FLASH_Power));

  /* Flash program and data EEPROM in IDDQ during wait for interrupt mode*/
  if(FLASH_Power != FLASH_Power_On)
  {
    FLASH->CR1 |= (uint8_t)FLASH_CR1_WAITM;
  }
  /* Flash program and data EEPROM not in IDDQ during wait for interrupt mode*/
  else
  {
    FLASH->CR1 &= (uint8_t)(~FLASH_CR1_WAITM);
  }
}

/**
  * @}
  */

/** @defgroup FLASH_Group2 FLASH Memory Programming functions
 *  @brief   FLASH Memory Programming functions
 *
@verbatim   
 ===============================================================================
                      FLASH Memory Programming functions
 ===============================================================================  

   The FLASH Memory Programming functions, includes the following functions:
    - void FLASH_DeInit(void);
    - void FLASH_Unlock(FLASH_MemType_TypeDef FLASH_MemType);
    - void FLASH_Lock(FLASH_MemType_TypeDef FLASH_MemType);
    - void FLASH_ProgramByte(uint32_t Address, uint8_t Data);
    - void FLASH_EraseByte(uint32_t Address);
    - void FLASH_ProgramWord(uint32_t Address, uint32_t Data);
    - uint8_t FLASH_ReadByte(uint32_t Address);
   
   Any operation of erase or program should follow these steps:

   1. Call the FLASH_Unlock(FLASH_MemType) function to enable the memory access

   2. Call the desired function to erase or program data

   3. Call the FLASH_Lock() function to disable the memory access 
     (it is recommended to protect the FLASH memory against possible unwanted operation)

@endverbatim
  * @{
  */
  
/**
  * @brief  Deinitializes the FLASH registers to their default reset values.
  * @param  None
  * @retval None
  */
void FLASH_DeInit(void)
{
  FLASH->CR1 = FLASH_CR1_RESET_VALUE;
  FLASH->CR2 = FLASH_CR2_RESET_VALUE;
  FLASH->IAPSR = FLASH_IAPSR_RESET_VALUE;
  (void) FLASH->IAPSR; /* Reading of this register causes the clearing of status flags */
}
  
/**
  * @brief  Unlocks the program or data EEPROM memory
  * @param  FLASH_MemType : Memory type to unlock
  *          This parameter can be one of the following values:
  *            @arg FLASH_MemType_Program: Program memory
  *            @arg FLASH_MemType_Data: Data EEPROM memory 
  * @retval None
  */
void FLASH_Unlock(FLASH_MemType_TypeDef FLASH_MemType)
{
  /* Check parameter */
  assert_param(IS_FLASH_MEMORY_TYPE(FLASH_MemType));
  
  /* Unlock program memory */
  if(FLASH_MemType == FLASH_MemType_Program)
  {
    FLASH->PUKR = FLASH_RASS_KEY1;
    FLASH->PUKR = FLASH_RASS_KEY2;
  }
  
  /* Unlock data memory */
  if(FLASH_MemType == FLASH_MemType_Data)
  {
    FLASH->DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
    FLASH->DUKR = FLASH_RASS_KEY1;
  }
}

/**
  * @brief  Locks the program or data EEPROM memory
  * @param  FLASH_MemType : Memory type
  *          This parameter can be one of the following values:
  *            @arg FLASH_MemType_Program: Program memory
  *            @arg FLASH_MemType_Data: Data EEPROM memory 
  * @retval None
  */
void FLASH_Lock(FLASH_MemType_TypeDef FLASH_MemType)
{
  /* Check parameter */
  assert_param(IS_FLASH_MEMORY_TYPE(FLASH_MemType));
  
  /* Lock memory */
  FLASH->IAPSR &= (uint8_t)FLASH_MemType;
}

/**
  * @brief  Programs one byte in program or data EEPROM memory
  * @param  Address : Address where the byte will be programmed
  * @param  Data : Value to be programmed
  * @retval None
  */
void FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
  /* Check parameters */
  assert_param(IS_FLASH_ADDRESS(Address));
  
  *(PointerAttr uint8_t*) (MemoryAddressCast)Address = Data;
}

/**
  * @brief  Erases one byte in the program or data EEPROM memory
  * @param  Address : Address of the byte to erase
  * @retval None
  */
void FLASH_EraseByte(uint32_t Address)
{
  /* Check parameter */
  assert_param(IS_FLASH_ADDRESS(Address));
  
  *(PointerAttr uint8_t*) (MemoryAddressCast)Address = FLASH_CLEAR_BYTE; /* Erase byte */
}

/**
  * @brief  Programs one word (4 bytes) in program or data EEPROM memory
  * @param  Address : The address where the data will be programmed
  * @param  Data : Value to be programmed
  * @retval None
  */
void FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  /* Check parameters */
  assert_param(IS_FLASH_ADDRESS(Address));
  /* Enable Word Write Once */
  FLASH->CR2 |= FLASH_CR2_WPRG;
  
  /* Write one byte - from lowest address*/
  *((PointerAttr uint8_t*)(MemoryAddressCast)Address)       = *((uint8_t*)(&Data));   
  /* Write one byte*/
  *(((PointerAttr uint8_t*)(MemoryAddressCast)Address) + 1) = *((uint8_t*)(&Data) + 1);
  /* Write one byte*/
  *(((PointerAttr uint8_t*)(MemoryAddressCast)Address) + 2) = *((uint8_t*)(&Data) + 2); 
  /* Write one byte - from higher address*/
  *(((PointerAttr uint8_t*)(MemoryAddressCast)Address) + 3) = *((uint8_t*)(&Data) + 3); 
}

/**
  * @brief  Reads one byte from flash memory
  * @param  Address : Address to read
  * @retval Value of the byte
  */
uint8_t FLASH_ReadByte(uint32_t Address)
{
  /* Read byte */
  return(*(PointerAttr uint8_t *) (MemoryAddressCast)Address);
}
/**
  * @}
  */

/** @defgroup FLASH_Group3 Option Bytes Programming functions
 *  @brief   Option Bytes Programming functions 
 *
@verbatim   
 ===============================================================================
                        Option Bytes Programming functions
 ===============================================================================  

   The FLASH_Option Bytes Programming_functions, includes the following functions:

   - void FLASH_ProgramOptionByte(uint16_t Address, uint8_t Data);
   - void FLASH_EraseOptionByte(uint16_t Address);
   - FunctionalState FLASH_GetReadOutProtectionStatus(void);
   - uint16_t FLASH_GetBootSize(void);
   - uint16_t FLASH_GetCodeSize(void);
   
   Any operation of erase or program should follow these steps:
   
   1. Call the FLASH_Unlock(FLASH_MemType_Data); function to enable the Flash 
      option control register access
   
   2. Call the desired function to erase or program data
      - void FLASH_ProgramOptionByte(uint16_t Address, uint8_t Data); => to program
        the option byte Address with the desired Data value.  
      - void FLASH_EraseOptionByte(uint16_t Address); => to erase the option byte
        Address. 			 
   
   3. Once all needed option bytes to be programmed are correctly written, call the
      FLASH_Lock(FLASH_MemType_Data) to disable the memory access ( It is recommended to
      protect the FLASH memory against possible unwanted operation)

@endverbatim
  * @{
  */
  
/**
  * @brief  Programs option byte
  * @param  Address : option byte address to program
  * @param  Data : Value to write
  * @retval None
  */
void FLASH_ProgramOptionByte(uint16_t Address, uint8_t Data)
{
  /* Check parameter */
  assert_param(IS_OPTION_BYTE_ADDRESS(Address));

  /* Enable write access to option bytes */
  FLASH->CR2 |= FLASH_CR2_OPT;

  /* Program option byte and his complement */
  *((PointerAttr uint8_t*)Address) = Data;

  FLASH_WaitForLastOperation(FLASH_MemType_Program);

  /* Disable write access to option bytes */
  FLASH->CR2 &= (uint8_t)(~FLASH_CR2_OPT);
}

/**
  * @brief  Erases option byte
  * @param  Address : Option byte address to erase
  * @retval None
  */
void FLASH_EraseOptionByte(uint16_t Address)
{
  /* Check parameter */
  assert_param(IS_OPTION_BYTE_ADDRESS(Address));

  /* Enable write access to option bytes */
  FLASH->CR2 |= FLASH_CR2_OPT;

  /* Erase option byte and his complement */
  *((PointerAttr uint8_t*)Address) = FLASH_CLEAR_BYTE;

  FLASH_WaitForLastOperation(FLASH_MemType_Program);

  /* Disable write access to option bytes */
  FLASH->CR2 &= (uint8_t)(~FLASH_CR2_OPT);
}

/**
  * @brief  Returns the FLASH Read Out Protection Status.
  * @param  None
  * @retval FLASH Read Out Protection Status.
  *         This parameter can be a ENABLE or DISABLE
  */
FunctionalState FLASH_GetReadOutProtectionStatus(void)
{
  FunctionalState state = DISABLE;
  
  if(OPT->ROP == FLASH_READOUTPROTECTION_KEY)
  {
    /* The status of the Flash read out protection is enabled*/
    state =  ENABLE;
  }
  else
  {
    /* The status of the Flash read out protection is disabled*/
    state =  DISABLE;
  }
  
  return state;
}

/**
  * @brief  Returns the Boot memory size in bytes
  * @param  None
  * @retval Boot memory size in bytes
  */
uint16_t FLASH_GetBootSize(void)
{
  uint16_t temp = 0;
  
  /* Calculates the number of bytes */
  temp = (uint16_t)((uint16_t)OPT->UBC * (uint16_t)128);
  
  /* Correction because size upper 8kb doesn't exist */
  if(OPT->UBC > 0x7F)
  {
    temp = 8192;
  }
  
  /* Return value */
  return(temp);
}

/**
 *
  * @brief  Returns the Code Area size in bytes
  * @param  None
  * @retval Code Area size in bytes
  */
uint16_t FLASH_GetCodeSize(void)
{
  uint16_t temp = 0;
  
  /* Calculates the number of bytes */
  temp = (uint16_t)((uint16_t)OPT->PCODESIZE * (uint16_t)128);
  
  /* Correction because size upper of 8kb doesn't exist */
  if(OPT->PCODESIZE > 0x7F)
  {
    temp = 8192;
  }
  
  /* Return value */
  return(temp);
}

/**
  * @}
  */

/** @defgroup FLASH_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
                  Interrupts and flags management functions
 ===============================================================================  

@endverbatim
  * @{
  */
  
/**
  * @brief  Enables or Disables the Flash interrupt mode
  * @param  NewState : The new state of the flash interrupt mode
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FLASH_ITConfig(FunctionalState NewState)
{
  /* Check parameter */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if(NewState != DISABLE)
  {
    /* Enables the interrupt sources */
    FLASH->CR1 |= FLASH_CR1_IE;
  }
  else
  {
    /* Disables the interrupt sources */
    FLASH->CR1 &= (uint8_t)(~FLASH_CR1_IE);
  }
}

/**
  * @brief  Checks whether the specified FLASH flag is set or not.
  * @param  FLASH_FLAG : specifies the Flash Flag to check.
  *          This parameter can be one of the following values:
  *            @arg FLASH_FLAG_HVOFF: End of high voltage
  *            @arg FLASH_FLAG_DUL: Data EEPROM unlocked
  *            @arg FLASH_FLAG_EOP: End of programming (write or erase operation)
  *            @arg FLASH_FLAG_PUL: Flash Program memory unlocked
  *            @arg FLASH_FLAG_WR_PG_DIS: Write attempted to protected page       
  * @retval Indicates the state of the Flash_FLAG.
  *         This parameter can be SET or RESET
  */
FlagStatus FLASH_GetFlagStatus(FLASH_FLAG_TypeDef FLASH_FLAG)
{
  FlagStatus status = RESET;
  assert_param(IS_FLASH_FLAGS(FLASH_FLAG));
  
  /* Check the status of the specified flash flag*/
  if((FLASH->IAPSR  & (uint8_t)FLASH_FLAG) != (uint8_t)RESET)
  {
    status = SET; /* Flash_FLAG is set*/
  }
  else
  {
    status = RESET; /* Flash_FLAG is reset*/
  }
  
  /* Return the Flash_FLAG status*/
  return status;
}

/**
  * @}
  */ 
  
/** @defgroup FLASH_Group5 Functions to be executed from RAM
 *  @brief  Functions to be executed from RAM
 *
@verbatim   
 ===============================================================================
                         Functions to be executed from RAM
 ===============================================================================  
 
 All the functions defined below must be executed from RAM exclusively, except
 for the FLASH_WaitForLastOperation function which can be executed from Flash.

 Steps of the execution from RAM differs from one toolchain to another:
 - For Cosmic Compiler:
    1- Define a segment FLASH_CODE by the mean of " #pragma section (FLASH_CODE)".
    This segment is defined in the stm8l15x_flash.c file.
  2- Uncomment the "#define RAM_EXECUTION  (1)" line in the stm8l15x.h file,
    or define it in Cosmic compiler preprocessor to enable the FLASH_CODE segment
   definition.
  3- In STVD Select Project\Settings\Linker\Category "input" and in the RAM section
    add the FLASH_CODE segment with "-ic" options.
  4- In main.c file call the _fctcpy() function with first segment character as 
    parameter "_fctcpy('F');" to load the declared moveable code segment
    (FLASH_CODE) in RAM before execution.
  5- By default the _fctcpy function is packaged in the Cosmic machine library,
    so the function prototype "int _fctcopy(char name);" must be added in main.c
    file.

  - For Raisonance Compiler
   1- Use the inram keyword in the function declaration to specify that it can be
    executed from RAM.
    This is done within the stm8l15x_flash.c file, and it's conditioned by 
    RAM_EXECUTION definition.
   2- Uncomment the "#define RAM_EXECUTION  (1)" line in the stm8l15x.h file, or 
   define it in Raisonance compiler preprocessor to enable the access for the 
   inram functions.
   3- An inram function code is copied from Flash to RAM by the C startup code. 
   In some applications, the RAM area where the code was initially stored may be
   erased or corrupted, so it may be desirable to perform the copy again. 
   Depending on the application memory model, the memcpy() or fmemcpy() functions
   should be used to perform the copy.
      • In case your project uses the SMALL memory model (code smaller than 64K),
       memcpy()function is recommended to perform the copy
      • In case your project uses the LARGE memory model, functions can be 
      everywhere in the 24-bits address space (not limited to the first 64KB of
      code), In this case, the use of memcpy() function will not be appropriate,
      you need to use the specific fmemcpy() function (which copies objects with
      24-bit addresses).
      - The linker automatically defines 2 symbols for each inram function:
           • __address__functionname is a symbol that holds the Flash address 
           where the given function code is stored.
           • __size__functionname is a symbol that holds the function size in bytes.
     And we already have the function address (which is itself a pointer)
  4- In main.c file these two steps should be performed for each inram function:
     • Import the "__address__functionname" and "__size__functionname" symbols
       as global variables:
         extern int __address__functionname; // Symbol holding the flash address
         extern int __size__functionname;    // Symbol holding the function size
     • In case of SMALL memory model use, Call the memcpy() function to copy the
      inram function to the RAM destination address:
                memcpy(functionname, // RAM destination address
                      (void*)&__address__functionname, // Flash source address
                      (int)&__size__functionname); // Code size of the function
     • In case of LARGE memory model use, call the fmemcpy() function to copy 
     the inram function to the RAM destination address:
                 memcpy(functionname, // RAM destination address
                      (void @far*)&__address__functionname, // Flash source address
                      (int)&__size__functionname); // Code size of the function

 - For IAR Compiler:
    1- Use the __ramfunc keyword in the function declaration to specify that it 
    can be executed from RAM.
    This is done within the stm8l15x_flash.c file, and it's conditioned by 
    RAM_EXECUTION definition.
    2- Uncomment the "#define RAM_EXECUTION  (1)" line in the stm8l15x.h file, or 
   define it in IAR compiler preprocessor to enable the access for the 
   __ramfunc functions.
 
 - Note: 
    1- Ignore the IAR compiler warnings, these warnings don't impact the FLASH Program/Erase
    operations.
    The code performing the Flash Program/erase must be executed from RAM; the variables
    initializations don't necessary require the execution from RAM, only CR2 registers 
    configuration and data programing must be executed from RAM.
    2- These warnings depends on IAR compiler: as the code generation is made using many
    runtime library functions to keep code size to a minimum.
    3- It is recommended to use High Speed Optimization with IAR (-Ohs), in order 
    to reduce the runtime library calls in the generated code.

 The Flash_DataProgram example provided within the STM8L15x_StdPeriph_Lib package
 details all the steps described above.

@endverbatim
  * @{
  */
   
/**
  * @brief
  *******************************************************************************
  *                         Execution from RAM enable
  *******************************************************************************
  *
  * To enable execution from RAM you can either uncomment the following define 
  * in the stm8l15x.h file or define it in your toolchain compiler preprocessor
  * - #define RAM_EXECUTION  (1) 
  */

#if defined (_COSMIC_) && defined (RAM_EXECUTION)
 #pragma section (FLASH_CODE)
#endif  /* _COSMIC_ && RAM_EXECUTION */

/**
  * @brief  Configures the power state for Flash program and data EEPROM during
  *         run, low power run and low power wait modes
  * @note   This function must be called and executed from RAM.
  * @param  FLASH_Power: power state of the Flash program and data EEPROM
  *          This parameter can be one of the following values:
  *            @arg FLASH_Power_IDDQ: Flash program and data EEPROM in IDDQ
  *            @arg FLASH_Power_On: Flash program and data EEPROM not in IDDQ 
  * @retval None
  */
IN_RAM(void FLASH_PowerRunModeConfig(FLASH_Power_TypeDef FLASH_Power))
{
  /* Check parameter */
  assert_param(IS_FLASH_POWER(FLASH_Power));
  
  if(FLASH_Power != FLASH_Power_On)
  {
    FLASH->CR1 |= (uint8_t)FLASH_CR1_EEPM;
  }
  else
  {
    FLASH->CR1 &= (uint8_t)(~FLASH_CR1_EEPM);
  }
}

/**
  * @brief  Checks the power status for Flash program and data EEPROM
  * @note   This function should be called and executed from RAM.
  * @param  None
  * @retval Flash program and data EEPROM power status
  *         The returned value can be one of the following:
  *         - FLASH_PowerStatus_IDDQDuringWaitMode: IDDQ during Wait mode
  *         - FLASH_PowerStatus_IDDQDuringRunMode: IDDQ during Run mode
  *         - FLASH_PowerStatus_IDDQDuringWaitAndRunModes: IDDQ during Wait/Run mode
  *         - FLASH_PowerStatus_On: Powered on during Wait and Run modes
  */
IN_RAM(FLASH_PowerStatus_TypeDef FLASH_GetPowerStatus(void))
{
  return((FLASH_PowerStatus_TypeDef)(FLASH->CR1 & (uint8_t)0x0C));
}

/**
  * @brief  Programs a memory block
  * @note   This function should be called and executed from RAM.
  * @param  FLASH_MemType : The type of memory to program
  *          This parameter can be one of the following values:
  *            @arg FLASH_MemType_Program: Program memory
  *            @arg FLASH_MemType_Data: Data EEPROM memory 
  * @param  BlockNum : The block number
  * @param  FLASH_ProgMode : The programming mode.
  *          This parameter can be one of the following values:
  *            @arg FLASH_ProgramMode_Standard: Standard programming mode
  *            @arg FLASH_ProgramMode_Fast: Fast programming mode
  * @param  Buffer : Pointer to buffer containing source data.
  * @retval None.
  */
IN_RAM(void FLASH_ProgramBlock(uint16_t BlockNum, FLASH_MemType_TypeDef FLASH_MemType,
                        FLASH_ProgramMode_TypeDef FLASH_ProgMode, uint8_t *Buffer))
{
  uint16_t Count = 0;
  uint32_t startaddress = 0;
  
  /* Check parameters */
  assert_param(IS_FLASH_MEMORY_TYPE(FLASH_MemType));
  assert_param(IS_FLASH_PROGRAM_MODE(FLASH_ProgMode));
  if(FLASH_MemType == FLASH_MemType_Program)
  {
    assert_param(IS_FLASH_PROGRAM_BLOCK_NUMBER(BlockNum));
    startaddress = FLASH_PROGRAM_START_PHYSICAL_ADDRESS;
  }
  else
  {
    assert_param(IS_FLASH_DATA_EEPROM_BLOCK_NUMBER(BlockNum));
    startaddress = FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS;
  }
  
  /* Point to the first block address */
  startaddress = startaddress + ((uint32_t)BlockNum * FLASH_BLOCK_SIZE);
  
  /* Selection of Standard or Fast programming mode */
  if(FLASH_ProgMode == FLASH_ProgramMode_Standard)
  {
    /* Standard programming mode */
    FLASH->CR2 |= FLASH_CR2_PRG;
  }
  else
  {
    /* Fast programming mode */
    FLASH->CR2 |= FLASH_CR2_FPRG;
  }
  
  /* Copy data bytes from RAM to FLASH memory */
  for(Count = 0; Count < FLASH_BLOCK_SIZE; Count++)
  {
    *((PointerAttr uint8_t*) (MemoryAddressCast)startaddress + Count) = ((uint8_t)(Buffer[Count]));
  }
}

/**
  * @brief  Erases a block in the program or data memory.
  * @note   This function should be called and executed from RAM.
  * @param  BlockNum : Indicates the block number to erase
  * @param  FLASH_MemType :  The type of memory to erase
  *          This parameter can be one of the following values:
  *            @arg FLASH_MemType_Program: Program memory
  *            @arg FLASH_MemType_Data: Data EEPROM memory 
  * @retval None.
  */
IN_RAM(void FLASH_EraseBlock(uint16_t BlockNum, FLASH_MemType_TypeDef FLASH_MemType))
{
  uint32_t startaddress = 0;
#if defined (STM8L15X_MD) || defined (STM8L15X_MDP) || defined (STM8L15X_LD) || \
  defined (STM8L05X_LD_VL) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
    uint32_t PointerAttr  *pwFlash;
  
#elif defined (STM8L15X_HD) || defined (STM8L05X_HD_VL)
  uint8_t PointerAttr  *pwFlash;
#endif
  
  /* Check parameters */
  assert_param(IS_FLASH_MEMORY_TYPE(FLASH_MemType));
  if(FLASH_MemType == FLASH_MemType_Program)
  {
    assert_param(IS_FLASH_PROGRAM_BLOCK_NUMBER(BlockNum));
    startaddress = FLASH_PROGRAM_START_PHYSICAL_ADDRESS;
  }
  else
  {
    assert_param(IS_FLASH_DATA_EEPROM_BLOCK_NUMBER(BlockNum));
    startaddress = FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS;
  }
  
  /* Point to the first block address */
#if defined (STM8L15X_MD) || defined (STM8L15X_MDP)|| defined (STM8L15X_LD) || \
  defined (STM8L05X_LD_VL) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
    pwFlash = (PointerAttr uint32_t *)(uint16_t)(startaddress + ((uint32_t)BlockNum * FLASH_BLOCK_SIZE));
  
#elif defined (STM8L15X_HD) || defined (STM8L05X_HD_VL)
  pwFlash = (PointerAttr uint8_t *)(uint32_t)(startaddress + ((uint32_t)BlockNum * FLASH_BLOCK_SIZE));
#endif
  
  /* Enable erase block mode */
  FLASH->CR2 |= FLASH_CR2_ERASE;
  
#if defined (STM8L15X_MD) || defined (STM8L15X_MDP) || defined (STM8L15X_LD) || \
  defined (STM8L05X_LD_VL) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
    *pwFlash = (uint32_t)0;  
  
#elif defined (STM8L15X_HD) || defined (STM8L05X_HD_VL)
  *pwFlash = (uint8_t)0;
  *(pwFlash + 1) = (uint8_t)0;
  *(pwFlash + 2) = (uint8_t)0;
  *(pwFlash + 3) = (uint8_t)0;
#endif
}

/**
  * @brief  Waits for a Flash operation to complete.
  * @note   The call and execution of this function must be done from RAM in case
  *         of Block operation, otherwise it can be executed from Flash
  * @param  FLASH_MemType : Memory type
  *          This parameter can be one of the following values:
  *            @arg FLASH_MemType_Program: Program memory
  *            @arg FLASH_MemType_Data: Data EEPROM memory 
  * @retval FLASH status
  */
IN_RAM(FLASH_Status_TypeDef FLASH_WaitForLastOperation(FLASH_MemType_TypeDef FLASH_MemType))
{
  uint16_t timeout = OPERATION_TIMEOUT;
  uint8_t flagstatus = 0x00;
  
  /* Wait until operation completion or write protected page occurred */
  if(FLASH_MemType == FLASH_MemType_Program)
  {
    while((flagstatus == 0x00) && (timeout != 0x00))
    {
      flagstatus = (uint8_t)(FLASH->IAPSR & (uint8_t)(FLASH_IAPSR_EOP |
                                                      FLASH_IAPSR_WR_PG_DIS));
      timeout--;
    }
  }
  else
  {
    while((flagstatus == 0x00) && (timeout != 0x00))
    {
      flagstatus = (uint8_t)(FLASH->IAPSR & (uint8_t)(FLASH_IAPSR_HVOFF |
                                                      FLASH_IAPSR_WR_PG_DIS));
      timeout--;
    }
  }
  
  if(timeout == 0x00)
  {
    flagstatus = FLASH_Status_TimeOut;
  }
  
  return((FLASH_Status_TypeDef)flagstatus);
}

#if defined (_COSMIC_) && defined (RAM_EXECUTION)
 /* End of FLASH_CODE section */
 #pragma section ()
#endif /* _COSMIC_ && RAM_EXECUTION */

/**
  * @}
  */

/**
  * @}
  */
   
  /**
  * @}
  */ 
  
  /**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
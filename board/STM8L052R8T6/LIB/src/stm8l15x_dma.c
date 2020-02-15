/**
  ******************************************************************************
  * @file    stm8l15x_dma.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides all the DMA firmware functions.
 * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Direct Memory Access controller (DMA):           
  *           - Initialization and Configuration
  *           - Data Counter
  *           - Interrupts and flags management
  *           
  *  @verbatim
  *      
  *          ===================================================================      
  *                                 How to use this driver
  *          =================================================================== 
  *          1. Enable The DMA controller clock using CLK_PeripheralClockConfig() 
  *            function: CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE).
  *
  *          2. Enable and configure the peripheral to be connected to the DMA 
  *            channel (except for internal SRAM / FLASH memories: no 
  *            initialization is necessary). 
  *        
  *          3. For a given Channel, program the Source and Destination 
  *             addresses, the transfer Direction, the Buffer Size, the 
  *             Peripheral and Memory Incrementation mode and Data Size, 
  *             the Circular or Normal mode, the channel transfer Priority 
  *             and the Memory-to-Memory transfer mode (for channel 3 only, 
  *             if needed) using the DMA_Init() function.
  *
  *          4. Enable the corresponding interrupt(s) using the function 
  *             DMA_ITConfig() if you need to use DMA interrupts. 
  *
  *          5. Enable the DMA channel using the DMA_Cmd() function. 
  *                
  *          6. Activate the needed channel Request using PPP_DMACmd() function 
  *            for any PPP peripheral except internal SRAM and FLASH (ie. TIM4, 
  *             USART ...). The function allowing this operation is provided in 
  *             each PPP peripheral driver (ie. TIM4_DMACmd for TIM4 peripheral).     
  *
  *          7. Optionally, you can configure the number of data to be 
  *             transferred when the channel is disabled (ie. after each 
  *             Transfer Complete event or when a Transfer Error occurs) using 
  *             the function DMA_SetCurrDataCounter().
  *             And you can get the number of remaining data to be transferred 
  *             using the function DMA_GetCurrDataCounter() at run time (when 
  *             the DMA channel is enabled and running).  
  *                   
  *          8. To control DMA events you can use one of the following 
  *              two methods:
  *               a- Check on DMA channel flags using the function 
  *                  DMA_GetFlagStatus().  
  *               b- Use DMA interrupts through the function DMA_ITConfig() 
  *                   at initialization phase and DMA_GetITStatus() function 
  *                   into interrupt routines in communication phase.  
  *              After checking on a flag you should clear it using 
  *              DMA_ClearFlag() function. And after checking on an interrupt 
  *              event you should clear it using DMA_ClearITPendingBit() 
  *              function.     
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
#include "stm8l15x_dma.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup DMA 
  * @brief  DMA driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DMA_Private_Functions
  * @{
  */ 

/** @defgroup DMA_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions
 *
@verbatim   
 ===============================================================================
                 Initialization and Configuration functions
 ===============================================================================  

  This subsection provides functions allowing to initialize the DMA channel 
  source and destination addresses, incrementation and data sizes, transfer 
  direction, buffer size, circular/normal mode selection, memory-to-memory mode 
  selection and channel priority value.
  
  - The DMA_Init() function follows the DMA configuration procedures.
  - All DMA channels can be enabled and disabled in the same time using 
    DMA_GlobalCmd() function.
  - The DMA has 4 channels, User can enable or disable channels using 
    DMA_Cmd() function.
  - The timeout duration (number of wait cycles starting from the latest 
    request) is configured using DMA_SetTimeOut() function. The DMA then waits 
    until this timeout has elapsed before requesting from the core a high 
    priority access to the bus. 

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the DMA Global Status register to its default reset
  *         values.
  * @param  None
  * @retval None
  */
void DMA_GlobalDeInit(void)
{
  /* Disable the  DMA    */
  DMA1->GCSR &= (uint8_t)~(DMA_GCSR_GE);

  /* Reset DMA Channelx control register */
  DMA1->GCSR  = (uint8_t)DMA_GCSR_RESET_VALUE;
}

/**
  * @brief  Deinitializes the DMA Channelx registers to their default reset 
  *         values.
  * @param  DMA_Channelx: selects the DMA Channelx where x can be 0 to 3
  * @retval None
  */
void DMA_DeInit(DMA_Channel_TypeDef* DMA_Channelx)
{
  /* Check the parameters */
  assert_param(IS_DMA_CHANNEL(DMA_Channelx));

  /* Disable the selected DMA Channelx */
  DMA_Channelx->CCR &= (uint8_t)~(DMA_CCR_CE);

  /* Reset DMA Channelx control register */
  DMA_Channelx->CCR  = DMA_CCR_RESET_VALUE;

  /* Reset DMA Channelx remaining bytes register */
  DMA_Channelx->CNBTR = DMA_CNBTR_RESET_VALUE;

  /* Reset DMA Channelx peripheral address register */
  if (DMA_Channelx == DMA1_Channel3)
  {
    DMA_Channelx->CPARH  = DMA_C3PARH_RESET_VALUE;
    DMA_Channelx->CM0EAR = DMA_C3M0EAR_RESET_VALUE;
  }
  else
  {
    DMA_Channelx->CPARH  = DMA_CPARH_RESET_VALUE;
  }
  DMA_Channelx->CPARL  = DMA_CPARL_RESET_VALUE;

  /* Reset DMA Channelx memory address register */
  DMA_Channelx->CM0ARH = DMA_CM0ARH_RESET_VALUE;
  DMA_Channelx->CM0ARL = DMA_CM0ARL_RESET_VALUE;

  /* Reset interrupt pending bits for DMA Channel */
  DMA_Channelx->CSPR = DMA_CSPR_RESET_VALUE;
}


/**
  * @brief  Initializes the DMA Channelx according to the specified parameters.
  * @param  DMA_Channelx: selects the DMA Channelx where x can be 0 to 3
  * @param  DMA_Memory0BaseAddr: Specifies  Memory 0 Base Address
  * @param  DMA_PeripheralMemory1BaseAddr: Specifies DMA channelx Peripheral 
  *         Base Address (if data is from/to  peripheral) or DMA channelx 
  *         Memory1 Base Address (if data is from Memory0 to Memory1).
  * @param  DMA_BufferSize: Specifies the size of the DMA channelx Buffer.
  *         This parameter must be a value greater than 0.
  * @param  DMA_DIR: Specifies the DMA Channelx transfer direction.
  *          This parameter can be one of the following values:
  *            @arg DMA_DIR_PeripheralToMemory: Data transfer direction is Peripheral To Memory
  *            @arg DMA_DIR_MemoryToPeripheral: Data transfer direction is Memory To Peripheral
  *            @arg DMA_DIR_Memory0ToMemory1: Data transfer direction is Memory0 To Memory 1    
  * @param  DMA_Mode: Specifies the DMA channelx mode.
  *          This parameter can be one of the following values:
  *            @arg DMA_Mode_Normal: DMA normal buffer mode
  *            @arg DMA_Mode_Circular: DMA circular buffer mode
  * @param  DMA_MemoryIncMode: Specifies the DMA channelx memory Incremental/Decremental mode 
  *          This parameter can be one of the following values:
  *            @arg DMA_MemoryIncMode_Dec: DMA memory incremented mode is decremental
  *            @arg DMA_MemoryIncMode_Inc: DMA memory incremented mode is incremental  
  * @param  DMA_Priority: Specifies the DMA channelx priority.
  *          This parameter can be one of the following values:
  *            @arg DMA_Priority_Low: Software Priority is Low
  *            @arg DMA_Priority_Medium: Software Priority is Medium
  *            @arg DMA_Priority_High: Software Priority is High
  *            @arg DMA_Priority_VeryHigh: Software Priority is Very High
  * @param  DMA_MemoryDataSize: Specifies the DMA channelx transfer Data size
  *          This parameter can be one of the following values:
  *            @arg DMA_MemoryDataSize_Byte: Memory Data Size is 1 Byte
  *            @arg DMA_MemoryDataSize_HalfWord: Memory Data Size is 2 Bytes  
  * @retval None
  */
void DMA_Init(DMA_Channel_TypeDef* DMA_Channelx,
              uint32_t DMA_Memory0BaseAddr,
              uint16_t DMA_PeripheralMemory1BaseAddr,
              uint8_t DMA_BufferSize,
              DMA_DIR_TypeDef DMA_DIR,
              DMA_Mode_TypeDef DMA_Mode,
              DMA_MemoryIncMode_TypeDef DMA_MemoryIncMode,
              DMA_Priority_TypeDef DMA_Priority,
              DMA_MemoryDataSize_TypeDef DMA_MemoryDataSize )
{
  /* Check the parameters */
  assert_param(IS_DMA_CHANNEL(DMA_Channelx));
  assert_param(IS_DMA_DIR(DMA_DIR));
  assert_param(IS_DMA_BUFFER_SIZE(DMA_BufferSize));
  assert_param(IS_DMA_MODE(DMA_Mode));
  assert_param(IS_DMA_MEMORY_INC_MODE(DMA_MemoryIncMode));
  assert_param(IS_DMA_PRIORITY(DMA_Priority));

  /*--------------------------- DMA Channelx CCR Configuration ---------------*/
  /* Disable the selected DMA Channelx */
  DMA_Channelx->CCR &= (uint8_t)~(DMA_CCR_CE);

  /* Reset DMA Channelx control register */
  DMA_Channelx->CCR  = DMA_CCR_RESET_VALUE;

  /* Set DMA direction & Mode & Incremental Memory mode */
  DMA_Channelx->CCR |= (uint8_t)((uint8_t)((uint8_t)DMA_DIR | \
                                           (uint8_t)DMA_Mode) | \
                                           (uint8_t)DMA_MemoryIncMode);

  /*Clear old priority and memory data size  option */
  DMA_Channelx->CSPR &= (uint8_t)(~(uint8_t)(DMA_CSPR_PL | DMA_CSPR_16BM));

  /* Set old priority and memory data size  option */
  DMA_Channelx->CSPR |= (uint8_t)((uint8_t)DMA_Priority | \
                                  (uint8_t)DMA_MemoryDataSize);

  /*--------------------------- DMA Channelx CNDTR Configuration -------------*/
  /* Write to DMA Channelx CNDTR */
  DMA_Channelx->CNBTR = (uint8_t)DMA_BufferSize;

  /*--------------------------- DMA Channelx CPAR Configuration --------------*/
  /* Write to DMA Channelx (0, 1 or 2)  Peripheral address  or  Write to 
  DMA Channel 3 Memory 1 address  */
  DMA_Channelx->CPARH = (uint8_t)(DMA_PeripheralMemory1BaseAddr >> (uint8_t)8);
  DMA_Channelx->CPARL = (uint8_t)(DMA_PeripheralMemory1BaseAddr);

  /*--------------------------- DMA Channelx CMAR Configuration --------------*/
  /* Write to DMA Channelx Memory address */
  if (DMA_Channelx == DMA1_Channel3)
  {
    DMA_Channelx->CM0EAR = (uint8_t)(DMA_Memory0BaseAddr >> (uint8_t)16);
  }
  DMA_Channelx->CM0ARH = (uint8_t)(DMA_Memory0BaseAddr >> (uint8_t)8);
  DMA_Channelx->CM0ARL = (uint8_t)(DMA_Memory0BaseAddr);

}

/**
  * @brief  Enables or disables All the DMA.
  * @param  NewState: new state of the DMA. This parameter can be: ENABLE 
  *                   or DISABLE.
  * @retval None
  */
void DMA_GlobalCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the  DMA      */
    DMA1->GCSR |= (uint8_t)DMA_GCSR_GE;
  }
  else
  {
    /* Disable the DMA */
    DMA1->GCSR &= (uint8_t)(~DMA_GCSR_GE);
  }
}

/**
  * @brief  Enables or disables the specified DMA Channelx.
  * @note   DMA_GlobalCmd function must be called first to enable or disable
  *         the global DMA.
  * @param  DMA_Channelx: selects the DMA Channelx where x can be 0 to 3
  * @param  NewState: new state of the DMA Channelx.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DMA_Cmd(DMA_Channel_TypeDef* DMA_Channelx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DMA_CHANNEL(DMA_Channelx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected DMA Channelx */
    DMA_Channelx->CCR |= DMA_CCR_CE;
  }
  else
  {
    /* Disable the selected DMA Channelx */
    DMA_Channelx->CCR &= (uint8_t)(~DMA_CCR_CE);
  }
}

/**
  * @brief  Sets the Time out Value.
  * @param  DMA_TimeOut: an integer from 0 to 63
  * @note   If timeout duration >0 (number of wait cycles starting from the
  *         latest request), the DMA waits until this timeout has elapsed before
  *         requesting from the core a high priority access to the bus.
  * @note   If timeout duration =0, there is no timeout and once a request is served,
  *         the DMA immediately asks to the CPU a high priority access to the bus.
  * @retval None
  */
void DMA_SetTimeOut(uint8_t DMA_TimeOut)
{
  /* Check the parameters */
  assert_param(IS_DMA_TIMEOUT(DMA_TimeOut));

  /* set the time out, GB and GE must be = 0 */
  DMA1->GCSR = 0;
  DMA1->GCSR = (uint8_t)(DMA_TimeOut << (uint8_t)2);

}

/**
  * @}
  */

/** @defgroup DMA_Group2 Data Counter functions
 *  @brief   Data Counter functions 
 *
@verbatim   
 ===============================================================================
                           Data Counter functions
 ===============================================================================  

  This subsection provides functions allowing to configure and read the buffer 
  size (number of data to be transferred). 

  The DMA data counter can be written only when the DMA channel is disabled 
  (ie. after transfer complete event).

  The DMA_SetCurrDataCounter() function can be used to write the Channel data 
  counter value.
  

  Note: It is advised to use this function rather than DMA_Init() (DMA_BufferSize 
        parameter) in situations where only the Data buffer needs to be reloaded.

  The DMA data counter can be read to indicate the number of remaining transfers 
  for the relative DMA channel. This counter is decremented at the end of each 
  data transfer and when the transfer is complete: 
   - If Normal mode is selected: the counter is set to 0.
   - If Circular mode is selected: the counter is reloaded with the initial value
     (configured before enabling the DMA channel)
   
  The DMA_GetCurrDataCounter() function can be used to read the Channel current 
  data counter value.
     
@endverbatim
  * @{
  */
  
/**
  * @brief  Set the number of data units to transfer for DMA Channelx.
  * @param  DMA_Channelx: selects the DMA Channelx where x can be 0 to 3
  * @param  DataNumber: The number of  data units to transfer, it can be any value
  *         from 0 to 255
  * @note   It is advised to use this function rather than DMA_Init() in situations
  *         where only the Data buffer needs to be reloaded.  
  * @retval None
  */
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMA_Channelx, uint8_t DataNumber)
{
  /* Check the parameters */
  assert_param(IS_DMA_CHANNEL(DMA_Channelx));

  /*Set the number of data units for DMA Channelx */
  DMA_Channelx->CNBTR = DataNumber;
}

/**
  * @brief  Returns the number of remaining data units in the current DMA Channelx transfer.
  * @param  DMA_Channelx: selects the DMA Channelx where x can be 0 to 3
  * @retval The number of remaining data units in the current DMA Channelx
  */
uint8_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMA_Channelx)
{
  /* Check the parameters */
  assert_param(IS_DMA_CHANNEL(DMA_Channelx));

  /* Return the number of remaining data units for DMA Channelx */
  return ((uint8_t)(DMA_Channelx->CNBTR));
}

/**
  * @}
  */

/** @defgroup DMA_Group3 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
                  Interrupts and flags management functions
 ===============================================================================  

  This subsection provides functions allowing to configure the DMA Interrupts 
  sources and check or clear the flags or pending bits status.
  The user should identify which mode will be used in his application to manage 
  the DMA controller events: Polling mode or Interrupt mode. 
    
  Polling Mode
  =============
    Each DMA channel can be managed through 2 event Flags:
    (x: DMA channel number )
       1. DMA1_FLAG_TCx: to indicate that a Transfer Complete event occurred
       2. DMA1_FLAG_HTx: to indicate that a Half-Transfer Complete event 
                          occurred

   In this Mode it is advised to use DMA_GetFlagStatus() and  DMA_ClearFlag() 
   functions. 
      

  Interrupt Mode
  ===============
    Each DMA channel can be managed through 2 Interrupts:

    Interrupt Source
    ----------------
       1. DMA_IT_TC: specifies the interrupt source for the Transfer Complete 
                     event.  
       2. DMA_IT_HT: specifies the interrupt source for the Half-transfer 
                      Complete event.
     
  In this Mode it is advised to use DMA_ITConfig(), DMA_GetITStatus() and
  DMA_ClearITPendingBit() functions.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified DMA Channelx interrupts.
  * @param  DMA_Channelx: selects the DMA Channelx where x can be 0 to 3
  * @param  DMA_ITx: specifies the DMA interrupts sources to be enabled or disabled. 
  *          This parameter can be one of the following values:
  *            @arg DMA_ITx_TC: Transaction Complete Interrupt
  *            @arg DMA_ITx_HT: Half Transaction Interrupt  
  * @param  NewState: new state of the specified DMA interrupts.
    *       This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DMA_ITConfig(DMA_Channel_TypeDef* DMA_Channelx, 
                  DMA_ITx_TypeDef DMA_ITx,
                  FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DMA_CHANNEL(DMA_Channelx));
  assert_param(IS_DMA_CONFIG_ITX(DMA_ITx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected DMA interrupts */
    DMA_Channelx->CCR |= (uint8_t)(DMA_ITx);
  }
  else
  {
    /* Disable the selected DMA interrupts */
    DMA_Channelx->CCR &= (uint8_t)~(DMA_ITx);
  }
}

/**
  * @brief  Checks whether the specified DMA Channelx flag is set or not.
  * @param  DMA_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg DMA1_FLAG_GB: Global Busy Flag
  *            @arg DMA1_FLAG_IFC0: Global Interrupt Flag Channel 0
  *            @arg DMA1_FLAG_IFC1: Global Interrupt Flag Channel 1
  *            @arg DMA1_FLAG_IFC2: Global Interrupt Flag Channel 2
  *            @arg DMA1_FLAG_IFC3: Global Interrupt Flag Channel 3
  *            @arg DMA1_FLAG_TC0: Transaction Complete Interrupt Flag Channel 0
  *            @arg DMA1_FLAG_TC1: Transaction Complete Interrupt Flag Channel 1
  *            @arg DMA1_FLAG_TC2: Transaction Complete Interrupt Flag Channel 2
  *            @arg DMA1_FLAG_TC2: Transaction Complete Interrupt Flag Channel 3
  *            @arg DMA1_FLAG_HT0: Half Transaction Interrupt Flag Channel 0
  *            @arg DMA1_FLAG_HT1: Half Transaction Interrupt Flag Channel 1
  *            @arg DMA1_FLAG_HT2: Half Transaction Interrupt Flag Channel 2
  *            @arg DMA1_FLAG_HT3: Half Transaction Interrupt Flag Channel 3
  *            @arg DMA1_FLAG_PEND0: DMA Request pending on Channel 0
  *            @arg DMA1_FLAG_PEND1: DMA Request pending on Channel 1
  *            @arg DMA1_FLAG_PEND2: DMA Request pending on Channel 2
  *            @arg DMA1_FLAG_PEND3: DMA Request pending on Channel 3
  *            @arg DMA1_FLAG_BUSY0: No DMA transfer on going in Channel 0
  *            @arg DMA1_FLAG_BUSY1: No DMA transfer on going in Channel 1 
  *            @arg DMA1_FLAG_BUSY2: No DMA transfer on going in Channel 2 
  *            @arg DMA1_FLAG_BUSY3: No DMA transfer on going in Channel 3                                        
  * @retval  The status of DMA_FLAG (SET or RESET).
  */
FlagStatus DMA_GetFlagStatus(DMA_FLAG_TypeDef DMA_FLAG)
{
  FlagStatus flagstatus = RESET;
  DMA_Channel_TypeDef* DMA_Channelx =  DMA1_Channel0;
  uint8_t tmpgir1 = 0;
  uint8_t tmpgcsr = 0;

  /* Check the parameters */
  assert_param(IS_DMA_GET_FLAG(DMA_FLAG));

  /* Get flags registers values*/
  tmpgcsr = DMA1->GCSR;
  tmpgir1 = DMA1->GIR1;

  if (((uint16_t)DMA_FLAG & (uint16_t)0x0F00) != (uint16_t)RESET)
  {
    /* find  the used DMA  channel */
    if (((uint16_t)DMA_FLAG & 0x0100) != (uint16_t)RESET)
    {
      DMA_Channelx = DMA1_Channel0;
    }
    else if  (((uint16_t)DMA_FLAG & 0x0200) != (uint16_t)RESET)
    {
      DMA_Channelx = DMA1_Channel1;
    }
    else if  (((uint16_t)DMA_FLAG & 0x0400) != (uint16_t)RESET)
    {
      DMA_Channelx = DMA1_Channel2;
    }
    else
    {
      DMA_Channelx = DMA1_Channel3;
    }

    /*   Get the specified DMA Channelx flag status. */
    flagstatus = (FlagStatus)((uint8_t)(DMA_Channelx->CSPR) & (uint8_t)DMA_FLAG);
  }
  else if (((uint16_t)DMA_FLAG & 0x1000) != (uint16_t)RESET)
  {
    /*   Get the specified DMA Channelx flag status. */
    flagstatus = (FlagStatus)(tmpgir1 & (uint8_t)DMA_FLAG);
  }
  else /*if ((DMA_FLAG & DMA_FLAG_GB) != (uint16_t)RESET)*/
  {
    /*   Get the specified DMA Channelx flag status. */
    flagstatus = (FlagStatus)(tmpgcsr & DMA_GCSR_GB);
  }

  /*  Return the specified DMA Channelx flag status. */
  return (flagstatus);
}

/**
  * @brief  Clears the DMA Channels selected flags.
  * @param  DMA_FLAG: specifies the flag to clear.
  *          This parameter can be one or a combination (for the same channel)of
  *          the following values:
  *            @arg DMA1_FLAG_TC0: Transaction Complete Interrupt Flag Channel 0
  *            @arg DMA1_FLAG_TC1: Transaction Complete Interrupt Flag Channel 1
  *            @arg DMA1_FLAG_TC2: Transaction Complete Interrupt Flag Channel 2
  *            @arg DMA1_FLAG_TC2: Transaction Complete Interrupt Flag Channel 3
  *            @arg DMA1_FLAG_HT0: Half Transaction Interrupt Flag Channel 0
  *            @arg DMA1_FLAG_HT1: Half Transaction Interrupt Flag Channel 1
  *            @arg DMA1_FLAG_HT2: Half Transaction Interrupt Flag Channel 2
  *            @arg DMA1_FLAG_HT3: Half Transaction Interrupt Flag Channel 3
  * @retval None
  */
void DMA_ClearFlag(DMA_FLAG_TypeDef DMA_FLAG)
{
  DMA_Channel_TypeDef* DMA_Channelx =  DMA1_Channel0;

  /* Check the parameters */
  assert_param(IS_DMA_CLEAR_FLAG(DMA_FLAG));

  /* Identify  the used DMA  channel */
  if (((uint16_t)DMA_FLAG & (uint16_t)0x0100) != (uint16_t)RESET)
  {
    DMA_Channelx = DMA1_Channel0;
  }
  else
  {
    if (((uint16_t)DMA_FLAG & (uint16_t)0x0200) != (uint16_t)RESET)
    {
      DMA_Channelx = DMA1_Channel1;
    }
    else
    {
      if (((uint16_t)DMA_FLAG & (uint16_t)0x0400) != (uint16_t)RESET)
      {
        DMA_Channelx = DMA1_Channel2;
      }
      else
      {
        DMA_Channelx = DMA1_Channel3;
      }
    }
  }

  /*Clears the DMA flags.*/
  DMA_Channelx->CSPR &= (uint8_t)~(uint8_t)((uint8_t)DMA_FLAG & (uint8_t)0x06);
}

/**
  * @brief  Checks whether the specified DMA Channelx interrupt has occurred or not.
  * @param  DMA_IT: specifies the DMA interrupt source to check.
  *          This parameter can be one or a combination of the following values:
  *            @arg DMA1_IT_TC0: Transaction Complete Interrupt Channel 0
  *            @arg DMA1_IT_TC1: Transaction Complete Interrupt Channel 1
  *            @arg DMA1_IT_TC2: Transaction Complete Interrupt Channel 2
  *            @arg DMA1_IT_TC3: Transaction Complete Interrupt Channel 3
  *            @arg DMA1_IT_HT0: Half Transaction Interrupt Channel 0
  *            @arg DMA1_IT_HT1: Half Transaction Interrupt Channel 1
  *            @arg DMA1_IT_HT2: Half Transaction Interrupt Channel 2
  *            @arg DMA1_IT_HT3: Half Transaction Interrupt Channel 3    
  * @retval ITStatus: The status of DMA_IT (SET or RESET).
  */
ITStatus DMA_GetITStatus(DMA_IT_TypeDef DMA_IT)
{
  ITStatus itstatus = RESET;
  uint8_t tmpreg = 0;
  uint8_t tmp2 = 0;
  DMA_Channel_TypeDef* DMA_Channelx =  DMA1_Channel0;

  /* Check the parameters */
  assert_param(IS_DMA_GET_IT(DMA_IT));

  /* Identify  the used DMA  channel */
  if ((DMA_IT & 0x10) != (uint8_t)RESET)
  {
    DMA_Channelx = DMA1_Channel0;
  }
  else
  {
    if  ((DMA_IT & 0x20) != (uint8_t)RESET)
    {
      DMA_Channelx = DMA1_Channel1;
    }
    else
    {
      if  ((DMA_IT & 0x40) != (uint8_t)RESET)
      {
        DMA_Channelx = DMA1_Channel2;
      }
      else
      {
        DMA_Channelx = DMA1_Channel3;
      }
    }
  }
  /*   Get the specified DMA Channelx interrupt status. */
  tmpreg =  DMA_Channelx->CSPR ;
  tmpreg &= DMA_Channelx->CCR ;
  tmp2 = (uint8_t)(DMA_IT & (uint8_t)(DMA_CCR_TCIE | DMA_CCR_HTIE));
  itstatus = (ITStatus)((uint8_t)tmpreg & (uint8_t)tmp2);

  /*   Return the specified DMA Channelx interrupt status. */
  return (itstatus);
}

/**
  * @brief  Clears the DMA Channelx’s interrupt pending bits.
  * @param  DMA_IT: specifies the DMA interrupt pending bit to clear.
  *          This parameter can be one or a combination(for the same channel)of 
  *          the following values:
  *            @arg DMA1_IT_TC0: Transaction Complete Interrupt Channel 0
  *            @arg DMA1_IT_TC1: Transaction Complete Interrupt Channel 1
  *            @arg DMA1_IT_TC2: Transaction Complete Interrupt Channel 2
  *            @arg DMA1_IT_TC3: Transaction Complete Interrupt Channel 3
  *            @arg DMA1_IT_HT0: Half Transaction Interrupt Channel 0
  *            @arg DMA1_IT_HT1: Half Transaction Interrupt Channel 1
  *            @arg DMA1_IT_HT2: Half Transaction Interrupt Channel 2
  *            @arg DMA1_IT_HT3: Half Transaction Interrupt Channel 3 
  * @retval None
  */
void DMA_ClearITPendingBit(DMA_IT_TypeDef DMA_IT)
{
  DMA_Channel_TypeDef* DMA_Channelx =  DMA1_Channel0;

  /* Check the parameters */
  assert_param(IS_DMA_CLEAR_IT(DMA_IT));
  /* Identify  the used DMA  channel */
  if ((DMA_IT & 0x10) != (uint8_t)RESET)
  {
    DMA_Channelx = DMA1_Channel0;
  }
  else
  {
    if ((DMA_IT & 0x20) != (uint8_t)RESET)
    {
      DMA_Channelx = DMA1_Channel1;
    }
    else
    {
      if ((DMA_IT & 0x40) != (uint8_t)RESET)
      {
        DMA_Channelx = DMA1_Channel2;
      }
      else
      {
        DMA_Channelx = DMA1_Channel3;
      }
    }
  }
  /*Clears the DMA Channelx’s interrupt pending bits*/
  DMA_Channelx->CSPR &= (uint8_t)~(uint8_t)(DMA_IT & (uint8_t)0x06);
}

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

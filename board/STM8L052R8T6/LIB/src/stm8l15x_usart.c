/**
  ******************************************************************************
  * @file    stm8l15x_usart.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Universal synchronous asynchronous receiver
  *          transmitter (USART):
  *           - Initialization and Configuration
  *           - Data transfers
  *           - Multi-Processor Communication
  *           - Half-duplex mode
  *           - Smartcard mode
  *           - IrDA mode
  *           - DMA transfers management
  *           - Interrupts and flags management
  *
  *  @verbatim
  *
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          1. Enable peripheral clock using CLK_PeripheralClockConfig(CLK_Peripheral_USARTx,
  *             ENABLE) function (Refer to the product datasheet for the available USART
  *           	peripherals)
  *
  *          2.  Enable the external Pull-up on the used USART Pins using the
  *              GPIO_ExternalPullUpConfig() function or an external pull-up equivalent resistor
	*              (RPU = 45 KOhm typical value).
	*
  *          3. Program the Baud Rate, Word Length , Stop Bit, Parity and Mode (Receiver/Transmitter)
  *           	using the USART_Init() function.
  *
  *          4. For synchronous mode, enable the clock and program the polarity,
  *             phase and last bit using the USART_ClockInit() function.
  *
  *          5. Enable the corresponding interrupt using the function USART_ITConfig() if you need
	*             to use interrupt mode.
  *
  *          6. When using the DMA mode
  *                   - Configure the DMA using DMA_Init() function
  *                   - Activate the needed channel Request using USART_DMACmd() function
  *
  *          7. Enable the USART using the USART_Cmd() function.
  *
  *          8. Enable the DMA using the DMA_Cmd() function, when using DMA mode.
  *
  *          Refer to Multi-Processor, half-duplex, Smartcard, IrDA sub-sections for more details.
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
#include "stm8l15x_usart.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup USART
  * @brief USART driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup USART_Private_Functions
  * @{
  */

/** @defgroup USART_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
                  Initialization and Configuration functions
 ===============================================================================

  This subsection provides a set of functions allowing to initialize the USART
  in asynchronous and in synchronous modes.
   - For the asynchronous mode only these parameters can be configured:
        - Baud Rate
        - Word Length
        - Stop Bit
        - Parity: If the parity is enabled, then the MSB bit of the data written
          in the data register is transmitted but is changed by the parity bit.
          Depending on the frame length defined by the M bit (8-bits or 9-bits),
          the possible USART frame formats are as listed in the following table:
   +-------------------------------------------------------------+
   |   M bit |  PCE bit  |            USART frame                |
   |---------------------|---------------------------------------|
   |    0    |    0      |    | SB | 8 bit data | STB |          |
   |---------|-----------|---------------------------------------|
   |    0    |    1      |    | SB | 7 bit data | PB | STB |     |
   |---------|-----------|---------------------------------------|
   |    1    |    0      |    | SB | 9 bit data | STB |          |
   |---------|-----------|---------------------------------------|
   |    1    |    1      |    | SB | 8 bit data | PB | STB |     |
   +-------------------------------------------------------------+
        - Receiver/transmitter modes

  The USART_Init() function follows the USART  asynchronous configuration procedure
  (details for the procedure are available in reference manual (RM0031)).

  - For the synchronous mode in addition to the asynchronous mode parameters these
    parameters should be also configured:
        - USART Clock Enabled
        - USART polarity
        - USART phase
        - USART LastBit

  These parameters can be configured using the USART_ClockInit() function.

@endverbatim
  * @{
  */

/**
  * @brief   Deinitializes the USART peripheral.
  * @param  USARTx: where x can be 1, 2 or 3 to select the specified USART peripheral.
  * @retval None
  */
void USART_DeInit(USART_TypeDef* USARTx)
{

  /* Clear the Idle Line Detected bit in the status register by a read
      to the USART_SR register followed by a Read to the USART_DR register */
  (void) USARTx->SR;
  (void) USARTx->DR;

  USARTx->BRR2 = USART_BRR2_RESET_VALUE;  /* Set USART_BRR2 to reset value 0x00 */
  USARTx->BRR1 = USART_BRR1_RESET_VALUE;  /* Set USART_BRR1 to reset value 0x00 */

  USARTx->CR1 = USART_CR1_RESET_VALUE;  /* Set USART_CR1 to reset value 0x00 */
  USARTx->CR2 = USART_CR2_RESET_VALUE;  /* Set USART_CR2 to reset value 0x00 */
  USARTx->CR3 = USART_CR3_RESET_VALUE;  /* Set USART_CR3 to reset value 0x00 */
  USARTx->CR4 = USART_CR4_RESET_VALUE;  /* Set USART_CR4 to reset value 0x00 */
}

/**
  * @brief  Initializes the USART according to the specified parameters.
  * @note   Configure in Push Pull or Open Drain mode the Tx pin by setting the
  *         correct I/O Port register according the product package and line
  *         configuration
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  BaudRate: The baudrate.
  * @param  USART_WordLength: the word length
  *           This parameter can be one of the following values:
  *            @arg USART_WordLength_8b: 8 bits Data
  *            @arg USART_WordLength_9b: 9 bits Data
  * @param  USART_StopBits: Stop Bit
  *           This parameter can be one of the following values:
  *            @arg USART_StopBits_1: One stop bit is transmitted at the end of frame
  *            @arg USART_StopBits_2: Two stop bits are transmitted at the end of frame
  *            @arg USART_StopBits_1_5: One and half stop bits
  * @param  USART_Parity: Parity
  *           This parameter can be one of the following values:
  *            @arg USART_Parity_No: No Parity
  *            @arg USART_Parity_Even: Even Parity
  *            @arg USART_Parity_Odd: Odd Parity
  * @param  USART_Mode: Mode
  *           This parameter can be one of the following values:
  *            @arg USART_Mode_Rx: Receive Enable
  *            @arg USART_Mode_Tx: Transmit Enable
  * @retval None
  */
void USART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, USART_WordLength_TypeDef
                USART_WordLength, USART_StopBits_TypeDef USART_StopBits,
                USART_Parity_TypeDef USART_Parity,  USART_Mode_TypeDef USART_Mode)
{
  uint32_t BaudRate_Mantissa = 0;

  /* Check the parameters */
  assert_param(IS_USART_BAUDRATE(BaudRate));

  assert_param(IS_USART_WORDLENGTH(USART_WordLength));

  assert_param(IS_USART_STOPBITS(USART_StopBits));

  assert_param(IS_USART_PARITY(USART_Parity));

  assert_param(IS_USART_MODE(USART_Mode));

  /* Clear the word length and Parity Control bits */
  USARTx->CR1 &= (uint8_t)(~(USART_CR1_PCEN | USART_CR1_PS | USART_CR1_M));
  /* Set the word length bit according to USART_WordLength value */
  /* Set the Parity Control bit to USART_Parity value */
  USARTx->CR1 |= (uint8_t)((uint8_t)USART_WordLength | (uint8_t)USART_Parity);

  /* Clear the STOP bits */
  USARTx->CR3 &= (uint8_t)(~USART_CR3_STOP);
  /* Set the STOP bits number according to USART_StopBits value */
  USARTx->CR3 |= (uint8_t)USART_StopBits;

  /* Clear the LSB mantissa of USARTDIV */
  USARTx->BRR1 &= (uint8_t)(~USART_BRR1_DIVM);
  /* Clear the MSB mantissa of USARTDIV */
  USARTx->BRR2 &= (uint8_t)(~USART_BRR2_DIVM);
  /* Clear the Fraction bits of USARTDIV */
  USARTx->BRR2 &= (uint8_t)(~USART_BRR2_DIVF);

  BaudRate_Mantissa  = (uint32_t)(CLK_GetClockFreq() / BaudRate );
  /* Set the fraction of USARTDIV */
  USARTx->BRR2 = (uint8_t)((BaudRate_Mantissa >> (uint8_t)8) & (uint8_t)0xF0);
  /* Set the MSB mantissa of USARTDIV */
  USARTx->BRR2 |= (uint8_t)(BaudRate_Mantissa & (uint8_t)0x0F);
  /* Set the LSB mantissa of USARTDIV */
  USARTx->BRR1 = (uint8_t)(BaudRate_Mantissa >> (uint8_t)4);

  /* Disable the Transmitter and Receiver */
  USARTx->CR2 &= (uint8_t)~(USART_CR2_TEN | USART_CR2_REN);
  /* Set TEN and REN bits according to USART_Mode value */
  USARTx->CR2 |= (uint8_t)USART_Mode;
}

/**
  * @brief  Initializes the USART Clock according to the specified parameters.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_Clock: Clock
  *           This parameter can be one of the following values:
  *            @arg USART_Clock_Disable: CK pin disabled
  *            @arg USART_Clock_Enable: CK pin enabled
  * @param  USART_CPOL: Clock Polarity
  *           This parameter can be one of the following values:
  *            @arg USART_CPOL_Low: CK to 0 when idle
  *            @arg USART_CPOL_High: CK to 1 when idle
  * @param  USART_CPHA: Clock Phase
  *           This parameter can be one of the following values:
  *            @arg USART_CPHA_1Edge: The first clock transition is the first data capture edge
  *            @arg USART_CPHA_2Edge: The second clock transition is the first data capture edge
  * @param  USART_LastBit: Last Bit
  *           This parameter can be one of the following values:
  *            @arg USART_LastBit_Disable: The clock pulse of the last data bit is
  *               not output to the SCLK pin
  *            @arg USART_LastBit_Enable: The clock pulse of the last data bit is
  *               output to the SCLK pin
  * @retval None
  */
void USART_ClockInit(USART_TypeDef* USARTx, USART_Clock_TypeDef USART_Clock,
                     USART_CPOL_TypeDef USART_CPOL, USART_CPHA_TypeDef USART_CPHA,
                     USART_LastBit_TypeDef USART_LastBit)
{
  /* Check the parameters */
  assert_param(IS_USART_CLOCK(USART_Clock));
  assert_param(IS_USART_CPOL(USART_CPOL));
  assert_param(IS_USART_CPHA(USART_CPHA));
  assert_param(IS_USART_LASTBIT(USART_LastBit));

  /* Clear the Clock Polarity, lock Phase, Last Bit Clock pulse */
  USARTx->CR3 &= (uint8_t)~(USART_CR3_CPOL | USART_CR3_CPHA | USART_CR3_LBCL);
  /* Set the Clock Polarity, lock Phase, Last Bit Clock pulse */
  USARTx->CR3 |= (uint8_t)((uint8_t)((uint8_t)(USART_CPOL | (uint8_t)USART_CPHA ) | USART_LastBit));

  if (USART_Clock != USART_Clock_Disable)
  {
    USARTx->CR3 |= (uint8_t)(USART_CR3_CLKEN); /* Set the Clock Enable bit */
  }
  else
  {
    USARTx->CR3 &= (uint8_t)(~USART_CR3_CLKEN); /* Clear the Clock Enable bit */
  }
}

/**
  * @brief  Enable the USART peripheral.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  NewState: The new state of the USART Communication.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    USARTx->CR1 &= (uint8_t)(~USART_CR1_USARTD); /**< USART Enable */
  }
  else
  {
    USARTx->CR1 |= USART_CR1_USARTD;  /**< USART Disable (for low power consumption) */
  }
}

/**
  * @brief  Sets the system clock prescaler.
  * @note   IrDA Low Power mode or smartcard mode should be enabled
  * @note   This function is related to SmartCard and IrDa mode.
  * @param  USARTx: Select the USARTx peripheral.
  * @param  USART_Prescaler: specifies the prescaler clock.
  * @note   In IrDA Low Power Mode the clock source is divided by the value given
  *         in the register (8 bits)
  *         - 0000 0000 Reserved
  *         - 0000 0001 divides the clock source by 1
  *         - 0000 0010 divides the clock source by 2
  *         - ...
  * @note   In Smart Card Mode the clock source is divided by the value given in the register
  *         (5 significant bits) multiplied by 2
  *         - 0 0000 Reserved
  *         - 0 0001 divides the clock source by 2
  *         - 0 0010 divides the clock source by 4
  *         - 0 0011 divides the clock source by 6
  *         - ...
  * @retval None
  */
void USART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler)
{
  /* Load the USART prescaler value*/
  USARTx->PSCR = USART_Prescaler;
}

/**
  * @brief  Transmits break characters.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @retval None
  */
void USART_SendBreak(USART_TypeDef* USARTx)
{
  USARTx->CR2 |= USART_CR2_SBK;
}

/**
  * @}
  */

/** @defgroup USART_Group2 Data transfers functions
 *  @brief   Data transfers functions
 *
@verbatim
 ===============================================================================
                            Data transfers functions
 ===============================================================================

  This subsection provides a set of functions allowing to manage the USART data
  transfers.

  During an USART reception, data shifts in least significant bit first through
  the RX pin. In this mode, the USART_DR register is similar to a buffer (RDR)
  between the internal bus and the received shift register.

  When a transmission is taking place, a write instruction to the USART_DR register
  stores the data in the TDR register which is copied in the shift register
  at the end of the current transmission.

  The read access of the USART_DR register can be done using the USART_ReceiveData8()
  or USART_ReceiveData9() functions and returns the RDR buffered value. Whereas a write
	access to the USART_DR can be done using USART_SendData8() or USART_SendData9()
	functions and stores the written data into TDR buffer.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the most recent received data by the USART peripheral.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @retval The received data.
  */
uint8_t USART_ReceiveData8(USART_TypeDef* USARTx)
{
  return USARTx->DR;
}

/**
  * @brief  Returns the most recent received data by the USART peripheral.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @retval The received data.
  */
uint16_t USART_ReceiveData9(USART_TypeDef* USARTx)
{
  uint16_t temp = 0;

  temp = ((uint16_t)(((uint16_t)((uint16_t)USARTx->CR1 & (uint16_t)USART_CR1_R8)) << 1));
  return (uint16_t)( ((uint16_t)((uint16_t)USARTx->DR) | temp) & ((uint16_t)0x01FF));
}

/**
  * @brief  Transmits 8 bit data through the USART peripheral.
  * @param  Data: The data to transmit.
  * @retval None
  */
void USART_SendData8(USART_TypeDef* USARTx, uint8_t Data)
{
  /* Transmit Data */
  USARTx->DR = Data;
}

/**
  * @brief  Transmits 9 bit data through the USART peripheral.
  * @param  USARTx: Select the USARTx peripheral.
  * @param  Data: The data to transmit.
  *         This parameter should be lower than 0x1FF.
  * @retval None
  */
void USART_SendData9(USART_TypeDef* USARTx, uint16_t Data)
{
  assert_param(IS_USART_DATA_9BITS(Data));

  /* Clear the transmit data bit 8     */
  USARTx->CR1 &= ((uint8_t)~USART_CR1_T8);

  /* Write the transmit data bit [8]   */
  USARTx->CR1 |= (uint8_t)(((uint8_t)(Data >> 2)) & USART_CR1_T8);

  /* Write the transmit data bit [0:7] */
  USARTx->DR   = (uint8_t)(Data);
}
/**
  * @}
  */

/** @defgroup USART_Group3 MultiProcessor Communication functions
 *  @brief   Multi-Processor Communication functions
 *
@verbatim
 ===============================================================================
                    Multi-Processor Communication functions
 ===============================================================================

  This subsection provides a set of functions allowing to manage the USART
  multiprocessor communication.

  For instance one of the USARTs can be the master, its TX output is connected to
  the RX input of the other USART. The others are slaves, their respective TX outputs
  are logically ANDed together and connected to the RX input of the master.

  USART multiprocessor communication is possible through the following procedure:
     1. Program the Baud rate, Word length = 9 bits, Stop bits, Parity, Mode transmitter
        or Mode receiver and hardware flow control values using the USART_Init()
        function.
     2. Configures the USART address using the USART_SetAddress() function.
     3. Configures the wake up method (USART_WakeUp_IdleLine or USART_WakeUp_AddressMark)
        using USART_WakeUpConfig() function only for the slaves.
     4. Enable the USART using the USART_Cmd() function.
     5. Enter the USART slaves in mute mode using USART_ReceiverWakeUpCmd() function.

  The USART Slave exit from mute mode when receiving the wake up condition.

@endverbatim
  * @{
  */

/**
  * @brief  Determines if the USART is in mute mode or not.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  NewState: The new state of the USART mode.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the mute mode USART by setting the RWU bit in the CR2 register */
    USARTx->CR2 |= USART_CR2_RWU;
  }
  else
  {
    /* Disable the mute mode USART by clearing the RWU bit in the CR1 register */
    USARTx->CR2 &= ((uint8_t)~USART_CR2_RWU);
  }
}

/**
  * @brief  Sets the address of the USART node.
  * @param  USARTx: Select the USARTx peripheral.
  * @param  Address: Indicates the address of the USART node.
  *         This parameter should be lower than 16
  * @retval None
  */
void USART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address)
{
  /* assert_param for USART_Address */
  assert_param(IS_USART_ADDRESS(USART_Address));

  /* Clear the USART address */
  USARTx->CR4 &= ((uint8_t)~USART_CR4_ADD);
  /* Set the USART address node */
  USARTx->CR4 |= USART_Address;
}

/**
  * @brief  Selects the USART WakeUp method.
  * @param  USART_WakeUp: Specifies the USART wakeup method.
  *           This parameter can be one of the following values:
  *            @arg USART_WakeUp_IdleLine: 0x01 Idle Line wake up
  *            @arg USART_WakeUp_AddressMark: 0x02 Address Mark wake up
  * @retval None
  */
void USART_WakeUpConfig(USART_TypeDef* USARTx, USART_WakeUp_TypeDef USART_WakeUp)
{
  assert_param(IS_USART_WAKEUP(USART_WakeUp));

  USARTx->CR1 &= ((uint8_t)~USART_CR1_WAKE);
  USARTx->CR1 |= (uint8_t)USART_WakeUp;
}
/**
  * @}
  */

/** @defgroup USART_Group4 Halfduplex mode function
 *  @brief   Half-duplex mode function
 *
@verbatim
 ===============================================================================
                         Half-duplex mode function
 ===============================================================================

  This subsection provides a function allowing to manage the USART
  Half-duplex communication.

  The USART can be configured to follow a single-wire half-duplex protocol where
  the TX and RX lines are internally connected.

  USART Half duplex communication is possible through the following procedure:
     1. Program the Baud rate, Word length, Stop bits, Parity, Mode transmitter
        or Mode receiver and hardware flow control values using the USART_Init()
        function.
     2. Configures the USART address using the USART_SetAddress() function.
     3. Enable the USART using the USART_Cmd() function.
     4. Enable the half duplex mode using USART_HalfDuplexCmd() function.

Note:
----
  1. The RX pin is no longer used
  2. In Half-duplex mode the following bits must be kept cleared:
       - CLKEN bits in the USART_CR3 register.
       - SCEN and IREN bits in the USART_CR5 register.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the USART’s Half Duplex communication.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  NewState new state of the USART Communication.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    USARTx->CR5 |= USART_CR5_HDSEL;  /**< USART Half Duplex Enable  */
  }
  else
  {
    USARTx->CR5 &= (uint8_t)~USART_CR5_HDSEL; /**< USART Half Duplex Disable */
  }
}

/**
  * @}
  */

/** @defgroup USART_Group5 Smartcard mode functions
 *  @brief   Smartcard mode functions
 *
@verbatim
 ===============================================================================
                               Smartcard mode functions
 ===============================================================================

  This subsection provides a set of functions allowing to manage the USART
  Smartcard communication.

  The Smartcard interface is designed to support asynchronous protocol Smartcards as
  defined in the ISO 7816-3 standard.

  The USART can provide a clock to the smartcard through the SCLK output.
  In smartcard mode, SCLK is not associated to the communication but is simply derived
  from the internal peripheral input clock through a 5-bit prescaler.

  Smartcard communication is possible through the following procedure:
     1. Configures the Smartcard Prsecaler using the USART_SetPrescaler() function.
     2. Configures the Smartcard Guard Time using the USART_SetGuardTime() function.
     3. Program the USART clock using the USART_ClockInit() function as following:
        - USART Clock enabled
        - USART CPOL Low
        - USART CPHA on first edge
        - USART Last Bit Clock Enabled
     4. Program the Smartcard interface using the USART_Init() function as following:
        - Word Length = 9 Bits
        - 1.5 Stop Bit
        - Even parity
        - BaudRate = 12096 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Tx and Rx enabled
     5. Optionally you can enable the parity error interrupt using the USART_ITConfig()
        function
     6. Enable the USART using the USART_Cmd() function.
     7. Enable the Smartcard NACK using the USART_SmartCardNACKCmd() function.
     8. Enable the Smartcard interface using the USART_SmartCardCmd() function.

  Please refer to the ISO 7816-3 specification for more details.

Note:
-----
  1.  It is also possible to choose 0.5 stop bit for receiving but it is recommended
      to use 1.5 stop bits for both transmitting and receiving to avoid switching
      between the two configurations.
  2. In smartcard mode, the following bits must be kept cleared:
     - HDSEL and IREN bits in the USART_CR5 register.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the USART Smart Card mode.
  * @param  USARTx: Select the USARTx peripheral.
  * @param  NewState: new state of the Smart Card mode.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the SC mode by setting the SCEN bit in the CR5 register */
    USARTx->CR5 |= USART_CR5_SCEN;
  }
  else
  {
    /* Disable the SC mode by clearing the SCEN bit in the CR5 register */
    USARTx->CR5 &= ((uint8_t)(~USART_CR5_SCEN));
  }
}

/**
  * @brief  Enables or disables NACK transmission.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  NewState: new state of the Smart Card mode.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the NACK transmission by setting the NACK bit in the CR5 register */
    USARTx->CR5 |= USART_CR5_NACK;
  }
  else
  {
    /* Disable the NACK transmission by clearing the NACK bit in the CR5 register */
    USARTx->CR5 &= ((uint8_t)~(USART_CR5_NACK));
  }
}

/**
  * @brief  Sets the specified USART guard time.
  * @note   SmartCard Mode should be Enabled
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_GuardTime: specifies the guard time.
  * @retval None
  */
void USART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime)
{
  /* Set the USART guard time */
  USARTx->GTR = USART_GuardTime;
}

/**
  * @}
  */

/** @defgroup USART_Group6 IrDA mode functions
 *  @brief   IrDA mode functions
 *
@verbatim
 ===============================================================================
                                IrDA mode functions
 ===============================================================================

  This subsection provides a set of functions allowing to manage the USART
  IrDA communication.

  IrDA is a half duplex communication protocol. If the Transmitter is busy, any data
  on the IrDA receive line will be ignored by the IrDA decoder and if the Receiver
  is busy, data on the TX from the USART to IrDA will not be encoded by IrDA.
  While receiving data, transmission should be avoided as the data to be transmitted
  could be corrupted.

  IrDA communication is possible through the following procedure:
     1. Program the Baud rate, Word length = 8 bits, Stop bits, Parity, Transmitter/Receiver
        modes and hardware flow control values using the USART_Init() function.
     2. Enable the USART using the USART_Cmd() function.
     3. Configures the IrDA pulse width by configuring the prescaler using
        the USART_SetPrescaler() function.
     4. Configures the IrDA  USART_IrDAMode_LowPower or USART_IrDAMode_Normal mode
        using the USART_IrDAConfig() function.
     5. Enable the IrDA using the USART_IrDACmd() function.

Note:
-----
  1. A pulse of width less than two and greater than one PSC period(s) may or may
     not be rejected.
  2. The receiver set up time should be managed by software. The IrDA physical layer
     specification specifies a minimum of 10 ms delay between transmission and
     reception (IrDA is a half duplex protocol).
  3. In IrDA mode, the following bits must be kept cleared:
    - STOP and CLKEN bits in the USART_CR3 register.
    - SCEN and HDSEL bits in the USART_CR5 register.

@endverbatim
  * @{
  */

/**
  * @brief  Configures the USART’s IrDA interface.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_IrDAMode specifies the IrDA mode.
  *           This parameter can be one of the following values:
  *            @arg USART_IrDAMode_Normal: IrDA Normal Mode
  *            @arg USART_IrDAMode_LowPower: IrDA Low Power Mode
  * @retval None
  */
void USART_IrDAConfig(USART_TypeDef* USARTx, USART_IrDAMode_TypeDef USART_IrDAMode)
{
  assert_param(IS_USART_IRDAMODE(USART_IrDAMode));

  if (USART_IrDAMode != USART_IrDAMode_Normal)
  {
    USARTx->CR5 |= USART_CR5_IRLP;
  }
  else
  {
    USARTx->CR5 &= ((uint8_t)~USART_CR5_IRLP);
  }
}

/**
  * @brief  Enables or disables the USART’s IrDA interface.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  NewState new state of the IrDA mode.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState)
{

  /* Check parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the IrDA mode by setting the IREN bit in the CR3 register */
    USARTx->CR5 |= USART_CR5_IREN;
  }
  else
  {
    /* Disable the IrDA mode by clearing the IREN bit in the CR3 register */
    USARTx->CR5 &= ((uint8_t)~USART_CR5_IREN);
  }
}

/**
  * @}
  */

/** @defgroup USART_Group7 DMA transfers management functions
 *  @brief   DMA transfers management functions
 *
@verbatim
 ===============================================================================
                      DMA transfers management functions
 ===============================================================================

@endverbatim
  * @{
  */


/**
  * @brief  Enables or disables the USART DMA interface.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_DMAReq Specifies the USART DMA transfer request to be enabled or disabled.
  *           This parameter can be one of the following values:
  *            @arg USART_DMAReq_TX: Receive DMA request Enable
  *            @arg USART_DMAReq_RX: Transmit DMA request Enable
  * @param  NewState Indicates the new state of the USART DMA request.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_DMACmd(USART_TypeDef* USARTx, USART_DMAReq_TypeDef USART_DMAReq,
                  FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_USART_DMAREQ(USART_DMAReq));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the DMA transfer for selected requests by setting the DMAT and/or
       DMAR bits in the USART CR5 register */
    USARTx->CR5 |= (uint8_t) USART_DMAReq;
  }
  else
  {
    /* Disable the DMA transfer for selected requests by clearing the DMAT and/or
       DMAR bits in the USART CR5 register */
    USARTx->CR5 &= (uint8_t)~USART_DMAReq;
  }
}

/**
  * @}
  */

/** @defgroup USART_Group8 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim
 ===============================================================================
                   Interrupts and flags management functions
 ===============================================================================

  This subsection provides a set of functions allowing to configure the USART
  Interrupts sources, DMA channels requests and check or clear the flags or
  pending bits status.
  The user should identify which mode will be used in his application to manage
  the communication: Polling mode, Interrupt mode or DMA mode.

  Polling Mode
  =============
  In Polling Mode, the USART communication can be managed by 9 flags:
     1. USART_FLAG_TXE: to indicate the status of the transmit buffer register
     2. USART_FLAG_RXNE: to indicate the status of the receive buffer register
     3. USART_FLAG_TC: to indicate the status of the transmit operation
     4. USART_FLAG_IDLE: to indicate the status of the Idle Line
     5. USART_FLAG_SBK: to indicate the status of the Send Break characters
     6. USART_FLAG_NE: to indicate if a noise error occur
     7. USART_FLAG_FE: to indicate if a frame error occur
     8. USART_FLAG_PE: to indicate if a parity error occur
     9. USART_FLAG_ORE: to indicate if an Overrun error occur

  In this Mode it is advised to use the following functions:
      - FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG);
      - void USART_ClearFlag(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG);

  Interrupt Mode
  ===============
  In Interrupt Mode, the USART communication can be managed by 7 interrupt sources
  and 6 pending bits:

  Pending Bits:
  -------------
     1. USART_IT_TXE: to indicate the status of the transmit buffer register
     2. USART_IT_RXNE: to indicate the status of the receive buffer register
     3. USART_IT_TC: to indicate the status of the transmit operation
     4. USART_IT_IDLE: to indicate the status of the Idle Line
     5. USART_IT_PE: to indicate if a parity error occur
     6. USART_IT_OR: to indicate if an Noise flag, overrun error and framing error in
											multibuffer communication error occur

  Interrupt Source:
  -----------------
     1. USART_IT_TXE: specifies the interrupt source for the Tx buffer empty
                       interrupt.
     2. USART_IT_RXNE: specifies the interrupt source for the Rx buffer not
                        empty interrupt.
     3. USART_IT_TC: specifies the interrupt source for the Transmit complete
											interrupt.
     4. USART_IT_IDLE: specifies the interrupt source for the Idle Line interrupt.
     5. USART_IT_PE: specifies the interrupt source for the parity error interrupt.
     6. USART_IT_ERR:  specifies the interrupt source for the errors interrupt.
		 7. USART_IT_OR: specifies the interrupt source for the overrun error interrupt.

    Note: Some parameters are coded in order to use them as interrupt source or
    ----  as pending bits.

  In this Mode it is advised to use the following functions:
     - void USART_ITConfig(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT, FunctionalState
                      		 NewState);
     - ITStatus USART_GetITStatus(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT);
     - void USART_ClearITPendingBit(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT);

  DMA Mode
  ========
  In DMA Mode, the USART communication can be managed by 2 DMA Channel requests:
     1. USART_DMAReq_Tx: specifies the Tx buffer DMA transfer request
     2. USART_DMAReq_Rx: specifies the Rx buffer DMA transfer request

  In this Mode it is advised to use the following function:
     - void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified USART interrupts.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_IT specifies the USART interrupt sources to be enabled or disabled.
  *         This parameter can be one of the following values:
  *            @arg USART_IT_TXE: Transmit Data Register empty interrupt
  *            @arg USART_IT_TC: Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_OR: Overrun error interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_ERR: Error interrupt
  * @param  NewState new state of the specified USART interrupts.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_ITConfig(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT, FunctionalState NewState)
{
  uint8_t usartreg, itpos = 0x00;
  assert_param(IS_USART_CONFIG_IT(USART_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Get the USART register index */
  usartreg = (uint8_t)((uint16_t)USART_IT >> 0x08);
  /* Get the USART IT index */
  itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)USART_IT & (uint8_t)0x0F));

  if (NewState != DISABLE)
  {
    /**< Enable the Interrupt bits according to USART_IT mask */
    if (usartreg == 0x01)
    {
      USARTx->CR1 |= itpos;
    }
    else if (usartreg == 0x05)
    {
      USARTx->CR5 |= itpos;
    }
    /*uartreg =0x02*/
    else
    {
      USARTx->CR2 |= itpos;
    }
  }
  else
  {
    /**< Disable the interrupt bits according to USART_IT mask */
    if (usartreg == 0x01)
    {
      USARTx->CR1 &= (uint8_t)(~itpos);
    }
    else if (usartreg == 0x05)
    {
      USARTx->CR5 &= (uint8_t)(~itpos);
    }
    /*uartreg =0x02*/
    else
    {
      USARTx->CR2 &= (uint8_t)(~itpos);
    }
  }
}

/**
  * @brief  Checks whether the specified USART flag is set or not.
  * @param  USARTx: Select the USARTx peripheral.
  * @param  USART_FLAG specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg USART_FLAG_TXE: Transmit Data Register empty
  *            @arg USART_FLAG_TC: Transmission Complete
  *            @arg USART_FLAG_RXNE: Read Data Register Not Empty
  *            @arg USART_FLAG_IDLE: Idle line detected
  *            @arg USART_FLAG_OR: OverRun error
  *            @arg USART_FLAG_NF: Noise error
  *            @arg USART_FLAG_FE: Framing Error
  *            @arg USART_FLAG_PE: Parity Error
  *            @arg USART_FLAG_SBK: Send Break characters
  * @retval FlagStatus (SET or RESET)
  */
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG)
{
  FlagStatus status = RESET;

  /* Check parameters */
  assert_param(IS_USART_FLAG(USART_FLAG));

  if (USART_FLAG == USART_FLAG_SBK)
  {
    if ((USARTx->CR2 & (uint8_t)USART_FLAG) != (uint8_t)0x00)
    {
      /* USART_FLAG is set*/
      status = SET;
    }
    else
    {
      /* USART_FLAG is reset*/
      status = RESET;
    }
  }
  else
  {
    if ((USARTx->SR & (uint8_t)USART_FLAG) != (uint8_t)0x00)
    {
      /* USART_FLAG is set*/
      status = SET;
    }
    else
    {
      /* USART_FLAG is reset*/
      status = RESET;
    }
  }
  /* Return the USART_FLAG status*/
  return status;
}

/**
  * @brief  Clears the USARTx's pending flags.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_FLAG: specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg USART_FLAG_TC:   Transmission Complete flag.
  *            @arg USART_FLAG_RXNE: Receive data register not empty flag.
  * @note   PE (Parity error), FE (Framing error), NE (Noise error), OR (OverRun error)
  *         and IDLE (Idle line detected) flags are cleared by software sequence: a read
  *         operation to USART_SR register (USART_GetFlagStatus())followed by a read
  *         operation to USART_DR register(USART_ReceiveData8() or USART_ReceiveData9()).
  * @note   RXNE flag can be also cleared by a read to the USART_DR register
  *         (USART_ReceiveData8()or USART_ReceiveData9()).
  * @note   TC flag can be also cleared by software sequence: a read operation to USART_SR
  *         register (USART_GetFlagStatus()) followed by a write operation to USART_DR
  *         register (USART_SendData8() or USART_SendData9()).
  * @note    TXE flag is cleared only by a write to the USART_DR register
  *         (USART_SendData8() or USART_SendData9()).
  * @note   SBK flag is cleared during the stop bit of break.
  * @retval None
  */
void USART_ClearFlag(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG)
{
  /* Check the parameters */
  assert_param(IS_USART_CLEAR_FLAG(USART_FLAG));
  /*< Clear RXNE or TC flags */
  USARTx->SR = (uint8_t)((uint16_t)~((uint16_t)USART_FLAG));
}

/**
  * @brief  Checks whether the specified USART interrupt has occurred or not.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_IT: Specifies the USART interrupt pending bit to check.
  *         This parameter can be one of the following values:
  *            @arg USART_IT_TXE: Transmit Data Register empty interrupt
  *            @arg USART_IT_TC: Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_OR: OverRun Error interrupt
  *            @arg USART_IT_PE: Parity Error interrupt
  *            @arg USART_IT_FE: Frame Error interrupt
  *            @arg USART_IT_NF: Noise Flag Error interrupt
  * @retval ITStatus The new state of USART_IT (SET or RESET).
  */
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT)
{
  ITStatus pendingbitstatus = RESET;
  uint8_t temp = 0;
  uint8_t itpos = 0;
  uint8_t itmask1 = 0;
  uint8_t itmask2 = 0;
  uint8_t enablestatus = 0;

  /* Check parameters */
  assert_param(IS_USART_GET_IT(USART_IT));

  /* Get the USART IT index */
  itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)USART_IT & (uint8_t)0x0F));
  /* Get the USART IT index */
  itmask1 = (uint8_t)((uint8_t)USART_IT >> (uint8_t)4);
  /* Set the IT mask*/
  itmask2 = (uint8_t)((uint8_t)1 << itmask1);

  /* Check the status of the specified USART pending bit*/
  if (USART_IT == USART_IT_PE)
  {
    /* Get the USART_IT enable bit status*/
    enablestatus = (uint8_t)((uint8_t)USARTx->CR1 & itmask2);
    /* Check the status of the specified USART interrupt*/

    if (((USARTx->SR & itpos) != (uint8_t)0x00) && enablestatus)
    {
      /* Interrupt occurred*/
      pendingbitstatus = SET;
    }
    else
    {
      /* Interrupt not occurred*/
      pendingbitstatus = RESET;
    }
  }

  else if (USART_IT == USART_IT_OR)
  {
    /* Get the USART_IT enable bit status*/
    enablestatus = (uint8_t)((uint8_t)USARTx->CR2 & itmask2);
    /* Check the status of the specified USART interrupt*/

    temp = (uint8_t)(USARTx->CR5 & USART_CR5_EIE);

    if (( (USARTx->SR & itpos) != 0x00) && ((enablestatus || temp)))
    {
      /* Interrupt occurred*/
      pendingbitstatus = SET;
    }
    else
    {
      /* Interrupt not occurred*/
      pendingbitstatus = RESET;
    }
  }

  else
  {
    /* Get the USART_IT enable bit status*/
    enablestatus = (uint8_t)((uint8_t)USARTx->CR2 & itmask2);
    /* Check the status of the specified USART interrupt*/
    if (((USARTx->SR & itpos) != (uint8_t)0x00) && enablestatus)
    {
      /* Interrupt occurred*/
      pendingbitstatus = SET;
    }
    else
    {
      /* Interrupt not occurred*/
      pendingbitstatus = RESET;
    }
  }

  /* Return the USART_IT status*/
  return  pendingbitstatus;
}

/**
  * @brief  Clears the USARTx’s interrupt pending bits.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_IT: specifies the interrupt pending bit to clear.
  *         This parameter can be one of the following values:
  *            @arg USART_IT_TC: Transmission complete interrupt.
  * @note   PE (Parity error), FE (Framing error), NE (Noise error),
  *         OR (OverRun error) and IDLE (Idle line detected) pending bits are
  *         cleared by software sequence: a read operation to USART_SR register
  *         (USART_GetITStatus()) followed by a read operation to USART_DR
  *         register (USART_ReceiveData8() or USART_ReceiveData9()).
  * @note   RXNE pending bit can be also cleared by a read to the USART_DR register
  *         (USART_ReceiveData8() or USART_ReceiveData9()).
  * @note   TC (Transmit complete) pending bit can be also cleared by software
  *         sequence: a read operation to USART_SR register (USART_GetITStatus())
  *         followed by a write operation to USART_DR register (USART_SendData8()
  *         or USART_SendData9()).
  * @note   TXE pending bit is cleared only by a write to the USART_DR register
  *         (USART_SendData8() or USART_SendData9()).
  * @retval None
  */
void USART_ClearITPendingBit(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT)
{
  assert_param(IS_USART_CLEAR_IT(USART_IT));

  /*< Clear RXNE or TC pending bit */
  USARTx->SR &= (uint8_t)(~USART_SR_TC);
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

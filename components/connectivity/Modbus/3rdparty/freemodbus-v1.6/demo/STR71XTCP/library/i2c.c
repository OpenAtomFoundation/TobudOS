/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : i2c.c
* Author             : MCD Application Team
* Date First Issued  : 09/05/2003
* Description        : This file provides Code sources I2C functions
********************************************************************************
* History:
*  24/05/2005 : V3.0
*  30/11/2004 : V2.0
*  14/07/2004 : V1.3
*  01/01/2004 : V1.2
*******************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "i2c.h"
#include "rccu.h"
#include <stdarg.h>

/*******************************************************************************
* Function Name  : I2C_Init
* Description    : Initializes I2C peripheral control and registers to their
*                  default reset values.
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : None.
*******************************************************************************/
void I2C_Init (I2C_TypeDef *I2Cx)
{
  //Initialize all the register of the specified I2C passed as parameter
  I2Cx->CR=0x0;
  I2Cx->CCR=0x0;
  I2Cx->ECCR=0x0;
  I2Cx->OAR1=0x0;
  I2Cx->OAR2=0x0;
  (void)I2Cx->SR1;
  (void)I2Cx->SR2;
  I2Cx->DR=0x0;
}

/*******************************************************************************
* Function Name  : I2C_OnOffConfig
* Description    : Enables or disables I2C peripheral.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  condition(ENABLE or DISABLE).
* Return         : None.
*******************************************************************************/
void I2C_OnOffConfig (I2C_TypeDef *I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE)
    {
      // Enable the I2C selected by setting twice the PE bit on the CR register
      I2Cx->CR |= I2C_PESET_Mask;
      I2Cx->CR |= I2C_PESET_Mask;
    }
  else
      // Disable the I2C selected
      I2Cx->CR &= ~I2C_PESET_Mask;
}

/*******************************************************************************
* Function Name  : I2C_FlagStatus
* Description    : Checks whether any I2C Flag is set or not.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Access(DIRECT or INDIRECT)
*                  Flag : the flag to be read
*                  input 4: an (u8) variable needed in the case
*                                     of the INDIRECT access
* Return         : the NewState of the Flag (SET or RESET).
*******************************************************************************/
FlagStatus I2C_FlagStatus (I2C_TypeDef *I2Cx, RegisterAccess Access, I2C_Flags Flag, ...)
{
  u32 Tmp;
  
  if (Access == DIRECT)
    // Store in Tmp variable the register where is located the flag
    Tmp = I2C_GetStatus(I2Cx)&Flag;
  else
  { 
    va_list list;  
    //Get the fourth register
    va_start(list,Flag);
    Tmp = va_arg(list,u32);
    Tmp&=Flag;
  }
  //Return the Flag NewState
  return Tmp != 0 ? SET : RESET;
}

/*******************************************************************************
* Function Name  :  I2C_FlagClear
* Description    : Clears the I2C Flag passed as a parameter
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Flag : the flag to be read
*                  input3: an (u8) parameter needed in the case that the flag
*                         to be cleared need a write in one register
* Return         : None.
*******************************************************************************/
void I2C_FlagClear (I2C_TypeDef *I2Cx, I2C_Flags Flag, ...)
{
  u8 Tmp = (u8)*((u32 *)&Flag + sizeof(Flag));

  if (Flag==I2C_ADD10 || Flag==I2C_EVF || Flag==I2C_BERR || Flag==I2C_ARLO ||
      Flag==I2C_STOPF || Flag==I2C_AF  || Flag==I2C_ENDAD)
  //flags that need a read of the SR2 register to be cleared
  {
    // Read the SR2 register
    (void)I2Cx->SR2;
    // Two flags need a second step to be cleared
    switch (Flag)
    {
      case  I2C_ADD10: I2Cx->DR = Tmp; break;
      case  I2C_ENDAD: I2Cx->CR|=0x20; break;
    }
  }
  else if (Flag==I2C_SB || Flag==I2C_ADSL || Flag==I2C_BTF || Flag==I2C_TRA)
  //flags that need a read of the SR1 register to be cleared
  {
  	// Read the SR1 register
    (void)I2Cx->SR1;
    if (Flag==I2C_SB) I2Cx->DR=Tmp;
    else if (Flag==I2C_BTF || Flag==I2C_TRA) (void)I2Cx->DR;
  }
  else if ( Flag==I2C_M_SL || Flag==I2C_GCAL)
  //flags that need the PE bit to be cleared
  {
    I2C_OnOffConfig (I2Cx, DISABLE);
    I2C_OnOffConfig (I2Cx, ENABLE);
  }
}

/*******************************************************************************
* Function Name  : I2C_SpeedConfig
* Description    : Selects I2C clock speed and configures its corresponding mode.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Clock: I2C expected clock in Hertz.
* Return         : None.
*******************************************************************************/
void I2C_SpeedConfig (I2C_TypeDef *I2Cx, u32 Clock)
{
  u32 FCLK;
  u16 result;
  // Get the FCLK frequency using the RCCU library
  FCLK = RCCU_FrequencyValue ( RCCU_FCLK );
  // Test on speed mode
  // Update the CCR and ECCR are updated
  if (Clock <=100000)
  // Standard mode selected
  {
    result = ((FCLK / Clock) - 7)/2;
    //Clear FM/SM bit
    I2Cx->CCR=result &0x7f;
  }
  else if (Clock <=400000)
  {
    // Fast mode selected
    result = ((FCLK/Clock)-9)/3;
    //set FM/SM bit
    I2Cx->CCR=result |0x80;
  }
  I2Cx->ECCR= result >>7;
}
/*******************************************************************************
* Function Name  : I2C_AddressConfig
* Description    : Defines the I2C bus address of the interface.
* Input          : I2Cx ( I2C0 or I2C1 ).
*                  Address: an u16 parameter indicating the address
*                           of the interface.
*                  Mode (I2C_Mode10,I2C_Mode7).
* Return         : None.
*******************************************************************************/
void I2C_AddressConfig (I2C_TypeDef *I2Cx, u16 Address, I2C_Addressing Mode)
{
  //Update OAR1 bit[7:1] by the lowest byte of address
  I2Cx->OAR1 = (u8)Address;

  if (Mode == I2C_Mode10)
    //Update Add8 and add9 bits in OAR2
    I2Cx->OAR2 |= (Address & 0x0300)>>7;
}

/*******************************************************************************
* Function Name  : I2C_FCLKConfig
* Description    : Configures frequency bits according to RCLK frequency.
*                  the selected I2C  must be disabled
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : None.
*******************************************************************************/
void I2C_FCLKConfig (I2C_TypeDef *I2Cx)
{
  u32 FCLK;
     // Get the FCLK frequency using the RCCU library
  FCLK = RCCU_FrequencyValue ( RCCU_FCLK );
     // Test the value of the FCLK and affect FR0,FR1&FR2 of the OAR2 register
  if (FCLK > 5000000)
    {
    if (FCLK <10000000)
      I2Cx->OAR2 |= 0x00;
    else if (FCLK <16670000)
      I2Cx->OAR2 |= 0x20;
    else if (FCLK < 26670000)
      I2Cx->OAR2 |= 0x40;
    else if (FCLK <40000000)
      I2Cx->OAR2 |= 0x60;
    else if (FCLK < 53330000)
      I2Cx->OAR2 |= 0x80;
    else if (FCLK < 66000000)
      I2Cx->OAR2 |= 0xA0;
    else if (FCLK <80000000)
      I2Cx->OAR2 |= 0xC0;
    else if (FCLK <100000000)
      I2Cx->OAR2 |= 0xE0;
   }
}

/*******************************************************************************
* Function Name  : I2C_AddressSend
* Description    : Transmits the address byte to select the slave device.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Address: an u16 parameter indicating the slave address
*                  Mode (I2C_Mode10,I2C_Mode7).
*                  Direction (I2C_RX,I2C_TX).
* Return         : None.
********************************************************************************/
void I2C_AddressSend (I2C_TypeDef *I2Cx, u16 Address, I2C_Addressing Mode, I2C_Direction Direction)
{
  if (Mode == I2C_Mode10 )
  //10 bit addressing mode
  {
    // Update the DR register by generated header
    I2Cx->DR = ((Address>>7)|0xf0)&0xfe;
    //Wait till I2C_ADD10 flag is set
    while ((I2Cx->SR1&0x40)==0);
    //clear I2C_ADD10 flag
    (void)I2Cx->SR2;
    I2Cx->DR=(u8)Address;
    //Test on the direction to define the read/write bit
    if (Direction == I2C_RX)
    {
      //Wait till I2C_ENDAD flag is set
      while ((I2Cx->SR2&0x20)==0);
      I2Cx->CR|=0x20;
      //Repeated START Generate
      I2C_STARTGenerate (I2Cx, ENABLE);
      //Test on SB flag status
      while ((I2Cx->SR1&0x01)==0);
      I2Cx->DR = ((Address>>7)|0xf1);
    }
  }
  else
  // 7 bit addressing mode
  {
    if (Direction == I2C_RX) Address|=0x01; else Address&=~0x01;
    I2Cx->DR=(u8)Address;
  }
}

/*******************************************************************************
* Function Name  : I2C_ByteSend
* Description    : Send a single byte of data.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Data : the byte to be sent to the slave
* Return         : None.
*******************************************************************************/
void I2C_ByteSend (I2C_TypeDef *I2Cx, u8 Data)
{
  //Write in the DR register the byte to be sent
  I2Cx->DR = Data;
}

/*******************************************************************************
* Function Name  : I2C_BufferSend
* Description    : Send data from a buffer whose number of bytes is known
* Input          : I2Cx ( I2C0 or I2C1 )
*                  PtrToBuffer :pointer to the byte of buffer to be transmitted.
*                  NbOfBytes:Number of byte of the buffer
* Return         : I2C_Tx_Status :transmission status (I2C_TX_AF, I2C_TX_ARLO,
*                  I2C_TX_BERR,I2C_TX_DATA_OK)
*******************************************************************************/
I2C_Tx_Status I2C_BufferSend (I2C_TypeDef *I2Cx, u8 *PtrToBuffer, u8 NbOfBytes)
{
  u8 Interruption_Status;
  u8 SentBytes;
  u8 error;
  I2C_Tx_Status I2C_TXTMP;

  //Test of the interrupt status
  Interruption_Status=0;
  Interruption_Status=I2Cx->CR & 0x01;
  I2C_ITConfig (I2Cx, DISABLE);
  //Wait till I2C_BTF bit is set
  while ((I2Cx->SR1 & 0x08 )==0);
  SentBytes=0;
  while (SentBytes<NbOfBytes)
  {
    I2Cx->DR= *(PtrToBuffer+SentBytes);
    //Wait till I2C_BTF bit is set or error detected
    do
      error = I2Cx->SR2 & 0x16;
    while ((I2Cx->SR1 & 0x08)==0 && error==0);
    // In case of error exit
    if (error) break;
    // increment SentBytes counter
    SentBytes++;
  }
  if (error)
  {
    if (error & 0x10)
      // I2C_TX_AF if Acknowledge failure is detected
      I2C_TXTMP = I2C_TX_AF;
    if (error & 0x02)
      //I2C_TX_ARLO if the ARLO bit is set in the SR2 register
      I2C_TXTMP = I2C_TX_ARLO;
    if (error & 0x04)
      // I2C_TX_BERR if the BERR bit is set in the SR2 register
      I2C_TXTMP=  I2C_TX_BERR;
  }
  else
    //I2C_TX_DATA_OK to show that the buffer is well sent
    I2C_TXTMP= I2C_TX_DATA_OK;

  //Restore the interrupt status
  if (Interruption_Status==1)
  I2C_ITConfig (I2Cx, ENABLE);

  return I2C_TXTMP;
}

/*******************************************************************************
* Function Name  : I2C_StringSend
* Description    : Send data from a buffer
* Input          : I2Cx ( I2C0 or I2C1 )
*                  PtrToBuffer :pointer to the byte of string to be transmitted.
* Return         : I2C_Tx_Status :transmission status (I2C_TX_AF, I2C_TX_ARLO,
*                  I2C_TX_BERR,I2C_TX_DATA_OK)
*******************************************************************************/
I2C_Tx_Status I2C_StringSend (I2C_TypeDef *I2Cx, char *PtrToString)
{
  u8 NbOfBytes=0;
  // count the number of byte composing the string passed as parameter.
  while (*(PtrToString+NbOfBytes)!= '\0')
    NbOfBytes++;
  // call I2C_BufferSend function to execute the send part
  return I2C_BufferSend (I2Cx,(u8 *)PtrToString,NbOfBytes);
}

/*******************************************************************************
* Function Name  : I2C_TransmissionStatus
* Description    : Report the NewState of the transmission
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : I2C_Tx_Status :transmission status (I2C_TX_NO, I2C_TX_SB,
*                   I2C_TX_AF, I2C_TX_ARLO, I2C_TX_BERR,I2C_TX_ADD_OK,
*                   I2C_TX_DATA_OK,I2C_TX_ONGOING)
*******************************************************************************/
I2C_Tx_Status I2C_TransmissionStatus (I2C_TypeDef *I2Cx)
{
  u8 SR1value;
  u8 SR2value;
  I2C_Tx_Status NewState;

  SR1value = I2Cx->SR1;
  SR2value = I2Cx->SR2;
  if ((I2Cx->SR1&0x10)==0)
    NewState=I2C_TX_NO;
  else if (I2Cx->SR1&0x01)
    //I2C_SB bit is set
    NewState=I2C_TX_SB;
  else if ((SR2value & 0x10)&&(I2Cx->CR&0x04))
    //I2C_ACK &I2C_AF are both set
    NewState=I2C_TX_AF;
  else if (SR2value & 0x04)
    //I2C_ARLO is set in multimaster mode
    NewState=I2C_TX_ARLO;
  else if (SR2value & 0x02)
    //I2C_BERR bit is set
    NewState=I2C_TX_BERR;
  else if ((SR1value & 0x80)&& (I2Cx->SR2&0x20))
    //I2C_EVF and I2C_ENDAD are both set
    NewState=I2C_TX_ADD_OK;
  else if ((I2Cx->SR1&0x20)&& (I2Cx->SR1&0x08))
    //I2C_TRA and I2C_BTF are both set
    NewState=I2C_TX_DATA_OK;
  else
    NewState=I2C_TX_ONGOING;

  return NewState;
}

/*******************************************************************************
* Function Name  : I2C_ByteReceive
* Description    : Returns the received byte.
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : the byte received
*******************************************************************************/
u8 I2C_ByteReceive (I2C_TypeDef *I2Cx)
{
   //Wait till I2C_BTF bit is set
  while ((I2Cx->SR1 & 0x08)==0);
  return I2Cx->DR;
}

/*******************************************************************************
* Function Name  : I2C_BufferReceive
* Description    : received a buffer. and return the status of error.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  PtrToBuffer :pointer to the byte of buffer received.
*                  NbOfBytes:Number of byte to be received
* Return         : I2C_Rx_Status:the NewState of the reception (,I2C_RX_AF,
*                               I2C_RX_ARLO,I2C_RX_BERR, I2C_RX_DATA_OK)
*******************************************************************************/
I2C_Rx_Status I2C_BufferReceive (I2C_TypeDef *I2Cx, u8 *PtrToBuffer, u8 NbOfBytes)
{
  u8 Interruption_Status;
  u8 ReceivedBytes;
  u8 error;
  I2C_Rx_Status I2C_RXTMP;

  //Test of the interrupt status
  Interruption_Status=0;
  Interruption_Status=I2Cx->CR & 0x01;
  I2C_ITConfig (I2Cx, DISABLE);
  ReceivedBytes=0;
  while (ReceivedBytes<NbOfBytes)
  {
    do
      error = I2Cx->SR2 & 0x16;
    while ((I2Cx->SR1 & 0x08)==0 && !error);
    if (error==0)
    // No error detected
    {
      *(PtrToBuffer+ReceivedBytes) = I2Cx->DR;
      ReceivedBytes++;
    }
    else break;
  }
  if (error)
  {
    if (error & 0x10)
      // I2C_RX_AF if Acknowledge failure is detected
      I2C_RXTMP= I2C_RX_AF;
    else if (error & 0x02)
      // I2C_RX_ARLO if the ARLO bit is set in the SR2 register
      I2C_RXTMP=  I2C_RX_ARLO;
    else if (error & 0x04)
      // I2C_RX_BERR if the BERR bit is set in the SR2 register
      I2C_RXTMP=  I2C_RX_BERR;
  }
  else
    //I2C_RX_DATA_OK to show that the buffer is well sent
    I2C_RXTMP= I2C_RX_DATA_OK;

  //Restore the interrupt status
  if (Interruption_Status==1)
  I2C_ITConfig (I2Cx, ENABLE);

  return I2C_RXTMP;
}

/*******************************************************************************
* Function Name  :I2C_ReceptionStatus
* Description    : Report the reception NewState.
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : I2C_Rx_Status:the NewState of the reception ( I2C_RX_NO,
*                  I2C_RX_SB,I2C_RX_AF,I2C_RX_ARLO,I2C_RX_BERR,I2C_RX_ADD_OK,
*                  I2C_RX_DATA_OK, I2C_RX_ONGOING)
*******************************************************************************/
I2C_Rx_Status I2C_ReceptionStatus (I2C_TypeDef *I2Cx)
{
  u8 SR1value;
  u8 SR2value;
  I2C_Rx_Status NewState;
  SR1value= I2Cx->SR1;
  SR2value= I2Cx->SR2;

  if ((I2Cx->SR1&0x10) == 0)
    NewState=I2C_RX_NO;
  else if (I2Cx->SR1&0x01)
    //I2C_SB bit is set
    NewState=I2C_RX_SB;
  else if ((SR2value & 0x10) && (I2Cx->CR&0x04))
    //I2C_ACK &I2C_AF are both set
    NewState=I2C_RX_AF;
  else if (SR2value & 0x04)
    //I2C_ARLO is set
    NewState=I2C_RX_ARLO;
  else if (SR2value & 0x02)
    //I2C_BERR bit is set
    NewState=I2C_RX_BERR;
  else if ((SR1value & 0x80) && (I2Cx->SR1&0x08)==0)
    //2C_EVF is set & I2C_BTF is not set
    NewState=I2C_RX_ADD_OK;
  else if ((I2Cx->SR1&0x20)==0 && (I2Cx->SR1&0x08))
    //II2C_TRA is cleared & I2C_BTF is set
    NewState=I2C_RX_DATA_OK;
  else
  NewState=I2C_RX_ONGOING;
  return NewState;
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/

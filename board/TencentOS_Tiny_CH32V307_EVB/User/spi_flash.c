#include "spi_flash.h"
#include "debug.h"





/*********************************************************************
 * @fn      SPI1_ReadWriteByte
 *
 * @brief   SPI1 read or write one byte.
 *
 * @param   TxData - write one byte data.
 *
 * @return  Read one byte data.
 */
u8 SPI1_ReadWriteByte(u8 TxData)
{
	u8 i=0;

	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
	{
		i++;
		if(i>200)return 0;
	}

	SPI_I2S_SendData(SPI3, TxData);
	i=0;

	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET)
	{
		i++;
		if(i>200)return 0;
	}

	return SPI_I2S_ReceiveData(SPI3);
}

/*********************************************************************
 * @fn      SPI_Flash_Init
 *
 * @brief   Configuring the SPI for operation flash.
 *
 * @return  none
 */
void SPI_Flash_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	SPI_InitTypeDef  SPI_InitStructure={0};

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA/*|RCC_APB2Periph_SPI1*/|RCC_APB2Periph_GPIOB, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI3, ENABLE );


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_15); //SPI3_NSS

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );//SPI3_SCK

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOB, &GPIO_InitStructure );//SPI3_MISO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );//SPI3_MOSI

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI3, &SPI_InitStructure);

	SPI_Cmd(SPI3, ENABLE);
}

/*********************************************************************
 * @fn      SPI_Flash_ReadSR
 *
 * @brief   Read W25Qxx status register.
 *        ！！BIT7  6   5   4   3   2   1   0
 *        ！！SPR   RV  TB  BP2 BP1 BP0 WEL BUSY
 *
 * @return  byte - status register value.
 */
u8 SPI_Flash_ReadSR(void)
{
	u8 byte=0;

	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
	SPI1_ReadWriteByte(W25X_ReadStatusReg);
	byte=SPI1_ReadWriteByte(0Xff);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);

	return byte;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_SR
 *
 * @brief   Write W25Qxx status register.
 *
 * @param   sr - status register value.
 *
 * @return  none
 */
void SPI_FLASH_Write_SR(u8 sr)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
	SPI1_ReadWriteByte(W25X_WriteStatusReg);
	SPI1_ReadWriteByte(sr);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*********************************************************************
 * @fn      SPI_Flash_Wait_Busy
 *
 * @brief   Wait flash free.
 *
 * @return  none
 */
void SPI_Flash_Wait_Busy(void)
{
	while((SPI_Flash_ReadSR()&0x01)==0x01);
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Enable
 *
 * @brief   Enable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Enable(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI1_ReadWriteByte(W25X_WriteEnable);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Disable
 *
 * @brief   Disable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Disable(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI1_ReadWriteByte(W25X_WriteDisable);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*********************************************************************
 * @fn      SPI_Flash_ReadID
 *
 * @brief   Read flash ID.
 *
 * @return  Temp - FLASH ID.
 */
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;

	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
	Delay_Ms(10);
	SPI1_ReadWriteByte(W25X_ManufactDeviceID);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;
	Temp|=SPI1_ReadWriteByte(0xFF);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);

	return Temp;
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Sector
 *
 * @brief   Erase one sector(4Kbyte).
 *
 * @param   Dst_Addr - 0 ！！ 2047
 *
 * @return  none
 */
void SPI_Flash_Erase_Sector(u32 Dst_Addr)
{
	Dst_Addr*=4096;
  SPI_FLASH_Write_Enable();
  SPI_Flash_Wait_Busy();
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI1_ReadWriteByte(W25X_SectorErase);
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));
  SPI1_ReadWriteByte((u8)Dst_Addr);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
  SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Sector
 *
 * @brief   Erase amount sector(4Kbyte).
 *
 * @param   addr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  errcode
 */
int SPI_Flash_Erase(u32 Addr, int size)
{
    uint32_t begin;
    uint32_t end;
    int i;

    if (size < 0
        || Addr > SPI_FLASH_TOTAL_SIZE
        || Addr + size > SPI_FLASH_TOTAL_SIZE)
    {
        return -1;
    }

    begin = Addr / SPI_FLASH_SECTOR * SPI_FLASH_SECTOR;
    end = (Addr + size - 1) / SPI_FLASH_SECTOR * SPI_FLASH_SECTOR;

    for (i = begin; i <= end; i += SPI_FLASH_SECTOR)
    {
        SPI_Flash_Erase_Sector(i);
    }

    return 0;
}

/*********************************************************************
 * @fn      SPI_Flash_Read
 *
 * @brief   Read data from flash.
 *
 * @param   pBuffer -
 *          ReadAddr -Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 size)
{
 	u16 i;

  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI1_ReadWriteByte(W25X_ReadData);
  SPI1_ReadWriteByte((u8)((ReadAddr)>>16));
  SPI1_ReadWriteByte((u8)((ReadAddr)>>8));
  SPI1_ReadWriteByte((u8)ReadAddr);

  for(i=0;i<size;i++)
	{
		pBuffer[i]=SPI1_ReadWriteByte(0XFF);
  }

	GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*********************************************************************
 * @fn      SPI_Flash_Write_Page
 *
 * @brief   Write data by one page.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 size)
{
 	u16 i;

  SPI_FLASH_Write_Enable();
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI1_ReadWriteByte(W25X_PageProgram);
  SPI1_ReadWriteByte((u8)((WriteAddr)>>16));
  SPI1_ReadWriteByte((u8)((WriteAddr)>>8));
  SPI1_ReadWriteByte((u8)WriteAddr);

  for(i=0;i<size;i++)
	{
		SPI1_ReadWriteByte(pBuffer[i]);
	}

  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
	SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_Write_NoCheck
 *
 * @brief   Write data to flash.(need Erase)
 *          All data in address rang is 0xFF.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 size)
{
	u16 pageremain;

	pageremain=256-WriteAddr%256;

	if(size<=pageremain)pageremain=size;

	while(1)
	{
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);

		if(size==pageremain)
		{
			break;
		}
	 	else
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;
			size-=pageremain;

			if(size>256)pageremain=256;
			else pageremain=size;
		}
	}
}

/*********************************************************************
 * @fn      SPI_Flash_Write
 *
 * @brief   Write data to flash.(no need Erase)
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 size)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
 	u16 i;

	secpos=WriteAddr/4096;
	secoff=WriteAddr%4096;
	secremain=4096-secoff;

	if(size<=secremain)secremain=size;

	while(1)
	{
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);

		for(i=0;i<secremain;i++)
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;
		}

		if(i<secremain)
		{
			SPI_Flash_Erase_Sector(secpos);

			for(i=0;i<secremain;i++)
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];
			}

			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);

		}
		else{
			SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);
		}

		if(size==secremain){
			break;
		}
		else
		{
			secpos++;
			secoff=0;

		  pBuffer+=secremain;
			WriteAddr+=secremain;
		  size-=secremain;

			if(size>4096)
			{
				secremain=4096;
			}
			else
			{
				secremain=size;
			}
		}
	}
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Chip
 *
 * @brief   Erase all FLASH pages.
 *
 * @return  none
 */
void SPI_Flash_Erase_Chip(void)
{
  SPI_FLASH_Write_Enable();
  SPI_Flash_Wait_Busy();
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI1_ReadWriteByte(W25X_ChipErase);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
	SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_PowerDown
 *
 * @brief   Enter power down mode.
 *
 * @return  none
 */
void SPI_Flash_PowerDown(void)
{
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI1_ReadWriteByte(W25X_PowerDown);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
  Delay_Us(3);
}

/*********************************************************************
 * @fn      SPI_Flash_WAKEUP
 *
 * @brief   Power down wake up.
 *
 * @return  none
 */
void SPI_Flash_WAKEUP(void)
{
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
	SPI1_ReadWriteByte(W25X_ReleasePowerDown);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
	Delay_Us(3);
}



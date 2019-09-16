/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   eth
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "bsp_eth.h"
#include "main.h"
#include "tos.h"
#include "err.h"
#include "sys.h"
#include "ethernetif.h"

/* Global Ethernet handle */
ETH_HandleTypeDef heth;

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */

extern sys_sem_t ousem;
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
//    /* Enable Peripheral clock */
//    __HAL_RCC_ETH_CLK_ENABLE();

    /**ETH GPIO Configuration
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PB11     ------> ETH_TX_EN
    PG13     ------> ETH_TXD0
    PG14     ------> ETH_TXD1
    */
    GPIO_InitStruct.Pin = ETH_MDC_Pin|ETH_RXD0_Pin|ETH_RXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_REF_CLK_Pin|ETH_MDIO_Pin|ETH_CRS_DV_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_TX_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(ETH_TX_EN_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_TXD0_Pin|ETH_TXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN ETH_MspInit 1 */
  /* Enable the Ethernet global Interrupt */
  HAL_NVIC_SetPriority(ETH_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);

  /* Enable ETHERNET clock  */
  __HAL_RCC_ETH_CLK_ENABLE();
  /* USER CODE END ETH_MspInit 1 */
  }
}

static void Eth_Reset(void)
{
   /* PHY RESET: PI1 */
  GPIO_InitTypeDef GPIO_InitStructure;
  __HAL_RCC_GPIOI_CLK_ENABLE();

  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull  = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_Delay(5);
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ETH_CLK_DISABLE();

    /**ETH GPIO Configuration
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PB11     ------> ETH_TX_EN
    PG13     ------> ETH_TXD0
    PG14     ------> ETH_TXD1
    */
    HAL_GPIO_DeInit(GPIOC, ETH_MDC_Pin|ETH_RXD0_Pin|ETH_RXD1_Pin);

    HAL_GPIO_DeInit(GPIOA, ETH_REF_CLK_Pin|ETH_MDIO_Pin|ETH_CRS_DV_Pin);

    HAL_GPIO_DeInit(ETH_TX_EN_GPIO_Port, ETH_TX_EN_Pin);

    HAL_GPIO_DeInit(GPIOG, ETH_TXD0_Pin|ETH_TXD1_Pin);

  /* USER CODE BEGIN ETH_MspDeInit 1 */

  /* USER CODE END ETH_MspDeInit 1 */
  }
}

HAL_StatusTypeDef Bsp_Eth_Init(void)
{
  HAL_StatusTypeDef ret;

  uint8_t MACAddr[6] ;

  HAL_ETH_DeInit(&heth);

  Eth_Reset();

  ETH->DMABMR |= ETH_DMABMR_SR;

  /* Init ETH */
  MACAddr[0] = lwipdev.mac[0];
  MACAddr[1] = lwipdev.mac[1];
  MACAddr[2] = lwipdev.mac[2];
  MACAddr[3] = lwipdev.mac[3];
  MACAddr[4] = lwipdev.mac[4];
  MACAddr[5] = lwipdev.mac[5];
  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  heth.Init.PhyAddress = LAN8720_PHY_ADDRESS;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.RxMode = ETH_RXINTERRUPT_MODE;  // rx mode
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
  heth.Init.Speed = ETH_SPEED_100M;       //speed
  heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;

	/* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
  ret = HAL_ETH_Init(&heth);
	if (ret == HAL_OK)
		printf("eth hardware init sucess...\n");
  else
    printf("eth hardware init faild...\n");

  /* Initialize Tx Descriptors list: Chain Mode */
  HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);

  /* Initialize Rx Descriptors list: Chain Mode  */
  HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
    /* Enable MAC and DMA transmission and reception */
  return ret;
}


void ETH_IRQHandler(void)
{
    tos_knl_irq_enter();

    HAL_ETH_IRQHandler(&heth);

    tos_knl_irq_leave();
}

/**
  * @brief  Ethernet Rx Transfer completed callback
  * @param  heth: ETH handle
  * @retval None
  */
extern k_sem_t s_xSemaphore;
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  LED2_TOGGLE;

  tos_sem_post(&s_xSemaphore);

}

void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  ;
}

void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
    printf("eth err\n");
}

int LAN8720A_Init(void){
  HAL_StatusTypeDef hal_eth_init_status;
  //初始化bsp—eth
  hal_eth_init_status = Bsp_Eth_Init();
  /* USER CODE BEGIN ETH_MspInit 1 */
  /* Enable the Ethernet global Interrupt */
  HAL_NVIC_SetPriority(ETH_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);

  /* Enable ETHERNET clock  */
  __HAL_RCC_ETH_CLK_ENABLE();
  /* USER CODE END ETH_MspInit 1 */

  /* Enable MAC and DMA transmission and reception */
  HAL_ETH_Start(&heth);
  if (hal_eth_init_status == HAL_OK)
  {
     return 0;
  }
  else
  {
     return -1;
  }
}

int LAN8720A_SendPacket(struct pbuf *p)
{
  int errval;
  struct pbuf *q;

  uint8_t *buffer = (uint8_t *)(heth.TxDesc->Buffer1Addr);
  __IO ETH_DMADescTypeDef *DmaTxDesc;
  uint32_t framelength = 0;
  uint32_t bufferoffset = 0;
  uint32_t byteslefttocopy = 0;
  uint32_t payloadoffset = 0;
  DmaTxDesc = heth.TxDesc;
  bufferoffset = 0;
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  /* Is this buffer available? If not, goto error */
  if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
  {
    errval = ERR_USE;
    goto error;
  }

  /* copy frame from pbufs to driver buffers */
  for(q = p; q != NULL; q = q->next)
    {
      /* Get bytes in current lwIP buffer */
      byteslefttocopy = q->len;
      payloadoffset = 0;

      /* Check if the length of data to copy is bigger than Tx buffer size*/
      while( (byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE )
      {
        /* Copy data to Tx buffer*/
        memcpy( (uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset) );

        /* Point to next descriptor */
        DmaTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);

        /* Check if the buffer is available */
        if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
        {
          errval = ERR_USE;
          goto error;
        }

        buffer = (uint8_t *)(DmaTxDesc->Buffer1Addr);

        byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
        payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
        framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
        bufferoffset = 0;
      }

      /* Copy the remaining bytes */
      memcpy( (uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)q->payload + payloadoffset), byteslefttocopy );
      bufferoffset = bufferoffset + byteslefttocopy;
      framelength = framelength + byteslefttocopy;
    }

  /* Prepare transmit descriptors to give to DMA */
  HAL_ETH_TransmitFrame(&heth, framelength);

  errval = ERR_OK;

error:

  /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
  if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
  {
    /* Clear TUS ETHERNET DMA flag */
    heth.Instance->DMASR = ETH_DMASR_TUS;

    /* Resume DMA transmission*/
    heth.Instance->DMATPDR = 0;
  }

  return errval;
}

struct pbuf *LAN8720A_Receive_Packet(void)
{
  struct pbuf *p = NULL;
  struct pbuf *q = NULL;
  uint16_t len = 0;
  uint8_t *buffer;
  __IO ETH_DMADescTypeDef *dmarxdesc;
  uint32_t bufferoffset = 0;
  uint32_t payloadoffset = 0;
  uint32_t byteslefttocopy = 0;
  uint32_t i=0;

  /* get received frame */
  if (HAL_ETH_GetReceivedFrame(&heth) != HAL_OK)
  {
//    printf("receive frame faild\n");
    return NULL;
  }
  /* Obtain the size of the packet and put it into the "len" variable. */
  len = heth.RxFrameInfos.length;
  buffer = (uint8_t *)heth.RxFrameInfos.buffer;

//  printf("receive frame len : %d\n", len);

  if (len > 0)
  {
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  }

  if (p != NULL)
  {
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    bufferoffset = 0;
    for(q = p; q != NULL; q = q->next)
    {
      byteslefttocopy = q->len;
      payloadoffset = 0;

      /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
      while( (byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE )
      {
        /* Copy data to pbuf */
        memcpy( (uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

        /* Point to next descriptor */
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
        buffer = (uint8_t *)(dmarxdesc->Buffer1Addr);

        byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
        payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
        bufferoffset = 0;
      }
      /* Copy remaining data in pbuf */
      memcpy( (uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), byteslefttocopy);
      bufferoffset = bufferoffset + byteslefttocopy;
    }
  }

    /* Release descriptors to DMA */
    /* Point to first descriptor */
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for (i=0; i< heth.RxFrameInfos.SegCount; i++)
    {
      dmarxdesc->Status |= ETH_DMARXDESC_OWN;
      dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }

    /* Clear Segment_Count */
    heth.RxFrameInfos.SegCount =0;

  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
  {
    /* Clear RBUS ETHERNET DMA flag */
    heth.Instance->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    heth.Instance->DMARPDR = 0;
  }
  return p;
}

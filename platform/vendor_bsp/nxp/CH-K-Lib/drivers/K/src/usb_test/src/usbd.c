#include "usb.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "message_manage.h"

/* alloc BDT */
ALIGN(512) tBDT tBDTtable[NUMBER_OF_PHYSICAL_ENDPOINTS];

static CONTROL_TRANSFER transfer;
USB_DEVICE device;

//各个端点的数据缓冲区
uint8_t EP0_OUT_ODD_Buffer[MAX_PACKET_SIZE_EP0];
uint8_t EP0_OUT_EVEN_Buffer[MAX_PACKET_SIZE_EP0];
uint8_t EP0_IN_ODD_Buffer[MAX_PACKET_SIZE_EP0];
uint8_t EP0_IN_EVEN_Buffer[MAX_PACKET_SIZE_EP0];
uint8_t EP1_OUT_ODD_Buffer[MAX_PACKET_SIZE_EP1];
uint8_t EP1_OUT_EVEN_Buffer[MAX_PACKET_SIZE_EP1];
uint8_t EP1_IN_ODD_Buffer[MAX_PACKET_SIZE_EP1];
uint8_t EP1_IN_EVEN_Buffer[MAX_PACKET_SIZE_EP1];
uint8_t EP2_OUT_ODD_Buffer[MAX_PACKET_SIZE_EP2];
uint8_t EP2_OUT_EVEN_Buffer[MAX_PACKET_SIZE_EP2];
uint8_t EP2_IN_ODD_Buffer[MAX_PACKET_SIZE_EP2];
uint8_t EP2_IN_EVEN_Buffer[MAX_PACKET_SIZE_EP2];
uint8_t EP3_OUT_ODD_Buffer[MAX_PACKET_SIZE_EP3];
uint8_t EP3_OUT_EVEN_Buffer[MAX_PACKET_SIZE_EP3];
uint8_t EP3_IN_ODD_Buffer[MAX_PACKET_SIZE_EP3];
uint8_t EP3_IN_EVEN_Buffer[MAX_PACKET_SIZE_EP3];
//指向各个缓冲区的地址指针
uint8_t *BufferPointer[]=
{
    EP0_OUT_ODD_Buffer,
    EP0_OUT_EVEN_Buffer,
    EP0_IN_ODD_Buffer,
    EP0_IN_EVEN_Buffer,
    EP1_OUT_ODD_Buffer,
    EP1_OUT_EVEN_Buffer,
    EP1_IN_ODD_Buffer,
    EP1_IN_EVEN_Buffer,
    EP2_OUT_ODD_Buffer,
    EP2_OUT_EVEN_Buffer,
    EP2_IN_ODD_Buffer,
    EP2_IN_EVEN_Buffer,
    EP3_OUT_ODD_Buffer,
    EP3_OUT_EVEN_Buffer,
    EP3_IN_ODD_Buffer,
    EP3_IN_EVEN_Buffer
};
//每个缓冲区大小
const uint32_t cEP_Size[]=
{
    MAX_PACKET_SIZE_EP0,    
    MAX_PACKET_SIZE_EP0,    
    MAX_PACKET_SIZE_EP0,    
    MAX_PACKET_SIZE_EP0,    
    MAX_PACKET_SIZE_EP1,    
    MAX_PACKET_SIZE_EP1,    
    MAX_PACKET_SIZE_EP1,    
    MAX_PACKET_SIZE_EP1,    
    MAX_PACKET_SIZE_EP2,    
    MAX_PACKET_SIZE_EP2,    
    MAX_PACKET_SIZE_EP2,    
    MAX_PACKET_SIZE_EP2,    
    MAX_PACKET_SIZE_EP3,
    MAX_PACKET_SIZE_EP3,
    MAX_PACKET_SIZE_EP3,
    MAX_PACKET_SIZE_EP3
};



//SETUP包后面只能跟DATA0

void USBD_DecodeSetupPacket(uint8_t *data, SETUP_PACKET *packet)
{
    /* Fill in the elements of a SETUP_PACKET structure from raw data */
    packet->bmRequestType.dataTransferDirection = (data[0] & 0x80) >> 7;
    packet->bmRequestType.Type = (data[0] & 0x60) >> 5;
    packet->bmRequestType.Recipient = data[0] & 0x1f;
    packet->bRequest = data[1];
    packet->wValue = (data[2] | (uint16_t)data[3] << 8);
    packet->wIndex = (data[4] | (uint16_t)data[5] << 8);
    packet->wLength = (data[6] | (uint16_t)data[7] << 8);
}


uint32_t USB_GetPhyEPNumber(void)
{
    return ARRAY_SIZE(USB0->ENDPOINT);
}


void USBD_EPRead(uint8_t ep, uint8_t *buf, uint32_t *bytesRead)
{
    uint32_t i, sz;
    uint8_t *p;
    ep *= 4;
    sz = tBDTtable[ep].byte_count;
    
    *bytesRead = sz;
    p = (uint8_t*)tBDTtable[ep].address;
    while(sz--)
    {
        *buf++ = *p++; 
    }
}

void USBD_EPWrite(uint8_t ep, uint8_t *buf, uint8_t len)
{
    uint8_t *p;
    uint8_t transfer_size;     //端点的数据长度 
    static uint8_t *pData;
    static uint8_t remain;
    static bool new_transfer = true;
    
    transfer_size = 0;
    
    ep *= 4;
    ep += 2; //EP值加2
    p = BufferPointer[ep];
  
    /* a new transfer */
    if(new_transfer)
    {
        pData = buf;
        remain = len;

        if((device.setup_pkt.wLength < len) && (ep == 2))
        {
            remain = device.setup_pkt.wLength;
        }
    }
    
    if(remain > cEP_Size[ep])
    {
        transfer_size = cEP_Size[ep];
        remain -= cEP_Size[ep];
        new_transfer = false;
    }
    else
    {
        transfer_size = remain;
        remain = 0;            
        new_transfer = true;
    }
    
    tBDTtable[ep].byte_count=(transfer_size);
    while(transfer_size--)
    {
        *p++ = *pData++; //将用户的数据赋值给EP存储区   
    }
    
    /* DATA0 DATA1 transfer */
    static uint8_t data1 = kUDATA0;
    data1 ^= 0x40;
    tBDTtable[ep].Stat.info = data1;
}


uint16_t USB_GetPacketSize(uint8_t ep)
{
    uint8_t size; 
    size = tBDTtable[ep << 2].byte_count;
    return(size & 0x03FF);
}



void USB_EnableInterface(void)
{
    uint32_t i, ep_num;
    
    /* enable all endpoint */
    ep_num = USB_GetPhyEPNumber();
    for(i = 0; i < ep_num;i++)
    {
        USB0->ENDPOINT[i].ENDPT |= (USB_ENDPT_EPHSHK_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK);
    }

    tBDTtable[bEP2IN_ODD].Stat.info= kUDATA1;
    tBDTtable[bEP2IN_ODD].byte_count = 0x00;
    tBDTtable[bEP2IN_ODD].address =(uint32_t  )EP2_IN_ODD_Buffer;            

    tBDTtable[bEP3OUT_ODD].Stat.info= kUDATA0;
    tBDTtable[bEP3OUT_ODD].byte_count = 0xFF;
    tBDTtable[bEP3OUT_ODD].address =(uint32_t)EP3_OUT_ODD_Buffer;            
}


void USB_EP0_OUT_Handler(SETUP_PACKET *packet)
{
    uint32_t i, size;
    uint8_t *p;
    for(i=0;i<8;i++)
    {
        //USB_DEBUG_LOG(USB_DEBUG_EP0, ("0x%X ", *p++));
    }
    //USB_DEBUG_LOG(USB_DEBUG_EP0, ("~~0x%X\r\n", packet->bmRequestType));
    
	if(packet->bmRequestType.dataTransferDirection == DEVICE_TO_HOST)
	{
        switch(packet->bmRequestType.Type)
        {
			case STANDARD_TYPE:
				switch(packet->bRequest)
				{
					case GET_CONFIGURATION:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get config\r\n"));
						break;	
					case GET_DESCRIPTOR:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get desc - "));
                        p = USBD_GetStdDesc(packet, &size);
                        USBD_EPWrite(EP0, p, size);
						break;
					case GET_INTERFACE:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get interface\r\n"));
						break;
					case GET_STATUS:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get status\r\n"));
						break;
                    default:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: unknown %d\r\n", packet->bRequest));
						break;
				}
				break;
			case CLASS_TYPE:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb class request\r\n"));
				break;
			case VENDOR_TYPE:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb vender request\r\n"));
				break;
			default:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("unknown request\r\n"));
				break;
		}	
	}
	else
	{
		switch(packet->bmRequestType.Type)
		{
			case STANDARD_TYPE:
				switch(packet->bRequest)
				{
					case CLEAR_FEATURE:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: clear feature\r\n"));
						break;
					case SET_ADDRESS:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: get addr:%d\r\n", packet->wValue));
                        device.state = ADDRESS;
						USBD_EPWrite(EP0, 0, 0);
						break;
					case SET_CONFIGURATION:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set config\r\n"));
                        USB_EnableInterface();
                        USBD_EPWrite(EP0, 0, 0);
                        device.state = CONFIGURED;
						break;
					case SET_DESCRIPTOR:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set desc\r\n"));
						break;
					case SET_INTERFACE:
						USB_DEBUG_LOG(USB_DEBUG_MIN, ("set standrd request: set interface\r\n"));
						break;
					default:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: unknown! %d\r\n", packet->bRequest));
					break;
				}
				break;
				case CLASS_TYPE:
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: usb request\r\n"));
				/*
					switch(Setup_Pkt->bRequest)
					{
						case 0x0A:
							USBD_EPWrite(EP0,0,0); //等待，发送0数据包即可
							UART_printf("设置空闲\r\n");
							break;
						default:
							UART_printf("未知的请求\r\n");
							break;
					}
				*/
					break;
				case VENDOR_TYPE:
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: usb vender request\r\n"));
					break;
				default:
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: unknown request\r\n"));
				break;
		}
	}
    
	tBDTtable[bEP0OUT_ODD].Stat.info= kUDATA0;
    USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
}


uint8_t USB_Init(void)
{
    device.state = DEFAULT;
    
    /* enable USB clock */
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    
    /* disable flash protect */
	FMC->PFAPR |= (FMC_PFAPR_M4AP_MASK);
    
    /* enable USB reguator */
	SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    
    /* disable memory protection */
    #ifdef MPU
    MPU->CESR=0;
    #endif
    
    /* reset USB moudle */
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while(USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK){};

    /* BDT adddress */
	USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);

    /* clear all IT bit */
    USB0->ISTAT |= 0xFF;
    
    /* enable USB reset IT */
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;
	USB0->USBTRC0 |= 0x40;

    /* enable USB moudle */
	USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    
    /* enable pull down reisger */
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
    
	fn_queue_init();
    
	return 0;
}


void USB_BusResetHandler(void)
{
    uint32_t i, phy_ep;
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb reset\r\n"));

    phy_ep = USB_GetPhyEPNumber();
    
    for(i = 1; i < phy_ep; i++)
    {
        USB0->ENDPOINT[i].ENDPT=0x00;
    }

    /*端点0 BDT 启动端点设置*/
    tBDTtable[bEP0OUT_ODD].byte_count = MAX_PACKET_SIZE_EP0;
    tBDTtable[bEP0OUT_ODD].address =(uint32_t)EP0_OUT_ODD_Buffer;
    tBDTtable[bEP0OUT_ODD].Stat.info = kUDATA1;   
   
    tBDTtable[bEP0OUT_EVEN].byte_count = MAX_PACKET_SIZE_EP0;
    tBDTtable[bEP0OUT_EVEN].address =(uint32_t)EP0_OUT_EVEN_Buffer;
    tBDTtable[bEP0OUT_EVEN].Stat.info = kUDATA1;     
   
    tBDTtable[bEP0IN_ODD].byte_count = MAX_PACKET_SIZE_EP0;
    tBDTtable[bEP0IN_ODD].address =(uint32_t)EP0_IN_ODD_Buffer;      
    tBDTtable[bEP0IN_ODD].Stat.info = kUDATA0;
   
    tBDTtable[bEP0IN_EVEN].byte_count = MAX_PACKET_SIZE_EP0; 
    tBDTtable[bEP0IN_EVEN].address =(uint32_t)EP0_IN_EVEN_Buffer;      
    tBDTtable[bEP0IN_EVEN].Stat.info = kUDATA0;

    /* reset all BDT to even */
    USB0->CTL |= USB_CTL_ODDRST_MASK;

    USB0->ENDPOINT[0].ENDPT = 0x0D;
    
    /* enable and clear all error states */
    USB0->ERRSTAT=0xFF;
    USB0->ERREN=0xFF;
    
    /* reset USB moudle addr */
    USB0->ADDR=0x00;
    
    /* clear all interrupts status */
    USB0->ISTAT=0xFF;
    
    /* enable USB module interrupts */
    USB0->INTEN |= USB_INTEN_TOKDNEEN_MASK;
    //USB0->INTEN |= USB_INTEN_SOFTOKEN_MASK;
    USB0->INTEN |= USB_INTEN_ERROREN_MASK;
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;  
}



void USB_EP0_IN_Handler(SETUP_PACKET *packet)
{
    if(device.state == ADDRESS)
    {
        USB0->ADDR = packet->wValue & 0xFF;
        device.state = CONFIGURED;
        USB_DEBUG_LOG(USB_DEBUG_EP0, ("new addr:%d\r\n", USB0->ADDR));
    }
    USBD_EPWrite(EP0, 0, 0); 
}

void USBD_StallEP(uint8_t ep)
{
    USB0->ENDPOINT[ep].ENDPT |= USB_ENDPT_EPSTALL_MASK;
}



static  MessageType_t m_Msg;


void USB_EPCallback(uint8_t ep, uint8_t dir)
{
     printf("EP%d DIR%d\r\n", ep, dir);

    if((ep == 2) && (dir == kUSB_IN))
    {
        m_Msg.m_Command = USB_DEVICE_CLASS;
        m_Msg.m_MessageType = kUSB_IN;
        fn_msg_push(m_Msg);
        // tBDTtable[bEP2IN_ODD].Stat.info= kUDATA0;
//        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    }
    if((ep == 3) && (dir == kUSB_OUT))
    {
        m_Msg.m_Command = USB_DEVICE_CLASS;
        m_Msg.m_MessageType = kUSB_OUT;
        m_Msg.m_MsgLen = USB_GetPacketSize(EP3);
        m_Msg.pMessage = BufferPointer[bEP3OUT_ODD];
        fn_msg_push(m_Msg);
        tBDTtable[EP3<<2].Stat.info= kSIE;
        tBDTtable[bEP3OUT_ODD].byte_count = MAX_PACKET_SIZE_EP3;
    }
    
}

void USB_TokenDone(void)
{
    uint8_t num;
    uint8_t dir;
    uint8_t buf[MAX_PACKET_SIZE_EP0];
    
    /* get endpoint direction and ep number */
    (USB0->STAT & USB_STAT_TX_MASK)?(dir = kUSB_IN):(dir = kUSB_OUT);
    num = ((USB0->STAT & USB_STAT_ENDP_MASK )>> USB_STAT_ENDP_SHIFT);
    
    uint32_t sz;
    

    
    /* EP0 Handling */
	if(num == 0)
	{
        USBD_EPRead(num, buf, &sz);
        USBD_DecodeSetupPacket(buf, &device.setup_pkt);
        
        if(!device.ClassSetup(&device.setup_pkt))
        {
            return;
        }
        
		if(dir == kUSB_IN)
		{
			USB_EP0_IN_Handler(&device.setup_pkt);
		}
		else
		{
			USB_EP0_OUT_Handler(&device.setup_pkt);
		}
	}
    else
    {
        /* other ep callback */
        USB_EPCallback(num, dir);
    }

}


void USB0_IRQHandler(void)
{    
    uint32_t istr, num, dir, ev_odd, stat;

    istr  = USB0->ISTAT;
    stat  = USB0->STAT;
    istr &= USB0->INTEN;
    
    if(istr & USB_ISTAT_USBRST_MASK)
    {
        USB_BusResetHandler();
    }
	else if(istr & USB_ISTAT_SOFTOK_MASK) 
	{
        USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb sof\r\n"));
		USB0->ISTAT |= USB_ISTAT_SOFTOK_MASK;   
	}
	else if(istr & USB_ISTAT_STALL_MASK)
	{
		USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb stall\r\n"));
        
        if(USB0->ENDPOINT[0].ENDPT & USB_ENDPT_EPSTALL_MASK)
        {
            USB0->ENDPOINT[0].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;
        }
    
        USB0->ISTAT |= USB_ISTAT_STALL_MASK;
	}
	else if(istr & USB_ISTAT_TOKDNE_MASK) 
	{
		USB_TokenDone();
        USB0->ISTAT |= USB_ISTAT_TOKDNE_MASK;
	}
	else if(istr & USB_ISTAT_SLEEP_MASK) 
	{
		USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb sleep\r\n"));
        USB0->ISTAT |= USB_ISTAT_SLEEP_MASK;      
	}
	else if(istr & USB_ISTAT_ERROR_MASK)
	{
        uint8_t err = USB0->ERRSTAT;
        USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb error:%0x%X\r\n", err));
        
        USB0->ISTAT |= USB_ISTAT_ERROR_MASK;
        USB0->ERRSTAT = 0xFF;
	}
    else
    {
        USB_DEBUG_LOG(USB_DEBUG_MIN, ("Unknown USB interrupt\r\n"));
    }
}


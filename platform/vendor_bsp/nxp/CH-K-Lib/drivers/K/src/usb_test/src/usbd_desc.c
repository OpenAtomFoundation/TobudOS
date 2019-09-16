#include "usb_desc.h"
#include "usb.h"

#define VENDOR_ID               (0x1234)
#define PRODUCT_ID              (0x5678)
#define PRODUCT_RELEASE         (0x1234)
    
static uint8_t DeviceSesc[]= 
{
        DEVICE_DESCRIPTOR_LENGTH,       /* bLength */
        DEVICE_DESCRIPTOR,              /* bDescriptorType */
        LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
        MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
        0x00,                           /* bDeviceClass */
        0x00,                           /* bDeviceSubClass */
        0x00,                           /* bDeviceprotocol */
        MAX_PACKET_SIZE_EP0,            /* bMaxPacketSize0 */
        (uint8_t)(LSB(VENDOR_ID)),                 /* idVendor (LSB) */
        (uint8_t)(MSB(VENDOR_ID)),                 /* idVendor (MSB) */
        (uint8_t)(LSB(PRODUCT_ID)),                /* idProduct (LSB) */
        (uint8_t)(MSB(PRODUCT_ID)),                /* idProduct (MSB) */
        (uint8_t)(LSB(PRODUCT_RELEASE)),           /* bcdDevice (LSB) */
        (uint8_t)(MSB(PRODUCT_RELEASE)),           /* bcdDevice (MSB) */
        STRING_OFFSET_IMANUFACTURER,    /* iManufacturer */
        STRING_OFFSET_IPRODUCT,         /* iProduct */
        STRING_OFFSET_ISERIAL,          /* iSerialNumber */
        0x01                            /* bNumConfigurations */
};

static uint8_t Report_Descriptor[]=
{
 //每行开始的第一字节为该条目的前缀，前缀的格式为：
 //D7~D4：bTag。D3~D2：bType；D1~D0：bSize。以下分别对每个条目注释。
 //这是一个全局（bType为1）条目，选择用途页为普通桌面Generic Desktop Page(0x01)
 //后面跟一字节数据（bSize为1），后面的字节数就不注释了，
 //自己根据bSize来判断。
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 //这是一个局部（bType为2）条目，说明接下来的应用集合用途用于鼠标
 0x09, 0x02, // USAGE (Mouse)
 //这是一个主条目（bType为0）条目，开集合，后面跟的数据0x01表示
 //该集合是一个应用集合。它的性质在前面由用途页和用途定义为
 //普通桌面用的鼠标。
 0xa1, 0x01, // COLLECTION (Application)
 //这是一个局部条目。说明用途为指针集合
 0x09, 0x01, //   USAGE (Pointer)
 //这是一个主条目，开集合，后面跟的数据0x00表示该集合是一个
 //物理集合，用途由前面的局部条目定义为指针集合。
 0xa1, 0x00, //   COLLECTION (Physical)
 //这是一个全局条目，选择用途页为按键（Button Page(0x09)）
 0x05, 0x09, //     USAGE_PAGE (Button)
 //这是一个局部条目，说明用途的最小值为1。实际上是鼠标左键。
 0x19, 0x01, //     USAGE_MINIMUM (Button 1)
 //这是一个局部条目，说明用途的最大值为3。实际上是鼠标中键。
 0x29, 0x03, //     USAGE_MAXIMUM (Button 3)
 //这是一个全局条目，说明返回的数据的逻辑值（就是我们返回的数据域的值啦）
 //最小为0。因为我们这里用Bit来表示一个数据域，因此最小为0，最大为1。
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 //这是一个全局条目，说明逻辑值最大为1。
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 //这是一个全局条目，说明数据域的数量为三个。
 0x95, 0x03, //     REPORT_COUNT (3)
 //这是一个全局条目，说明每个数据域的长度为1个bit。
 0x75, 0x01, //     REPORT_SIZE (1)
 //这是一个主条目，说明有3个长度为1bit的数据域（数量和长度
 //由前面的两个全局条目所定义）用来做为输入，
 //属性为：Data,Var,Abs。Data表示这些数据可以变动，Var表示
 //这些数据域是独立的，每个域表示一个意思。Abs表示绝对值。
 //这样定义的结果就是，第一个数据域bit0表示按键1（左键）是否按下，
 //第二个数据域bit1表示按键2（右键）是否按下，第三个数据域bit2表示
 //按键3（中键）是否按下。
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 //这是一个全局条目，说明数据域数量为1个
 0x95, 0x01, //     REPORT_COUNT (1)
 //这是一个全局条目，说明每个数据域的长度为5bit。
 0x75, 0x05, //     REPORT_SIZE (5)
 //这是一个主条目，输入用，由前面两个全局条目可知，长度为5bit，
 //数量为1个。它的属性为常量（即返回的数据一直是0）。
 //这个只是为了凑齐一个字节（前面用了3个bit）而填充的一些数据
 //而已，所以它是没有实际用途的。
 0x81, 0x03, //     INPUT (Cnst,Var,Abs)
 //这是一个全局条目，选择用途页为普通桌面Generic Desktop Page(0x01)
 0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
 //这是一个局部条目，说明用途为X轴
 0x09, 0x30, //     USAGE (X)
 //这是一个局部条目，说明用途为Y轴
 0x09, 0x31, //     USAGE (Y)
 //这是一个局部条目，说明用途为滚轮
 0x09, 0x38, //     USAGE (Wheel)
 //下面两个为全局条目，说明返回的逻辑最小和最大值。
 //因为鼠标指针移动时，通常是用相对值来表示的，
 //相对值的意思就是，当指针移动时，只发送移动量。
 //往右移动时，X值为正；往下移动时，Y值为正。
 //对于滚轮，当滚轮往上滚时，值为正。
 0x15, 0x81, //     LOGICAL_MINIMUM (-127)
 0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
 //这是一个全局条目，说明数据域的长度为8bit。
 0x75, 0x08, //     REPORT_SIZE (8)
 //这是一个全局条目，说明数据域的个数为3个。
 0x95, 0x03, //     REPORT_COUNT (3)
 //这是一个主条目。它说明这三个8bit的数据域是输入用的，
 //属性为：Data,Var,Rel。Data说明数据是可以变的，Var说明
 //这些数据域是独立的，即第一个8bit表示X轴，第二个8bit表示
 //Y轴，第三个8bit表示滚轮。Rel表示这些值是相对值。
 0x81, 0x06, //     INPUT (Data,Var,Rel)
 //下面这两个主条目用来关闭前面的集合用。
 //我们开了两个集合，所以要关两次。bSize为0，所以后面没数据。
 0xc0,       //   END_COLLECTION
 0xc0        // END_COLLECTION
};
//通过上面的报告描述符的定义，我们知道返回的输入报告具有4字节。
//第一字节的低3位用来表示按键是否按下的，高5位为常数0，无用。
//第二字节表示X轴改的变量，第三字节表示Y轴的改变量，第四字节表示
//滚轮的改变量。我们在中断端点1中应该要按照上面的格式返回实际的
//鼠标数据。
///////////////////////////报告描述符完毕////////////////////////////
//字符串描述符0

static uint8_t LangIDDesc[] = 
{
        0x04,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        0x09,0x04,          /*bString Lang ID - 0x0409 - English*/
};

static uint8_t stringImanufacturerDesc[]=
{
        0x12,                                            /*bLength*/
        STRING_DESCRIPTOR,                               /*bDescriptorType 0x03*/
        'm',0,'b',0,'e',0,'d',0,'.',0,'o',0,'r',0,'g',0, /*bString iManufacturer - mbed.org*/
};

static uint8_t stringIproductDesc[]=
{
        0x16,                                                       /*bLength*/
        STRING_DESCRIPTOR,                                          /*bDescriptorType 0x03*/
        'U',0,'S',0,'B',0,' ',0,'D',0,'E',0,'V',0,'I',0,'C',0,'E',0 /*bString iProduct - USB DEVICE*/
};


static uint8_t qualiferDesc[]=
{
        0x0A,
        0x06,
        LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
        MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
        0x00,                           /* bDeviceClass */
        0x00,                           /* bDeviceSubClass */
        0x00,                           /* bDeviceprotocol */
        MAX_PACKET_SIZE_EP0,            /* bMaxPacketSize0 */
        0x01,                            /* bNumConfigurations */    
        0x00
};
    
//字符串描述符3
static uint8_t stringIserialDesc[] = 
{
        0x16,                                                           /*bLength*/
        STRING_DESCRIPTOR,                                              /*bDescriptorType 0x03*/
        '0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,    /*bString iSerial - 0123456789*/
};	 


//配置描述符
static uint8_t Configuration_Descriptor[9+9+9+7+7]= 
{
	//配置描述符
	0x09,		    //bLength域，配置描述符的长度：9字节
	0x02,		    //bDescriptorType域：0x02表示本描述符为配置描述符
	sizeof(Configuration_Descriptor)&0xFF,(sizeof(Configuration_Descriptor)>>8)&0xFF,	    //wTotalLength域，配置信息的总长度（包括配置、接口和端点）：32字节
	0x01,		    //bNumInterfaces域，该配置所支持的接口数（至少一个）：1
	0x01,		    //bConfigurationValue域，配置值：1 
	0x00,		    //iConfiguration域，配置字符串描述符索引：0
	0x80,		    //bmAttibutes域，配置的属性(具有总线供电、自供电及过程唤醒的特性) 
	                //位7：1-必须为1；位6：0-自供电；位5：0-不支持远程唤醒
	0x32,		    //MaxPower域，设备从总线提取的最大电流以2mA为单位：50*2mA＝100mA

	//接口描述符
	0x09,           //bLength域，接口描述符长度：9字节
	0x04,           //bDescriptorType域：0x04表示本描述符为接口描述符
	0x00,           //bInterfaceNumber域，接口号
	0x00,           //bAlternateSetting域，接口的可替换设置值
	0x02,           //bNumEndpoints域，接口使用的端点数（除端点0）：2
	0x03,           //bInterfaceClass域，接口所属的USB设备类：0xFF表示供应商自定义
	0x02,           //bInterfaceSubClass域，接口所属的USB设备子类：0xFF表示供应商自定义
	0x01,           //bInterfaceProtocol域，接口采用的USB设备类协议：0xFF表示供应商自定义
	0x00,           //iInterface域，接口字符串描述符的索引：0

 /******************HID描述符************************/
 //bLength字段。本HID描述符下只有一个下级描述符。所以长度为9字节。
 0x09,
 //bDescriptorType字段。HID描述符的编号为0x21。
 0x21,
 //bcdHID字段。本协议使用的HID2.0协议。注意低字节在先。
    LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
    MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
 //bCountyCode字段。设备适用的国家代码，这里选择为美国，代码0x21。
 0x21,
 //bNumDescriptors字段。下级描述符的数目。我们只有一个报告描述符。
 0x01,
 //bDescriptorType字段。下级描述符的类型，为报告描述符，编号为0x22。
 0x22,
 //bDescriptorLength字段。下级描述符的长度。下级描述符为报告描述符。
 sizeof(Report_Descriptor)&0xFF,
 (sizeof(Report_Descriptor)>>8)&0xFF,  
  
	 //端点OUT描述符
	0x07,           //bLength域，端点描述符长度：7字节
	0x05,           //bDescriptorType域：0x05表示本描述符为端点描述符
	0x82,           //bEndpointAddress域，端点号和传输方向：端点2、IN
	0x03,           //bmAttributes域，端点特性：数据端点、块传输
	0x40,0x00,      //wMaxPacketSize域，端点支持最大数据包长度：32字节          
	0x64,           //bInterval域，轮询间隔，以ms为单位。

	 //端点IN描述符
	0x07,           //bLength域，端点描述符长度：7字节
	0x05,           //bDescriptorType域：0x05表示本描述符为端点描述符
	0x03,           //bEndpointAddress域，端点号和传输方向：端点3、OUT   
	0x03,           //bmAttributes域，端点特性：数据端点、块传输
	0x40,0x00,      //wMaxPacketSize域，端点支持最大数据包长度：32字节        
	0x64,           //bInterval域，轮询间隔，以ms为单位。
};



uint8_t* USBD_GetStdDesc(SETUP_PACKET *packet, uint32_t *len)
{
    uint8_t *p;
    
    *len = 0;
    p = 0;
    
    switch(DESCRIPTOR_TYPE(packet->wValue))
	{
		case DEVICE_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("device desc\r\n"));
            p = (uint8_t*)DeviceSesc;
            *len = sizeof(DeviceSesc);
			break;
        
		case CONFIGURATION_DESCRIPTOR:
			USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("config desc\r\n"));
            p = (uint8_t*)Configuration_Descriptor;
            *len = sizeof(Configuration_Descriptor);
			break;
        
		case STRING_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc:"));
            switch (DESCRIPTOR_INDEX(packet->wValue))
			{
				case STRING_OFFSET_LANGID:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("language\r\n"));
                    p = (uint8_t*)LangIDDesc;
                    *len = sizeof(LangIDDesc);
					break;
				case STRING_OFFSET_IMANUFACTURER:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("vender\r\n"));
                    p = (uint8_t*)stringImanufacturerDesc;
                    *len = sizeof(stringImanufacturerDesc);
					break;
				case STRING_OFFSET_IPRODUCT:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("vender string\r\n"));
                    p = (uint8_t*)stringIproductDesc;
                    *len = sizeof(stringIproductDesc);
					break;
				case STRING_OFFSET_ISERIAL:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("vender serail number\r\n"));
                    p = (uint8_t*)stringIserialDesc;
                    *len = sizeof(stringIserialDesc);
					break;
				case STRING_OFFSET_ICONFIGURATION:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("congiuration\r\n"));
					break;
				case STRING_OFFSET_IINTERFACE:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("interface\r\n"));
					break;
				default: 
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("unknow:%d\r\n",DESCRIPTOR_INDEX(packet->wValue)));
					break; 
			}
			break;
        case REPORT_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report desc\r\n"));
            p = (uint8_t*)Report_Descriptor;
            *len = sizeof(Report_Descriptor);
            break;
        case QUALIFIER_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report qualifer\r\n"));
            p = (uint8_t*)qualiferDesc;
            *len = sizeof(qualiferDesc);
            break;
        
		default:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("desc:unknow%d\r\n",  DESCRIPTOR_TYPE(packet->wValue)));
			break;
    }
    return p;
}




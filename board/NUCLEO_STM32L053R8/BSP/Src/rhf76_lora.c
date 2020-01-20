#include "rhf76_lora.h"
#include "tos_k.h"

uint16_t rx_rd_index  = 0;
uint16_t rx_wr_index = 0;
uint16_t rx_count      = 0;
uint8_t g_rx_buf[UART_RXBUF_MAXSIZE];

void g_ring_buf(uint8_t in_data)
{
    g_rx_buf[rx_wr_index]=in_data;
    rx_wr_index++;
    rx_count++;

    if (rx_wr_index == UART_RXBUF_MAXSIZE)
    {
        rx_wr_index = 0;
    }

     /* Check for overflow */
    if (rx_count == UART_RXBUF_MAXSIZE)
    {
        rx_wr_index = 0;
        rx_count      = 0;
        rx_rd_index  = 0;
    }
}

static int get_rx_byte(void)
{
    int c = -1;

    __disable_irq();
    if (rx_count > 0)
    {
        c = g_rx_buf[rx_rd_index];

        rx_rd_index++;
        if (rx_rd_index == UART_RXBUF_MAXSIZE)
        {
            rx_rd_index = 0;
        }
        rx_count--;
    }
    __enable_irq();

    return c;
}


void drv_uart_send(char * buf, int len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, len,0xFFFF);
}

static int dev_uart_recv(uint8_t *buf, uint32_t len, uint32_t timeout)
{
      int c=0, i=0;

      do
      {
          c = get_rx_byte();
          if (c < 0)
          {
               timeout--;
               HAL_Delay(1);
          }
          else
          {
               buf[i++] = (char)c;
          }
      }while(i<len && timeout>0);

      return i;
}


bool rhf76_send_cmd(char * cmd, uint32_t len,char * reply1, char * reply2, uint32_t waittime)
{
    char buf[256] = {0};
    rx_rd_index = 0;
    rx_wr_index = 0;
    rx_count = 0;
    drv_uart_send((char*)cmd,len);
    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )
        return true;

    HAL_Delay( waittime );
    dev_uart_recv((uint8_t *)buf, 120, 20);
    printf("%s",buf);
    if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
        return ( ( bool ) strstr ( buf, reply1 ) ||
                     ( bool ) strstr ( buf, reply2 ) );

    else if ( reply1 != 0 )
        return ( ( bool ) strstr ( buf, reply1 ) );

    else
        return ( ( bool ) strstr ( buf, reply2 ) );
}


void rhf76_reset ( void )
{
	rhf76_send_cmd ( "AT+RESET\r\n", strlen("AT+RESET\r\n"),"OK", 0, 2500 );
}

void rhf76_set_id(ENUM_ID_TypeDef id_type,char *id)
{
	char cmd[64] = {0};
	switch(id_type)
	{
		case devaddr:
			snprintf(cmd, 64, "%s=\"%s\",\"%s\"\r\n", "AT+ID", "devaddr", id);
			break;
		case deveui:
			snprintf(cmd, 64, "%s=\"%s\",\"%s\"\r\n", "AT+ID", "deveui", id);
			break;
		case appeui:
			snprintf(cmd, 64, "%s=\"%s\",\"%s\"\r\n", "AT+ID", "appeui", id);
			break;
	}
}

void rhf76_set_key(ENUM_KEY_TypeDef key_type,char *key)
{
	char cmd[64] = {0};
	switch(key_type)
	{
		case appkey:
			snprintf(cmd, 64, "%s=\"%s\",%s\r\n", "AT+KEY", "appkey", key);
			break;
		case appskey:
			snprintf(cmd, 64, "%s=\"%s\",%s\r\n", "AT+KEY", "appskey", key);
			break;
		case nwkskey:
			snprintf(cmd, 64, "%s=\"%s\",%s\r\n", "AT+KEY", "nwkskey", key);
			break;
	}
}


void rhf76_set_class ( ENUM_CLASS_TypeDef type )
{
	switch( type )
	{
		case CLASS_A:
			rhf76_send_cmd ( "AT+CLASS=A\r\n", strlen("AT+CLASS=A\r\n"),"+CLASS: A", 0, 2500 );
			break;
		case CLASS_B:
			rhf76_send_cmd ( "AT+CLASS=B\r\n", strlen("AT+CLASS=B\r\n"),"+CLASS: B", 0, 2500 );
			break;
		case CLASS_C:
			rhf76_send_cmd ( "AT+CLASS=C\r\n", strlen("AT+CLASS=C\r\n"),"+CLASS: C", 0, 2500 );
			break;
		default:
			break;
	}
}

void rhf76_set_chanel(void)
{
	rhf76_send_cmd ( "at+ch=num,0-7\r\n", strlen("at+ch=num,0-7\r\n"),"NUM, 0-7", 0, 2500 );
}

void rhf76_set_adr(void)
{
	rhf76_send_cmd ( "at+adr=off\r\n", strlen("at+adr=off\r\n"),"OFF", 0, 2500 );
}
void rhf76_set_mode ( ENUM_MODE_TypeDef mode )
{
	switch( mode )
	{
		case LWOTAA:
			rhf76_send_cmd ( "AT+MODE=LWOTAA\r\n", strlen("AT+MODE=LWOTAA\r\n"),"+MODE: LWOTAA", 0, 2500 );
			break;
		case LWABP:
			rhf76_send_cmd ( "AT+MODE=LWABP\r\n", strlen("AT+MODE=LWABP\r\n"),"+MODE: LWABP", 0, 2500 );
			break;
		default:
			break;
	}
}

int32_t rhf76_join(void)
{
    if(rhf76_send_cmd("AT+join\r\n",strlen("AT+join\r\n"), "Network joined", "Done",14000))
    {
        return 0;
    }

    else
        return -1;
}

void rhf76_init(void)
{
    char *key = "2B7E151628AED2A6ABF7158809CF4F3C";
    char *appeui_id = "70B3D57ED00E0017";
    HAL_Delay(1000);
    printf("Init RHF76 LoRa ...\n" );
    rhf76_reset();
    printf("\n< 1 >\n");
    rhf76_set_class(CLASS_A);
    printf("\n< 2 >\n");
    rhf76_set_chanel();
    rhf76_set_adr();
    rhf76_set_mode(LWOTAA);
    rhf76_set_id(appeui,appeui_id);
    rhf76_set_key(appkey,key);
    printf("\n< 3 >\n");
    rhf76_join();
    HAL_Delay(3000);
    printf("Init RHF76 LoRa done\n");
}

int32_t rhf76_recv_timeout(int8_t * buf, uint32_t len,
                           int32_t timeout)
{
    return dev_uart_recv((uint8_t *)buf, len, timeout);
}

int32_t rhf76_recv( int8_t * buf, uint32_t len)
{
    return rhf76_recv_timeout(buf, len, 5000);
}

static char __num2hex(uint8_t num)
{
    if (num <= 0x9) {
        return num + '0';
    }

    if ((0xA <= num) && (num <= 0xF)) {
        return num - 0xA + 'A';
    }

    return (char)-1;
}

static void __hex2str(uint8_t *in, char *out, int len)
{
	int i = 0;

	for (i = 0; i < len; ++i) {
		out[i * 2] = __num2hex(in[i] >> 4);
		out[i * 2 + 1] = __num2hex(in[i] & 0x0F);
	}
	out[2 * len] = '\0';
}

int rhf76_send(const void  *buf, uint32_t len)
{
    char *str_buf = NULL;

    str_buf = tos_mmheap_calloc(2 * len + 1, sizeof(char));
    if (!str_buf) {
        return -1;
    }
    __hex2str((uint8_t *)buf, str_buf, len);

    char cmd[100] = {0};

    snprintf(cmd, sizeof(cmd), "AT+CMSGHEX=\"%s\"\r\n", str_buf);
    cmd[sizeof(cmd) - 1] = '\0';
    tos_mmheap_free(str_buf);

    rhf76_send_cmd((char *)cmd, strlen(cmd), (char *)"ACK Received","Done",10000);
    return len;
}

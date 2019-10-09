#include <stdlib.h>
#include <string.h>
#include "atcmd-board.h"
#include "board.h"
#include "at_parser.h"
#include "uart.h"
#include "LoRaApi.h"

extern Uart_t Uart1;
extern uint8_t DevEui[];
extern uint8_t JoinEui[];
extern uint8_t AppKey[];
extern uint8_t NwkKey[];
extern uint32_t DevAddr;
extern uint8_t FNwkSIntKey[];
extern uint8_t SNwkSIntKey[];
extern uint8_t NwkSEncKey[];
extern uint8_t AppSKey[];
volatile uint8_t g_DeepSleepFlag = 0;
uint8_t isspace(char x)
{
    if(x== ' ' || x== '\t' || x== '\n' || x== '\f' || x== '\b' || x== '\r')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*ASC码转整形*/
int asc2int(const uint8_t * asc, uint8_t len)
{
    uint8_t i = 0;
    char c;
    char sign;
    int total = 0;

    while(isspace(*asc))
    {
        ++asc;
        if(i++ >len)
        {
            break;
        }
    }

    c = *asc++;
    sign = c;
    if(c == '-' || c == '+')
    {
        c = *asc++;
    }

    for(i = 0;i < len;i++)
    {
        if(c <= '9' && c >= '0')
        {
            total = 10 * total + (c - '0');
            c = * asc++;
        }
        else
        {
            break;
        }
    }

    if(sign == '-')
    {
        return -total;
    }
    else
    {
        return total;
    }
}

uint16_t uart_receive(uint8_t *buf, uint16_t buf_len)
{
    uint16_t n = 0;
    UartGetBuffer(&Uart1, buf, buf_len, &n);
    return n;
}

static at_uart_ops_t uart_ops = {
    .init = NULL,   /* uart has already been inited*/
    .deinit = NULL,
    .send = NULL,
    .recv = uart_receive,
    .recv_timeout = uart_receive,
};

uint8_t (*func)(uint8_t *, uint16_t size);

uint8_t at_ver_handle(uint8_t *param, uint16_t size)
{
    printf("v1.0\r\n");
    return 0;
}

uint8_t at_model_handle(uint8_t *param, uint16_t size)
{
    printf("RHF76\r\n");
    return 0;
}

uint8_t at_reset_handle(uint8_t *param, uint16_t size)
{
    BoardResetMcu();
    return 0;
}

uint8_t at_sleep_handle(uint8_t *param, uint16_t size)
{
    /* FIXME */
		if(param[0] == '?')
		{
			printf("%d\r\n",g_DeepSleepFlag);
			return 0;
		}
		else
		{
			if(size > 1 || (param[0] != '0' && param[0] != '1'))
			{
					printf("PARAM ERROR\r\n");
					return 1;
			}
			uint32_t flag = asc2int(param, size);
			if(flag == 1 || flag == 0)
			{
					g_DeepSleepFlag = flag;
					printf("SLEEP MODE SET OK\r\n");
					return 0;
			}
			else
			{
					printf("PARAM ERROR\r\n");
					return 1;
			}
		}
}

uint8_t at_join_handle(uint8_t *param, uint16_t size)
{
    uint8_t type;
    MibRequestConfirm_t mibReq;

    if(param) {
        if(param[0] == '?') {
            mibReq.Type = MIB_NETWORK_ACTIVATION;
            LoRaMacMibGetRequestConfirm( &mibReq );

            if(mibReq.Param.NetworkActivation == ACTIVATION_TYPE_OTAA)
                printf("OTAA\r\n");
            else if(mibReq.Param.NetworkActivation == ACTIVATION_TYPE_ABP)
                printf("ABP\r\n");
            else
                printf("Not Joined!\r\n");

        } else {
							type = atoi((char *)param);
							LoRa_JoinNetwork(type);
							printf("Joined OK %d!\r\n",type);
        }
        return 0;
    }
    else
        return 1;
}

uint8_t at_tx_handle(uint8_t *param, uint16_t size)
{
    uint8_t i = 0;
    uint8_t j = 0;
    char buf[64] = {0};
    uint8_t type;
    uint8_t port;

    
    /* AT format: AT+TX=<type>,<port>,<data>*/
    /* extract <type> */
    for(i = 0, j = 0; param[i] != ','; i++, j++)
        buf[j] = param[i];
    buf[j+1] = '\0';
    
    type = atoi(buf);
    
    /* extract <port> */
    i++; /* skip ',' */
    for(j = 0; param[i] != ','; i++, j++)
        buf[j] = param[i];
    buf[j+1] = '\0';
    
    port = atoi(buf);
    
    i++;
    
    /* the left is data part */
    return Lora_Send(type, port, param + i, size - i);
}

static uint8_t __str2hex(uint8_t *str, uint8_t slen, uint8_t *hex, uint8_t hlen)
{
    char buf[3];
    uint8_t i   = 0;
    uint8_t j   = 0;
    uint8_t k   = 0;
    uint8_t val = 0;

    for(i = 0; i < slen; i++) {
        for(j = 0; j < 2; i++) {
            if(str[i] == ' ')
                continue;
            buf[j] = str[i];
            j++;
        }
        buf[j] = '\0';

        val = (uint8_t)strtol(buf, NULL, 16);

        if(k < hlen)
            hex[k++] = val;
        else
            return 0;
    }
    return 1;
}

uint8_t at_key_handle(uint8_t *param, uint16_t size)
{
    /*
     * AT+KEY=NWKSKEY, "2B7E151628AED2A6ABF7158809CF4F3C"
     * AT+KEY=APPSKEY, "2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C"
     * AT+KEY=APPKEY, "2B7E151628AED2A6ABF7158809CF4F3C"
     */
    uint8_t key[16];
    uint8_t i = 0;
    MibRequestConfirm_t mibReq;
    
    while(param[i++] != '\"');
    __str2hex(&param[i], size - i, key, 16);

    if(memcpy(param, "NWKSKEY", 7) == 0) {
        mibReq.Type = MIB_NWK_S_ENC_KEY;
        mibReq.Param.NwkSEncKey = key;
				memcpy1(NwkSEncKey,key,16);
    } else if (memcpy(param, "APPSKEY", 7)) {
        mibReq.Type = MIB_APP_S_KEY;
        mibReq.Param.AppSKey = key;
				memcpy1(AppSKey,key,16);
    } else if (memcpy(param, "APPKEY", 6)) {
        mibReq.Type = MIB_APP_KEY;
        mibReq.Param.AppKey = key;
				memcpy1(AppKey,key,16);
    } else {
        printf("ERROR\r\n");
        return 1;
    }
		printf_device_info();
    LoRaMacMibSetRequestConfirm( &mibReq );

    return 0;
}

static at_table_t g_at_cmd[]=
{
    /*通用指令*/
    {"AT+VERSION=?",    at_ver_handle},
    {"AT+MODEL=?",      at_model_handle},
    {"AT+RESET",        at_reset_handle},
    {"AT+SLEEP=",       at_sleep_handle},
    /*LORAWAN指令*/
    {"AT+JOIN=",        at_join_handle},
    {"AT+TX=",          at_tx_handle},
    {"AT+KEY=",         at_key_handle},
    {AT_CMD_END,        NULL},
};

int at_cmd_init()
{
    return at_parser_init(&uart_ops, g_at_cmd);
}

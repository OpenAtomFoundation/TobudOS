#if 0
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "console.h"
#include "syscon.h"
#include "hal_uart.h"
#include "sw_timer.h"
#include "timer_itf.h"
#include "hal_chipsleep.h"
#include "hal_gpio.h"
#include "hal_extint.h"
#include "rom_callback.h"
#include "cal_main.h"
#include "freertos.h"
#include "task.h"


ALARM_HANDLE_T* wakeup_alm;

typedef struct
{
    uint8_t enable;
    GPIO_Num io_num;
    GPIO_Value io_value;
}Awake_GPIO;

typedef struct
{
    uint8_t enable;
    EXTI_IntSrc trig_gpio;
    EXTI_Trig_Type trig_lvl;
}Wakeup_GPIO;

typedef struct
{
    Wakeup_GPIO exti_gpio;
    Awake_GPIO signal_gpio;
}AT_Wakeup_t;


AT_Wakeup_t wakeup_gpio;


typedef struct
{
    uint8_t gpio_hw_en;             // whether the hardware support gpio
    uint8_t gpio_valid;
}AT_Pin_WorkMode;

AT_Pin_WorkMode at_gpio[GPIO_NUM_MAX];
#endif

#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"

#include "atcmd/at_cmd_basic.h"
#include "atcmd/at_parser.h"
#include "hal/hal_syscon.h"
#include "hal/syscon_types.h"
//#include "console/console.h"

char at_at_excute(char *str)
{
    LOG(LOG_LVL_INFO,"AT excute\r\n");

    at_printf("OK\r\n");

    return AT_OK;
}

char at_ate0_excute(char *str)
{
    LOG(LOG_LVL_INFO,"ATE0 excute\r\n");

    console_echo_enable(0);

    at_printf("OK\r\n");

    return AT_OK;
}


char at_ate1_excute(char *str)
{
    LOG(LOG_LVL_INFO,"ATE1 excute\r\n");

    console_echo_enable(1);

    at_printf("OK\r\n");

    return AT_OK;
}



char at_rst_excute(char *str)
{
    LOG(LOG_LVL_INFO,"at_rst excute\r\n");

    HAL_SYSCON_SoftwareResetCore(SW_RST_CORE_ALL);


    return AT_OK;
}


char at_gmr_excute(char *str)
{
#if 0
    uint32_t rom_version;
    LOG(LOG_LVL_INFO,"at_gmr excute\n");
    rom_version = *(volatile uint32_t *)(0xcc);
    at_printf("%x\n",rom_version & 0x00ffffff);
#endif
    at_printf("OK\r\n");
    return AT_OK;
}


char at_gslp_excute(char *str)
{
#if 0
    LOG(LOG_LVL_INFO,"at_gslp excute\n");
    //    at_printf("atoi_%d\n",atoi(str));
    if( (*str>=49)&&(*str<=57))   //input must be non-zero number , ascii code of '1' is 49,ascii code of '9' is 57
    {
        wakeup_alm = create_alarm(NULL, 0);
        rtos_start_alarm(wakeup_alm,atoi(str));
    }
    else
    {
        at_printf("input error,input must be non-zero number \n");
    }
    at_printf("OK\n");
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;
}

char at_restore_excute(char *str)
{
#if 0
    LOG(LOG_LVL_INFO,"at_restore excute\n");


    at_printf("OK\n");
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;
}


char at_sleep_get(char *str)
{
#if 0
    SLEEP_Mode mode = Sleep_Get_SleepMode();

    LOG(LOG_LVL_INFO,"at_sleep get\n");

    at_printf("%d\n",mode);

    at_printf("OK\n");
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;
}


char at_sleep_set(char *str)
{
#if 0
    uint8_t sleep_mode;
    LOG(LOG_LVL_INFO,"at_sleep set\n");
    sleep_mode = atoi(str);
    if(sleep_mode > 1)
    {
        at_printf("ERROR\n");
        return AT_ERROR;
    }
    else
    {
        Sleep_Set_SleepMode((SLEEP_Mode)atoi(str));
        at_printf("OK\n");
    }
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;
}


char at_uart_cur_get(char *str)
{
#if 0
    char ret = AT_ERROR;
    LOG(LOG_LVL_INFO,"at_uart_cur get\n");
    extern UART_InitTypeDef console_config;
    uint8_t data_len;
    uint8_t stop_bit;
    switch(console_config.UART_DataLen)
    {

        case Data_Length_5_bits:
            data_len = 5;
            break;
        case Data_Length_6_bits:
            data_len = 6;
            break;
        case Data_Length_7_bits:
            data_len = 7;
            break;
        case Data_Length_8_bits:
            data_len = 8;
            break;
        default:
            ret = AT_ERROR;
    }

    if(console_config.UART_DataLen == Data_Length_5_bits)
    {
        if(console_config.UART_StopBits == stop_bit_1_5_OR_2)
            stop_bit = 2;
    }
    else
    {
        if(console_config.UART_StopBits == stop_bit_1_5_OR_2)
            stop_bit = 3;
        else
            stop_bit = 1;
    }

    at_printf("AT+UART=%d,%d,%d,%d,%d\n",console_config.UART_BaudRate,data_len,stop_bit,console_config.UART_Parity,console_config.FlowControl);
    ret = AT_OK;
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;

}


//uint8_t gpio_valid_list[GPIO_NUM_MAX];
void at_gpio_define(void)           //define which pin can be use as gpio
{
#if 0
    memset(at_gpio,0,GPIO_NUM_MAX*sizeof(AT_Pin_WorkMode));

    at_gpio[GPIO0_0].gpio_hw_en = 0;      //uart rxd
    at_gpio[GPIO0_1].gpio_hw_en = 0;      //uart txd
    at_gpio[GPIO0_2].gpio_hw_en = 0;      //swd
    at_gpio[GPIO0_3].gpio_hw_en = 0;      //swck
    at_gpio[GPIO0_4].gpio_hw_en = 1;
    at_gpio[GPIO0_5].gpio_hw_en = 1;
    at_gpio[GPIO0_6].gpio_hw_en = 1;
    at_gpio[GPIO0_7].gpio_hw_en = 1;
    at_gpio[GPIO0_8].gpio_hw_en = 1;
    at_gpio[GPIO0_9].gpio_hw_en = 0;      //boot mode
    at_gpio[GPIO0_10].gpio_hw_en = 0;     //boot mode
    at_gpio[GPIO0_11].gpio_hw_en = 0;     //boot mode
    at_gpio[GPIO0_12].gpio_hw_en = 1;
    at_gpio[GPIO0_13].gpio_hw_en = 1;
    at_gpio[GPIO0_14].gpio_hw_en = 1;
    at_gpio[GPIO0_15].gpio_hw_en = 1;
    at_gpio[GPIO0_16].gpio_hw_en = 1;
    at_gpio[GPIO0_17].gpio_hw_en = 0;     //debug host
    at_gpio[GPIO0_18].gpio_hw_en = 0;     //debug host
    at_gpio[GPIO0_19].gpio_hw_en = 1;
    at_gpio[GPIO0_20].gpio_hw_en = 1;     //gpio0[20]  

    at_gpio[GPIO1_0].gpio_hw_en = 1;      //gpio1[0]  
    at_gpio[GPIO1_1].gpio_hw_en = 1;
    at_gpio[GPIO1_2].gpio_hw_en = 1;
    at_gpio[GPIO1_3].gpio_hw_en = 1;

    at_gpio[GPIO1_4].gpio_hw_en = 0;      //qspi_clk
    at_gpio[GPIO1_5].gpio_hw_en = 0;      //qspi_cs  
    at_gpio[GPIO1_6].gpio_hw_en = 0;      //qspi_io0
    at_gpio[GPIO1_7].gpio_hw_en = 0;      //qspi_io1
    at_gpio[GPIO1_8].gpio_hw_en = 0;      //qspi_io2
    at_gpio[GPIO1_9].gpio_hw_en = 0;      //qspi_io3

    at_gpio[GPIO1_10].gpio_hw_en = 1;
    at_gpio[GPIO1_11].gpio_hw_en = 1;
    at_gpio[GPIO1_12].gpio_hw_en = 1;
    at_gpio[GPIO1_13].gpio_hw_en = 1;
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
}

#if 0
void exti_atwakeup_isr(AT_Wakeup_t *param)
{

    HAL_EXTI_IntClr(EXT_INT_1);
    LOG(LOG_LVL_INFO,"int_triggered\n");

    //    HAL_EXTI_Deinit(EXT_INT_1);
    if(param->signal_gpio.enable)
        HAL_GPIO_WritePin(param->signal_gpio.io_num,param->signal_gpio.io_value);

    at_printf("[%s, %d]\r\n", __func__, __LINE__);


}
#endif
#if 0
static void exti_atwakeup_setting(EXTI_IntSrc trig_gpio, EXTI_Trig_Type trig_lvl, GPIO_Num awake_GPIO, GPIO_Value value)
{

    EXTI_InitTypeDef config;
    config.int_en = true;
    config.src = trig_gpio;
    config.type = trig_lvl; 

    wakeup_gpio.exti_gpio.enable = 1;
    wakeup_gpio.exti_gpio.trig_gpio = trig_gpio;
    wakeup_gpio.exti_gpio.trig_lvl = trig_lvl;

    wakeup_gpio.signal_gpio.enable = 1;
    wakeup_gpio.signal_gpio.io_num = awake_GPIO;
    wakeup_gpio.signal_gpio.io_value = value;

    HAL_EXTINT_Register(exti_atwakeup_isr,&wakeup_gpio);
    HAL_EXTI_Init(config);

    GPIO_InitTypeDef wakeup_status_cfg;
    wakeup_status_cfg.dir = GPIO_OUTPUT;
    wakeup_status_cfg.value = (GPIO_Value)(!value);
    HAL_GPIO_Init(awake_GPIO,wakeup_status_cfg);

    //    NVIC_EnableIRQ(EXTERNAL_IRQn);            nvic should enable in intc.c

    at_printf("[%s, %d]\r\n", __func__, __LINE__);
}

static void exti_atwakeup_disable()
{

    wakeup_gpio.exti_gpio.enable = 0;
    wakeup_gpio.signal_gpio.enable = 0;

    HAL_EXTI_Deinit(wakeup_gpio.exti_gpio.trig_gpio);

    GPIO_InitTypeDef wakeup_status_cfg;
    wakeup_status_cfg.dir = GPIO_INPUT;
    wakeup_status_cfg.value = GPIO_VALUE_HIGH;
    HAL_GPIO_Init(wakeup_gpio.signal_gpio.io_num,wakeup_status_cfg);

    //    NVIC_EnableIRQ(EXTERNAL_IRQn);

    at_printf("[%s, %d]\r\n", __func__, __LINE__);
}
#endif

//AT+WAKEUPGPIO=1,1,1,5,0               //wakeup_en,extint_1,low_level,GPIO0_5,gpio_low
char at_wakeupgpio_set(char *str)
{
#if 0
    uint8_t offset;
    uint8_t wakeup_en;
    EXTI_IntSrc trig_gpio;
    EXTI_Trig_Type trig_lvl;
    GPIO_Num awake_gpio;
    GPIO_Value value;
    char ret = AT_ERROR;

    char tmp_str[2];

    offset = at_get_before_comma(str,0,tmp_str,NULL);
    wakeup_en = atoi(tmp_str);

    offset = at_get_before_comma(str,offset+1,tmp_str,NULL);
    trig_gpio = (EXTI_IntSrc)atoi(tmp_str);

    offset = at_get_before_comma(str,offset+1,tmp_str,NULL);
    trig_lvl = (EXTI_Trig_Type)atoi(tmp_str);

    offset = at_get_before_comma(str,offset+1,tmp_str,NULL);
    awake_gpio = (GPIO_Num)atoi(tmp_str);

    offset = at_get_before_comma(str,offset+1,tmp_str,NULL);
    value = (GPIO_Value)atoi(tmp_str);

    LOG(LOG_LVL_INFO,"wakeupgpio--%d,%d,%d,%d,%d\n",wakeup_en,trig_gpio,trig_lvl,awake_gpio,value);
    if(wakeup_en == 1)
    {
        exti_atwakeup_setting(trig_gpio,trig_lvl,awake_gpio,value);
        ret = AT_OK;
        at_printf("OK\n");
    }

    else if(wakeup_en == 0)
    {
        exti_atwakeup_disable();
        ret = AT_OK;
        at_printf("OK\n");
    }

    return ret;
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return 0;
}


//AT+SYSGPIODIR=5,1     //GPIO0_5,output
char at_sysgpiodir_set(char *str)
{
#if 0
    GPIO_Num gpio_num;
    GPIO_Direction dir;
    uint8_t offset;
    char tmp_str[2];
    char ret = AT_ERROR;

    offset = at_get_before_comma(str,0,tmp_str,NULL);
    gpio_num = (GPIO_Num)atoi(tmp_str);

    offset = at_get_before_comma(str,offset+1,tmp_str,NULL);
    dir = (GPIO_Direction)atoi(tmp_str);

    if(at_gpio[gpio_num].gpio_hw_en == 1)
    {
        HAL_GPIO_SetDir(gpio_num,dir);
        at_printf("OK\n");
        ret = AT_OK;
    }
    else
    {
        at_printf("NOT SUPPORT\n");
        at_printf("ERROR\n");
        ret = AT_ERROR;
    }
    return ret;
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;
}

//AT+SYSGPIOWRITE=5,1   //GPIO0_5,high_level
char at_sysgpiowrite_set(char *str)
{
#if 0
    GPIO_Num gpio_num;
    GPIO_Value value;
    uint8_t offset;
    char tmp_str[2];
    char ret = AT_ERROR;

    offset = at_get_before_comma(str,0,tmp_str,NULL);
    gpio_num = (GPIO_Num)atoi(tmp_str);

    offset = at_get_before_comma(str,offset+1,tmp_str,NULL);
    value = (GPIO_Value)atoi(tmp_str);

    if(at_gpio[gpio_num].gpio_hw_en == 1)
    {
        if(HAL_GPIO_GetDir(gpio_num) == GPIO_OUTPUT)
        {
            HAL_GPIO_WritePin(gpio_num,value);
            ret = AT_OK;
            at_printf("OK\n");
        }
        else
        {
            ret = AT_ERROR;
            at_printf("NOT OUTPUT\n");
            at_printf("ERROR\n");
        }
    }
    else
    {
        ret = AT_ERROR;
        at_printf("NOT SUPPORT\n");
        at_printf("ERROR\n");
    }

    return ret;
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;
}


char at_sysgpioread_set(char *str)
{
#if 0
    GPIO_Num gpio_num;
    char tmp_str[2];
    char ret = AT_ERROR;

    at_get_before_comma(str,0,tmp_str,NULL);
    gpio_num = (GPIO_Num)atoi(tmp_str);

    if(at_gpio[gpio_num].gpio_hw_en == 1)
    {
        if(HAL_GPIO_GetDir(gpio_num) == GPIO_INPUT)
        {
            ret = AT_OK;
            at_printf("+SYSGPIOREAD:%d,%d,%d\n",gpio_num,GPIO_INPUT,HAL_GPIO_ReadPin(gpio_num));
            at_printf("OK\n");
        }
        else
        {
            ret = AT_ERROR;
            at_printf("ERROR\n");
        }
    }
    else
    {
        ret = AT_ERROR;
        at_printf("NOT SUPPORT\n");
        at_printf("ERROR\n");
    }
    return ret;
#endif
    at_printf("[%s, %d]\r\n", __func__, __LINE__);
    return AT_OK;

}



char at_cal_excute(char *str)
{
    char ret = AT_ERROR;

    ART_ASSERT(str);
    if(str[0] == '1')
    {
        //        cal_Self_Only();
        ret = AT_OK;
        at_printf("OK\r\n");

    }
    else if(str[0] == '2')
    {
        //        cal_CoPHY_Only();
        ret = AT_OK;
        at_printf("OK\r\n");
    }
    else
    {
        ret = AT_ERROR;
        at_printf("ERROR\r\n");
    }



    return ret;
}

#include "mcu_init.h"
#include "serial/serial.h"
#include "utils/debug/log.h"
#include "wifi/wifi.h"
#include "tos_kv.h"
#include "hal/flash.h"
#include "hal/hal_interrupt.h"
#include "utils/debug/CmBacktrace/cm_backtrace.h"

uint8_t test_no_init_data[128] __attribute__((section("no_init_data"),zero_init));

extern Serial_t m_LogSerial;

int fputc(int ch, FILE *f)
{
    serial_putchar(&m_LogSerial, ch);
    return ch;
}

int _write(int fd, char *ptr, int len)
{
    len = serial_write(&m_LogSerial, (const void *)ptr, len);
    return len;
}

int fgetc(FILE *f)
{
    uint8_t ch = 0;
    return ch;
}


//int flash_write(uint32_t addr, const void *buf, size_t len){
//    FLASH_Program(addr,len, (uint8_t *)buf);
//}
//int flash_read(uint32_t addr, void *buf, size_t len){
//    FLASH_ReadByCache(addr, len, (uint8_t *)buf);
//}

//void nvds_kv_init(void)
//{
//    kv_flash_drv_t  flash_drv;
//    kv_flash_prop_t flash_prop;
//    
//    flash_drv.write = flash_write;
//    flash_drv.read  = flash_read;
//    flash_drv.erase = FLASH_Erase;
//    
//    flash_prop.sector_size_log2 = 8;
//    flash_prop.pgm_type         = KV_FLASH_PROGRAM_TYPE_BYTE;
//    flash_prop.flash_start      = SYSTEM_PARAMETER_OFFSET;
//    flash_prop.flash_size       = SIZE_4KB*2;
//}

void board_init(void)
{
    SetSysClock();
    set_interrupt_priority();
    switch_global_interrupt(true);
    
    //3.rf preprocess,img cal
    wifi_rf_preprocess();
    wifi_rf_image_cal();

    log_init();//init log serial
    cm_backtrace_init("wifi app", "HD_V2", "SW_V0.8");

//    nvds_kv_init();
}


void SysTick_Handler(void)
{
    if (tos_knl_is_running())
    {
        tos_knl_irq_enter();
        tos_tick_handler();
        tos_knl_irq_leave();
    }
}





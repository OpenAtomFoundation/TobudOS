#include "tos_k.h"
#include <stdio.h>
#include "mcu_init.h"
#include "stdlib.h"
#include "gd32vf103_gpio.h"
#include "nrf24l01_gd32v_hal.h"
#include "lcd.h"

#define USE_SPI1

k_sem_t sem_nrf;
k_sem_t sem_led;
k_mutex_t mutex_lcd;

#define TASK_SIZE (8*1024)
k_task_t task_nrf24_handle;
uint8_t task_nrf24_stk[TASK_SIZE];

#define LED_TASK_SIZE 1024
k_task_t led_handle;
uint8_t led_stk[LED_TASK_SIZE];


#define LCD_TASK_SIZE 1024
k_task_t lcd_handle;
uint8_t lcd_stk[LCD_TASK_SIZE];


#define CE_GPIO_PORT    GPIOA
#define CE_PIN          GPIO_PIN_3
#define CSN_GPIO_PORT   GPIOB
#define CSN_PIN         GPIO_PIN_12
#define IRQ_GPIO_PORT   GPIOB
#define IRQ_PIN         GPIO_PIN_5

void task_nrf24();

char lcd_buf[64];

void task_led(void *arg)
{
    int task_cnt1 = 0;
    while (1) {
        tos_sem_pend(&sem_led, ~0);
        gpio_bit_reset(LEDB_GPIO_PORT, LEDB_PIN);
        tos_task_delay(50);
        gpio_bit_set(LEDB_GPIO_PORT, LEDB_PIN);
    }
}


void task_lcd(void *arg)
{
    strcpy(lcd_buf, "initializing...");
    LCD_SetDisplayMode(LCD_DISPMODE_HORIZONTAL_MIRROR);
    LCD_Clear(BLACK);
    const uint16_t x = 0;
    const uint16_t y = 32;
    uint8_t oldlen = strlen(lcd_buf);
    for(int i=0; ; i++) {
        tos_mutex_pend(&mutex_lcd);
        uint8_t newlen = strlen(lcd_buf);
        if(newlen < oldlen) {
            LCD_Clear(BLACK);
        }
        oldlen = newlen;
        LCD_ShowString(x, y, lcd_buf, CYAN);
    }
}

void nrf24l01_init() {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

#ifdef USE_SPI0
	#define  SPIx  SPI0
    rcu_periph_clock_enable(RCU_SPI0);
    /* spi GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
#endif

#ifdef USE_SPI1
    #define SPIx SPI1
    rcu_periph_clock_enable(RCU_SPI1);
    /* spi GPIO config:SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
#endif

    {
        spi_parameter_struct spi_init_struct;
        /* deinitilize SPI and the parameters */
        spi_i2s_deinit(SPIx);
        spi_struct_para_init(&spi_init_struct);

        /* spi parameter config */
        spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
        spi_init_struct.device_mode          = SPI_MASTER;
        spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
        spi_init_struct.nss                  = SPI_NSS_SOFT;
        spi_init_struct.prescale             = SPI_PSC_8;
        spi_init_struct.endian               = SPI_ENDIAN_MSB;
        spi_init(SPIx, &spi_init_struct);

        spi_enable(SPIx);

        spi_ti_mode_disable(SPIx); // use motorola mode
        spi_crc_off(SPIx);
        spi_crc_polynomial_set(SPIx, 7);
        spi_nssp_mode_enable(SPIx);
    }

    {
        nrf_hal_init_t nhi;
        nhi.spi = SPIx;
        nhi.ce_port = CE_GPIO_PORT;
        nhi.ce_pin  = CE_PIN;
        nhi.csn_port= CSN_GPIO_PORT;
        nhi.csn_pin = CSN_PIN;

        nrf_init_t ni = {
                .init		= nrf_hal_init,
				.ce			= nrf_hal_ce,
				.csn		= nrf_hal_csn,
				.spi_recv	= nrf_hal_spi_recv,
				.spi_send	= nrf_hal_spi_send,
				.private	= &nhi,
        };

        nrf_init(&ni);
    }


    tos_sem_create(&sem_nrf, 1);
    tos_sem_create(&sem_led, 1);
    tos_mutex_create(&mutex_lcd);
    tos_task_create(&task_nrf24_handle, "task_nrf24",   task_nrf24,     NULL, 5, task_nrf24_stk, TASK_SIZE,     0);
    tos_task_create(&led_handle,        "led",          task_led,       NULL, 6, led_stk,        LED_TASK_SIZE, 0);
    tos_task_create(&lcd_handle,        "lcd",          task_lcd,       NULL, 7, lcd_stk,        LCD_TASK_SIZE, 0);
}

void EXTI5_9_IRQHandler(void)
{
   if (tos_knl_is_running()) {
        tos_knl_irq_enter();

        if (RESET != exti_interrupt_flag_get(IRQ_PIN)){

            exti_interrupt_flag_clear(IRQ_PIN);

            uint8_t status = 0;
            nrf_read_reg_byte(REG_STATUS, &status);

            if(status & _BV(RX_DR)) {
                  tos_sem_post(&sem_nrf);
            }
        }

       tos_knl_irq_leave();
    }
}

#define ADDRLEN 5
void print_rxaddr(uint8_t pipe) {
    uint8_t addr[ADDRLEN];

    uint8_t addrlen = ADDRLEN;
    nrf_get_rxaddr(pipe, addr, &addrlen);

    printf("pipe %u addr: ", pipe);
    for(int i=0; i<ADDRLEN; i++) {
        printf("%u ", addr[i]);
    }
    printf("\n");
}


void init_nrf24l01_irq() {
    // nrf24 irq pin
    gpio_init(IRQ_GPIO_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, IRQ_PIN);
    gpio_bit_set(IRQ_GPIO_PORT, IRQ_PIN);

    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(EXTI5_9_IRQn, 1, 1);

    /* connect EXTI line to GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_5);

    exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_5);
}

void test_nrf24l01_irq_rx()
{

    init_nrf24l01_irq();

    nrf_delay(200);
    nrf_csn(1);
    nrf_ce(0);
    nrf_delay(200);

    nrf_set_standby_mode();
    nrf_set_receive_mode();

    nrf_enable_rx_irq();

    nrf_set_rf_channel(64);
    nrf_set_datarate(NRF_2Mbps);
    uint8_t rxaddr[ADDRLEN] = { 0xCB, 0xA7, 0xF9, 0xAC, 0xE0 };
    nrf_set_rxaddr(0, rxaddr, ADDRLEN);
    nrf_enable_dynamic_payload(0);
    nrf_enable_rxaddr(0);

    while(1) {
        tos_sem_pend(&sem_nrf, ~0);

        uint8_t buf[32];
        uint8_t len = 0;
        uint8_t pipe = 0xFF;

        nrf_read_payload(buf, &len, &pipe);

        tos_sem_post(&sem_led);

        strncpy(lcd_buf, buf, len);
        tos_mutex_post(&mutex_lcd);
        printf("received %u bytes from pipe %u: ", len, pipe);

        for(int i=0; i<len; i++) {
            printf("%x ", buf[i]);
        }
        printf("\n");
    }
}

void test_nrf24l01_rx() {
    nrf_delay(200);
    nrf_hal_csn(1);
    nrf_hal_ce(0);
    nrf_delay(200);

    nrf_set_standby_mode();
    nrf_set_receive_mode();
    nrf_disable_rx_irq();

    nrf_set_rf_channel(64);
    nrf_set_datarate(NRF_2Mbps);
    uint8_t rxaddr[ADDRLEN] = { 0xCB, 0xA7, 0xF9, 0xAC, 0xE0 };
    nrf_set_rxaddr(0, rxaddr, ADDRLEN);
    nrf_enable_dynamic_payload(0);
    nrf_enable_rxaddr(0);

    nrf_flush_rx();

    while(1) {
        uint8_t buf[36];
        uint8_t len = 0;
        uint8_t pipe = 0xFF;

        nrf_poll_read_payload(buf, &len, &pipe);

        tos_sem_post(&sem_led);
        strncpy(lcd_buf, buf, len);
        tos_mutex_post(&mutex_lcd);
        printf("received %u bytes from pipe %u: ", len, pipe);

        for(int i=0; i<len; i++) {
            printf("%x ", buf[i]);
        }
        printf("\n");
    }
}


void test_nrf24l01_tx() {
    nrf_delay(200);
    nrf_hal_csn(1);
    nrf_hal_ce(0);
    nrf_delay(200);

    nrf_set_standby_mode();
    nrf_set_send_mode();
    nrf_disable_tx_irq();
    nrf_set_rf_channel(64);
    nrf_set_datarate(NRF_2Mbps);
    nrf_enable_dynamic_payload(0);
    uint8_t txaddr[] = { 0xCB, 0xA7, 0xF9, 0xAC, 0xE0 };
    nrf_set_txaddr(txaddr, 5);

    nrf_flush_rx();
    nrf_flush_tx();
    uint8_t cnt = 0;
    while(1) {
        nrf_flush_rx();
        nrf_flush_tx();
        uint8_t buf[32];
        snprintf(buf, sizeof(buf), "TOS nRF24L01+ %u", cnt++);
        nrf_write_payload(buf, strlen(buf)+1);
        tos_sem_post(&sem_led);
        nrf_delay(100);
    }
}

void task_nrf24() {
    test_nrf24l01_irq_rx();
    //test_nrf24l01_rx();
    //test_nrf24l01_tx();
}

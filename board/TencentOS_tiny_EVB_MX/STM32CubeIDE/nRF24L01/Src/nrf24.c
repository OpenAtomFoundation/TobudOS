#include "tos_k.h"
#include <stdio.h>
#include "stdlib.h"
#include "nrf24l01_stm32_hal.h"
#include "stm32l4xx_hal.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

extern k_sem_t sem_led;
k_sem_t sem_nrf;

#define TASK_SIZE (8*1024)
k_task_t task_nrf24_handle;
uint8_t task_nrf24_stk[TASK_SIZE];

#define LED_TASK_SIZE 1024
k_task_t led_handle;
uint8_t led_stk[LED_TASK_SIZE];

void task_nrf24();

k_sem_t sem_led;

void task_led(void *arg)
{
    while (1) {
        tos_sem_pend(&sem_led, ~0);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        tos_task_delay(50);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    }
}



void nrf24l01_init() {
    {
        nrf_hal_init_t nhi;
        nhi.spi = &hspi1;
        nhi.ce_port = CE_GPIO_Port;
        nhi.ce_pin  = CE_Pin;
        nhi.csn_port= CSN_GPIO_Port;
        nhi.csn_pin = CSN_Pin;

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
    tos_task_create(&task_nrf24_handle, "task_nrf24",   task_nrf24,     NULL, 5, task_nrf24_stk, TASK_SIZE,     0);
    tos_task_create(&led_handle,        "led",          task_led,       NULL, 6, led_stk,        LED_TASK_SIZE, 0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_PIN_8 != GPIO_Pin) {
		return;
	}

	uint8_t status = 0;
	nrf_read_reg_byte(REG_STATUS, &status);

	if(status & _BV(RX_DR)) {
		  tos_sem_post(&sem_nrf);
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

void print_txaddr() {
    uint8_t addr[ADDRLEN];
    uint8_t addrlen = ADDRLEN;
    nrf_get_txaddr(addr, &addrlen);
    for(int i=0; i<ADDRLEN; i++) {
        printf("%u ", addr[i]);
    }
    printf("\n");
}

void test_nrf24l01_irq_rx()
{
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
    print_rxaddr(0);

    while(1) {
        tos_sem_pend(&sem_nrf, ~0);

        uint8_t buf[32];
        uint8_t len = 0;
        uint8_t pipe = 0xFF;

        nrf_read_payload(buf, &len, &pipe);

        tos_sem_post(&sem_led);

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
    print_txaddr();
    print_rxaddr(0);

    nrf_flush_rx();
    nrf_flush_tx();
    uint8_t cnt = 0;
    while(1) {
        nrf_flush_rx();
        nrf_flush_tx();
        uint8_t buf[32];
        snprintf((char*)buf, sizeof(buf), "TOS nRF24L01+ %u", cnt++);
        nrf_write_payload(buf, strlen((char*)buf)+1);
        tos_sem_post(&sem_led);
        nrf_delay(100);
    }
}

void task_nrf24() {
    //test_nrf24l01_irq_rx();
    test_nrf24l01_tx();
}

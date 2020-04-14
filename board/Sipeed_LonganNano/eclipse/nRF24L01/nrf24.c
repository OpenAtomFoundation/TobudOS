#include "nrf24.h"
#include "tos_k.h"
#include <stdio.h>
extern k_sem_t sem_led;
k_sem_t sem_nrf_recv;
int flag = 0;
#define TASK_SIZE (8*1024)
k_task_t task_nrf24_handle;
uint8_t task_nrf24_stk[TASK_SIZE];

void task_nrf24();

void nrf24l01_init() {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI0);
	#define  SPIx  SPI0


    /* spi GPIO config:SCK/PB13, MISO/PB14, MOSI/PB15 */
    //gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
    //gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);


    /* spi GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);



    tos_task_create(&task_nrf24_handle, "task_nrf24", task_nrf24,  NULL, 5, task_nrf24_stk, TASK_SIZE, 0);

    nrf_hal_init_t nhi;
    nhi.spi = SPIx;
    nhi.ce_port = GPIOA;
    nhi.ce_pin  = GPIO_PIN_3;
    nhi.csn_port= GPIOB;
    nhi.csn_pin = GPIO_PIN_5;

    gpio_init(nhi.ce_port,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, nhi.ce_pin);
    gpio_init(nhi.csn_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, nhi.csn_pin);

    gpio_bit_set(nhi.ce_port, nhi.ce_pin);
    gpio_bit_set(nhi.csn_port, nhi.csn_pin);


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

    nrf_init(&nhi);

}

void EXTI0_IRQHandler(void)
{
   if (tos_knl_is_running()) {
        tos_knl_irq_enter();

        if (RESET != exti_interrupt_flag_get(GPIO_PIN_0)){

            exti_interrupt_flag_clear(GPIO_PIN_0);

            uint8_t status = 0;
            nrf_hal_read_reg_byte(REG_STATUS, &status);

            if(status & _BV(RX_DR)) {
                  tos_sem_post(&sem_nrf_recv);
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

void task_nrf24() {

    if(1)
    {
        tos_sem_create(&sem_nrf_recv, 1);
        // nrf24 irq pin
        gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
        gpio_bit_set(GPIOB, GPIO_PIN_0);

        eclic_global_interrupt_enable();
        eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
        eclic_irq_enable(EXTI0_IRQn, 1, 1);

        /* connect EXTI line to GPIO pin */
        gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_0);

        exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(EXTI_0);

    }

    uint8_t data = 0;

    nrf_delay(200);

    nrf_hal_csn(1);
    nrf_hal_ce(0);

    nrf_delay(200);

    nrf_set_standby_mode();

    nrf_set_receive_mode();
    nrf_enable_rx_irq();

    nrf_set_rf_channel(64);
    nrf_set_datarate(NRF_2Mbps);
    uint8_t rxaddr[ADDRLEN] = { 0xAA, 0xCC, 0xEE, 0x00, 0x00 };
    nrf_set_rxaddr(0, rxaddr, ADDRLEN);

    nrf_enable_dynamic_payload(0);
    nrf_enable_dynamic_payload(1);

    nrf_enable_rxaddr(0);
    nrf_enable_rxaddr(1);

    print_rxaddr(0);
    print_rxaddr(1);
    print_rxaddr(2);


    nrf_flush_rx();


    while(1) {
        tos_sem_pend(&sem_nrf_recv, ~0);

        uint8_t buf[32];
        uint8_t len = 0;
        uint8_t status = 0;
        nrf_hal_read_reg_byte(REG_STATUS, &status);

        uint8_t pipe = ((status>>1) & 0x07);
        nrf_read_payload(buf, &len);

        if(pipe >= 6) {
            printf("shit happens\n");
        }

        nrf_hal_set_reg_bit(REG_STATUS, _BV(RX_DR));

        nrf_flush_rx();

        printf("received %u bytes from pipe %u: ", len, pipe);

        tos_sem_post(&sem_led);

        for(int i=0; i<len; i++) {
            printf("%x ", buf[i]);
        }
        printf("\n");

    }

}


uint8_t nrf_hal_test_rx_old() {

    uint8_t data = 0;

    nrf_delay(200);

    nrf_hal_csn(1);
    nrf_hal_ce(0);

    nrf_delay(200);

    nrf_set_standby_mode();

    nrf_set_receive_mode();
    nrf_disable_rx_irq();

    nrf_set_rf_channel(64);
    nrf_set_datarate(NRF_2Mbps);
    uint8_t rxaddr[ADDRLEN] = { 0xAA, 0xCC, 0xEE, 0x00, 0x00 };
    nrf_set_rxaddr(0, rxaddr, ADDRLEN);

    nrf_enable_dynamic_payload(0);
    nrf_enable_dynamic_payload(1);

    nrf_enable_rxaddr(0);
    nrf_enable_rxaddr(1);

    print_rxaddr(0);
    print_rxaddr(1);
    print_rxaddr(2);


    nrf_flush_rx();
    while(1) {
        uint8_t buf[32];
        uint8_t len = 0;
        uint8_t status = 0;
        nrf_hal_read_reg_byte(REG_STATUS, &status);

        if((status & _BV(RX_DR)) == 0) {
            printf("nodata %x\n", status);
            nrf_delay(100);
        }

        uint8_t pipe = 0xFF;
        uint32_t loop_cnt = 0;
        while(1) {
            nrf_hal_read_reg_byte(REG_STATUS, &status);
            pipe = status;
            pipe >>= 1;
            pipe &= 0x07;
            if(pipe < 6) {
                break;
            }
            nrf_delay(1);
            loop_cnt++;
        }
        nrf_read_payload(buf, &len);

        if(loop_cnt > 0) {
            printf("loopcnt %u\n", loop_cnt);
        }

        nrf_hal_set_reg_bit(REG_STATUS, _BV(RX_DR));
        nrf_hal_read_reg_byte(REG_STATUS, &status);

        nrf_flush_rx();

        printf("received %u bytes from pipe %u: ", len, pipe);


        for(int i=0; i<len; i++) {
            printf("%x ", buf[i]);
        }
        printf("\n");

    }

    return data;
}


uint8_t nrf_hal_test_tx() {

    uint8_t data = 0;

    nrf_delay(200);
    nrf_hal_csn(1);
    nrf_hal_ce(0);
    nrf_delay(200);

    nrf_set_standby_mode();
    nrf_set_send_mode();

    nrf_set_rf_channel(100);
    nrf_set_datarate(NRF_2Mbps);

    nrf_enable_dynamic_payload(0);
    uint8_t txaddr[] = { 1, 2, 3, 4, 0 };
    nrf_set_txaddr(txaddr, 5);

    nrf_flush_rx();
    nrf_flush_tx();
    uint32_t cnt = 0;
    while(1) {
        nrf_flush_rx();
        nrf_flush_tx();
        uint8_t buf[] = {0x0A, 0x0C, 0x0E, cnt++ };
        nrf_write_payload(buf, sizeof(buf));

        while(1) {
            uint8_t status = 0;
            nrf_hal_read_reg_byte(REG_STATUS, &status);

            printf("status %x\n", status);
            if(status & _BV(MAX_RT)) {
                printf("send error....\n");
                nrf_hal_set_reg_bit(REG_STATUS, _BV(MAX_RT));
                nrf_flush_tx();
                break;
            }else if(status &  _BV(TX_DS)) {
                printf("sended....\n");
                nrf_hal_set_reg_bit(REG_STATUS, _BV(TX_DS));
                break;
            } else {
                printf("sending....\n");
            }
        }

        nrf_delay(100);
    }

    return data;
}

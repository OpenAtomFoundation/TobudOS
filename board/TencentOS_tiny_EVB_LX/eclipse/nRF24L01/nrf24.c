#include "nrf24.h"

void nrf24l01_init() {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI1);
    uint32_t spi = SPI1;

#if 1
    /* spi GPIO config:SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

#else
    /* spi GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
#endif

    nrf_hal_init_t nhi;
    nhi.spi = spi;
    nhi.ce_port = GPIOC;
    nhi.ce_pin  = GPIO_PIN_4;
    nhi.csn_port= GPIOB;
    nhi.csn_pin = GPIO_PIN_12;

    gpio_init(nhi.ce_port,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, nhi.ce_pin);
    gpio_init(nhi.csn_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, nhi.csn_pin);

    gpio_bit_set(nhi.ce_port, nhi.ce_pin);
    gpio_bit_set(nhi.csn_port, nhi.csn_pin);

    nrf_init(&nhi);


    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(spi);
    spi_struct_para_init(&spi_init_struct);

    /* spi parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_256;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(spi, &spi_init_struct);



    //spi_i2s_data_frame_format_config(spi, SPI_FRAMESIZE_8BIT);
    //spi_i2s_interrupt_disable(spi, SPI_I2S_INT_TBE);
    //spi_i2s_interrupt_disable(spi, SPI_I2S_INT_RBNE);
    //spi_i2s_interrupt_disable(spi, SPI_I2S_INT_ERR);

    spi_enable(spi);



    spi_ti_mode_disable(spi); // use motorola mode
    spi_crc_off(spi);
    spi_crc_polynomial_set(spi,7);

    spi_nssp_mode_enable(spi);
    spi_i2s_data_frame_format_config(spi, SPI_FRAMESIZE_8BIT);
}

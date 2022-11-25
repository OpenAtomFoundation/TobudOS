#ifndef _TOS_HAL_SPI_H_
#define _TOS_HAL_SPI_H_

typedef enum hal_spi_port_en {
    HAL_SPI_PORT_0 = 0,
    HAL_SPI_PORT_1,
    HAL_SPI_PORT_2,
    HAL_SPI_PORT_3,
} hal_spi_port_t;

typedef struct hal_spi_st {
    hal_spi_port_t      port;
    void                *private_spi;
} hal_spi_t;

__API__ int tos_hal_spi_init(hal_spi_t *spi, hal_spi_port_t port);

__API__ int tos_hal_spi_transfer(hal_spi_t *spi, const uint8_t *send_buf, uint8_t *recv_buf, size_t size, uint32_t timeout);

__API__ int tos_hal_spi_deinit(hal_spi_t *spi);

#endif

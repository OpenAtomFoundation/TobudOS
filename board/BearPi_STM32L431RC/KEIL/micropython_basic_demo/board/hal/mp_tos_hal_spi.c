#include "tos_k.h"
#include "mp_tos_hal_spi.h"
#include "stm32l4xx_hal.h"
#include "spi.h"

__API__ int tos_hal_spi_init(hal_spi_t *spi, hal_spi_port_t port)
{
    if (!spi) {
        return -1;
    }
    
    if (port == HAL_SPI_PORT_1) {
        spi->private_spi = &hspi1;
        MX_SPI1_Init();
    } else if (port == HAL_SPI_PORT_2) {
        spi->private_spi = &hspi2;
        MX_SPI2_Init();
    } else if (port == HAL_SPI_PORT_3) {
        spi->private_spi = &hspi3;
        MX_SPI3_Init();
    } else {
        return -1;
    }

    return 0;
}

__API__ int tos_hal_spi_transfer(hal_spi_t *spi, const uint8_t *send_buf, uint8_t *recv_buf, size_t size, uint32_t timeout)
{
    HAL_StatusTypeDef hal_status;
    SPI_HandleTypeDef *spi_handle;

    if (!spi || (!send_buf && !recv_buf)) {
        return -1;
    }

    if (!spi->private_spi) {
        return -1;
    }

    spi_handle = (SPI_HandleTypeDef *)spi->private_spi;

    if (send_buf && recv_buf) {
        hal_status = HAL_SPI_TransmitReceive(spi_handle, send_buf, recv_buf, size, timeout);
    } else if (send_buf) {
        hal_status = HAL_SPI_Transmit(spi_handle, send_buf, size, timeout);
    } else if (recv_buf) {
        hal_status = HAL_SPI_Receive(spi_handle, send_buf, size, timeout);
    }

    if (hal_status != HAL_OK) {
        return -1;
    }
    return 0;
}

__API__ int tos_hal_spi_deinit(hal_spi_t *spi)
{
    HAL_StatusTypeDef hal_status;
    SPI_HandleTypeDef *spi_handle;

    if (!spi) {
        return -1;
    }

    if (!spi->private_spi) {
        return -1;
    }

    spi_handle = (SPI_HandleTypeDef *)spi->private_spi;
    
    hal_status = HAL_SPI_DeInit(spi_handle);
    HAL_SPI_MspDeInit(spi_handle);

    if (hal_status != HAL_OK) {
        return -1;
    }

    return 0;
}

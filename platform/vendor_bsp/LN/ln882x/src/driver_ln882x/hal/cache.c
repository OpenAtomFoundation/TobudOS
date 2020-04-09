#include "hal/spi_type.h"
#include "ll/ll_qspi.h"
#include "hal/qspi.h"
#include "hal/cache.h"
#include "ll/ll_cache.h"
#include "ll/ll_syscon.h"
#include "types.h"
#include "proj_config.h"

#pragma push
#pragma O2

/**
 * @brief Init flash cache.
 *
 * @param flash_base_addr Base address of flash.
 */
void flash_cache_init(uint32_t flash_base_addr)
{
	LL_QSPI_Enable( SSI_Disabled );
	LL_QSPI_SckDivSet( AHBUS_CLOCK / QSPI_CLK );
	LL_QSPI_RxSampleDlySet(1);
	LL_QSPI_CtrlR0Set( Quad_SPI_Format, DFS_32_32_bits, 0, 0, 0, Receive_Only, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI );
	LL_QSPI_CtrlR1Set( (CACHE_LINE_SIZE_IN_BYTES / sizeof(uint32_t)) - 1 );
	LL_QSPI_SPICtrl( 8, LL_QSPI_INSTRUCTION_LEN_8_BITS, 6, LL_QSPI_BOTH_STANDARD_SPI_MODE );
	LL_QSPI_IntMaskAll();
	LL_QSPI_SlaveSelect( QSPI_SLAVE_INDEX );
	LL_QSPI_Enable( SSI_Enabled );

	LL_CACHE_FlashBaseAddrSet( flash_base_addr );
	LL_CACHE_QspiDRAddrSet( (uint32_t) LL_QSPI_DataRegAddrGet() );
	LL_CACHE_FlashCmdSet( 0, 2, RIGHT_JUSTIFIED, FLASH_LITTLE_ENDIAN, RIGHT_JUSTIFIED, FLASH_QUAD_READ_CMD );
	LL_CACHE_Enable( CACHE_ENABLE );
	LL_CACHE_FlushAll( 1 );
	LL_CACHE_FlushEnable( CACHE_FLUSH_ENABLE );
	LL_SYSCON_FlashOrMirrorMode(1);
	while(LL_CACHE_IsFlushing());
}


/**
 * @brief Disable flash cache.
 *
 */
void flash_cache_disable(void)
{
	while(LL_CACHE_StateGet() == CACHE_STATE_FILLING);
	LL_CACHE_Enable(CACHE_DISABLE);
	LL_QSPI_Enable(SSI_Disabled);
}

/**
 * @brief Flush flash in a range.
 *
 * @param low_addr Lower address to flush.
 * @param high_addr Higher address to flush.
 */
void flash_cache_flush(uint32_t low_addr,uint32_t high_addr)
{
	LL_CACHE_FlushAddrLow(low_addr);
	LL_CACHE_FlushAddrHigh(high_addr);
	LL_CACHE_FlushEnable(CACHE_FLUSH_ENABLE);
	while(LL_CACHE_IsFlushing() == CACHE_FLUSH_STATE_INPROGRESS);
}

/**
 * @brief Flush all flash.
 *
 */
void flash_cache_flush_all(void)
{
	LL_CACHE_FlushAll(1);
	LL_CACHE_FlushEnable(CACHE_FLUSH_ENABLE);
	while(LL_CACHE_IsFlushing() == CACHE_FLUSH_STATE_INPROGRESS);
}

#pragma pop



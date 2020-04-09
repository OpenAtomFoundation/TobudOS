#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/**
 * @brief Init flash cache.
 *
 * @param flash_base_addr Base address of flash.
 */
void flash_cache_init(uint32_t flash_base_addr);

/**
 * @brief Disable flash cache.
 */
void flash_cache_disable(void);

/**
 * @brief Flush cache block.
 * @param low_addr Lower address to flush.
 * @param high_addr Higher address to flush.
 */
void flash_cache_flush(uint32_t low_addr,uint32_t high_addr);

/**
 * @brief Flush all flash.
 *
 */
void flash_cache_flush_all(void);


#ifdef __cplusplus
    }
#endif // __cplusplus
#endif

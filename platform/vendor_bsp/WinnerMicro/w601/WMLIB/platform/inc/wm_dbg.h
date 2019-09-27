#ifndef __WM_DBG_H__
#define __WM_DBG_H__

#include "wm_debug.h"

/* Define the module switcher */
#define TLS_FLASH_DBG                       TLS_DBG_OFF
#define TLS_SPI_DBG                         TLS_DBG_OFF
#define TLS_IO_DBG                          TLS_DBG_OFF
#define TLS_DMA_DBG                         TLS_DBG_OFF
#define TLS_WL_DBG                          TLS_DBG_OFF
#define TLS_WPA_DBG                         TLS_DBG_OFF

/* flash info */
#if (TLS_FLASH_DBG && TLS_DBG_LEVEL_INFO)
#define TLS_DBGPRT_FLASH_INFO(f, a...)          __TLS_DBGPRT_INFO(f, ##a)
#else
#define TLS_DBGPRT_FLASH_INFO(f, a...)
#endif

/* flash warnning */
#if (TLS_FLASH_DBG && TLS_DBG_LEVEL_WARNING)
#define TLS_DBGPRT_FLASH_WARNING(f, a...)       __TLS_DBGPRT_WARNING(f, ##a)
#else
#define TLS_DBGPRT_FLASH_WARNING(f, a...)
#endif

/* flash error */
#if (TLS_FLASH_DBG && TLS_DBG_LEVEL_ERR)
#define TLS_DBGPRT_FLASH_ERR(f, a...)           __TLS_DBGPRT_ERR(f, ##a)
#else
#define TLS_DBGPRT_FLASH_ERR(f, a...)
#endif


#if (TLS_SPI_DBG && TLS_DBG_LEVEL_INFO)
#define TLS_DBGPRT_SPI_INFO(f, a...)          __TLS_DBGPRT_INFO(f, ##a)
#else
#define TLS_DBGPRT_SPI_INFO(f, a...)
#endif

#if (TLS_SPI_DBG && TLS_DBG_LEVEL_WARNING)
#define TLS_DBGPRT_SPI_WARNING(f, a...)       __TLS_DBGPRT_WARNING(f, ##a)
#else
#define TLS_DBGPRT_SPI_WARNING(f, a...)
#endif

#if (TLS_SPI_DBG && TLS_DBG_LEVEL_ERR)
#define TLS_DBGPRT_SPI_ERR(f, a...)           __TLS_DBGPRT_ERR(f, ##a)
#else
#define TLS_DBGPRT_SPI_ERR(f, a...)
#endif


#if (TLS_IO_DBG && TLS_DBG_LEVEL_INFO)
#define TLS_DBGPRT_IO_INFO(f, a...)          __TLS_DBGPRT_INFO(f, ##a)
#else
#define TLS_DBGPRT_IO_INFO(f, a...)
#endif

#if (TLS_IO_DBG && TLS_DBG_LEVEL_WARNING)
#define TLS_DBGPRT_IO_WARNING(f, a...)       __TLS_DBGPRT_WARNING(f, ##a)
#else
#define TLS_DBGPRT_IO_WARNING(f, a...)
#endif

#if (TLS_IO_DBG && TLS_DBG_LEVEL_ERR)
#define TLS_DBGPRT_IO_ERR(f, a...)           __TLS_DBGPRT_ERR(f, ##a)
#else
#define TLS_DBGPRT_IO_ERR(f, a...)
#endif


#if (TLS_DMA_DBG && TLS_DBG_LEVEL_INFO)
#define TLS_DBGPRT_DMA_INFO(f, a...)          __TLS_DBGPRT_INFO(f, ##a)
#else
#define TLS_DBGPRT_DMA_INFO(f, a...)
#endif

#if (TLS_DMA_DBG && TLS_DBG_LEVEL_WARNING)
#define TLS_DBGPRT_DMA_WARNING(f, a...)       __TLS_DBGPRT_WARNING(f, ##a)
#else
#define TLS_DBGPRT_DMA_WARNING(f, a...)
#endif

#if (TLS_DMA_DBG && TLS_DBG_LEVEL_ERR)
#define TLS_DBGPRT_DMA_ERR(f, a...)           __TLS_DBGPRT_ERR(f, ##a)
#else
#define TLS_DBGPRT_DMA_ERR(f, a...)
#endif

#endif /* __DBG_H__ */


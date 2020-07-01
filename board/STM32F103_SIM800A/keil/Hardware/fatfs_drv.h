#ifndef FATFS_DRV_H
#define FATFS_DRV_H

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "hal_spi_flash.h"

DSTATUS w25q64_fatfs_initialize(BYTE);
DSTATUS w25q64_fatfs_status(BYTE);
DRESULT w25q64_fatfs_read (BYTE, BYTE*, DWORD, UINT);
#if 1
DRESULT w25q64_fatfs_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if 1
  DRESULT w25q64_fatfs_ioctl (BYTE, BYTE, void*);
#endif  /* _USE_IOCTL == 1 */

#endif

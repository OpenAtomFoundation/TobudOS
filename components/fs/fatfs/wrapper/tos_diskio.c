/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "tos_ff.h"			/* Obtains integer types */
#include "tos_diskio.h"		/* Declarations of disk functions */

DSTATUS tos_disk_status (
	vfs_inode_t *dev
)
{
    vfs_blkdev_geo_t geo;

    if (!dev || !dev->ops.bd_ops->geometry) {
        return STA_NODISK;
    }

    if (dev->ops.bd_ops->geometry(dev, &geo) != 0) {
        return STA_NOINIT;
    }

    if (!geo.is_available) {
        return STA_NOINIT;
    }

    return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS tos_disk_initialize (
	vfs_inode_t *dev
)
{
    if (!dev || !dev->ops.bd_ops->open) {
        return STA_NODISK;
    }

    if (dev->ops.bd_ops->open(dev) != 0) {
        return STA_NOINIT;
    }

    return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT tos_disk_read (
	vfs_inode_t *dev,
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    if (!dev || !dev->ops.bd_ops->read) {
        return RES_PARERR;
    }

    if (dev->ops.bd_ops->read(dev, buff, sector, count) != 0) {
        return RES_ERROR;
    }

    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT tos_disk_write (
	vfs_inode_t *dev,
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    if (!dev || !dev->ops.bd_ops->write) {
        return RES_PARERR;
    }

    if (dev->ops.bd_ops->write(dev, buff, sector, count) != 0) {
        return RES_ERROR;
    }

    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT tos_disk_ioctl (
	vfs_inode_t *dev,
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    if (!dev || !dev->ops.bd_ops->ioctl) {
        return RES_PARERR;
    }

    if (dev->ops.bd_ops->ioctl(dev, cmd, (unsigned long)buff) != 0) {
        return RES_ERROR;
    }

    return RES_OK;
}




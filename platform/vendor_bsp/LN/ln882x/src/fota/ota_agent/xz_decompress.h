
#ifndef __XZ_DECOMPRESS_H__
#define __XZ_DECOMPRESS_H__

#include "stdint.h"

#define prob_t  uint16_t //uint32_t

#define INTERNAL_DATA_BUF_SIZE    (16*1024)
#define DICT_BUF_SIZE             (4*1024)
#define BLOCK_BUF_SIZE            (1*1024)

typedef enum {
	XZ_RET_OK                   =  0,
	XZ_RET_DATA_ERR             = -1,
	XZ_RET_NOT_ENOUGH_MEM       = -2,
	XZ_RET_STREAM_VER_NOT_SUPP  = -3,
	XZ_RET_FILE_TOO_LONG        = -4,
	XZ_RET_PROPERTIES_ERR       = -5,
	XZ_RET_MEM_ALLOC_FILED      = -6,
}xz_errno_t;


typedef struct
{
	int(*Read)(uint8_t** buf, uint32_t* buf_len);
} read_cb_t;

typedef struct
{
	int(*Write)(uint32_t offset, uint8_t* buf, uint32_t buf_len);
} write_cb_t;

typedef struct
{
	uint8_t header[13];
} xz_header_t;


int xz_decompress_buf_to_buf(uint8_t* header, write_cb_t* out_cb, read_cb_t* in_cb);

#endif  /* __XZ_DECOMPRESS_H__ */


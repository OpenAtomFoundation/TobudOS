/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-25 23:54:38
 * @LastEditTime: 2020-06-17 15:10:03
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _FIFO_H_
#define _FIFO_H_

#include "salof_defconfig.h"

#ifdef SALOF_USING_LOG

#define			FIFO_READ		0
#define			FIFO_WRITE		1

#define 		FIFO_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define 		FIFO_MIN(a,b) (((a) < (b)) ? (a) : (b))

struct salof_fifo {
	unsigned int    size;       /* fifo size */
	unsigned int    in;         /* data input pointer (in % size) */
	unsigned int    out;        /* data output pointer (out % size) */
	salof_mutex     mutex;   	/* mutex */   
	salof_sem		sem;		/* sem */    
	void            *buff;    /* data area */
};
typedef struct salof_fifo * salof_fifo_t;

salof_fifo_t salof_fifo_create(unsigned int size);
unsigned int salof_fifo_write(salof_fifo_t fifo, void *buff, unsigned int len, unsigned int timeout);
unsigned int salof_fifo_read(salof_fifo_t fifo, void *buff, unsigned int len, unsigned int timeout);
unsigned int salof_fifo_read_able(salof_fifo_t fifo);
unsigned int salof_fifo_write_able(salof_fifo_t fifo);

#endif

#endif // !_FIFO_H_


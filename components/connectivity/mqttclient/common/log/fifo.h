/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-25 23:54:38
 * @LastEditTime : 2019-12-28 01:04:08
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _FIFO_H_
#define _FIFO_H_

#include "config.h"

#define			FIFO_READ		0
#define			FIFO_WRITE		1

#define 		FIFO_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define 		FIFO_MIN(a,b) (((a) < (b)) ? (a) : (b))

struct fifo
{
	unsigned int    size;       /* fifo size */
	unsigned int    in;         /* data input pointer (in % size) */
	unsigned int    out;        /* data output pointer (out % size) */
	salof_mutex     mutex;   	/* mutex */   
	salof_sem		sem;		/* sem */    
	void            *buff;    /* data area */
};
typedef struct fifo * fifo_t;

fifo_t fifo_create(unsigned int size);
unsigned int fifo_write(fifo_t fifo, void *buff, unsigned int len, unsigned int timeout);
unsigned int fifo_read(fifo_t fifo, void *buff, unsigned int len, unsigned int timeout);
unsigned int fifo_read_able(fifo_t fifo);
unsigned int fifo_write_able(fifo_t fifo);

#endif // !_FIFO_H_


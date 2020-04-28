
#ifndef __FIFO_BUF_H__
#define __FIFO_BUF_H__

#include "types.h"

#ifndef EOF
#define EOF (-1)
#endif

typedef struct FIFOBuffer
{
	unsigned char * volatile head;
	unsigned char * volatile tail;
	unsigned char *begin;
	unsigned char *end;
} FIFOBuffer;


bool fifo_isempty(const FIFOBuffer *fb);

bool fifo_isfull(const FIFOBuffer *fb);

void fifo_push(FIFOBuffer *fb, unsigned char c);

unsigned char fifo_pop(FIFOBuffer *fb);

void fifo_flush(FIFOBuffer *fb);

/**
 * Thread safe version of fifo_isfull()
 */
bool fifo_isfull_locked(const FIFOBuffer *_fb);


void fifo_init(FIFOBuffer *fb, unsigned char *buf, unsigned int size);

unsigned int fifo_len(FIFOBuffer *fb);

#endif /* __FIFO_BUF_H__ */

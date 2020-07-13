#include "ln88xx.h"
#include "utils/fifo/fifobuf.h"
#include "utils/debug/art_assert.h"

/**
 * Check whether the fifo is empty
 *
 * \note Calling fifo_isempty() is safe while a concurrent
 *       execution context is calling fifo_push() or fifo_pop()
 *       only if the CPU can atomically update a pointer
 *       (which the AVR and other 8-bit processors can't do).
 *
 * \sa fifo_isempty_locked
 */
bool fifo_isempty(const FIFOBuffer *fb)
{
	return fb->head == fb->tail;
}

/**
 * Check whether the fifo is full
 *
 * \note Calling fifo_isfull() is safe while a concurrent
 *       execution context is calling fifo_pop() and the
 *       CPU can update a pointer atomically.
 *       It is NOT safe when the other context calls
 *       fifo_push().
 *       This limitation is not usually problematic in a
 *       consumer/producer scenario because the
 *       fifo_isfull() and fifo_push() are usually called
 *       in the producer context.
 */
bool fifo_isfull(const FIFOBuffer *fb)
{
	return ((fb->head == fb->begin) && (fb->tail == fb->end)) || (fb->tail == fb->head - 1);
}

/**
 * Push a character on the fifo buffer.
 *
 * \note Calling \c fifo_push() on a full buffer is undefined.
 *       The caller must make sure the buffer has at least
 *       one free slot before calling this function.
 *
 * \note It is safe to call fifo_pop() and fifo_push() from
 *       concurrent contexts, unless the CPU can't update
 *       a pointer atomically (which the AVR and other 8-bit
 *       processors can't do).
 *
 * \sa fifo_push_locked
 */
void fifo_push(FIFOBuffer *fb, unsigned char c)
{
	if(!fifo_isfull(fb))
	{
		/* fifo not full */
		if ((fb->tail >= fb->begin) && (fb->tail < fb->end))
		{
			/* Write at tail position */
			*(fb->tail) = c;
			/* Move tail forward */
			fb->tail++;
		}
		else if (fb->tail == fb->end)
		{
			/* Write at tail position */
			*(fb->tail) = c;
			/* wrap tail around */
			fb->tail = fb->begin;
		}
		else
		{
			/* invalid */
			ART_ASSERT(0);
		}
	}
}

/**
 * Pop a character from the fifo buffer.
 *
 * \note Calling \c fifo_pop() on an empty buffer is undefined.
 *       The caller must make sure the buffer contains at least
 *       one character before calling this function.
 *
 * \note It is safe to call fifo_pop() and fifo_push() from
 *       concurrent contexts.
 */
unsigned char fifo_pop(FIFOBuffer *fb)
{
	unsigned char ch = 0;

	if (!fifo_isempty(fb))
	{
		if ((fb->head >= fb->begin) && (fb->head < fb->end))
		{
			/* get char and move head forward */
			ch = *(fb->head++);
		}
		else if (fb->head == fb->end)
		{
			/* get char and wrap head around */
			ch = *(fb->end);
			fb->head = fb->begin;
		}
		else
		{
			/* invalid */
			ART_ASSERT(0);
		}
	}

	return ch;
}

/**
 * Make the fifo empty, discarding all its current contents.
 */
void fifo_flush(FIFOBuffer *fb)
{
    fb->head = fb->tail = fb->begin;
}

/**
 * Thread safe version of fifo_isfull()
 */
bool fifo_isfull_locked(const FIFOBuffer *_fb)
{
	bool result;
	uint32_t __irq_mask;
	
	/**GLOBAL_INT_DISABLE()**/
	{
		__irq_mask = __get_PRIMASK();
		__disable_irq();
	}
	result = fifo_isfull(_fb);
	/**GLOBAL_INT_RESTORE()**/
	{
		if(__irq_mask==0)	{
			__enable_irq();
		}
	}
	return result;
}

/**
 * FIFO Initialization.
 */
void fifo_init(FIFOBuffer *fb, unsigned char *buf, unsigned int size)
{
	/* FIFO buffers have a known bug with 1-byte buffers. */
    ART_ASSERT(size > 1);

    fb->head = fb->tail = fb->begin = buf;
    fb->end = buf + size - 1;
}

/**
 * \return Lenght of the FIFOBuffer \a fb.
 */
unsigned int fifo_len(FIFOBuffer *fb)
{
	return fb->end - fb->begin + 1;
}



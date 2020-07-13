#include "fifo.h"
#include <string.h>

#ifdef SALOF_USING_LOG

static unsigned int _flbs(unsigned int x)   /* find last bit set*/
{
    unsigned int r = 32;

    if (!x)
        return 0;
    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }
    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }
    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }
    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }
    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }
    return r;
}

static unsigned int _salof_fifo_align(unsigned int x)
{
    return (1 << (_flbs(x-1)-1));       //memory down alignment
}

salof_fifo_t salof_fifo_create(unsigned int size)
{
    salof_fifo_t fifo;

    if (0 == size)
        return NULL;

    if (size & (size - 1))
        size = _salof_fifo_align(size);

    fifo = (salof_fifo_t)salof_alloc((sizeof(struct salof_fifo) + size));

    if (NULL != fifo) {
        fifo->buff = (unsigned char *)fifo + sizeof(struct salof_fifo);

        fifo->mutex = salof_mutex_create();
        fifo->sem = salof_sem_create();
        if ((NULL == fifo->mutex) || (NULL == fifo->sem)) {
            salof_free(fifo);
            return NULL;
        }

        fifo->size = size;
        fifo->in = 0;
        fifo->out = 0;

        return fifo;
    }

    return NULL;
}

unsigned int salof_fifo_write(salof_fifo_t fifo, void *buff, unsigned int len, unsigned int timeout)
{
    int err, l;

    if((!fifo) || (!buff) || (!len))
        return 0;

    err = salof_mutex_pend(fifo->mutex, timeout);
    if(err == -1)
        return 0;

    len = FIFO_MIN(len, (fifo->size - fifo->in + fifo->out));

    l = FIFO_MIN(len, (fifo->size - (fifo->in & (fifo->size -1))));
    memcpy(((unsigned char *)fifo->buff + (fifo->in & (fifo->size -1))), buff, l);
    memcpy(fifo->buff, (unsigned char *)buff + l, len - l);

    fifo->in += len;

    salof_mutex_post(fifo->mutex);
    salof_sem_post(fifo->sem);

    return len;
}

unsigned int salof_fifo_read(salof_fifo_t fifo, void *buff, unsigned int len, unsigned int timeout)
{
    int l;

    salof_sem_pend(fifo->sem, timeout);

    if((!fifo) || (!buff) || (!len))
        return 0;
    
    len = FIFO_MIN(len, fifo->in - fifo->out);

    l = FIFO_MIN(len, (fifo->size - (fifo->out & (fifo->size -1))));
    memcpy(buff, ((unsigned char *)fifo->buff + (fifo->out & (fifo->size -1))), l);
    memcpy((unsigned char *)buff + l, fifo->buff, len - l);

    fifo->out += len;

    return len;
}

unsigned int salof_fifo_read_able(salof_fifo_t fifo)
{
    if(NULL == fifo)
        return 0;

    else if(fifo->in == fifo->out)
        return 0;

    else if(fifo->in > fifo->out)
        return (fifo->in - fifo->out);
    
    return (fifo->size - (fifo->out - fifo->in));
}

unsigned int salof_fifo_write_able(salof_fifo_t fifo)
{
    return (fifo->size - salof_fifo_read_able(fifo));
}

#endif

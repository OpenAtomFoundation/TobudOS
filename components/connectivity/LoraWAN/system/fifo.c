/*!
 * \file      fifo.h
 *
 * \brief     FIFO buffer implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "fifo.h"

static uint16_t FifoNext( Fifo_t *fifo, uint16_t index )
{
    return ( index + 1 ) % fifo->Size;
}

void FifoInit( Fifo_t *fifo, uint8_t *buffer, uint16_t size )
{
    fifo->Begin = 0;
    fifo->End = 0;
    fifo->Data = buffer;
    fifo->Size = size;
}

void FifoPush( Fifo_t *fifo, uint8_t data )
{
    fifo->End = FifoNext( fifo, fifo->End );
    fifo->Data[fifo->End] = data;
}

uint8_t FifoPop( Fifo_t *fifo )
{
    uint8_t data = fifo->Data[FifoNext( fifo, fifo->Begin )];

    fifo->Begin = FifoNext( fifo, fifo->Begin );
    return data;
}

void FifoFlush( Fifo_t *fifo )
{
    fifo->Begin = 0;
    fifo->End = 0;
}

bool IsFifoEmpty( Fifo_t *fifo )
{
    return ( fifo->Begin == fifo->End );
}

bool IsFifoFull( Fifo_t *fifo )
{
    return ( FifoNext( fifo, fifo->End ) == fifo->Begin );
}

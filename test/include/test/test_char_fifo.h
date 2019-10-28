#ifndef _TEST_CHAR_FIFO_H_
#define  _TEST_CHAR_FIFO_H_

#define FIFO_BUFFER_SIZE        5

extern k_chr_fifo_t test_fifo_00;
extern k_chr_fifo_t test_fifo_01;
extern k_chr_fifo_t test_fifo_02;

extern uint8_t fifo_buffer_00[FIFO_BUFFER_SIZE];
extern uint8_t fifo_buffer_01[FIFO_BUFFER_SIZE];
extern uint8_t fifo_buffer_02[FIFO_BUFFER_SIZE];

#endif


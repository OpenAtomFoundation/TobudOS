#ifndef _TEST_MMBLK_H_
#define  _TEST_MMBLK_H_

#define MMBLK_BLK_NUM       5
#define MMBLK_BLK_SIZE      0x20

extern k_mmblk_pool_t test_mmblk_pool_00;
extern k_mmblk_pool_t test_mmblk_pool_01;
extern k_mmblk_pool_t test_mmblk_pool_02;

extern uint8_t mmblk_pool_buffer_00[MMBLK_BLK_NUM * MMBLK_BLK_SIZE];
extern uint8_t mmblk_pool_buffer_01[MMBLK_BLK_NUM * MMBLK_BLK_SIZE];
extern uint8_t mmblk_pool_buffer_02[MMBLK_BLK_NUM * MMBLK_BLK_SIZE];

#endif


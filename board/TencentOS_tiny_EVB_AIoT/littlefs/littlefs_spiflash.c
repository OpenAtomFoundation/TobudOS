#include "tos_k.h"
#include "w25q64.h"
#include "lfs.h"

static k_mutex_t lfs_mutex;

static int lfs_spiflash_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    int ret;
    
    ret = w25qxx_read((uint8_t *)buffer, block * c->block_size + off, size);
    
    return ret == 0 ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_spiflash_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    int ret;
    
    ret = w25qxx_page_program((uint8_t *)buffer, block * c->block_size + off, size);
    
    return ret == 0 ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_spiflash_erase(const struct lfs_config *c, lfs_block_t block)
{
    int ret;
    
    ret = w25qxx_erase_sector(block * c->block_size);
    
    return ret == 0 ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_spiflash_sync(const struct lfs_config *c)
{
    return LFS_ERR_OK;
}

static int lfs_spiflash_lock(const struct lfs_config *c)
{
    k_err_t err;
    
    err = tos_mutex_pend(&lfs_mutex);
    
    return err == K_ERR_NONE ? LFS_ERR_OK : LFS_ERR_INVAL;
}

static int lfs_spiflash_unlock(const struct lfs_config *c)
{
    k_err_t err;
    
    err = tos_mutex_post(&lfs_mutex);
    
    return err == K_ERR_NONE ? LFS_ERR_OK : LFS_ERR_INVAL;
}

const struct lfs_config cfg = {
    // block device operations
    .read  = lfs_spiflash_read,
    .prog  = lfs_spiflash_prog,
    .erase = lfs_spiflash_erase,
    .sync  = lfs_spiflash_sync,
    .lock  = lfs_spiflash_lock,
    .unlock = lfs_spiflash_unlock,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = 128,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};

void littlefs_task()
{
    int err;
    lfs_t lfs;
    lfs_file_t file;
    
    char test_ctx[] = "hello,little fs!";
    char read_buf[16] = {0};
    lfs_ssize_t nbytes;
    
    tos_mutex_create(&lfs_mutex);
    
    memset(&lfs, 0, sizeof(lfs));
    err = lfs_mount(&lfs, &cfg);
    if (err != LFS_ERR_OK) {
        printf("mount fail, format...\r\n");
        err = lfs_format(&lfs, &cfg);
        printf("format fail: %d\r\n", err);
    }
    printf("mount success.\r\n");
    
    err = lfs_file_open(&lfs, &file, "test.txt", LFS_O_CREAT | LFS_O_RDWR);
    if (err != LFS_ERR_OK) {
        printf("open fail: %d\r\n", err);
    }
    printf("open success.\r\n");
    
    nbytes = lfs_file_write(&lfs, &file, test_ctx, sizeof(test_ctx));
    printf("write %d bytes.\r\n", nbytes);
    
    err = lfs_file_seek(&lfs, &file, 0, LFS_SEEK_SET);
    if (err != 0) {
        printf("seek fail: %d\r\n", err);
    }
    printf("seek success.\r\n");
    
    nbytes = lfs_file_read(&lfs, &file, read_buf, nbytes);
    read_buf[nbytes] = '\0';
    printf("read %d bytes.\r\n", nbytes);
    printf("read ctx:%s\r\n", read_buf);
    
    err = lfs_file_close(&lfs, &file);
    if (err != LFS_ERR_OK) {
        printf("close fail: %d\r\n", err);
    }
    printf("close success.\r\n");
   
    err = lfs_unmount(&lfs);
    if (err != LFS_ERR_OK) {
        printf("unmount fail, %d\r\n", err);
    }
    printf("umount success.\r\n");
    
    tos_mutex_destroy(&lfs_mutex);
}

void application_entry(void *arg)
{
    uint16_t device_id;
    
    w25qxx_init();
    
    printf("flash id is 0x%04x\r\n", w25qxx_read_deviceid());
    
    littlefs_task();
    
    while (1) {
        tos_task_delay(1000);
    }
}


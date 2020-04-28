#include "tos_k.h"
#include "tos_hal.h"
#include "ff.h"

char workbuf[4096];

void application_entry(void *arg)
{
    FRESULT res;
    uint32_t length;
    FIL file;
    FATFS sd_fatfs;

    res = f_mount(&sd_fatfs, "1:", 1);
    if (res != FR_OK) {
        printf("unknown fs. res = %d\ndo disk format....\n", res);
        res = f_mkfs("1:", FM_ANY, 0, workbuf, sizeof(workbuf));
        if (res == FR_OK) {
            f_setlabel("0:sample");
            res = f_mount(&sd_fatfs, "1:", 1);
        } else {
            printf("format error %d\n\r", res);
        }
    }

    res = f_open(&file, "1://test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        printf("open file error:%d\n\r",res);
    }

    res = f_write(&file, "fatfs sample content", strlen("fatfs sample content"), &length);
    if (res == FR_OK) {
        printf("write ok: %d\n", length);
        printf("write data:\n%s\n", "fatfs sample content");
    } else {
        printf("write error: %d\n", res);
    }
    f_close(&file);

    res = f_open(&file, "1://test_file.txt", FA_OPEN_EXISTING | FA_READ);
    if (res != FR_OK) {
        printf("open file error: %d\n", res);
    }

    memset(workbuf, 0, sizeof(workbuf));
    res = f_read(&file, workbuf, sizeof(workbuf), &length);
    if (res == FR_OK) {
        printf("read ok: %d\n",length);
        printf("read data:\n%s\n", workbuf);
    } else {
        printf("read error: %d\n", res);
    }
    
    f_sync(&file);
    
    f_close(&file);
}


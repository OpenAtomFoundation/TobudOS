#include "nvds.h"
#include "hal/flash.h"


static void nvds_read_flash(uint32_t offset, uint32_t len, uint8_t *buf)
{
    FLASH_ReadByCache(offset, len, buf);
}

static void nvds_write_flash(uint32_t offset, uint32_t len, uint8_t *buf)
{
    FLASH_Program(offset, len, buf);
}

static void nvds_erase_flash(uint32_t offset, uint32_t len)
{
    FLASH_Erase(offset, len);
}

static uint16_t nvds_get_valid_flag(void)
{
    uint16_t flag = 0;
    nvds_read_flash(USR_NVDS_FLAG_OFFSET, sizeof(uint16_t), (uint8_t *)&flag);
    return flag;
}

static void nvds_set_valid_flag(nvds_valid_sect_t sect)
{
    uint16_t flag = sect;
    nvds_write_flash(USR_NVDS_FLAG_OFFSET, sizeof(uint16_t), (uint8_t *)&flag);
}

static uint32_t nvds_get_magic_code(uint32_t offset)
{
    uint32_t code = 0;
    nvds_read_flash(offset, sizeof(uint32_t), (uint8_t *)&code);
    return code;
}
static void nvds_set_magic_code(uint32_t offset)
{
    uint32_t code = NVDS_SECT_MAGIC_CODE;
    nvds_write_flash(offset, sizeof(uint32_t), (uint8_t *)&code);
}


int nvds_write(uint32_t offset, uint8_t* data, uint16_t len)
{
    int ret = NVDS_FAIL;
    uint8_t rd_data = 0;
    
    if((offset + len) > SIZE_4KB){
        return ret;
    }

    if(nvds_get_valid_flag() == NVDS_SECT1_VALID)
    {
        nvds_erase_flash(USR_NVDS_SECT2_OFFSET, SIZE_4KB);

        nvds_set_magic_code(USR_NVDS_SECT2_OFFSET + SECT_MAGIC_CODE_OFFSET);

        for(uint32_t i = SECT_MAGIC_CODE_LEN; i < offset; i++){
            nvds_read_flash(USR_NVDS_SECT1_OFFSET + i, sizeof(uint8_t), &rd_data);
            if(rd_data != NVDS_NONE_TAG){
                nvds_write_flash(USR_NVDS_SECT2_OFFSET + i, sizeof(uint8_t), &rd_data);            
            }
        }

        nvds_write_flash(USR_NVDS_SECT2_OFFSET + offset, len, data);

        if((offset+len) < SIZE_4KB){
            for(uint32_t i = (offset+len); i < SIZE_4KB; i++){
                nvds_read_flash(USR_NVDS_SECT1_OFFSET + i, sizeof(uint8_t), &rd_data);
                if(rd_data != NVDS_NONE_TAG){
                    nvds_write_flash(USR_NVDS_SECT2_OFFSET + i, sizeof(uint8_t), &rd_data);
                }
            }
        }

        nvds_erase_flash(USR_NVDS_FLAG_OFFSET, SIZE_4KB);

        nvds_set_valid_flag(NVDS_SECT2_VALID);

        ret = NVDS_OK;
    }
    else
    {
        nvds_erase_flash(USR_NVDS_SECT1_OFFSET, SIZE_4KB);

        nvds_set_magic_code(USR_NVDS_SECT1_OFFSET + SECT_MAGIC_CODE_OFFSET);

        for(uint32_t i = SECT_MAGIC_CODE_LEN; i < offset; i++){
            nvds_read_flash(USR_NVDS_SECT2_OFFSET + i, sizeof(uint8_t), &rd_data);
            if(rd_data != NVDS_NONE_TAG){
                nvds_write_flash(USR_NVDS_SECT1_OFFSET + i, sizeof(uint8_t), &rd_data);
            }
        }

        nvds_write_flash(USR_NVDS_SECT1_OFFSET + offset, len, data);
        
        if((offset+len) < SIZE_4KB){
            for(uint32_t i = (offset+len); i < SIZE_4KB; i++){
                nvds_read_flash(USR_NVDS_SECT2_OFFSET + i, sizeof(uint8_t), &rd_data);
                if(rd_data != NVDS_NONE_TAG){
                    nvds_write_flash(USR_NVDS_SECT1_OFFSET + i, sizeof(uint8_t), &rd_data);
                }
            }
        }

        nvds_erase_flash(USR_NVDS_FLAG_OFFSET, SIZE_4KB);

        nvds_set_valid_flag(NVDS_SECT1_VALID);

        ret = NVDS_OK;
    }
    
    return ret;
}

int nvds_read(uint32_t offset, uint8_t* data, uint16_t len)
{
    int ret = NVDS_FAIL;
    
    if((offset + len) > SIZE_4KB){
        return ret;
    }
    
    if(nvds_get_valid_flag() == NVDS_SECT1_VALID)
    {
        nvds_read_flash(USR_NVDS_SECT1_OFFSET+offset, len, data);
        ret = NVDS_OK;
    }
    else if(nvds_get_valid_flag() == NVDS_SECT2_VALID)
    {
        nvds_read_flash(USR_NVDS_SECT2_OFFSET+offset, len, data);
        ret = NVDS_OK;
    }
    else
    {
        if(nvds_get_magic_code(USR_NVDS_SECT2_OFFSET+SECT_MAGIC_CODE_OFFSET) == NVDS_SECT_MAGIC_CODE)
        {
            nvds_read_flash(USR_NVDS_SECT2_OFFSET+offset, len, data);//default: sect2
            ret = NVDS_OK;
        }
        else 
        {
            ret = NVDS_FAIL;
        }
    }
    
    return ret;
}





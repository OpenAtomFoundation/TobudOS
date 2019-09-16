/*!
    \file    gd32f20x_nandflash_eval.c
    \brief   nandflash(hynix HY27UF081G2A) driver

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f20x.h"
#include "gd32f20x_nandflash_eval.h"

/* defines the physical address of nand flash, and it is determined by the hardware */
#define BANK1_NAND_ADDR     ((uint32_t)0x70000000)
#define BANK2_NAND_ADDR     ((uint32_t)0x80000000)
#define BANK_NAND_ADDR      BANK1_NAND_ADDR

/* define operating nand flash macro */
#define NAND_CMD_AREA       *(__IO uint8_t *)(BANK_NAND_ADDR | EXMC_CMD_AREA)
#define NAND_ADDR_AREA      *(__IO uint8_t *)(BANK_NAND_ADDR | EXMC_ADDR_AREA)
#define NAND_DATA_AREA      *(__IO uint8_t *)(BANK_NAND_ADDR | EXMC_DATA_AREA)

/* the macro of calculate nand flash operating address */
#define ROW_ADDRESS         (address.page + (address.block + (address.zone * NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)

/* page bit count per block */
#define PAGE_BIT            6

/* function prototypes */
static uint8_t exmc_nand_getstatus(void);
static uint8_t exmc_nand_writedata(uint8_t *pbuffer, nand_address_struct physicaladdress, uint16_t bytecount);
static uint8_t exmc_nand_readdata(uint8_t *pbuffer, nand_address_struct phyaddress, uint16_t bytecount);
static uint8_t exmc_nand_writepage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount);
static uint8_t exmc_nand_readpage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount);
static uint8_t exmc_nand_eraseblock(uint32_t blocknum);
static uint8_t exmc_nand_readstatus(void);

/*!
    \brief      nand flash peripheral initialize
    \param[in]  sdram_device: specifie the SDRAM device
    \param[out] none
    \retval     none
*/
void exmc_nandflash_init(uint32_t nand_bank)
{
    exmc_nand_parameter_struct nand_init_struct;
    exmc_nand_pccard_timing_parameter_struct nand_timing_init_struct;

    /* enable EXMC clock*/
    rcu_periph_clock_enable(RCU_EXMC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* GPIOD configuration */
    /* D0(PD14),D1(PD15),D2(PD0),D3(PD1),A16(PD11),A17(PD12),NE1(PD7),NOE(PD4),NWE(PD5),NWAIT(PD6) pin configuration */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15 |
                                                         GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
      
    /* GPIOE configuration */
    /* D4(PE7),D5(PE8),D6(PE9),D7(PE10) pin configuration */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

    /* EXMC configuration */
    nand_timing_init_struct.setuptime = 1U;
    nand_timing_init_struct.waittime = 3U;
    nand_timing_init_struct.holdtime = 2U;
    nand_timing_init_struct.databus_hiztime = 2U;

    nand_init_struct.nand_bank = EXMC_BANK1_NAND;
    nand_init_struct.ecc_size = EXMC_ECC_SIZE_2048BYTES;
    nand_init_struct.atr_latency = EXMC_ALE_RE_DELAY_1_HCLK;
    nand_init_struct.ctr_latency = EXMC_CLE_RE_DELAY_1_HCLK;
    nand_init_struct.ecc_logic = ENABLE;
    nand_init_struct.databus_width = EXMC_NAND_DATABUS_WIDTH_8B;
    nand_init_struct.wait_feature = ENABLE;
    nand_init_struct.common_space_timing = &nand_timing_init_struct;
    nand_init_struct.attribute_space_timing = &nand_timing_init_struct;

    exmc_nand_init(&nand_init_struct);

    /* enable EXMC NAND bank1 */
    exmc_nand_enable(EXMC_BANK1_NAND);
}


/*!
    \brief      read NAND flash ID
    \param[in]  nand_id: structure of nand flash ID
    \param[out] none
    \retval     none
*/
void nand_read_id(nand_id_struct* nand_id)
{
    uint32_t data = 0U;

    /* send command to the command area */
    NAND_CMD_AREA = NAND_CMD_READID;

    /* send address to the address area */
    NAND_ADDR_AREA = 0x00U;

    /* read id from NAND flash */
    data = *(__IO uint32_t *)(BANK_NAND_ADDR | EXMC_DATA_AREA);

    nand_id->maker_id = ADDR_1ST_CYCLE (data);
    nand_id->device_id = ADDR_2ND_CYCLE (data);
    nand_id->third_id = ADDR_3RD_CYCLE (data);
    nand_id->fourth_id = ADDR_4TH_CYCLE (data);
}

/*!
    \brief      write a set of data to nand flash for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be written
    \param[in]  address: the address of the data to be written
    \param[in]  bytecount: byte count to be written(bytecount + address.page_in_offset <= nand_page_total_size)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_writepage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount)
{
    uint16_t i;
    uint32_t pbuffer_addr;
    
    /* send 1st cycle page programming command to the command area */
    NAND_CMD_AREA = NAND_CMD_WRITE_1ST;

    /* send address to the address area, for HY27UF081G2A
                    bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
       first byte:  A7   A6   A5   A4   A3   A2   A1   A0    (bit7 - bit0 of page address)
       second byte: 0    0    0    0    A11  A10  A9   A8    (bit11 - bit8 of page address, high 4bit must be zero)
       third byte£º A19  A18  A17  A16  A15  A14  A13  A12
       fourth byte£ºA27  A26  A25  A24  A23  A22  A21  A20
    */
    NAND_ADDR_AREA = (uint8_t)address.page_in_offset;
    NAND_ADDR_AREA = (uint8_t)(address.page_in_offset >> 8);
    NAND_ADDR_AREA = ADDR_1ST_CYCLE(ROW_ADDRESS);
    NAND_ADDR_AREA = ADDR_2ND_CYCLE(ROW_ADDRESS);

    pbuffer_addr = (uint32_t)pbuffer;
    /* write data to data area */
    for(i = 0U; i < bytecount; i++){
        NAND_DATA_AREA = *(uint8_t *)pbuffer_addr;
        pbuffer_addr++;
    }

    /* send 2nd cycle page programming command to the command area */
    NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

    /* check operation stauts */
    if (NAND_READY == exmc_nand_getstatus()){
        return NAND_OK;
    }
    return NAND_FAIL;
}

/*!
    \brief      read a set of data from nand flash for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer filling data to be read
    \param[in]  address: the address of the data to be read
    \param[in]  bytecount: byte count to be read(bytecount + address.page_in_offset <= NAND_PAGE_TOTAL_SIZE)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_readpage(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount)
{
    uint16_t i;
    uint32_t pbuffer_addr;
    /* send 1st cycle read command to the command area */
    NAND_CMD_AREA = NAND_CMD_READ1_1ST;

     /* send address to the address area, for HY27UF081G2A
                     bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
        first byte:  A7   A6   A5   A4   A3   A2   A1   A0    (bit7 - bit0 of page address)
        second byte: 0    0    0    0    A11  A10  A9   A8    (bit11 - bit8 of page address, high 4bit must be zero)
        third byte£º A19  A18  A17  A16  A15  A14  A13  A12
        fourth byte£ºA27  A26  A25  A24  A23  A22  A21  A20
    */
    NAND_ADDR_AREA = (uint8_t)address.page_in_offset;
    NAND_ADDR_AREA = (uint8_t)(address.page_in_offset >> 8);
    NAND_ADDR_AREA = ADDR_1ST_CYCLE(ROW_ADDRESS);
    NAND_ADDR_AREA = ADDR_2ND_CYCLE(ROW_ADDRESS);
    
    /* send 2nd cycle read command to the command area */
    NAND_CMD_AREA = NAND_CMD_READ1_2ND;
    
    pbuffer_addr = (uint32_t)pbuffer;            
    /* read data to pbuffer */
    for(i = 0U; i < bytecount; i++){
        *(uint8_t *)pbuffer_addr = NAND_DATA_AREA; 
        pbuffer_addr++;
    }

    /* check operation stauts */
    if (NAND_READY == exmc_nand_getstatus()){
        return NAND_OK;
    }
    return NAND_FAIL;
}

/*!
    \brief      write the spare area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be written
    \param[in]  address: the address of the data to be written
    \param[in]  bytecount: byte count to be written(bytecount + (address.page_in_offset - NAND_PAGE_SIZE) <= NAND_SPARE_AREA_SIZE)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t exmc_nand_writespare(uint8_t *pbuffer, nand_address_struct address, uint16_t bytecount)
{
    /* address.page_in_offset > NAND_PAGE_SIZE */
    if(address.page_in_offset <= NAND_PAGE_SIZE){
        return NAND_FAIL;
    }

    /* bytecount + address.page_in_offset < NAND_PAGE_TOTAL_SIZE */
    if (bytecount + address.page_in_offset >= NAND_PAGE_TOTAL_SIZE){
        return NAND_FAIL;
    }

    /* write spare area */
    return exmc_nand_writepage(pbuffer, address, bytecount);
}

/*!
    \brief      read the spare area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be read
    \param[in]  address: the address of the data to be read
    \param[in]  bytecount: byte count to be read(bytecount + (address.page_in_offset - NAND_PAGE_SIZE) <= NAND_SPARE_AREA_SIZE)
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t exmc_nand_readspare(uint8_t *pbuffer,nand_address_struct address, uint16_t bytecount)
{
    /* address.page_in_offset > NAND_PAGE_SIZE */
    if(address.page_in_offset <= NAND_PAGE_SIZE){
        return NAND_FAIL;
    }

    /* bytecount + address.page_in_offset < NAND_PAGE_TOTAL_SIZE */
    if (bytecount + address.page_in_offset >= NAND_PAGE_TOTAL_SIZE){
        return NAND_FAIL;
    }

    /* read spare area */
    return exmc_nand_readpage(pbuffer, address, bytecount);
}

/*!
    \brief      write the main area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be written
    \param[in]  physicaladdress: the address of the data to be written
    \param[in]  bytecount: byte count to be written
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_writedata(uint8_t *pbuffer, nand_address_struct physicaladdress, uint16_t bytecount)
{
    uint32_t pbuffer_addr;
    
    /* erase block before write data based on the feature of nand flash */
    exmc_nand_eraseblock((uint32_t)physicaladdress.block);

    /* if the number of data bytes to be written plus the offset is greater than the page size,
       the automatic next page */
    while(bytecount + physicaladdress.page_in_offset > NAND_PAGE_SIZE){
        if(exmc_nand_writepage(pbuffer, physicaladdress, NAND_PAGE_SIZE - physicaladdress.page_in_offset) != NAND_OK){
            return NAND_FAIL;
        }
        /* compute address of the next block */
        bytecount -= NAND_PAGE_SIZE - physicaladdress.page_in_offset;
        pbuffer_addr = (uint32_t)pbuffer;
        pbuffer_addr += NAND_PAGE_SIZE - (uint32_t)physicaladdress.page_in_offset;
        pbuffer = (uint8_t *)pbuffer_addr;
        physicaladdress.page++;
        physicaladdress.page_in_offset = 0U;
    }

    /* write the last less than one block of data */
    if(bytecount > 0U){
        if(exmc_nand_writepage(pbuffer, physicaladdress,bytecount) != NAND_OK){
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      read the main area information for the specified pages addresses
    \param[in]  pbuffer: pointer on the buffer containing data to be read
    \param[in]  phyaddress: the address of the data to be read
    \param[in]  bytecount: byte count to be read
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
static uint8_t exmc_nand_readdata(uint8_t *pbuffer, nand_address_struct phyaddress, uint16_t bytecount)
{
    uint32_t pbuffer_addr; 
    /* if the number of data bytes to be read plus the offset is greater than the page size, the automatic next page */
    while(bytecount + phyaddress.page_in_offset > NAND_PAGE_SIZE){
        if(exmc_nand_readpage(pbuffer, phyaddress, NAND_PAGE_SIZE - phyaddress.page_in_offset) != NAND_OK){
            return NAND_FAIL;
        }
        phyaddress.page++;
        pbuffer_addr = (uint32_t)pbuffer;
        pbuffer_addr += NAND_PAGE_SIZE - (uint32_t)phyaddress.page_in_offset;
        pbuffer = (uint8_t *)pbuffer_addr;
        bytecount -= NAND_PAGE_SIZE - phyaddress.page_in_offset;
        phyaddress.page_in_offset = 0U;
    }

    if(bytecount > 0U){
        if(exmc_nand_readpage(pbuffer, phyaddress, bytecount) != NAND_OK){
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      erase data specified block
    \param[in]  blocknum: block number to be erased data
    \param[out] none
    \retval     NAND memory status
*/
static uint8_t exmc_nand_eraseblock(uint32_t blocknum)
{
    /* send 1st cycle erase command to command area */
    NAND_CMD_AREA = NAND_CMD_ERASE_1ST;

    /* block number into a block number and the page number */
    blocknum <<= PAGE_BIT;
    NAND_ADDR_AREA = ADDR_1ST_CYCLE(blocknum);
    NAND_ADDR_AREA = ADDR_2ND_CYCLE(blocknum);

    /* send 2nd cycle erase command to command area */
    NAND_CMD_AREA = NAND_CMD_ERASE_2ND;

    return (exmc_nand_getstatus());
}

/*!
    \brief      reset nand flash
    \param[in]  none
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_reset(void)
{
    NAND_CMD_AREA = NAND_CMD_RESET;

    /* check operation stauts */
    if (NAND_READY == exmc_nand_getstatus()){
        return NAND_OK;
    }

    return NAND_FAIL;
}

/*!
    \brief      reads the NAND memory status
    \param[in]  none
    \param[out] none
    \retval     NAND memory status
*/
static uint8_t exmc_nand_readstatus(void)
{
    uint8_t data;
    uint8_t status = NAND_BUSY;

    /* send read status command to the command area */
    NAND_CMD_AREA = NAND_CMD_STATUS;

    data = NAND_DATA_AREA;

    if((data & NAND_ERROR) == NAND_ERROR){
        status = NAND_ERROR;
    }
    else if((data & NAND_READY) == NAND_READY){
        status = NAND_READY;
    }
    else{
        status = NAND_BUSY;
    }

    return (status);
}

/*!
    \brief      get the NAND operation status
    \param[in]  none
    \param[out] none
    \retval     new status of the NAND operation
*/
static uint8_t exmc_nand_getstatus(void)
{
    uint32_t timeout = 0x10000U;
    uint8_t status = NAND_READY;

    status = exmc_nand_readstatus();

    /* waiting for NAND operation over, it will exit after a timeout */
    while ((status != NAND_READY) && (timeout != 0x00U)){
        status = exmc_nand_readstatus();
        timeout--;
    }

    if(timeout == 0x00U){
        status =  NAND_TIMEOUT_ERROR;
    }

    return (status);
}

/*!
    \brief      write the main area information for the specified logic addresses
    \param[in]  memaddr: the logic address of the data to be written
    \param[in]  pwritebuf: pointer on the buffer containing data to be written
    \param[in]  bytecount: byte count to be written
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_write(uint32_t memaddr, uint8_t *pwritebuf, uint32_t bytecount)
{
    uint32_t temp_blockremainsize;
    uint32_t pwritebuf_addr;
    nand_address_struct  physicaladdress;
    uint32_t  temp;

    temp = memaddr % (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);
    
    /* compute physical zone number */
    physicaladdress.zone= (uint16_t)(memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE * NAND_ZONE_SIZE));

    /* compute physical block number */
    physicaladdress.block = (uint16_t)(memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE));

    /* compute physical page number */
    physicaladdress.page = (uint16_t)(temp / NAND_PAGE_SIZE);

    /* compute physical offset into page  */
    physicaladdress.page_in_offset = (uint16_t)(temp % NAND_PAGE_SIZE);
    temp_blockremainsize = (NAND_BLOCK_SIZE * NAND_PAGE_SIZE)-(NAND_PAGE_SIZE * (uint32_t)physicaladdress.page + 
                          (uint32_t)physicaladdress.page_in_offset);

    /* if the number of data bytes to be written plus the offset is greater than the block size, the automatic next block. */
    while(bytecount > temp_blockremainsize){
        if(NAND_FAIL == exmc_nand_writedata(pwritebuf,physicaladdress, (uint16_t)temp_blockremainsize)){
            return NAND_FAIL;
        }

        physicaladdress.block++;
        pwritebuf_addr = (uint32_t)pwritebuf;
        pwritebuf_addr += temp_blockremainsize;
        pwritebuf = (uint8_t *)pwritebuf_addr;
        bytecount -= temp_blockremainsize;
        physicaladdress.page = 0U;
        physicaladdress.page_in_offset = 0U;
        temp_blockremainsize = (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);
    }

    if(bytecount > 0U){
        if(NAND_FAIL == exmc_nand_writedata(pwritebuf, physicaladdress, (uint16_t)bytecount)){
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      read the main area information for the specified logic addresses
    \param[in]  memaddr: the logic address of the data to be read
    \param[in]  preadbuf: pointer on the buffer containing data to be read
    \param[in]  bytecount: byte count to be reas
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_read(uint32_t memaddr, uint8_t *preadbuf, uint32_t bytecount)
{
    uint32_t temp_blockremainsize;
    nand_address_struct  physicaladdress;
    uint32_t  temp;
    uint32_t preadbuf_addr;

    temp = memaddr % (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);

    /* compute physical zone number */
    physicaladdress.zone= (uint16_t)(memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE * NAND_ZONE_SIZE));

    /* compute physical block number */
    physicaladdress.block = (uint16_t)(memaddr / (NAND_BLOCK_SIZE * NAND_PAGE_SIZE));
      
    /* compute physical page number */
    physicaladdress.page = (uint16_t)(temp / NAND_PAGE_SIZE);

    /* compute physical offset into page  */
    physicaladdress.page_in_offset = (uint16_t)(temp % NAND_PAGE_SIZE);

    temp_blockremainsize=(NAND_BLOCK_SIZE * NAND_PAGE_SIZE) - ((NAND_PAGE_SIZE * (uint32_t)physicaladdress.page) + (uint32_t)physicaladdress.page_in_offset);

    /* if the number of data bytes to be read plus the offset is greater than the block size, the automatic next block */
    while(bytecount > temp_blockremainsize){
        if(NAND_FAIL == exmc_nand_readdata(preadbuf,physicaladdress, (uint16_t)temp_blockremainsize)){
            return NAND_FAIL;
        }
        physicaladdress.block++;
        preadbuf_addr = (uint32_t)preadbuf;
        preadbuf_addr += temp_blockremainsize;
        preadbuf = (uint8_t *)preadbuf_addr;
        bytecount -= temp_blockremainsize;
        physicaladdress.page = 0U;
        physicaladdress.page_in_offset = 0U;
        temp_blockremainsize = (NAND_BLOCK_SIZE * NAND_PAGE_SIZE);
    }

    if(bytecount > 0U){
        if(exmc_nand_readdata(preadbuf,physicaladdress, (uint16_t)bytecount) == NAND_FAIL){
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      format nand flash
    \param[in]  none
    \param[out] none
    \retval     NAND_OK, NAND_FAIL
*/
uint8_t nand_format(void)
{
    uint32_t i;

    for (i = 0U; i < NAND_BLOCK_COUNT; i++){
        if(NAND_READY != exmc_nand_eraseblock(i)){
            return NAND_FAIL;
        }
    }

    return NAND_OK;
}

/*!
    \brief      fill the buffer with specified value
    \param[in]  pbuffer: pointer on the buffer to fill
    \param[in]  buffer_lenght: size of the buffer to fill
    \param[in]  value: value to fill on the buffer
    \param[out] none
    \retval     none
*/
void fill_buffer_nand(uint8_t *pbuffer, uint16_t buffer_lenght, uint32_t value)
{
    uint16_t index = 0U;
    uint32_t buf_addr;
    
    buf_addr = (uint32_t)pbuffer;
    
    for (index = 0U; index < buffer_lenght; index++){
        *(uint8_t *)buf_addr = (uint8_t)(value + index);
        buf_addr++;
    }
}

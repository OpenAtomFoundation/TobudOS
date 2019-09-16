/**
  ******************************************************************************
  * @file    nfc.c
  * @author  YANDLD
  * @version V2.5
  * @date    2015.3.5
  * \date    2015.10.04 FreeXc完善了nfc模块的注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
 #include "common.h"
 #include "nfc.h"


#ifdef NFC

/* Defines for NAND command values */
#define NFC_READ_PAGE_CMD_CODE        	    0x7EF8
#define NFC_PROGRAM_PAGE_CMD_CODE           0x7FD8
#define NFC_DMA_PROGRAM_PAGE_CMD_CODE       0xFFC0  
#define NFC_BLOCK_ERASE_CMD_CODE      	    0x4ED8    
#define NFC_READ_ID_CMD_CODE          	    0x4804
#define NFC_RESET_CMD_CODE            	    0x4040

#define PAGE_READ_CMD_BYTE1                 0x00
#define PAGE_READ_CMD_BYTE2                 0x30
#define PROGRAM_PAGE_CMD_BYTE1              0x80
#define PROGRAM_PAGE_CMD_BYTE2              0x10
#define READ_STATUS_CMD_BYTE                0x70
#define BLOCK_ERASE_CMD_BYTE1               0x60
#define BLOCK_ERASE_CMD_BYTE2               0xD0
#define READ_ID_CMD_BYTE                    0x90
#define RESET_CMD_BYTE                      0xFF

    
static volatile uint32_t NFCBufAddr[4] =
{
    (NFC_BASE + 0x00000000),
    (NFC_BASE + 0x00001000),
    (NFC_BASE + 0x00002000),
    (NFC_BASE + 0x00003000),
};

/**
 * \brief get buffer address of NFC
 * \param[in] nfcbufNum buffer num
 * \return buffer address
 */
uint32_t NFC_GetBufAddr(uint8_t nfcbufNum)
{
    return NFCBufAddr[nfcbufNum];
}

/**
 * @brief  initialize NFC struct
 * @param[in]  NFC_InitStruct 指向NFC初始化结构体指针
 * @retval None
 */
void NFC_Init(NFC_InitTypeDef *NFC_InitStruct)
{
    /* Enable the NFC clock gate in the SIM */
    SIM->SCGC3 |= SIM_SCGC3_NFC_MASK; 	
    
    /* Disable the MPU to allow NFC to access memory */
    MPU->CESR &= 0xFFFFFFFE;
    
    /* NFC clock is bus clock */
    SIM->SOPT2 &= SIM_SOPT2_NFCSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_NFCSRC(0);
    
    /* NFC clock = Busclock/4 */
    SIM->CLKDIV4 &= ~SIM_CLKDIV4_NFCDIV_MASK;
    SIM->CLKDIV4 &= ~SIM_CLKDIV4_NFCFRAC_MASK;
    SIM->CLKDIV4 |= SIM_CLKDIV4_NFCDIV(3) |SIM_CLKDIV4_NFCFRAC(0);
    
    /* Enable all of the NFC control signal pads */
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK|SIM_SCGC5_PORTB_MASK|SIM_SCGC5_PORTC_MASK|SIM_SCGC5_PORTD_MASK|SIM_SCGC5_PORTE_MASK;
    
    PORTC->PCR[16] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_RB pad */
    PORTD->PCR[8] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_CLE pad */
    PORTD->PCR[9] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_ALE pad */
    PORTD->PCR[10] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_RE pad */
    PORTC->PCR[11] =  PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_WE pad */
    
    /* chip select */
    switch(NFC_InitStruct->cs)
    {
        case 0:
            NFC->RAR |= (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
            PORTC->PCR[17] = PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;  /* Enable NFC_CE0 pad */
            break;
        case 1:
            NFC->RAR |= (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
            PORTC->PCR[18] = PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;  /* Enable NFC_CE1 pad */   
            break;
        default:
            break;
    }
    
    /* data width and sector size */
    switch(NFC_InitStruct->width)
    {
        case kNFC_Width8Bit:
            NFC->CFG &= ~NFC_CFG_BITWIDTH_MASK;
            NFC->SECSZ = NFC_InitStruct->sectorSize;
        
            PORTC->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA7 pad */
            PORTC->PCR[6] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA6 pad */
            PORTC->PCR[7] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA5 pad */
            PORTC->PCR[8] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA4 pad */
            PORTC->PCR[9] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA3 pad */
            PORTC->PCR[10] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA2 pad */
            PORTD->PCR[4] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA1 pad */
            PORTD->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA0 pad */
        
            break;
        case kNFC_Width16Bit:
            NFC->CFG |= NFC_CFG_BITWIDTH_MASK;
            NFC->SECSZ = NFC_InitStruct->sectorSize + 1;
        
            PORTB->PCR[20] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA15 pad */
            PORTB->PCR[21] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA14 pad */
            PORTB->PCR[22] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA13 pad */
            PORTB->PCR[23] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA12 pad */
            PORTC->PCR[0] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA11 pad */ 
            PORTC->PCR[1] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA10 pad */
            PORTC->PCR[2] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA9 pad */
            PORTC->PCR[4] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA8 pad */
            PORTC->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA7 pad */
            PORTC->PCR[6] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA6 pad */
            PORTC->PCR[7] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA5 pad */
            PORTC->PCR[8] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA4 pad */
            PORTC->PCR[9] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA3 pad */
            PORTC->PCR[10] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA2 pad */
            PORTD->PCR[4] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA1 pad */
            PORTD->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA0 pad */
            
            break;
        default:
            break;
    }
    
    /* disable EDO */
    NFC->CFG &= ~NFC_CFG_FAST_MASK;
    
    /* Disable auto address and buff num increment */
    NFC->CFG &= ~(NFC_CFG_AIAD_MASK | NFC_CFG_AIBN_MASK);   
    
    /* Set NFC to STOP if a write error is detected */
    NFC->CFG |= NFC_CFG_STOPWERR_MASK;
    
    /* Disable row address increment */
    NFC->RAI = 0;
    
    /* Disable swap */
    NFC->SWAP = 0;
    
}

/**
 * \brief send NFC reset cmd
 * \retval None
 */
void NFC_SendResetCmd(void) 
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Write the NFC_CMD2 register with the command byte and code for a reset */
    NFC->CMD2 = NFC_CMD2_BYTE1(RESET_CMD_BYTE) | NFC_CMD2_CODE(NFC_RESET_CMD_CODE);
	
    /* Set Start Bit to send reset to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */ 
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
}

/**
 * \brief read NFC flash 
 * \param[in] nfcBufNum     first NFC internal buffer to program to the NAND (auto increment will be used )
 * \param[out] id0 store the NFC flash ID0(SR1)
 * \param[out] id1 store the NFC flash ID1(SR2)
 * \retval None
 */
void NFC_ReadFlashID(uint8_t nfcBufNum, uint32_t* id0, uint32_t* id1) 
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Write the NFC_CMD2 register with the command byte and code for a reset */
    NFC->CMD2 = NFC_CMD2_BYTE1(READ_ID_CMD_BYTE) 
              | NFC_CMD2_CODE(NFC_READ_ID_CMD_CODE)
              | NFC_CMD2_BUFNO(nfcBufNum);
	
    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */ 
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
 
    /* Load the flash ID values into fls_id0 and fls_id1 */
    *((volatile uint32_t *)(id0)) = NFC->SR1;
    *((volatile uint32_t *)(id1)) = NFC->SR2;
}

/**
 * \brief Erase NFC block
 * \details Erases the NFC block containing the specified address. This function
 *          will only erase one block at a time. If multiple blocks need to be
 *          erased, then the function should be called once per block to erase.
 *
 * \param[in] cs chip select
 * \param[in] row_addr   NAND flash row addr for the block to erase (up to 24 bits) 
 * \note  raw_addr: how many page in this chip, there is no block addr meaning, so in blockerase fun:, raw_addr[0:5] is ignored
 *        coloum_addr: how many byte in a page,
 */
void NFC_BlockErase(uint32_t cs, uint32_t row_addr)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );
    
    /* Disable ECC during block erase */
    NFC->CFG &= ~NFC_CFG_ECCMODE(0x7);
    
    /* Make sure the column address is cleared - not needed for block erase */
    NFC->CAR = 0x0;

    /* Set the chip select to use */
    switch(cs)
    {
        case 0:
            NFC->RAR = (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
            break;
        case 1:
            NFC->RAR = (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
            break;
        default:
            break;
    }

    /* Set the row address */
    NFC->RAR |= row_addr;      

    /* Write the NFC_CMD2 register with the command byte and code for an erase */
    NFC->CMD2 = (NFC_CMD2_BYTE1(BLOCK_ERASE_CMD_BYTE1) 
              | NFC_CMD2_CODE(NFC_BLOCK_ERASE_CMD_CODE) );
    
    /* Write the NFC_CMD1 register with the command byte2 and byte3 for an erase */
    NFC->CMD1 = NFC_CMD1_BYTE2(BLOCK_ERASE_CMD_BYTE2) 
                | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE);    
	
    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */ 
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
}


/**
 * \brief NFC page prpgram
 * \details Programs a single page worth of data from the NFC into the NAND
 *          flash memory at the specified NFC address. This function will only
 *          program one NAND page at a time. If multiple pages need to be
 *          programmed, then the function should be called once per page to
 *          write. Data must be loaded into the NFC's buffers before calling
 *          this function.
 *
 * \param[in] cs            chip select
 * \param[in] nfcbufNum     first NFC internal buffer to program to the NAND (auto increment will be used )
 * \param[in] row_addr      NAND flash row addr for the block to program (up to 24 bits)
 * \param[in] col_addr      NAND flash col addr for the page to program (up to 16 bits)
 * \note the column address should be aligned to the NAND page size
 */

void NFC_PageProgram(uint8_t cs, uint8_t nfcbufNum, uint32_t row_addr, uint16_t col_addr)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );
        
    /* Make sure ECC is enabled before reading */
    NFC->CFG |= NFC_CFG_ECCMODE(0x7);

    /* Set the chip select to use */
    switch(cs)
    {
        case 0:
            NFC->RAR = (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
            break;
        case 1:
            NFC->RAR = (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
            break;
        default:
            break;
    }

    /* Set the row address */
    NFC->RAR |= row_addr;      
    
    /* Set the column address */
    NFC->CAR = col_addr;
            
    /* Write the NFC_CMD2 register with the command byte and code for an erase */
    NFC->CMD2 = (NFC_CMD2_BYTE1(PROGRAM_PAGE_CMD_BYTE1) 
              | NFC_CMD2_CODE(NFC_PROGRAM_PAGE_CMD_CODE) 
              | NFC_CMD2_BUFNO(nfcbufNum));
    
    /* Write the NFC_CMD1 register with the command byte2 and byte3 for an erase */
    NFC->CMD1 = NFC_CMD1_BYTE2(PROGRAM_PAGE_CMD_BYTE2) 
                | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE);    

    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */ 
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);  
}
/** 
 * \brief NFC page read
 * \details Reads a single page worth of data from the NAND flash at the 
 *          specified address into the NFC's buffers. This function will only
 *          read one NAND page at a time. If multiple pages need to be
 *          read, then the function should be called once per page to
 *          read. Data will be loaded into the NFC's buffers after the function
 *          completes.
 *
 * \param[in] cs            chip select
 * \param[in] nfcBufNum     first NFC internal buffer to program to the NAND (auto increment will be used )
 * \param[in] row_addr      NAND flash row addr for the block to program (up to 24 bits)
 * \param[in] col_addr      NAND flash col addr for the page to program (up to 16 bits)
 * \note the column address should be aligned to the NAND page size
 */
void NFC_PageRead(uint8_t cs, uint8_t nfcBufNum, uint32_t row_addr, uint16_t col_addr)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );
    
    /* Make sure ECC is enabled before reading */
    NFC->CFG |= NFC_CFG_ECCMODE(0x7);
        
    /* Set the chip select to use */
    switch(cs)
    {
        case 0:
            NFC->RAR = (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
            break;
        case 1:
            NFC->RAR = (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
            break;
        default:
            break;
    }

    /* Set the row address */
    NFC->RAR |= row_addr;      
    
    /* Set the column address */
    NFC->CAR = col_addr;
    
    /* Write the NFC_CMD2 register with the command byte and code for an erase */
    NFC->CMD2 = (NFC_CMD2_BYTE1(PAGE_READ_CMD_BYTE1) 
              | NFC_CMD2_CODE(NFC_READ_PAGE_CMD_CODE) 
              | NFC_CMD2_BUFNO(nfcBufNum));
    
    /* Write the NFC_CMD1 register with the command byte2 and byte3 for an erase */
    NFC->CMD1 = NFC_CMD1_BYTE2(PAGE_READ_CMD_BYTE2) 
                | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE);    

    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */ 
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);  
}

#endif

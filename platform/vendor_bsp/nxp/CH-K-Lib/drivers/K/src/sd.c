/**
  ******************************************************************************
  * @file    sd.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @date    2015.10.04 FreeXc 完善了 sd 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "sd.h"
#include "gpio.h"
#include "common.h"

#if defined(SDHC)

static const Reg_t ClkTbl[] =
{
#ifdef SIM_SCGC3_SDHC_MASK
    {(void*)&(SIM->SCGC3), SIM_SCGC3_SDHC_MASK},
#else
    {(void*)&(SIM->SCGC3), SIM_SCGC3_ESDHC_MASK},
#endif
};


static struct sd_card_handler sdh;


#define ESDHC_BUS_WIDTH_1BIT                 (0x00)
#define ESDHC_BUS_WIDTH_4BIT                 (0x01)
#define ESDHC_BUS_WIDTH_8BIT                 (0x02)

#define ESDHC_XFERTYP_CMDTYP_NORMAL          (0x00)
#define ESDHC_XFERTYP_CMDTYP_SUSPEND         (0x01)
#define ESDHC_XFERTYP_CMDTYP_RESUME          (0x02)
#define ESDHC_XFERTYP_CMDTYP_ABORT           (0x03)
/* Response Type Select */
#define ESDHC_XFERTYP_RSPTYP_NO              (0x00)
#define ESDHC_XFERTYP_RSPTYP_136             (0x01)
#define ESDHC_XFERTYP_RSPTYP_48              (0x02)
#define ESDHC_XFERTYP_RSPTYP_48BUSY          (0x03)

#define ESDHC_CMD0   (SDHC_XFERTYP_CMDINX(0)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD1   (SDHC_XFERTYP_CMDINX(1)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD2   (SDHC_XFERTYP_CMDINX(2)  | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
#define ESDHC_CMD3   (SDHC_XFERTYP_CMDINX(3)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD4   (SDHC_XFERTYP_CMDINX(4)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD5   (SDHC_XFERTYP_CMDINX(5)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD6   (SDHC_XFERTYP_CMDINX(6)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD6  (SDHC_XFERTYP_CMDINX(6)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD7   (SDHC_XFERTYP_CMDINX(7)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD8   (SDHC_XFERTYP_CMDINX(8)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD9   (SDHC_XFERTYP_CMDINX(9)  | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
#define ESDHC_CMD10  (SDHC_XFERTYP_CMDINX(10) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
#define ESDHC_CMD11  (SDHC_XFERTYP_CMDINX(11) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD12  (SDHC_XFERTYP_CMDINX(12) | SDHC_XFERTYP_CMDTYP(ESDHC_XFERTYP_CMDTYP_ABORT) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD13  (SDHC_XFERTYP_CMDINX(13) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD13 (SDHC_XFERTYP_CMDINX(13) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD15  (SDHC_XFERTYP_CMDINX(15) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD16  (SDHC_XFERTYP_CMDINX(16) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD17  (SDHC_XFERTYP_CMDINX(17) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_DTDSEL_MASK | SDHC_XFERTYP_DPSEL_MASK)
#define ESDHC_CMD18  (SDHC_XFERTYP_CMDINX(18) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_DTDSEL_MASK | SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_BCEN_MASK | SDHC_XFERTYP_MSBSEL_MASK | SDHC_XFERTYP_AC12EN_MASK)
#define ESDHC_CMD20  (SDHC_XFERTYP_CMDINX(20) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD22 (SDHC_XFERTYP_CMDINX(22) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_DTDSEL_MASK)
#define ESDHC_ACMD23 (SDHC_XFERTYP_CMDINX(23) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD24  (SDHC_XFERTYP_CMDINX(24) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48)|SDHC_XFERTYP_DPSEL_MASK)
#define ESDHC_CMD25  (SDHC_XFERTYP_CMDINX(25) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_MSBSEL_MASK | SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_BCEN_MASK | SDHC_XFERTYP_AC12EN_MASK)
#define ESDHC_CMD26  (SDHC_XFERTYP_CMDINX(26) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD27  (SDHC_XFERTYP_CMDINX(27) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD28  (SDHC_XFERTYP_CMDINX(28) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD29  (SDHC_XFERTYP_CMDINX(29) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD30  (SDHC_XFERTYP_CMDINX(30) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD32  (SDHC_XFERTYP_CMDINX(32) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD33  (SDHC_XFERTYP_CMDINX(33) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD34  (SDHC_XFERTYP_CMDINX(34) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD35  (SDHC_XFERTYP_CMDINX(35) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD36  (SDHC_XFERTYP_CMDINX(36) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD37  (SDHC_XFERTYP_CMDINX(37) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD38  (SDHC_XFERTYP_CMDINX(38) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD39  (SDHC_XFERTYP_CMDINX(39) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD40  (SDHC_XFERTYP_CMDINX(40) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD41 (SDHC_XFERTYP_CMDINX(41) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD42  (SDHC_XFERTYP_CMDINX(42) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_ACMD42 (SDHC_XFERTYP_CMDINX(42) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_ACMD51 (SDHC_XFERTYP_CMDINX(51) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD52  (SDHC_XFERTYP_CMDINX(52) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD53  (SDHC_XFERTYP_CMDINX(53) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD55  (SDHC_XFERTYP_CMDINX(55) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD56  (SDHC_XFERTYP_CMDINX(56) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD60  (SDHC_XFERTYP_CMDINX(60) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD61  (SDHC_XFERTYP_CMDINX(61) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))


#define ESDHC_PROCTL_DTW_1BIT                (0x00)
#define ESDHC_PROCTL_DTW_4BIT                (0x01)
#define ESDHC_PROCTL_DTW_8BIT                (0x10)


#define SDHC_HAL_ADMA2_DESC_VALID_MASK           (1 << 0)
#define SDHC_HAL_ADMA2_DESC_END_MASK             (1 << 1)
#define SDHC_HAL_ADMA2_DESC_INT_MASK             (1 << 2)
#define SDHC_HAL_ADMA2_DESC_ACT1_MASK            (1 << 4)
#define SDHC_HAL_ADMA2_DESC_ACT2_MASK            (1 << 5)
#define SDHC_HAL_ADMA2_DESC_TYPE_NOP             (SDHC_HAL_ADMA2_DESC_VALID_MASK)
#define SDHC_HAL_ADMA2_DESC_TYPE_RCV             (SDHC_HAL_ADMA2_DESC_ACT1_MASK | SDHC_HAL_ADMA2_DESC_VALID_MASK)
#define SDHC_HAL_ADMA2_DESC_TYPE_TRAN            (SDHC_HAL_ADMA2_DESC_ACT2_MASK | SDHC_HAL_ADMA2_DESC_VALID_MASK)
#define SDHC_HAL_ADMA2_DESC_TYPE_LINK            (SDHC_HAL_ADMA2_DESC_ACT1_MASK | SDHC_HAL_ADMA2_DESC_ACT2_MASK | SDHC_HAL_ADMA2_DESC_VALID_MASK)
#define SDHC_HAL_ADMA2_DESC_LEN_SHIFT            (16)
#define SDHC_HAL_ADMA2_DESC_LEN_MASK             (0xFFFFU)
#define SDHC_HAL_ADMA2_DESC_MAX_LEN_PER_ENTRY    (SDHC_HAL_ADMA2_DESC_LEN_MASK)

uint32_t SD_StatusWait (uint32_t mask);

//static void SetADMA2Table(uint32_t dir, uint32_t *buffer, uint32_t length)
//{
//    static uint32_t RAdmaTableAddr[sizeof(adma2_t)];
//    static uint32_t WAdmaTableAddr[sizeof(adma2_t)];
//    if(dir == 0)
//    {
//        ((adma2_t *)RAdmaTableAddr)->address = buffer;
//        ((adma2_t *)RAdmaTableAddr)->attribute = ((SDHC_HAL_ADMA2_DESC_LEN_MASK & length) << SDHC_HAL_ADMA2_DESC_LEN_SHIFT) | SDHC_HAL_ADMA2_DESC_TYPE_TRAN | SDHC_HAL_ADMA2_DESC_END_MASK; 
//        SDHC->ADSADDR = (uint32_t)RAdmaTableAddr;
//    }
//    else
//    {
//        ((adma2_t *)WAdmaTableAddr)->address = buffer;
//        ((adma2_t *)WAdmaTableAddr)->attribute = ((SDHC_HAL_ADMA2_DESC_LEN_MASK & length) << SDHC_HAL_ADMA2_DESC_LEN_SHIFT) | SDHC_HAL_ADMA2_DESC_TYPE_TRAN | SDHC_HAL_ADMA2_DESC_END_MASK; 
//        SDHC->ADSADDR = (uint32_t)WAdmaTableAddr; 
//    }
//}

/**
 * \brief SDHC wait function
 * \note internal function
 */
static void SDHC_WaitCommandLineIdle(void)
{
    volatile uint32_t timeout = 0;
    while (SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK))
    {
        __NOP();
        timeout++;
        if(timeout > 50*1000) break;
    }
}

/**
 * @brief Set SDHC baud rate
 * \param[in] clock 时钟频率
 * \param[in] baudrate 波特率设置
 * \return None
 */                                                            
static void SD_SetBaudRate(uint32_t clock, uint32_t baudrate)
{
	uint32_t pres, div, min, minpres = 0x80, mindiv = 0x0F;
	int  val;
    
    /* Find closest setting */
    min = (uint32_t)-1;
    for (pres = 2; pres <= 256; pres <<= 1)
    {
        for (div = 1; div <= 16; div++)
        {
            val = pres * div * baudrate - clock;
            if (val >= 0)
            {
                if (min > val)
                {
                    min = val;
                    minpres = pres;
                    mindiv = div;
                }
            }
        }
   }
    
	SDHC->SYSCTL &= (~ SDHC_SYSCTL_SDCLKEN_MASK);
    SDHC->SYSCTL &= ~(SDHC_SYSCTL_DTOCV_MASK | SDHC_SYSCTL_SDCLKFS_MASK | SDHC_SYSCTL_DVS_MASK);
	SDHC->SYSCTL = div | (SDHC_SYSCTL_DTOCV(0x0E) | SDHC_SYSCTL_SDCLKFS(minpres >> 1) | SDHC_SYSCTL_DVS(mindiv - 1));
	while (0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_SDSTB_MASK));
	SDHC->SYSCTL |= SDHC_SYSCTL_SDCLKEN_MASK;
} 

/**
 * @brief Set SDHC baud rate
 * \param[in] cmd 指令指针
 * \retval   0     ESDHC_OK
 * \retval other error code
 */                                                            
uint32_t SDHC_SendCmd(SDHC_Cmd_t *cmd)
{
    uint32_t xfertyp;
    uint32_t blkattr;
    xfertyp = cmd->cmd;
    
    SDHC_WaitCommandLineIdle();
    
    /* resume cmd must set DPSEL */
    if (ESDHC_XFERTYP_CMDTYP_RESUME == ((xfertyp & SDHC_XFERTYP_CMDTYP_MASK) >> SDHC_XFERTYP_CMDTYP_SHIFT))
    {
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
    }
    if(cmd->blkCount >= 1)
    {
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
    }
    
    /* set block size and block cnt */
    blkattr = SDHC_BLKATTR_BLKSIZE(cmd->blkSize) | SDHC_BLKATTR_BLKCNT(cmd->blkCount);
    
    if (cmd->blkCount > 1)
    {
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
        xfertyp |= SDHC_XFERTYP_BCEN_MASK;
        xfertyp |= SDHC_XFERTYP_MSBSEL_MASK;
    }
    
    /* clear status */
    SDHC->IRQSTAT |= SDHC_IRQSTAT_CIE_MASK |
                    SDHC_IRQSTAT_CEBE_MASK | 
                    SDHC_IRQSTAT_CCE_MASK | 
                    SDHC_IRQSTAT_CC_MASK | 
                    SDHC_IRQSTAT_CTOE_MASK | 
                    SDHC_IRQSTAT_CRM_MASK;
        
    /* issue cmd */
    SDHC->CMDARG = cmd->arg;
    SDHC->BLKATTR = blkattr;
    SDHC->XFERTYP = xfertyp;

    /* waitting for respond */
    if(SD_StatusWait (SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK) != SDHC_IRQSTAT_CC_MASK)
    {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK;
        return ESDHC_ERROR_cmd_FAILED;
    }
    
    /* get respond data if it has response */
    if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) != SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
    {
        cmd->resp[0] = SDHC->CMDRSP[0];
        cmd->resp[1] = SDHC->CMDRSP[1];
        cmd->resp[2] = SDHC->CMDRSP[2];
        cmd->resp[3] = SDHC->CMDRSP[3];
    }
    return ESDHC_OK;
}

/**
 * @brief SDHC initialize card
 * \note internal function
 * \param[in] cmd 指令指针
 * \retval   0     ESDHC_OK
 * \retval other error code
 */                                                            
static uint8_t SD_InitCard(void)
{
	volatile uint32_t delay_cnt = 0;
	uint8_t result;  
	uint32_t i = 0;
	uint8_t hc = 0;     
    SDHC_Cmd_t cmd;
	/* initalize 80 clock */
	SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
	while (SDHC->SYSCTL & SDHC_SYSCTL_INITA_MASK){};
    /* CMD0 -> CMD8 -> while(CMD55+ACMD41) ->CMD2 -> CMD3 ->CMD9 -> CMD7-> CMD16->(CMD55+ACMD6) */

	/* CMD0 : Idle status without response */
	cmd.cmd = ESDHC_CMD0;
	cmd.arg = 0;
	cmd.blkCount = 0;
    cmd.blkSize = 512;
	result = SDHC_SendCmd(&cmd);
	if(result != ESDHC_OK) 
    {
        LIB_TRACE("CMD0 error\r\n");
        return ESDHC_ERROR_INIT_FAILED;
    }
	/* CMD8 : check whether chip supports HC with 48bits response*/
	cmd.cmd = ESDHC_CMD8;
	cmd.arg = 0x000001AA; 	/* voltage supplied : 0001b ; chech pattern : 10101010b */
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if (result > 0) 
	{
		result = ESDHC_ERROR_INIT_FAILED;
	}
	if (result == 0) //SDHC
	{
        LIB_TRACE("SDHC detected, response is 0x%x 0x%x 0x%x 0x%x\r\n",cmd.resp[3],cmd.resp[2],cmd.resp[1],cmd.resp[0]);
		hc = true;  					
	}
    
	do 
	{								 
		for(delay_cnt = 0; delay_cnt < 1000; delay_cnt++);
		i++;
		
		cmd.cmd = ESDHC_CMD55;    /* the response Arg should be 0x120 */
		cmd.arg = 0;
        cmd.blkCount = 0;
        result = SDHC_SendCmd(&cmd);
		LIB_TRACE("CMD55 response is 0x%x 0x%x 0x%x 0x%x\r\n",cmd.resp[3],cmd.resp[2],cmd.resp[1],cmd.resp[0]);
		cmd.cmd = ESDHC_ACMD41;
		if(hc)
		{
			cmd.arg = 0x40300000; /* bit[23-8] represent Voltage range and response 0xc0ff80000 represent 2.7-3.6V*/ 
		}
		else
		{
			cmd.arg = 0x00300000;
		}
		result = SDHC_SendCmd(&cmd);
		LIB_TRACE("ACMD41 response is 0x%x 0x%x 0x%x 0x%x\r\n",cmd.resp[3],cmd.resp[2],cmd.resp[1],cmd.resp[0]);
	}while ((0 == (cmd.resp[0] & 0x80000000)) && (i < 300)); /* busy bit31 if 0,On Initialization;1 Initialization Complete */
    if(i == 300)
    {
        LIB_TRACE("Timeout\r\n");
        return ESDHC_ERROR_INIT_FAILED;
    }
    
	//CMD2 CID
	cmd.cmd = ESDHC_CMD2;
	cmd.arg = 0;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != ESDHC_OK) return ESDHC_ERROR_INIT_FAILED;    
	sdh.CID[0] = cmd.resp[0];
	sdh.CID[1] = cmd.resp[1];
	sdh.CID[2] = cmd.resp[2];
	sdh.CID[3] = cmd.resp[3];
	LIB_TRACE("CID[0]:0x%X\r\n", sdh.CID[0]);
    LIB_TRACE("CID[1]:0x%X\r\n", sdh.CID[1]);
    LIB_TRACE("CID[2]:0x%X\r\n", sdh.CID[2]);
    LIB_TRACE("CID[3]:0x%X\r\n", sdh.CID[3]);
	//CMD3 RCA
	cmd.cmd = ESDHC_CMD3;
	cmd.arg = 0;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != ESDHC_OK) return ESDHC_ERROR_INIT_FAILED;   
    sdh.RCA = cmd.resp[0]>>16;
	//CMD9 CSD
	cmd.cmd = ESDHC_CMD9;
	cmd.arg = sdh.RCA<<16;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != ESDHC_OK) return ESDHC_ERROR_INIT_FAILED;  
	sdh.CSD[0] = cmd.resp[0];
	sdh.CSD[1] = cmd.resp[1];
	sdh.CSD[2] = cmd.resp[2];
	sdh.CSD[3] = cmd.resp[3];
	
	//CMD7 
	cmd.cmd = ESDHC_CMD7;
	cmd.arg = sdh.RCA<<16;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != ESDHC_OK) return ESDHC_ERROR_INIT_FAILED;  
	//CMD16 
	cmd.cmd = ESDHC_CMD16;
	cmd.arg = 512;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != ESDHC_OK) return ESDHC_ERROR_INIT_FAILED;  
	
	//CMD55
	cmd.cmd = ESDHC_CMD55;
	cmd.arg = sdh.RCA<<16;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != ESDHC_OK) return ESDHC_ERROR_INIT_FAILED;  
	//ACMD6 
	cmd.cmd = ESDHC_ACMD6;
	cmd.arg = 2;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);//
	if(result != ESDHC_OK) return ESDHC_ERROR_INIT_FAILED;  

	SDHC->PROCTL &= (~SDHC_PROCTL_DTW_MASK);
	SDHC->PROCTL |=  SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_4BIT);

	if((sdh.CSD[3]>>22)&0x03)
	{
        sdh.card_type = SD_CARD_TYPE_SDHC;
	}
	else
	{
        sdh.card_type = SD_CARD_TYPE_SD;
	}
    
	return ESDHC_OK;	
}

/**
 * @brief SDHC quick initialize
 * \param[in] baudrate 波特率
 * \retval   0     ESDHC_OK
 * \retval other error code
 */                                                            
uint32_t SD_QuickInit(uint32_t baudrate)
{
    SD_InitTypeDef Init;
    Init.baudrate = baudrate;
    MPU->CESR &= (uint32_t) ~(0x1);
    /* init pinmux */
    PORT_PinMuxConfig(HW_GPIOE, 0, kPinAlt4); /* ESDHC.D1  */
    PORT_PinMuxConfig(HW_GPIOE, 1, kPinAlt4); /* ESDHC.D0  */
    PORT_PinMuxConfig(HW_GPIOE, 2, kPinAlt4); /* ESDHC.CLK */
    PORT_PinMuxConfig(HW_GPIOE, 3, kPinAlt4); /* ESDHC.CMD */
    PORT_PinMuxConfig(HW_GPIOE, 4, kPinAlt4); /* ESDHC.D3  */
    PORT_PinMuxConfig(HW_GPIOE, 5, kPinAlt4); /* ESDHC.D2  */
    
    PORT_PinPullConfig(HW_GPIOE, 0, kPullUp);
    PORT_PinPullConfig(HW_GPIOE, 1, kPullUp);
    PORT_PinPullConfig(HW_GPIOE, 2, kPullUp);
    PORT_PinPullConfig(HW_GPIOE, 3, kPullUp);
    PORT_PinPullConfig(HW_GPIOE, 4, kPullUp);
    PORT_PinPullConfig(HW_GPIOE, 5, kPullUp);
    
    SD_Init(&Init);
    return SD_InitCard();
}

/**
 * @brief SDHC complete initialize
 * \param[in] Init SD初始化结构体指针
 * \retval   0     ESDHC_OK
 * \retval other error code
 */                                                            
uint8_t SD_Init(SD_InitTypeDef* Init)
{
    uint32_t clock;
    
    IP_CLK_ENABLE(0);
    
    /* reset module */
	SDHC->SYSCTL = SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_SDCLKFS(0x80);
	while(SDHC->SYSCTL & SDHC_SYSCTL_RSTA_MASK);

    /* use ADMA2 mode */
	//SDHC->PROCTL = SDHC_PROCTL_EMODE(2)|SDHC_PROCTL_DMAS(2); 
    SDHC->PROCTL = SDHC_PROCTL_EMODE(2);
        
    /* set watermark */
	SDHC->WML = SDHC_WML_RDWML(0x80) | SDHC_WML_WRWML(0x80);

    /* select core clock */
    #ifdef SIM_SOPT2_ESDHCSRC_MASK
    SIM->SOPT2 &= ~SIM_SOPT2_ESDHCSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_ESDHCSRC(0);
    #else
    SIM->SOPT2 &= ~SIM_SOPT2_SDHCSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_SDHCSRC(0);
    #endif
    clock = GetClock(kCoreClock);
	SD_SetBaudRate(clock, Init->baudrate);
	SDHC->IRQSTAT = 0xFFFFFFFF;
	SDHC->IRQSTATEN = 0xFFFFFFFF;

	return ESDHC_OK;
}

/**
 * @brief SDHC 块读操作
 * \param[in] sector 块
 * \param[out] buf 数据的存放地址
 * \param[in] len 个数
 * \retval   0     ESDHC_OK
 * \retval other error code
 */                                                            
uint8_t SDHC_ReadBlock(uint32_t sector, uint8_t *buf, uint32_t len)
{
    uint32_t ret, i,j;
    uint32_t *p;
	SDHC_Cmd_t cmd; 
	if(sdh.card_type == SD_CARD_TYPE_SD)
        sector = sector<<9;
    
    if(!len)
        return 0;
    
    if(len == 1)
        cmd.cmd = ESDHC_CMD17;
    else
        cmd.cmd = ESDHC_CMD18; 
    
    cmd.arg = sector; 
    cmd.blkSize = 512;
    cmd.blkCount = len;
    
    SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK;
    
   // SetADMA2Table(0, (uint32_t*)buffer, 512*len);
    ret = SDHC_SendCmd(&cmd);
    if(ret != ESDHC_OK)
        return ESDHC_ERROR_DATA_TRANSFER; 
    
    p = (uint32_t*)buf;
    for(i = 0; i < len; i++)
    {
        if (((uint32_t)buf & 0x03) == 0)
        {
            while((SDHC->PRSSTAT & SDHC_PRSSTAT_BREN_MASK) == 0);
            for (j = (512+3)>>2;j!= 0;j--)
            {
                *p++ = SDHC->DATPORT;
            }
        }
    }
    
  //  ret = SD_StatusWait(SDHC_IRQSTAT_TC_MASK);
  //  SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK;
    
    if(ret != ESDHC_OK)
        return ESDHC_ERROR_DATA_TRANSFER;

    return ESDHC_OK;
}

/**
 * @brief SDHC 块写操作
 * \param[in] sector 块
 * \param[in] buf 待写入数据的地址
 * \param[in] len 个数
 * \retval   0     ESDHC_OK
 * \retval other error code
 */                                                            
uint8_t SDHC_WriteBlock(uint32_t sector, uint8_t *buf, uint32_t len)
{
    uint16_t ret, i, j;
    uint32_t *p;
	SDHC_Cmd_t cmd; 
	if(sdh.card_type == SD_CARD_TYPE_SD)
        sector = sector<<9;
    
    if(!len)
        return 0;
    
    if(len == 1)
        cmd.cmd = ESDHC_CMD24;
    else
        cmd.cmd = ESDHC_CMD25; 
    
    cmd.arg = sector; 
    cmd.blkSize = 512;
    cmd.blkCount = len;
    
    SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK;
    
   // SetADMA2Table(1, (uint32_t*)buf, 512*len);
    ret = SDHC_SendCmd(&cmd);
    if(ret != ESDHC_OK)
    {
        return ESDHC_ERROR_DATA_TRANSFER; 
    }
    
    p = (uint32_t*)buf;
    
    for(i = 0; i < len; i++)
    {
        if (((uint32_t)buf & 0x03) == 0)
        {
            while((SDHC->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK) == 0);

            for (j = (512+3)>>2;j!= 0;j--)
            {
                SDHC->DATPORT = *p++;
            }
        }
    }
    
   // ret = SD_StatusWait(SDHC_IRQSTAT_TC_MASK | SDHC_IRQSTAT_DINT_MASK);
   // SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK | SDHC_IRQSTAT_DINT_MASK;
    
    if(ret != ESDHC_OK)
        return ESDHC_ERROR_DATA_TRANSFER;

    /* waitting for card is OK */
    do
    {
        cmd.cmd = ESDHC_CMD13;
        cmd.arg = sdh.RCA<<16;
        cmd.blkCount = 0;
        
        cmd.resp[0] = 0;
        
        ret = SDHC_SendCmd(&cmd);
        if(ret != ESDHC_OK)
        {
            continue;
        }
        if (cmd.resp[0] & 0xFFD98008)
        {
            break;
        }

    } while (0x000000900 != (cmd.resp[0] & 0x00001F00));
   return ESDHC_OK;
}

/**
 * \brief read SD single block data
 * \param[in] sector 块
 * \param[out] buf 数据的存放地址
 * \retval   0     ESDHC_OK
 * \retval other error code
 */ 
uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buf)
{
    return SDHC_ReadBlock(sector, buf, 1);
}

/**
 * \brief write SD single block data
 * \param[in] sector 块
 * \param[in] buf 待写入数据的地址
 * \retval   0     ESDHC_OK
 * \retval other error code
 */ 
uint8_t SD_WriteSingleBlock(uint32_t sector, uint8_t *buf)
{
    return SDHC_WriteBlock(sector, buf, 1);
}

/**
 * @brief GetSD size
 * @retval size in MB
 */ 
uint32_t SD_GetSizeInMB(void)
{
	uint32_t BlockBumber;
	uint32_t Muti;
	uint32_t BlockLen;
	uint32_t Capacity;
	if((sdh.CSD[3]>>22)&0x03) /* SDHC  */
	{
		BlockLen = (sdh.CSD[2]>>24)&0xFF;
		Capacity=((sdh.CSD[1]>>8)&0xFFFFFF)+1;
		Capacity=(Capacity+1)/2;
		return Capacity;
	}
	else /* non SDHC */
	{
        Muti = (sdh.CSD[1]>>7)&0x7;
        Muti = 2<<(Muti+1);
        BlockBumber = ((sdh.CSD[2]>>0)&0x03);
        BlockBumber = (BlockBumber<<10) + ((sdh.CSD[1]>>22)&0x0FFF);
        BlockBumber++;
        BlockBumber=BlockBumber * Muti;
        BlockLen = (sdh.CSD[2]>>8)&0x0F;
        BlockLen = 2<<(BlockLen-1);
        Capacity=BlockBumber * BlockLen;
        Capacity=Capacity/1024/1024;
        return Capacity;
	}
}

/**
 * \brief 等待状态位
 * \param[in] mask 相关标志位
 * \return 相对应的状态
 */
uint32_t SD_StatusWait (uint32_t  mask)
{
    volatile uint32_t timeout;
    uint32_t ret;
    timeout = 0;
    do
    {
        timeout++;
        __NOP();
        ret = SDHC->IRQSTAT & mask;
        if(timeout > 50*1000) break;
    } 
    while (0 == ret);
    return ret;
}


/**
 * @brief SD_ReadMultiBlock legcy support
 * \note this function is same as SDHC_ReadBlock(...)
 */ 		
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buf, uint32_t len)
{
    return SDHC_ReadBlock(sector, buf, len);
}

/**
 * @brief SD_WriteMultiBlock legcy support
 * \note this function is same as SDHC_WriteBlock(...)
 */ 	
uint8_t SD_WriteMultiBlock(uint32_t sector, uint8_t *buf, uint32_t len)
{
    return SDHC_WriteBlock(sector, buf, len);
}

#endif

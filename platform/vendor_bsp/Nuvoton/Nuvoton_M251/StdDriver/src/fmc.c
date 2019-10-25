/**************************************************************************//**
 * @file     fmc.c
 * @version  V3.00
 * @brief    M251 Series Flash Memory Controller(FMC) driver source file
 *
 * @note
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup FMC_Driver FMC Driver
  @{
*/

/** @addtogroup FMC_EXPORTED_FUNCTIONS FMC Exported Functions
  @{
*/

/**
  * @brief      Set boot source from LDROM or APROM after next software reset
  *
  * @param[in]  i32BootSrc
  *                1: Boot from LDROM
  *                0: Boot from APROM
  *
  * @return    None
  *
  * @details   This function is used to switch APROM boot or LDROM boot. User need to call
  *            FMC_SetBootSource to select boot source first, then use CPU reset or
  *            System Reset Request to reset system.
  *
  */
void FMC_SetBootSource(int32_t i32BootSrc)
{
    if (i32BootSrc)
    {
        FMC->ISPCTL |= FMC_ISPCTL_BS_Msk; /* Boot from LDROM */
    }
    else
    {
        FMC->ISPCTL &= ~FMC_ISPCTL_BS_Msk;/* Boot from APROM */
    }
}


/**
  * @brief    Disable ISP Functions
  *
  * @param    None
  *
  * @return   None
  *
  * @details  This function will clear ISPEN bit of ISPCON to disable ISP function
  *
  */
void FMC_Close(void)
{
    FMC->ISPCTL &= ~FMC_ISPCTL_ISPEN_Msk;
}


/**
  * @brief    Get the current boot source
  *
  * @param    None
  *
  * @retval   0 This chip is currently booting from APROM
  * @retval   1 This chip is currently booting from LDROM
  *
  * @note     This function only show the boot source.
  *           User need to read ISPSTA register to know if IAP mode supported or not in relative boot.
  */
int32_t FMC_GetBootSource(void)
{
    int32_t  ret = 0;

    if (FMC->ISPCTL & FMC_ISPCTL_BS_Msk)
    {
        ret = 1;
    }

    return ret;
}


/**
  * @brief    Enable FMC ISP function
  *
  * @param    None
  *
  * @return   None
  *
  * @details  ISPEN bit of ISPCON must be set before we can use ISP commands.
  *           Therefore, To use all FMC function APIs, user needs to call FMC_Open() first to enable ISP functions.
  *
  * @note     ISP functions are write-protected. user also needs to unlock it by calling SYS_UnlockReg() before using all ISP functions.
  *
  */
void FMC_Open(void)
{
    FMC->ISPCTL |=  FMC_ISPCTL_ISPEN_Msk;
}


/**
  * @brief       Read the User Configuration words.
  *
  * @param[out]  u32Config  The word buffer to store the User Configuration data.
  * @param[in]   u32Count   The word count to be read.
  *
  * @retval       0 Success
  * @retval      -1 Failed
  *
  * @details     This function is used to read the settings of user configuration.
  *              if u32Count = 1, Only CONFIG0 will be returned to the buffer specified by u32Config.
  *              if u32Count = 2, Both CONFIG0 and CONFIG1 will be returned.
  */
int32_t FMC_ReadConfig(uint32_t u32Config[], uint32_t u32Count)
{
    uint32_t i;

    for (i = 0u; i < u32Count; i++)
    {
        u32Config[i] = FMC_Read(FMC_CONFIG_BASE + i * 4u);
    }

    return 0;
}


/**
  * @brief    Write User Configuration
  *
  * @param[in]  u32Config The word buffer to store the User Configuration data.
  * @param[in]  u32Count The word count to program to User Configuration.
  *
  * @retval     0 Success
  * @retval    -1 Failed
  *
  * @details  User must enable User Configuration update before writing it.
  *           User must erase User Configuration before writing it.
  *           User Configuration is also be page erase. User needs to backup necessary data
  *           before erase User Configuration.
  */
int32_t FMC_WriteConfig(uint32_t u32Config[], uint32_t u32Count)
{
    int32_t  ret = 0;
    uint32_t i;

    for (i = 0u; i < u32Count; i++)
    {
        FMC_Write(FMC_CONFIG_BASE + i * 4u, u32Config[i]);

        if (FMC_Read(FMC_CONFIG_BASE + i * 4u) != u32Config[i])
        {
            ret = 1;
        }
    }

    return ret;
}


/**
  * @brief Run CRC32 checksum calculation and get result.
  *
  * @param[in] u32addr   Starting flash address. It must be a page aligned address.
  * @param[in] u32count  Byte count of flash to be calculated. It must be multiple of 512 bytes.
  *
  * @return Success or not.
  * @retval   0           Success.
  * @retval   0xFFFFFFFF  Invalid parameter.
  *
  * details   Run ISP checksum command to calculate specify area
  */
uint32_t  FMC_GetChkSum(uint32_t u32addr, uint32_t u32count)
{
    uint32_t   ret;

    if ((u32addr % 512UL) || (u32count % 512UL))
    {
        ret = 0xFFFFFFFF;
    }
    else
    {
        FMC->ISPCMD  = FMC_ISPCMD_CAL_CHECKSUM;
        FMC->ISPADDR = u32addr;
        FMC->ISPDAT  = u32count;
        FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;

        while (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk) { }

        FMC->ISPCMD = FMC_ISPCMD_CHECKSUM;
        FMC->ISPADDR    = u32addr;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

        while (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk) { }

        ret = FMC->ISPDAT;
    }

    return ret;
}


/**
  * @brief Run flash all one verification and get result.
  *
  * @param[in] u32addr   Starting flash address. It must be a page aligned address.
  * @param[in] u32count  Byte count of flash to be calculated. It must be multiple of 512 bytes.
  *
  * @retval   READ_ALLONE_YES      The contents of verified flash area are 0xFFFFFFFF.
  * @retval   READ_ALLONE_NOT  Some contents of verified flash area are not 0xFFFFFFFF.
  * @retval   READ_ALLONE_CMD_FAIL  Unexpected error occurred.
  *
  * @details  Run ISP check all one command to check specify area is all one or not.
  */
uint32_t  FMC_CheckAllOne(uint32_t u32addr, uint32_t u32count)
{
    uint32_t  ret = READ_ALLONE_CMD_FAIL;

    FMC->ISPSTS = 0x80UL;   /* clear check all one bit */

    FMC->ISPCMD   = FMC_ISPCMD_RUN_ALL1;
    FMC->ISPADDR  = u32addr;
    FMC->ISPDAT   = u32count;
    FMC->ISPTRG   = FMC_ISPTRG_ISPGO_Msk;

    while (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk) { }

    do
    {
        FMC->ISPCMD = FMC_ISPCMD_READ_ALL1;
        FMC->ISPADDR    = u32addr;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

        while (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk) { }
    }
    while (FMC->ISPDAT == 0UL);

    if (FMC->ISPDAT == READ_ALLONE_YES)
    {
        ret = FMC->ISPDAT;
    }

    if (FMC->ISPDAT == READ_ALLONE_NOT)
    {
        ret = FMC->ISPDAT;
    }

    return ret;
}


/**
  * @brief  Check the XOM is actived or not.
  *
  * @param[in] xom_num    The XOM number. The value will always be 0 in M251 series.
  *
  * @retval   1   XOM is actived.
  * @retval   0   XOM is not actived.
  * @retval   -2  Invalid XOM number.
  *
  * @details To get specify XOM active status
  */
int32_t FMC_Is_XOM_Actived(uint32_t xom_num)
{
    uint32_t u32act;
    int32_t  ret = 0;

    if (xom_num >= 1UL)
    {
        ret = -2;
    }

    if (ret >= 0)
    {
        u32act = (((FMC->XOMSTS) & 0xful) & (1ul << xom_num)) >> xom_num;
        ret = (int32_t)u32act;
    }

    return ret;
}


/**
  * @brief     Config XOM Region
  * @param[in] xom_num    The XOM number. The value will always be 0 in M251 series.
  * @param[in] xom_base   The XOM region base address.
  * @param[in] xom_page   The XOM page number of region size.
  *
  * @retval   0   Success
  * @retval   1   XOM is has already actived.
  * @retval   -1  Program failed.
  * @retval   -2  Invalid XOM number.
  *
  * @details  Program XOM base address and XOM size(page)
  */
int32_t FMC_Config_XOM(uint32_t xom_num, uint32_t xom_base, uint8_t xom_page)
{
    int32_t  ret = 0;

    if (xom_num >= 1UL)
    {
        ret = -2;
    }

    if (ret == 0)
    {
        ret = FMC_Is_XOM_Actived(xom_num);
    }

    if (ret == 0)
    {
        FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
        FMC->ISPADDR = FMC_XOM_BASE + (xom_num * 0x10u);
        FMC->ISPDAT = xom_base;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

        while (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk) {}

        if (FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
        {
            FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
            ret = -1;
        }
    }

    if (ret == 0)
    {
        FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
        FMC->ISPADDR = FMC_XOM_BASE + (xom_num * 0x10u + 0x04u);
        FMC->ISPDAT = xom_page;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

        while (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk) {}

        if (FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
        {
            FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
            ret = -1;
        }
    }

    if (ret == 0)
    {
        FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
        FMC->ISPADDR = FMC_XOM_BASE + (xom_num * 0x10u + 0x08u);
        FMC->ISPDAT = 0u;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

        while (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk) {}

        if (FMC->ISPSTS & FMC_ISPSTS_ISPFF_Msk)
        {
            FMC->ISPSTS |= FMC_ISPSTS_ISPFF_Msk;
            ret = -1;
        }
    }

    return ret;
}


/**
  * @brief  Execute Erase XOM Region
  *
  * @param[in]  xom_num  The XOM number. The value will always be 0 in M251 series.
  *
  * @return   XOM erase success or not.
  * @retval    0  Success
  * @retval   -1  Erase failed
  * @retval   -2  Invalid XOM number.
  *
  * @details Execute FMC_ISPCMD_PAGE_ERASE command to erase XOM.
  */
int32_t FMC_Erase_XOM(uint32_t xom_num)
{
    uint32_t u32Addr;
    int32_t i32Active, err = 0;

    if (xom_num >= 1UL)
    {
        err = -2;
    }

    i32Active = FMC_Is_XOM_Actived(xom_num);


    if (i32Active)
    {
        u32Addr =  FMC->XOMR0STS0;
        FMC->ISPCMD = FMC_ISPCMD_PAGE_ERASE;
        FMC->ISPADDR = u32Addr;
        FMC->ISPDAT = 0x55aa03u;
        FMC->ISPTRG = 0x1u;
#if ISBEN
        __ISB();
#endif

        while (FMC->ISPTRG) {}

        /* Check ISPFF flag to know whether erase OK or fail. */
        if (FMC->ISPCTL & FMC_ISPCTL_ISPFF_Msk)
        {
            FMC->ISPCTL |= FMC_ISPCTL_ISPFF_Msk;
            err = -1;
        }
    }
    else
    {
        err = -1;
    }

    return err;
}


/**
 * @brief      Flash page erase
 *
 * @param[in]  u32Addr  Flash address including APROM, LDROM, Data Flash, and CONFIG
 *
 * @details    To do flash page erase. The target address could be APROM, LDROM, Data Flash, or CONFIG.
 *             The page size is 512 bytes.
 *
 * @retval      0 Success
 * @retval     -1 Erase failed
 *
 */
int32_t FMC_Erase(uint32_t u32Addr)
{
    int32_t  ret = 0;

    FMC->ISPCMD = FMC_ISPCMD_PAGE_ERASE;
    FMC->ISPADDR = u32Addr;
    FMC->ISPTRG = 0x1u;
#if ISBEN
    __ISB();
#endif

    while (FMC->ISPTRG) {}

    /* Check ISPFF flag to know whether erase OK or fail. */
    if (FMC->ISPCTL & FMC_ISPCTL_ISPFF_Msk)
    {
        FMC->ISPCTL |= FMC_ISPCTL_ISPFF_Msk;
        ret = -1;
    }

    return ret;
}


/**
 * @brief       Read 32-bit Data from specified address of flash
 *
 * @param[in]   u32Addr  Flash address include APROM, LDROM, Data Flash, and CONFIG
 *
 * @return      The data of specified address
 *
 * @details     To read word data from Flash include APROM, LDROM, Data Flash, and CONFIG.
 *
 */
uint32_t FMC_Read(uint32_t u32Addr)
{
    FMC->ISPCMD = FMC_ISPCMD_READ;
    FMC->ISPADDR = u32Addr;
    FMC->ISPDAT = 0u;
    FMC->ISPTRG = 0x1u;
#if ISBEN
    __ISB();
#endif

    while (FMC->ISPTRG) {}

    return FMC->ISPDAT;
}



/**
 * @brief      Program 32-bit data into specified address of flash
 *
 * @param[in]  u32Addr  Flash address include APROM, LDROM, Data Flash, and CONFIG
 * @param[in]  u32Data  32-bit Data to program
 *
 * @return     None
 *
 * @details    To program word data into Flash include APROM, LDROM, Data Flash, and CONFIG.
 *             The corresponding functions in CONFIG are listed in FMC section of Technical Reference Manual.
 *
 */
void FMC_Write(uint32_t u32Addr, uint32_t u32Data)
{
    FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
    FMC->ISPADDR = u32Addr;
    FMC->ISPDAT = u32Data;
    FMC->ISPTRG = 0x1u;
#if ISBEN
    __ISB();
#endif

    while (FMC->ISPTRG) {}
}

/*@}*/ /* end of group FMC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group FMC_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/



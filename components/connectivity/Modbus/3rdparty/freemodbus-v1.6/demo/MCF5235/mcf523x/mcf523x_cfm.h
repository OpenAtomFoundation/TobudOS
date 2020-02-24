/************************************************************************************
 * Copyright @ 1995-2005 metrowerks inc. All rights reserved.                       *
 *                                                                                  *
 *                                                                                  *
 *    $RCSfile: mcf523x_cfm.h,v $                                                            *
 *    $Revision$                                                              *
 *    $Date$                                                  *
 *                                                                                  *
 * DESCRIPTION                                                                      *
 *   Register and bit definitions for the MCF523X.                                  *
 *                                                                                  *
 *                                                                                  *
 *                                                                                  *
 * NOTE                                                                             *
 *                                                                                  *
 *                                                                                  *
 *                                                                                  *
 * HISTORY                                                                          *
 *                                                                                  *	
 ************************************************************************************/

#ifndef __MCF523X_CFM_H__
#define __MCF523X_CFM_H__

/*********************************************************************
*
* Chip Configuration Module (CFM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CFM_CCR     (*(vuint16*)(void*)(&__IPSBAR[0x110004]))
#define MCF_CFM_LPCR    (*(vuint8 *)(void*)(&__IPSBAR[0x110007]))
#define MCF_CFM_CIR     (*(vuint16*)(void*)(&__IPSBAR[0x11000A]))
#define MCF_CFM_RCON    (*(vuint16*)(void*)(&__IPSBAR[0x110008]))

/* Bit definitions and macros for MCF_CFM_CCR */
#define MCF_CFM_CCR_BMT(x)        (((x)&0x0007)<<0)
#define MCF_CFM_CCR_BME           (0x0008)
#define MCF_CFM_CCR_SZEN          (0x0040)
#define MCF_CFM_CCR_MODE(x)       (((x)&0x0007)<<8)
#define MCF_CFM_CCR_MODE(x)       ((uint16)(((x)&0x0007)<<8))
#define MCF_CFM_CCR_BMT(x)        ((uint16)(((x)&0x0007)))

/* Bit definitions and macros for MCF_CFM_LPCR */
#define MCF_CFM_LPCR_STPMD(x)     (((x)&0x03)<<3)
#define MCF_CFM_LPCR_LPMD(x)      (((x)&0x03)<<6)
#define MCF_CFM_LPCR_LPMD_STOP    (0xC0)
#define MCF_CFM_LPCR_LPMD_WAIT    (0x80)
#define MCF_CFM_LPCR_LPMD_DOZE    (0x40)
#define MCF_CFM_LPCR_LPMD_RUN     (0x00)
#define MCF_CFM_LPCR_STPMD(x)     ((uint8)(((x)&0x0003)<<3))

/* Bit definitions and macros for MCF_CFM_CIR */
#define MCF_CFM_CIR_PRN(x)        (((x)&0x003F)<<0)
#define MCF_CFM_CIR_PIN(x)        (((x)&0x03FF)<<6)

/* Bit definitions and macros for MCF_CFM_RCON */
#define MCF_CFM_RCON_MODE         (0x0001)
#define MCF_CFM_RCON_BOOTPS(x)    (((x)&0x0003)<<3)
#define MCF_CFM_RCON_RLOAD        (0x0020)
#define MCF_CFM_RCON_RCSC(x)      (((x)&0x0003)<<8)

/********************************************************************/

#endif /* __MCF523X_CFM_H__ */

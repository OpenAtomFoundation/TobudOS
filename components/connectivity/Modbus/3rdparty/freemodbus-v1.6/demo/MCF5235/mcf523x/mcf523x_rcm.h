/************************************************************************************
 * Copyright @ 1995-2005 metrowerks inc. All rights reserved.                       *
 *                                                                                  *
 *                                                                                  *
 *    $RCSfile: mcf523x_rcm.h,v $                                                            *
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

#ifndef __MCF523X_RCM_H__
#define __MCF523X_RCM_H__

/*********************************************************************
*
* Reset Configuration Module (RCM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_RCM_RCR    (*(vuint8 *)(void*)(&__IPSBAR[0x110000]))
#define MCF_RCM_RSR    (*(vuint8 *)(void*)(&__IPSBAR[0x110001]))

/* Bit definitions and macros for MCF_RCM_RCR */
#define MCF_RCM_RCR_FRCRSTOUT    (0x40)
#define MCF_RCM_RCR_SOFTRST      (0x80)

/* Bit definitions and macros for MCF_RCM_RSR */
#define MCF_RCM_RSR_LOL          (0x01)
#define MCF_RCM_RSR_LOC          (0x02)
#define MCF_RCM_RSR_EXT          (0x04)
#define MCF_RCM_RSR_POR          (0x08)
#define MCF_RCM_RSR_WDR          (0x10)
#define MCF_RCM_RSR_SOFT         (0x20)

/********************************************************************/

#endif /* __MCF523X_RCM_H__ */

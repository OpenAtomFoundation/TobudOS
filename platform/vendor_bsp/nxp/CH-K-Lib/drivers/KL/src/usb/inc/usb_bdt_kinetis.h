/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 ******************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************//*!
 *
 * @file usb_bdt_kinetis.h
 *
 * @author 
 *
 * @version 
 *
 * @date Jun-05-2009
 *
 * @brief The file contains definitions of Buffer Descriptor Table.
 *
 *****************************************************************************/

#ifndef _USBBDT_H
#define _USBBDT_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "types.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
/* Buffer Descriptor Status Register Initialization Parameters */
#define _BDTSTALL      (0x04)           /* Buffer Stall enable */
#define _DATA0         (0x00)           /* DATA0 packet expected next */
#define _DATA1         (0x40)           /* DATA1 packet expected next */
#define _DTS           (0x08)           /* DTS Mask */
#define _SIE           (0x80)           /* SIE owns buffer */
#define _CPU           (0x00)           /* CPU owns buffer */
#define _KEEP          (0x20)           /* keep bit */  
                       
#define MAX_BDT_INDEX  (64)             /* Maximum BDT Indexes */


/******************************************************************************
 * Types
 *****************************************************************************/
 /* This structure is an exact replica of the BDT MAP in the USB RAM 
    The BDT_STAT defines the stat byte of the buffer descriptor vector.
    McuCtlBit structure defines the bits that have a meaning from CPU 
    point of view.SieCtlBit structure defines the bits that have a 
    meaning from USB controller point of view.      
 */
 
#if defined(__CWCC__)
#pragma align_array_members on
#endif
#pragma pack(push)
#pragma pack(1)
typedef struct _MCU_CTL_BIT{
        uint_8 :1;
        uint_8 :1;
        uint_8 bdtstall:1;            /* Buffer Stall Enable */
        uint_8 dts:1;                 /* Data Toggle Synch Enable */
        uint_8 keep:1;                /* Address Increment Disable */
        uint_8 ninc:1;                /* BD Keep Enable */
        uint_8 data:1;                /* Data Toggle Synch Value */
        uint_8 own:1;                 /* USB Ownership */
}MCU_CTL_BIT; /* read Stat */

typedef struct _SIE_CTL_BIT{
        uint_8 :1;
        uint_8 :1;
        uint_8 pid0:1;                /* Packet Identifier bit 0 */
        uint_8 pid1:1;                /* Packet Identifier bit 1 */
        uint_8 pid2:1;                /* Packet Identifier bit 2 */
        uint_8 pid3:1;                /* Packet Identifier bit 3 */
        uint_8 :1;
        uint_8 own:1;        
}SIE_CTL_BIT;  /* write Stat */
      
typedef struct _REC_PID{
        uint_8    :2;
        uint_8 pid:4;                 /* Packet Identifier */
        uint_8    :2;
}REC_PID;
      
typedef union _BD_STAT
{
    uint_8 _byte;                    
    MCU_CTL_BIT McuCtlBit;
    SIE_CTL_BIT SieCtlBit;
    REC_PID RecPid;
} BD_STAT;                            /* Buffer Descriptor Status Register */

typedef struct _BUFF_DSC
{
        BD_STAT Stat;
        uint_8  reserved1;
        uint_16 cnt;                  /* Count of bytes receieved or sent */      
                                      /* six MSB bits are reserved ones */      
        uint_32 addr;                 /* Buffer Address */
} BUFF_DSC, *P_BUFF_DSC;              /* Buffer Descriptor Table */

typedef struct _g_bdtmap {

  BUFF_DSC ep_dsc[MAX_BDT_INDEX];     /* Endpoint Descriptor */  
}BDTMAP; 
#if defined(__CWCC__)
#pragma align_array_members off
#pragma options align=reset
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma pack()
#else /* e.g. gcc */
#pragma pack(pop)
#endif
/******************************************************************************
 * Global Functions - None
 *****************************************************************************/

#endif 

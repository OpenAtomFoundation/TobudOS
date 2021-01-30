/*********************************************************************
*          Portions COPYRIGHT 2016 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.32 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_FlexColor_Private.h
Purpose     : Private declarations for GUIDRV_FlexColor driver
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
#include "GUIDRV_FlexColor.h"

#ifndef GUIDRV_FLEXCOLOR_PRIVATE_H
#define GUIDRV_FLEXCOLOR_PRIVATE_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define LCD_WRITE_BUFFER_SIZE           500

#define FLEXCOLOR_CF_MANAGE_ORIENTATION (1 << 0)
#define FLEXCOLOR_CF_RAM_ADDR_SET       (1 << 1)
#define FLEXCOLOR_CF_SET_CURSOR         (1 << 2)

#define FLEXCOLOR_CF_DEFAULT            FLEXCOLOR_CF_RAM_ADDR_SET

#define FLEXCOLOR_MAX_NUM_DUMMY_READS   5
#define FLEXCOLOR_NUM_DUMMY_READS       1

#define PUSH_RECT 0
#define POP_RECT  1

//
// Use unique context identified
//
#define DRIVER_CONTEXT DRIVER_CONTEXT_FLEXCOLOR

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct DRIVER_CONTEXT DRIVER_CONTEXT;

/*********************************************************************
*
*       DRIVER_CONTEXT
*/
struct DRIVER_CONTEXT {
  //
  // Data
  //
  int xSize, ySize;   // Display size
  int vxSize, vySize; // Virtual display size
  int x0, y0, x1, y1; // Current rectangle
  int NumDummyReads;  // Number of required dummy reads
  U16 RegEntryMode;   // Can be used for storing additional configuration bits for 'EntryMode' register which is modified by the driver
  U16 Flags;
  LCD_PIXELINDEX IndexMask;
  int FirstSEG, FirstCOM;
  int Orientation;
  int BitsPerPixel;
  int Shift;
  GUI_DEVICE * pDevice;
  const GUI_DEVICE_API * pMemdev_API;
  //
  // Cache
  //
  void * pVRAM;
  int CacheLocked;
  int CacheStat;
  int xPos, yPos;
  U32 Addr;
  GUI_RECT CacheRect;
  //
  // Buffers
  //
  void * pWriteBuffer;
  void * pLineBuffer;
  U8  aPair_8 [3 + FLEXCOLOR_MAX_NUM_DUMMY_READS];
  U16 aPair_16[3 + FLEXCOLOR_MAX_NUM_DUMMY_READS];
  //
  // Functions for writing single items (data, cmd) regardless of the interface and getting the status
  //
  U16  (* pfReadReg)     (DRIVER_CONTEXT * _pContext);
  void (* pfSetReg)      (DRIVER_CONTEXT * _pContext, U16 _Data);
  void (* pfWritePara)   (DRIVER_CONTEXT * _pContext, U16 _Data);
  void (* pfSetInterface)(DRIVER_CONTEXT * _pContext, int _BusWidth);
  //
  // Setting read mode, required for RA8870
  //
  void (* pfSetReadMode)(DRIVER_CONTEXT * _pContext, int OnOff, int SetCursor);
  //
  // Cache related function pointers
  //
  void (* pfSendCacheRect)(DRIVER_CONTEXT * _pContext, int _x0, int _y0, int _x1, int _y1);
  U32  (* pfReadData)     (DRIVER_CONTEXT * _pContext);
  void (* pfWriteData)    (DRIVER_CONTEXT * _pContext, U32 _PixelIndex);
  //
  // Controller specific routines
  //
  void (* pfSetRect)            (DRIVER_CONTEXT * _pContext, int _x0, int _y0, int _x1, int _y1);
  void (* pfSetPhysRAMAddr)     (DRIVER_CONTEXT * _pContext, int _x, int _y);
  void (* pfSetOrientation)     (DRIVER_CONTEXT * _pContext);
  U16  (* pfReadPixel_16bpp_B16)(DRIVER_CONTEXT * _pContext);
  U16  (* pfReadPixel_16bpp_B8) (DRIVER_CONTEXT * _pContext);
  U32  (* pfReadPixel_18bpp_B9) (DRIVER_CONTEXT * _pContext);
  U32  (* pfReadPixel_18bpp_B18)(DRIVER_CONTEXT * _pContext);
  void (* pfReadRect_16bpp_B16) (GUI_DEVICE * _pDevice, int _x0, int _y0, int _x1, int _y1, U16 * _pBuffer);
  void (* pfReadRect_16bpp_B8)  (GUI_DEVICE * _pDevice, int _x0, int _y0, int _x1, int _y1, U16 * _pBuffer);
  void (* pfReadRect_18bpp_B9)  (GUI_DEVICE * _pDevice, int _x0, int _y0, int _x1, int _y1, U32 * _pBuffer);
  void (* pfReadRect_18bpp_B18) (GUI_DEVICE * _pDevice, int _x0, int _y0, int _x1, int _y1, U32 * _pBuffer);
  //
  // Custom read functions
  //
  U16 (* pfReadPixelCust_16bpp) (int LayerIndex);
  U32 (* pfReadPixelCust_18bpp) (int LayerIndex);
  void(* pfReadMPixelCust_16bpp)(int LayerIndex, U16 * pBuffer, U32 NumPixels);
  void(* pfReadMPixelCust_18bpp)(int LayerIndex, U32 * pBuffer, U32 NumPixels);
  //
  // Mode dependent drawing functions
  //
  void    (* pfDrawBitmap   )(GUI_DEVICE *  _pDevice, int _x0, int _y0, int _xsize, int _ysize, int _BitsPerPixel, int _BytesPerLine, const U8 * _pData, int _Diff, const LCD_PIXELINDEX * _pTrans);
  void    (* pfFillRect     )(GUI_DEVICE *  _pDevice, int _x0, int _y0, int _x1, int _y1);
  unsigned(* pfGetPixelIndex)(GUI_DEVICE *  _pDevice, int _x, int _y);
  void    (* pfSetPixelIndex)(GUI_DEVICE *  _pDevice, int _x, int _y, int _ColorIndex);
  int     (* pfControlCache )(GUI_DEVICE *  _pDevice, int _Cmd);
  void    (* pfRefresh      )(GUI_DEVICE *  _pDevice);
  //
  // Controller dependent function pointers
  //
  void (* pfReadRect)(void);
  //
  // Orientation
  //
  int  (* pfLog2PhysX)(DRIVER_CONTEXT * _pContext, int _x, int _y);
  int  (* pfLog2PhysY)(DRIVER_CONTEXT * _pContext, int _x, int _y);
  //
  // Function pointer for setting up pfLog2Phys<X,Y>
  //
  void (* pfSetLog2Phys)(DRIVER_CONTEXT * _pContext);
  //
  // Hardware routines
  //
  GUI_PORT_API HW_API;
};

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void GUIDRV_FlexColor__InitOnce       (GUI_DEVICE * pDevice);
int  GUIDRV_FlexColor__ControlCache   (GUI_DEVICE * pDevice, int Cmd);
void GUIDRV_FlexColor__Refresh        (GUI_DEVICE * pDevice);

void GUIDRV_FlexColor__AddCacheRect   (DRIVER_CONTEXT * pContext);
void GUIDRV_FlexColor__ManageRect     (DRIVER_CONTEXT * pContext, int Cmd);
void GUIDRV_FlexColor__SetCacheAddr   (DRIVER_CONTEXT * pContext, int x, int y);
void GUIDRV_FlexColor__SetCacheRect   (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);
void GUIDRV_FlexColor__SetLog2Phys    (DRIVER_CONTEXT * pContext);
void GUIDRV_FlexColor__SetSubRect     (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);

unsigned int GUIDRV_FlexColor__GetPixelIndexCache  (GUI_DEVICE * pDevice, int x, int y);
void         GUIDRV_FlexColor__SetPixelIndexCache  (GUI_DEVICE * pDevice, int x, int y, int PixelIndex);
unsigned int GUIDRV_FlexColor__GetPixelIndexNoCache(GUI_DEVICE * pDevice, int x, int y);
void         GUIDRV_FlexColor__SetPixelIndexNoCache(GUI_DEVICE * pDevice, int x, int y, int PixelIndex);

void GUIDRV_FlexColor__SetFunc66712(GUI_DEVICE * pDevice, U16 AndMask_SetAddrRAM);

/*********************************************************************
*
*       Simulation (Segger internal use only)
*
**********************************************************************
*/
#if defined(WIN32) && defined(LCD_SIMCONTROLLER)

  extern GUI_PORT_API SIM_FlexColor_HW_API;

  void SIM_FlexColor_Config      (GUI_DEVICE * pDevice, int Orientation, int xSize, int ySize, int FirstSEG, int FirstCOM, int BitsPerPixel, int NumDummyReads);
  void SIM_FlexColor_SetBus8     (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus9     (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus16    (GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetBus32    (GUI_DEVICE * pDevice, int Shift);
  void SIM_FlexColor_SetFunc66702(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66708(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66709(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66712(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66714(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66715(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66718(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66719(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66720(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66721(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66722(GUI_DEVICE * pDevice);
  void SIM_FlexColor_SetFunc66772(GUI_DEVICE * pDevice);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* GUIDRV_FLEXCOLOR_PRIVATE_H */

/*************************** End of file ****************************/

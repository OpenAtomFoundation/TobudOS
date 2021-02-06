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
File        : GUIDRV_TemplateI_Private.h
Purpose     : Interface definition for GUIDRV_TemplateI driver
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
  
#include "GUIDRV_TemplateI.h"
#include "GUIDRV_NoOpt_1_8.h"

#ifndef GUIDRV_TEMPLATE_I_PRIVATE_H
#define GUIDRV_TEMPLATE_I_PRIVATE_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PRIVATE_DEVFUNC_ONINITHOOK 0x1000

//
// Use unique context identified
//
#define DRIVER_CONTEXT DRIVER_CONTEXT_TEMPLATE_I

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct DRIVER_CONTEXT DRIVER_CONTEXT;

typedef void (* T_ONINITHOOK)(DRIVER_CONTEXT * pContext);

/*********************************************************************
*
*       MANAGE_VMEM_API
*/
typedef struct {
  //
  // TBD: Add private function pointers...
  //
  int Dummy;
} MANAGE_VMEM_API;

/*********************************************************************
*
*       DRIVER_CONTEXT
*/
struct DRIVER_CONTEXT {
  //
  // Common data
  //
  int xSize, ySize;
  int vxSize, vySize;
  //
  // Driver specific data
  //
  //
  // Accelerators for calculation
  //
  int BytesPerLine;
  int BitsPerPixel;
  //
  // VRAM
  //
  U8 * pVMEM;
  //
  // Pointer to driver internal initialization routine
  //
  void (* pfInit) (GUI_DEVICE * pDevice);
  void (* pfCheck)(GUI_DEVICE * pDevice);
  //
  // API-Tables
  //
  MANAGE_VMEM_API ManageVMEM_API; // Memory management
  GUI_PORT_API    HW_API;         // Hardware routines
};

/*********************************************************************
*
*       LOG2PHYS_xxx
*/
#define LOG2PHYS_X      (                  x    )
#define LOG2PHYS_X_OX   (pContext->xSize - x - 1)
#define LOG2PHYS_X_OY   (                  x    )
#define LOG2PHYS_X_OXY  (pContext->xSize - x - 1)
#define LOG2PHYS_X_OS   (                  y    )
#define LOG2PHYS_X_OSX  (pContext->ySize - y - 1)
#define LOG2PHYS_X_OSY  (                  y    )
#define LOG2PHYS_X_OSXY (pContext->ySize - y - 1)

#define LOG2PHYS_Y      (                  y    )
#define LOG2PHYS_Y_OX   (                  y    )
#define LOG2PHYS_Y_OY   (pContext->ySize - y - 1)
#define LOG2PHYS_Y_OXY  (pContext->ySize - y - 1)
#define LOG2PHYS_Y_OS   (                  x    )
#define LOG2PHYS_Y_OSX  (                  x    )
#define LOG2PHYS_Y_OSY  (pContext->xSize - x - 1)
#define LOG2PHYS_Y_OSXY (pContext->xSize - x - 1)

/*********************************************************************
*
*       _SetPixelIndex_##EXT
*/
#define DEFINE_SETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                                      \
static void _SetPixelIndex_##EXT(GUI_DEVICE * pDevice, int x, int y, int PixelIndex) { \
  DRIVER_CONTEXT * pContext;                                                           \
                                                                                       \
  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;                                    \
  pContext->xSize = pContext->xSize; /* Keep compiler happy */                         \
  _SetPixelIndex(pDevice, X_PHYS, Y_PHYS, PixelIndex);                                 \
}

/*********************************************************************
*
*       _GetPixelIndex_##EXT
*/
#define DEFINE_GETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                              \
static unsigned int _GetPixelIndex_##EXT(GUI_DEVICE * pDevice, int x, int y) { \
  LCD_PIXELINDEX PixelIndex;                                                   \
  DRIVER_CONTEXT * pContext;                                                   \
                                                                               \
  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;                            \
  pContext->xSize = pContext->xSize; /* Keep compiler happy */                 \
  PixelIndex = _GetPixelIndex(pDevice, X_PHYS, Y_PHYS);                        \
  return PixelIndex;                                                           \
}

/*********************************************************************
*
*       _GetDevProp_##EXT
*/
#define DEFINE_GETDEVPROP(EXT, MX, MY, SWAP)                    \
static I32 _GetDevProp_##EXT(GUI_DEVICE * pDevice, int Index) { \
  switch (Index) {                                              \
  case LCD_DEVCAP_MIRROR_X: return MX;                          \
  case LCD_DEVCAP_MIRROR_Y: return MY;                          \
  case LCD_DEVCAP_SWAP_XY:  return SWAP;                        \
  }                                                             \
  return _GetDevProp(pDevice, Index);                           \
}

/*********************************************************************
*
*       DEFINE_FUNCTIONS
*/
#define DEFINE_FUNCTIONS(EXT, X_PHYS, Y_PHYS, MX, MY, SWAP) \
  DEFINE_SETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                 \
  DEFINE_GETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                 \
  DEFINE_GETDEVPROP(EXT, MX, MY, SWAP)                      \
  DEFINE_GUI_DEVICE_API(EXT)


/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void (*GUIDRV__TemplateI_GetDevFunc(GUI_DEVICE ** ppDevice, int Index))(void);
void   GUIDRV__TemplateI_SetOrg    (GUI_DEVICE *  pDevice,  int x, int y);
I32    GUIDRV__TemplateI_GetDevProp(GUI_DEVICE *  pDevice,  int Index);
void   GUIDRV__TemplateI_GetRect   (GUI_DEVICE *  pDevice,  LCD_RECT * pRect);

#endif

/*************************** End of file ****************************/

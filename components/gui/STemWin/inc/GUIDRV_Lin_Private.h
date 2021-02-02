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
File        : GUIDRV_Lin_Private.h
Purpose     : Common definitions and common code for all LIN-drivers
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
  
#ifndef GUIDRV_LIN_PRIVATE_H
#define GUIDRV_LIN_PRIVATE_H

#include <string.h>

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Common definitions for all variants of the LIN driver
*
**********************************************************************
*/
#if defined(WIN32)
  //
  // Simulation prototypes
  //
  U16  SIM_Lin_ReadMem16  (unsigned int Off);
  U32  SIM_Lin_ReadMem32  (unsigned int Off);
  U8   SIM_Lin_ReadMem08p (U8  * p);
  U32  SIM_Lin_ReadMem32p (U32 * p);
  void SIM_Lin_WriteMem16 (unsigned int Off, U16 Data);
  void SIM_Lin_WriteMem32 (unsigned int Off, U32 Data);
  void SIM_Lin_WriteMem08p(U8  * p, U8 Data);
  void SIM_Lin_WriteMem16p(U16 * p, U16 Data);
  void SIM_Lin_WriteMem32p(U32 * p, U32 Data);
  void SIM_Lin_memcpy     (void * pDst, const void * pSrc, int Len);
  void SIM_Lin_memset     (void * pDst, U8 Value, U32 Len);
  void SIM_Lin_SetVRAMAddr(int LayerIndex, void * pVRAM);
  void SIM_Lin_SetVRAMSize(int LayerIndex, int vxSize, int vySize, int xSize, int ySize);
  void SIM_Lin_CopyBuffer (int IndexSrc, int IndexDst);
  void SIM_Lin_ShowBuffer (int Index);
  //
  // Access macro definition for internal simulation
  //
  #define LCD_READ_MEM16(VRAMAddr, Off)        SIM_Lin_ReadMem16(Off)
  #define LCD_READ_MEM32(VRAMAddr, Off)        SIM_Lin_ReadMem32(Off)
  #define LCD_READ_MEM08P(p)                   SIM_Lin_ReadMem08p(p)
  #define LCD_READ_MEM32P(p)                   SIM_Lin_ReadMem32p(p)
  #define LCD_WRITE_MEM16(VRAMAddr, Off, Data) SIM_Lin_WriteMem16(Off, Data)
  #define LCD_WRITE_MEM32(VRAMAddr, Off, Data) SIM_Lin_WriteMem32(Off, Data)
  #define LCD_WRITE_MEM08P(p, Data)            SIM_Lin_WriteMem08p(p, Data)
  #define LCD_WRITE_MEM16P(p, Data)            SIM_Lin_WriteMem16p(p, Data)
  #define LCD_WRITE_MEM32P(p, Data)            SIM_Lin_WriteMem32p(p, Data)
  #undef  GUI_MEMCPY
  #define GUI_MEMCPY(pDst, pSrc, Len)          SIM_Lin_memcpy(pDst, pSrc, Len)
  #undef  GUI_MEMSET
  #define GUI_MEMSET(pDst, Value, Len)         SIM_Lin_memset(pDst, Value, Len)
#else
  //
  // Access macro definition for hardware
  //
  #define LCD_READ_MEM16(VRAMAddr, Off)        (*((U16 *)VRAMAddr + (U32)Off))
  #define LCD_READ_MEM32(VRAMAddr, Off)        (*((U32 *)VRAMAddr + (U32)Off))
  #define LCD_READ_MEM08P(p)                   (*((U8  *)p))
  #define LCD_READ_MEM32P(p)                   (*((U32 *)p))
  #define LCD_WRITE_MEM16(VRAMAddr, Off, Data) *((U16 *)VRAMAddr + (U32)Off) = Data
  #define LCD_WRITE_MEM32(VRAMAddr, Off, Data) *((U32 *)VRAMAddr + (U32)Off) = Data
  #define LCD_WRITE_MEM08P(p, Data)            *((U8  *)p) = Data
  #define LCD_WRITE_MEM16P(p, Data)            *((U16 *)p) = Data
  #define LCD_WRITE_MEM32P(p, Data)            *((U32 *)p) = Data
#endif

#define WRITE_MEM16(VRAMAddr, Off, Data) LCD_WRITE_MEM16(VRAMAddr, Off, Data)
#define WRITE_MEM32(VRAMAddr, Off, Data) LCD_WRITE_MEM32(VRAMAddr, Off, Data)
#define READ_MEM08P(p)                   LCD_READ_MEM08P(p)
#define READ_MEM16(VRAMAddr, Off)        LCD_READ_MEM16(VRAMAddr, Off)
#define READ_MEM32(VRAMAddr, Off)        LCD_READ_MEM32(VRAMAddr, Off)
#define READ_MEM32P(p)                   LCD_READ_MEM32P(p)
#define WRITE_MEM08P(p, Data)            LCD_WRITE_MEM08P(p, Data)
#define WRITE_MEM16P(p, Data)            LCD_WRITE_MEM16P(p, Data)
#define WRITE_MEM32P(p, Data)            LCD_WRITE_MEM32P(p, Data)

#define OFF2PTR08(VRAMAddr, Off)     (U8  *)((U8 *)VRAMAddr + (Off     ))
#define OFF2PTR16(VRAMAddr, Off)     (U16 *)((U8 *)VRAMAddr + (Off << 1))
#define OFF2PTR32(VRAMAddr, Off)     (U32 *)((U8 *)VRAMAddr + (Off << 2))

//
// Use unique context identified
//
#define DRIVER_CONTEXT DRIVER_CONTEXT_LIN

//
// Definition of default members for DRIVER_CONTEXT structure
//
#define DEFAULT_CONTEXT_MEMBERS                               \
  U32 VRAMAddr;                                               \
  U32 BaseAddr;                                               \
  int BufferIndex;                                            \
  int xSize, ySize;                                           \
  int vxSize, vySize;                                         \
  int vxSizePhys;                                             \
  int xPos, yPos;                                             \
  int Alpha;                                                  \
  int IsVisible;                                              \
  void (* pfFillRect)  (int /* LayerIndex */,                 \
                        int /* x0 */,                         \
                        int /* y0 */,                         \
                        int /* x1 */,                         \
                        int /* y1 */,                         \
                        U32 /* PixelIndex */);                \
  void (* pfCopyBuffer)(int /* LayerIndex */,                 \
                        int /* IndexSrc */,                   \
                        int /* IndexDst */);                  \
  void (* pfDrawBMP1)  (int /* LayerIndex */,                 \
                        int /* x */,                          \
                        int /* y */,                          \
                        U8 const * /* p */,                   \
                        int /* Diff */,                       \
                        int /* xSize */,                      \
                        int /* ySize */,                      \
                        int /* BytesPerLine */,               \
                        const LCD_PIXELINDEX * /* pTrans */); \
  void (* pfDrawBMP8)  (int /* LayerIndex */,                 \
                        int /* x */,                          \
                        int /* y */,                          \
                        U8 const * /* p */,                   \
                        int /* xSize */,                      \
                        int /* ySize */,                      \
                        int /* BytesPerLine */,               \
                        const LCD_PIXELINDEX * /* pTrans */); \
  void (* pfCopyRect)  (int /* LayerIndex */,                 \
                        int /* x0 */,                         \
                        int /* y0 */,                         \
                        int /* x1 */,                         \
                        int /* y1 */,                         \
                        int /* xSize */,                      \
                        int /* ySize */);

#ifndef   PRIVATE_CONTEXT_MEMBERS
  #define PRIVATE_CONTEXT_MEMBERS
#endif

//
// Definition of default function management for _GetDevFunc()
//
#define DEFAULT_MANAGEMENT_GETDEVFUNC()                                             \
  case LCD_DEVFUNC_SET_VRAM_ADDR:                                                   \
    return (void (*)(void))_SetVRAMAddr;                                            \
  case LCD_DEVFUNC_SET_VSIZE:                                                       \
    return (void (*)(void))_SetVSize;                                               \
  case LCD_DEVFUNC_SET_SIZE:                                                        \
    return (void (*)(void))_SetSize;                                                \
  case LCD_DEVFUNC_SETPOS:                                                          \
    return (void (*)(void))_SetPos;                                                 \
  case LCD_DEVFUNC_GETPOS:                                                          \
    return (void (*)(void))_GetPos;                                                 \
  case LCD_DEVFUNC_SETALPHA:                                                        \
    return (void (*)(void))_SetAlpha;                                               \
  case LCD_DEVFUNC_SETVIS:                                                          \
    return (void (*)(void))_SetVis;                                                 \
  case LCD_DEVFUNC_INIT:                                                            \
    return (void (*)(void))_Init;                                                   \
  case LCD_DEVFUNC_ON:                                                              \
    return (void (*)(void))_On;                                                     \
  case LCD_DEVFUNC_OFF:                                                             \
    return (void (*)(void))_Off;                                                    \
  case LCD_DEVFUNC_ALPHAMODE:                                                       \
    return (void (*)(void))_SetAlphaMode;                                           \
  case LCD_DEVFUNC_CHROMAMODE:                                                      \
    return (void (*)(void))_SetChromaMode;                                          \
  case LCD_DEVFUNC_CHROMA:                                                          \
    return (void (*)(void))_SetChroma;                                              \
  case LCD_DEVFUNC_COPYBUFFER:                                                      \
    return (void (*)(void))_CopyBuffer;                                             \
  case LCD_DEVFUNC_SHOWBUFFER:                                                      \
    return (void (*)(void))_ShowBuffer;                                             \
  case LCD_DEVFUNC_SETFUNC:                                                         \
    return (void (*)(void))_SetDevFunc;                                             \
  case LCD_DEVFUNC_FILLRECT:                                                        \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfFillRect; \
  case LCD_DEVFUNC_DRAWBMP_1BPP:                                                    \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfDrawBMP1; \
  case LCD_DEVFUNC_DRAWBMP_8BPP:                                                    \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfDrawBMP8; \
  case LCD_DEVFUNC_COPYRECT:                                                        \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfCopyRect;

//
// Definition of private function management for _GetDevFunc()
//
#ifndef   PRIVATE_MANAGEMENT_GETDEVFUNC
  #define PRIVATE_MANAGEMENT_GETDEVFUNC()
#endif

//
// Definition of default function management for _SetDevFunc()
//
#define DEFAULT_MANAGEMENT_SETDEVFUNC()                                                                                                                                      \
    case LCD_DEVFUNC_FILLRECT:                                                                                                                                               \
      pContext->pfFillRect   = (void (*)(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex))pFunc;                                                              \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_COPYBUFFER:                                                                                                                                             \
      pContext->pfCopyBuffer = (void (*)(int LayerIndex, int IndexSrc, int IndexDst))pFunc;                                                                                  \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_DRAWBMP_1BPP:                                                                                                                                           \
      pContext->pfDrawBMP1   = (void (*)(int LayerIndex, int x, int y, U8 const * p, int Diff, int xSize, int ySize, int BytesPerLine, const LCD_PIXELINDEX * pTrans))pFunc; \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_DRAWBMP_8BPP:                                                                                                                                           \
      pContext->pfDrawBMP8   = (void (*)(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine, const LCD_PIXELINDEX * pTrans))pFunc;           \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_COPYRECT:                                                                                                                                               \
      pContext->pfCopyRect   = (void (*)(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize))pFunc;                                                        \
      break;

//
// Definition of private function management for _GetDevFunc()
//
#ifndef   PRIVATE_MANAGEMENT_SETDEVFUNC
  #define PRIVATE_MANAGEMENT_SETDEVFUNC()
#endif

//
// Endian related definitions
//
#ifndef   LCD_MIRROR
  #define LCD_MIRROR 0
#endif

#if (LCD_MIRROR == 2)
#define MIRROR(x) x = ((x & 0x000000ffUL) <<  8) \
                    | ((x & 0x0000ff00UL) >>  8) \
                    | ((x & 0x00ff0000UL) <<  8) \
                    | ((x & 0xff000000UL) >>  8)
#else
#define MIRROR(x) x = ((x & 0x000000ffUL) << 24) \
                    | ((x & 0x0000ff00UL) <<  8) \
                    | ((x & 0x00ff0000UL) >>  8) \
                    | ((x & 0xff000000UL) >> 24)
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//
// DRIVER_CONTEXT structure consisting of default and private members
//
typedef struct {
  DEFAULT_CONTEXT_MEMBERS
  PRIVATE_CONTEXT_MEMBERS
} DRIVER_CONTEXT;

/*********************************************************************
*
*       Static code (common for all)
*
**********************************************************************
*/
/*********************************************************************
*
*       _InitOnce
*
* Purpose:
*   Allocates a fixed block for the context of the driver
*
* Return value:
*   0 on success, 1 on error
*/
static int _InitOnce(GUI_DEVICE * pDevice) {
  if (pDevice->u.pContext == NULL) {
    pDevice->u.pContext = GUI_ALLOC_GetFixedBlock(sizeof(DRIVER_CONTEXT));
    GUI__memset((U8 *)pDevice->u.pContext, 0, sizeof(DRIVER_CONTEXT));
  }
  return pDevice->u.pContext ? 0 : 1;
}

/*********************************************************************
*
*       _GetRect
*
* Purpose:
*   Returns the display size.
*/
static void _GetRect(GUI_DEVICE * pDevice, LCD_RECT * pRect) {
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pRect->x0 = 0;
  pRect->y0 = 0;
  pRect->x1 = pContext->vxSize - 1;
  pRect->y1 = pContext->vySize - 1;
}

/*********************************************************************
*
*       _SetVis
*
* Purpose:
*   Sets the visibility of the given layer by sending a LCD_X_SETVIS command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetVis(GUI_DEVICE * pDevice, int OnOff) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETVIS_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->IsVisible = OnOff;
    Data.OnOff = OnOff;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETVIS, (void *)&Data);
  }
}

/*********************************************************************
*
*       _SetPos
*
* Purpose:
*   Sets the position of the given layer by sending a LCD_X_SETPOS command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetPos(GUI_DEVICE * pDevice, int xPos, int yPos) {
  DRIVER_CONTEXT * pContext;
  int xSizeDisplay, ySizeDisplay, xSizeLayer, ySizeLayer, BitsPerPixel;
  LCD_X_SETPOS_INFO PosInfo = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->xPos = xPos;
    pContext->yPos = yPos;

    xSizeDisplay  = LCD_GetXSizeDisplay();
    ySizeDisplay  = LCD_GetYSizeDisplay();
    xSizeLayer    = pContext->xSize;
    ySizeLayer    = pContext->ySize;
    BitsPerPixel  = pDevice->pDeviceAPI->pfGetDevProp(pDevice, LCD_DEVCAP_BITSPERPIXEL);
    PosInfo.BytesPerPixel = (BitsPerPixel + 7) / 8;
    if (xPos < 0) {
      PosInfo.Off -= xPos * PosInfo.BytesPerPixel;
      PosInfo.xPos = 0;
      PosInfo.xLen = xSizeLayer + xPos;
    } else {
      PosInfo.xPos = xPos;
      PosInfo.xLen = xSizeLayer;
      if ((PosInfo.xPos + PosInfo.xLen) > xSizeDisplay) {
        PosInfo.xLen = xSizeDisplay - xPos;
      }
    }
    if (yPos < 0) {
      PosInfo.Off -= yPos * PosInfo.BytesPerPixel * xSizeLayer;
      PosInfo.yPos = 0;
      PosInfo.yLen = ySizeLayer + yPos;
    } else {
      PosInfo.yPos = yPos;
      PosInfo.yLen = ySizeLayer;
      if ((PosInfo.yPos + PosInfo.yLen) > ySizeDisplay) {
        PosInfo.yLen = ySizeDisplay - yPos;
      }
    }
    if ((PosInfo.xLen <= 0) || (PosInfo.yLen <= 0) || (PosInfo.xPos >= xSizeDisplay) || (PosInfo.yPos >= ySizeDisplay)) {
      if (pContext->IsVisible == 1) {
        _SetVis(pDevice, 0);
      }
      return;
    }
    if (pContext->IsVisible == 0) {
      _SetVis(pDevice, 1);
    }
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETPOS, (void *)&PosInfo);
  }
}

/*********************************************************************
*
*       _GetPos
*
* Purpose:
*   Returns the position of the given layer.
*/
static void _GetPos(GUI_DEVICE * pDevice, int * pxPos, int * pyPos) {
  DRIVER_CONTEXT * pContext;

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    *pxPos = pContext->xPos;
    *pyPos = pContext->yPos;
  }
}

/*********************************************************************
*
*       _SetAlpha
*
* Purpose:
*   Sets the alpha value of the given layer by sending a LCD_X_SETALPHA command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetAlpha(GUI_DEVICE * pDevice, int Alpha) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETALPHA_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->Alpha = Alpha;
    Data.Alpha = Alpha;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETALPHA, (void *)&Data);
  }
}

/*********************************************************************
*
*       _Init
*
* Purpose:
*   Called during the initialization process of emWin.
*/
static int  _Init(GUI_DEVICE * pDevice) {
  int r;

  r = _InitOnce(pDevice);
  r |= LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_INITCONTROLLER, NULL);
  return r;
}

/*********************************************************************
*
*       _On
*
* Purpose:
*   Sends a LCD_X_ON command to LCD_X_DisplayDriver().
*/
static void _On (GUI_DEVICE * pDevice) {
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_ON, NULL);
}

/*********************************************************************
*
*       _Off
*
* Purpose:
*   Sends a LCD_X_OFF command to LCD_X_DisplayDriver().
*/
static void _Off (GUI_DEVICE * pDevice) {
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_OFF, NULL);
}

/*********************************************************************
*
*       _SetAlphaMode
*
* Purpose:
*   Sets the alpha mode of the given layer by sending a LCD_X_SETALPHAMODE command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetAlphaMode(GUI_DEVICE * pDevice, int AlphaMode) {
  LCD_X_SETALPHAMODE_INFO Data = {0};

  Data.AlphaMode = AlphaMode;
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETALPHAMODE, (void *)&Data);
}

/*********************************************************************
*
*       _SetChromaMode
*
* Purpose:
*   Sets the chroma mode of the given layer by sending a LCD_X_SETCHROMAMODE command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetChromaMode(GUI_DEVICE * pDevice, int ChromaMode) {
  LCD_X_SETCHROMAMODE_INFO Data = {0};

  Data.ChromaMode = ChromaMode;
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETCHROMAMODE, (void *)&Data);
}

/*********************************************************************
*
*       _SetChroma
*
* Purpose:
*   Sets the chroma values of the given layer by sending a LCD_X_SETCHROMA command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetChroma(GUI_DEVICE * pDevice, LCD_COLOR ChromaMin, LCD_COLOR ChromaMax) {
  LCD_X_SETCHROMA_INFO Data = {0};

  Data.ChromaMin = ChromaMin;
  Data.ChromaMax = ChromaMax;
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETCHROMA, (void *)&Data);
}

/*********************************************************************
*
*       _CopyBuffer
*
* Purpose:
*   Copies the source buffer to the destination buffer and routes
*   further drawing operations to the destination buffer.
*
*   (Required for using multiple buffers)
*/
static void _CopyBuffer(GUI_DEVICE * pDevice, int IndexSrc, int IndexDst) {
  DRIVER_CONTEXT * pContext;
  #if (!defined(WIN32))
    U32 AddrSrc, AddrDst;
    I32 BufferSize;
    int BitsPerPixel;
  #endif

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (IndexSrc != IndexDst) {
      #if defined(WIN32)
        SIM_Lin_CopyBuffer(IndexSrc, IndexDst);
      #else
        BitsPerPixel = pDevice->pDeviceAPI->pfGetDevProp(pDevice, LCD_DEVCAP_BITSPERPIXEL);
        BufferSize = (((U32)pContext->xSize * pContext->ySize * BitsPerPixel) >> 3);
        AddrSrc = pContext->BaseAddr + BufferSize * IndexSrc;
        AddrDst = pContext->BaseAddr + BufferSize * IndexDst;
        if (pContext->pfCopyBuffer) {
          //
          // Use custom callback function for copy operation
          //
          pContext->pfCopyBuffer(pDevice->LayerIndex, IndexSrc, IndexDst);
        } else {
          //
          // Calculate pointers for copy operation
          //
          GUI_MEMCPY((void *)AddrDst, (void *)AddrSrc, BufferSize);
        }
        //
        // Set destination buffer as target for further drawing operations
        //
        pContext->VRAMAddr = AddrDst;
      #endif
    }
  }
}

/*********************************************************************
*
*       _ShowBuffer
*
* Purpose:
*   Sends a LCD_X_SHOWBUFFER command to LCD_X_DisplayDriver() to make the given buffer visible.
*
*   (Required for using multiple buffers)
*/
static void _ShowBuffer(GUI_DEVICE * pDevice, int Index) {
  LCD_X_SHOWBUFFER_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    #if defined(WIN32)
      SIM_Lin_ShowBuffer(Index);
    #else
      Data.Index = Index;
      LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SHOWBUFFER, (void *)&Data);
    #endif
  }
}

/*********************************************************************
*
*       _SetOrg
*
* Purpose:
*   Calls the driver callback function with the display origin to be set
*/
static void _SetOrg(GUI_DEVICE * pDevice, int x, int y) {
  #if (!defined(WIN32))
    DRIVER_CONTEXT * pContext;
    int Orientation;
  #endif
  LCD_X_SETORG_INFO Data = {0};

  #if defined(WIN32)
    LCDSIM_SetOrg(x, y, pDevice->LayerIndex);
  #else
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    Orientation  = LCD_GetMirrorXEx(pDevice->LayerIndex) * GUI_MIRROR_X;
    Orientation |= LCD_GetMirrorYEx(pDevice->LayerIndex) * GUI_MIRROR_Y;
    Orientation |= LCD_GetSwapXYEx (pDevice->LayerIndex) * GUI_SWAP_XY;
    switch (Orientation) {
    case 0:
      Data.xPos = x;
      Data.yPos = y;
      break;
    case GUI_MIRROR_X:
      Data.xPos = pContext->vxSize - pContext->xSize - x;
      Data.yPos = y;
      break;
    case GUI_MIRROR_Y:
      Data.xPos = x;
      Data.yPos = pContext->vySize - pContext->ySize - y;
      break;
    case GUI_MIRROR_X | GUI_MIRROR_Y:
      Data.xPos = pContext->vxSize - pContext->xSize - x;
      Data.yPos = pContext->vySize - pContext->ySize - y;
      break;
    case GUI_SWAP_XY:
      Data.xPos = y;
      Data.yPos = x;
      break;
    case GUI_SWAP_XY | GUI_MIRROR_X:
      Data.xPos = pContext->vySize - pContext->ySize  - y;
      Data.yPos = x;
      break;
    case GUI_SWAP_XY | GUI_MIRROR_Y:
      Data.xPos = y;
      Data.yPos = pContext->vxSize - pContext->xSize  - x;
      break;
    case GUI_SWAP_XY | GUI_MIRROR_X | GUI_MIRROR_Y:
      Data.xPos = pContext->vySize - pContext->ySize  - y;
      Data.yPos = pContext->vxSize - pContext->xSize  - x;
      break;
    }
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETORG, (void *)&Data);
  #endif
}

/*********************************************************************
*
*       _SetVRAMAddr
*/
static void _SetVRAMAddr(GUI_DEVICE * pDevice, void * pVRAM) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETVRAMADDR_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->VRAMAddr = pContext->BaseAddr = (U32)pVRAM;
    Data.pVRAM = pVRAM;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETVRAMADDR, (void *)&Data);
  }
  #if defined(WIN32)
    SIM_Lin_SetVRAMAddr(pDevice->LayerIndex, pVRAM);
  #endif
}

/*********************************************************************
*
*       _SetVSize
*/
static void _SetVSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
  DRIVER_CONTEXT * pContext;
  #if defined(WIN32)
    int NumBuffers;
  #endif

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    #if defined(WIN32)
      NumBuffers = GUI_MULTIBUF_GetNumBuffers();
    #endif
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (LCD_GetSwapXYEx(pDevice->LayerIndex)) {
      #if defined(WIN32)
        pContext->vxSize = xSize * NumBuffers;
      #else
        pContext->vxSize = xSize;
      #endif
      pContext->vySize = ySize;
      pContext->vxSizePhys = ySize;
    } else {
      pContext->vxSize = xSize;
      #if defined(WIN32)
        pContext->vySize = ySize * NumBuffers;
      #else
        pContext->vySize = ySize;
      #endif
      pContext->vxSizePhys = xSize;
    }
  }
  #if defined(WIN32)
    SIM_Lin_SetVRAMSize(pDevice->LayerIndex, pContext->vxSize, pContext->vySize, pContext->xSize, pContext->ySize);
  #endif
}

/*********************************************************************
*
*       _SetSize
*/
static void _SetSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETSIZE_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (pContext->vxSizePhys == 0) {
      if (LCD_GetSwapXYEx(pDevice->LayerIndex)) {
        pContext->vxSizePhys = ySize;
      } else {
        pContext->vxSizePhys = xSize;
      }
    }
    pContext->xSize = xSize;
    pContext->ySize = ySize;
    Data.xSize = xSize;
    Data.ySize = ySize;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETSIZE, (void *)&Data);
  }
}

#if defined(__cplusplus)
}
#endif

#endif /* GUIDRV_LIN_PRIVATE_H */

/*************************** End of file ****************************/

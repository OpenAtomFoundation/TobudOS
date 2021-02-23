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
File        : GUI_SetOrientationC0.c
Purpose     : Runtime display orientation without cache
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
  
#include "GUI_SetOrientation.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define XY2PTR(x, y) (PIXEL *)(pContext->pData + y * pContext->BytesPerLine + x * pContext->pDrawingAPI->BytesPerPixel)

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Sort
*
* Purpose:
*   Sorts the values pointed by the given pointers. Please note that
*   the same static function is also in GUI_SetOrientationC0.h
*   to enable better compiler optimization.
*/
static void _Sort(int * p0, int * p1) {
  int temp;

  if (*p0 > *p1) {
    temp = *p0;
    *p0  = *p1;
    *p1  = temp;
  }
}

/*********************************************************************
*
*       Static code: Bitmap drawing routines
*
**********************************************************************
*/
/*********************************************************************
*
*       Draw Bitmap 1 BPP
*/
static void _DrawBitLine1BPP(GUI_DEVICE * pDevice, unsigned x, unsigned y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX IndexMask, Index0, Index1, Pixel;
  unsigned (* pfGetPixelIndex)(GUI_DEVICE *, int, int);
  PIXEL * pData;
  int x_phys, y_phys;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  Index0 = *(pTrans + 0);
  Index1 = *(pTrans + 1);
  x += Diff;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
  case 0:
    do {
      *pData = (PIXEL)(*p & (0x80 >> Diff)) ? Index1 : Index0;
      pData += pContext->PixelOffset;
      if (++Diff == 8) {
        Diff = 0;
        p++;
      }
    } while (--xsize);
    break;
  case LCD_DRAWMODE_TRANS:
    do {
      if (*p & (0x80 >> Diff)) {
        *pData = Index1;
      }
      pData += pContext->PixelOffset;
      if (++Diff == 8) {
        Diff = 0;
        p++;
      }
    } while (--xsize);
    break;
  case LCD_DRAWMODE_XOR | LCD_DRAWMODE_TRANS:
  case LCD_DRAWMODE_XOR:
    pfGetPixelIndex = pDevice->pDeviceAPI->pfGetPixelIndex;
    IndexMask = pDevice->pColorConvAPI->pfGetIndexMask();
    do {
      if (*p & (0x80 >> Diff)) {
        Pixel = pfGetPixelIndex(pDevice, x, y);
        Pixel ^= IndexMask;
        *pData = Pixel;
      }
      pData += pContext->PixelOffset;
      x++;
      if (++Diff == 8) {
        Diff = 0;
        p++;
      }
    } while (--xsize);
    break;
  }
}

/*********************************************************************
*
*       Draw Bitmap 2 BPP
*/
static void _DrawBitLine2BPP(GUI_DEVICE * pDevice, int x, int y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixels, PixelIndex;
  int CurrentPixel, Shift, Index;
  PIXEL * pData;
  int x_phys, y_phys;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  Pixels = *p;
  CurrentPixel = Diff;
  x += Diff;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
  case 0:
    if (pTrans) {
      do {
        Shift = (3 - CurrentPixel) << 1;
        Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
        PixelIndex = *(pTrans + Index);
        *pData = (PIXEL)PixelIndex;
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 4) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    } else {
      do {
        Shift = (3 - CurrentPixel) << 1;
        Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
        *pData = (PIXEL)Index;
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 4) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    }
    break;
  case LCD_DRAWMODE_TRANS:
    if (pTrans) {
      do {
        Shift = (3 - CurrentPixel) << 1;
        Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
        if (Index) {
          PixelIndex = *(pTrans + Index);
          *pData = (PIXEL)PixelIndex;
        }
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 4) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    } else {
      do {
        Shift = (3 - CurrentPixel) << 1;
        Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
        if (Index) {
          *pData = (PIXEL)Index;
        }
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 4) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    }
    break;
  }
}

/*********************************************************************
*
*       Draw Bitmap 4 BPP
*/
static void _DrawBitLine4BPP(GUI_DEVICE * pDevice, int x, int y, U8 const * p, int Diff, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixels, PixelIndex;
  int CurrentPixel, Shift, Index;
  PIXEL * pData;
  int x_phys, y_phys;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  Pixels = *p;
  CurrentPixel = Diff;
  x += Diff;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
  case 0:
    if (pTrans) {
      do {
        Shift = (1 - CurrentPixel) << 2;
        Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
        PixelIndex = *(pTrans + Index);
        *pData = (PIXEL)PixelIndex;
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 2) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    } else {
      do {
        Shift = (1 - CurrentPixel) << 2;
        Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
        *pData = (PIXEL)Index;
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 2) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    }
    break;
  case LCD_DRAWMODE_TRANS:
    if (pTrans) {
      do {
        Shift = (1 - CurrentPixel) << 2;
        Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
        if (Index) {
          PixelIndex = *(pTrans + Index);
          *pData = (PIXEL)PixelIndex;
        }
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 2) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    } else {
      do {
        Shift = (1 - CurrentPixel) << 2;
        Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
        if (Index) {
          *pData = (PIXEL)Index;
        }
        pData += pContext->PixelOffset;
        if (++CurrentPixel == 2) {
          CurrentPixel = 0;
          Pixels = *(++p);
        }
      } while (--xsize);
    }
    break;
  }
}

/*********************************************************************
*
*       Draw Bitmap 8 BPP
*/
static void _DrawBitLine8BPP(GUI_DEVICE * pDevice, int x, int y, U8 const * p, int xsize, const LCD_PIXELINDEX * pTrans) {
  LCD_PIXELINDEX Pixel;
  PIXEL * pData;
  int x_phys, y_phys;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  switch (GUI_pContext->DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
  case 0:
    if (pTrans) {
      do {
        Pixel = *p++;
        *pData = (PIXEL)*(pTrans + Pixel);
        pData += pContext->PixelOffset;
      } while (--xsize);
    } else {
      do {
        *pData = (PIXEL)*p++;
        pData += pContext->PixelOffset;
      } while (--xsize);
    }
    break;
  case LCD_DRAWMODE_TRANS:
    if (pTrans) {
      do {
        Pixel = *p++;
        if (Pixel) {
          *pData = (PIXEL)*(pTrans + Pixel);
        }
        pData += pContext->PixelOffset;
      } while (--xsize);
    } else {
      do {
        Pixel = *p++;
        if (Pixel) {
          *pData = (PIXEL)Pixel;
        }
        pData += pContext->PixelOffset;
      } while (--xsize);
    }
    break;
  }
}

/*********************************************************************
*
*       Draw Bitmap 16 BPP
*/
static void _DrawBitLine16BPP(GUI_DEVICE * pDevice, int x, int y, U16 const * p, int xsize) {
  PIXEL * pData;
  int x_phys, y_phys, PixelOffset;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  PixelOffset = pContext->PixelOffset;
  do {
    *pData = (PIXEL)*p++;
    pData += PixelOffset;
  } while (--xsize);
}

/*********************************************************************
*
*       Draw Bitmap 32 BPP
*/
static void _DrawBitLine32BPP(GUI_DEVICE * pDevice, int x, int y, U32 const * p, int xsize) {
  PIXEL * pData;
  int x_phys, y_phys, PixelOffset;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  PixelOffset = pContext->PixelOffset;
  do {
    *pData = (PIXEL)*p++;
    pData += PixelOffset;
  } while (--xsize);
}

/*********************************************************************
*
*       Static code: API functions for drawing operations, no cache
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawBitmap_CX
*/
static void _DrawBitmap_CX(GUI_DEVICE * pDevice, int x0, int y0,
                       int xSize, int ySize,
                       int BitsPerPixel,
                       int BytesPerLine,
                       const U8 * pData, int Diff,
                       const LCD_PIXELINDEX * pTrans) {
  int x0_phys, y0_phys, x1_phys, y1_phys;
  int i;
  PIXEL * pDataBM;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  switch (BitsPerPixel) {
  case 1:
    for (i = 0; i < ySize; i++) {
      _DrawBitLine1BPP(pDevice, x0, i + y0, pData, Diff, xSize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 2:
    for (i = 0; i < ySize; i++) {
      _DrawBitLine2BPP(pDevice, x0, i + y0, pData, Diff, xSize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 4:
    for (i = 0; i < ySize; i++) {
      _DrawBitLine4BPP(pDevice, x0, i + y0, pData, Diff, xSize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 8:
    for (i = 0; i < ySize; i++) {
      _DrawBitLine8BPP(pDevice, x0, i + y0, pData, xSize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 16:
    for (i = 0; i < ySize; i++) {
      _DrawBitLine16BPP(pDevice, x0, i + y0, (U16 *)pData, xSize);
      pData += BytesPerLine;
    }
    break;
  case 32:
    for (i = 0; i < ySize; i++) {
      _DrawBitLine32BPP(pDevice, x0, i + y0, (U32 *)pData, xSize);
      pData += BytesPerLine;
    }
    break;
  }

  pContext->pfLog2Phys(pContext, x0 + Diff,             y0,             &x0_phys, &y0_phys);
  pContext->pfLog2Phys(pContext, x0 + Diff + xSize - 1, y0 + ySize - 1, &x1_phys, &y1_phys);
  _Sort(&x0_phys, &x1_phys);
  _Sort(&y0_phys, &y1_phys);
  pDataBM = XY2PTR(x0_phys, y0_phys);
  pDevice = pDevice->pNext;
  pDevice->pDeviceAPI->pfDrawBitmap(pDevice,
                                    x0_phys, y0_phys,
                                    x1_phys - x0_phys + 1,
                                    y1_phys - y0_phys + 1,
                                    pContext->pDrawingAPI->BytesPerPixel << 3,
                                    pContext->pDrawingAPI->BytesPerPixel * pContext->vxSize,
                                    (U8 *)pDataBM, 0, NULL);
}

/*********************************************************************
*
*       _GetPixelIndex_CX
*/
static unsigned int _GetPixelIndex_CX(GUI_DEVICE * pDevice, int x, int y) {
  PIXEL * pData;
  PIXEL Pixel;
  int x_phys, y_phys;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  Pixel = *pData;
  return Pixel;
}

/*********************************************************************
*
*       _SetPixelIndex_CX
*/
static void _SetPixelIndex_CX(GUI_DEVICE * pDevice, int x, int y, int PixelIndex) {
  PIXEL * pData;
  int x_phys, y_phys;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pContext->pfLog2Phys(pContext, x, y, &x_phys, &y_phys);
  pData = XY2PTR(x_phys, y_phys);
  *pData = (PIXEL)PixelIndex;
  pDevice = pDevice->pNext;
  pDevice->pDeviceAPI->pfSetPixelIndex(pDevice, x_phys, y_phys, PixelIndex);
}

/*********************************************************************
*
*       _XorPixel_CX
*/
static void _XorPixel_CX(GUI_DEVICE * pDevice, int x, int y) {
  PIXEL Pixel, IndexMask;

  IndexMask = pDevice->pColorConvAPI->pfGetIndexMask();
  Pixel = pDevice->pDeviceAPI->pfGetPixelIndex(pDevice, x, y);
  Pixel ^= IndexMask;
  pDevice->pDeviceAPI->pfSetPixelIndex(pDevice, x, y, Pixel);
}

/*********************************************************************
*
*       _DrawHLine_CX
*/
static void _DrawHLine_CX(GUI_DEVICE * pDevice, int x0, int y, int x1) {
  pDevice->pDeviceAPI->pfFillRect(pDevice, x0, y, x1, y);
}

/*********************************************************************
*
*       _DrawVLine_CX
*/
static void _DrawVLine_CX(GUI_DEVICE * pDevice, int x, int y0, int y1) {
  pDevice->pDeviceAPI->pfFillRect(pDevice, x, y0, x, y1);
}

/*********************************************************************
*
*       _FillRect_CX
*/
static void _FillRect_CX(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1) {
  PIXEL * pData;
  PIXEL * pLine;
  PIXEL * pPixel;
  PIXEL Pixel, IndexMask;
  int x0_phys, y0_phys, x1_phys, y1_phys;
  int NumPixels, NumLines;
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pContext->pfLog2Phys(pContext, x0, y0, &x0_phys, &y0_phys);
  pContext->pfLog2Phys(pContext, x1, y1, &x1_phys, &y1_phys);
  _Sort(&x0_phys, &x1_phys);
  _Sort(&y0_phys, &y1_phys);
  pData = pLine = XY2PTR(x0_phys, y0_phys);
  NumLines = y1_phys - y0_phys + 1;
  if (GUI_pContext->DrawMode & LCD_DRAWMODE_XOR) {
    IndexMask = pDevice->pColorConvAPI->pfGetIndexMask();
    do {
      pPixel    = pLine;
      NumPixels = x1_phys - x0_phys + 1;
      do {
        *pPixel++ ^= IndexMask;
      } while (--NumPixels);
      pLine += pContext->vxSize;
    } while (--NumLines);
  } else {
    Pixel = (PIXEL)LCD__GetColorIndex();
    if (sizeof(Pixel) == 1) {
      NumPixels = x1_phys - x0_phys + 1;
      do {
        GUI_MEMSET((U8 *)pLine, Pixel, NumPixels);
        pLine += pContext->vxSize;
      } while (--NumLines);
    } else {
      do {
        pPixel    = pLine;
        NumPixels = x1_phys - x0_phys + 1;
        do {
          *pPixel++ = Pixel;
        } while (--NumPixels);
        pLine += pContext->vxSize;
      } while (--NumLines);
    }
  }
  pDevice = pDevice->pNext;
  pDevice->pDeviceAPI->pfDrawBitmap(pDevice,
                                    x0_phys, y0_phys,
                                    x1_phys - x0_phys + 1,
                                    y1_phys - y0_phys + 1,
                                    pContext->pDrawingAPI->BytesPerPixel << 3,
                                    pContext->pDrawingAPI->BytesPerPixel * pContext->vxSize,
                                    (U8 *)pData, 0, NULL);
}

/*********************************************************************
*
*       Static data: Drawing API(s)
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_OrientationAPI_CX
*/
const GUI_ORIENTATION_API API_NAME = {
  _DrawBitmap_CX,
  _DrawHLine_CX,
  _DrawVLine_CX,
  _FillRect_CX,
  _GetPixelIndex_CX,
  _SetPixelIndex_CX,
  _XorPixel_CX,
  BYTES_PER_PIXEL
};

/*************************** End of file ****************************/

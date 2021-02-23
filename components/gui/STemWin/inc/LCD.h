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
File        : LCD.h
Purpose     : Declares LCD interface functions
----------------------------------------------------------------------
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
  
#ifndef LCD_H
#define LCD_H

#include "GUI_ConfDefaults.h" /* Used for GUI_CONST_STORAGE */
#include "Global.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Basic type defines
*
* The follwing are defines for types used in the LCD-driver and the
* GUI layers on top of that. Since "C" does not provide data types of
* fixed length which are identical on all platforms, this is done here.
* For most 16/32 controllers, the settings will work fine. However, if
* you have similar defines in other sections of your program, you might
* want to change or relocate these defines, e.g. in a TYPE.h file.
*/
#define I16P I16              /*   signed 16 bits OR MORE ! */
#define U16P U16              /* unsigned 16 bits OR MORE ! */

/*********************************************************************
*
*       Settings for windows simulation
*
* Some settings in the configuration may conflict with the values required
* in the Simulation. This is why we ignore the target settings for data
* types and use the correct settings for the simulation.
* (U32 could be defined as long, which would yield a 64 bit type on
* the PC)
*/
#ifdef WIN32
  #pragma warning( disable : 4244 )  // Disable warning messages in simulation
  #pragma warning( disable : 4761 )  // Disable warning "integral size mismatch in argument; conversion supplied"
#endif

/*********************************************************************
*
*       Constants
*/
#define LCD_ERR0 (0x10)
#define LCD_ERR_CONTROLLER_NOT_FOUND (LCD_ERR0+1)
#define LCD_ERR_MEMORY               (LCD_ERR0+2)

/*********************************************************************
*
*       Drawing modes
*/
#define LCD_DRAWMODE_NORMAL (0)
#define LCD_DRAWMODE_XOR    (1<<0)
#define LCD_DRAWMODE_TRANS  (1<<1)
#define LCD_DRAWMODE_REV    (1<<2)

/*********************************************************************
*
*       Typedefs
*/
typedef int LCD_DRAWMODE;
typedef U32 LCD_COLOR;

/*********************************************************************
*
*       Data structures
*/
typedef struct { I16P x,y; } GUI_POINT;
typedef struct { I16 x0,y0,x1,y1; } LCD_RECT;

typedef struct {
  int              NumEntries;
  char             HasTrans;
  const LCD_COLOR * pPalEntries;
} LCD_LOGPALETTE;

/* This is used for the simulation only ! */
typedef struct {
  int x,y;
  unsigned char KeyStat;
} LCD_tMouseState;

typedef struct {
  int               NumEntries;
  const LCD_COLOR * pPalEntries;
} LCD_PHYSPALETTE;

/*********************************************************************
*
*       LCD_L0_... color conversion
*/
typedef LCD_COLOR      tLCDDEV_Index2Color  (unsigned Index);
typedef unsigned int   tLCDDEV_Color2Index  (LCD_COLOR Color);
typedef unsigned int   tLCDDEV_GetIndexMask (void);

typedef void tLCDDEV_Index2ColorBulk(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex);
typedef void tLCDDEV_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex);

/*********************************************************************
*
*       Color conversion API tables
*/
typedef struct {
  tLCDDEV_Color2Index  * pfColor2Index;
  tLCDDEV_Index2Color  * pfIndex2Color;
  tLCDDEV_GetIndexMask * pfGetIndexMask;
  int NoAlpha;
  tLCDDEV_Color2IndexBulk * pfColor2IndexBulk;
  tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk;
} LCD_API_COLOR_CONV;

extern const LCD_API_COLOR_CONV LCD_API_ColorConv_0;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_2;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_4;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_5;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_8;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_24;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_2;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_4;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_5;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_6;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1616I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_111;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_222;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_233;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_323;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_332;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_444_12;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_444_12_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_444_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_555;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_565;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_556;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_655;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_666;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_666_9;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_822216;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_84444;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8666;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8666_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_88666I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M111;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M1555I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M222;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M233;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M323;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M332;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M4444I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M444_12;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M444_12_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M444_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M555;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M565;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M556;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M655;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M666;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M666_9;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M8565;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M8888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M8888I;

#define GUICC_0         &LCD_API_ColorConv_0
#define GUICC_1         &LCD_API_ColorConv_1
#define GUICC_1_2       &LCD_API_ColorConv_1_2
#define GUICC_1_4       &LCD_API_ColorConv_1_4
#define GUICC_1_5       &LCD_API_ColorConv_1_5
#define GUICC_1_8       &LCD_API_ColorConv_1_8
#define GUICC_1_16      &LCD_API_ColorConv_1_16
#define GUICC_1_24      &LCD_API_ColorConv_1_24
#define GUICC_2         &LCD_API_ColorConv_2
#define GUICC_4         &LCD_API_ColorConv_4
#define GUICC_5         &LCD_API_ColorConv_5
#define GUICC_6         &LCD_API_ColorConv_6
#define GUICC_8         &LCD_API_ColorConv_8
#define GUICC_16        &LCD_API_ColorConv_16
#define GUICC_1616I     &LCD_API_ColorConv_1616I
#define GUICC_111       &LCD_API_ColorConv_111
#define GUICC_222       &LCD_API_ColorConv_222
#define GUICC_233       &LCD_API_ColorConv_233
#define GUICC_323       &LCD_API_ColorConv_323
#define GUICC_332       &LCD_API_ColorConv_332
#define GUICC_M4444I    &LCD_API_ColorConv_M4444I
#define GUICC_444_12    &LCD_API_ColorConv_444_12
#define GUICC_444_12_1  &LCD_API_ColorConv_444_12_1
#define GUICC_444_16    &LCD_API_ColorConv_444_16
#define GUICC_555       &LCD_API_ColorConv_555
#define GUICC_565       &LCD_API_ColorConv_565
#define GUICC_556       &LCD_API_ColorConv_556
#define GUICC_655       &LCD_API_ColorConv_655
#define GUICC_666       &LCD_API_ColorConv_666
#define GUICC_666_9     &LCD_API_ColorConv_666_9
#define GUICC_822216    &LCD_API_ColorConv_822216
#define GUICC_84444     &LCD_API_ColorConv_84444
#define GUICC_8666      &LCD_API_ColorConv_8666
#define GUICC_8666_1    &LCD_API_ColorConv_8666_1
#define GUICC_88666I    &LCD_API_ColorConv_88666I
#define GUICC_888       &LCD_API_ColorConv_888
#define GUICC_8888      &LCD_API_ColorConv_8888
#define GUICC_M111      &LCD_API_ColorConv_M111
#define GUICC_M1555I    &LCD_API_ColorConv_M1555I
#define GUICC_M222      &LCD_API_ColorConv_M222
#define GUICC_M233      &LCD_API_ColorConv_M233
#define GUICC_M323      &LCD_API_ColorConv_M323
#define GUICC_M332      &LCD_API_ColorConv_M332
#define GUICC_M444_12   &LCD_API_ColorConv_M444_12
#define GUICC_M444_12_1 &LCD_API_ColorConv_M444_12_1
#define GUICC_M444_16   &LCD_API_ColorConv_M444_16
#define GUICC_M555      &LCD_API_ColorConv_M555
#define GUICC_M565      &LCD_API_ColorConv_M565
#define GUICC_M556      &LCD_API_ColorConv_M556
#define GUICC_M655      &LCD_API_ColorConv_M655
#define GUICC_M666      &LCD_API_ColorConv_M666
#define GUICC_M666_9    &LCD_API_ColorConv_M666_9
#define GUICC_M8565     &LCD_API_ColorConv_M8565
#define GUICC_M888      &LCD_API_ColorConv_M888
#define GUICC_M8888     &LCD_API_ColorConv_M8888
#define GUICC_M8888I    &LCD_API_ColorConv_M8888I

void GUICC_M1555I_SetCustColorConv(tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M565_SetCustColorConv  (tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M4444I_SetCustColorConv(tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M888_SetCustColorConv  (tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M8888I_SetCustColorConv(tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);

/*********************************************************************
*
*       Compatibility defines for older versions
*/
#define GUI_COLOR_CONV_1    GUICC_1
#define GUI_COLOR_CONV_2    GUICC_2
#define GUI_COLOR_CONV_4    GUICC_4
#define GUI_COLOR_CONV_8666 GUICC_8666
#define GUI_COLOR_CONV_888  GUICC_888
#define GUI_COLOR_CONV_8888 GUICC_8888
#define GUI_COLOR_CONV_565  GUICC_565
#define GUI_COLOR_CONV_M565 GUICC_M565

/*********************************************************************
*
*       LCDDEV function table
*
*  Below the routines which need to in an LCDDEV routine table are
*  defined. All of these routines have to be in the low-level driver
*  (LCD_L0) or in the memory device which can be used to replace the
*  driver.
*  The one exception to this is the SetClipRect routine, which would
*  be identical for all drivers and is therefor contained in the
*  level above (LCD).
*/
typedef void         tLCDDEV_DrawPixel    (int x, int y);
typedef void         tLCDDEV_DrawHLine    (int x0, int y0,  int x1);
typedef void         tLCDDEV_DrawVLine    (int x , int y0,  int y1);
typedef void         tLCDDEV_FillRect     (int x0, int y0, int x1, int y1);
typedef unsigned int tLCDDEV_GetPixelIndex(int x, int y);
typedef void         tLCDDEV_SetPixelIndex(int x, int y, int ColorIndex);
typedef void         tLCDDEV_XorPixel     (int x, int y);
typedef void         tLCDDEV_FillPolygon  (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
typedef void         tLCDDEV_FillPolygonAA(const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
typedef void         tLCDDEV_GetRect      (LCD_RECT * pRect);
typedef int          tLCDDEV_Init         (void);
typedef void         tLCDDEV_On           (void);
typedef void         tLCDDEV_Off          (void);
typedef void         tLCDDEV_SetLUTEntry  (U8 Pos, LCD_COLOR color);
typedef void *       tLCDDEV_GetDevFunc   (int Index);
typedef I32          tLCDDEV_GetDevProp   (int Index);
typedef void         tLCDDEV_SetOrg       (int x, int y);

/*********************************************************************
*
*       Memory device API tables
*/
typedef struct GUI_DEVICE     GUI_DEVICE;
typedef struct GUI_DEVICE_API GUI_DEVICE_API;

typedef void tLCDDEV_DrawBitmap   (int x0, int y0, int xsize, int ysize,
                       int BitsPerPixel, int BytesPerLine,
                       const U8 * pData, int Diff,
                       const void * pTrans);   /* Really LCD_PIXELINDEX, but is void to avoid compiler warnings */
#define GUI_MEMDEV_APILIST_1  &GUI_MEMDEV_DEVICE_1
#define GUI_MEMDEV_APILIST_8  &GUI_MEMDEV_DEVICE_8
#define GUI_MEMDEV_APILIST_16 &GUI_MEMDEV_DEVICE_16
#define GUI_MEMDEV_APILIST_32 &GUI_MEMDEV_DEVICE_32

/*********************************************************************
*
*       Defines for device capabilities
*
* The following is the list of device capabilities which can, but do
* not have to be implemented in the driver. This way the driver can be
* enhanced in the future without affecting the driver interface,
* keeping older drivers compatible.
* More DevCaps can always be added in the future, as older drivers
* are guaranteed to return 0 for all unimplemented features or queries.
*
* The values below define the legal parameters to the LCD_GetDeviceCaps
* and the LCD_GetpCapFunc routines.
*/

#define LCD_DEVCAP_XSIZE             0x01    /* Quest horiz. res. of display */
#define LCD_DEVCAP_YSIZE             0x02    /* Quest vert. res. of display */
#define LCD_DEVCAP_VXSIZE            0x03    /* Quest vert. res. of virtual disp.*/
#define LCD_DEVCAP_VYSIZE            0x04    /* Quest vert. res. of virtual disp.*/
#define LCD_DEVCAP_XORG              0x05    /* X-origin ... usually 0 */
#define LCD_DEVCAP_YORG              0x06    /* Y-origin ... usually 0 */
#define LCD_DEVCAP_CONTROLLER        0x07    /* LCD Controller (Numerical) */
#define LCD_DEVCAP_BITSPERPIXEL      0x08    /* Bits per pixel ... 1/2/4/8 */
#define LCD_DEVCAP_NUMCOLORS         0x09    /* Quest number of colors */
#define LCD_DEVCAP_XMAG              0x0A
#define LCD_DEVCAP_YMAG              0x0B
#define LCD_DEVCAP_MIRROR_X          0x0C
#define LCD_DEVCAP_MIRROR_Y          0x0D
#define LCD_DEVCAP_SWAP_XY           0x0E
#define LCD_DEVCAP_SWAP_RB           0x0F

int LCD_GetXSizeMax(void);
int LCD_GetYSizeMax(void);
int LCD_GetVXSizeMax(void);
int LCD_GetVYSizeMax(void);
int LCD_GetBitsPerPixelMax(void);
void LCD_SetDisplaySize(int xSizeDisplay, int ySizeDisplay);
int LCD_GetXSizeDisplay(void);
int LCD_GetYSizeDisplay(void);

int LCD_GetXSizeEx          (int LayerIndex);
int LCD_GetYSizeEx          (int LayerIndex);
int LCD_GetVXSizeEx         (int LayerIndex);
int LCD_GetVYSizeEx         (int LayerIndex);
int LCD_GetBitsPerPixelEx   (int LayerIndex);
U32 LCD_GetNumColorsEx      (int LayerIndex);
int LCD_GetXMagEx           (int LayerIndex);
int LCD_GetYMagEx           (int LayerIndex);
int LCD_GetMirrorXEx        (int LayerIndex);
int LCD_GetMirrorYEx        (int LayerIndex);
int LCD_GetSwapXYEx         (int LayerIndex);
int LCD_GetReversLUTEx      (int LayerIndex);
int LCD_GetPhysColorsInRAMEx(int LayerIndex);

int LCD_GetXSize            (void);
int LCD_GetYSize            (void);
int LCD_GetVXSize           (void);
int LCD_GetVYSize           (void);
int LCD_GetBitsPerPixel     (void);
U32 LCD_GetNumColors        (void);
int LCD_GetXMag             (void);
int LCD_GetYMag             (void);
int LCD_GetMirrorX          (void);
int LCD_GetMirrorY          (void);
int LCD_GetSwapXY           (void);
int LCD_GetReversLUT        (void);
int LCD_GetPhysColorsInRAM  (void);

I32 LCD__GetBPP      (U32 IndexMask);
I32 LCD__GetBPPDevice(U32 IndexMask);

tLCDDEV_Index2Color * LCD_GetpfIndex2ColorEx(int LayerIndex);
tLCDDEV_Color2Index * LCD_GetpfColor2IndexEx(int LayerIndex);

tLCDDEV_Color2Index * LCD_GetpfColor2Index(void);

int LCD_GetNumLayers(void);

LCD_COLOR * LCD_GetPalette  (void);
LCD_COLOR * LCD_GetPaletteEx(int LayerIndex);

void (* LCD_GetDevFunc(int LayerIndex, int Item))(void);

/*********************************************************************
*
*       Values for requesting and setting function pointers (display driver)
*/
                                       /* Request of a function pointer for... */
#define LCD_DEVFUNC_READRECT      0x01 /* ...reading a rectangular display area */
#define LCD_DEVFUNC_SETALPHA      0x02 /* ...setting the alpha blending factor */
#define LCD_DEVFUNC_SETPOS        0x03 /* ...setting the layer position */
#define LCD_DEVFUNC_GETPOS        0x04 /* ...getting the layer position */
#define LCD_DEVFUNC_SETSIZE       0x05 /* ...setting the layer size */
#define LCD_DEVFUNC_SETVIS        0x06 /* ...setting the visibility of a layer */
#define LCD_DEVFUNC_24BPP         0x07 /* ...drawing 24bpp bitmaps */
#define LCD_DEVFUNC_NEXT_PIXEL    0x08 /* ...drawing a bitmap pixel by pixel */
#define LCD_DEVFUNC_SET_VRAM_ADDR 0x09 /* ...setting the VRAM address */
#define LCD_DEVFUNC_SET_VSIZE     0x0A /* ...setting the VRAM size */
#define LCD_DEVFUNC_SET_SIZE      0x0B /* ...setting the display size */
#define LCD_DEVFUNC_INIT          0x0C /* ...initializing the display controller */
#define LCD_DEVFUNC_CONTROLCACHE  0x0D /* ...controlling the cache */
#define LCD_DEVFUNC_ON            0x0E /* ...switching the display on */
#define LCD_DEVFUNC_OFF           0x0F /* ...switching the display off */
#define LCD_DEVFUNC_SETLUTENTRY   0x10 /* ...setting a LUT entry */
#define LCD_DEVFUNC_FILLPOLY      0x11 /* ...filling a polygon */
#define LCD_DEVFUNC_FILLPOLYAA    0x12 /* ...filling an antialiased polygon */
#define LCD_DEVFUNC_ALPHAMODE     0x13 /* ...setting the alpha blending mode */
#define LCD_DEVFUNC_CHROMAMODE    0x14 /* ...setting the chroma blending mode */
#define LCD_DEVFUNC_CHROMA        0x15 /* ...setting the chroma values */
#define LCD_DEVFUNC_SETFUNC       0x16 /* ...setting a function pointer */
#define LCD_DEVFUNC_REFRESH       0x17 /* ...refreshing the display */
#define LCD_DEVFUNC_SETRECT       0x18 /* ...setting the drawing rectangle */
                                       /* Setting a function pointer for... */
#define LCD_DEVFUNC_FILLRECT      0x19 /* ...filling a rectangular area */
#define LCD_DEVFUNC_DRAWBMP_1BPP  0x20 /* ...drawing a 1bpp bitmap */
#define LCD_DEVFUNC_COPYBUFFER    0x21 /* ...copying complete frame buffers */
#define LCD_DEVFUNC_SHOWBUFFER    0x22 /* ...shows the given buffer */
#define LCD_DEVFUNC_COPYRECT      0x23 /* ...filling a rectangular area */
#define LCD_DEVFUNC_DRAWBMP_16BPP 0x24 /* ...drawing a 16bpp bitmap */
#define LCD_DEVFUNC_DRAWBMP_8BPP  0x25 /* ...drawing a 8bpp bitmap */
#define LCD_DEVFUNC_READPIXEL     0x26 /* ...reading a pixel index */
#define LCD_DEVFUNC_READMPIXELS   0x27 /* ...reading multiple pixel indices */
#define LCD_DEVFUNC_DRAWBMP_32BPP 0x28 /* ...drawing a 32bpp bitmap */

/*********************************************************************
*
*       Values for requesting function pointers (memory devices)
*/
                                          /* Request of a function pointer for... */
#define MEMDEV_DEVFUNC_WRITETOACTIVE 0x16 /* ...writing the memory device */

/*********************************************************************
*
*       Values for requesting data
*/
                                       /* Request pointer to... */
#define LCD_DEVDATA_MEMDEV        0x01 /* ...default memory device API */
#define LCD_DEVDATA_PHYSPAL       0x02 /* ...physical palette */

/*********************************************************************
*
*       Structures for passing data to LCD_X_DisplayDriver()
*/
typedef struct {
  void * pVRAM;
} LCD_X_SETVRAMADDR_INFO;

typedef struct {
  int xPos, yPos;
} LCD_X_SETORG_INFO;

typedef struct {
  LCD_COLOR Color;
  U8 Pos;
} LCD_X_SETLUTENTRY_INFO;

typedef struct {
  int xSize, ySize;
} LCD_X_SETSIZE_INFO;

typedef struct {
  int xPos, yPos;
  int xLen, yLen;
  int BytesPerPixel;
  U32 Off;
} LCD_X_SETPOS_INFO;

typedef struct {
  int Alpha;
} LCD_X_SETALPHA_INFO;

typedef struct {
  int OnOff;
} LCD_X_SETVIS_INFO;

typedef struct {
  int AlphaMode;
} LCD_X_SETALPHAMODE_INFO;

typedef struct {
  int ChromaMode;
} LCD_X_SETCHROMAMODE_INFO;

typedef struct {
  LCD_COLOR ChromaMin;
  LCD_COLOR ChromaMax;
} LCD_X_SETCHROMA_INFO;

typedef struct {
  int Index;
} LCD_X_SHOWBUFFER_INFO;

/*********************************************************************
*
*       Commands for LCD_X_DisplayDriver()
*/
#define LCD_X_INITCONTROLLER 0x01 /* Initializing the display controller */
#define LCD_X_SETVRAMADDR    0x02 /* Setting the video RAM address */
#define LCD_X_SETORG         0x03 /* Setting the origin within a layer */
#define LCD_X_SETLUTENTRY    0x04 /* Setting an entry of the LUT */
#define LCD_X_ON             0x05 /* Switching the display on */
#define LCD_X_OFF            0x06 /* Switching the display off */
#define LCD_X_SETSIZE        0x07 /* Setting the layer size */
#define LCD_X_SETPOS         0x08 /* Setting the layer position */
#define LCD_X_SETVIS         0x09 /* Setting the visibility of a layer */
#define LCD_X_SETALPHA       0x0A /* Setting the alpha value of the layer */
#define LCD_X_SETALPHAMODE   0x0B /* Setting the alpha blending mode */
#define LCD_X_SETCHROMAMODE  0x0C /* Setting the chroma blending mode */
#define LCD_X_SETCHROMA      0x0D /* Setting the chroma values */
#define LCD_X_SHOWBUFFER     0x0E /* Switching to the given buffer */

int  LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData);
void LCD_X_Config(void);

/*********************************************************************
*
*       Set layer properties
*/
int LCD_SetAlphaEx     (int LayerIndex, int Alpha);
int LCD_SetPosEx       (int LayerIndex, int xPos, int yPos);
int LCD_SetSizeEx      (int LayerIndex, int xSize, int ySize);
int LCD_SetVisEx       (int LayerIndex, int OnOff);
int LCD_SetVRAMAddrEx  (int LayerIndex, void * pVRAM);
int LCD_SetVSizeEx     (int LayerIndex, int xSize, int ySize);
int LCD_SetAlphaModeEx (int LayerIndex, int AlphaMode);
int LCD_SetChromaModeEx(int LayerIndex, int ChromaMode);
int LCD_SetChromaEx    (int LayerIndex, LCD_COLOR ChromaMin, LCD_COLOR ChromaMax);

int LCD_SetAlpha     (int Alpha);
int LCD_SetVRAMAddr  (void * pVRAM);
int LCD_SetVSize     (int xSize, int ySize);
int LCD_SetSize      (int xSize, int ySize);
int LCD_SetVis       (int OnOff);
int LCD_SetPos       (int xPos, int yPos);
int LCD_SetAlphaMode (int AlphaMode);
int LCD_SetChromaMode(int ChromaMode);
int LCD_SetChroma    (LCD_COLOR ChromaMin, LCD_COLOR ChromaMax);
int LCD_SetLUTEntry  (U8 Pos, LCD_COLOR Color);
int LCD_SetDevFunc   (int LayerIndex, int IdFunc, void (* pDriverFunc)(void));

/*********************************************************************
*
*       Get layer properties
*/
int LCD_GetPosEx(int LayerIndex, int * pxPos, int * pyPos);

int LCD_GetPos  (int * pxPos, int * pyPos);

/*********************************************************************
*
*       Display refresh (optional)
*/
int LCD_Refresh  (void);
int LCD_RefreshEx(int LayerIndex);

/*********************************************************************
*
*       NEXT_PIXEL API support
*/
typedef struct {
  int  (* pfStart)   (int x0, int y0, int x1, int y1);
  void (* pfSetPixel)(int PixelIndex);
  void (* pfNextLine)(void);
  void (* pfEnd)     (void);
} LCD_API_NEXT_PIXEL;

LCD_API_NEXT_PIXEL * LCD_GetNextPixelAPI(void);

/*********************************************************************
*
*      LCD_CLIP function table
*/
typedef void tLCD_HL_DrawHLine    (int x0, int y0,  int x1);
typedef void tLCD_HL_DrawPixel    (int x0, int y0);

typedef struct {
  tLCD_HL_DrawHLine * pfDrawHLine;
  tLCD_HL_DrawPixel * pfDrawPixel;
} tLCD_HL_APIList;

void LCD_DrawHLine(int x0, int y0,  int x1);
void LCD_DrawPixel(int x0, int y0);
void LCD_DrawVLine(int x,  int y0,  int y1);


/*********************************************************************
*
*       Declarations for LCD_
*/
void LCD_SetClipRectEx(const LCD_RECT * pRect);
void LCD_SetClipRectMax(void);

/* Get device capabilities (0 if not supported) */
I32  LCD_GetDevCap  (int Index);
I32  LCD_GetDevCapEx(int LayerIndex, int Index);

/* Initialize LCD using config-parameters */
int emWin_LCD_Init(void);
int LCD_InitColors(void);

void LCD_SetBkColor   (LCD_COLOR Color); /* Set background color */
void LCD_SetColor     (LCD_COLOR Color); /* Set foreground color */
void LCD_SetPixelIndex(int x, int y, int ColorIndex);

/* Palette routines (Not available on all drivers) */
void LCD_InitLUT(void);
int  LCD_SetLUTEntryEx(int LayerIndex, U8 Pos, LCD_COLOR Color);
void LCD_SetLUTEx(int LayerIndex, const LCD_PHYSPALETTE * pPalette);
void LCD_SetLUT  (const LCD_PHYSPALETTE * pPalette);

LCD_DRAWMODE LCD_SetDrawMode  (LCD_DRAWMODE dm);
void LCD_SetColorIndex(unsigned PixelIndex);
void LCD_SetBkColorIndex(unsigned PixelIndex);
void LCD_FillRect(int x0, int y0, int x1, int y1);
typedef void tLCD_SetPixelAA(int x, int y, U8 Intens);

void LCD_SetPixelAA4_Trans  (int x, int y, U8 Intens);
void LCD_SetPixelAA4_NoTrans(int x, int y, U8 Intens);

void LCD_SetPixelAA8_Trans  (int x, int y, U8 Intens);
void LCD_SetPixelAA8_NoTrans(int x, int y, U8 Intens);

LCD_COLOR    LCD_AA_MixColors16 (LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
LCD_COLOR    LCD_AA_MixColors256(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
LCD_COLOR    LCD_MixColors256   (LCD_COLOR Color, LCD_COLOR BkColor, unsigned Intens);
LCD_COLOR    LCD_GetPixelColor(int x, int y);     /* Get RGB color of pixel */
unsigned int LCD_GetPixelIndex(int x, int y);
int          LCD_GetBkColorIndex (void);
int          LCD_GetColorIndex (void);
#if (GUI_USE_ARGB)
U32          LCD_AA_SetOrMask(U32 OrMask);
#else
U32          LCD_AA_SetAndMask(U32 AndMask);
#endif

/* Configuration */
int LCD_SetMaxNumColors(unsigned MaxNumColors);

/*********************************************************************
*
*       Optional support for rotation
*/
#if GUI_SUPPORT_ROTATION

typedef void tLCD_DrawBitmap(int x0, int y0, int xsize, int ysize,
                             int xMul, int yMul, int BitsPerPixel, int BytesPerLine,
                             const U8 * pPixel, const void * pTrans);
typedef void tRect2TextRect (LCD_RECT * pRect);

struct tLCD_APIList_struct {
  tLCD_DrawBitmap   * pfDrawBitmap;
  tRect2TextRect    * pfRect2TextRect;
  tRect2TextRect    * pfTransformRect;
};

typedef struct tLCD_APIList_struct tLCD_APIList;

extern tLCD_APIList LCD_APIListCCW;
extern tLCD_APIList LCD_APIListCW;
extern tLCD_APIList LCD_APIList180;

#define GUI_ROTATION   tLCD_APIList
#define GUI_ROTATE_CCW &LCD_APIListCCW
#define GUI_ROTATE_CW  &LCD_APIListCW
#define GUI_ROTATE_180 &LCD_APIList180
#define GUI_ROTATE_0   0

tLCD_SetPixelAA * LCD__GetPfSetPixel(int BitsPerPixel);

#endif

/*********************************************************************
*
*       Physical color access, internally used only
*/
void LCD__SetPhysColor(U8 Pos, LCD_COLOR Color);

/*********************************************************************
*
*       Cache control
*/
#define LCD_CC_UNLOCK (0)    /* Default mode: Cache is transparent */
#define LCD_CC_LOCK   (1)    /* Cache is locked, no write operations */
#define LCD_CC_FLUSH  (2)    /* Flush cache, do not change mode */

int LCD_ControlCache  (int Cmd);
int LCD_ControlCacheEx(int LayerIndex, int Cmd);

/*********************************************************************
*
*       Color conversion
*/
unsigned         LCD_Color2Index     (LCD_COLOR Color);
LCD_COLOR        LCD_Index2Color     (int Index);
LCD_COLOR        LCD_Index2ColorEx   (int i, unsigned LayerIndex);

/*********************************************************************
*
*       LCD_X_...
*/
unsigned char LCD_X_Read00(void);
unsigned char LCD_X_Read01(void);
void LCD_X_Write00 (unsigned char c);
void LCD_X_Write01 (unsigned char c);
void LCD_X_WriteM01(unsigned char * pData, int NumBytes);

#if defined(__cplusplus)
  }
#endif

#endif /* LCD_H */

/*************************** End of file ****************************/

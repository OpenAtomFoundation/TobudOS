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
File        : GUI_Private.h
Purpose     : GUI internal declarations
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
  
#ifndef GUI_PRIVATE_H
#define GUI_PRIVATE_H

#include "GUI.h"
#include "LCD_Protected.h"
#include "GUI_Debug.h"
#if GUI_WINSUPPORT
  #include "WM_GUI.h"
#endif

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defaults for config switches
*
**********************************************************************

  The config switches below do not affect the interface in GUI.h and
  are therefor not required to be in GUI.h.
*/

/* Short address area.
   For  most compilers, this is "near" or "__near"
   We do not use this except for some CPUs which we know to always have some
   near memory, because the GUI_Context and some other data will be declared
   to be in this short address (near) memory area as it has a major effect
   on performance.
   Please define in GUIConf.h (if you want to use it)
*/
#ifndef GUI_SADDR
  #define GUI_SADDR
#endif

#ifndef GUI_DEFAULT_FONT
  #define GUI_DEFAULT_FONT    &GUI_Font6x8
#endif

#ifndef GUI_DEFAULT_CURSOR
  #define GUI_DEFAULT_CURSOR  &GUI_CursorArrowM
#endif

#ifndef GUI_DEFAULT_BKCOLOR
  #define GUI_DEFAULT_BKCOLOR GUI_BLACK
#endif

#ifndef GUI_DEFAULT_COLOR
  #define GUI_DEFAULT_COLOR   GUI_WHITE
#endif

/*********************************************************************
*
*       Angles
*
**********************************************************************
*/
#define GUI_45DEG  512
#define GUI_90DEG  (2 * GUI_45DEG)
#define GUI_180DEG (4 * GUI_45DEG)
#define GUI_360DEG (8 * GUI_45DEG)


/*********************************************************************
*
*       Locking checks
*
**********************************************************************
*/
#if defined (WIN32) && defined (_DEBUG) && GUI_OS
  #define GUI_ASSERT_LOCK()    GUITASK_AssertLock()
  #define GUI_ASSERT_NO_LOCK() GUITASK_AssertNoLock()
  void GUITASK_AssertLock(void);
  void GUITASK_AssertNoLock(void);
#else
  #define GUI_ASSERT_LOCK()
  #define GUI_ASSERT_NO_LOCK()
#endif

/*********************************************************************
*
*       Division tables
*
**********************************************************************
*/
extern const U8 GUI__aConvert_15_255[(1 << 4)];
extern const U8 GUI__aConvert_31_255[(1 << 5)];
extern const U8 GUI__aConvert_63_255[(1 << 6)];
extern const U8 GUI__aConvert_255_15[(1 << 8)];
extern const U8 GUI__aConvert_255_31[(1 << 8)];
extern const U8 GUI__aConvert_255_63[(1 << 8)];

/*********************************************************************
*
*       Usage internals
*
**********************************************************************
*/
typedef GUI_HMEM GUI_USAGE_Handle;
typedef struct tsUSAGE_APIList tUSAGE_APIList;
typedef struct GUI_Usage GUI_USAGE;
#define GUI_USAGE_h GUI_USAGE_Handle



typedef GUI_USAGE_h tUSAGE_CreateCompatible(GUI_USAGE * p);
typedef void        tUSAGE_AddPixel        (GUI_USAGE * p, int x, int y);
typedef void        tUSAGE_AddHLine        (GUI_USAGE * p, int x0, int y0, int len);
typedef void        tUSAGE_Clear           (GUI_USAGE * p);
typedef void        tUSAGE_Delete          (GUI_USAGE_h h);
typedef int         tUSAGE_GetNextDirty    (GUI_USAGE * p, int * pxOff, int yOff);
#define GUI_USAGE_LOCK_H(h) ((GUI_USAGE *)GUI_LOCK_H(h))


void GUI_USAGE_DecUseCnt(GUI_USAGE_Handle  hUsage);

GUI_USAGE_Handle GUI_USAGE_BM_Create(int x0, int y0, int xsize, int ysize, int Flags);
void    GUI_USAGE_Select(GUI_USAGE_Handle hUsage);
void    GUI_USAGE_AddRect(GUI_USAGE * pUsage, int x0, int y0, int xSize, int ySize);
#define GUI_USAGE_AddPixel(p, x,y)            p->pAPI->pfAddPixel(p,x,y)
#define GUI_USAGE_AddHLine(p,x,y,len)         p->pAPI->pfAddHLine(p,x,y,len)
#define GUI_USAGE_Clear(p)                    p->pAPI->pfClear(p)
#define GUI_USAGE_Delete(p)                   p->pAPI->pfDelete(p)
#define GUI_USAGE_GetNextDirty(p,pxOff, yOff) p->pAPI->pfGetNextDirty(p,pxOff, yOff)

struct tsUSAGE_APIList {
  tUSAGE_AddPixel         * pfAddPixel;
  tUSAGE_AddHLine         * pfAddHLine;
  tUSAGE_Clear            * pfClear;
  tUSAGE_CreateCompatible * pfCreateCompatible;
  tUSAGE_Delete           * pfDelete;
  tUSAGE_GetNextDirty     * pfGetNextDirty;
} ;

struct GUI_Usage {
  I16P x0, y0, XSize, YSize;
  const tUSAGE_APIList * pAPI;
  I16 UseCnt;
};


/*********************************************************************
*
*       GUI_MEMDEV
*
**********************************************************************
*/
#if GUI_SUPPORT_MEMDEV

typedef struct {
  GUI_DEVICE * pDevice;
  I16P                   x0, y0, XSize, YSize;
  unsigned               BytesPerLine;
  unsigned               BitsPerPixel;
  GUI_HMEM               hUsage;
} GUI_MEMDEV;

#define      GUI_MEMDEV_LOCK_H(h) ((GUI_MEMDEV *)GUI_LOCK_H(h))

void         GUI_MEMDEV__CopyFromLCD (GUI_MEMDEV_Handle hMem);
void         GUI_MEMDEV__GetRect     (GUI_RECT * pRect);
unsigned     GUI_MEMDEV__Color2Index (LCD_COLOR Color);
LCD_COLOR    GUI_MEMDEV__Index2Color (int Index);
unsigned int GUI_MEMDEV__GetIndexMask(void);
void         GUI_MEMDEV__SetAlphaCallback(unsigned(* pcbSetAlpha)(U8));

GUI_MEMDEV_Handle GUI_MEMDEV__CreateFixed(int x0, int y0, int xSize, int ySize, int Flags,
                                          const GUI_DEVICE_API     * pDeviceAPI,
                                          const LCD_API_COLOR_CONV * pColorConvAPI);

void              GUI_MEMDEV__DrawSizedAt        (GUI_MEMDEV_Handle hMem, int xPos, int yPos, int xSize, int ySize);
GUI_MEMDEV_Handle GUI_MEMDEV__GetEmptyCopy32     (GUI_MEMDEV_Handle hMem, int * pxSize, int * pySize, int * pxPos, int * pyPos);
void              GUI_MEMDEV__ReadLine           (int x0, int y, int x1, LCD_PIXELINDEX * pBuffer);
void              GUI_MEMDEV__WriteToActiveAlpha (GUI_MEMDEV_Handle hMem,int x, int y);
void              GUI_MEMDEV__WriteToActiveAt    (GUI_MEMDEV_Handle hMem,int x, int y);
void              GUI_MEMDEV__WriteToActiveOpaque(GUI_MEMDEV_Handle hMem,int x, int y);
void            * GUI_MEMDEV__XY2PTR             (int x,int y);
void            * GUI_MEMDEV__XY2PTREx           (GUI_MEMDEV * pDev, int x,int y);
void              GUI_MEMDEV__BlendColor32       (GUI_MEMDEV_Handle hMem, U32 BlendColor, U8 BlendIntens);

unsigned GUI__AlphaPreserveTrans(int OnOff);

extern unsigned GUI_MEMDEV__TimePerFrame;

#define GUI_TIME_PER_FRAME (GUI_TIMER_TIME)GUI_MEMDEV__TimePerFrame

#define GUI_POS_AUTO -4095   /* Position value for auto-pos */

#endif


/*********************************************************************
*
*       LCD_HL_ level defines
*
**********************************************************************
*/
#define LCD_HL_DrawHLine             GUI_pContext->pLCD_HL->pfDrawHLine
#define LCD_HL_DrawPixel             GUI_pContext->pLCD_HL->pfDrawPixel


/*********************************************************************
*
*       Helper functions
*
**********************************************************************
*/
#define GUI_ZEROINIT(Obj) GUI_MEMSET(Obj, 0, sizeof(Obj))
int  GUI_cos(int angle);
int  GUI_sin(int angle);
extern const U32 GUI_Pow10[10];

/* Multi-touch */
void GUI_MTOUCH__ManagePID(int OnOff);

/* Anti-aliased drawing */
int  GUI_AA_Init       (int x0, int x1);
int  GUI_AA_Init_HiRes (int x0, int x1);
void GUI_AA_Exit       (void);
I16  GUI_AA_HiRes2Pixel(int HiRes);

void GL_FillCircleAA_HiRes (int x0, int y0, int r);
void GL_FillEllipseAA_HiRes(int x0, int y0, int rx, int ry);

void GUI_AA__DrawCharAA2(int x0, int y0, int XSize, int YSize, int BytesPerLine, const U8 * pData);
void GUI_AA__DrawCharAA4(int x0, int y0, int XSize, int YSize, int BytesPerLine, const U8 * pData);
void GUI_AA__DrawCharAA8(int x0, int y0, int XSize, int YSize, int BytesPerLine, const U8 * pData);

/* Alpha blending helper functions */
#define GUI_ALPHABLENDING_DONE  (1 << 0)

int      GUI__GetAlphaBuffer    (U32 ** ppCurrent, U32 ** ppConvert, U32 ** ppData, int * pVXSizeMax);
int      GUI__AllocAlphaBuffer  (int AllocDataBuffer);
U32    * GUI__DoAlphaBlending   (int x, int y, U32 * pData, int xSize, tLCDDEV_Index2Color * pfIndex2Color_DEV, int * pDone);
unsigned GUI__SetAlphaBufferSize(int xSize);

/* System independent font routines */
int        GUI_SIF__GetCharDistX       (U16P c, int * pSizeX);
void       GUI_SIF__GetFontInfo        (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
char       GUI_SIF__IsInFont           (const GUI_FONT * pFont, U16 c);
const U8 * GUI_SIF__GetpCharInfo       (const GUI_FONT * pFont, U16P c, unsigned SizeOfCharInfo);
int        GUI_SIF__GetNumCharAreas    (const GUI_FONT * pFont);
int        GUI_SIF__GetCharDistX_ExtFrm(U16P c, int * pSizeX);
void       GUI_SIF__GetFontInfo_ExtFrm (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
char       GUI_SIF__IsInFont_ExtFrm    (const GUI_FONT * pFont, U16 c);
int        GUI_SIF__GetCharInfo_ExtFrm (U16P c, GUI_CHARINFO_EXT * pInfo);
void       GUI_SIF__ClearLine_ExtFrm   (const char * s, int Len);

/* External binary font routines */
int        GUI_XBF__GetOff       (const GUI_XBF_DATA * pXBF_Data, unsigned c, U32 * pOff);
int        GUI_XBF__GetOffAndSize(const GUI_XBF_DATA * pXBF_Data, unsigned c, U32 * pOff, U16 * pSize);
int        GUI_XBF__GetCharDistX (U16P c, int * pSizeX);
void       GUI_XBF__GetFontInfo  (const GUI_FONT * pFont, GUI_FONTINFO * pInfo);
char       GUI_XBF__IsInFont     (const GUI_FONT * pFont, U16 c);
int        GUI_XBF__GetCharInfo  (U16P c, GUI_CHARINFO_EXT * pInfo);
void       GUI_XBF__ClearLine    (const char * s, int Len);

/* Conversion routines */
void GUI_AddHex     (U32 v, U8 Len, char ** ps);
void GUI_AddBin     (U32 v, U8 Len, char ** ps);
void GUI_AddDecMin  (I32 v, char ** ps);
void GUI_AddDec     (I32 v, U8 Len, char ** ps);
void GUI_AddDecShift(I32 v, U8 Len, U8 Shift, char ** ps);
long GUI_AddSign    (long v, char ** ps);
int  GUI_Long2Len   (I32 v);

#define GUI_UC__GetCharSize(sText)  GUI_pUC_API->pfGetCharSize(sText)
#define GUI_UC__GetCharCode(sText)  GUI_pUC_API->pfGetCharCode(sText)

int   GUI_UC__CalcSizeOfChar   (U16 Char);
U16   GUI_UC__GetCharCodeInc   (const char ** ps);
int   GUI_UC__NumChars2NumBytes(const char * s, int NumChars);
int   GUI_UC__NumBytes2NumChars(const char * s, int NumBytes);

int  GUI__GetLineNumChars  (const char * s, int MaxNumChars);
int  GUI__GetNumChars      (const char * s);
int  GUI__GetOverlap       (U16 Char);
int  GUI__GetLineDistX     (const char * s, int Len);
int  GUI__GetFontSizeY     (void);
int  GUI__HandleEOLine     (const char ** ps);
void GUI__DispLine         (const char * s, int Len, const GUI_RECT * pr);
void GUI__AddSpaceHex      (U32 v, U8 Len, char ** ps);
void GUI__CalcTextRect     (const char * pText, const GUI_RECT * pTextRectIn, GUI_RECT * pTextRectOut, int TextAlign);

void GUI__ClearTextBackground(int xDist, int yDist);

int  GUI__WrapGetNumCharsDisp       (const char * pText, int xSize, GUI_WRAPMODE WrapMode);
int  GUI__WrapGetNumCharsToNextLine (const char * pText, int xSize, GUI_WRAPMODE WrapMode);
int  GUI__WrapGetNumBytesToNextLine (const char * pText, int xSize, GUI_WRAPMODE WrapMode);
void GUI__memset    (U8  * p, U8 Fill, int NumBytes);
void GUI__memset16  (U16 * p, U16 Fill, int NumWords);
int  GUI__strlen    (const char * s);
int  GUI__strcmp    (const char * s0, const char * s1);
int  GUI__strcmp_hp (GUI_HMEM hs0, const char * s1);

/* Get cursor position */
int  GUI__GetCursorPosX     (const char * s, int Index, int MaxNumChars);
int  GUI__GetCursorPosChar  (const char * s, int x, int NumCharsToNextLine);
U16  GUI__GetCursorCharacter(const char * s, int Index, int MaxNumChars, int * pIsRTL);

/* Arabic support (tbd) */
U16  GUI__GetPresentationForm     (U16 Char, U16 Next, U16 Prev, int * pIgnoreNext, const char * s);
int  GUI__IsArabicCharacter       (U16 c);

/* BiDi support */
int  GUI__BIDI_Log2Vis           (const char * s, int NumChars, char * pBuffer, int BufferSize);
int  GUI__BIDI_GetCursorPosX     (const char * s, int NumChars, int Index);
int  GUI__BIDI_GetCursorPosChar  (const char * s, int NumChars, int x);
U16  GUI__BIDI_GetLogChar        (const char * s, int NumChars, int Index);
int  GUI__BIDI_GetCharDir        (const char * s, int NumChars, int Index);
int  GUI__BIDI_IsNSM             (U16 Char);
U16  GUI__BIDI_GetCursorCharacter(const char * s, int Index, int MaxNumChars, int * pIsRTL);
int  GUI__BIDI_GetWordWrap       (const char * s, int xSize, int * pxDist);
int  GUI__BIDI_GetCharWrap       (const char * s, int xSize);

const char * GUI__BIDI_Log2VisBuffered(const char * s, int * pMaxNumChars);

extern int GUI__BIDI_Enabled;

extern int (* _pfGUI__BIDI_Log2Vis         )(const char * s, int NumChars, char * pBuffer, int BufferSize);
extern int (* _pfGUI__BIDI_GetCursorPosX   )(const char * s, int NumChars, int Index);
extern int (* _pfGUI__BIDI_GetCursorPosChar)(const char * s, int NumChars, int x);
extern U16 (* _pfGUI__BIDI_GetLogChar      )(const char * s, int NumChars, int Index);
extern int (* _pfGUI__BIDI_GetCharDir      )(const char * s, int NumChars, int Index);
extern int (* _pfGUI__BIDI_IsNSM           )(U16 Char);

/* BiDi-related function pointers */
extern const char * (* GUI_CharLine_pfLog2Vis)(const char * s, int * pMaxNumChars);

extern int (* GUI__GetCursorPos_pfGetPosX)     (const char * s, int MaxNumChars, int Index);
extern int (* GUI__GetCursorPos_pfGetPosChar)  (const char * s, int MaxNumChars, int x);
extern U16 (* GUI__GetCursorPos_pfGetCharacter)(const char * s, int MaxNumChars, int Index, int * pIsRTL);

extern int (* GUI__Wrap_pfGetWordWrap)(const char * s, int xSize, int * pxDist);
extern int (* GUI__Wrap_pfGetCharWrap)(const char * s, int xSize);

/* Proportional  font support */
const GUI_FONT_PROP * GUIPROP__FindChar(const GUI_FONT_PROP * pProp, U16P c);

/* Extended proportional font support */
const GUI_FONT_PROP_EXT * GUIPROP_EXT__FindChar(const GUI_FONT_PROP_EXT * pPropExt, U16P c);
void  GUIPROP_EXT__DispLine      (const char * s, int Len);
void  GUIPROP_EXT__ClearLine     (const char * s, int Len);
void  GUIPROP_EXT__SetfpClearLine(void (* fpClearLine)(const char * s, int Len));

/* Reading data routines */
U16 GUI__Read16(const U8 ** ppData);
U32 GUI__Read32(const U8 ** ppData);

/* Virtual screen support */
void GUI__GetOrg(int * px, int * py);
void GUI__SetOrgHook(void(* pfHook)(int x, int y));

/* Timer support */
int              GUI_TIMER__IsActive       (void);
GUI_TIMER_TIME   GUI_TIMER__GetPeriod      (void);
GUI_TIMER_HANDLE GUI_TIMER__GetNextTimer   (GUI_TIMER_HANDLE hTimer, U32 * pContext);
GUI_TIMER_HANDLE GUI_TIMER__GetFirstTimer  (U32 * pContext);
GUI_TIMER_HANDLE GUI_TIMER__GetNextTimerLin(GUI_TIMER_HANDLE hTimer, U32 * pContext);

/* Get function pointers for color conversion */
tLCDDEV_Index2Color * GUI_GetpfIndex2ColorEx(int LayerIndex);
tLCDDEV_Color2Index * GUI_GetpfColor2IndexEx(int LayerIndex);

int GUI_GetBitsPerPixelEx(int LayerIndex);

LCD_PIXELINDEX * LCD_GetpPalConvTable        (const LCD_LOGPALETTE * pLogPal);
LCD_PIXELINDEX * LCD_GetpPalConvTableUncached(const LCD_LOGPALETTE * pLogPal);
LCD_PIXELINDEX * LCD_GetpPalConvTableBM      (const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int LayerIndex);

/* Setting a function for converting a color palette to an array of index values */
void GUI_SetFuncGetpPalConvTable(LCD_PIXELINDEX * (* pFunc)(const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int LayerIndex));

/*********************************************************************
*
*       Format definitions used by streamed bitmaps
*
*   IMPORTANT: DO NOT CHANGE THESE VALUES!
*   THEY HAVE TO CORRESPOND TO THE DEFINITIONS WITHIN THE CODE OF THE BITMAPCONVERTER!
*/
#define GUI_STREAM_FORMAT_INDEXED    100 /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE4       6   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE8       7   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_565        8   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M565       9   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_555        10  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M555       11  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE16      12  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLEM16     13  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_8888       16  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE32      15  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_24         17  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLEALPHA   18  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_444_12     19  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M444_12    20  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_444_12_1   21  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M444_12_1  22  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_444_16     23  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M444_16    24  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_A555       25  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_AM555      26  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_A565       27  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_AM565      28  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M8888I     29  /* DO NOT CHANGE */


void GUI__ReadHeaderFromStream  (GUI_BITMAP_STREAM * pBitmapHeader, const U8 * pData);
void GUI__CreateBitmapFromStream(const GUI_BITMAP_STREAM * pBitmapHeader, const void * pData, GUI_BITMAP * pBMP, GUI_LOGPALETTE * pPAL, const GUI_BITMAP_METHODS * pMethods);

/* Cache management */
int GUI__ManageCache  (int Cmd);
int GUI__ManageCacheEx(int LayerIndex, int Cmd);

/*********************************************************************
*
*       2d - GL
*
**********************************************************************
*/
void GL_DispChar         (U16 c);
void GL_DrawArc          (int x0, int y0, int rx, int ry, int a0, int a1);
void GL_DrawBitmap       (const GUI_BITMAP * pBM, int x0, int y0);
void GL_DrawCircle       (int x0, int y0, int r);
void GL_DrawEllipse      (int x0, int y0, int rx, int ry, int w);
void GL_DrawHLine        (int y0, int x0, int x1);
void GL_DrawPolygon      (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GL_DrawPoint        (int x,  int y);
void GL_DrawLine1        (int x0, int y0, int x1, int y1);
void GL_DrawLine1Ex      (int x0, int y0, int x1, int y1, unsigned * pPixelCnt);
void GL_DrawLineRel      (int dx, int dy);
void GL_DrawLineTo       (int x,  int y);
void GL_DrawLineToEx     (int x,  int y, unsigned * pPixelCnt);
void GL_DrawLine         (int x0, int y0, int x1, int y1);
void GL_DrawLineEx       (int x0, int y0, int x1, int y1, unsigned * pPixelCnt);
void GL_MoveTo           (int x,  int y);
void GL_FillCircle       (int x0, int y0, int r);
void GL_FillCircleAA     (int x0, int y0, int r);
void GL_FillEllipse      (int x0, int y0, int rx, int ry);
void GL_FillPolygon      (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GL_SetDefault       (void);


/*********************************************************************
*
*       Callback pointers for dynamic linkage
*
**********************************************************************
Dynamic linkage pointers reduces configuration hassles.
*/
typedef int  GUI_tfTimer(void);
typedef int  WM_tfHandlePID(void);


/*********************************************************************
*
*       Cursors
*
**********************************************************************
*/
extern GUI_CONST_STORAGE unsigned char  GUI_Pixels_ArrowS[45];
extern GUI_CONST_STORAGE unsigned char  GUI_Pixels_ArrowM[60];
extern GUI_CONST_STORAGE unsigned char  GUI_Pixels_ArrowL[150];
extern GUI_CONST_STORAGE unsigned char  GUI_Pixels_CrossS[33];
extern GUI_CONST_STORAGE unsigned char  GUI_Pixels_CrossM[126];
extern GUI_CONST_STORAGE unsigned char  GUI_Pixels_CrossL[248];
extern GUI_CONST_STORAGE unsigned char  GUI_PixelsHeaderM[5 * 17];

extern GUI_CONST_STORAGE GUI_LOGPALETTE GUI_CursorPal;
extern GUI_CONST_STORAGE GUI_LOGPALETTE GUI_CursorPalI;


/*********************************************************************
*
*       Text rotation
*
**********************************************************************
*/
extern GUI_RECT  GUI_RectDispString; /* Used by LCD_Rotate...() and GUI_DispStringInRect() */

/*********************************************************************
*
*       Flag for setting transparency for 'EXT' fonts
*
**********************************************************************
*/
extern U8 GUI__CharHasTrans;

/*********************************************************************
*
*       Multitasking support
*
**********************************************************************
*/
extern int GUITASK__EntranceCnt;

/*********************************************************************
*
*       Bitmap related functions
*
**********************************************************************
*/

int       GUI_GetBitmapPixelIndex(const GUI_BITMAP * pBMP, unsigned x, unsigned y);
GUI_COLOR GUI_GetBitmapPixelColor(const GUI_BITMAP * pBMP, unsigned x, unsigned y);
int       GUI_GetBitmapPixelIndexEx(int BitsPerPixel, int BytesPerLine, const U8 * pData, unsigned x, unsigned y);

void      GUI__DrawBitmap16bpp (int x0, int y0, int xsize, int ysize, const U8 * pPixel, const LCD_LOGPALETTE * pLogPal, int xMag, int yMag, tLCDDEV_Index2Color * pfIndex2Color, const LCD_API_COLOR_CONV * pColorConvAPI);
void      GUI__DrawBitmapA16bpp(int x0, int y0, int xSize, int ySize, const U8 * pPixel, const LCD_LOGPALETTE * pLogPal, int xMag, int yMag, tLCDDEV_Index2Color * pfIndex2Color);
void      GUI__SetPixelAlpha   (int x, int y, U8 Alpha, LCD_COLOR Color);
LCD_COLOR GUI__MixColors       (LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
void      GUI__MixColorsBulk   (U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens);

extern const GUI_UC_ENC_APILIST GUI_UC_None;

/*********************************************************************
*
*       LCDDEV_L0_xxx
*
**********************************************************************
*/
#define LCDDEV_L0_Color2Index         GUI__apDevice[GUI_pContext->SelLayer]->pColorConvAPI->pfColor2Index
#define LCDDEV_L0_Index2Color         GUI__apDevice[GUI_pContext->SelLayer]->pColorConvAPI->pfIndex2Color

#define LCDDEV_L0_DrawBitmap          GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawBitmap
#define LCDDEV_L0_DrawHLine           GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawHLine
#define LCDDEV_L0_DrawVLine           GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawVLine
#define LCDDEV_L0_DrawPixel           GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawPixel
#define LCDDEV_L0_FillRect            GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfFillRect
#define LCDDEV_L0_GetPixel            GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetPixel
#define LCDDEV_L0_GetRect             GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetRect
#define LCDDEV_L0_GetPixelIndex       GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetPixelIndex
#define LCDDEV_L0_SetPixelIndex       GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfSetPixelIndex
#define LCDDEV_L0_XorPixel            GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfXorPixel
#define LCDDEV_L0_GetDevFunc          GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetDevFunc

void LCD_ReadRect  (int x0, int y0, int x1, int y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);
void GUI_ReadRect  (int x0, int y0, int x1, int y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);
void GUI_ReadRectEx(int x0, int y0, int x1, int y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);

void LCD_ReadRectNoClip(int x0, int y0, int x1, int y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);

/*********************************************************************
*
*       Internal color management
*
**********************************************************************
*/
typedef struct {
  void         (* pfSetColor)   (LCD_COLOR Index);
  void         (* pfSetBkColor) (LCD_COLOR Index);
  LCD_DRAWMODE (* pfSetDrawMode)(LCD_DRAWMODE dm);
} LCD_SET_COLOR_API;

extern const LCD_SET_COLOR_API * LCD__pSetColorAPI;

#define LCD__SetBkColorIndex(Index) (*GUI_pContext->LCD_pBkColorIndex = Index)
#define LCD__SetColorIndex(Index)   (*GUI_pContext->LCD_pColorIndex   = Index)
#define LCD__GetBkColorIndex()      (*GUI_pContext->LCD_pBkColorIndex)
#define LCD__GetColorIndex()        (*GUI_pContext->LCD_pColorIndex)

/* The following 2 defines are only required for compatibility to older versions of the TTF library */
#define LCD_BKCOLORINDEX (*GUI_pContext->LCD_pBkColorIndex)
#define LCD_COLORINDEX   (*GUI_pContext->LCD_pColorIndex)

/*********************************************************************
*
*       EXTERNs for GL_CORE
*
**********************************************************************
*/
extern const GUI_FONT * GUI__pFontDefault;

extern GUI_SADDR GUI_CONTEXT * GUI_pContext;

extern GUI_DEVICE * GUI__apDevice[GUI_NUM_LAYERS];

//
// Function pointer for converting a palette containing a color array into an index array
//
extern LCD_PIXELINDEX * (* GUI_pfGetpPalConvTable)(const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int LayerIndex);

//
// Function pointer for mixing up 2 colors
//
extern LCD_COLOR (* GUI__pfMixColors)(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);

//
// Function pointer for mixing up arrays of colors
//
extern void (* GUI__pfMixColorsBulk)(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens);

//
// API list to be used for MultiBuffering
//
extern const GUI_MULTIBUF_API    GUI_MULTIBUF_APIList;
extern const GUI_MULTIBUF_API_EX GUI_MULTIBUF_APIListEx;

#ifdef  GL_CORE_C
  #define GUI_EXTERN
#else
  #define GUI_EXTERN extern
#endif

GUI_EXTERN   int  (* GUI_pfUpdateSoftLayer)(void);

#ifdef WIN32
  GUI_EXTERN void (* GUI_pfSoftlayerGetPixel)(int x, int y, void * p);
#endif

GUI_EXTERN void (* GUI_pfHookMTOUCH)(const GUI_MTOUCH_STATE * pState);

GUI_EXTERN const GUI_UC_ENC_APILIST * GUI_pUC_API; /* Unicode encoding API */

GUI_EXTERN GUI_SADDR char             GUI_DecChar;
GUI_EXTERN           GUI_tfTimer    * GUI_pfTimerExec;
GUI_EXTERN           WM_tfHandlePID * WM_pfHandlePID;
GUI_EXTERN   void (* GUI_pfDispCharStyle)(U16 Char);

GUI_EXTERN           int GUI__BufferSize; // Required buffer size in pixels for alpha blending and/or antialiasing
GUI_EXTERN           int GUI_AA__ClipX0;  // x0-clipping value for AA module

GUI_EXTERN           I8  GUI__aNumBuffers[GUI_NUM_LAYERS]; // Number of buffers used per layer
GUI_EXTERN           U8  GUI__PreserveTrans;
GUI_EXTERN           U8  GUI__IsInitialized;

#if GUI_SUPPORT_ROTATION
  GUI_EXTERN const tLCD_APIList * GUI_pLCD_APIList; /* Used for rotating text */
#endif

GUI_EXTERN I16 GUI_OrgX, GUI_OrgY;

#undef GUI_EXTERN

#if defined(__cplusplus)
}
#endif 

#endif /* GUI_PRIVATE_H */

/*************************** End of file ****************************/

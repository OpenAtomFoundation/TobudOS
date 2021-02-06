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
File        : GUI.h
Purpose     : GUI API include file
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
  
#ifndef  GUI_H
#define  GUI_H

#include "GUI_ConfDefaults.h"
#include "GUI_Type.h"
#include "GUI_Version.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Disable string function warning with newer MSVC versions
*/
#if defined (_MSC_VER)
  #if (_MSC_VER > 1200)
    #pragma warning( disable : 4996)
  #endif
#endif

/*********************************************************************
*
*       Macros, function replacement
*/
#define GUI_COUNTOF(a)          (sizeof(a) / sizeof(a[0]))
#define GUI_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define GUI_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define GUI_ZEROFILL(p, Size)   (memset(p, 0, Size))

/*********************************************************************
*
*       Support for multitasking systems (locking)
*/
typedef struct GUI_CONTEXT GUI_CONTEXT;

#if !GUI_OS
  #define GUI_LOCK()
  #define GUI_UNLOCK()
  #define GUITASK_INIT()
  #define GUITASK_COPY_CONTEXT()
#else
  void GUI_Lock(void);
  void GUI_Unlock(void);
  void GUITASK_Init(void);
  void GUITASK_CopyContext(void);
  void GUITASK_SetMaxTask(int MaxTask);
  int  GUITASK_GetMaxTask(void);
  GUI_CONTEXT * GUITASK_GetpContext(int Index);
  #define GUI_LOCK()             GUI_Lock()
  #define GUI_UNLOCK()           GUI_Unlock()
  #define GUITASK_INIT()         GUITASK_Init()
  #define GUITASK_COPY_CONTEXT() GUITASK_CopyContext()
#endif

/*********************************************************************
*
*       API table of a display driver
*/
struct GUI_DEVICE_API {
  //
  // Data
  //
  int DeviceClassIndex;
  //
  // Drawing functions
  //
  void     (* pfDrawBitmap   )(GUI_DEVICE *  pDevice,  int x0, int y0, int xsize, int ysize, int BitsPerPixel, int BytesPerLine, const U8 * pData, int Diff, const LCD_PIXELINDEX * pTrans);
  void     (* pfDrawHLine    )(GUI_DEVICE *  pDevice,  int x0, int y0,  int x1);
  void     (* pfDrawVLine    )(GUI_DEVICE *  pDevice,  int x , int y0,  int y1);
  void     (* pfFillRect     )(GUI_DEVICE *  pDevice,  int x0, int y0, int x1, int y1);
  unsigned (* pfGetPixelIndex)(GUI_DEVICE *  pDevice,  int x, int y);
  void     (* pfSetPixelIndex)(GUI_DEVICE *  pDevice,  int x, int y, int ColorIndex);
  void     (* pfXorPixel     )(GUI_DEVICE *  pDevice,  int x, int y);
  //
  // Set origin
  //
  void     (* pfSetOrg       )(GUI_DEVICE *  pDevice,  int x, int y);
  //
  // Request information
  //
  void   (*(* pfGetDevFunc)   (GUI_DEVICE ** ppDevice, int Index))(void);
  I32      (* pfGetDevProp   )(GUI_DEVICE *  pDevice,  int Index);
  void    *(* pfGetDevData   )(GUI_DEVICE *  pDevice,  int Index);
  void     (* pfGetRect      )(GUI_DEVICE *  pDevice,  LCD_RECT * pRect);
};

/*********************************************************************
*
*       Device classes
*/
typedef enum {
  DEVICE_CLASS_DRIVER = 0,
  DEVICE_CLASS_DRIVER_MODIFIER,   // Zoom or delta-pixel modifier
  DEVICE_CLASS_VNC,
  DEVICE_CLASS_SPRITE,
  DEVICE_CLASS_MEMDEV,
  DEVICE_CLASS_ALPHA,
  DEVICE_CLASS_AUTOALPHA,
  DEVICE_CLASS_MEASDEV
} DEVICE_CLASS;

#define GUI_DEVICE_STAYONTOP 1

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_Template_API;

//
// Macros to be used in configuration files
//
#define GUIDRV_WIN32       &GUIDRV_Win_API

#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_TEMPLATE         &GUIDRV_Win_API

#else

  #define GUIDRV_TEMPLATE         &GUIDRV_Template_API

#endif

/*********************************************************************
*
*       Definition of GUI_DEVICE structure
*/
struct GUI_DEVICE {
  //
  // Linking
  //
  GUI_DEVICE * pNext;
  GUI_DEVICE * pPrev;
  //
  // Data
  //
  union {
    GUI_HMEM hContext; // Handle of payload data like sprite- or memory device context
    void   * pContext; // Pointer for context data in a fixed block
  } u;
  //
  // API pointers
  //
  const GUI_DEVICE_API     * pDeviceAPI;
  const LCD_API_COLOR_CONV * pColorConvAPI;
  U16 Flags;
  int LayerIndex;
};

extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_1;
extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_8;
extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_16;
extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_32;

/*********************************************************************
*
*       GUI_CONTEXT
*
*  This structure is public for one reason only:
*  To allow the application to save and restore the context.
*/
typedef union {
  U8  aColorIndex8[2];
  U16 aColorIndex16[2];
  U32 aColorIndex32[2];
} LCD_COLORINDEX_UNION;

struct GUI_CONTEXT {
  //
  // Variables in LCD module
  //
  LCD_COLORINDEX_UNION uLCD;
  LCD_RECT       ClipRect;
  U8             DrawMode;
  U8             SelLayer;
  U8             TextStyle;
  //
  // Variables in GL module
  //
  GUI_RECT * pClipRect_HL;                 // High level clip rectangle ... Speed optimization so drawing routines can optimize
  U8         PenSize;
  U8         PenShape;
  U8         LineStyle;
  //
  // Variables in GUICHAR module
  //
  const GUI_FONT * pAFont;
  I16P LBorder;
  I16P DispPosX, DispPosY;
  I16P DrawPosX, DrawPosY;
  I16P TextMode, TextAlign;
  GUI_COLOR Color, BkColor;                // Required only when changing devices and for speed opt (caching)
  //
  // Pointer to  color indices
  //
  LCD_PIXELINDEX * LCD_pBkColorIndex;
  LCD_PIXELINDEX * LCD_pColorIndex;
  LCD_PIXELINDEX * LCD_pAColorIndex;
  //
  // Variables in WM module
  //
  #if GUI_WINSUPPORT
    const GUI_RECT * WM__pUserClipRect;
    GUI_HWIN hAWin;
    int xOff, yOff;
    U8 WM_IsActive;
  #endif
  //
  // Array of pointers to device chains
  //
  //GUI_DEVICE * apDevice[GUI_NUM_LAYERS];
  GUI_DEVICE * apDriver[GUI_NUM_LAYERS];
  //
  // Variables in MEMDEV module (with memory devices only)
  //
  GUI_HMEM    hDevData;
  //
  // Variables in Anitaliasing module
  //
  const tLCD_HL_APIList * pLCD_HL;       // Required to reroute drawing (HLine & Pixel) to the AA module
  U8 AA_Factor;
  U8 AA_HiResEnable;
  void (* AA_pfSetPixelAA)(int x, int y, U8 Intens); // Function to be used for drawing a single pixel
};

/* Rename GUI_SaveContext in order to avoid crashes if wrong GUIConf is used */
#if (GUI_WINSUPPORT == 1)
  #define GUI_SaveContext GUI_SaveContext_W
#else
  #define GUI_SaveContext GUI_SaveContext_
#endif

/*********************************************************************
*
*       Device management
*/
GUI_DEVICE * GUI_DEVICE_Create       (const GUI_DEVICE_API * pDeviceAPI, const LCD_API_COLOR_CONV * pColorConvAPI, U16 Flags, int LayerIndex);
GUI_DEVICE * GUI_DEVICE_CreateAndLink(const GUI_DEVICE_API * pDeviceAPI, const LCD_API_COLOR_CONV * pColorConvAPI, U16 Flags, int LayerIndex);
void         GUI_DEVICE_Delete       (GUI_DEVICE * pDevice);
int          GUI_DEVICE_Link         (GUI_DEVICE * pDevice);
void         GUI_DEVICE_Unlink       (GUI_DEVICE * pDevice);
GUI_DEVICE * GUI_DEVICE__GetpDriver  (int LayerIndex);
GUI_DEVICE * GUI_DEVICE__GetpDevice  (int LayerIndex, int DeviceClass);

GUI_DEVICE * GUI_DEVICE_UnlinkTaskDevices(void);
void         GUI_DEVICE_LinkDevices      (GUI_DEVICE * pDevice);

void _RegisterExit(void);
/*********************************************************************
*
*       GUI_DIRTYDEVICE
*/
typedef struct {
  void * pData;         // Pointer to first pixel
  int    x0, y0;        // Coordinates of the upper left pixel
  int    xSize, ySize;  // Size of dirty rectangle
  int    LineOff;       // Virtual xSize in pixels
  int    BytesPerPixel; // Number of bytes required per pixel
  int    IsDirty;       // Indicates if dirty pixels exist
} GUI_DIRTYDEVICE_INFO;

int GUI_DIRTYDEVICE_Create      (void);
int GUI_DIRTYDEVICE_CreateEx    (int LayerIndex);
int GUI_DIRTYDEVICE_CreateExInfo(GUI_DIRTYDEVICE_INFO * pInfo, int LayerIndex);
int GUI_DIRTYDEVICE_Delete      (void);
int GUI_DIRTYDEVICE_DeleteEx    (int LayerIndex);
int GUI_DIRTYDEVICE_Fetch       (GUI_DIRTYDEVICE_INFO * pInfo);
int GUI_DIRTYDEVICE_FetchEx     (GUI_DIRTYDEVICE_INFO * pInfo, int LayerIndex);

/*********************************************************************
*
*       GUI_SOFTLAYER
*/
typedef struct {
  int xPos;
  int yPos;
  int xSize;
  int ySize;
  int Visible;
} GUI_SOFTLAYER_CONFIG;

int  GUI_SOFTLAYER_Enable           (GUI_SOFTLAYER_CONFIG * pConfig, int NumLayers, GUI_COLOR CompositeColor);
int  GUI_SOFTLAYER_Refresh          (void);
void GUI_SOFTLAYER_SetCompositeColor(U32 Color);
int  GUI_SOFTLAYER_MULTIBUF_Enable  (int OnOff);

/*********************************************************************
*
*       General routines
*/
int          GUI_Init             (void);
int          GUI_IsInitialized    (void);
void         GUI_Exit             (void);
void         GUI_SetDefaultFont   (const GUI_FONT * pFont);
void         GUI_SetDefault       (void);
GUI_DRAWMODE GUI_SetDrawMode      (GUI_DRAWMODE dm);
const char * GUI_GetVersionString (void);
void         GUI_SaveContext      (      GUI_CONTEXT * pContext);
void         GUI_RestoreContext   (const GUI_CONTEXT * pContext);
void         GUI_SetScreenSizeX   (int xSize);
void         GUI_SetScreenSizeY   (int ySize);
int          GUI_GetScreenSizeX   (void);
int          GUI_GetScreenSizeY   (void);
const GUI_RECT * GUI_SetClipRect  (const GUI_RECT * pRect);
void         GUI_SetRefreshHook   (void (* pFunc)(void));
void         GUI_SetControlHook   (void (* pFunc)(int LayerIndex, int Cmd));
void         MainTask             (void);

/*********************************************************************
*
*       Rectangle helper functions
*/
int  GUI_RectsIntersect(const GUI_RECT * pr0, const GUI_RECT * pr1);
void GUI_MoveRect       (GUI_RECT * pRect, int x, int y);
void GUI_MergeRect      (GUI_RECT * pDest, const GUI_RECT * pr0, const GUI_RECT * pr1);
int  GUI__IntersectRects(GUI_RECT * pDest, const GUI_RECT * pr0, const GUI_RECT * pr1);
void GUI__IntersectRect (GUI_RECT * pDest, const GUI_RECT * pr0);
void GUI__ReduceRect    (GUI_RECT * pDest, const GUI_RECT * pRect, int Dist);

/*********************************************************************
*
*       Misc helper functions
*/
I32  GUI__ATan2(I32 x, I32 y, I32 * ph);
I32  GUI__ASinHQ(I32 SinHQ);
int  GUI__CompactPixelIndices  (LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel);
int  GUI__CompactPixelIndicesEx(LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel, const LCD_API_COLOR_CONV * pColorConvAPI);
int  GUI__ConvertColor2Index   (LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel, const LCD_API_COLOR_CONV * pColorConvAPI, void * pResult);
void GUI__Config(void);
I32  GUI__CosHQ(I32 Ang1000);
int  GUI__DivideRound     (int a, int b);
I32  GUI__DivideRound32   (I32 a, I32 b);
void GUI__ExpandPixelIndices   (void * pBuffer, int NumPixels, int BitsPerPixel);
void GUI__ExpandPixelIndicesEx (void * pBuffer, int NumPixels, int BitsPerPixel, const LCD_API_COLOR_CONV * pColorConvAPI);
void GUI__memcpy(void * pDest, const void * pSrc, int NumBytes);
int  GUI__SetText(GUI_HMEM * phText, const char * s);
I32  GUI__SinHQ(I32 Ang1000);
I32  GUI__sqrt32(I32 Square);
void GUI__DrawTwinArc2(int xl, int xr, int y0,         int r, GUI_COLOR ColorR0, GUI_COLOR ColorR1, GUI_COLOR ColorFill);
void GUI__DrawTwinArc4(int x0, int y0, int x1, int y1, int r, GUI_COLOR ColorR0, GUI_COLOR ColorR1, GUI_COLOR ColorFill);
void GUI__FillTrippleArc(int x0, int y0, int Size, GUI_COLOR ColorR0, GUI_COLOR ColorR1, GUI_COLOR ColorR2, GUI_COLOR ColorFill);
void GUI__RegisterExit(GUI_REGISTER_EXIT * pRegisterExit);

/*********************************************************************
*
*       Get / Set Attributes
*/
GUI_COLOR GUI_GetBkColor     (void);
int       GUI_GetBkColorIndex(void);
GUI_COLOR GUI_GetColor       (void);
int       GUI_GetColorIndex  (void);
U8        GUI_GetLineStyle   (void);
U8        GUI_GetPenSize     (void);
U8        GUI_GetPenShape    (void);
unsigned  GUI_GetPixelIndex  (int x, int y);

void      GUI_SetBkColor   (GUI_COLOR);
void      GUI_SetColor     (GUI_COLOR);
void      GUI_SetBkColorIndex(int Index);
void      GUI_SetColorIndex(int Index);

U8        GUI_SetPenSize   (U8 Size);
U8        GUI_SetPenShape  (U8 Shape);
U8        GUI_SetLineStyle (U8 Style);

/* Get/Set Character used as decimal point (usually '.' or ',') */
char      GUI_GetDecChar(void);
char      GUI_SetDecChar(char c);

/*********************************************************************
*
*       Color / Index related functions
*/
int       GUI_Color2Index(GUI_COLOR color);
GUI_COLOR GUI_Color2VisColor(GUI_COLOR color);
char      GUI_ColorIsAvailable(GUI_COLOR color);
GUI_COLOR GUI_Index2Color(int Index);
U32       GUI_CalcColorDist (GUI_COLOR Color0, GUI_COLOR  Color1);
U32       GUI_CalcVisColorError(GUI_COLOR color);

/*********************************************************************
*
*       Error handler
*/
void GUI_SetOnErrorFunc(void (* pFunc)(const char * s));

/*********************************************************************
*
*       Logging (for debugging primarily)
*/
void GUI_Log      (const char * s);
void GUI_Log1     (const char * s, I32 p0);
void GUI_Log2     (const char * s, I32 p0, I32 p1);
void GUI_Log3     (const char * s, I32 p0, I32 p1, I32 p2);
void GUI_Log4     (const char * s, I32 p0, I32 p1, I32 p2,I32 p3);
void GUI_Warn     (const char * s);
void GUI_Warn1    (const char * s, I32 p0);
void GUI_Warn2    (const char * s, I32 p0, I32 p1);
void GUI_Warn3    (const char * s, I32 p0, I32 p1, I32 p2);
void GUI_Warn4    (const char * s, I32 p0, I32 p1, I32 p2, I32 p3);
void GUI_ErrorOut (const char * s);
void GUI_ErrorOut1(const char * s, I32 p0);
void GUI_ErrorOut2(const char * s, I32 p0, I32 p1);
void GUI_ErrorOut3(const char * s, I32 p0, I32 p1, I32 p2);
void GUI_ErrorOut4(const char * s, I32 p0, I32 p1, I32 p2, I32 p3);

/*********************************************************************
*
*       2d - GL
*/
void GUI_Clear            (void);
void GUI_ClearRect        (int x0, int y0, int x1, int y1);
void GUI_ClearRectEx      (const GUI_RECT * pRect);
void GUI_CopyRect         (int x0, int y0, int x1, int y1, int dx, int dy);
void GUI_DrawArc          (int x0, int y0, int rx, int ry, int a0, int a1);
void GUI_DrawBitmap       (const GUI_BITMAP * pBM, int x0, int y0);
void GUI_DrawBitmapMag    (const GUI_BITMAP * pBM, int x0, int y0, int XMul, int YMul);
void GUI_DrawBitmapEx     (const GUI_BITMAP * pBM, int x0, int y0, int xCenter, int yCenter, int xMag, int yMag);
void GUI_DrawBitmapExp    (int x0, int y0, int XSize, int YSize, int XMul,  int YMul, int BitsPerPixel, int BytesPerLine, const U8 * pData, const GUI_LOGPALETTE * pPal);
void GUI_DrawBitmapHWAlpha(const GUI_BITMAP * pBM, int x0, int y0);
void GUI_DrawCircle       (int x0, int y0, int r);
void GUI_DrawEllipse      (int x0, int y0, int rx, int ry);
void GUI_DrawGradientH    (int x0, int y0, int x1, int y1, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientV    (int x0, int y0, int x1, int y1, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientRoundedH(int x0, int y0, int x1, int y1, int rd, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientRoundedV(int x0, int y0, int x1, int y1, int rd, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGraph        (I16 * pay, int NumPoints, int x0, int y0);
void GUI_DrawGraphEx      (I16 * pay, int NumPoints, int x0, int y0, int Numerator, int Denominator, int MirrorX);
void GUI_DrawHLine        (int y0, int x0, int x1);
void GUI_DrawLine         (int x0, int y0, int x1, int y1);
void GUI_DrawLineRel      (int dx, int dy);
void GUI_DrawLineTo       (int x, int y);
void GUI_DrawPie          (int x0, int y0, int r, int a0, int a1, int Type);
void GUI_DrawPixel        (int x, int y);
void GUI_DrawPoint        (int x, int y);
void GUI_DrawPolygon      (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GUI_DrawPolyLine     (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GUI_DrawFocusRect    (const GUI_RECT  * pRect, int Dist);
void GUI_DrawRect         (int x0, int y0, int x1, int y1);
void GUI_DrawRectEx       (const GUI_RECT * pRect);
void GUI_DrawRoundedFrame (int x0, int y0, int x1, int y1, int r, int w);
void GUI_DrawRoundedRect  (int x0, int y0, int x1, int y1, int r);
void GUI_DrawVLine        (int x0, int y0, int y1);
void GUI_FillCircle       (int x0, int y0, int r);
void GUI_FillEllipse      (int x0, int y0, int rx, int ry);
void GUI_FillPolygon      (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GUI_FillRect         (int x0, int y0, int x1, int y1);
void GUI_FillRectEx       (const GUI_RECT * pRect);
void GUI_FillRoundedFrame (int x0, int y0, int x1, int y1, int r, int w);
void GUI_FillRoundedRect  (int x0, int y0, int x1, int y1, int r);
void GUI_FillRoundedRectB (int x0, int y0, int x1, int y1, int r);
void GUI_FillRoundedRectT (int x0, int y0, int x1, int y1, int r);
void GUI_GetClientRect    (GUI_RECT * pRect);
void GUI_InvertRect       (int x0, int y0, int x1, int y1);
void GUI_MoveRel          (int dx, int dy);
void GUI_MoveTo           (int x, int y);
void GUI_SetAlphaMask8888 (U32 OrMask, U32 AndMask);

/*********************************************************************
*
*       Graphic file support
*/
typedef int GUI_GET_DATA_FUNC(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);

/*********************************************************************
*
*       GIF file support
*/
int GUI_GIF_Draw           (const void * pGIF, U32 NumBytes,         int x0, int y0);
int GUI_GIF_DrawEx         (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int GUI_GIF_DrawSub        (const void * pGIF, U32 NumBytes,         int x0, int y0, int Index);
int GUI_GIF_DrawSubEx      (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Index);
int GUI_GIF_DrawSubScaled  (const void * pGIF, U32 NumBytes,         int x0, int y0, int Index, int Num, int Denom);
int GUI_GIF_DrawSubScaledEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Index, int Num, int Denom);
int GUI_GIF_GetComment     (const void * pGIF, U32 NumBytes,         U8 * pBuffer, int MaxSize, int Index);
int GUI_GIF_GetCommentEx   (GUI_GET_DATA_FUNC * pfGetData, void * p, U8 * pBuffer, int MaxSize, int Index);
int GUI_GIF_GetImageInfo   (const void * pGIF, U32 NumBytes,         GUI_GIF_IMAGE_INFO * pInfo, int Index);
int GUI_GIF_GetImageInfoEx (GUI_GET_DATA_FUNC * pfGetData, void * p, GUI_GIF_IMAGE_INFO * pInfo, int Index);
int GUI_GIF_GetInfo        (const void * pGIF, U32 NumBytes,         GUI_GIF_INFO * pInfo);
int GUI_GIF_GetInfoEx      (GUI_GET_DATA_FUNC * pfGetData, void * p, GUI_GIF_INFO * pInfo);
int GUI_GIF_GetXSize       (const void * pGIF);
int GUI_GIF_GetXSizeEx     (GUI_GET_DATA_FUNC * pfGetData, void * p);
int GUI_GIF_GetYSize       (const void * pGIF);
int GUI_GIF_GetYSizeEx     (GUI_GET_DATA_FUNC * pfGetData, void * p);
int GUI_GIF_SetFillTrans   (int OnOff);

/*********************************************************************
*
*       BMP file support
*/
int  GUI_BMP_Draw        (const void * pFileData,                  int x0, int y0);
int  GUI_BMP_DrawEx      (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int  GUI_BMP_DrawScaled  (const void * pFileData,                  int x0, int y0, int Num, int Denom);
int  GUI_BMP_DrawScaledEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom);
int  GUI_BMP_GetXSize    (const void * pFileData);
int  GUI_BMP_GetXSizeEx  (GUI_GET_DATA_FUNC * pfGetData, void * p);
int  GUI_BMP_GetYSize    (const void * pFileData);
int  GUI_BMP_GetYSizeEx  (GUI_GET_DATA_FUNC * pfGetData, void * p);
void GUI_BMP_EnableAlpha (void);
void GUI_BMP_DisableAlpha(void);

/*********************************************************************
*
*       PNG file support
*/
int GUI_PNG_Draw      (const void * pFileData, int DataSize, int x0, int y0);
int GUI_PNG_DrawEx    (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int GUI_PNG_GetXSize  (const void * pFileData, int FileSize);
int GUI_PNG_GetXSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p);
int GUI_PNG_GetYSize  (const void * pFileData, int FileSize);
int GUI_PNG_GetYSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p);

/*********************************************************************
*
*       JPEG file support
*/
typedef struct {
  int XSize;
  int YSize;
} GUI_JPEG_INFO;

int GUI_JPEG_Draw        (const void * pFileData, int DataSize,    int x0, int y0);
int GUI_JPEG_DrawEx      (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int GUI_JPEG_DrawScaled  (const void * pFileData, int DataSize,    int x0, int y0, int Num, int Denom);
int GUI_JPEG_DrawScaledEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom);
int GUI_JPEG_GetInfo     (const void * pFileData, int DataSize,    GUI_JPEG_INFO * pInfo);
int GUI_JPEG_GetInfoEx   (GUI_GET_DATA_FUNC * pfGetData, void * p, GUI_JPEG_INFO * pInfo);

/*********************************************************************
*
*       MOVIE file support
*/
#define GUI_MOVIE_NOTIFICATION_PREDRAW  0 // Immediately before frame is drawn
#define GUI_MOVIE_NOTIFICATION_POSTDRAW 1 // Immediately after a frame is drawn
#define GUI_MOVIE_NOTIFICATION_START    2 // Send when start playing a movie
#define GUI_MOVIE_NOTIFICATION_STOP     3 // Movie has stopped
#define GUI_MOVIE_NOTIFICATION_DELETE   4 // Movie has been deleted

typedef GUI_HMEM GUI_MOVIE_HANDLE;

typedef void GUI_MOVIE_FUNC(GUI_MOVIE_HANDLE hMovie, int Notification, U32 CurrentFrame);

typedef struct {
  int xSize;         // X-size of images
  int ySize;         // Y-size of images
  int msPerFrame;    // Default duration of 1 frame
  U32 NumFrames;     // Number of frames
} GUI_MOVIE_INFO;

GUI_MOVIE_HANDLE GUI_MOVIE_Create       (const void * pFileData, U32 FileSize, GUI_MOVIE_FUNC * pfNotify);
GUI_MOVIE_HANDLE GUI_MOVIE_CreateEx     (GUI_GET_DATA_FUNC * pfGetData, void * pParam, GUI_MOVIE_FUNC * pfNotify);
int              GUI_MOVIE_Delete       (GUI_MOVIE_HANDLE hMovie);
U32              GUI_MOVIE_GetFrameIndex(GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_GetInfo      (const void * pFileData, U32 FileSize, GUI_MOVIE_INFO * pInfo);
int              GUI_MOVIE_GetInfoEx    (GUI_GET_DATA_FUNC * pfGetData, void * pParam, GUI_MOVIE_INFO * pInfo);
int              GUI_MOVIE_GetPos       (GUI_MOVIE_HANDLE hMovie, int * pxPos, int * pyPos, int * pxSize, int * pySize);
int              GUI_MOVIE_GotoFrame    (GUI_MOVIE_HANDLE hMovie, U32 Frame);
int              GUI_MOVIE_Pause        (GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_Play         (GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_SetPeriod    (GUI_MOVIE_HANDLE hMovie, unsigned Period);
int              GUI_MOVIE_SetPos       (GUI_MOVIE_HANDLE hMovie, int xPos, int yPos);
int              GUI_MOVIE_ShowScaled   (GUI_MOVIE_HANDLE hMovie, int xPos, int yPos, int num, int denom, int DoLoop);
int              GUI_MOVIE_Show         (GUI_MOVIE_HANDLE hMovie, int xPos, int yPos, int DoLoop);

/*********************************************************************
*
*       Cursor routines
*/
#define GUI_CURSOR_SHOW 0
#define GUI_CURSOR_HIDE 1

typedef struct {
  const GUI_BITMAP  * pBitmap;
  int                 xHot;
  int                 yHot;
} GUI_CURSOR;

typedef struct {
  const GUI_BITMAP ** ppBm;
  int                 xHot;
  int                 yHot;
  unsigned            Period;
  const unsigned    * pPeriod;
  int                 NumItems;
} GUI_CURSOR_ANIM;

#if GUI_SUPPORT_CURSOR
  int                GUI_CURSOR_GetState     (void);
  int                GUI_CURSOR_GetStateEx   (int Layer);
  void               GUI_CURSOR_Hide         (void);
  void               GUI_CURSOR_HideEx       (int Layer);
  const GUI_CURSOR * GUI_CURSOR_Select       (const GUI_CURSOR * pCursor);
  const GUI_CURSOR * GUI_CURSOR_SelectEx     (const GUI_CURSOR * pCursor, int Layer);
  int                GUI_CURSOR_SelectAnim   (const GUI_CURSOR_ANIM * pCursorAnim);
  int                GUI_CURSOR_SelectAnimEx (const GUI_CURSOR_ANIM * pCursorAnim, int LayerIndex);
  int                GUI_CURSOR_SetBitmap    (const GUI_BITMAP * pBM);
  int                GUI_CURSOR_SetBitmapEx  (const GUI_BITMAP * pBM, int Layer);
  void               GUI_CURSOR_SetPosition  (int x, int y);
  void               GUI_CURSOR_SetPositionEx(int xNewPos, int yNewPos, int Layer);
  void               GUI_CURSOR_Show         (void);
  void               GUI_CURSOR_ShowEx       (int Layer);
  GUI_HSPRITE        GUI_CURSOR__GetSpriteEx (int LayerIndex, int * pxPos, int * pyPos);
  void               GUI_CURSOR__SetSpriteEx (GUI_HSPRITE hSprite, const GUI_CURSOR * pCursor, int LayerIndex);
#else
  #define GUI_CURSOR_Show();
  #define GUI_CURSOR_Clear();
#endif

/*********************************************************************
*
*       Sprite support
*/
#define GUI_SPRITE_CF_STAYONTOP (1 << 0)
#define GUI_SPRITE_CF_SHOW      (1 << 1)

#define GUI_SPRITE_SHOW 0
#define GUI_SPRITE_HIDE 1

GUI_HSPRITE GUI_SPRITE__CreateEx           (const GUI_BITMAP * pBM, int x, int y, int Layer, U16 Flags); /* Not to be documented, only used by cursor modul */
void        GUI_SPRITE__SetCallback        (GUI_HSPRITE hSprite, GUI_HMEM hContext, void (* pCB)(GUI_HSPRITE, int));
GUI_HSPRITE GUI_SPRITE_Create              (const GUI_BITMAP * pBM, int x, int y);
GUI_HSPRITE GUI_SPRITE_CreateAnim          (const GUI_BITMAP ** ppBm, int x, int y, unsigned Period, const unsigned * pPeriod, int NumItems);
GUI_HSPRITE GUI_SPRITE_CreateEx            (const GUI_BITMAP * pBM, int x, int y, int Layer);
GUI_HSPRITE GUI_SPRITE_CreateExAnim        (const GUI_BITMAP ** ppBm, int x, int y, unsigned Period, const unsigned * pPeriod, int NumItems, int LayerIndex);
GUI_HSPRITE GUI_SPRITE_CreateHidden        (const GUI_BITMAP * pBM, int x, int y);
GUI_HSPRITE GUI_SPRITE_CreateHiddenEx      (const GUI_BITMAP * pBM, int x, int y, int Layer);
void        GUI_SPRITE_Delete              (GUI_HSPRITE hSprite);
int         GUI_SPRITE_GetState            (GUI_HSPRITE hSprite);
void        GUI_SPRITE_Hide                (GUI_HSPRITE hSprite);
int         GUI_SPRITE_SetBitmap           (GUI_HSPRITE hSprite, const GUI_BITMAP * pBM);
int         GUI_SPRITE_SetBitmapAndPosition(GUI_HSPRITE hSprite, const GUI_BITMAP * pBM, int x, int y);
int         GUI_SPRITE_SetLoop             (GUI_HSPRITE hSprite, int OnOff);
void        GUI_SPRITE_SetPosition         (GUI_HSPRITE hSprite, int x, int y);
int         GUI_SPRITE_StartAnim           (GUI_HSPRITE hSprite);
int         GUI_SPRITE_StopAnim            (GUI_HSPRITE hSprite);
void        GUI_SPRITE_Show                (GUI_HSPRITE hSprite);

/*********************************************************************
*
*       Cursors and their bitmaps
*/
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowS,  GUI_CursorArrowSI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowM,  GUI_CursorArrowMI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowL,  GUI_CursorArrowLI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossS,  GUI_CursorCrossSI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossM,  GUI_CursorCrossMI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossL,  GUI_CursorCrossLI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorHeaderM, GUI_CursorHeaderMI;

extern GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowS, GUI_BitmapArrowSI;
extern GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowM, GUI_BitmapArrowMI;
extern GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowL, GUI_BitmapArrowLI;
extern GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapCrossS, GUI_BitmapCrossSI;
extern GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapCrossM, GUI_BitmapCrossMI;
extern GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapCrossL, GUI_BitmapCrossLI;

extern GUI_CONST_STORAGE GUI_CURSOR_ANIM GUI_CursorAnimHourglassM;

/*********************************************************************
*
*       Wrap modes
*/
typedef enum { GUI_WRAPMODE_NONE, GUI_WRAPMODE_WORD, GUI_WRAPMODE_CHAR } GUI_WRAPMODE;

/*********************************************************************
*
*       Text related routines
*/
void  GUI_DispCEOL             (void);
void  GUI_DispChar             (U16 c);
void  GUI_DispCharAt           (U16 c, I16P x, I16P y);
void  GUI_DispChars            (U16 c, int Cnt);
void  GUI_DispNextLine         (void);
void  GUI_DispString           (const char * s);
void  GUI_DispStringAt         (const char * s, int x, int y);
void  GUI_DispStringAtCEOL     (const char * s, int x, int y);
void  GUI_DispStringHCenterAt  (const char * s, int x, int y);
void  GUI__DispStringInRect    (const char * s, GUI_RECT * pRect, int TextAlign, int MaxNumChars);
void  GUI_DispStringInRect     (const char * s, GUI_RECT * pRect, int TextAlign);
#if GUI_SUPPORT_ROTATION
  void  GUI_DispStringInRectEx (const char * s, GUI_RECT * pRect, int TextAlign, int MaxLen, const GUI_ROTATION * pLCD_Api);
#endif
void  GUI_DispStringInRectMax  (const char * s, GUI_RECT * pRect, int TextAlign, int MaxLen); /* Not to be doc. */
void  GUI_DispStringInRectWrap (const char * s, GUI_RECT * pRect, int TextAlign, GUI_WRAPMODE WrapMode); /* Not to be doc. */
void  GUI_DispStringLen        (const char * s, int Len);
void  GUI_GetTextExtend        (GUI_RECT* pRect, const char * s, int Len);
int   GUI_GetYAdjust           (void);
int   GUI_GetDispPosX          (void);
int   GUI_GetDispPosY          (void);
const GUI_FONT * GUI_GetFont(void);
int   GUI_GetCharDistX         (U16 c);
int   GUI_GetCharDistXEx       (U16 c, int * pSizeX);
int   GUI_GetStringDistX       (const char * s);
GUI_DRAWMODE GUI_GetDrawMode   (void);
int   GUI_GetFontDistY         (void);
int   GUI_GetFontSizeY         (void);
void  GUI_GetFontInfo          (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
void  GUI_GetOrg               (int * px, int * py);
int   GUI_GetYSizeOfFont       (const GUI_FONT * pFont);
int   GUI_GetYDistOfFont       (const GUI_FONT * pFont);
int   GUI_GetTextAlign         (void);
int   GUI_GetTextMode          (void);
char  GUI_IsInFont             (const GUI_FONT * pFont, U16 c);
int   GUI_SetTextAlign         (int Align);
int   GUI_SetTextMode          (int Mode);
char  GUI_SetTextStyle         (char Style);
int   GUI_SetLBorder           (int x);
const GUI_FONT * GUI_SetFont(const GUI_FONT * pNewFont);
char  GUI_GotoXY               (int x, int y);
char  GUI_GotoX                (int x);
char  GUI_GotoY                (int y);
int   GUI_WrapGetNumLines      (const char * pText, int xSize, GUI_WRAPMODE WrapMode);

int   GUI_GetLeadingBlankCols (U16 c);
int   GUI_GetTrailingBlankCols(U16 c);


/*********************************************************************
*
*       System independent fonts (SIF)
*/
void GUI_SIF_CreateFont(const void * pFontData, GUI_FONT * pFont, const GUI_SIF_TYPE * pFontType);
void GUI_SIF_DeleteFont(GUI_FONT * pFont);

/*********************************************************************
*
*       External binary fonts (XBF)
*/
int  GUI_XBF_CreateFont(GUI_FONT * pFont, GUI_XBF_DATA * pXBF, const GUI_XBF_TYPE * pFontType, GUI_XBF_GET_DATA_FUNC * pfGetData, void * pVoid);
void GUI_XBF_DeleteFont(GUI_FONT * pFont);

/*********************************************************************
*
*       TrueType support (TTF)
*/
int  GUI_TTF_CreateFont   (GUI_FONT * pFont, GUI_TTF_CS * pCS);
int  GUI_TTF_CreateFontAA (GUI_FONT * pFont, GUI_TTF_CS * pCS);
void GUI_TTF_DestroyCache (void);
void GUI_TTF_Done         (void);
int  GUI_TTF_GetFamilyName(GUI_FONT * pFont, char * pBuffer, int NumBytes);
int  GUI_TTF_GetStyleName (GUI_FONT * pFont, char * pBuffer, int NumBytes);
void GUI_TTF_SetCacheSize (unsigned MaxFaces, unsigned MaxSizes, U32 MaxBytes);

/*********************************************************************
*
*       Resource file support
*/
int          GUI_LANG_GetLang          (void);
int          GUI_LANG_GetNumItems      (int IndexLang);
const char * GUI_LANG_GetText          (int IndexText);
int          GUI_LANG_GetTextBuffered  (int IndexText, char * pBuffer, int SizeOfBuffer);
int          GUI_LANG_GetTextBufferedEx(int IndexText, int IndexLang, char * pBuffer, int SizeOfBuffer);
const char * GUI_LANG_GetTextEx        (int IndexText, int IndexLang);
int          GUI_LANG_LoadCSV          (U8 * pFileData, U32 FileSize);
int          GUI_LANG_LoadCSVEx        (GUI_GET_DATA_FUNC * pfGetData, void * p);
int          GUI_LANG_LoadText         (U8 * pFileData, U32 FileSize, int IndexLang);
int          GUI_LANG_LoadTextEx       (GUI_GET_DATA_FUNC * pfGetData, void * p, int IndexLang);
int          GUI_LANG_SetLang          (int IndexLang);
unsigned     GUI_LANG_SetMaxNumLang    (unsigned MaxNumLang);
U16          GUI_LANG_SetSep           (U16 Sep);

/*********************************************************************
*
*       Unicode support
*/
int   GUI_UC_ConvertUC2UTF8   (const U16 * s, int Len, char * pBuffer, int BufferSize);
int   GUI_UC_ConvertUTF82UC   (const char * s, int Len, U16 * pBuffer, int BufferSize);
int   GUI_UC_Encode           (char * s, U16 Char);
int   GUI_UC_GetCharSize      (const char * s);
U16   GUI_UC_GetCharCode      (const char * s);
void  GUI_UC_SetEncodeNone    (void);
void  GUI_UC_SetEncodeUTF8    (void);
int   GUI_UC_EnableBIDI       (int OnOff);

void GUI_UC_DispString(const U16 * s);
void GUI_UC2DB (U16 Code, U8 * pOut);
U16  GUI_DB2UC (U8 Byte0, U8 Byte1);

/*********************************************************************
*
*       Drawing of binary, decimal and hexadecimal values
*/
void GUI_DispBin  (U32  v, U8 Len);
void GUI_DispBinAt(U32  v, I16P x, I16P y, U8 Len);
void GUI_DispDec  (I32 v, U8 Len);
void GUI_DispDecAt (I32 v, I16P x, I16P y, U8 Len);
void GUI_DispDecMin(I32 v);
void GUI_DispDecShift(I32 v, U8 Len, U8 Shift);
void GUI_DispDecSpace(I32 v, U8 MaxDigits);
void GUI_DispHex  (U32 v, U8 Len);
void GUI_DispHexAt(U32 v, I16P x, I16P y, U8 Len);
void GUI_DispSDec(I32 v, U8 Len);
void GUI_DispSDecShift(I32 v, U8 Len, U8 Shift);

/*********************************************************************
*
*       Drawing of floating point values
*/
void GUI_DispFloat    (float v, char Len);
void GUI_DispFloatFix (float v, char Len, char Fract);
void GUI_DispFloatMin (float v, char Fract);
void GUI_DispSFloatFix(float v, char Len, char Fract);
void GUI_DispSFloatMin(float v, char Fract);

/*********************************************************************
*
*       Dynamic memory management
*/
typedef struct {
  U32 TotalBytes;
  U32 FreeBytes;
  U32 UsedBytes;
  U32 AllocSize;
  U32 NumFixedBytes;
  U32 MaxUsedBytes;
} GUI_ALLOC_INFO;

GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumFreeBlocks(void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumFreeBytes (void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumUsedBlocks(void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumUsedBytes (void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetMaxUsedBytes (void);

void GUI_ALLOC_GetMemInfo  (GUI_ALLOC_INFO * pInfo);
void GUI_ALLOC_SuppressPeak(int OnOff);

GUI_HMEM           GUI_ALLOC_AllocInit       (const void * pInitData, GUI_ALLOC_DATATYPE Size);
GUI_HMEM           GUI_ALLOC_AllocNoInit     (GUI_ALLOC_DATATYPE size);
GUI_HMEM           GUI_ALLOC_AllocZero       (GUI_ALLOC_DATATYPE size);
void               GUI_ALLOC_AssignMemory    (void * p, U32 NumBytes);
void               GUI_ALLOC_Free            (GUI_HMEM  hMem);
void               GUI_ALLOC_FreeFixedBlock  (void * p);
void               GUI_ALLOC_FreePtrArray    (GUI_HMEM * pArray, int NumElems);
void               GUI_ALLOC_FreePtr         (GUI_HMEM * phMem);
void *             GUI_ALLOC_GetFixedBlock   (GUI_ALLOC_DATATYPE Size);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetMaxSize      (void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetSize         (GUI_HMEM  hMem);
void *             GUI_ALLOC_h2p             (GUI_HMEM  hMem);
GUI_HMEM           GUI_ALLOC_p2h             (void * p);
void               GUI_ALLOC_Init            (void);
void               GUI_ALLOC_Lock            (void);
void *             GUI_ALLOC_LockH           (GUI_HMEM  hMem);
GUI_HMEM           GUI_ALLOC_Realloc         (GUI_HMEM hOld, int NewSize);
GUI_ALLOC_DATATYPE GUI_ALLOC_RequestSize     (void);
void               GUI_ALLOC_SetAvBlockSize  (U32 BlockSize);
void               GUI_ALLOC_Unlock          (void);
void *             GUI_ALLOC_UnlockH         (void ** pp);
int                GUI_ALLOC_SetMaxPercentage(int MaxPercentage);

/*********************************************************************
*
*       Memory devices: GUI_MEMDEV
*/
#define GUI_MEMDEV_HASTRANS       0
#define GUI_MEMDEV_NOTRANS  (1 << 0)

typedef GUI_HMEM GUI_MEMDEV_Handle;
typedef void     GUI_CALLBACK_VOID_P        (void * p);
typedef int      GUI_ANIMATION_CALLBACK_FUNC(int TimeRem, void * pVoid);
typedef void     GUI_DRAWMEMDEV_16BPP_FUNC  (void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc);

extern GUI_ANIMATION_CALLBACK_FUNC * GUI_MEMDEV__pCbAnimation;
extern void                        * GUI_MEMDEV__pVoid;

typedef struct {
  GUI_RECT rView, rPrev;
  char FirstCall;
} GUI_AUTODEV;

typedef struct {
  char DrawFixed;
  char IsMeasurement;
} GUI_AUTODEV_INFO;

int  GUI_MEMDEV_CreateAuto(GUI_AUTODEV * pAutoDev);
void GUI_MEMDEV_DeleteAuto(GUI_AUTODEV * pAutoDev);
int  GUI_MEMDEV_DrawAuto  (GUI_AUTODEV * pAutoDev, GUI_AUTODEV_INFO * pAutoDevInfo, GUI_CALLBACK_VOID_P * pfDraw, void * pData);

/* Create a memory device which is compatible to the selected LCD */
GUI_MEMDEV_Handle GUI_MEMDEV_Create       (int x0, int y0, int xSize, int ySize);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateEx     (int x0, int y0, int xSize, int ySize, int Flags);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateFixed  (int x0, int y0, int xSize, int ySize, int Flags,
                                           const GUI_DEVICE_API     * pDeviceAPI,
                                           const LCD_API_COLOR_CONV * pColorConvAPI);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateFixed32(int x0, int y0, int xSize, int ySize);

void GUI_MEMDEV_Clear                (GUI_MEMDEV_Handle hMem);
int  GUI_MEMDEV_ClearAlpha           (GUI_MEMDEV_Handle hMemData, GUI_MEMDEV_Handle hMemMask);
void GUI_MEMDEV_CopyFromLCD          (GUI_MEMDEV_Handle hMem);
void GUI_MEMDEV_CopyFromLCDAA        (GUI_MEMDEV_Handle hMem);
void GUI_MEMDEV_CopyToLCD            (GUI_MEMDEV_Handle hMem);
void GUI_MEMDEV_CopyToLCDAA          (GUI_MEMDEV_Handle hMem);
void GUI_MEMDEV_CopyToLCDAt          (GUI_MEMDEV_Handle hMem, int x, int y);
int  GUI_MEMDEV_CompareWithLCD       (GUI_MEMDEV_Handle hMem, int * px, int * py, int * pExp, int * pAct);
void GUI_MEMDEV_Delete               (GUI_MEMDEV_Handle MemDev);
void GUI_MEMDEV_DrawPerspectiveX     (GUI_MEMDEV_Handle hMem, int x, int y, int h0, int h1, int dx, int dy);
int  GUI_MEMDEV_GetXPos              (GUI_MEMDEV_Handle hMem);
int  GUI_MEMDEV_GetXSize             (GUI_MEMDEV_Handle hMem);
int  GUI_MEMDEV_GetYPos              (GUI_MEMDEV_Handle hMem);
int  GUI_MEMDEV_GetYSize             (GUI_MEMDEV_Handle hMem);
void GUI_MEMDEV_MarkDirty            (GUI_MEMDEV_Handle hMem, int x0, int y0, int x1, int y1);
void GUI_MEMDEV_ReduceYSize          (GUI_MEMDEV_Handle hMem, int YSize);
void GUI_MEMDEV_Rotate               (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
void GUI_MEMDEV_RotateAlpha          (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag, U8 Alpha);
void GUI_MEMDEV_RotateHR             (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, I32 dx, I32 dy, int a, int Mag);
void GUI_MEMDEV__Rotate              (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag, U32 Mask);
void GUI_MEMDEV__RotateHR            (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, I32 dx, I32 dy, int a, int Mag, U32 Mask);
void GUI_MEMDEV_RotateHQ             (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
void GUI_MEMDEV_RotateHQAlpha        (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag, U8 Alpha);
void GUI_MEMDEV_RotateHQHR           (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, I32 dx, I32 dy, int a, int Mag);
void GUI_MEMDEV_RotateHQT            (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
void GUI_MEMDEV_RotateHQTI           (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
GUI_MEMDEV_Handle GUI_MEMDEV_Select  (GUI_MEMDEV_Handle hMem);  /* Select (activate) a particular memory device. */
void  GUI_MEMDEV_SetOrg              (GUI_MEMDEV_Handle hMem, int x0, int y0);
void  GUI_MEMDEV_WriteAt             (GUI_MEMDEV_Handle hMem, int x, int y);
void  GUI_MEMDEV_Write               (GUI_MEMDEV_Handle hMem);
void  GUI_MEMDEV_WriteAlphaAt        (GUI_MEMDEV_Handle hMem, int Alpha, int x, int y);
void  GUI_MEMDEV_WriteAlpha          (GUI_MEMDEV_Handle hMem, int Alpha);
void  GUI_MEMDEV_WriteExAt           (GUI_MEMDEV_Handle hMem, int x, int y, int xMag, int yMag, int Alpha);
void  GUI_MEMDEV_WriteEx             (GUI_MEMDEV_Handle hMem, int xMag, int yMag, int Alpha);
void  GUI_MEMDEV_WriteOpaque         (GUI_MEMDEV_Handle hMem);
void  GUI_MEMDEV_WriteOpaqueAt       (GUI_MEMDEV_Handle hMem, int x, int y);
int   GUI_MEMDEV_Draw                (GUI_RECT * pRect, GUI_CALLBACK_VOID_P * pfDraw, void * pData, int NumLines, int Flags);
void* GUI_MEMDEV_GetDataPtr          (GUI_MEMDEV_Handle hMem);
void  GUI_MEMDEV_SetColorConv        (GUI_MEMDEV_Handle hMem, const LCD_API_COLOR_CONV * pColorConvAPI);
const LCD_API_COLOR_CONV * GUI_MEMDEV_GetColorConv(GUI_MEMDEV_Handle hMemDev);
int   GUI_MEMDEV_GetBitsPerPixel     (GUI_MEMDEV_Handle hMemDev);
int   GUI_MEMDEV_FadeInDevices       (GUI_MEMDEV_Handle hMem0, GUI_MEMDEV_Handle hMem1, int Period);
int   GUI_MEMDEV_FadeOutDevices      (GUI_MEMDEV_Handle hMem0, GUI_MEMDEV_Handle hMem1, int Period);
void  GUI_MEMDEV_SerializeBMP        (GUI_MEMDEV_Handle hDev, GUI_CALLBACK_VOID_U8_P * pfSerialize, void * p);
void  GUI_MEMDEV_SetAnimationCallback(GUI_ANIMATION_CALLBACK_FUNC * pCbAnimation, void * pVoid);
void  GUI_MEMDEV__FadeDevice         (GUI_MEMDEV_Handle hMemWin, GUI_MEMDEV_Handle hMemBk, GUI_MEMDEV_Handle hMemDst, U8 Intens);
void  GUI_MEMDEV__FadeDeviceEx       (GUI_MEMDEV_Handle hMemWin, GUI_MEMDEV_Handle hMemBk, GUI_MEMDEV_Handle hMemDst, U8 Intens, int xPosWin, int yPosWin);
int   GUI_MEMDEV_PunchOutDevice      (GUI_MEMDEV_Handle hMemData, GUI_MEMDEV_Handle hMemMask);
void  GUI_MEMDEV_SetTimePerFrame     (unsigned TimePerFrame);

void  GUI_SelectLCD(void);

/* Blurring, dithering and blending */
GUI_MEMDEV_Handle GUI_MEMDEV_CreateBlurredDevice32  (GUI_MEMDEV_Handle hMem, U8 Depth);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateBlurredDevice32HQ(GUI_MEMDEV_Handle hMem, U8 Depth);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateBlurredDevice32LQ(GUI_MEMDEV_Handle hMem, U8 Depth);
void              GUI_MEMDEV_SetBlurHQ              (void);
void              GUI_MEMDEV_SetBlurLQ              (void);
int               GUI_MEMDEV_BlendColor32           (GUI_MEMDEV_Handle hMem, U32 BlendColor, U8 BlendIntens);
int               GUI_MEMDEV_Dither32               (GUI_MEMDEV_Handle hMem, const LCD_API_COLOR_CONV * pColorConvAPI);

/* Optional custom drawing of 16bpp memory devices */
void GUI_MEMDEV_SetDrawMemdev16bppFunc(GUI_DRAWMEMDEV_16BPP_FUNC * pfDrawMemdev16bppFunc);

/*********************************************************************
*
*       Alpha blending
*/
typedef struct {
  U32 UserAlpha;
} GUI_ALPHA_STATE;

#define GUI_MAKE_ALPHA(Alpha, Color) ((U32)(((U32)Alpha << 24) | (Color & 0xFFFFFF)))

unsigned GUI_EnableAlpha         (unsigned OnOff);
U32      GUI_RestoreUserAlpha    (GUI_ALPHA_STATE * pAlphaState);
unsigned GUI_SetAlpha            (U8 Alpha);
U32      GUI_SetUserAlpha        (GUI_ALPHA_STATE * pAlphaState, U32 UserAlpha);
void     GUI_SetFuncAlphaBlending(void (* pfAlphaBlending)(LCD_COLOR *, LCD_COLOR *, LCD_COLOR *, U32));
void     GUI_SetFuncMixColors    (LCD_COLOR (* pFunc)(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens));
void     GUI_SetFuncMixColorsBulk(void (* pFunc)(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens));
unsigned GUI_PreserveTrans       (unsigned OnOff);

/*********************************************************************
*
*       Multi layer support
*/
unsigned GUI_SelectLayer(unsigned Index);
unsigned GUI_GetSelLayer(void);

int  GUI_SetLayerPosEx  (unsigned Index, int xPos, int yPos);
int  GUI_SetLayerSizeEx (unsigned Index, int xSize, int ySize);
int  GUI_SetLayerVisEx  (unsigned Index, int OnOff);
int  GUI_SetLayerAlphaEx(unsigned Index, int Alpha);
void GUI_GetLayerPosEx  (unsigned Index, int * pxPos, int * pyPos);

void     GUI_AssignCursorLayer(unsigned Index, unsigned CursorLayer);
unsigned GUI_GetCursorLayer   (unsigned Index);

/*********************************************************************
*
*       Multiple buffers and display origin
*/
void GUI_SetOrg(int x, int y);

void GUI_MULTIBUF_Begin          (void);
void GUI_MULTIBUF_BeginEx        (int LayerIndex);
void GUI_MULTIBUF_End            (void);
void GUI_MULTIBUF_EndEx          (int LayerIndex);
void GUI_MULTIBUF_Config         (int NumBuffers);
void GUI_MULTIBUF_ConfigEx       (int LayerIndex, int NumBuffers);
void GUI_MULTIBUF_Confirm        (int Index);
void GUI_MULTIBUF_ConfirmEx      (int LayerIndex, int BufferIndex);
int  GUI_MULTIBUF_GetNumBuffers  (void);
int  GUI_MULTIBUF_GetNumBuffersEx(int LayerIndex);
void GUI_MULTIBUF_UseSingleBuffer(void);

/*********************************************************************
*
*       emWinSPY
*/
int  GUI_SPY_Process      (GUI_tSend pfSend, GUI_tRecv pfRecv, void * pConnectInfo);
void GUI_SPY_SetMemHandler(GUI_tMalloc pMalloc, GUI_tFree pFree);
int  GUI_SPY_StartServer  (void);
int  GUI_SPY_X_StartServer(void);

/*********************************************************************
*
*       Basics for animations
*/
#define ANIM_LINEAR     GUI_ANIM__Linear
#define ANIM_ACCEL      GUI_ANIM__Accel
#define ANIM_DECEL      GUI_ANIM__Decel
#define ANIM_ACCELDECEL GUI_ANIM__AccelDecel

#define GUI_ANIM_START   0
#define GUI_ANIM_RUNNING 1
#define GUI_ANIM_END     2

#ifndef   GUI_ANIM_RANGE
  #define GUI_ANIM_RANGE 32767L
#endif

typedef GUI_HMEM GUI_ANIM_HANDLE;

typedef I32 (* GUI_ANIM_GETPOS_FUNC)(GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);

typedef struct {
  int Pos;
  int State;
  GUI_ANIM_HANDLE hAnim;
  GUI_TIMER_TIME Period;
} GUI_ANIM_INFO;

typedef void GUI_ANIMATION_FUNC(GUI_ANIM_INFO * pInfo, void * pVoid);

I32 GUI_ANIM__Linear    (GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);
I32 GUI_ANIM__Decel     (GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);
I32 GUI_ANIM__Accel     (GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);
I32 GUI_ANIM__AccelDecel(GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);

int             GUI_ANIM_AddItem(GUI_ANIM_HANDLE hAnim, GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_ANIM_GETPOS_FUNC pfGetPos, void * pVoid, GUI_ANIMATION_FUNC * pfAnim);
GUI_ANIM_HANDLE GUI_ANIM_Create (GUI_TIMER_TIME Period, unsigned MinTimePerFrame, void * pVoid, void (* pfSliceInfo)(int State, void * pVoid));
void            GUI_ANIM_Delete (GUI_ANIM_HANDLE hAnim);
int             GUI_ANIM_Exec   (GUI_ANIM_HANDLE hAnim);
void            GUI_ANIM_Start  (GUI_ANIM_HANDLE hAnim);

/*********************************************************************
*
*       Display orientation
*/
/*********************************************************************
*
*       GUI_ORIENTATION_API
*/
typedef struct {
  void     (* pfDrawBitmap   )(GUI_DEVICE * pDevice, int x0, int y0, int xsize, int ysize, int BitsPerPixel, int BytesPerLine, const U8 * pData, int Diff, const LCD_PIXELINDEX * pTrans);
  void     (* pfDrawHLine    )(GUI_DEVICE * pDevice, int x0, int y0,  int x1);
  void     (* pfDrawVLine    )(GUI_DEVICE * pDevice, int x , int y0,  int y1);
  void     (* pfFillRect     )(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1);
  unsigned (* pfGetPixelIndex)(GUI_DEVICE * pDevice, int x, int y);
  void     (* pfSetPixelIndex)(GUI_DEVICE * pDevice, int x, int y, int ColorIndex);
  void     (* pfXorPixel     )(GUI_DEVICE * pDevice, int x, int y);
  int      BytesPerPixel;
} GUI_ORIENTATION_API;

extern const GUI_ORIENTATION_API GUI_OrientationAPI_C0;
extern const GUI_ORIENTATION_API GUI_OrientationAPI_C8;
extern const GUI_ORIENTATION_API GUI_OrientationAPI_C16;
extern const GUI_ORIENTATION_API GUI_OrientationAPI_C32;

#define GUI_ORIENTATION_C0  &GUI_OrientationAPI_C0
#define GUI_ORIENTATION_C8  &GUI_OrientationAPI_C8
#define GUI_ORIENTATION_C16 &GUI_OrientationAPI_C16
#define GUI_ORIENTATION_C32 &GUI_OrientationAPI_C32

int GUI_SetOrientation        (int Orientation);
int GUI_SetOrientationEx      (int Orientation, int LayerIndex);
int GUI_SetOrientationExCached(int Orientation, int LayerIndex, const GUI_ORIENTATION_API * pAPI);

/*********************************************************************
*
*       Measure device: GUI_MEASDEV
*/
typedef GUI_HMEM GUI_MEASDEV_Handle;

GUI_MEASDEV_Handle GUI_MEASDEV_Create (void);
void               GUI_MEASDEV_Delete (GUI_MEASDEV_Handle hMemDev);
void               GUI_MEASDEV_Select (GUI_MEASDEV_Handle hMem);
void               GUI_MEASDEV_GetRect(GUI_MEASDEV_Handle hMem, GUI_RECT * pRect);
void               GUI_MEASDEV_ClearRect(GUI_MEASDEV_Handle hMem);

/*********************************************************************
*
*       Polygon helpers
*/
void GUI_RotatePolygon (GUI_POINT * pDest, const GUI_POINT * pSrc, int NumPoints, float Angle);
void GUI_MagnifyPolygon(GUI_POINT * pDest, const GUI_POINT * pSrc, int NumPoints, int Mag);
void GUI_EnlargePolygon(GUI_POINT * pDest, const GUI_POINT * pSrc, int NumPoints, int Len);

/*********************************************************************
*
*       Streamed bitmaps
*/
#define GUI_BITMAPSTREAM_GET_BUFFER     1
#define GUI_BITMAPSTREAM_RELEASE_BUFFER 2
#define GUI_BITMAPSTREAM_MODIFY_PALETTE 3

#define DECLARE_CREATE_FROM_STREAM(ID) int GUI_CreateBitmapFromStream##ID(GUI_BITMAP * pBMP, GUI_LOGPALETTE * pPAL, const void * p);

DECLARE_CREATE_FROM_STREAM(IDX)
DECLARE_CREATE_FROM_STREAM(RLE4)
DECLARE_CREATE_FROM_STREAM(RLE8)
DECLARE_CREATE_FROM_STREAM(565)
DECLARE_CREATE_FROM_STREAM(M565)
DECLARE_CREATE_FROM_STREAM(555)
DECLARE_CREATE_FROM_STREAM(M555)
DECLARE_CREATE_FROM_STREAM(A565)
DECLARE_CREATE_FROM_STREAM(AM565)
DECLARE_CREATE_FROM_STREAM(A555)
DECLARE_CREATE_FROM_STREAM(AM555)
DECLARE_CREATE_FROM_STREAM(RLE16)
DECLARE_CREATE_FROM_STREAM(RLEM16)
DECLARE_CREATE_FROM_STREAM(24)
DECLARE_CREATE_FROM_STREAM(Alpha)
DECLARE_CREATE_FROM_STREAM(M8888I)
DECLARE_CREATE_FROM_STREAM(RLEAlpha)
DECLARE_CREATE_FROM_STREAM(RLE32)
DECLARE_CREATE_FROM_STREAM(444_12)
DECLARE_CREATE_FROM_STREAM(M444_12)
DECLARE_CREATE_FROM_STREAM(444_12_1)
DECLARE_CREATE_FROM_STREAM(M444_12_1)
DECLARE_CREATE_FROM_STREAM(444_16)
DECLARE_CREATE_FROM_STREAM(M444_16)

int  GUI_CreateBitmapFromStream   (GUI_BITMAP * pBMP, GUI_LOGPALETTE * pPAL, const void * p);
void GUI_DrawStreamedBitmap       (const void * p, int x, int y);
void GUI_DrawStreamedBitmapAuto   (const void * p, int x, int y);
int  GUI_DrawStreamedBitmapEx     (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapExAuto (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap555Ex  (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM555Ex (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap565Ex  (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM565Ex (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapA555Ex (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapAM555Ex(GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapA565Ex (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapAM565Ex(GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap24Ex   (GUI_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
void GUI_GetStreamedBitmapInfo    (const void * p, GUI_BITMAPSTREAM_INFO * pInfo);
int  GUI_GetStreamedBitmapInfoEx  (GUI_GET_DATA_FUNC * pfGetData, const void * p, GUI_BITMAPSTREAM_INFO * pInfo);
void GUI_SetStreamedBitmapHook    (GUI_BITMAPSTREAM_CALLBACK pfStreamedBitmapHook);

void LCD__RLE4_SetFunc (GUI_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off, const LCD_LOGPALETTE * pLogPal);
void LCD__RLE8_SetFunc (GUI_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off, const LCD_LOGPALETTE * pLogPal);
void LCD__RLE16_SetFunc(GUI_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off);
void LCD__RLE32_SetFunc(GUI_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off);

/*********************************************************************
*
*       BMP-export
*/
void GUI_BMP_Serialize     (GUI_CALLBACK_VOID_U8_P * pfSerialize, void * p);
void GUI_BMP_SerializeEx   (GUI_CALLBACK_VOID_U8_P * pfSerialize, int x0, int y0, int xSize, int ySize, void * p);
void GUI_BMP_SerializeExBpp(GUI_CALLBACK_VOID_U8_P * pfSerialize, int x0, int y0, int xSize, int ySize, void * p, int BitsPerPixel);

/*********************************************************************
*
*       Time / execution related routines
*/
void           GUI_Delay  (int Period);
GUI_TIMER_TIME GUI_GetTime(void);
int            GUI_Exec(void);         /* Execute all jobs ... Return 0 if nothing was done. */
int            GUI_Exec1(void);        /* Execute one job  ... Return 0 if nothing was done. */

/*********************************************************************
*
*       MessageBox
*/
int     GUI_MessageBox   (const char * sMessage, const char * sCaption, int Flags);
#define GUI_MESSAGEBOX_CF_MOVEABLE (1 << 0)
#define GUI_MESSAGEBOX_CF_MODAL    (1 << 1)

#define GUI_MB_OK                20
#define GUI_MB_WARNING           21

/*********************************************************************
*
*       GUI_TIMER module
*/
#define GUI_TIMER_CF_WINDOW (1 << 0)
#define GUI_TIMER_CF_CURSOR (1 << 1)

typedef GUI_HMEM GUI_TIMER_HANDLE;

typedef struct {
  GUI_TIMER_TIME   Time;
  U32              Context;
  GUI_TIMER_HANDLE hTimer;
} GUI_TIMER_MESSAGE;

typedef void GUI_TIMER_CALLBACK(/*const*/ GUI_TIMER_MESSAGE* pTM);

GUI_TIMER_HANDLE GUI_TIMER_Create   (GUI_TIMER_CALLBACK * cb, GUI_TIMER_TIME Time, U32 Context, U16 Flags);
void             GUI_TIMER_Delete   (GUI_TIMER_HANDLE hObj);

/* Methods changing properties */
GUI_TIMER_TIME GUI_TIMER_GetPeriod(GUI_TIMER_HANDLE hObj);
void           GUI_TIMER_SetPeriod(GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Period);
void           GUI_TIMER_SetTime  (GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Period);
void           GUI_TIMER_SetDelay (GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Delay);
void           GUI_TIMER_Restart  (GUI_TIMER_HANDLE hObj);
int            GUI_TIMER_GetFlag  (GUI_TIMER_HANDLE hObj, int Flag); /* Not to be documented */
int            GUI_TIMER_Exec     (void);

/*********************************************************************
*
*       Anti Aliasing
*/
#define GUI_AA_TRANS   0 // Foreground color mixed up with current content of framebuffer
#define GUI_AA_NOTRANS 1 // Foreground color mixed up with current background color

void GUI_AA_DisableHiRes     (void);
void GUI_AA_EnableHiRes      (void);
int  GUI_AA_GetFactor        (void);
void GUI_AA_SetFactor        (int Factor);
void GUI_AA_DrawArc          (int x0, int y0, int rx, int ry, int a0, int a1);
void GUI_AA_DrawLine         (int x0, int y0, int x1, int y1);
void GUI_AA_DrawPolyOutline  (const GUI_POINT * pSrc, int NumPoints, int Thickness, int x, int y);
void GUI_AA_DrawPolyOutlineEx(const GUI_POINT * pSrc, int NumPoints, int Thickness, int x, int y, GUI_POINT * pBuffer);
void GUI_AA_DrawRoundedRect  (int x0, int y0, int x1, int y1, int r);
void GUI_AA_DrawRoundedRectEx(GUI_RECT * pRect, int r);
void GUI_AA_FillCircle       (int x0, int y0, int r);
void GUI_AA_FillEllipse      (int x0, int y0, int rx, int ry);
void GUI_AA_FillPolygon      (GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GUI_AA_FillRoundedRect  (int x0, int y0, int x1, int y1, int r);
void GUI_AA_FillRoundedRectEx(GUI_RECT * pRect, int r);
//int  GUI_AA_PreserveTrans    (int OnOff);
int  GUI_AA_SetDrawMode      (int Mode);
void GUI_AA_SetpfDrawCharAA4 (int (* pfDrawChar)(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine));

#define GUI_AA_PreserveTrans(OnOff) GUI_PreserveTrans(OnOff)

/*********************************************************************
*
*       Keyboard
*/
/* Message layer */
void GUI_StoreKeyMsg(int Key, int Pressed);
void GUI_SendKeyMsg (int Key, int Pressed);
int  GUI_PollKeyMsg (void);
void GUI_GetKeyState(GUI_KEY_STATE * pState);

void GUI_KEY__SetHook(void (* pfHook)(const GUI_KEY_STATE *));

/* Application layer */
int  GUI_GetKey(void);
int  GUI_WaitKey(void);
void GUI_StoreKey(int c);
void GUI_ClearKeyBuffer(void);

/*********************************************************************
*
*       Task synchronization
*/
void GUI_WaitEvent            (void);
void GUI_SignalEvent          (void);
void GUI_SetSignalEventFunc   (GUI_SIGNAL_EVENT_FUNC     pfSignalEvent);
void GUI_SetWaitEventFunc     (GUI_WAIT_EVENT_FUNC       pfWaitEvent);
void GUI_SetWaitEventTimedFunc(GUI_WAIT_EVENT_TIMED_FUNC pfWaitEventTimed);

/*********************************************************************
*
*       Joystick, generic
*/
void GUI_JOYSTICK_StoreState(const GUI_PID_STATE * pState);

/*********************************************************************
*
*       PID  (Pointer input device ... mouse/touch)
*/
void GUI_PID_StoreState     (const GUI_PID_STATE * pState);
int  GUI_PID_GetState       (      GUI_PID_STATE * pState);
void GUI_PID_GetCurrentState(      GUI_PID_STATE * pState);
int  GUI_PID_IsEmpty        (void);
int  GUI_PID_IsPressed      (void);
void GUI_PID__SetHook       (void (* pfHook)(const GUI_PID_STATE *));

/*********************************************************************
*
*       Mouse, generic
*/
int  GUI_MOUSE_GetState  (      GUI_PID_STATE * pState);
void GUI_MOUSE_StoreState(const GUI_PID_STATE * pState);

/*********************************************************************
*
*       TOUCH screen, generic
*/
int  GUI_TOUCH_GetLayer     (void);
int  GUI_TOUCH_GetState     (GUI_PID_STATE * pState);
void GUI_TOUCH_GetUnstable  (int * px, int * py);  /* for diagnostics only */
void GUI_TOUCH_SetLayer     (int Layer);
void GUI_TOUCH_StoreState   (int x, int y);
void GUI_TOUCH_StoreStateEx (const GUI_PID_STATE * pState);
void GUI_TOUCH_StoreUnstable(int x, int y);

/*********************************************************************
*
*       Mouse, PS2 driver
*/
void GUI_MOUSE_DRIVER_PS2_Init(void);               /* optional */
void GUI_MOUSE_DRIVER_PS2_OnRx(unsigned char Data);

/*********************************************************************
*
*       TOUCH screen, analog driver
*/
int  GUI_TOUCH_CalcCoefficients (int NumPoints, int * pxRef, int * pyRef, int * pxSample, int * pySample, int xSize, int ySize);
int  GUI_TOUCH_Calibrate        (int Coord, int Log0, int Log1, int Phys0, int Phys1);
int  GUI_TOUCH_CalibratePoint   (int * px, int * py);
void GUI_TOUCH_EnableCalibration(int OnOff);
void GUI_TOUCH_Exec             (void);
int  GUI_TOUCH_GetxPhys         (void);    /* for diagnostics only */
int  GUI_TOUCH_GetyPhys         (void);    /* for diagnostics only */
void GUI_TOUCH_SetCalibration   (int (* pFunc)(int *, int *)); /* Not to be documented */
void GUI_TOUCH_SetOrientation   (unsigned Orientation);
int  GUI_TOUCH_TransformPoint   (int * px, int * py);          /* Not to be documented */

/*********************************************************************
*
*       TOUCH: imports
*
* Please note: The following functions are required by the module.
* They need to be part of your application software (or rather, part
* of the hardware-layer of your software).
*/
void GUI_TOUCH_X_ActivateX(void);
void GUI_TOUCH_X_ActivateY(void);
void GUI_TOUCH_X_Disable  (void);
int  GUI_TOUCH_X_MeasureX (void);
int  GUI_TOUCH_X_MeasureY (void);

/*********************************************************************
*
*       GUI_X_
*
* Externals, to be defined by application
*
* The externals defined below should be defined by the
* application. They are per default contained in the module
* GUI_X.c.
* Note that a lot if not all of these are not required in most target
* systems.
* For this module, samples are available for configurations
* with or without operating system.
*/
//
// Configuration
//
void GUI_X_Config(void);
void GUI_X_Init  (void);

//
// Timing routines
//
GUI_TIMER_TIME GUI_X_GetTime(void);
void           GUI_X_Delay  (int Period);

//
// Multitask routines - required only if multitasking is used (#define GUI_OS 1)
//
void GUI_X_Unlock   (void);
void GUI_X_Lock     (void);
U32  GUI_X_GetTaskId(void);
void GUI_X_InitOS   (void);

//
// Event driving (optional with multitasking)
//
void GUI_X_ExecIdle      (void);
void GUI_X_WaitEvent     (void);
void GUI_X_WaitEventTimed(int Period);
void GUI_X_SignalEvent   (void);

//
// Recording (logs/warnings and errors) - required only for higher levels
//
void GUI_X_Log     (const char * s);
void GUI_X_Warn    (const char * s);
void GUI_X_ErrorOut(const char * s);

/*********************************************************************
*
*       Constants for fonts and bitmaps
*/
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE4;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE4Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE8;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE8Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE16Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLEM16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLEM16Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE32;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE32Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLEAlpha;

extern const GUI_BITMAP_METHODS GUI_BitmapMethods444_12;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM444_12;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods444_12_1;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM444_12_1;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods444_16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM444_16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods555;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM555;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods24;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods888;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM888;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods8888;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM8888I;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsA565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsAM565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsA555;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsAM555;

#define GUI_COMPRESS_RLE4 0
#define GUI_COMPRESS_RLE8 0

#define GUI_DRAW_RLE4         &GUI_BitmapMethodsRLE4       /* Method table ! */
#define GUI_DRAW_RLE8         &GUI_BitmapMethodsRLE8       /* Method table ! */
#define GUI_DRAW_RLE16        &GUI_BitmapMethodsRLE16      /* Method table ! */
#define GUI_DRAW_RLEM16       &GUI_BitmapMethodsRLEM16     /* Method table ! */
#define GUI_DRAW_RLE32        &GUI_BitmapMethodsRLE32      /* Method table ! */
#define GUI_DRAW_RLEALPHA     &GUI_BitmapMethodsRLEAlpha   /* Method table ! */

#define GUI_DRAW_BMP444_12    &GUI_BitmapMethods444_12     /* Method table ! */
#define GUI_DRAW_BMPM444_12   &GUI_BitmapMethodsM444_12    /* Method table ! */
#define GUI_DRAW_BMP444_12_1  &GUI_BitmapMethods444_12_1   /* Method table ! */
#define GUI_DRAW_BMPM444_12_1 &GUI_BitmapMethodsM444_12_1  /* Method table ! */
#define GUI_DRAW_BMP444_16    &GUI_BitmapMethods444_16     /* Method table ! */
#define GUI_DRAW_BMPM444_16   &GUI_BitmapMethodsM444_16    /* Method table ! */
#define GUI_DRAW_BMP555       &GUI_BitmapMethods555        /* Method table ! */
#define GUI_DRAW_BMPM555      &GUI_BitmapMethodsM555       /* Method table ! */
#define GUI_DRAW_BMP565       &GUI_BitmapMethods565        /* Method table ! */
#define GUI_DRAW_BMPM565      &GUI_BitmapMethodsM565       /* Method table ! */
#define GUI_DRAW_BMP24        &GUI_BitmapMethods24         /* Method table ! */
#define GUI_DRAW_BMP888       &GUI_BitmapMethods888        /* Method table ! */
#define GUI_DRAW_BMPM888      &GUI_BitmapMethodsM888       /* Method table ! */
#define GUI_DRAW_BMP8888      &GUI_BitmapMethods8888       /* Method table ! */
#define GUI_DRAW_BMPM8888I    &GUI_BitmapMethodsM8888I     /* Method table ! */
#define GUI_DRAW_BMPA555      &GUI_BitmapMethodsA555       /* Method table ! */
#define GUI_DRAW_BMPAM555     &GUI_BitmapMethodsAM555      /* Method table ! */
#define GUI_DRAW_BMPA565      &GUI_BitmapMethodsA565       /* Method table ! */
#define GUI_DRAW_BMPAM565     &GUI_BitmapMethodsAM565      /* Method table ! */

extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_Ext;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_Frm;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA2;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA4;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA2_EXT;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA4_EXT;

extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_Ext;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_Frm;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_AA2_Ext;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_AA4_Ext;

/*********************************************************************
*
*       GUI_KEY_...
*
* These ID values are basically meant to be used with widgets
* Note that we have chosen the values to be close to existing
* "standards", so do not change them unless forced to.
*
*/
#define GUI_KEY_BACKSPACE         8         /* ASCII: BACKSPACE Crtl-H */
#define GUI_KEY_TAB               9         /* ASCII: TAB       Crtl-I */
#define GUI_KEY_BACKTAB           10
#define GUI_KEY_ENTER             13        /* ASCII: ENTER     Crtl-M */
#define GUI_KEY_LEFT              16
#define GUI_KEY_UP                17
#define GUI_KEY_RIGHT             18
#define GUI_KEY_DOWN              19
#define GUI_KEY_HOME              23
#define GUI_KEY_END               24
#define GUI_KEY_SHIFT             25
#define GUI_KEY_CONTROL           26
#define GUI_KEY_ESCAPE            27        /* ASCII: ESCAPE    0x1b   */
#define GUI_KEY_INSERT            29
#define GUI_KEY_DELETE            30
#define GUI_KEY_SPACE             32
#define GUI_KEY_PGUP              33
#define GUI_KEY_PGDOWN            34

#define GUI_KEY_F1                40
#define GUI_KEY_F2                41

/*********************************************************************
*
*       Dialog item IDs
*
*  The IDs below are arbitrary values. They just have to be unique
*  within the dialog.
*
*  If you use your own Ids, we recommend to use values above GUI_ID_USER.
*/
#define GUI_ID_OK                1
#define GUI_ID_CANCEL            2
#define GUI_ID_YES               3
#define GUI_ID_NO                4
#define GUI_ID_CLOSE             5
#define GUI_ID_HELP              6
#define GUI_ID_MAXIMIZE          7
#define GUI_ID_MINIMIZE          8

#define GUI_ID_VSCROLL    0xFE
#define GUI_ID_HSCROLL    0xFF

#define GUI_ID_EDIT0      0x100
#define GUI_ID_EDIT1      0x101
#define GUI_ID_EDIT2      0x102
#define GUI_ID_EDIT3      0x103
#define GUI_ID_EDIT4      0x104
#define GUI_ID_EDIT5      0x105
#define GUI_ID_EDIT6      0x106
#define GUI_ID_EDIT7      0x107
#define GUI_ID_EDIT8      0x108
#define GUI_ID_EDIT9      0x109

#define GUI_ID_LISTBOX0   0x110
#define GUI_ID_LISTBOX1   0x111
#define GUI_ID_LISTBOX2   0x112
#define GUI_ID_LISTBOX3   0x113
#define GUI_ID_LISTBOX4   0x114
#define GUI_ID_LISTBOX5   0x115
#define GUI_ID_LISTBOX6   0x116
#define GUI_ID_LISTBOX7   0x117
#define GUI_ID_LISTBOX8   0x118
#define GUI_ID_LISTBOX9   0x119

#define GUI_ID_CHECK0     0x120
#define GUI_ID_CHECK1     0x121
#define GUI_ID_CHECK2     0x122
#define GUI_ID_CHECK3     0x123
#define GUI_ID_CHECK4     0x124
#define GUI_ID_CHECK5     0x125
#define GUI_ID_CHECK6     0x126
#define GUI_ID_CHECK7     0x127
#define GUI_ID_CHECK8     0x128
#define GUI_ID_CHECK9     0x129

#define GUI_ID_SLIDER0    0x130
#define GUI_ID_SLIDER1    0x131
#define GUI_ID_SLIDER2    0x132
#define GUI_ID_SLIDER3    0x133
#define GUI_ID_SLIDER4    0x134
#define GUI_ID_SLIDER5    0x135
#define GUI_ID_SLIDER6    0x136
#define GUI_ID_SLIDER7    0x137
#define GUI_ID_SLIDER8    0x138
#define GUI_ID_SLIDER9    0x139

#define GUI_ID_SCROLLBAR0 0x140
#define GUI_ID_SCROLLBAR1 0x141
#define GUI_ID_SCROLLBAR2 0x142
#define GUI_ID_SCROLLBAR3 0x143

#define GUI_ID_RADIO0     0x150
#define GUI_ID_RADIO1     0x151
#define GUI_ID_RADIO2     0x152
#define GUI_ID_RADIO3     0x153
#define GUI_ID_RADIO4     0x154
#define GUI_ID_RADIO5     0x155
#define GUI_ID_RADIO6     0x156
#define GUI_ID_RADIO7     0x157

#define GUI_ID_TEXT0      0x160
#define GUI_ID_TEXT1      0x161
#define GUI_ID_TEXT2      0x162
#define GUI_ID_TEXT3      0x163
#define GUI_ID_TEXT4      0x164
#define GUI_ID_TEXT5      0x165
#define GUI_ID_TEXT6      0x166
#define GUI_ID_TEXT7      0x167
#define GUI_ID_TEXT8      0x168
#define GUI_ID_TEXT9      0x169

#define GUI_ID_BUTTON0    0x170
#define GUI_ID_BUTTON1    0x171
#define GUI_ID_BUTTON2    0x172
#define GUI_ID_BUTTON3    0x173
#define GUI_ID_BUTTON4    0x174
#define GUI_ID_BUTTON5    0x175
#define GUI_ID_BUTTON6    0x176
#define GUI_ID_BUTTON7    0x177
#define GUI_ID_BUTTON8    0x178
#define GUI_ID_BUTTON9    0x179

#define GUI_ID_DROPDOWN0  0x180
#define GUI_ID_DROPDOWN1  0x181
#define GUI_ID_DROPDOWN2  0x182
#define GUI_ID_DROPDOWN3  0x183

#define GUI_ID_MULTIEDIT0 0x190
#define GUI_ID_MULTIEDIT1 0x191
#define GUI_ID_MULTIEDIT2 0x192
#define GUI_ID_MULTIEDIT3 0x193

#define GUI_ID_LISTVIEW0  0x200
#define GUI_ID_LISTVIEW1  0x201
#define GUI_ID_LISTVIEW2  0x202
#define GUI_ID_LISTVIEW3  0x203

#define GUI_ID_PROGBAR0   0x210
#define GUI_ID_PROGBAR1   0x211
#define GUI_ID_PROGBAR2   0x212
#define GUI_ID_PROGBAR3   0x213

#define GUI_ID_GRAPH0     0x220
#define GUI_ID_GRAPH1     0x221
#define GUI_ID_GRAPH2     0x222
#define GUI_ID_GRAPH3     0x223

#define GUI_ID_MULTIPAGE0 0x230
#define GUI_ID_MULTIPAGE1 0x231
#define GUI_ID_MULTIPAGE2 0x232
#define GUI_ID_MULTIPAGE3 0x233

#define GUI_ID_TREEVIEW0  0x240
#define GUI_ID_TREEVIEW1  0x241
#define GUI_ID_TREEVIEW2  0x242
#define GUI_ID_TREEVIEW3  0x243

#define GUI_ID_ICONVIEW0  0x250
#define GUI_ID_ICONVIEW1  0x251
#define GUI_ID_ICONVIEW2  0x252
#define GUI_ID_ICONVIEW3  0x253

#define GUI_ID_LISTWHEEL0 0x260
#define GUI_ID_LISTWHEEL1 0x261
#define GUI_ID_LISTWHEEL2 0x262
#define GUI_ID_LISTWHEEL3 0x263

#define GUI_ID_IMAGE0     0x270
#define GUI_ID_IMAGE1     0x271
#define GUI_ID_IMAGE2     0x272
#define GUI_ID_IMAGE3     0x273
#define GUI_ID_IMAGE4     0x274
#define GUI_ID_IMAGE5     0x275
#define GUI_ID_IMAGE6     0x276
#define GUI_ID_IMAGE7     0x277
#define GUI_ID_IMAGE8     0x278
#define GUI_ID_IMAGE9     0x279

#define GUI_ID_SPINBOX0   0x280
#define GUI_ID_SPINBOX1   0x281
#define GUI_ID_SPINBOX2   0x282
#define GUI_ID_SPINBOX3   0x283
#define GUI_ID_SPINBOX4   0x284
#define GUI_ID_SPINBOX5   0x285
#define GUI_ID_SPINBOX6   0x286
#define GUI_ID_SPINBOX7   0x287
#define GUI_ID_SPINBOX8   0x288
#define GUI_ID_SPINBOX9   0x289

#define GUI_ID_CALENDAR0  0x290

#define GUI_ID_KNOB0      0x300
#define GUI_ID_KNOB1      0x301
#define GUI_ID_KNOB2      0x302
#define GUI_ID_KNOB3      0x303
#define GUI_ID_KNOB4      0x304
#define GUI_ID_KNOB5      0x305
#define GUI_ID_KNOB6      0x306
#define GUI_ID_KNOB7      0x307
#define GUI_ID_KNOB8      0x308
#define GUI_ID_KNOB9      0x309

#define GUI_ID_SWIPELIST0 0x320
#define GUI_ID_SWIPELIST1 0x321
#define GUI_ID_SWIPELIST2 0x322

#define GUI_ID_USER       0x800

/*********************************************************************
*
*       Mouse buttons
*/
#define GUI_LBUTTON (1 << 0) /* Left button */
#define GUI_RBUTTON (1 << 1) /* Right button */
#define GUI_MBUTTON (1 << 2) /* Middle button */
#define GUI_DBUTTON (1 << 7) /* Double-click button */

/*********************************************************************
*
*       Text styles
*/
#define GUI_TS_NORMAL           (0)
#define GUI_TS_UNDERLINE        (1 << 0)
#define GUI_TS_STRIKETHRU       (1 << 1)
#define GUI_TS_OVERLINE         (1 << 2)

/*********************************************************************
*
*       Line styles
*/
#define GUI_LS_SOLID        (0)
#define GUI_LS_DASH         (1)
#define GUI_LS_DOT          (2)
#define GUI_LS_DASHDOT      (3)
#define GUI_LS_DASHDOTDOT   (4)

/*********************************************************************
*
*       Pen shapes
*/
#define GUI_PS_ROUND        (0)
#define GUI_PS_FLAT         (1)
#define GUI_PS_SQUARE       (2)

/*********************************************************************
*
*       Standard colors
*/
#if (GUI_USE_ARGB)
  #define GUI_BLUE          0xFF0000FF
  #define GUI_GREEN         0xFF00FF00
  #define GUI_RED           0xFFFF0000
  #define GUI_CYAN          0xFF00FFFF
  #define GUI_MAGENTA       0xFFFF00FF
  #define GUI_YELLOW        0xFFFFFF00
  #define GUI_LIGHTBLUE     0xFF8080FF
  #define GUI_LIGHTGREEN    0xFF80FF80
  #define GUI_LIGHTRED      0xFFFF8080
  #define GUI_LIGHTCYAN     0xFF80FFFF
  #define GUI_LIGHTMAGENTA  0xFFFF80FF
  #define GUI_LIGHTYELLOW   0xFFFFFF80
  #define GUI_DARKBLUE      0xFF000080
  #define GUI_DARKGREEN     0xFF008000
  #define GUI_DARKRED       0xFF800000
  #define GUI_DARKCYAN      0xFF008080
  #define GUI_DARKMAGENTA   0xFF800080
  #define GUI_DARKYELLOW    0xFF808000
  #define GUI_WHITE         0xFFFFFFFF
  #define GUI_LIGHTGRAY     0xFFD3D3D3
  #define GUI_GRAY          0xFF808080
  #define GUI_DARKGRAY      0xFF404040
  #define GUI_BLACK         0xFF000000
  #define GUI_BROWN         0xFFA52A2A
  #define GUI_ORANGE        0xFFFFA500
  #define GUI_TRANSPARENT   0x00000000

  #define GUI_GRAY_3F       0xFF3F3F3F
  #define GUI_GRAY_50       0xFF505050
  #define GUI_GRAY_55       0xFF555555
  #define GUI_GRAY_60       0xFF606060
  #define GUI_GRAY_7C       0xFF7C7C7C
  #define GUI_GRAY_9A       0xFF9A9A9A
  #define GUI_GRAY_AA       0xFFAAAAAA
  #define GUI_GRAY_C0       0xFFC0C0C0
  #define GUI_GRAY_C8       0xFFC8C8C8
  #define GUI_GRAY_D0       0xFFD0D0D0
  #define GUI_GRAY_E7       0xFFE7E7E7
  #define GUI_BLUE_98       0xFF000098
#else
  #define GUI_BLUE          0x00FF0000
  #define GUI_GREEN         0x0000FF00
  #define GUI_RED           0x000000FF
  #define GUI_CYAN          0x00FFFF00
  #define GUI_MAGENTA       0x00FF00FF
  #define GUI_YELLOW        0x0000FFFF
  #define GUI_LIGHTBLUE     0x00FF8080
  #define GUI_LIGHTGREEN    0x0080FF80
  #define GUI_LIGHTRED      0x008080FF
  #define GUI_LIGHTCYAN     0x00FFFF80
  #define GUI_LIGHTMAGENTA  0x00FF80FF
  #define GUI_LIGHTYELLOW   0x0080FFFF
  #define GUI_DARKBLUE      0x00800000
  #define GUI_DARKGREEN     0x00008000
  #define GUI_DARKRED       0x00000080
  #define GUI_DARKCYAN      0x00808000
  #define GUI_DARKMAGENTA   0x00800080
  #define GUI_DARKYELLOW    0x00008080
  #define GUI_WHITE         0x00FFFFFF
  #define GUI_LIGHTGRAY     0x00D3D3D3
  #define GUI_GRAY          0x00808080
  #define GUI_DARKGRAY      0x00404040
  #define GUI_BLACK         0x00000000
  #define GUI_BROWN         0x002A2AA5
  #define GUI_ORANGE        0x0000A5FF
  #define GUI_TRANSPARENT   0xFF000000

  #define GUI_GRAY_3F       0x003F3F3F
  #define GUI_GRAY_50       0x00505050
  #define GUI_GRAY_55       0x00555555
  #define GUI_GRAY_60       0x00606060
  #define GUI_GRAY_7C       0x007C7C7C
  #define GUI_GRAY_9A       0x009A9A9A
  #define GUI_GRAY_AA       0x00AAAAAA
  #define GUI_GRAY_C0       0x00C0C0C0
  #define GUI_GRAY_C8       0x00C8C8C8
  #define GUI_GRAY_D0       0x00D0D0D0
  #define GUI_GRAY_E7       0x00E7E7E7
  #define GUI_BLUE_98       0x00980000
#endif

#define GUI_INVALID_COLOR 0xFFFFFFF      /* Invalid color - more than 24 bits */

#if (GUI_USE_ARGB)
  #define GUI_MAKE_COLOR(ABGR)  (((((U32)ABGR) & 0xFF000000ul) ^ 0xFF000000ul) | ((((U32)ABGR) & 0x00FF0000ul) >> 16) | (((U32)ABGR) & 0x0000FF00ul) | ((((U32)ABGR) & 0x000000FFul) << 16))
  #define GUI_MAKE_TRANS(Alpha) (255 - (Alpha))
#else
  #define GUI_MAKE_COLOR(ABGR)  (ABGR)
  #define GUI_MAKE_TRANS(Alpha) (Alpha)
#endif

#if (GUI_USE_ARGB)
  #define GUI_TRANS_BYTE 0x00
#else
  #define GUI_TRANS_BYTE 0xFF
#endif


/*********************************************************************
*
*       MultiTouch-Support
*/
//
// Defines
//
#define GUI_MTOUCH_FLAG_DOWN (1 << 0)
#define GUI_MTOUCH_FLAG_MOVE (1 << 1)
#define GUI_MTOUCH_FLAG_UP   (1 << 2)

//
// Variables
//
extern T_GUI_MTOUCH_STOREEVENT GUI_MTOUCH__pStoreEvent;

//
// Interface
//
void GUI_MTOUCH_Enable          (int OnOff);
int  GUI_MTOUCH_GetEvent        (GUI_MTOUCH_EVENT * pEvent);
int  GUI_MTOUCH_GetTouchInput   (GUI_MTOUCH_EVENT * pEvent, GUI_MTOUCH_INPUT * pBuffer, unsigned Index);
int  GUI_MTOUCH_IsEmpty         (void);
void GUI_MTOUCH_SetOrientation  (int Orientation);
void GUI_MTOUCH_SetOrientationEx(int Orientation, int LayerIndex);
void GUI_MTOUCH_StoreEvent      (GUI_MTOUCH_EVENT * pEvent, GUI_MTOUCH_INPUT * pInput);

/*********************************************************************
*
*       Coordinates used in touch driver
*/
#define GUI_COORD_X 0
#define GUI_COORD_Y 1

/*********************************************************************
*
*       Addresses of standard fonts
*/
//
// Proportional fonts
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8_ASCII,        GUI_Font8_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font10S_ASCII,      GUI_Font10S_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font10_ASCII,       GUI_Font10_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13_ASCII,       GUI_Font13_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13B_ASCII,      GUI_Font13B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13H_ASCII,      GUI_Font13H_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13HB_ASCII,     GUI_Font13HB_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font16_ASCII,       GUI_Font16_1,       GUI_Font16_HK,    GUI_Font16_1HK;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font16B_ASCII,      GUI_Font16B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font20_ASCII,       GUI_Font20_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font20B_ASCII,      GUI_Font20B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font24_ASCII,       GUI_Font24_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font24B_ASCII,      GUI_Font24B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font32_ASCII,       GUI_Font32_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font32B_ASCII,      GUI_Font32B_1;

//
// Proportional fonts, framed
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font20F_ASCII;

//
// Monospaced
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font4x6;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font6x8,            GUI_Font6x9;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font6x8_ASCII,      GUI_Font6x8_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x8,            GUI_Font8x9;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x8_ASCII,      GUI_Font8x8_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x10_ASCII;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x12_ASCII;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x13_ASCII,     GUI_Font8x13_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x15B_ASCII,    GUI_Font8x15B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x16,           GUI_Font8x17,       GUI_Font8x18;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x16x1x2,       GUI_Font8x16x2x2,   GUI_Font8x16x3x3;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x16_ASCII,     GUI_Font8x16_1;

//
// Digits
//
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD24x32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD36x48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD48x64;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD64;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD60x80;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD80;

//
// Comic fonts
//
extern GUI_CONST_STORAGE GUI_FONT GUI_FontComic18B_ASCII, GUI_FontComic18B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontComic24B_ASCII, GUI_FontComic24B_1;

/*********************************************************************
*
*       Macros to be used
*/
//
// Proportional fonts
//
#define GUI_FONT_8_ASCII        &GUI_Font8_ASCII
#define GUI_FONT_8_1            &GUI_Font8_1
#define GUI_FONT_10S_ASCII      &GUI_Font10S_ASCII
#define GUI_FONT_10S_1          &GUI_Font10S_1
#define GUI_FONT_10_ASCII       &GUI_Font10_ASCII
#define GUI_FONT_10_1           &GUI_Font10_1
#define GUI_FONT_13_ASCII       &GUI_Font13_ASCII
#define GUI_FONT_13_1           &GUI_Font13_1
#define GUI_FONT_13B_ASCII      &GUI_Font13B_ASCII
#define GUI_FONT_13B_1          &GUI_Font13B_1
#define GUI_FONT_13H_ASCII      &GUI_Font13H_ASCII
#define GUI_FONT_13H_1          &GUI_Font13H_1
#define GUI_FONT_13HB_ASCII     &GUI_Font13HB_ASCII
#define GUI_FONT_13HB_1         &GUI_Font13HB_1
#define GUI_FONT_16_ASCII       &GUI_Font16_ASCII
#define GUI_FONT_16_1           &GUI_Font16_1
#define GUI_FONT_16_HK          &GUI_Font16_HK
#define GUI_FONT_16_1HK         &GUI_Font16_1HK
#define GUI_FONT_16B_ASCII      &GUI_Font16B_ASCII
#define GUI_FONT_16B_1          &GUI_Font16B_1
#define GUI_FONT_20_ASCII       &GUI_Font20_ASCII
#define GUI_FONT_20_1           &GUI_Font20_1
#define GUI_FONT_20B_ASCII      &GUI_Font20B_ASCII
#define GUI_FONT_20B_1          &GUI_Font20B_1
#define GUI_FONT_24_ASCII       &GUI_Font24_ASCII
#define GUI_FONT_24_1           &GUI_Font24_1
#define GUI_FONT_24B_ASCII      &GUI_Font24B_ASCII
#define GUI_FONT_24B_1          &GUI_Font24B_1
#define GUI_FONT_32_ASCII       &GUI_Font32_ASCII
#define GUI_FONT_32_1           &GUI_Font32_1
#define GUI_FONT_32B_ASCII      &GUI_Font32B_ASCII
#define GUI_FONT_32B_1          &GUI_Font32B_1

//
// Proportional fonts, framed
//
#define GUI_FONT_20F_ASCII      &GUI_Font20F_ASCII

//
// Monospaced
//
#define GUI_FONT_4X6            &GUI_Font4x6
#define GUI_FONT_6X8            &GUI_Font6x8
#define GUI_FONT_6X8_ASCII      &GUI_Font6x8_ASCII
#define GUI_FONT_6X8_1          &GUI_Font6x8_1
#define GUI_FONT_6X9            &GUI_Font6x9
#define GUI_FONT_8X8            &GUI_Font8x8
#define GUI_FONT_8X8_ASCII      &GUI_Font8x8_ASCII
#define GUI_FONT_8X8_1          &GUI_Font8x8_1
#define GUI_FONT_8X9            &GUI_Font8x9
#define GUI_FONT_8X10_ASCII     &GUI_Font8x10_ASCII
#define GUI_FONT_8X12_ASCII     &GUI_Font8x12_ASCII
#define GUI_FONT_8X13_ASCII     &GUI_Font8x13_ASCII
#define GUI_FONT_8X13_1         &GUI_Font8x13_1
#define GUI_FONT_8X15B_ASCII    &GUI_Font8x15B_ASCII
#define GUI_FONT_8X15B_1        &GUI_Font8x15B_1
#define GUI_FONT_8X16           &GUI_Font8x16
#define GUI_FONT_8X17           &GUI_Font8x17
#define GUI_FONT_8X18           &GUI_Font8x18
#define GUI_FONT_8X16X1X2       &GUI_Font8x16x1x2
#define GUI_FONT_8X16X2X2       &GUI_Font8x16x2x2
#define GUI_FONT_8X16X3X3       &GUI_Font8x16x3x3
#define GUI_FONT_8X16_ASCII     &GUI_Font8x16_ASCII
#define GUI_FONT_8X16_1         &GUI_Font8x16_1

//
// Digits
//
#define GUI_FONT_D24X32         &GUI_FontD24x32
#define GUI_FONT_D32            &GUI_FontD32
#define GUI_FONT_D36X48         &GUI_FontD36x48
#define GUI_FONT_D48            &GUI_FontD48
#define GUI_FONT_D48X64         &GUI_FontD48x64
#define GUI_FONT_D64            &GUI_FontD64
#define GUI_FONT_D60X80         &GUI_FontD60x80
#define GUI_FONT_D80            &GUI_FontD80

//
// Comic fonts
//
#define GUI_FONT_COMIC18B_ASCII &GUI_FontComic18B_ASCII
#define GUI_FONT_COMIC18B_1     &GUI_FontComic18B_1
#define GUI_FONT_COMIC24B_ASCII &GUI_FontComic24B_ASCII
#define GUI_FONT_COMIC24B_1     &GUI_FontComic24B_1

/*********************************************************************
*
*       Text and drawing modes
*
* These defines come in two flavors: the long version (.._DRAWMODE_..)
* and the short ones (.._DM_..). They are identical, feel free to use
* which ever one you like best.
*/
#define GUI_DRAWMODE_NORMAL  LCD_DRAWMODE_NORMAL
#define GUI_DRAWMODE_XOR     LCD_DRAWMODE_XOR
#define GUI_DRAWMODE_TRANS   LCD_DRAWMODE_TRANS
#define GUI_DRAWMODE_REV     LCD_DRAWMODE_REV
#define GUI_DM_NORMAL        LCD_DRAWMODE_NORMAL
#define GUI_DM_XOR           LCD_DRAWMODE_XOR
#define GUI_DM_TRANS         LCD_DRAWMODE_TRANS
#define GUI_DM_REV           LCD_DRAWMODE_REV

#define GUI_TEXTMODE_NORMAL  LCD_DRAWMODE_NORMAL
#define GUI_TEXTMODE_XOR     LCD_DRAWMODE_XOR
#define GUI_TEXTMODE_TRANS   LCD_DRAWMODE_TRANS
#define GUI_TEXTMODE_REV     LCD_DRAWMODE_REV
#define GUI_TM_NORMAL        LCD_DRAWMODE_NORMAL
#define GUI_TM_XOR           LCD_DRAWMODE_XOR
#define GUI_TM_TRANS         LCD_DRAWMODE_TRANS
#define GUI_TM_REV           LCD_DRAWMODE_REV

/* Text alignment flags, horizontal */
#define GUI_TA_HORIZONTAL  (3<<0)
#define GUI_TA_LEFT        (0<<0)
#define GUI_TA_RIGHT	     (1<<0)
#define GUI_TA_CENTER	     (2<<0)
#define GUI_TA_HCENTER	   GUI_TA_CENTER  /* easier to remember :-)  */

/* Text alignment flags, vertical */
#define GUI_TA_VERTICAL   (3<<2)
#define GUI_TA_TOP	      (0<<2)
#define GUI_TA_BOTTOM	    (1<<2)
#define GUI_TA_BASELINE   (2<<2)
#define GUI_TA_VCENTER    (3<<2)

/* General orientation flags */
#define GUI_MIRROR_X (1 << 0)
#define GUI_MIRROR_Y (1 << 1)
#define GUI_SWAP_XY  (1 << 2)

/*********************************************************************
*
*       Min/Max coordinates
*
* Define minimum and maximum coordinates in x and y
*/
#if 1
  #define GUI_XMIN -16383
  #define GUI_XMAX  16383
  #define GUI_YMIN -16383
  #define GUI_YMAX  16383
#else
  #define GUI_XMIN -4095
  #define GUI_XMAX  4095
  #define GUI_YMIN -4095
  #define GUI_YMAX  4095
#endif

/*********************************************************************
*
*       Defines for constants
*/
#define	________	0x0
#define	_______X	0x1
#define	______X_	0x2
#define	______XX	0x3
#define	_____X__	0x4
#define	_____X_X	0x5
#define	_____XX_	0x6
#define	_____XXX	0x7
#define	____X___	0x8
#define	____X__X	0x9
#define	____X_X_	0xa
#define	____X_XX	0xb
#define	____XX__	0xc
#define	____XX_X	0xd
#define	____XXX_	0xe
#define	____XXXX	0xf
#define	___X____	0x10
#define	___X___X	0x11
#define	___X__X_	0x12
#define	___X__XX	0x13
#define	___X_X__	0x14
#define	___X_X_X	0x15
#define	___X_XX_	0x16
#define	___X_XXX	0x17
#define	___XX___	0x18
#define	___XX__X	0x19
#define	___XX_X_	0x1a
#define	___XX_XX	0x1b
#define	___XXX__	0x1c
#define	___XXX_X	0x1d
#define	___XXXX_	0x1e
#define	___XXXXX	0x1f
#define	__X_____	0x20
#define	__X____X	0x21
#define	__X___X_	0x22
#define	__X___XX	0x23
#define	__X__X__	0x24
#define	__X__X_X	0x25
#define	__X__XX_	0x26
#define	__X__XXX	0x27
#define	__X_X___	0x28
#define	__X_X__X	0x29
#define	__X_X_X_	0x2a
#define	__X_X_XX	0x2b
#define	__X_XX__	0x2c
#define	__X_XX_X	0x2d
#define	__X_XXX_	0x2e
#define	__X_XXXX	0x2f
#define	__XX____	0x30
#define	__XX___X	0x31
#define	__XX__X_	0x32
#define	__XX__XX	0x33
#define	__XX_X__	0x34
#define	__XX_X_X	0x35
#define	__XX_XX_	0x36
#define	__XX_XXX	0x37
#define	__XXX___	0x38
#define	__XXX__X	0x39
#define	__XXX_X_	0x3a
#define	__XXX_XX	0x3b
#define	__XXXX__	0x3c
#define	__XXXX_X	0x3d
#define	__XXXXX_	0x3e
#define	__XXXXXX	0x3f
#define	_X______	0x40
#define	_X_____X	0x41
#define	_X____X_	0x42
#define	_X____XX	0x43
#define	_X___X__	0x44
#define	_X___X_X	0x45
#define	_X___XX_	0x46
#define	_X___XXX	0x47
#define	_X__X___	0x48
#define	_X__X__X	0x49
#define	_X__X_X_	0x4a
#define	_X__X_XX	0x4b
#define	_X__XX__	0x4c
#define	_X__XX_X	0x4d
#define	_X__XXX_	0x4e
#define	_X__XXXX	0x4f
#define	_X_X____	0x50
#define	_X_X___X	0x51
#define	_X_X__X_	0x52
#define	_X_X__XX	0x53
#define	_X_X_X__	0x54
#define	_X_X_X_X	0x55
#define	_X_X_XX_	0x56
#define	_X_X_XXX	0x57
#define	_X_XX___	0x58
#define	_X_XX__X	0x59
#define	_X_XX_X_	0x5a
#define	_X_XX_XX	0x5b
#define	_X_XXX__	0x5c
#define	_X_XXX_X	0x5d
#define	_X_XXXX_	0x5e
#define	_X_XXXXX	0x5f
#define	_XX_____	0x60
#define	_XX____X	0x61
#define	_XX___X_	0x62
#define	_XX___XX	0x63
#define	_XX__X__	0x64
#define	_XX__X_X	0x65
#define	_XX__XX_	0x66
#define	_XX__XXX	0x67
#define	_XX_X___	0x68
#define	_XX_X__X	0x69
#define	_XX_X_X_	0x6a
#define	_XX_X_XX	0x6b
#define	_XX_XX__	0x6c
#define	_XX_XX_X	0x6d
#define	_XX_XXX_	0x6e
#define	_XX_XXXX	0x6f
#define	_XXX____	0x70
#define	_XXX___X	0x71
#define	_XXX__X_	0x72
#define	_XXX__XX	0x73
#define	_XXX_X__	0x74
#define	_XXX_X_X	0x75
#define	_XXX_XX_	0x76
#define	_XXX_XXX	0x77
#define	_XXXX___	0x78
#define	_XXXX__X	0x79
#define	_XXXX_X_	0x7a
#define	_XXXX_XX	0x7b
#define	_XXXXX__	0x7c
#define	_XXXXX_X	0x7d
#define	_XXXXXX_	0x7e
#define	_XXXXXXX	0x7f
#define	X_______	0x80
#define	X______X	0x81
#define	X_____X_	0x82
#define	X_____XX	0x83
#define	X____X__	0x84
#define	X____X_X	0x85
#define	X____XX_	0x86
#define	X____XXX	0x87
#define	X___X___	0x88
#define	X___X__X	0x89
#define	X___X_X_	0x8a
#define	X___X_XX	0x8b
#define	X___XX__	0x8c
#define	X___XX_X	0x8d
#define	X___XXX_	0x8e
#define	X___XXXX	0x8f
#define	X__X____	0x90
#define	X__X___X	0x91
#define	X__X__X_	0x92
#define	X__X__XX	0x93
#define	X__X_X__	0x94
#define	X__X_X_X	0x95
#define	X__X_XX_	0x96
#define	X__X_XXX	0x97
#define	X__XX___	0x98
#define	X__XX__X	0x99
#define	X__XX_X_	0x9a
#define X__XX_XX	0x9b
#define X__XXX__	0x9c
#define X__XXX_X	0x9d
#define	X__XXXX_	0x9e
#define	X__XXXXX	0x9f
#define	X_X_____	0xa0
#define	X_X____X	0xa1
#define	X_X___X_	0xa2
#define	X_X___XX	0xa3
#define	X_X__X__	0xa4
#define	X_X__X_X	0xa5
#define	X_X__XX_	0xa6
#define	X_X__XXX	0xa7
#define	X_X_X___	0xa8
#define	X_X_X__X	0xa9
#define	X_X_X_X_	0xaa
#define	X_X_X_XX	0xab
#define	X_X_XX__	0xac
#define	X_X_XX_X	0xad
#define	X_X_XXX_	0xae
#define	X_X_XXXX	0xaf
#define	X_XX____	0xb0
#define X_XX___X	0xb1
#define	X_XX__X_	0xb2
#define	X_XX__XX	0xb3
#define	X_XX_X__	0xb4
#define	X_XX_X_X	0xb5
#define	X_XX_XX_	0xb6
#define	X_XX_XXX	0xb7
#define	X_XXX___	0xb8
#define	X_XXX__X	0xb9
#define	X_XXX_X_	0xba
#define	X_XXX_XX	0xbb
#define	X_XXXX__	0xbc
#define	X_XXXX_X	0xbd
#define	X_XXXXX_	0xbe
#define	X_XXXXXX	0xbf
#define	XX______	0xc0
#define	XX_____X	0xc1
#define	XX____X_	0xc2
#define	XX____XX	0xc3
#define	XX___X__	0xc4
#define	XX___X_X	0xc5
#define	XX___XX_	0xc6
#define	XX___XXX	0xc7
#define	XX__X___	0xc8
#define	XX__X__X	0xc9
#define	XX__X_X_	0xca
#define	XX__X_XX	0xcb
#define	XX__XX__	0xcc
#define	XX__XX_X	0xcd
#define	XX__XXX_	0xce
#define XX__XXXX	0xcf
#define	XX_X____	0xd0
#define	XX_X___X	0xd1
#define	XX_X__X_	0xd2
#define	XX_X__XX	0xd3
#define	XX_X_X__	0xd4
#define	XX_X_X_X	0xd5
#define	XX_X_XX_	0xd6
#define	XX_X_XXX	0xd7
#define	XX_XX___	0xd8
#define	XX_XX__X	0xd9
#define	XX_XX_X_	0xda
#define	XX_XX_XX	0xdb
#define	XX_XXX__	0xdc
#define	XX_XXX_X	0xdd
#define	XX_XXXX_	0xde
#define	XX_XXXXX	0xdf
#define	XXX_____	0xe0
#define	XXX____X	0xe1
#define	XXX___X_	0xe2
#define	XXX___XX	0xe3
#define	XXX__X__	0xe4
#define	XXX__X_X	0xe5
#define	XXX__XX_	0xe6
#define	XXX__XXX	0xe7
#define	XXX_X___	0xe8
#define	XXX_X__X	0xe9
#define	XXX_X_X_	0xea
#define	XXX_X_XX	0xeb
#define	XXX_XX__	0xec
#define	XXX_XX_X	0xed
#define	XXX_XXX_	0xee
#define	XXX_XXXX	0xef
#define	XXXX____	0xf0
#define	XXXX___X	0xf1
#define	XXXX__X_	0xf2
#define	XXXX__XX	0xf3
#define	XXXX_X__	0xf4
#define	XXXX_X_X	0xf5
#define	XXXX_XX_	0xf6
#define	XXXX_XXX	0xf7
#define	XXXXX___	0xf8
#define	XXXXX__X	0xf9
#define	XXXXX_X_	0xfa
#define	XXXXX_XX	0xfb
#define	XXXXXX__	0xfc
#define	XXXXXX_X	0xfd
#define	XXXXXXX_	0xfe
#define	XXXXXXXX	0xff

/*********************************************************************
*
*       Compatibility with older versions
*/
#define GUI_DispString_UC      GUI_UC_DispString
#define TOUCH_X_ActivateX      GUI_TOUCH_X_ActivateX
#define TOUCH_X_ActivateY      GUI_TOUCH_X_ActivateY
#define TOUCH_X_Disable        GUI_TOUCH_X_Disable
#define TOUCH_X_MeasureX       GUI_TOUCH_X_MeasureX
#define TOUCH_X_MeasureY       GUI_TOUCH_X_MeasureY
#define GUI_SelLayer           GUI_SelectLayer
#define GUI_MEMDEV_FadeDevices GUI_MEMDEV_FadeInDevices
#if defined(__cplusplus)
}
#endif

#endif   /* ifdef GUI_H */

/*************************** End of file ****************************/

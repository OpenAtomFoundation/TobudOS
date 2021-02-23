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
File        : GUI_Type.h
Purpose     : Include file define the types used for GUI
---------------------------END-OF-HEADER------------------------------

Attention : Do not modify this file ! If you do, you will not
            be able do update to a later GUI version !

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
  
#ifndef  GUITYPE_H_INCLUDED
#define  GUITYPE_H_INCLUDED

#include "LCD.h"
#include "GUIConf.h"

/*********************************************************************
*
*       Common types
*/
typedef const char *  GUI_ConstString;

typedef LCD_COLOR       GUI_COLOR;
typedef LCD_LOGPALETTE  GUI_LOGPALETTE;
typedef LCD_DRAWMODE    GUI_DRAWMODE;
typedef LCD_RECT        GUI_RECT;

typedef struct {
  void      (* pfDraw)  (int x0,
                         int y0,
                         int xsize, 
                         int ysize, 
                         const U8 * pPixel, 
                         const LCD_LOGPALETTE * pLogPal, 
                         int xMag, 
                         int yMag);
  GUI_COLOR (* pfIndex2Color)(unsigned Index);
  void      (* pfDrawHW)(int x0,
                         int y0,
                         int xsize, 
                         int ysize, 
                         const U8 * pPixel, 
                         const LCD_LOGPALETTE * pLogPal, 
                         int xMag, 
                         int yMag);
  const LCD_API_COLOR_CONV * pColorConvAPI;
} GUI_BITMAP_METHODS;

typedef struct {
  U16P XSize;
  U16P YSize;
  U16P BytesPerLine;
  U16P BitsPerPixel;
  const U8 * pData;
  const GUI_LOGPALETTE * pPal;
  const GUI_BITMAP_METHODS * pMethods;
} GUI_BITMAP;

/* This structure may not be changed because the data that it
   expects is read in binary form (via any kind of interface,
   at runtime).
   This structure should therefor not be changed.
*/
typedef struct {
  U16 ID;
  U16 Format;
  U16 XSize;
  U16 YSize;
  U16 BytesPerLine;
  U16 BitsPerPixel;
  U16 NumColors;
  U16 HasTrans;
} GUI_BITMAP_STREAM;

typedef struct {
  int    Cmd;
  U32    v;
  void * p;
} GUI_BITMAPSTREAM_PARAM;

typedef struct {
  int XSize;
  int YSize;
  int BitsPerPixel;
  int NumColors;
  int HasTrans;
} GUI_BITMAPSTREAM_INFO;

typedef void * (* GUI_BITMAPSTREAM_CALLBACK)(GUI_BITMAPSTREAM_PARAM * pParam);

typedef struct {
  int x,y;
  U8  Pressed;
  U8  Layer;
} GUI_PID_STATE;

typedef struct {
  int Key;
  int Pressed;
} GUI_KEY_STATE;

typedef struct {
  int xPos;
  int yPos;
  int xSize;
  int ySize;
  int Delay;
} GUI_GIF_IMAGE_INFO;

typedef struct {
  int xSize;
  int ySize;
  int NumImages;
} GUI_GIF_INFO;

typedef struct GUI_REGISTER_EXIT GUI_REGISTER_EXIT;

struct GUI_REGISTER_EXIT {
  void (* pfVoid)(void);
  GUI_REGISTER_EXIT * pNext;
};

typedef struct {
  void (* cbBegin)(void);
  void (* cbEnd)  (void);
} GUI_MULTIBUF_API;

typedef struct {
  void (* cbBeginEx)(int LayerIndex);
  void (* cbEndEx)  (int LayerIndex);
} GUI_MULTIBUF_API_EX;

/*********************************************************************
*
*       FONT structures
*/

/* Translation list. Translates a character code into up to 2
   indices of images to display on top of each other;
   'á' -> index('a'), index('´') */
typedef struct {
  I16P c0;
  I16P c1;
} GUI_FONT_TRANSLIST;

typedef struct {
  U16P FirstChar;
  U16P LastChar;
  const GUI_FONT_TRANSLIST * pList;
} GUI_FONT_TRANSINFO;

typedef struct {
  U8 XSize;
  U8 XDist;
  U8 BytesPerLine;
  const unsigned char * pData;
} GUI_CHARINFO;

typedef struct {
  U8 XSize;
  U8 YSize;
  I8 XPos;
  I8 YPos;
  U8 XDist;
  const unsigned char * pData;
} GUI_CHARINFO_EXT;

typedef struct GUI_FONT_PROP {
  U16P First;                                  /* First character               */
  U16P Last;                                   /* Last character                */
  const GUI_CHARINFO         * paCharInfo;     /* Address of first character    */
  const struct GUI_FONT_PROP * pNext;          /* Pointer to next               */
} GUI_FONT_PROP;

typedef struct GUI_FONT_PROP_EXT {
  U16P First;                                  /* First character               */
  U16P Last;                                   /* Last character                */
  const GUI_CHARINFO_EXT         * paCharInfo; /* Address of first character    */
  const struct GUI_FONT_PROP_EXT * pNext;      /* Pointer to next               */
} GUI_FONT_PROP_EXT;

typedef struct {
  const unsigned char      * pData;
  const U8                 * pTransData;
  const GUI_FONT_TRANSINFO * pTrans;
  U16P                       FirstChar;
  U16P                       LastChar;
  U8                         XSize;
  U8                         XDist;
  U8                         BytesPerLine;
} GUI_FONT_MONO;

/*********************************************************************
*
*       FONT structures
*
* This structure is used when retrieving information about a font.
* It is designed for future expansion without incompatibilities.
*/
typedef struct {
  U16 Flags;
  U8 Baseline;
  U8 LHeight;     /* height of a small lower case character (a,x) */
  U8 CHeight;     /* height of a small upper case character (A,X) */
} GUI_FONTINFO;

#define GUI_FONTINFO_FLAG_PROP    (1 << 0)    /* Is proportional */
#define GUI_FONTINFO_FLAG_MONO    (1 << 1)    /* Is monospaced */
#define GUI_FONTINFO_FLAG_AA      (1 << 2)    /* Is an antialiased font */
#define GUI_FONTINFO_FLAG_AA2     (1 << 3)    /* Is an antialiased font, 2bpp */
#define GUI_FONTINFO_FLAG_AA4     (1 << 4)    /* Is an antialiased font, 4bpp */
#define GUI_FONTINFO_FLAG_PROPFRM (1 << 5)    /* Is proportional, framed */

/*********************************************************************
*
*       UNICODE Encoding
*/
typedef U16  tGUI_GetCharCode   (const char * s);
typedef int  tGUI_GetCharSize   (const char * s);
typedef int  tGUI_CalcSizeOfChar(U16 Char);
typedef int  tGUI_Encode        (char * s, U16 Char);

typedef struct {
  tGUI_GetCharCode    * pfGetCharCode;
  tGUI_GetCharSize    * pfGetCharSize;
  tGUI_CalcSizeOfChar * pfCalcSizeOfChar;
  tGUI_Encode         * pfEncode;
} GUI_UC_ENC_APILIST;

/*********************************************************************
*
*       FONT Encoding
*/
typedef int  tGUI_GetLineDistX(const char * s, int Len);
typedef int  tGUI_GetLineLen  (const char * s, int MaxLen);
typedef void tGL_DispLine     (const char * s, int Len);

typedef struct {
  tGUI_GetLineDistX * pfGetLineDistX;
  tGUI_GetLineLen   * pfGetLineLen;
  tGL_DispLine      * pfDispLine;
} tGUI_ENC_APIList;

extern const tGUI_ENC_APIList GUI_ENC_APIList_SJIS;
extern const tGUI_ENC_APIList GUI_ENC_APIList_EXT;

/*********************************************************************
*
*       FONT methods
*/
typedef struct GUI_FONT GUI_FONT;

typedef void GUI_DISPCHAR    (U16 c);
typedef int  GUI_GETCHARDISTX(U16P c, int * pSizeX);
typedef void GUI_GETFONTINFO (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
typedef char GUI_ISINFONT    (const GUI_FONT * pFont, U16 c);
typedef int  GUI_GETCHARINFO (U16P c, GUI_CHARINFO_EXT * pInfo);

#define DECLARE_FONT(Type)                                     \
void GUI##Type##_DispChar    (U16P c);                         \
int  GUI##Type##_GetCharDistX(U16P c, int * pSizeX);                         \
void GUI##Type##_GetFontInfo (const GUI_FONT * pFont, GUI_FONTINFO * pfi); \
char GUI##Type##_IsInFont    (const GUI_FONT * pFont, U16 c); \
int  GUI##Type##_GetCharInfo (U16P c, GUI_CHARINFO_EXT * pInfo)

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

DECLARE_FONT(MONO);
DECLARE_FONT(PROP);
DECLARE_FONT(PROP_EXT);
DECLARE_FONT(PROP_FRM);
DECLARE_FONT(PROPAA);
DECLARE_FONT(PROP_AA2);
DECLARE_FONT(PROP_AA2_EXT);
DECLARE_FONT(PROP_AA4);
DECLARE_FONT(PROP_AA4_EXT);

/* MONO: Monospaced fonts */
#define GUI_FONTTYPE_MONO       \
  GUIMONO_DispChar,             \
  GUIMONO_GetCharDistX,         \
  GUIMONO_GetFontInfo,          \
  GUIMONO_IsInFont,             \
  (GUI_GETCHARINFO *)0,         \
  (tGUI_ENC_APIList*)0

/* PROP: Proportional fonts */
#define GUI_FONTTYPE_PROP       \
  GUIPROP_DispChar,             \
  GUIPROP_GetCharDistX,         \
  GUIPROP_GetFontInfo,          \
  GUIPROP_IsInFont,             \
  (GUI_GETCHARINFO *)0,         \
  (tGUI_ENC_APIList*)0

/* PROP_EXT: Extended proportional fonts */
#define GUI_FONTTYPE_PROP_EXT       \
  GUIPROP_EXT_DispChar,             \
  GUIPROP_EXT_GetCharDistX,         \
  GUIPROP_EXT_GetFontInfo,          \
  GUIPROP_EXT_IsInFont,             \
  GUIPROP_EXT_GetCharInfo,          \
  &GUI_ENC_APIList_EXT

/* PROP_FRM: Extended proportional fonts, framed */
#define GUI_FONTTYPE_PROP_FRM       \
  GUIPROP_FRM_DispChar,             \
  GUIPROP_FRM_GetCharDistX,         \
  GUIPROP_FRM_GetFontInfo,          \
  GUIPROP_FRM_IsInFont,             \
  (GUI_GETCHARINFO *)0,             \
  (tGUI_ENC_APIList*)0

/* PROP: Proportional fonts SJIS */
#define GUI_FONTTYPE_PROP_SJIS  \
  GUIPROP_DispChar,             \
  GUIPROP_GetCharDistX,         \
  GUIPROP_GetFontInfo,          \
  GUIPROP_IsInFont,             \
  (GUI_GETCHARINFO *)0,         \
  &GUI_ENC_APIList_SJIS

/* PROPAA: Proportional, antialiased fonts */
#define GUI_FONTTYPE_PROPAA       \
  GUIPROPAA_DispChar,             \
  GUIPROPAA_GetCharDistX,         \
  GUIPROPAA_GetFontInfo,          \
  GUIPROPAA_IsInFont,             \
  (GUI_GETCHARINFO *)0,           \
  (tGUI_ENC_APIList*)0

/* PROP_AA2: Proportional, antialiased fonts, 2bpp */
#define GUI_FONTTYPE_PROP_AA2       \
  GUIPROP_AA2_DispChar,             \
  GUIPROP_AA2_GetCharDistX,         \
  GUIPROP_AA2_GetFontInfo,          \
  GUIPROP_AA2_IsInFont,             \
  (GUI_GETCHARINFO *)0,             \
  (tGUI_ENC_APIList*)0

/* PROP_AA2_EXT: Proportional, antialiased fonts, 2bpp, extended font information */
#define GUI_FONTTYPE_PROP_AA2_EXT   \
  GUIPROP_AA2_EXT_DispChar,         \
  GUIPROP_EXT_GetCharDistX,         \
  GUIPROP_EXT_GetFontInfo,          \
  GUIPROP_EXT_IsInFont,             \
  GUIPROP_EXT_GetCharInfo,          \
  &GUI_ENC_APIList_EXT

/* PROP_AA2: Proportional, antialiased fonts, 2bpp, SJIS encoding */
#define GUI_FONTTYPE_PROP_AA2_SJIS  \
  GUIPROP_AA2_DispChar,             \
  GUIPROP_AA2_GetCharDistX,         \
  GUIPROP_AA2_GetFontInfo,          \
  GUIPROP_AA2_IsInFont,             \
  (GUI_GETCHARINFO *)0,             \
  &GUI_ENC_APIList_SJIS

/* PROP_AA4: Proportional, antialiased fonts, 4bpp */
#define GUI_FONTTYPE_PROP_AA4       \
  GUIPROP_AA4_DispChar,             \
  GUIPROP_AA4_GetCharDistX,         \
  GUIPROP_AA4_GetFontInfo,          \
  GUIPROP_AA4_IsInFont,             \
  (GUI_GETCHARINFO *)0,             \
  (tGUI_ENC_APIList*)0

/* PROP_AA4_EXT: Proportional, antialiased fonts, 4bpp, extended font information */
#define GUI_FONTTYPE_PROP_AA4_EXT   \
  GUIPROP_AA4_EXT_DispChar,         \
  GUIPROP_EXT_GetCharDistX,         \
  GUIPROP_EXT_GetFontInfo,          \
  GUIPROP_EXT_IsInFont,             \
  GUIPROP_EXT_GetCharInfo,          \
  &GUI_ENC_APIList_EXT

/* PROP_AA4: Proportional, antialiased fonts, 4bpp, SJIS encoding */
#define GUI_FONTTYPE_PROP_AA4_SJIS  \
  GUIPROP_AA4_DispChar,             \
  GUIPROP_AA4_GetCharDistX,         \
  GUIPROP_AA4_GetFontInfo,          \
  GUIPROP_AA4_IsInFont,             \
  (GUI_GETCHARINFO *)0,             \
  &GUI_ENC_APIList_SJIS

#if defined(__cplusplus)
  }
#endif

struct GUI_FONT {
  GUI_DISPCHAR     * pfDispChar; 
  GUI_GETCHARDISTX * pfGetCharDistX; 
  GUI_GETFONTINFO  * pfGetFontInfo; 
  GUI_ISINFONT     * pfIsInFont;
  GUI_GETCHARINFO  * pfGetCharInfo;
  const tGUI_ENC_APIList* pafEncode;
  U8 YSize;
  U8 YDist;
  U8 XMag;
  U8 YMag;
  union {
    const void              * pFontData;
    const GUI_FONT_MONO     * pMono;
    const GUI_FONT_PROP     * pProp;
    const GUI_FONT_PROP_EXT * pPropExt;
  } p;
  U8 Baseline;
  U8 LHeight;     /* Height of a small lower case character (a,x) */
  U8 CHeight;     /* Height of a small upper case character (A,X) */
};

/*********************************************************************
*
*       Bitmap serialization (BMP)
*/
typedef void GUI_CALLBACK_VOID_U8_P(U8 Data, void * p);

/*********************************************************************
*
*       System independent font structures (SIF)
*/
typedef struct {
  U32 ID;           /* Font file ID */
  U16 YSize;        /* Height of font */
  U16 YDist;        /* Space of font Y */
  U16 Baseline;     /* Index of baseline */
  U16 LHeight;      /* Height of a small lower case character (a) */
  U16 CHeight;      /* Height of a upper case character (A) */
  U16 NumAreas;     /* Number of character areas */
} GUI_SI_FONT;

typedef struct {
  U16 First;        /* Index of first character */
  U16 Last;         /* Index of last character */
} GUI_SIF_CHAR_AREA;

typedef struct {
  U16 XSize;        /* Size of bitmap data in X */
  U16 XDist;        /* Number of pixels for increment cursor in X */
  U16 BytesPerLine; /* Number of bytes per line */
  U16 Dummy;
  U32 OffData;      /* Offset of pixel data */
} GUI_SIF_CHARINFO;

typedef struct {
  U16 XSize;        /* Size of bitmap data in X */
  U16 YSize;        /* Size of bitmap data in X */
  I16 XOff;         /* Display offset of bitmap data in X */
  I16 YOff;         /* Display offset of bitmap data in Y */
  U16 XDist;        /* Number of pixels for increment cursor in X */
  U16 Dummy;
  U32 OffData;      /* Offset of pixel data */
} GUI_SIF_CHARINFO_EXT;

typedef struct tGUI_SIF_APIList_struct {
  GUI_DISPCHAR          * pfDispChar;
  GUI_GETCHARDISTX      * pfGetCharDistX;
  GUI_GETFONTINFO       * pfGetFontInfo;
  GUI_ISINFONT          * pfIsInFont;
  GUI_GETCHARINFO       * pfGetCharInfo;
  const tGUI_ENC_APIList* pafEncode;
} tGUI_SIF_APIList;

#define GUI_SIF_TYPE              tGUI_SIF_APIList
#define GUI_SIF_TYPE_PROP         &GUI_SIF_APIList_Prop
#define GUI_SIF_TYPE_PROP_EXT     &GUI_SIF_APIList_Prop_Ext
#define GUI_SIF_TYPE_PROP_FRM     &GUI_SIF_APIList_Prop_Frm
#define GUI_SIF_TYPE_PROP_AA2     &GUI_SIF_APIList_Prop_AA2
#define GUI_SIF_TYPE_PROP_AA4     &GUI_SIF_APIList_Prop_AA4
#define GUI_SIF_TYPE_PROP_AA2_EXT &GUI_SIF_APIList_Prop_AA2_EXT
#define GUI_SIF_TYPE_PROP_AA4_EXT &GUI_SIF_APIList_Prop_AA4_EXT

/*********************************************************************
*
*       External binary font structures (XBF)
*/
typedef int GUI_XBF_GET_DATA_FUNC(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer);

typedef struct {
  U16 First;                         /* First character of font */
  U16 Last;                          /* Last character of font */
  void * pVoid;                      /* Void pointer passed to GetData-function */
  GUI_XBF_GET_DATA_FUNC * pfGetData; /* Pointer to callback function */
} GUI_XBF_DATA;

typedef struct tGUI_XBF_APIList_struct {
  GUI_DISPCHAR          * pfDispChar;
  GUI_GETCHARDISTX      * pfGetCharDistX;
  GUI_GETFONTINFO       * pfGetFontInfo;
  GUI_ISINFONT          * pfIsInFont;
  GUI_GETCHARINFO       * pfGetCharInfo;
  const tGUI_ENC_APIList* pafEncode;
} tGUI_XBF_APIList;

#define GUI_XBF_TYPE              tGUI_XBF_APIList
#define GUI_XBF_TYPE_PROP         &GUI_XBF_APIList_Prop
#define GUI_XBF_TYPE_PROP_EXT     &GUI_XBF_APIList_Prop_Ext
#define GUI_XBF_TYPE_PROP_FRM     &GUI_XBF_APIList_Prop_Frm
#define GUI_XBF_TYPE_PROP_AA2_EXT &GUI_XBF_APIList_Prop_AA2_Ext
#define GUI_XBF_TYPE_PROP_AA4_EXT &GUI_XBF_APIList_Prop_AA4_Ext

/*********************************************************************
*
*       TrueType support (TTF)
*/
typedef struct {
  const void * pData;      /* Pointer to TTF font file in addressable memory area */
  U32 NumBytes;            /* Size of file in bytes */
} GUI_TTF_DATA;

typedef struct {
  GUI_TTF_DATA * pTTF;     /* Pointer to GUI_TTF_DATA structure which contains location and size of font file */
  U32 aImageTypeBuffer[4]; /* Buffer for image type structure */
  int PixelHeight;         /* Pixel height of new font. It means the height of the surrounding rectangle
                            * between the glyphs 'g' anf 'f'. Please notice that it is not the distance
                            * between two lines of text. With other words the value returned byGUI_GetFontSizeY()
                            * is not identically with this value. */
  int FaceIndex;           /* Some font files can contain more than one font face. In case of more than one face
                            * this index specifies the zero based face index to be used to create the font. 
                            * Usually 0. */
} GUI_TTF_CS;

/*********************************************************************
*
*       Task synchronization
*/
typedef void (* GUI_SIGNAL_EVENT_FUNC)    (void);
typedef void (* GUI_WAIT_EVENT_FUNC)      (void);
typedef void (* GUI_WAIT_EVENT_TIMED_FUNC)(int Period);

/*********************************************************************
*
*       Memory management
*/
#ifndef     GUI_HMEM
  #define     GUI_HMEM        I32
#endif
#define     GUI_HMEM_NULL     (0)
typedef     GUI_HMEM      GUI_HWIN;
typedef     GUI_HMEM      GUI_HSPRITE;

/*********************************************************************
*
*       Multi touch input
*/
#ifndef   GUI_MTOUCH_MAX_NUM_POINTS
  #define GUI_MTOUCH_MAX_NUM_POINTS 10
#endif

typedef struct {
  I32 x;
  I32 y;
  U32 Id;
  U16 Flags;
} GUI_MTOUCH_INPUT;

typedef struct {
  int            LayerIndex;
  unsigned       NumPoints;
  GUI_TIMER_TIME TimeStamp;
  GUI_HMEM       hInput;
} GUI_MTOUCH_EVENT;

//
// Used for emWinSPY with reduced data types and array sizes
//
typedef struct {
  U8  Layer;
  U8  NumPoints;
  I16 ax[5];
  I16 ay[5];
  U16 aId[5];
  U8  aFlags[5];
} GUI_MTOUCH_STATE;

typedef void (* T_GUI_MTOUCH_STOREEVENT)(GUI_MTOUCH_EVENT *, GUI_MTOUCH_INPUT * pInput);

/*********************************************************************
*
*       Hardware routines
*/
typedef struct {
  //
  // 8 Bit access
  //
  void (* pfWrite8_A0)  (U8 Data);
  void (* pfWrite8_A1)  (U8 Data);
  void (* pfWriteM8_A0) (U8 * pData, int NumItems);
  void (* pfWriteM8_A1) (U8 * pData, int NumItems);
  U8   (* pfRead8_A0)   (void);
  U8   (* pfRead8_A1)   (void);
  void (* pfReadM8_A0)  (U8 * pData, int NumItems);
  void (* pfReadM8_A1)  (U8 * pData, int NumItems);
  //
  // 16 Bit access
  //
  void (* pfWrite16_A0) (U16 Data);
  void (* pfWrite16_A1) (U16 Data);
  void (* pfWriteM16_A0)(U16 * pData, int NumItems);
  void (* pfWriteM16_A1)(U16 * pData, int NumItems);
  U16  (* pfRead16_A0)  (void);
  U16  (* pfRead16_A1)  (void);
  void (* pfReadM16_A0) (U16 * pData, int NumItems);
  void (* pfReadM16_A1) (U16 * pData, int NumItems);
  //
  // 32 Bit access
  //
  void (* pfWrite32_A0) (U32 Data);
  void (* pfWrite32_A1) (U32 Data);
  void (* pfWriteM32_A0)(U32 * pData, int NumItems);
  void (* pfWriteM32_A1)(U32 * pData, int NumItems);
  U32  (* pfRead32_A0)  (void);
  U32  (* pfRead32_A1)  (void);
  void (* pfReadM32_A0) (U32 * pData, int NumItems);
  void (* pfReadM32_A1) (U32 * pData, int NumItems);
  //
  // SPI access
  //
  void (* pfSetCS)      (U8 NotActive);
  //
  // Common routines
  //
  void (* pfFlushBuffer)(void);
} GUI_PORT_API;

/*********************************************************************
*
*       Send/Receive function for VNC and/or emWinSPY
*/
typedef int    (* GUI_tSend)  (const U8 * pData, int len, void * p);
typedef int    (* GUI_tRecv)  (      U8 * pData, int len, void * p);

/*********************************************************************
*
*       Memory allocation replacement for emWinSPY
*/
typedef void * (* GUI_tMalloc)(unsigned int);
typedef void   (* GUI_tFree)  (void *);

#endif  /* GUITYPE_H_INCLUDED */

/*************************** End of file ****************************/

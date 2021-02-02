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
File        : GUI_JPEG_Private.h
Purpose     : Private header
----------------------------------------------------------------------

Explanation of terms:
  
  Component   - Color channel, e.g., Red or Luminance.
  Sample      - Single component value (i.e., one number in the image data).
  Coefficient - Frequency coefficient (a DCT transform output number).
  Block       - 8x8 group of samples or coefficients.
  MCU         - (Minimum Coded Unit) is an interleaved set of blocks of size
                determined by the sampling factors, or a single block in a
                noninterleaved scan.

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
  
#ifndef GUI_JPEG_PRIVATE_H
#define GUI_JPEG_PRIVATE_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define JPEG_LOCK_H(h) (GUI_JPEG_DCONTEXT *)GUI_LOCK_H(h)

/*********************************************************************
*
*       Scan types
*/
#define GRAYSCALE 0
#define YH1V1     1
#define YH2V1     2
#define YH1V2     3
#define YH2V2     4

/*********************************************************************
*
*       Maximum number of...
*/
#define MAX_COMPONENTS  4
#define MAX_HUFFTABLES  8
#define MAX_QUANTTABLES 4
#define MAX_COMPSINSCAN 4
#define MAX_BLOCKSPERMCU 10

/*********************************************************************
*
*       Marker definitions
*/
#define M_SOF0  0xc0 /* Start Of Frame */
#define M_SOF1  0xc1
#define M_SOF2  0xc2
#define M_SOF3  0xc3

#define M_SOF5  0xc5
#define M_SOF6  0xc6
#define M_SOF7  0xc7

#define M_JPG   0xc8
#define M_SOF9  0xc9
#define M_SOF10 0xca
#define M_SOF11 0xcb

#define M_SOF13 0xcd
#define M_SOF14 0xce
#define M_SOF15 0xcf

#define M_DHT   0xc4 /* Define Huffman Table */

#define M_DAC   0xcc /* Define Arithmetic Coding Table */

#define M_RST0  0xd0
#define M_RST1  0xd1
#define M_RST2  0xd2
#define M_RST3  0xd3
#define M_RST4  0xd4
#define M_RST5  0xd5
#define M_RST6  0xd6
#define M_RST7  0xd7

#define M_SOI   0xd8 /* Start Of Image */
#define M_EOI   0xd9
#define M_SOS   0xda /* Start Of Scan */
#define M_DQT   0xdb /* Define Quantisation Table */
#define M_DRI   0xdd /* Define Restart Interval */

#define M_APP0  0xe0 /* Application Usage */
#define M_APP14 0xee /* Adobe marker */

#define M_TEM   0x01

#define ADOBE_TRANSFORM_RGB   0
#define ADOBE_TRANSFORM_YCBCR 1

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/* Default parameter structure for reading data from memory */
typedef struct {
  const U8 * pFileData;
  I32   FileSize;
} GUI_JPEG_PARAM;

/* Huffman table definition */
typedef struct {
  unsigned aLookUp[256];
  U8       aCodeSize[256];
  unsigned aTree[512];
} HUFF_TABLE;

/* Coefficient buffer used for progressive JPEGs */
typedef struct {
  int NumBlocksX;
  int NumBlocksY;
  unsigned BlockSize;
  GUI_HMEM hData;
} COEFF_BUFFER;

typedef struct GUI_JPEG_DCONTEXT GUI_JPEG_DCONTEXT;

struct GUI_JPEG_DCONTEXT {
  /* Function pointer for drawing one line of completely decoded pixels */
  void (* pfWritePixels)(int x0, int y0, GUI_JPEG_DCONTEXT * pContext, GUI_COLOR (* pfGetColor)(const U8 ** ppData, unsigned SkipCnt), tLCDDEV_Color2Index * pfColor2Index);
  /* Function pointer for reading one byte */
  int (* pfGetU8)(GUI_JPEG_DCONTEXT * pContext, U8 * pByte);

  GUI_GET_DATA_FUNC * pfGetData; /* 'GetData' Function pointer */
  void * pParam;                 /* Pointer passed to 'GetData' function */
  U32 Off;                       /* Data pointer */
  /* Image size */
  U16 xSize;
  U16 ySize;
  /* Input buffer */
  const U8 * pBuffer;
  unsigned   NumBytesInBuffer;
  U8         StartOfFile;
  U8         aStuff[4];        /* Stuff back buffer */
  U8         NumBytesStuffed;  /* Number of stuffed bytes */
  /* Bit buffer */
  U32 BitBuffer;
  int NumBitsLeft;
  /* Huffman tables */
  U8 aHuffNumTableAvail[MAX_HUFFTABLES];
  U8 aaHuffNum[MAX_HUFFTABLES][17];   /* Pointer to number of Huffman codes per bit size */
  U8 aaHuffVal[MAX_HUFFTABLES][256];  /* Pointer to Huffman codes */
  HUFF_TABLE aHuffTable[MAX_HUFFTABLES];
  HUFF_TABLE * apDC_Huff[MAX_BLOCKSPERMCU];
  HUFF_TABLE * apAC_Huff[MAX_BLOCKSPERMCU];
  /* Quantization tables */
  U16 aaQuantTbl[MAX_QUANTTABLES][64];
  U16 * apQuantTbl[MAX_QUANTTABLES];
  /* Component information */
  U8 NumCompsPerFrame;                      /* Number of components per frame */
  U8 aCompHSamp[MAX_COMPONENTS];            /* Component's horizontal sampling factor */
  U8 aCompVSamp[MAX_COMPONENTS];            /* Component's vertical sampling factor */
  U8 aCompQuant[MAX_COMPONENTS];            /* Component's quantization table selector */
  U8 aCompId   [MAX_COMPONENTS];            /* Component's ID */
  U8 NumCompsPerScan;                       /* Number of components per scan */
  U8 aCompList[MAX_COMPSINSCAN];            /* Components in this scan */
  U8 aCompDC_Tab[MAX_COMPONENTS];           /* Component's DC Huffman coding table selector */
  U8 aCompAC_Tab[MAX_COMPONENTS];           /* Component's AC Huffman coding table selector */
  unsigned * apComponent[MAX_BLOCKSPERMCU]; /* Points into the table aLastDC_Val[] */
  unsigned   aLastDC_Val[MAX_COMPONENTS];   /* Table of last DC values */
  /* Data used for progressive scans */
  U8 SpectralStart;                        /* Spectral selection start */
  U8 SpectralEnd;                          /* Spectral selection end */
  U8 SuccessiveLow;                        /* Successive approximation low */
  U8 SuccessiveHigh;                       /* Successive approximation high */
  COEFF_BUFFER aDC_Coeffs[MAX_COMPONENTS]; /* DC coefficient buffer for progressive scan */
  COEFF_BUFFER aAC_Coeffs[MAX_COMPONENTS]; /* AC coefficient buffer for progressive scan */
  int aBlockY_MCU[MAX_COMPONENTS];         /*  */
  GUI_HMEM hBmpLine;
  /* Common */
  U8 TransformationRequired;
  U8 IsProgressive;             /* Flag is set to 1 if JPEG is progressive */
  U8 ScanType;                  /* Gray, Yh1v1, Yh1v2, Yh2v1, Yh2v2 */
  int MaxMCUsPerRow;            /* Maximum number of MCUs per row */
  int MaxMCUsPerCol;            /* Maximum number of MCUs per column */
  int MaxBlocksPerMCU;          /* Maximum number of blocks per MCU */
  int MaxBlocksPerRow;          /* Maximum number of blocks per row */
  int MaxMCU_xSize;             /* MCU's max. X size in pixels */
  int MaxMCU_ySize;             /* MCU's max. Y size in pixels */
  int DestBytesPerPixel;        /* 4 (RGB) or 1 (Y) */
  int DestBytesPerScanline;     /* Rounded up */
  int RealDestBytesPerScanline; /* Actual bytes */
  int EOB_Run;                  /* 'End Of Band' run */
  int RestartInterval;
  int RestartsLeft;
  int NextRestartNum;
  int MCUsPerRow;
  int MCUsPerCol;
  int NumBlocksPerMCU;
  int aMCU_Org[MAX_BLOCKSPERMCU];
  /* Block buffer */
  GUI_HMEM hBlocks;
  GUI_HMEM hBlockMaxZagSet;
  /* Sample buffer */
  GUI_HMEM hSampleBuf;
  U8     * pSampleBuf;
  /* Status */
  int TotalLinesLeft; /* Total number of lines left in image */
  int MCULinesLeft;   /* Total number of lines left in current MCU */
  /* Output buffer(s) */
  GUI_HMEM hScanLine0; /* Buffer 0 */
  GUI_HMEM hScanLine1; /* Buffer 1, only used for V2 sampling factors */
  U8 BufferIndex;
  /* Arrays used for converting YCbCr to RGB */
  int aCRR[256];
  int aCBB[256];
  I32 aCRG[256];
  I32 aCBG[256];
  /* Banding */
  U8 BandingRequired;        /* Flag if banding is required */
  unsigned NumBands;         /* Number of required bands for drawing the complete frame */
  int NumBlocksPerBand; /* Number of vertical blocks per band */
  int FirstBlockOfBand;
  int aFirstBlockOfBand[MAX_COMPONENTS];
  int bpp;
};

/*********************************************************************
*
*       Private interface
*
**********************************************************************
*/
int       GUI_JPEG__AllocBandingCoeffBuffer (GUI_HMEM hContext);
int       GUI_JPEG__DecodeLine              (GUI_JPEG_DCONTEXT * pContext);
int       GUI_JPEG__DecodeProgressiveBanding(GUI_JPEG_DCONTEXT * pContext);
void      GUI_JPEG__Free                    (GUI_JPEG_DCONTEXT * pContext);
GUI_COLOR GUI_JPEG__GetColorGray            (const U8 ** ppData, unsigned SkipCnt);
GUI_COLOR GUI_JPEG__GetColorRGB             (const U8 ** ppData, unsigned SkipCnt);
int       GUI_JPEG__GetData                 (void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off);
int       GUI_JPEG__InitDraw                (GUI_HMEM hContext);
int       GUI_JPEG__ReadUntilSOF            (GUI_HMEM hContext);
void      GUI_JPEG__SetNextBand             (GUI_JPEG_DCONTEXT * pContext);
int       GUI_JPEG__SkipLine                (GUI_JPEG_DCONTEXT * pContext);
int       GUI_JPEG__GetInfoEx               (GUI_HMEM hContext, GUI_JPEG_INFO * pInfo);

#endif

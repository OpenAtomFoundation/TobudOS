/*---------------------------------------------------------------------------*
 | @author  : LightningSemi WLAN Software Team                               |
 | @version : V2.0.0                                                         |
 | @date    : 1-May-2018                                                     |
 | @brief   : IEEE 802.11 a/b/g/e/i/n MAC Software                           |
 |---------------------------------------------------------------------------|
 |                       COPYRIGHT(c) 2018 LightningSemi                     |
 *---------------------------------------------------------------------------*/


/*****************************************************************************/
/*                                                                           */
/*  File Name         : itypes.h                                             */
/*                                                                           */
/*  Description       : This file contains all the data type definitions for */
/*                      MAC implementation.                                  */
/*                                                                           */
/*  List of Functions : None                                                 */
/*  Issues / Problems : None                                                 */
/*                                                                           */
/*****************************************************************************/

#ifndef ITYPES_H
#define ITYPES_H

/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/

#define INLINE static __inline

/*****************************************************************************/
/* Data Types                                                                */
/*****************************************************************************/

typedef int            WORD32;
typedef short          WORD16;
typedef char           WORD8;
typedef unsigned int   UWORD32;
typedef unsigned short UWORD16;
typedef unsigned char  UWORD8;
typedef signed char    SWORD8;

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/

typedef enum {BFALSE = 0,
              BTRUE  = 1
} BOOL_T;

#endif /* ITYPES_H */

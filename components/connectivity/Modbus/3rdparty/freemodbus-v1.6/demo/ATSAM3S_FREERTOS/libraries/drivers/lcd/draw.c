/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 *
 * Implementation of draw function on LCD, Include draw text, image
 * and basic shapes (line, rectangle, circle).
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <string.h>

#include "draw.h"
#include "font.h"
#include <board.h>
#include <utility/assert.h>
#include <ili9325/ili9325.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Fills the given LCD buffer with a particular color.
 *
 * \param color  Fill color.
 */
void LCDD_Fill(uint32_t color)
{
    uint32_t i;

    LCD_SetCursor(0, 0);
    LCD_WriteRAM_Prepare();
    for (i = 0; i < (BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT); i++) {

        LCD_WriteRAM(color);
    }
}

/**
 * \brief Draw a pixel on LCD of given color.
 *
 * \param x  X-coordinate of pixel.
 * \param y  Y-coordinate of pixel.
 * \param color  Pixel color.
 */
void LCDD_DrawPixel(
    uint32_t x,
    uint32_t y,
    uint32_t color)
{
    LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare();
    LCD_WriteRAM(color);
}

/**
 * \brief Read a pixel from LCD.
 *
 * \param x  X-coordinate of pixel.
 * \param y  Y-coordinate of pixel.
 *
 * \return color  Readed pixel color.
 */
uint32_t LCDD_ReadPixel(
    uint32_t x,
    uint32_t y)
{
    uint32_t color;

    LCD_SetCursor(x, y);
    LCD_ReadRAM_Prepare();
    color = LCD_ReadRAM();

    return color;
}

/*
 * \brief Draw a line on LCD, horizontal and vertical line are supported.
 *
 * \param x         X-coordinate of line start.
 * \param y         Y-coordinate of line start.
 * \param length    line length.
 * \param direction line direction: 0 - horizontal, 1 - vertical.
 * \param color     Pixel color.
 */
void LCDD_DrawLine(
    uint32_t x,
    uint32_t y,
    uint32_t length,
    uint32_t direction,
    uint32_t color)
{
    uint32_t i = 0;

    LCD_SetCursor(x, y);

    if(direction == DIRECTION_HLINE) {
        LCD_WriteRAM_Prepare();
        for(i = 0; i < length; i++) {
            LCD_WriteRAM(color);
        }
    }
    else {
        for(i = 0; i < length; i++) {
            LCD_WriteRAM_Prepare();
            LCD_WriteRAM(color);
            y++;
            LCD_SetCursor(x, y);
        }
    }
}

/*
 * \brief Draws a rectangle on LCD, at the given coordinates.
 *
 * \param x      X-coordinate of upper-left rectangle corner.
 * \param y      Y-coordinate of upper-left rectangle corner.
 * \param width  Rectangle width in pixels.
 * \param height  Rectangle height in pixels.
 * \param color  Rectangle color.
 */
void LCDD_DrawRectangle(
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint32_t color)
{
    LCDD_DrawLine(x, y, width, DIRECTION_HLINE, color);
    LCDD_DrawLine(x, (y + height), width, DIRECTION_HLINE, color);

    LCDD_DrawLine(x, y, height, DIRECTION_VLINE, color);
    LCDD_DrawLine((x + width), y, height, DIRECTION_VLINE, color);
}

/*
 * \brief Draws a rectangle with fill inside on LCD, at the given coordinates.
 *
 * \param x      X-coordinate of upper-left rectangle corner.
 * \param y      Y-coordinate of upper-left rectangle corner.
 * \param width  Rectangle width in pixels.
 * \param height  Rectangle height in pixels.
 * \param color  Rectangle color.
 */
void LCDD_DrawRectangleWithFill(
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint32_t color)
{
    uint32_t i;

    for (i = 0; i < height; i++) {
        LCDD_DrawLine(x, y+i, width, DIRECTION_HLINE, color);
    }
}

/**
 * \brief Draws a circle on LCD, at the given coordinates.
 *
 * \param x      X-coordinate of circle center.
 * \param y      Y-coordinate of circle center.
 * \param r      circle radius.
 * \param color  circle color.
 */
void LCDD_DrawCircle(
        uint32_t x,
        uint32_t y,
        uint32_t r,
        uint32_t color)
{
    signed int    d;    /* Decision Variable */
    uint32_t  curX; /* Current X Value */
    uint32_t  curY; /* Current Y Value */

    d = 3 - (r << 1);
    curX = 0;
    curY = r;

    while (curX <= curY)
    {
        LCDD_DrawPixel(x + curX, y + curY, color);
        LCDD_DrawPixel(x + curX, y - curY, color);
        LCDD_DrawPixel(x - curX, y + curY, color);
        LCDD_DrawPixel(x - curX, y - curY, color);
        LCDD_DrawPixel(x + curY, y + curX, color);
        LCDD_DrawPixel(x + curY, y - curX, color);
        LCDD_DrawPixel(x - curY, y + curX, color);
        LCDD_DrawPixel(x - curY, y - curX, color);

        if (d < 0) {
            d += (curX << 2) + 6;
        }
        else {
            d += ((curX - curY) << 2) + 10;
            curY--;
        }
        curX++;
    }
}

/**
 * \brief Draws a string inside a LCD buffer, at the given coordinates. Line breaks
 * will be honored.
 *
 * \param x        X-coordinate of string top-left corner.
 * \param y        Y-coordinate of string top-left corner.
 * \param pString  String to display.
 * \param color    String color.
 */
void LCDD_DrawString(
    uint32_t x,
    uint32_t y,
    const uint8_t *pString,
    uint32_t color)
{
    unsigned xorg = x;

    while (*pString != 0) {
        if (*pString == '\n') {

            y += gFont.height + 2;
            x = xorg;
        }
        else {

            LCDD_DrawChar(x, y, *pString, color);
            x += gFont.width + 2;
        }
        pString++;
    }
}

/**
 * \brief Returns the width & height in pixels that a string will occupy on the screen
 * if drawn using LCDD_DrawString.
 *
 * \param pString  String.
 * \param pWidth   Pointer for storing the string width (optional).
 * \param pHeight  Pointer for storing the string height (optional).
 *
 * \return String width in pixels.
 */
void LCDD_GetStringSize(
    const uint8_t *pString,
    uint32_t *pWidth,
    uint32_t *pHeight)
{
    uint32_t width = 0;
    uint32_t height = gFont.height;

    while (*pString != 0) {

        if (*pString == '\n') {

            height += gFont.height + 2;
        }
        else {

            width += gFont.width + 2;
        }
        pString++;
    }

    if (width > 0) width -= 2;

    if (pWidth) *pWidth = width;
    if (pHeight) *pHeight = height;
}

/*
 * \brief Draw a raw image at given position on LCD.
 *
 * \param x         X-coordinate of image start.
 * \param y         Y-coordinate of image start.
 * \param pImage    Image buffer.
 * \param width     Image width.
 * \param height    Image height.
 */
void LCDD_DrawImage(
    uint32_t x,
    uint32_t y,
    const uint8_t *pImage,
    uint32_t width,
    uint32_t height)
{
    uint32_t row, col;

    for(row = y; row < (y + height); row++) {
        LCD_SetCursor(x, row);
        LCD_WriteRAM_Prepare();
        for(col = x; col < (x + width); col++) {
            LCD_D() = *pImage++;
            LCD_D() = *pImage++;
            LCD_D() = *pImage++;
        }
    }
}


/*
Library:					SPI LCD - LCD
Written by:				Mohamed Yaqoob (MYaqoobEmbedded YouTube Channel)
Date Written:			09/12/2018
Last modified:		-/-
Description:			This is an STM32 device driver library for the LCD SPI LCD display, using STM HAL libraries
										
* Copyright (C) 2018 - M. Yaqoob
   This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
   of the GNU General Public Licenseversion 3 as published by the Free Software Foundation.
	
   This software library is shared with puplic for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
   or indirectly by this software, read more about this on the GNU General Public License.
*/

//List of includes
#include <stdbool.h>
//** CHANGE BASED ON STM32 CHIP F4/F7/F1...**//
#include "stm32f1xx_hal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//LCD dimensions defines
#define HX8357D                    0xD  ///< Our internal const for D type
#define HX8357B                    0xB  ///< Our internal const for B type

#define HX8357_TFTWIDTH            320  ///< 320 pixels wide
#define HX8357_TFTHEIGHT           480  ///< 480 pixels tall

#define LCD_PIXEL_COUNT	HX8357_TFTWIDTH*HX8357_TFTHEIGHT

#define HX8357_NOP                0x00  ///< No op
#define HX8357_SWRESET            0x01  ///< software reset
#define HX8357_RDDID              0x04  ///< Read ID
#define HX8357_RDDST              0x09  ///< (unknown)

#define HX8357_RDPOWMODE          0x0A  ///< Read power mode Read power mode
#define HX8357_RDMADCTL           0x0B  ///< Read MADCTL
#define HX8357_RDCOLMOD           0x0C  ///< Column entry mode
#define HX8357_RDDIM              0x0D  ///< Read display image mode
#define HX8357_RDDSDR             0x0F  ///< Read dosplay signal mode

#define HX8357_SLPIN              0x10  ///< Enter sleep mode
#define HX8357_SLPOUT             0x11  ///< Exit sleep mode
#define HX8357B_PTLON             0x12  ///< Partial mode on
#define HX8357B_NORON             0x13  ///< Normal mode

#define HX8357_INVOFF             0x20  ///< Turn off invert
#define HX8357_INVON              0x21  ///< Turn on invert
#define HX8357_DISPOFF            0x28  ///< Display on
#define HX8357_DISPON             0x29  ///< Display off

#define HX8357_CASET              0x2A  ///< Column addr set
#define HX8357_PASET              0x2B  ///< Page addr set
#define HX8357_RAMWR              0x2C  ///< Write VRAM
#define HX8357_RAMRD              0x2E  ///< Read VRAm

#define HX8357B_PTLAR             0x30  ///< (unknown)
#define HX8357_TEON               0x35  ///< Tear enable on
#define HX8357_TEARLINE           0x44  ///< (unknown)
#define HX8357_MADCTL             0x36  ///< Memory access control
#define HX8357_COLMOD             0x3A  ///< Color mode

#define HX8357_SETOSC             0xB0  ///< Set oscillator
#define HX8357_SETPWR1            0xB1  ///< Set power control
#define HX8357B_SETDISPLAY        0xB2  ///< Set display mode
#define HX8357_SETRGB             0xB3  ///< Set RGB interface
#define HX8357D_SETCOM            0xB6  ///< Set VCOM voltage

#define HX8357B_SETDISPMODE       0xB4  ///< Set display mode
#define HX8357D_SETCYC            0xB4  ///< Set display cycle reg
#define HX8357B_SETOTP            0xB7  ///< Set OTP memory
#define HX8357D_SETC              0xB9  ///< Enable extension command

#define HX8357B_SET_PANEL_DRIVING 0xC0  ///< Set panel drive mode
#define HX8357D_SETSTBA           0xC0  ///< Set source option
#define HX8357B_SETDGC            0xC1  ///< Set DGC settings
#define HX8357B_SETID             0xC3  ///< Set ID
#define HX8357B_SETDDB            0xC4  ///< Set DDB
#define HX8357B_SETDISPLAYFRAME   0xC5  ///< Set display frame
#define HX8357B_GAMMASET          0xC8  ///< Set Gamma correction
#define HX8357B_SETCABC           0xC9  ///< Set CABC
#define HX8357_SETPANEL           0xCC  ///< Set Panel

#define HX8357B_SETPOWER          0xD0  ///< Set power control
#define HX8357B_SETVCOM           0xD1  ///< Set VCOM
#define HX8357B_SETPWRNORMAL      0xD2  ///< Set power normal

#define HX8357B_RDID1             0xDA  ///< Read ID #1
#define HX8357B_RDID2             0xDB  ///< Read ID #2
#define HX8357B_RDID3             0xDC  ///< Read ID #3
#define HX8357B_RDID4             0xDD  ///< Read ID #4

#define HX8357D_SETGAMMA          0xE0  ///< Set Gamma

#define HX8357B_SETGAMMA          0xC8 ///< Set Gamma
#define HX8357B_SETPANELRELATED   0xE9 ///< Set panel related

#define MADCTL_MY  0x80 ///< Bottom to top
#define MADCTL_MX  0x40 ///< Right to left
#define MADCTL_MV  0x20 ///< Reverse Mode
#define MADCTL_ML  0x10 ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH  0x04 ///< LCD refresh right to left

// Plan is to move this to GFX header (with different prefix), though
// defines will be kept here for existing code that might be referencing
// them. Some additional ones are in the LCD lib -- add all in GFX!
// Color definitions
#define	HX8357_BLACK   0x0000 ///< BLACK color for drawing graphics
#define	HX8357_BLUE    0x001F ///< BLUE color for drawing graphics
#define	HX8357_RED     0xF800 ///< RED color for drawing graphics
#define	HX8357_GREEN   0x07E0 ///< GREEN color for drawing graphics
#define HX8357_CYAN    0x07FF ///< CYAN color for drawing graphics
#define HX8357_MAGENTA 0xF81F ///< MAGENTA color for drawing graphics
#define HX8357_YELLOW  0xFFE0 ///< YELLOW color for drawing graphics
#define HX8357_WHITE   0xFFFF ///< WHITE color for drawing graphics


//Functions defines Macros
#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define min(a,b) (((a)<(b))?(a):(b))

//***** Functions prototypes *****//
//1. Write Command to LCD
void LCD_SendCommand(uint8_t com);
//2. Write data to LCD
void LCD_SendData(uint8_t data);
//2.2 Write multiple/DMA
void LCD_SendData_Multi(uint16_t Colordata, uint32_t size);


//3. Set cursor position
void LCD__SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
//4. Initialise function
void LCD_Init(SPI_HandleTypeDef *spiLcdHandle, GPIO_TypeDef *csPORT, uint16_t csPIN, GPIO_TypeDef *dcPORT, uint16_t dcPIN, GPIO_TypeDef *resetPORT, uint16_t resetPIN);
//5. Write data to a single pixel
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color); //Draw single pixel to LCD
//6. Fill the entire screen with a background color
void LCD_Fill(uint16_t color); //Fill entire LCD with color
//7. Rectangle drawing functions
void LCD_Fill_Rect(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, uint16_t color);
//8. Circle drawing functions
void LCD_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
static void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void LCD_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
//9. Line drawing functions
void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void LCD_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void LCD_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
//10. Triangle drawing
void LCD_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void LCD_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
//11. Text printing functions
void LCD_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void LCD_printText(char text[], int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size);
//12. Image print (RGB 565, 2 bytes per pixel)
void LCD_printImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data, uint32_t size);
//13. Set screen rotation
void LCD_setRotation(uint8_t rotate);
//14. Get screen rotation
uint8_t LCD_getRotation(void);

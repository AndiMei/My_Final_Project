/*
 * myOled.h
 *
 *  Created on: Feb 1, 2021
 *      Author: Andi
 */

#ifndef INC_MYOLED_H_
#define INC_MYOLED_H_

#include "stm32f4xx_hal.h"
#include "myFonts.h"
/* I2C address */
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR         0x3D << 1
#endif

/* ------- SSD1306 settings ------- */
/* SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            128
#endif
/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           64
#endif

/* SSD1306 color enumeration */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;


/* Initializes SSD1306 LCD */
uint8_t Display_Init(void);

/* Updates buffer from internal RAM to LCD */
void Display_UpdateScreen(void);

/* Toggles pixels invertion inside internal RAM */
void Display_ToggleInvert(void);

/* Fills entire LCD with desired color */
void Display_Fill(SSD1306_COLOR_t Color);

/* Draws pixel at desired location */
void Display_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);

/* Sets cursor pointer to desired location for strings */
void Display_GotoXY(uint16_t x, uint16_t y);

/* Puts character to internal RAM */
char Display_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);

/* Puts string to internal RAM */
char Display_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color);

/* Puts string to internal RAM */
void Display_Float(float data, FontDef_t* Font, SSD1306_COLOR_t color);

/* Puts number to internal RAM */
void Display_Angka3u(uint16_t data, FontDef_t* Font, SSD1306_COLOR_t color);



/* Draws line on LCD */
void Display_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c);

/* Draws rectangle on LCD */
void Display_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/* Draws filled rectangle on LCD */
void Display_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/* Draws triangle on LCD */
void Display_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

/* Draws circle to STM buffer */
void Display_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/* Draws filled circle to STM buffer */
void Display_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

#ifndef SSD1306_I2C_TIMEOUT
#define SSD1306_I2C_TIMEOUT					20000
#endif

/* Initializes SSD1306 LCD */
void SSD1306_I2C_Init();

/* Writes single byte to slave */
void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

/* Writes multi bytes to slave */
void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

/* Draws the Bitmap */
void SSD1306_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

/* scroll the screen for fixed rows */
void SSD1306_ScrollRight(uint8_t start_row, uint8_t end_row);

void SSD1306_ScrollLeft(uint8_t start_row, uint8_t end_row);

void SSD1306_Scrolldiagright(uint8_t start_row, uint8_t end_row);

void SSD1306_Scrolldiagleft(uint8_t start_row, uint8_t end_row);


void SSD1306_Stopscroll(void);

/* inverts the display i = 1->inverted, i = 0->normal */
void SSD1306_InvertDisplay (int i);

/* clear the display */
void Display_Clear (void);


#endif /* INC_MYOLED_H_ */

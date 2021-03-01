/*
 * myFonts.h
 *
 *  Created on: Feb 1, 2021
 *      Author: Andi
 */

#ifndef INC_MYFONTS_H_
#define INC_MYFONTS_H_

/**
 * Currently, these fonts are supported:
 *  - 7 x 10 pixels
 *  - 11 x 18 pixels
 *  - 16 x 26 pixels
 */
#include "main.h"
#include "string.h"

/* Font structure used on my LCD libraries */
typedef struct {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef_t;

/* String length and height */
typedef struct {
	uint16_t Length;      /*!< String length in units of pixels */
	uint16_t Height;      /*!< String height in units of pixels */
} FONTS_SIZE_t;


/* 7 x 10 pixels font size structure */
extern FontDef_t Font_7x10;

/* 11 x 18 pixels font size structure */
extern FontDef_t Font_11x18;

/* 16 x 26 pixels font size structure */
extern FontDef_t Font_16x26;

/* Calculates string length and height in units of pixels depending on string and font used */
char* FONTS_GetStringSize(char* str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font);

#endif /* INC_MYFONTS_H_ */

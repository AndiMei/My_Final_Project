/*
 * myConverter.c
 *
 *  Created on: Mar 8, 2021
 *      Author: Andi
 */


#include "myConverter.h"

//char buff[9];
char* uint_to_string_digit(uint16_t number, uint8_t digit){
	static char buff[10];

//	memset(buff, 0 , sizeof(buff));
	switch(digit){
	case 1:
		buff[0] = number%10 + 0x30;
		break;
	case 2:
		buff[0] = number%100/10 + 0x30;
		buff[1] = number%10 + 0x30;
		break;
	case 3:
		buff[0] = number/100 + 0x30;
		buff[1] = number%100/10 + 0x30;
		buff[2] = number%10 + 0x30;
		break;
	case 4:
		buff[0] = number/1000 + 0x30;
		buff[1] = number%1000/100 + 0x30;
		buff[2] = number%100/10 + 0x30;
		buff[3] = number%10 + 0x30;
		break;
	case 5:
		buff[0] = number/10000 + 0x30;
		buff[1] = number%10000/1000 + 0x30;
		buff[2] = number%1000/100 + 0x30;
		buff[3] = number%100/10 + 0x30;
		buff[4] = number%10 + 0x30;
	}
	return buff;
}

char* uint_to_string(uint16_t number){
	uint8_t digit=0;
	if(number >= 10000) digit = 5;
	else if(number >= 1000) digit = 4;
	else if(number >= 100) digit = 3;
	else if(number >= 10) digit = 2;
	else digit = 1;
	return uint_to_string_digit(number, digit);
}

char* int_to_string_digit(int32_t number, uint8_t digit){
	char* pBuff;
	static char buff[10];
	char sign;
	if(number < 0){
		sign = '-';
		number = -number;
	}
	else{
		sign = '+';
	}

	pBuff = uint_to_string_digit((int16_t)number, digit);
	if(sign == '-'){
		buff[0] = sign;
		buff[1] = pBuff[0];
		buff[2] = pBuff[1];
		buff[3] = pBuff[2];
		buff[4] = pBuff[3];
		buff[5] = pBuff[4];
	}
	else{
		buff[0] = pBuff[0];
		buff[1] = pBuff[1];
		buff[2] = pBuff[2];
		buff[3] = pBuff[3];
		buff[4] = pBuff[4];
	}
	return buff;
}

char* int_to_string(int16_t number){
	uint8_t digit=0;
	char sign;
	char* pBuff;
	static char buff[10];

	if(number < 0){
		sign = '-';
		number = -number;
	}
	else{
		sign = '+';
	}

	if(number >= 10000) digit = 5;
	else if(number >= 1000) digit = 4;
	else if(number >= 100) digit = 3;
	else if(number >= 10) digit = 2;
	else digit = 1;

	pBuff = uint_to_string_digit((int16_t)number, digit);
	if(sign == '-'){
		buff[0] = sign;
		buff[1] = pBuff[0];
		buff[2] = pBuff[1];
		buff[3] = pBuff[2];
		buff[4] = pBuff[3];
		buff[5] = pBuff[4];
	}
	else{
		buff[0] = pBuff[0];
		buff[1] = pBuff[1];
		buff[2] = pBuff[2];
		buff[3] = pBuff[3];
		buff[4] = pBuff[4];
	}
	return buff;

}

char* float_to_string(float number){
	char* pBuff = (char*)malloc(6);
	char* pBuff2 = (char*)malloc(6);
	static char buff[10];
	char sign;
	int32_t y = (int32_t)number;

	if(number < 0){
		sign = '-';
		number = -number;
		y = -y;
	}
	else{
		sign = '+';
	}

	number = number - y;
	number = number * 100;

	pBuff = uint_to_string(12345);

//	pBuff2 = uint_to_string_digit((uint16_t)number, 2);
	pBuff2 = uint_to_string(67);
	if(sign == '-'){
		if(y >= 10000){
			buff[0] = sign;
			buff[1] = pBuff[0];
			buff[2] = pBuff[1];
			buff[3] = pBuff[2];
			buff[4] = pBuff[3];
			buff[5] = pBuff[4];
			buff[6] = '.';
			buff[7] = pBuff2[0];
			buff[8] = pBuff2[1];
		}
		else if(y >= 1000){
			buff[0] = sign;
			buff[1] = pBuff[0];
			buff[2] = pBuff[1];
			buff[3] = pBuff[2];
			buff[4] = pBuff[3];
			buff[5] = '.';
			buff[6] = pBuff2[0];
			buff[7] = pBuff2[1];
		}
		else if(y >= 100){
			buff[0] = sign;
			buff[1] = pBuff[0];
			buff[2] = pBuff[1];
			buff[3] = pBuff[2];
			buff[4] = '.';
			buff[5] = pBuff2[0];
			buff[6] = pBuff2[1];
		}
		else if(y >= 10){
			buff[0] = sign;
			buff[1] = pBuff[0];
			buff[2] = pBuff[1];
			buff[3] = '.';
			buff[4] = pBuff2[0];
			buff[5] = pBuff2[1];
		}
		else{
			buff[0] = sign;
			buff[1] = pBuff[0];
			buff[2] = '.';
			buff[3] = pBuff2[0];
			buff[4] = pBuff2[1];
		}
	}
	else{
		if(y >= 10000){
			buff[0] = pBuff[0];
			buff[1] = pBuff[1];
			buff[2] = pBuff[2];
			buff[3] = pBuff[3];
			buff[4] = pBuff[4];
			buff[5] = '.';
			buff[6] = pBuff2[0];
			buff[7] = pBuff2[1];
		}
		else if(y >= 1000){
			buff[0] = pBuff[0];
			buff[1] = pBuff[1];
			buff[2] = pBuff[2];
			buff[3] = pBuff[3];
			buff[4] = '.';
			buff[5] = pBuff2[0];
			buff[6] = pBuff2[1];
		}
		else if(y >= 100){
			buff[0] = pBuff[0];
			buff[1] = pBuff[1];
			buff[2] = pBuff[2];
			buff[3] = '.';
			buff[4] = pBuff2[0];
			buff[5] = pBuff2[1];
		}
		else if(y >= 10){
			buff[0] = pBuff[0];
			buff[1] = pBuff[1];
			buff[2] = '.';
			buff[3] = pBuff2[0];
			buff[4] = pBuff2[1];
		}
		else{
			buff[0] = pBuff[0];
			buff[1] = '.';
			buff[2] = pBuff2[0];
			buff[3] = pBuff2[1];
		}
	}
	return buff;
}


//}
//
//void Display_PutFloat(float data, uint8_t precision, FontDef_t* Font, SSD1306_COLOR_t color){
//	int32_t y = (int32_t)data;
//	uint16_t precision_factor = pow(10, precision);
//
//	if(data < 0){
//		Display_Putc('-', Font, color);
//		y = -y;
//		data = -data;
//	}
//	//	else{ Display_Putc('+', Font, color);}
//	data = data - y;
//	data = data * precision_factor;
//	Display_PutUint((uint16_t)y, Font, color);
//	Display_Putc('.', Font, color);
//	Display_PutUintDigit((uint16_t)data, precision, Font, color);
//}

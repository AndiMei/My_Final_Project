/*
 * mySwitch.c
 *
 *  Created on: Mar 8, 2021
 *      Author: Andi
 */

#include "mySwitch.h"

uint8_t readEncoder(void){
	static uint8_t debounce = 0xFF;
	uint8_t dir = 0;

	if(!HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin)){
		debounce = debounce << 1;
	}
	else{
		debounce = (debounce << 1) | 1;
	}
	if(debounce == 0x03 && HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin)){
		dir = 1;
	}
	else if(debounce == 0x03 && !HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin)){
		dir = 2;
	}
	return dir;
}

_Bool switchEncoder(void){
	static uint8_t debounce = 0xFF;
	_Bool detect = 0;
	if(!HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin)){
		debounce = debounce << 1;
	}
	else{
		debounce = (debounce << 1) | 1;
	}
	if(debounce==0x03){
		detect = 1;
	}
	return detect;
}

_Bool encoderCW(void){
	static uint8_t debounce = 0xFF;
	_Bool detect = 0;
	if(!HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin)){
		debounce = debounce << 1;
	}
	else{
		debounce = (debounce << 1) | 1;
	}
	if(debounce == 0x03 && !HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin)){
		detect = 1;
	}
	return detect;
}

_Bool encoderCCW(void){
	static uint8_t debounce = 0xFF;
	_Bool detect = 0;
	if(!HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin)){
		debounce = debounce << 1;
	}
	else{
		debounce = (debounce << 1) | 1;
	}
	if(debounce == 0x03 && HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin)){
		detect = 1;
	}
	return detect;
}

//_Bool switchUp(void){
//	static uint8_t debounce = 0xFF;
//	_Bool detect = 0;
//	if(!HAL_GPIO_ReadPin(SW_UP_GPIO_Port, SW_UP_Pin)){
//		debounce = debounce << 1;
//	}
//	else{
//		debounce = (debounce << 1) | 1;
//	}
//	if(debounce == 0x03){
//		detect = 1;
//	}
//	return detect;
//}

//_Bool switchDown(void){
//	static uint8_t debounce = 0xFF;
//	_Bool detect = 0;
//	if(!HAL_GPIO_ReadPin(SW_DOWN_GPIO_Port, SW_DOWN_Pin)){
//		debounce = debounce << 1;
//	}
//	else{
//		debounce = (debounce << 1) | 1;
//	}
//	if(debounce == 0x03){
//		detect = 1;
//	}
//	return detect;
//}

uint8_t switchDown(void){
	static uint8_t debounce = 0xFF;
	static uint32_t press;
	_Bool longPress = 0;
	uint8_t detect = 0;
	if(!HAL_GPIO_ReadPin(SW_DOWN_GPIO_Port, SW_DOWN_Pin)){
		debounce = debounce << 1;
		detect = 0;
		if(!longPress){
			press++;
			if(press > 100000){
				longPress = 1;
				detect = 2;
			}
		}

	}
	else{
		debounce = (debounce << 1) | 1;
		if(debounce==0xFF){
			longPress = 0;
		}
		detect = 0;
		press = 0;
	}

	if((debounce==0x03) && (!longPress)){
		detect = 1;
	}
	return detect;
}


_Bool switchLeft(void){
	static uint8_t debounce = 0xFF;
	_Bool detect = 0;
	if(!HAL_GPIO_ReadPin(SW_LEFT_GPIO_Port, SW_LEFT_Pin)){
		debounce = debounce << 1;
	}
	else{
		debounce = (debounce << 1) | 1;
	}
	if(debounce == 0x03){
		detect = 1;
	}
	return detect;
}

_Bool switchRight(void){
	static uint8_t debounce = 0xFF;
	_Bool detect = 0;
	if(!HAL_GPIO_ReadPin(SW_RIGHT_GPIO_Port, SW_RIGHT_Pin)){
		debounce = debounce << 1;
	}
	else{
		debounce = (debounce << 1) | 1;
	}
	if(debounce == 0x03){
		detect = 1;
	}
	return detect;
}
//
//_Bool pernahNol=0;
//uint32_t press;
//_Bool longPress = 0;
//_Bool flag = 0;

uint8_t switchUp(void){
	static uint8_t debounce = 0xFF;
	static uint32_t press;
	static _Bool longPress = 0;
	uint8_t detect = 0;
	if(!HAL_GPIO_ReadPin(SW_UP_GPIO_Port, SW_UP_Pin)){
		debounce = debounce << 1;
		detect = 0;
		if(!longPress){
			press++;
			if(press > 100000){
				longPress = 1;
				detect = 2;
			}
		}

	}
	else{
		debounce = (debounce << 1) | 1;
		if(debounce==0xFF){
			longPress = 0;
		}
		detect = 0;
		press = 0;
	}

	if((debounce==0x03) && (!longPress)){
		detect = 1;
	}
	return detect;
}

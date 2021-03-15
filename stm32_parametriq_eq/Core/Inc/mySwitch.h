/*
 * mySwitch.h
 *
 *  Created on: Mar 8, 2021
 *      Author: Andi
 */

#ifndef INC_MYSWITCH_H_
#define INC_MYSWITCH_H_

#include "main.h"

_Bool encSW_last;
_Bool encSW;
uint8_t encResult;
uint16_t sw_debounce;
uint16_t clk_debounce;

uint8_t readEncoder(void);
_Bool switchEncoder(void);
_Bool switchUp(void);
_Bool switchDown(void);
_Bool switchLeft(void);
_Bool switchRight(void);
_Bool encoderCW(void);
_Bool encoderCCW(void);

#endif /* INC_MYSWITCH_H_ */

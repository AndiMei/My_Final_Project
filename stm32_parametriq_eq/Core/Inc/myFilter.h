/*
 * myFilter.h
 *
 *  Created on: Jan 18, 2021
 *      Author: Andi
 */

#ifndef INC_MYFILTER_H_
#define INC_MYFILTER_H_

#include "main.h"

void shelv(float *pCoeffs, _Bool type, float gain, float fc, float fs);
void peak(float *pCoeffs, float gain, float fc, float bw, float fs);

#endif /* INC_MYFILTER_H_ */

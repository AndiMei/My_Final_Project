/*
 * myIIR.h
 *
 *  Created on: Jan 18, 2021
 *      Author: Andi
 */

#ifndef INC_MYIIR_H_
#define INC_MYIIR_H_

#include "main.h"

/* deklarasi variable IIR filter */

int32_t L_sample_in, R_sample_in;
int32_t L_sample, R_sample;
uint32_t L_sample_out, R_sample_out;

float L_x, L_y1, L_y2, L_y3, L_y4, L_y5;
float R_x, R_y1, R_y2, R_y3, R_y4, R_y5;

float L_cS1[5];
float L_cS2[5];
float L_cS3[5];
float L_cS4[5];
float L_cS5[5];

float R_cS1[5];
float R_cS2[5];
float R_cS3[5];
float R_cS4[5];
float R_cS5[5];

float L_xZ11, L_xZ12, L_yZ11, L_yZ12;
float L_xZ21, L_xZ22, L_yZ21, L_yZ22;
float L_xZ31, L_xZ32, L_yZ31, L_yZ32;
float L_xZ41, L_xZ42, L_yZ41, L_yZ42;
float L_xZ51, L_xZ52, L_yZ51, L_yZ52;

float R_xZ11, R_xZ12, R_yZ11, R_yZ12;
float R_xZ21, R_xZ22, R_yZ21, R_yZ22;
float R_xZ31, R_xZ32, R_yZ31, R_yZ32;
float R_xZ41, R_xZ42, R_yZ41, R_yZ42;
float R_xZ51, R_xZ52, R_yZ51, R_yZ52;

/* Function Prototype */
uint32_t IIR_Right(uint32_t data);
uint32_t IIR_Left(uint32_t data);

#endif /* INC_MYIIR_H_ */

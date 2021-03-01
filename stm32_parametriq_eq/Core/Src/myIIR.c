/*
 * myIIR.c
 *
 *  Created on: Jan 18, 2021
 *      Author: Andi
 */
#include "myIIR.h"

uint32_t IIR_Right(uint32_t data){
	R_sample_in = 0;
	R_sample_out = 0;
	/* ubah 24bit 2's comp ke int 32 bit */
	if((data&(1<<23)) != 0){
		R_sample_in = data | ~((1<<24)-1);
	}
	else{
		R_sample_in = data;
	}

	R_x = (float)R_sample_in/4;
	/* stage 1 */
	R_y1 = R_cS1[0] * R_x  + R_cS1[1] * R_xZ11 + R_cS1[2] * R_xZ12 - R_cS1[3] * R_yZ11 - R_cS1[4] * R_yZ12;
	/* stage 1 */
	R_xZ12 = R_xZ11;
	R_xZ11 = R_x;
	R_yZ12 = R_yZ11;
	R_yZ11 = R_y1;
	/* stage 2 */
	R_y2 = R_cS2[0] * R_y1 + R_cS2[1] * R_xZ21 + R_cS2[2] * R_xZ22 - R_cS2[3] * R_yZ21 - R_cS2[4] * R_yZ22;
	/* stage 2 */
	R_xZ22 = R_xZ21;
	R_xZ21 = R_y1;
	R_yZ22 = R_yZ21;
	R_yZ21 = R_y2;
	/* stage 3 */
	R_y3 = R_cS3[0] * R_y2 + R_cS3[1] * R_xZ31 + R_cS3[2] * R_xZ32 - R_cS3[3] * R_yZ31 - R_cS3[4] * R_yZ32;
	/* stage 3 */
	R_xZ32 = R_xZ31;
	R_xZ31 = R_y2;
	R_yZ32 = R_yZ31;
	R_yZ31 = R_y3;
	/* stage 4 */
	R_y4 = R_cS4[0] * R_y3 + R_cS4[1] * R_xZ41 + R_cS4[2] * R_xZ42 - R_cS4[3] * R_yZ41 - R_cS4[4] * R_yZ42;
	/* stage 4 */
	R_xZ42 = R_xZ41;
	R_xZ41 = R_y3;
	R_yZ42 = R_yZ41;
	R_yZ41 = R_y4;
	/* stage 5 */
	R_y5 = R_cS5[0] * R_y4 + R_cS5[1] * R_xZ51 + R_cS5[2] * R_xZ52 - R_cS5[3] * R_yZ51 - R_cS5[4] * R_yZ52;
	/* stage 5 */
	R_xZ52 = R_xZ51;
	R_xZ51 = R_y4;
	R_yZ52 = R_yZ51;
	R_yZ51 = R_y5;
	R_sample = (int32_t) R_y5;

	/* ubah int 32bit ke 24bit 2's comp */
	if(R_sample < 0){
		R_sample_out |= (1<<24) | R_sample;
	}
	else{
		R_sample_out = R_sample;
	}

	return R_sample_out;
}

uint32_t IIR_Left(uint32_t data){
	L_sample_in = 0;
	L_sample_out = 0;
	/* ubah 24bit 2's comp ke int 32 bit */
	if((data&(1<<23)) != 0){
		L_sample_in = data | ~((1<<24)-1);
	}
	else{
		L_sample_in = data;
	}

	L_x = (float)L_sample_in/4;
	/* stage 1 */
	L_y1 = L_cS1[0] * L_x  + L_cS1[1] * L_xZ11 + L_cS1[2] * L_xZ12 - L_cS1[3] * L_yZ11 - L_cS1[4] * L_yZ12;
	/* stage 1 */
	L_xZ12 = L_xZ11;
	L_xZ11 = L_x;
	L_yZ12 = L_yZ11;
	L_yZ11 = L_y1;
	/* stage 2 */
	L_y2 = L_cS2[0] * L_y1 + L_cS2[1] * L_xZ21 + L_cS2[2] * L_xZ22 - L_cS2[3] * L_yZ21 - L_cS2[4] * L_yZ22;
	/* stage 2 */
	L_xZ22 = L_xZ21;
	L_xZ21 = L_y1;
	L_yZ22 = L_yZ21;
	L_yZ21 = L_y2;
	/* stage 3 */
	L_y3 = L_cS3[0] * L_y2 + L_cS3[1] * L_xZ31 + L_cS3[2] * L_xZ32 - L_cS3[3] * L_yZ31 - L_cS3[4] * L_yZ32;
	/* stage 3 */
	L_xZ32 = L_xZ31;
	L_xZ31 = L_y2;
	L_yZ32 = L_yZ31;
	L_yZ31 = L_y3;
	/* stage 4 */
	L_y4 = L_cS4[0] * L_y3 + L_cS4[1] * L_xZ41 + L_cS4[2] * L_xZ42 - L_cS4[3] * L_yZ41 - L_cS4[4] * L_yZ42;
	/* stage 4 */
	L_xZ42 = L_xZ41;
	L_xZ41 = L_y3;
	L_yZ42 = L_yZ41;
	L_yZ41 = L_y4;
	/* stage 5 */
	L_y5 = L_cS5[0] * L_y4 + L_cS5[1] * L_xZ51 + L_cS5[2] * L_xZ52 - L_cS5[3] * L_yZ51 - L_cS5[4] * L_yZ52;
	/* stage 5 */
	L_xZ52 = L_xZ51;
	L_xZ51 = L_y4;
	L_yZ52 = L_yZ51;
	L_yZ51 = L_y5;
	L_sample = (int32_t) L_y5;

	/* ubah int 32bit ke 24bit 2's comp */
	if(L_sample < 0){
		L_sample_out |= (1<<24) | L_sample;
	}
	else{
		L_sample_out = L_sample;
	}

	return L_sample_out;
}

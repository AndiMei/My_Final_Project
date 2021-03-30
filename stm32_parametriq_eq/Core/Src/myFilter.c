/*
 * myFilter.c
 *
 *  Created on: Jan 18, 2021
 *      Author: Andi
 */

#include "myFilter.h"


void shelv(float *pCoeffs, _Bool type, float gain, float fc, float fs){
	float cf_PI = 3.14159265359;
	float V = pow(10,gain/20.0);
	float K = tan(cf_PI*fc/fs);
	float norm = 0;

	/* Low Shelving Filter*/
	if(!type){
		/* Boost*/
		if(gain > 0){
			norm = 1.0f / (1 + sqrt(2) * K + K * K);
			pCoeffs[0] = (1 + sqrt(2*V)*K + V * K * K) * norm;
			pCoeffs[1] = (2 * (V * K * K - 1)) * norm;
			pCoeffs[2] = (1 - sqrt(2*V)*K + V * K * K) * norm;
			pCoeffs[3] = (2 * (K * K - 1)) * norm;
			pCoeffs[4] = (1 - sqrt(2) * K + K * K) * norm;
		}
		/* Cut */
		else if(gain < 0){
			norm = 1.0f / (V + sqrt(2*V) * K + K * K);
			pCoeffs[0] = (V * (1 + sqrt(2) * K + K * K)) * norm;
			pCoeffs[1] = (2 * V * (K * K - 1)) * norm;
			pCoeffs[2] = (V * (1 - sqrt(2) * K + K * K)) * norm;
			pCoeffs[3] = (2 * (K * K - V) ) * norm;
			pCoeffs[4] = (V - sqrt(2 * V) * K + K * K) * norm;
		}
		/* All Pass */
		else{
			pCoeffs[0] = V;
			pCoeffs[1] = 0.0f;
			pCoeffs[2] = 0.0f;
			pCoeffs[3] = 0.0f;
			pCoeffs[4] = 0.0f;
		}
	}
	/* High Shelving Filter */
	else{
		/* Boost */
		if(gain > 0){
			norm = 1.0f / (1 + sqrt(2) * K + K * K);
			pCoeffs[0] = (V + sqrt(2 * V) * K + K * K) * norm;
			pCoeffs[1] = (2 * (K * K - V)) * norm;
			pCoeffs[2] = (V - sqrt(2 * V) * K + K * K) * norm;
			pCoeffs[3] = (2 * (K * K - 1)) * norm;
			pCoeffs[4] = (1 - sqrt(2) * K + K * K);
		}
		/* Cut */
		if(gain < 0){
			norm = 1.0f / (1 + sqrt(2*V) * K + V * K * K);
			pCoeffs[0] = (V * (1 + sqrt(2) * K + K * K)) * norm;
			pCoeffs[1] = (2 * V * (K * K - 1)) * norm;
			pCoeffs[2] = (V * (1 - sqrt(2) * K + K * K)) * norm;
			pCoeffs[3] = (2 * (V * K * K - 1)) * norm;
			pCoeffs[4] = (1 - sqrt(2 * V) * K + V * K * K) * norm;
		}
		/* All Pas */
		else{
			pCoeffs[0] = V;
			pCoeffs[1] = 0.0f;
			pCoeffs[2] = 0.0f;
			pCoeffs[3] = 0.0f;
			pCoeffs[4] = 0.0f;
		}
	}
}


void peak(float *pCoeffs, float gain, float fc, float bw, float fs){
	float cf_PI = 3.14159265359;
	float V = pow(10,gain/20.0);
	float K = tan(cf_PI*fc/fs);
	float Q = fc / bw;
	float norm = 0;

	/* Boost */
	if(gain > 0){
		norm = 1.0f / (1 + K / Q + K * K);
		pCoeffs[0] = (1 + (V / Q) * K + K * K) * norm;
		pCoeffs[1] = (2 * (K * K - 1)) * norm;
		pCoeffs[2] = (1 - (V / Q) * K + K * K) * norm;
		pCoeffs[3] = (2 * (K * K - 1)) * norm;
		pCoeffs[4] = (1 - K / Q + K * K) * norm;
	}
	/* Cut */
	else if(gain < 0){
		norm = 1.0f / (1 + K / (V * Q) + K * K);
		pCoeffs[0] = (1 + K / Q + K * K) * norm;
		pCoeffs[1] = (2 * (K * K - 1)) * norm;
		pCoeffs[2] = (1 - K / Q + K * K) * norm;
		pCoeffs[3] = (2 * (K * K - 1)) * norm;
		pCoeffs[4] = (1 - K / (V * Q) + K * K) * norm;
	}
	/* All Pass */
	else{
		pCoeffs[0] = V;
		pCoeffs[1] = 0;
		pCoeffs[2] = 0;
		pCoeffs[3] = 0;
		pCoeffs[4] = 0;
	}
}

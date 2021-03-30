/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "myIIR.h"
#include "myFilter.h"
#include "myOled.h"
#include "myFonts.h"
#include "mySwitch.h"
#include "myEEPROM.h"
#include "string.h"
#include "stdlib.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BLOCK_SIZE 4
#define FREQSAMPLING 48000
float cobafloat;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_i2s2_ext_rx;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

//enum state0{home_dis, home, setting_dis, setting , save_dis, save} myState;
//enum state1{preset_dis, l_gain_dis, l_gain, r_gain_dis, r_gain, not_select} state_home;
enum state2{
	start, display_home, preset, l_level, r_level, save, save2, display_setting,
	band, l_gain, r_gain, l_fc, r_fc, l_bw, r_bw, set_default
}state_home;
//int8_t last_state = 1;
uint32_t L_Buff = 0;
uint32_t R_Buff = 0;

uint16_t rxBuff[BLOCK_SIZE*2];
uint16_t txBuff[BLOCK_SIZE*2];

uint8_t bakso;

uint16_t cntVal=0;

float L_Samplef;
float R_Samplef;

float min = 8388608;
float max = 0;

/* W0 C0 B0 G120 F20000 B100 */
#define LEN_SERIAL 21
char data_serial_rx[] = "2#3#+10.3#12525#0.37";

#define LEN_PARAMEQ 5
char *strParamEQ[LEN_PARAMEQ];

char data_serial[] = "R0C0B0G120F20000B100";

//char data_serial_coba[] = "1#2#3#7.0#80.5#0.23\n";
char data_serial_coba[] = "ABCD\n";

/* RW $ Preset $ Band $ Gain $ Fc $ Q */
/* 0$0$0$+00.0$+00000$0.00 */

char *dataSerial = data_serial_coba;

char *token;
char delim[2] = "#";
char *array[16];

#define MAX_PRESET	10
#define MAX_BAND	5

int8_t EQ_preset;

//int8_t EQ_level_R[MAX_PRESET];
//int8_t EQ_level_L[MAX_PRESET];

int8_t EQ_band;

//float EQ_gain[MAX_PRESET][MAX_BAND];
//int32_t EQ_fc[MAX_PRESET][MAX_BAND];
//int16_t EQ_Q[MAX_PRESET][MAX_BAND];

struct EQ{

	int8_t level_R;
	int8_t level_L;

	float gain_R[MAX_BAND];
	float gain_L[MAX_BAND];

	uint32_t fc_R[MAX_BAND];
	uint32_t fc_L[MAX_BAND];

	int8_t bw_R[MAX_BAND-2];
	int8_t bw_L[MAX_BAND-2];
};
struct EQ myPreset[MAX_PRESET];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2S2_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C3_Init(void);
/* USER CODE BEGIN PFP */
void saveToEeprom(void);
void LoadFromEeprom(void);
//result_float = 0.0f;

float readWord(_Bool ch, uint8_t buffer){
	int32_t result_int32 = 0;
	uint32_t result_uint32 = 0;
	if(!ch){
		/* read L ch */
		result_uint32 = (rxBuff[0+buffer]<<8) | (rxBuff[1+buffer]>>8);
	}
	else{
		/* write L ch */
		result_uint32 = (rxBuff[2+buffer]<<8) | (rxBuff[3+buffer]>>8);
	}

	/* ubah 24bit 2's comp ke int 32 bit */
	if((result_uint32&(1<<23)) != 0){
		result_int32 = result_uint32 | ~((1<<24)-1);
	}
	else{
		result_int32 = result_uint32;
	}
	return (float)result_int32;
}


void writeWord(float dataf, _Bool ch, uint8_t buffer){
	uint32_t result_uint32 = 0;
	/* ubah int 32bit ke 24bit 2's comp */
	if(dataf < 0){
		result_uint32 |= (1<<24) | (int32_t)dataf;
	}
	else{
		result_uint32 = (int32_t)dataf;
	}

	if(!ch){
		/* write L ch */
		txBuff[0+buffer] = (result_uint32>>8) & 0x0000FFFF;
		txBuff[1+buffer] = (result_uint32<<8) & 0x0000FF00;

	}
	else{
		/* write L ch */
		txBuff[2+buffer] = (result_uint32>>8) & 0x0000FFFF;
		txBuff[3+buffer] = (result_uint32<<8) & 0x0000FF00;
	}
}

void Calc_Coeff_Filter(void){
	/* Hitung koefisien filter Kanan */
	shelv(&R_cS1[0], 0, myPreset[EQ_preset].gain_R[0], myPreset[EQ_preset].fc_R[0], FREQSAMPLING);
	peak(&R_cS2[0], myPreset[EQ_preset].gain_R[1], myPreset[EQ_preset].fc_R[1], myPreset[EQ_preset].bw_R[0], FREQSAMPLING);
	peak(&R_cS3[0], myPreset[EQ_preset].gain_R[2], myPreset[EQ_preset].fc_R[2], myPreset[EQ_preset].bw_R[1], FREQSAMPLING);
	peak(&R_cS4[0], myPreset[EQ_preset].gain_R[3], myPreset[EQ_preset].fc_R[3], myPreset[EQ_preset].bw_R[2], FREQSAMPLING);
	shelv(&R_cS5[0], 1, myPreset[EQ_preset].gain_R[4], myPreset[EQ_preset].fc_R[4], FREQSAMPLING);

	/* Hitung koefisien filter kiri */
	shelv(&L_cS1[0], 0, myPreset[EQ_preset].gain_L[0], myPreset[EQ_preset].fc_L[0], FREQSAMPLING);
	peak(&L_cS2[0], myPreset[EQ_preset].gain_L[1], myPreset[EQ_preset].fc_L[1], myPreset[EQ_preset].bw_L[0], FREQSAMPLING);
	peak(&L_cS3[0], myPreset[EQ_preset].gain_L[2], myPreset[EQ_preset].fc_L[2], myPreset[EQ_preset].bw_L[1], FREQSAMPLING);
	peak(&L_cS4[0], myPreset[EQ_preset].gain_L[3], myPreset[EQ_preset].fc_L[3], myPreset[EQ_preset].bw_L[2], FREQSAMPLING);
	shelv(&L_cS5[0], 1, myPreset[EQ_preset].gain_L[4], myPreset[EQ_preset].fc_L[4], FREQSAMPLING);
}

void Default_Setting(void){
	int8_t i = 0;
	int8_t j = 0;

	for(i=0; i<MAX_PRESET; i++){
		myPreset[i].level_R = 100;
		myPreset[i].level_L = 100;

		for(j=0; j<MAX_BAND; j++){
			myPreset[i].gain_R[j] = 0;
			myPreset[i].gain_L[j] = 0;

			if(EQ_band<3){
				myPreset[i].bw_R[j] = 100;
				myPreset[i].bw_L[j] = 100;
			}

		}

		myPreset[i].fc_R[0] = 70;
		myPreset[i].fc_R[1] = 300;
		myPreset[i].fc_R[2] = 1000;
		myPreset[i].fc_R[3] = 3000;
		myPreset[i].fc_R[4] = 12000;

		myPreset[i].fc_L[0] = 70;
		myPreset[i].fc_L[1] = 300;
		myPreset[i].fc_L[2] = 1000;
		myPreset[i].fc_L[3] = 3000;
		myPreset[i].fc_L[4] = 12000;
	}
	HAL_Delay(10);
}

char *apaIni;

//float jajal = 8.123f;
//void Flash_Write(uint32_t flash_addr, uint32_t flash_data){
//
//	HAL_FLASH_Unlock();
//	FLASH_Erase_Sector(FLASH_SECTOR_11, VOLTAGE_RANGE_3);
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x080E0000, 0x11111111);
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x080E0004, *(uint32_t *)&jajal);
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x080E0008, 0xFFFFFFFF);
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x080E000C, 0xFFFFFFFF);
//	HAL_FLASH_Lock();
//}
//
//uint32_t Flash_Read(uint32_t flash_addr){
//	uint32_t flash_data;
//	flash_data = *(uint32_t*) flash_addr;
//	return flash_data;
//}

//case preset_dis:
//				/* update tampilan preset */
//				Display_DrawRectangle(0, 0, 128, 12, 1);
//				Display_GotoXY(32, 2);
//				Display_Puts("PRESET ", &Font_7x10, 1);
//				Display_PutUint(EQ_preset, 1,&Font_7x10 , 1);
//				Display_UpdateScreen();
//				state_home = last_state;
//				break;
//
//			case not_select:
//				Display_DrawFilledRectangle(0, 17, 128, 21, 0);
//				Display_DrawRectangle(0, 17, 61, 21, 1);
//				Display_GotoXY(7, 19);
//				Display_PutUint(EQ_level_L[EQ_preset], 3, &Font_11x18, 1);
//				Display_Putc('%', &Font_11x18, 1);
//				Display_DrawFilledRectangle(66, 17, 61, 21, 1);
//				Display_GotoXY(73, 19);
//				Display_PutUint(EQ_level_R[EQ_preset], 3, &Font_11x18, 0);
//				Display_Putc('%', &Font_11x18, 0);
//				Display_UpdateScreen();

void myTask(void){
	/* parameter home_page */
	static _Bool preset_selected = 1;
	static _Bool l_selected = 0;
	static _Bool r_selected = 0;

	/* parameter advanced */
	static _Bool band_selected = 1;
	static _Bool l_gain_selected = 0;
	static _Bool r_gain_selected = 0;
	static _Bool l_fc_selected = 0;
	static _Bool r_fc_selected = 0;
	static _Bool l_bw_selected = 0;
	static _Bool r_bw_selected = 0;

	static uint8_t last_state = preset;

	switch(state_home){
	case start:
		Display_GotoXY(6, 2);
		Display_Puts("Selamat Datang ", &Font_7x10, 1);
		Display_UpdateScreen();
		/* Baca preset dari EEPROM */
		LoadFromEeprom();
		/* Hitung koefisien filter untuk pertama kaili on */
		Calc_Coeff_Filter();
		HAL_Delay(1000);
		state_home = display_home;
		break;

	case display_home:
		/* clear baris 1 */
		Display_DrawFilledRectangle(0, 0, 128, 12, 0);
		/* tampilan preset */
		preset_selected? Display_DrawFilledRectangle(0, 0, 128, 12, 1) : Display_DrawRectangle(0, 0, 128, 12, 1);
		Display_GotoXY((128-(8*7))/2, 2);
		Display_Puts("PRESET ", &Font_7x10, !preset_selected);
		Display_PutUint(EQ_preset,&Font_7x10 , !preset_selected);
		/* clear baris 2 */
		Display_DrawFilledRectangle(0, 17, 128, 21, 0);

		/* tampilan L level */
		l_selected? Display_DrawFilledRectangle(0, 17, 61, 21, 1) : Display_DrawRectangle(0, 17, 61, 21, 1);
		if(myPreset[EQ_preset].level_L >= 100)	Display_GotoXY((61-(11*4))/2, 19);
		else if(myPreset[EQ_preset].level_L >= 10)	Display_GotoXY((61-(11*3))/2, 19);
		else Display_GotoXY((61-(11*2))/2, 19);
		Display_PutUint(myPreset[EQ_preset].level_L, &Font_11x18, !l_selected);
		Display_Putc('%', &Font_11x18, !l_selected);

		/* tampilan R level */
		r_selected? Display_DrawFilledRectangle(66, 17, 61, 21, 1) : Display_DrawRectangle(66, 17, 61, 21, 1);
		if(myPreset[EQ_preset].level_R >= 100)	Display_GotoXY(66+(61-(11*4))/2, 19);
		else if(myPreset[EQ_preset].level_R >= 10)	Display_GotoXY(66+(61-(11*3))/2, 19);
		else Display_GotoXY(66+(61-(11*2))/2, 19);
		Display_PutUint(myPreset[EQ_preset].level_R, &Font_11x18, !r_selected);
		Display_Putc('%', &Font_11x18, !r_selected);

		/* Update semua layar */
		Display_UpdateScreen();

		/* Hitung koefisien filter setiap pergantian parameter */
		Calc_Coeff_Filter();

		state_home = last_state;

		break;

	case preset:
		if(switchUp()==2){
			state_home = set_default;

		}
		if(switchRight()){
			preset_selected = 0;
			l_selected = 0;
			r_selected = 1;
			state_home = save;
			last_state = r_level;
		}
		if(switchLeft()){
			preset_selected = 0;
			l_selected = 1;
			r_selected = 0;
			state_home = save;
			last_state = l_level;
		}
		if(encoderCW()){
			EQ_preset++;
			if(EQ_preset>9) EQ_preset=0;
			preset_selected = 1;
			l_selected = 0;
			r_selected = 0;
			state_home = display_home;
			last_state = preset;
		}
		if(encoderCCW()){
			EQ_preset--;
			if(EQ_preset<0) EQ_preset=9;
			preset_selected = 1;
			l_selected = 0;
			r_selected = 0;
			state_home = display_home;
			last_state = preset;
		}
		else if(switchEncoder()){
			state_home = display_setting;
			last_state = band;
			Display_Clear();
		}
		break;

	case l_level:
		if(encoderCW()){
			myPreset[EQ_preset].level_L += 2;
			if(myPreset[EQ_preset].level_L >= 100) myPreset[EQ_preset].level_L=0;
			state_home = display_home;
			last_state = l_level;
		}
		if(encoderCCW()){
			myPreset[EQ_preset].level_L -= 2;
			if(myPreset[EQ_preset].level_L < 0) myPreset[EQ_preset].level_L=100;
			state_home = display_home;
			last_state = l_level;
		}
		if(switchLeft()){
			preset_selected = 1;
			l_selected = 0;
			r_selected = 0;
			state_home = save;
			last_state = preset;
		}
		else if(switchRight()){
			preset_selected = 0;
			l_selected = 0;
			r_selected = 1;
			state_home = save;
			last_state = r_level;
		}
		break;

	case r_level:
		if(encoderCW()){
			myPreset[EQ_preset].level_R += 2;
			if(myPreset[EQ_preset].level_R >= 100) myPreset[EQ_preset].level_R=0;
			state_home = display_home;
			last_state = r_level;
		}
		if(encoderCCW()){
			myPreset[EQ_preset].level_R -= 2;
			if(myPreset[EQ_preset].level_R < 0) myPreset[EQ_preset].level_R=100;
			state_home = display_home;
			last_state = r_level;
		}
		if(switchLeft()){
			preset_selected = 0;
			l_selected = 1;
			r_selected = 0;
			state_home = save;
			last_state = l_level;
		}
		else if(switchRight()){
			preset_selected = 1;
			l_selected = 0;
			r_selected = 0;
			state_home = save;
			last_state = preset;
		}
		if(switchEncoder()==1){
			state_home = save;
			last_state = preset;
		}
		break;

	case save:
		Display_GotoXY(3, 50);
		Display_Puts("Saved !", &Font_7x10, 1);
		/* Update semua layar */
		Display_UpdateScreen();
		/* Simpan data di EEPROM */
		saveToEeprom();
		/* clear baris 3 */
		Display_DrawFilledRectangle(0, 49, 128, 12, 0);
		Display_UpdateScreen();
		state_home = display_home;
		break;

	case save2:
		/* Simpan data di EEPROM */
		saveToEeprom();
		/* clear baris 3 */
		Display_DrawFilledRectangle(0, 49, 128, 12, 0);
		Display_UpdateScreen();
		state_home = display_setting;
		break;

	case set_default:
		Display_GotoXY(3, 50);
		Display_Puts("Set to Default !", &Font_7x10, 1);
		/* Update semua layar */
		Display_UpdateScreen();
		/* panggil fungsi default */
		Default_Setting();
		/* Simpan data di EEPROM */
		saveToEeprom();
		/* clear baris 3 */
		Display_DrawFilledRectangle(0, 49, 128, 12, 0);
		Display_UpdateScreen();
		state_home = display_home;
		break;

	case display_setting:
		/* clear baris 1 */
		Display_DrawFilledRectangle(0, 0, 128, 12, 0);
		/* tampilan band */
		band_selected? Display_DrawFilledRectangle(0, 0, 128, 12, 1) : Display_DrawRectangle(0, 0, 128, 12, 1);
		switch(EQ_band){
		case 0:
			Display_GotoXY((128-7*8)/2, 2);
			Display_Puts("LOW FREQ", &Font_7x10, !band_selected); break;
		case 1:
			Display_GotoXY((128-7*12)/2, 2);
			Display_Puts("LOW-MID FREQ", &Font_7x10, !band_selected); break;
		case 2:
			Display_GotoXY((128-7*8)/2, 2);
			Display_Puts("MID FREQ", &Font_7x10, !band_selected); break;
		case 3:
			Display_GotoXY((128-7*13)/2, 2);
			Display_Puts("MID-HIGH FREQ", &Font_7x10, !band_selected); break;
		case 4:
			Display_GotoXY((128-7*9)/2, 2);
			Display_Puts("HIGH FREQ", &Font_7x10, !band_selected); break;
		}

		/* clear baris 2 */
		Display_DrawFilledRectangle(0, 17, 128, 12, 0);
		/* tampilan gain L */
		l_gain_selected? Display_DrawFilledRectangle(0, 17, 61, 12, 1) : Display_DrawRectangle(0, 17, 61, 12, 1);
		if(myPreset[EQ_preset].gain_L[EQ_band]>=10) Display_GotoXY((61-7*6)/2, 19);
		else if(myPreset[EQ_preset].gain_L[EQ_band]>=0) Display_GotoXY((61-7*5)/2, 19);
		else if(myPreset[EQ_preset].gain_L[EQ_band]>= -10) Display_GotoXY((61-7*6)/2, 19);
		else Display_GotoXY((61-7*7)/2, 19);
		Display_PutFloat(myPreset[EQ_preset].gain_L[EQ_band], 1, &Font_7x10, !l_gain_selected);
		Display_Puts("dB", &Font_7x10, !l_gain_selected);

		/* tampilan gain R */
		r_gain_selected? Display_DrawFilledRectangle(66, 17, 61, 12, 1) : Display_DrawRectangle(66, 17, 61, 12, 1);
		if(myPreset[EQ_preset].gain_R[EQ_band]>=10) Display_GotoXY(66+(61-7*6)/2, 19);
		else if(myPreset[EQ_preset].gain_R[EQ_band]>=0) Display_GotoXY(66+(61-7*5)/2, 19);
		else if(myPreset[EQ_preset].gain_R[EQ_band]>= -10) Display_GotoXY(66+(61-7*6)/2, 19);
		else Display_GotoXY(66+(61-7*7)/2, 19);
		Display_PutFloat(myPreset[EQ_preset].gain_R[EQ_band], 1, &Font_7x10, !r_gain_selected);
		Display_Puts("dB", &Font_7x10, !r_gain_selected);

		/* clear baris 3 */
		Display_DrawFilledRectangle(0, 34, 128, 12, 0);
		/* tampilan fc L */
		l_fc_selected? Display_DrawFilledRectangle(0, 34, 61, 12, 1) : Display_DrawRectangle(0, 34, 61, 12, 1);
		if(myPreset[EQ_preset].fc_L[EQ_band] >= 10000){
			Display_GotoXY((61-7*8)/2, 36);
			Display_PutFloatDigit((float)myPreset[EQ_preset].fc_L[EQ_band]/1000.0f, 2, 2, &Font_7x10, !l_fc_selected);
			Display_Puts("kHz", &Font_7x10, !l_fc_selected);
		}
		else if(myPreset[EQ_preset].fc_L[EQ_band] >= 1000){
			Display_GotoXY((61-7*7)/2, 36);
			Display_PutFloatDigit((float)myPreset[EQ_preset].fc_L[EQ_band]/1000.0f, 1, 2, &Font_7x10, !l_fc_selected);
			Display_Puts("kHz", &Font_7x10, !l_fc_selected);
		}
		else if(myPreset[EQ_preset].fc_L[EQ_band] >= 100){
			Display_GotoXY((61-7*5)/2, 36);
			Display_PutInt(myPreset[EQ_preset].fc_L[EQ_band], 3,0, &Font_7x10, !l_fc_selected);
			Display_Puts("Hz", &Font_7x10, !l_fc_selected);
		}
		else{
			Display_GotoXY((61-7*4)/2, 36);
			Display_PutInt(myPreset[EQ_preset].fc_L[EQ_band], 2,0, &Font_7x10, !l_fc_selected);
			Display_Puts("Hz", &Font_7x10, !l_fc_selected);
		}

		/* tampilan fc R */
		r_fc_selected? Display_DrawFilledRectangle(66, 34, 61, 12, 1) : Display_DrawRectangle(66, 34, 61, 12, 1);
		Display_GotoXY(69, 36);
		if(myPreset[EQ_preset].fc_R[EQ_band] >= 10000){
			Display_GotoXY(66+(61-7*8)/2, 36);
			Display_PutFloatDigit((float)myPreset[EQ_preset].fc_R[EQ_band]/1000.0f, 2, 2, &Font_7x10, !r_fc_selected);
			Display_Puts("kHz", &Font_7x10, !r_fc_selected);
		}
		else if(myPreset[EQ_preset].fc_R[EQ_band] >= 1000){
			Display_GotoXY(66+(61-7*7)/2, 36);
			Display_PutFloatDigit((float)myPreset[EQ_preset].fc_R[EQ_band]/1000.0f, 1, 2, &Font_7x10, !r_fc_selected);
			Display_Puts("kHz", &Font_7x10, !r_fc_selected);
		}
		else if(myPreset[EQ_preset].fc_R[EQ_band] >= 100){
			Display_GotoXY(66+(61-7*5)/2, 36);
			Display_PutInt(myPreset[EQ_preset].fc_R[EQ_band], 3,0, &Font_7x10, !r_fc_selected);
			Display_Puts("Hz", &Font_7x10, !r_fc_selected);
		}
		else{
			Display_GotoXY(66+(61-7*4)/2, 36);
			Display_PutInt(myPreset[EQ_preset].fc_R[EQ_band], 2,0, &Font_7x10, !r_fc_selected);
			Display_Puts("Hz", &Font_7x10, !r_fc_selected);
		}

		/* clear baris 3 */
		Display_DrawFilledRectangle(0, 51, 128, 12, 0);
		/* tampilan bw L */
		l_bw_selected? Display_DrawFilledRectangle(0, 51, 61, 12, 1) : Display_DrawRectangle(0, 51, 61, 12, 1);
		if(myPreset[EQ_preset].bw_L[EQ_band] >= 100)	Display_GotoXY((61-(7*4))/2, 53);
		else if(myPreset[EQ_preset].bw_L[EQ_band] >= 10)	Display_GotoXY((61-(7*3))/2, 53);
		else Display_GotoXY((61-(7*2))/2, 53);
		Display_PutUint(myPreset[EQ_preset].bw_L[EQ_band], &Font_7x10, !l_bw_selected);
		Display_Puts("%", &Font_7x10, !l_bw_selected);

		/* tampilan bw R */
		r_bw_selected? Display_DrawFilledRectangle(66, 51, 61, 12, 1) : Display_DrawRectangle(66, 51, 61, 12, 1);
		if(myPreset[EQ_preset].bw_R[EQ_band] >= 100)	Display_GotoXY(66+(61-(7*4))/2, 53);
		else if(myPreset[EQ_preset].bw_R[EQ_band] >= 10)	Display_GotoXY(66+(61-(7*3))/2, 53);
		else Display_GotoXY(66+(61-(7*2))/2, 53);
		Display_PutUint(myPreset[EQ_preset].bw_R[EQ_band], &Font_7x10, !r_bw_selected);
		Display_Puts("%", &Font_7x10, !r_bw_selected);

		if(EQ_band<1 || EQ_band>3) Display_DrawFilledRectangle(0, 51, 128, 12, 0);

		/* Hitung koefisien filter setiap pergantian parameter */
		Calc_Coeff_Filter();

		/* update screen */
		Display_UpdateScreen();
		state_home = last_state;
		break;

	case band:
		if(encoderCW()){
			EQ_band++;
			if(EQ_band>4) EQ_band = 0;
			state_home = display_setting;
			last_state = band;
		}
		if(encoderCCW()){
			EQ_band--;
			if(EQ_band<0) EQ_band = 4;
			state_home = display_setting;
			last_state = band;
		}
		if(switchEncoder()){
			state_home = display_home;
			last_state = preset;
			Display_Clear();
		}
		if(switchLeft()){
			band_selected = 0;
			l_gain_selected = 1;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = l_gain;
		}
		else if(switchRight()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 1;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = r_gain;
		}
		break;

	case l_gain:
		if(encoderCW()){
			myPreset[EQ_preset].gain_L[EQ_band] += 0.1;
			if(myPreset[EQ_preset].gain_L[EQ_band] > 12.0){
				myPreset[EQ_preset].gain_L[EQ_band] = -12.0;
			}
			state_home = display_setting;
			last_state = l_gain;
		}
		if(encoderCCW()){
			myPreset[EQ_preset].gain_L[EQ_band] -= 0.1;
			if(myPreset[EQ_preset].gain_L[EQ_band] < -12.0){
				myPreset[EQ_preset].gain_L[EQ_band] = 12.0;
			}
			state_home = display_setting;
			last_state = l_gain;
		}

		if(switchUp()){
			band_selected = 1;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = band;
		}
		else if(switchDown()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 1;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = l_fc;
		}
		else if(switchLeft() || switchRight()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 1;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = r_gain;
		}
		break;

	case r_gain:
		if(encoderCW()){
			myPreset[EQ_preset].gain_R[EQ_band] += 0.1;
			if(myPreset[EQ_preset].gain_R[EQ_band] > 12.0){
				myPreset[EQ_preset].gain_R[EQ_band] = -12.0;
			}
			state_home = display_setting;
			last_state = r_gain;
		}
		if(encoderCCW()){
			myPreset[EQ_preset].gain_R[EQ_band] -= 0.1;
			if(myPreset[EQ_preset].gain_R[EQ_band] < -12.0){
				myPreset[EQ_preset].gain_R[EQ_band] = 12.0;
			}
			state_home = display_setting;
			last_state = r_gain;
		}
		if(switchUp()){
			band_selected = 1;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = band;
		}
		else if(switchDown()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 1;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = r_fc;
		}
		else if(switchLeft() || switchRight()){
			band_selected = 0;
			l_gain_selected = 1;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = l_gain;
		}
		break;

	case l_fc:
		if(encoderCW()){
			if(myPreset[EQ_preset].fc_L[EQ_band] >= 1000){
				myPreset[EQ_preset].fc_L[EQ_band] += 100.0000000f;
			}
			else if(myPreset[EQ_preset].fc_L[EQ_band] >= 100){
				myPreset[EQ_preset].fc_L[EQ_band] += 10.0000000f;
			}
			else{
				myPreset[EQ_preset].fc_L[EQ_band] += 1.0000000f;
			}

			if(myPreset[EQ_preset].fc_L[EQ_band] > 16000){
				myPreset[EQ_preset].fc_L[EQ_band] = 50;
			}

			state_home = display_setting;
			last_state = l_fc;
		}
		if(encoderCCW()){
			if(myPreset[EQ_preset].fc_L[EQ_band] >= 1000){
				myPreset[EQ_preset].fc_L[EQ_band] -= 100.0000000f;
			}
			else if(myPreset[EQ_preset].fc_L[EQ_band] >= 100){
				myPreset[EQ_preset].fc_L[EQ_band] -= 10.0000000f;
			}
			else{
				myPreset[EQ_preset].fc_L[EQ_band] -= 1.0000000f;
			}

			if(myPreset[EQ_preset].fc_L[EQ_band] < 50){
				myPreset[EQ_preset].fc_L[EQ_band] = 16000;
			}
			state_home = display_setting;
			last_state = l_fc;
		}
		if(switchUp()){
			band_selected = 0;
			l_gain_selected = 1;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = l_gain;
		}
		else if(switchDown()){
			if((EQ_band > 0) && (EQ_band <4)){
				band_selected = 0;
				l_gain_selected = 0;
				r_gain_selected = 0;
				l_fc_selected = 0;
				r_fc_selected = 0;
				l_bw_selected = 1;
				r_bw_selected = 0;
				state_home = save2;
				last_state = l_bw;
			}
			else{
				band_selected = 1;
				l_gain_selected = 0;
				r_gain_selected = 0;
				l_fc_selected = 0;
				r_fc_selected = 0;
				l_bw_selected = 0;
				r_bw_selected = 0;
				state_home = save2;
				last_state = band;
			}

		}
		else if(switchLeft() || switchRight()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 1;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = r_fc;
		}
		break;

	case r_fc:
		if(encoderCW()){
			if(myPreset[EQ_preset].fc_R[EQ_band] >= 1000){
				myPreset[EQ_preset].fc_R[EQ_band] += 100;
			}
			else if(myPreset[EQ_preset].fc_R[EQ_band] >= 100){
				myPreset[EQ_preset].fc_R[EQ_band] += 10;
			}
			else{
				myPreset[EQ_preset].fc_R[EQ_band] += 1;
			}

			if(myPreset[EQ_preset].fc_R[EQ_band] > 16000){
				myPreset[EQ_preset].fc_R[EQ_band] = 50;
			}

			state_home = display_setting;
			last_state = r_fc;
		}
		if(encoderCCW()){
			if(myPreset[EQ_preset].fc_R[EQ_band] >= 1000){
				myPreset[EQ_preset].fc_R[EQ_band] -= 100;
			}
			else if(myPreset[EQ_preset].fc_R[EQ_band] >= 100){
				myPreset[EQ_preset].fc_R[EQ_band] -= 10;
			}
			else{
				myPreset[EQ_preset].fc_R[EQ_band] -= 1;
			}

			if(myPreset[EQ_preset].fc_R[EQ_band] < 50){
				myPreset[EQ_preset].fc_R[EQ_band] = 16000;
			}
			state_home = display_setting;
			last_state = r_fc;
		}
		if(switchUp()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 1;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = r_gain;
		}
		else if(switchDown()){
			if((EQ_band > 0) && (EQ_band <4)){
				band_selected = 0;
				l_gain_selected = 0;
				r_gain_selected = 0;
				l_fc_selected = 0;
				r_fc_selected = 0;
				l_bw_selected = 0;
				r_bw_selected = 1;
				state_home = save2;
				last_state = r_bw;
			}
			else{
				band_selected = 1;
				l_gain_selected = 0;
				r_gain_selected = 0;
				l_fc_selected = 0;
				r_fc_selected = 0;
				l_bw_selected = 0;
				r_bw_selected = 0;
				state_home = save2;
				last_state = band;
			}
		}
		else if(switchLeft() || switchRight()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 1;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = l_fc;
		}
		break;

	case l_bw:
		if(encoderCW()){
			myPreset[EQ_preset].bw_L[EQ_band] += 1;
			if(myPreset[EQ_preset].bw_L[EQ_band] > 100){
				myPreset[EQ_preset].bw_L[EQ_band] = 0;
			}
			state_home = display_setting;
			last_state = l_bw;
		}
		if(encoderCCW()){
			myPreset[EQ_preset].bw_L[EQ_band] -= 1;
			if(myPreset[EQ_preset].bw_L[EQ_band] < 0){
				myPreset[EQ_preset].bw_L[EQ_band] = 100;
			}
			state_home = display_setting;
			last_state = l_bw;
		}
		if(switchUp()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 1;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = l_fc;
		}
		else if(switchDown()){
			band_selected = 1;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = band;
		}
		else if(switchLeft() || switchRight()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 1;

			state_home = save2;
			last_state = r_bw;
		}
		break;

	case r_bw:
		if(encoderCW()){
			myPreset[EQ_preset].bw_R[EQ_band] += 1;
			if(myPreset[EQ_preset].bw_R[EQ_band] > 100){
				myPreset[EQ_preset].bw_R[EQ_band] = 0;
			}
			state_home = display_setting;
			last_state = r_bw;
		}
		if(encoderCCW()){
			myPreset[EQ_preset].bw_R[EQ_band] -= 1;
			if(myPreset[EQ_preset].bw_R[EQ_band] < 0){
				myPreset[EQ_preset].bw_R[EQ_band] = 100;
			}
			state_home = display_setting;
			last_state = r_bw;
		}
		if(switchUp()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 1;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = r_fc;
		}
		else if(switchDown()){
			band_selected = 1;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 0;
			r_bw_selected = 0;

			state_home = save2;
			last_state = band;
		}
		else if(switchLeft() || switchRight()){
			band_selected = 0;
			l_gain_selected = 0;
			r_gain_selected = 0;
			l_fc_selected = 0;
			r_fc_selected = 0;
			l_bw_selected = 1;
			r_bw_selected = 0;

			state_home = save2;
			last_state = l_bw;
		}
		break;
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t selectAddr[2] ={0x00, 0x00};
uint8_t cobaKirim[3] = {25, 5, 15};
uint8_t cobaTerima[5];
//uint8_t flag = 0;

//struct Coba{
//	uint16_t andi;
//	int8_t meong;
////	uint8_t mei[3];
////	int8_t pras;
//	float pipi;
//};



//struct EQ2{
//
//	int8_t level_R;
//	int8_t level_L;
//
//	float32_t gain_R[MAX_BAND];
//	float gain_L[MAX_BAND];
//
//	uint32_t fc_R[MAX_BAND];
//	uint32_t fc_L[MAX_BAND];
//
//	uint8_t bw_R[MAX_BAND];
//	uint8_t bw_L[MAX_BAND];
//};
//
//
//struct EQ2 myPreset2[MAX_PRESET];

//void EEPROM_save(uint8_t pData){
//	uint8_t lengthData = sizeof(pData);
//	static uint8_t buff[lengthData];
//
//	if(lengthData<32){
//		f
//	}
//}

//uint16_t bytesToWrite(uint16_t size, uint16_t offset){
//	if((size+offset)<32){
//		return size;
//	} else{
//		return 32 - offset;
//	}
//}

//int ukuran;
//uint16_t andi2;
//int8_t meong2;
//float pipi2;

float maxj = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	__HAL_RCC_DMA1_CLK_ENABLE();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2S2_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(20);

	//  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);

//	Display_Init();
	Display_Init();


//		Display_GotoXY(0, 0);
//		Display_Puts("ANDI", &Font_11x18, 1);
//		Display_GotoXY(13, 30);
////		Display_Angka3u(123, &Font_11x18, 1);
////		Display_GotoXY(13, 50);
//		Display_Puts("Prasetyo", &Font_7x10, 1);
//		Display_UpdateScreen();
//		HAL_Delay(2000);
//
//	// 5 huruf 18 huruf
//	Display_DrawRectangle(0, 0, 61, 12, 1);
//	Display_GotoXY(3, 2); Display_Puts("PRESET 9", &Font_7x10, 1);
//	Display_DrawRectangle(66, 0, 61, 12, 1);
//	Display_GotoXY(69, 2); Display_Puts("BAND 5", &Font_7x10, 1);
//
//	//	Display_DrawRectangle(0, 17, 61, 12, 1);
//	Display_DrawFilledRectangle(0, 17, 61, 12, 1);
//	Display_GotoXY(3, 19); Display_Puts("-12.0 dB", &Font_7x10, 0);
//	Display_DrawRectangle(66, 17, 61, 12, 1);
//	Display_GotoXY(69, 19); Display_Puts("+12.0 dB", &Font_7x10, 1);
//	//	Display_GotoXY(5, 16); Display_Puts("BAND 3", &Font_7x10, 1);
//
//	Display_DrawRectangle(0, 34, 61, 12, 1);
//	Display_GotoXY(3, 36); Display_Puts("20 Hz", &Font_7x10, 1);
//	Display_DrawRectangle(66, 34, 61, 12, 1);
//	Display_GotoXY(69, 36); Display_Puts("20 kHz", &Font_7x10, 1);
//
//	Display_DrawRectangle(0, 51, 61, 12, 1);
//	Display_GotoXY(3, 53); Display_Puts("0.02", &Font_7x10, 1);
//	Display_DrawRectangle(66, 51, 61, 12, 1);
//	Display_GotoXY(69, 53); Display_Puts("1.00", &Font_7x10, 1);
//
//	//	Display_GotoXY(0, 11); Display_Puts("234567890234567892345", &Font_7x10, 1);
//	//	Display_GotoXY(0, 22); Display_Puts("234567890234567892345", &Font_7x10, 1);
//	//	Display_GotoXY(0, 33); Display_Puts("234567890234567892345", &Font_7x10, 1);
//	//	Display_GotoXY(0, 44); Display_Puts("234567890234567892345", &Font_7x10, 1);
//	//	Display_GotoXY(0, 55); Display_Puts("234567890234567892345", &Font_7x10, 1);
//
//	Display_UpdateScreen();
//
//	HAL_Delay(2000);
//	Display_Clear();
//
//	/* Hitung koefisien filter Kanan */
//	shelv(&R_cS1[0], 0, 0, 100, FREQSAMPLING);
//	peak(&R_cS2[0], 0, 300, 50, FREQSAMPLING);
//	peak(&R_cS3[0], 0, 1000, 50, FREQSAMPLING);
//	peak(&R_cS4[0], 0, 5000, 50, FREQSAMPLING);
//	shelv(&R_cS5[0], 1, 0, 12000, FREQSAMPLING);
//
//	/* Hitung koefisien filter kiri */
//	shelv(&L_cS1[0], 0, 0, 100, FREQSAMPLING);
//	peak(&L_cS2[0], 0, 300, 50, FREQSAMPLING);
//	peak(&L_cS3[0], 0, 1000, 50, FREQSAMPLING);
//	peak(&L_cS4[0], 0, 5000, 50, FREQSAMPLING);
//	shelv(&L_cS5[0], 1, 0, 12000, FREQSAMPLING);
//
	/* DMA I2S dimulai */
	HAL_GPIO_WritePin(I2S_EN_GPIO_Port, I2S_EN_Pin, 1);
	HAL_I2SEx_TransmitReceive_DMA(&hi2s2, txBuff, rxBuff, BLOCK_SIZE);
//
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//	bakso[0] = 0xAA;
//	bakso[1] = 0x11;
//	bakso[2] = 0x01;
//
////		_Bool complete=0;
//	//  SSD1306_Clear();
//	//	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)data_serial, strlen(data_serial));
//	//	HAL_UART_Receive_DMA(&huart1, (uint8_t*)data_serial_rx, strlen(data_serial_rx));
//	HAL_UART_Receive_IT(&huart1, (uint8_t*)data_serial_rx, LEN_SERIAL);
//
//	EQ_preset = atoi(strParamEQ[0]);
	//	EQ_fc = 20;
	//	if(EQ_fc<0){
	//		EQ_fc=-EQ_fc;
	//		gain[0] = '-';
	//	}
	//	else{
	//		gain[0] = '+';
	//	}
	//	gain[1] = EQ_fc/10000 + 0x30; 			/* puluhan ribu */
	//	gain[2] = (EQ_fc%10000)/1000 + 0x30; 	/* ribuan */
	//	gain[3] = (EQ_fc%1000)/100 + 0x30;		/* ratusan */
	//	gain[4] = (EQ_fc%100)/10 + 0x30;		/* puluhan */
	//	gain[5] = (EQ_fc%10) + 0x30;			/* satuan */
	//	gain[6] = '\0';
	//
	//
	//
	//	angka_f = atoff(angkaF);

	//			  myLCD::Data(x/100+0x30);           // menulis ratusan
	//			    myLCD::Data((x%100)/10+0x30);      // menulis puluhan
	//			    myLCD::Data(x%10+0x30);
	//	Flash_Write(0x080E0000, 0xABCD1234);
//	Flash_Write(0x080E0000, 0x11111111);
//	Flash_Write(0x080E000C, 0x22222222);
//	cntVal = Flash_Read(0x080E0000);


//  	  myName[0].andi = 10;
//  	  myName[1].mei[0] = 13;
//  	  myName[2].mei[1] = 14;
//  	  myName[3].mei[2] = 15;
//  	  myName[4].pras  =11;
//  	  myName[5].pipi = 18.23;
//  	  myName jeneng;

//  	jeneng.andi = 65535;
//  	jeneng.meong = -9;
//  	jeneng.pipi = 10.23;
//  int size = sizeof(myName);
//  unsigned char* pData;
//  unsigned char eeprom[size];
//  unsigned char pDataRecv[size];
//  myName* read_back;
//  pData = (unsigned char*)&jeneng;
//  memcpy(eeprom, pData, size);
//  uint8_t* addrOfStruct = (uint8_t*)(&jeneng);
//  uint16_t sizeOfStruct = sizeof(myName);
//  HAL_I2C_Mem_Write(&hi2c3, 0xA0, 0x00, I2C_MEMADD_SIZE_16BIT, addrOfStruct, sizeOfStruct, 100);
////  uint16_t _andi ((myName*)eeprom)->andi;
//  memcpy(pDataRecv, eeprom, size);
//  read_back = (myName*)(pDataRecv);
//  uint16_t andi2 = read_back->andi;
//  int8_t meong2 = read_back->meong;
//  float pipi2 = read_back->pipi;

//  struct Coba jenengku[10];
//  uint8_t* addrOfStruct = (uint8_t*)&jenengku;


//  uint16_t sizeOfBuf = sizeof(myName);
//  uint8_t receiveBuf[sizeOfBuf];
//  myName* baca;

//  myPreset[0].level_R = 25;
//  myPreset[0].level_L = 15;
//  myPreset[1].level_R = 12;
//  myPreset[1].level_L = 4;

//  myPreset
//  EQ* baca;
//
//  /* kirim */

//


//
//  uint8_t kirim32bit[32] = {
//		  0,1,2,3,4,5,6,7,8,9,10,11,12,13,
//		  14,15,16,17,18,19,20,21,22,23,24,25,
//		  26,27,28,29,30,31
//  };
//
//  uint8_t kirim32bit2[32] = {
//  		  32,33,34,35,36,37,38,39,
//		  40,41,42,43,44,45,46,47,48,49,
//		  50,51,52,53,54,55,56,57,58,59,
//		  60,61,62,63
//  };
//
//  uint8_t terima32bit[32];
//  uint8_t terima32bit2[32];
//
//  uint8_t kirim2bit[2] = {
//		  32,33
//  };
//  uint8_t kirimen[128];
//  uint8_t tomponen[128];
//
//  for(int i=0; i<128; i++){
//	  kirimen[i] = i;
//  }

//  EEPROM_ErasePage(1);
//  HAL_I2C_Mem_Write(&hi2c3, 0xA0, 0, I2C_MEMADD_SIZE_16BIT, kirim32bit, sizeof(kirim32bit), 1000);
//  HAL_Delay(50);
//  HAL_I2C_Mem_Write(&hi2c3, 0xA0, 32, I2C_MEMADD_SIZE_16BIT, kirim32bit2, sizeof(kirim32bit2), 1000);
//  HAL_Delay(50);

    /* terima */
		myTask();
	HAL_GPIO_WritePin(L_SIGN_GPIO_Port, L_SIGN_Pin, 1);
	HAL_GPIO_WritePin(R_SIGN_GPIO_Port, R_SIGN_Pin, 1);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
//	cobafloat = 2374/1000.0f;
	while (1)
	{
		myTask();
//		if(switchUp()){
//			apaIni = "ATAS";
//		}
//		if(switchDown()){
//			apaIni = "BAWAH";
//		}
//		if(switchLeft()){
//			apaIni = "KIRI";
//		}
//		if(switchRight()){
//			apaIni = "KANAN";
//		}
//		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
//		HAL_Delay(1000);
//		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
//		HAL_Delay(1000);
//		bakso = switchCombineUpEnc();
//		if(bakso==1){
//			pernahSatu = 1;
//		}
//		if(bakso==2){
//			pernahDua = 2;
//		}
//		myTask();
//		if(L_Samplef > 2000000){
//			HAL_GPIO_WritePin(L_SIGN_GPIO_Port, L_SIGN_Pin, 0);
//		}
//		else{
//			HAL_GPIO_WritePin(L_SIGN_GPIO_Port, L_SIGN_Pin, 1);
//		}
//
//		if(R_Samplef > 2000000){
//			HAL_GPIO_WritePin(R_SIGN_GPIO_Port, R_SIGN_Pin, 0);
//		}
//		else{
//			HAL_GPIO_WritePin(R_SIGN_GPIO_Port, R_SIGN_Pin, 1);
//		}

//		 EEPROM_Read(0, 0, reciveBuff, sizeOfBuff);
//
//		memcpy(&myPreset2, reciveBuff, sizeof(myPreset2));
//		EEPROM_Write(0, 0, kirimen, sizeof(kirimen));
//		EEPROM_Read(0, 0, tomponen, sizeof(tomponen));
//		HAL_Delay(1);
//		HAL_I2C_Mem_Read(&hi2c3, 0xA0, 0, I2C_MEMADD_SIZE_16BIT, terima32bit, sizeof(terima32bit), 1000);
//		HAL_Delay(50);
//		HAL_I2C_Mem_Read(&hi2c3, 0xA0, 32, I2C_MEMADD_SIZE_16BIT, terima32bit2, sizeof(terima32bit2), 1000);
//		HAL_Delay(50);
//		baca = (EQ*)receiveBuf;
//
//		myName = baca->andi;
//		meong2 = baca->meong;
//		pipi2 = baca->pipi;
//		HAL_I2C_Master_Transmit(&hi2c3, 0xA0, selectAddr, 2, 1000);
//		HAL_Delay(1);
//		HAL_I2C_Master_Receive(&hi2c3, 0xA0, cobaTerima, 5, 100);
//		HAL_Delay(1);
//		HAL_I2C_Mem_Write(&hi2c3, 0xA0, 0x00, I2C_MEMADD_SIZE_16BIT, cobaKirim, 3, 100);
//		HAL_Delay(10);
//		flag =  HAL_I2C_Mem_Read(&hi2c3, 0xA0, 0x00, I2C_MEMADD_SIZE_16BIT, cobaTerima, 5, 1000);
//		HAL_I2C_Mem_Write(&hi2c3, 0xA0, 0x00, I2C_MEMADD_SIZE_8BIT, cobaKirim, 3, 1000);

//		HAL_I2C_Mem_Read(&hi2c3, 0xA0, 0x00, I2C_MEMADD_SIZE_16BIT, cobaTerima, 5, 1000);
//		HAL_Delay(10);
//		HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
//		HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
//		HAL_Delay(100);

//		HAL_I2C_Master_Transmit(&hi2c3, 0xA0, cobaKirim, 5, 1000);
////		HAL_Delay(1);
//		HAL_I2C_Master_Transmit(&hi2c3, 0xA0, selectAddr, 2, 1000);
//		HAL_I2C_Master_Receive(&hi2c3, 0xA0, cobaTerima, 5, 1000);
//		HAL_Delay(11);


		//		HAL_UART_Receive(&huart1, (uint8_t*)data_serial_rx, strlen(data_serial_rx), 100);

		//		int i=0;
		////		token = strtok(data_serial_rx, "#");
		////		while(token != NULL){
		////			array[i++]=token;
		////			token = strtok(NULL,"#");
		////		}
		//		/* W0C0B0G120F20000B100\n */
		////		if(data_serial_rx[0] == 'W'){
		////			EQ_preset = data_serial)
		////		}
		//
		//		token = strtok(data_serial_rx, "#");
		//		while(token != NULL){
		//			strParamEQ[i++] = token;
		//			token = strtok(NULL, "#");
		//		}
		//		EQ_preset = atoi(strParamEQ[0]);
		//		EQ_band = atoi(strParamEQ[1]);
		//		EQ_gain = atof(strParamEQ[2]);
		//		EQ_fc = atoi(strParamEQ[3]);
		//		EQ_Q = atof(strParamEQ[4]);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 50;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_24B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(I2S_EN_GPIO_Port, I2S_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_Pin|TP_D_Pin|TP_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, TP_B_Pin|TP_A_Pin|R_SIGN_Pin|L_SIGN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : I2S_EN_Pin */
  GPIO_InitStruct.Pin = I2S_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(I2S_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin TP_D_Pin TP_C_Pin */
  GPIO_InitStruct.Pin = LED_Pin|TP_D_Pin|TP_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TP_B_Pin TP_A_Pin R_SIGN_Pin L_SIGN_Pin */
  GPIO_InitStruct.Pin = TP_B_Pin|TP_A_Pin|R_SIGN_Pin|L_SIGN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_UP_Pin SW_DOWN_Pin SW_LEFT_Pin SW_RIGHT_Pin */
  GPIO_InitStruct.Pin = SW_UP_Pin|SW_DOWN_Pin|SW_LEFT_Pin|SW_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_SW_Pin ENC_CLK_Pin ENC_DT_Pin */
  GPIO_InitStruct.Pin = ENC_SW_Pin|ENC_CLK_Pin|ENC_DT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
	L_Samplef = readWord(0,0);
	R_Samplef = readWord(1,0);

	L_Samplef = L_Samplef * (myPreset[EQ_preset].level_L * 0.01);
	R_Samplef = R_Samplef * (myPreset[EQ_preset].level_R * 0.01);

	L_Samplef = IIR_Left(L_Samplef);
	R_Samplef = IIR_Right(R_Samplef);

	writeWord(L_Samplef, 0, 0);
	writeWord(R_Samplef, 1, 0);
}

void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s){
	L_Samplef = readWord(0,4);
	R_Samplef = readWord(1,4);

	L_Samplef = L_Samplef * (myPreset[EQ_preset].level_L * 0.01);
	R_Samplef = R_Samplef * (myPreset[EQ_preset].level_R * 0.01);

	L_Samplef = IIR_Left(L_Samplef);
	R_Samplef = IIR_Right(R_Samplef);

	writeWord(L_Samplef, 0, 4);
	writeWord(R_Samplef, 1, 4);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//	for(int i=0; i<strlen(data_serial_rx); i++){
	//		data_serial_rx[i] = 0;
	//	}

	HAL_UART_Receive_IT(&huart1, (uint8_t*)data_serial_rx, LEN_SERIAL);
}

/* ------------------------------------------ EEPROM ------------------------------------------ */
void saveToEeprom(void){
	/* Baca preset terakhir */
	EEPROM_WriteByte(0, 0, (uint8_t*)&EQ_preset);
//	uint8_t buff_preset[1];
//	buff_preset[0] = EQ_preset;
//	EEPROM_Write(0, 0, buff_preset, 1);
	/* Baca semua preset */
	uint8_t* addrOfStruct = (uint8_t*)&myPreset;
	uint16_t sizeOfStruct = sizeof(myPreset);
	EEPROM_Write(1, 0, addrOfStruct, sizeOfStruct);
}

void LoadFromEeprom(void){
	/* Tulis preset terakhir */
//	uint8_t buff_preset[1];
	EEPROM_ReadByte(0, 0, (uint8_t*)&EQ_preset);
//	EEPROM_Read(0, 0, buff_preset, 1);
//	HAL_I2C_Mem_Read(&h, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)
//	EQ_preset =  buff_preset[0];
	/* Tulis semua prset */
    uint16_t sizeOfBuff = sizeof(myPreset);
    uint8_t reciveBuff[sizeOfBuff];
    EEPROM_Read(1, 0, reciveBuff, sizeOfBuff);
    memcpy(&myPreset, reciveBuff, sizeof(myPreset));
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

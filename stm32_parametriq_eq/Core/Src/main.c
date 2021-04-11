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
#include "stdio.h"
#include "myConverter.h"


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

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

/* Declares state */
enum state2{
	start, display_home, preset, l_level, r_level, save, save2, display_setting,
	band, l_gain, r_gain, l_fc, r_fc, l_bw, r_bw, set_default, setting_wireless, connected
}state_home;

uint32_t L_Buff = 0;
uint32_t R_Buff = 0;

uint16_t I2S_rxBuff[BLOCK_SIZE*2];
uint16_t I2S_txBuff[BLOCK_SIZE*2];

float L_Samplef;
float R_Samplef;

float min = 8388608;
float max = 0;

/* W0 C0 B0 G120 F20000 B100 */
#define LEN_SERIAL 21
//char data_serial_rx[] = "2#3#+10.3#12525#0.37";
char data_serial_rx[50];
uint8_t buff_uart[1];

#define LEN_PARAMEQ 5
char *strParamEQ[LEN_PARAMEQ];

char data_serial[] = "R0C0B0G120F20000B100";

//char data_serial_coba[] = "1#2#3#7.0#80.5#0.23\n";
char data_serial_coba[] = "ABCD\n";

/* RW $ Preset $ Band $ Gain $ Fc $ Q */
/* 0$0$0$+00.0$+00000$0.00 */

char *dataSerial = data_serial_coba;

char *token;
char delim[2] = "_";
char *array[16];

#define MAX_PRESET	11
#define MAX_BAND	5

int8_t EQ_preset;
int8_t EQ_band;
int8_t EQ_Con;
char* jajalString;
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

char *str_param_EQ[5];
char *read_preset;
char *read_band;
char *read_channel;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2S2_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void saveToEeprom(void);
void LoadFromEeprom(void);
float I2S_readWord(_Bool ch, uint8_t buffer);
void I2S_writeWord(float dataf, _Bool ch, uint8_t buffer);
void Calc_Coeff_Filter(void);
void Default_Setting(void);
void myTask(void);
void send_init_EQ(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
	MX_I2C3_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	HAL_Delay(20);
	Display_Init();

	HAL_GPIO_WritePin(L_SIGN_GPIO_Port, L_SIGN_Pin, 1);
	HAL_GPIO_WritePin(R_SIGN_GPIO_Port, R_SIGN_Pin, 1);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
	HAL_GPIO_WritePin(I2S_EN_GPIO_Port, I2S_EN_Pin, 1);
	/* DMA I2S dimulai */
	HAL_I2SEx_TransmitReceive_DMA(&hi2s2, I2S_txBuff, I2S_rxBuff, BLOCK_SIZE);
	/* TIM2 dimulai */
	HAL_TIM_Base_Start_IT(&htim2);

	//  HAL_UART_Receive_IT(&huart1, buff_uart, 1);
	HAL_UART_Receive_DMA(&huart1, buff_uart, 1);
	//	send_init_EQ();
	/* USER CODE END 2 */
//	char delim[2] = "#";
	token = strtok("0_1_2_3", delim);

	/* walk through other tokens */
	int i=0;
	while( token != NULL ) {
		str_param_EQ[i++]=token;
		token = strtok(NULL, delim);
	}
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		myTask();

		//	  jajalString = float_to_string(12345.67);



		//	  HAL_UART_Receive(&huart1, (uint8_t*)data_serial_rx, 7, 100);
		//	  HAL_UART_Transmit(&huart1,(uint8_t*)data_serial_rx , 7, 100);
		//	  L_input = 100000;

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
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 4199;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 9;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

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
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();

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
/* ------------------------------------- DSP PROCESS ------------------------------------------ */
void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
	L_Samplef = I2S_readWord(0,0);
	R_Samplef = I2S_readWord(1,0);

	L_input = L_Samplef;
	R_input = R_Samplef;

	L_Samplef = L_Samplef * (myPreset[EQ_preset].level_L * 0.01);
	R_Samplef = R_Samplef * (myPreset[EQ_preset].level_R * 0.01);

	L_Samplef = IIR_Left(L_Samplef);
	R_Samplef = IIR_Right(R_Samplef);

	I2S_writeWord(L_Samplef, 0, 0);
	I2S_writeWord(R_Samplef, 1, 0);
}
void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s){
	L_Samplef = I2S_readWord(0,4);
	R_Samplef = I2S_readWord(1,4);

	L_input = L_Samplef;
	R_input = R_Samplef;

	L_Samplef = L_Samplef * (myPreset[EQ_preset].level_L * 0.01);
	R_Samplef = R_Samplef * (myPreset[EQ_preset].level_R * 0.01);

	L_Samplef = IIR_Left(L_Samplef);
	R_Samplef = IIR_Right(R_Samplef);

	I2S_writeWord(L_Samplef, 0, 4);
	I2S_writeWord(R_Samplef, 1, 4);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
	if(R_input > 1000000){
		HAL_GPIO_WritePin(R_SIGN_GPIO_Port, R_SIGN_Pin, 0);
	}
	else{
		HAL_GPIO_WritePin(R_SIGN_GPIO_Port, R_SIGN_Pin, 1);
	}
	if(L_input > 1000000){
		HAL_GPIO_WritePin(L_SIGN_GPIO_Port, L_SIGN_Pin, 0);
	}
	else{
		HAL_GPIO_WritePin(L_SIGN_GPIO_Port, L_SIGN_Pin, 1);
	}
}

char test_kirim[] = "0#0#R#+12.0#00078#080$";
char* jajal_kirim;
uint8_t ukuran;

char str_preset[2];
char str_band[2];
char str_channel[2];
char str_level[4];
char str_gain[11];
char str_fc[6];
char str_bw[4];
char str_send[200];
char str_temp[25];

uint8_t strLength=0;
int pingPiro=0;
//char str_bw[] = "100";

void send_init_EQ(void){
	//	static uint8_t iNow=0;
	for(int i=0; i<MAX_PRESET; i++){
		for(int j=0; j<MAX_BAND; j++){
			for(int k=0; k<2; k++){
				sprintf(str_preset, "%d", i);
				sprintf(str_band, "%d", j);
				sprintf(str_channel, "%d", k);
				/* L CH */
				if(k==0){
					sprintf(str_level, "%d", myPreset[i].level_L);
					sprintf(str_gain, "%.1f", myPreset[i].gain_L[j]);
					sprintf(str_fc, "%lu", myPreset[i].fc_L[j]);
					if(j>0 && j<4){
						sprintf(str_bw, "%d", myPreset[i].bw_L[j-1]);
					}
				}
				/* R CH */
				else{
					sprintf(str_level, "%d", myPreset[i].level_R);
					sprintf(str_gain, "%.1f", myPreset[i].gain_R[j]);
					sprintf(str_fc, "%lu", myPreset[i].fc_R[j]);
					if(j>0 && j<4){
						sprintf(str_bw, "%d", myPreset[i].bw_R[j-1]);
					}
				}

				strcpy(str_temp, str_preset);	strcat(str_temp, "#");
				strcat(str_temp, str_band);		strcat(str_temp, "#");
				strcat(str_temp, str_channel);	strcat(str_temp, "#");
				strcat(str_temp, str_level);	strcat(str_temp, "#");
				strcat(str_temp, str_gain);		strcat(str_temp, "#");

				if(j>0 && j<4){
					strcat(str_temp, str_fc);	strcat(str_temp, "#");
					strcat(str_temp, str_bw);	strcat(str_temp, "$");
				}
				else{
					strcat(str_temp, str_fc);	strcat(str_temp, "$");
				}
			}
		}


	}
	//	ukuran = strlen(str_send);
	ukuran = strlen(str_temp);
	HAL_UART_Transmit(&huart1, (uint8_t*)str_temp, ukuran,100);
}


char yahek[11];
_Bool pernahMasuk=0;
uint8_t ukuranMasuk;
_Bool enWritePreset=0;

void send_preset(uint8_t nPreset){
	for(int j=0; j<MAX_BAND; j++){
		for(int k=0; k<2; k++){
			sprintf(str_preset, "%d", nPreset);
			sprintf(str_band, "%d", j);
			sprintf(str_channel, "%d", k);
			/* L CH */
			if(k==0){
				sprintf(str_level, "%d", myPreset[nPreset].level_L);
				sprintf(str_gain, "%.1f", myPreset[nPreset].gain_L[j]);
				sprintf(str_fc, "%lu", myPreset[nPreset].fc_L[j]);
				if(j>0 && j<4){
					sprintf(str_bw, "%d", myPreset[nPreset].bw_L[j-1]);
				}
			}
			/* R CH */
			else{
				sprintf(str_level, "%d", myPreset[nPreset].level_R);
				sprintf(str_gain, "%.1f", myPreset[nPreset].gain_R[j]);
				sprintf(str_fc, "%lu", myPreset[nPreset].fc_R[j]);
				if(j>0 && j<4){
					sprintf(str_bw, "%d", myPreset[nPreset].bw_R[j-1]);
				}
			}

			strcpy(str_temp, str_preset);	strcat(str_temp, "#");
			strcat(str_temp, str_band);		strcat(str_temp, "#");
			strcat(str_temp, str_channel);	strcat(str_temp, "#");
			strcat(str_temp, str_level);	strcat(str_temp, "#");
			strcat(str_temp, str_gain);		strcat(str_temp, "#");

			if(j>0 && j<4){
				strcat(str_temp, str_fc);	strcat(str_temp, "#");
				strcat(str_temp, str_bw);	strcat(str_temp, "$");
			}
			else{
				strcat(str_temp, str_fc);	strcat(str_temp, "$");
			}

			HAL_UART_Transmit(&huart1, (uint8_t*)str_temp, strlen(str_temp),100);
		}
	}
}



const char s[2] = "-";
char *token;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	static uint8_t pos;

	data_serial_rx[pos] = buff_uart[0];
	pos++;
	if(buff_uart[0]=='$') {
		pos=0;
		ukuran=strlen(data_serial_rx);
		if(strcmp(data_serial_rx, "RP0$")==0){
			send_preset(0);
		}
		else if(strcmp(data_serial_rx, "RP1$")==0){
			send_preset(1);
		}
		else if(strcmp(data_serial_rx, "RP2$")==0){
			send_preset(2);
		}
		else if(strcmp(data_serial_rx, "RP3$")==0){
			send_preset(3);
		}
		else if(strcmp(data_serial_rx, "RP4$")==0){
			send_preset(4);
		}
		else if(strcmp(data_serial_rx, "RP5$")==0){
			send_preset(5);
		}
		else if(strcmp(data_serial_rx, "RP6$")==0){
			send_preset(6);
		}
		else if(strcmp(data_serial_rx, "RP7$")==0){
			send_preset(7);
		}
		else if(strcmp(data_serial_rx, "RP8$")==0){
			send_preset(8);
		}
		else if(strcmp(data_serial_rx, "RP9$")==0){
			send_preset(9);
		}
		else if(strcmp(data_serial_rx, "WP$")==0){
			enWritePreset=1;
		}

		if(enWritePreset==1){
//			if(strcmp(data_serial_rx, "WP$")!=0){
//				strcpy(yahek,data_serial_rx);
//
//				/* get the first token */
//				token = strtok("A#N#D#I", "#");
//
//				/* walk through other tokens */
//				while( token != NULL ) {
//					token = strtok(NULL, "#");
//				}
////				enWritePreset=0;
//
//			}

		}
		strcpy(data_serial_rx, "");
		//		ukuranMasuk = strlen(data_serial_rx);
		//		if(strcmp(data_serial_rx, "P0$\0")==0){
		//			pernahMasuk=1;
		//
		//		}
		//		jajal_kirim = uint_to_string(12345);
		//		jajal_kirim = float_to_string(12345.76);
		//		sprintf(yahek, "%d", -123);
		////		char str_bw[4] = uint_to_string_digit(1234, 4);
		//		ukuran = strlen(str_send);
		//				send_init_EQ();
		//		HAL_UART_Transmit(&huart1, (uint8_t*)str_send, ukuran,100);
		//		jajal_kirim = uint_to_string_digit(67, 2);
		//		HAL_UART_Transmit(&huart1, (uint8_t*)jajal_kirim, ukuran,100);
	}
}
/* ------------------------------------------ EEPROM ------------------------------------------ */
void saveToEeprom(void){
	/* Baca preset terakhir */
	EEPROM_WriteByte(0, 0, (uint8_t*)&EQ_preset);
	/* Baca semua preset */
	uint8_t* addrOfStruct = (uint8_t*)&myPreset;
	uint16_t sizeOfStruct = sizeof(myPreset);
	EEPROM_Write(1, 0, addrOfStruct, sizeOfStruct);
}
void LoadFromEeprom(void){
	/* Tulis preset terakhir */
	EEPROM_ReadByte(0, 0, (uint8_t*)&EQ_preset);
	/* Tulis semua prset */
	uint16_t sizeOfBuff = sizeof(myPreset);
	uint8_t reciveBuff[sizeOfBuff];
	EEPROM_Read(1, 0, reciveBuff, sizeOfBuff);
	memcpy(&myPreset, reciveBuff, sizeof(myPreset));
}

/* -------------------------------------- I2S [AK4556] ---------------------------------------- */
float I2S_readWord(_Bool ch, uint8_t buffer){
	int32_t result_int32 = 0;
	uint32_t result_uint32 = 0;
	if(!ch){
		/* read L ch */
		result_uint32 = (I2S_rxBuff[0+buffer]<<8) | (I2S_rxBuff[1+buffer]>>8);
	}
	else{
		/* write L ch */
		result_uint32 = (I2S_rxBuff[2+buffer]<<8) | (I2S_rxBuff[3+buffer]>>8);
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
void I2S_writeWord(float dataf, _Bool ch, uint8_t buffer){
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
		I2S_txBuff[0+buffer] = (result_uint32>>8) & 0x0000FFFF;
		I2S_txBuff[1+buffer] = (result_uint32<<8) & 0x0000FF00;

	}
	else{
		/* write L ch */
		I2S_txBuff[2+buffer] = (result_uint32>>8) & 0x0000FFFF;
		I2S_txBuff[3+buffer] = (result_uint32<<8) & 0x0000FF00;
	}
}


/* --------------------------------------- EQ PRESET ------------------------------------------ */
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
}


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	//	for(int i=0; i<strlen(data_serial_rx); i++){
//	//		data_serial_rx[i] = 0;
//	//	}
////	HAL_UART_Transmit(&huart1, (uint8_t*)data_serial_rx, 8,100);
////	HAL_UART_Receive_IT(&huart1, (uint8_t*)data_serial_rx, LEN_SERIAL);
//	HAL_UART_Receive_IT(&huart1, (uint8_t*)data_serial_rx, 8);
//}

void myTask(void){
	/* parameter home_page */
	static _Bool preset_selected = 1;
	static _Bool l_selected = 0;
	static _Bool r_selected = 0;
	static _Bool con_selected = 0;

	/* parameter connected */
	static _Bool oneTime=0;
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

		/* clear baris 3 */
		Display_DrawFilledRectangle(0, 50, 128, 12, 0);
		/* tampilan setting wireless */
		con_selected? Display_DrawFilledRectangle(0, 50, 128, 12, 1) : Display_DrawRectangle(0, 50, 128, 12, 1);
		//		Display_GotoXY(0, 50);
		//		Display_Puts("Connectivity:", &Font_7x10, 1);
		switch(EQ_Con){
		case 0:
			Display_GotoXY((128-(13*7))/2, 52);
			Display_Puts("No Connection", &Font_7x10, !con_selected);
			break;
		case 1:
			Display_GotoXY((128-(9*7))/2, 52);
			Display_Puts("Bluetooth", &Font_7x10, !con_selected);
			break;
		case 2:
			Display_GotoXY((128-(4*7))/2, 52);
			Display_Puts("WiFi", &Font_7x10, !con_selected);
			break;
		}

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
				if(EQ_preset>MAX_PRESET-1) EQ_preset=0;
				preset_selected = 1;
				l_selected = 0;
				r_selected = 0;
				state_home = display_home;
				last_state = preset;
			}
			if(encoderCCW()){
				EQ_preset--;
				if(EQ_preset<0) EQ_preset=MAX_PRESET-1;
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
				myPreset[EQ_preset].level_L += 5;
				if(myPreset[EQ_preset].level_L >= 100) myPreset[EQ_preset].level_L=0;
				state_home = display_home;
				last_state = l_level;
			}
			if(encoderCCW()){
				myPreset[EQ_preset].level_L -= 5;
				if(myPreset[EQ_preset].level_L < 0) myPreset[EQ_preset].level_L=100;
				state_home = display_home;
				last_state = l_level;
			}
			if(switchLeft()){
				preset_selected = 0;
				l_selected = 0;
				r_selected = 0;
				con_selected = 1;
				state_home = save;
				last_state = setting_wireless;
			}
			else if(switchRight()){
				preset_selected = 1;
				l_selected = 0;
				r_selected = 0;
				con_selected = 0;
				state_home = save;
				last_state = preset;
			}
			break;

		case r_level:
			if(encoderCW()){
				myPreset[EQ_preset].level_R += 5;
				if(myPreset[EQ_preset].level_R >= 100) myPreset[EQ_preset].level_R=0;
				state_home = display_home;
				last_state = r_level;
			}
			if(encoderCCW()){
				myPreset[EQ_preset].level_R -= 5;
				if(myPreset[EQ_preset].level_R < 0) myPreset[EQ_preset].level_R=100;
				state_home = display_home;
				last_state = r_level;
			}
			if(switchLeft()){
				preset_selected = 1;
				l_selected = 0;
				r_selected = 0;
				con_selected = 0;
				state_home = save;
				last_state = preset;
			}
			else if(switchRight()){
				preset_selected = 0;
				l_selected = 0;
				r_selected = 0;
				con_selected = 1;
				state_home = save;
				last_state = setting_wireless;
			}
			if(switchEncoder()==1){
				state_home = save;
				last_state = preset;
			}
			break;
		case setting_wireless:
			if(encoderCW()){
				state_home = display_home;
				EQ_Con++; if(EQ_Con>2) EQ_Con=0;
				last_state = setting_wireless;
			}
			if(encoderCCW()){
				state_home = display_home;
				EQ_Con--; if(EQ_Con<0) EQ_Con=2;
				last_state = setting_wireless;
			}
			if(switchLeft()){
				preset_selected = 0;
				l_selected = 0;
				r_selected = 1;
				con_selected = 0;
				state_home = save;
				last_state = r_level;
			}
			else if(switchRight()){
				preset_selected = 0;
				l_selected = 1;
				r_selected = 0;
				state_home = save;
				last_state = l_level;
			}
			if(switchEncoder()){
				if(EQ_Con!=0){
					oneTime=1;
					state_home = connected;
				}
			}
			break;

		case connected:
			if(oneTime){
				oneTime=0;
				Display_Clear();
				switch(EQ_Con){
				case 1:
					Display_GotoXY((128-(12*7))/2, 0);
					Display_Puts("Bluetooth On", &Font_7x10, 1);

					strcpy(str_send, "BTON$");
					strLength = strlen(str_send);
					HAL_UART_Transmit(&huart1, (uint8_t*)str_send,strLength , 100);
					break;
				case 2:

					Display_GotoXY((128-(7*7))/2, 0);
					Display_Puts("WiFi On", &Font_7x10, 1);

					strcpy(str_send, "WIFION$");
					strLength = strlen(str_send);
					HAL_UART_Transmit(&huart1, (uint8_t*)str_send,strLength , 100);
					break;
				}
				Display_UpdateScreen();
			}

			if(switchEncoder()){
				switch(EQ_Con){
				case 1:
					strcpy(str_send, "BTOFF$");
					strLength = strlen(str_send);
					HAL_UART_Transmit(&huart1, (uint8_t*)str_send,strLength , 100);
					break;
				case 2:
					strcpy(str_send, "WIFIOFF$");
					strLength = strlen(str_send);
					HAL_UART_Transmit(&huart1, (uint8_t*)str_send,strLength , 100);
					break;
				}
				state_home = display_home;
				last_state = setting_wireless;
				Display_Clear();
			}
			break;
		case save:
			//		Display_GotoXY(3, 50);
			//		Display_Puts("Saved !", &Font_7x10, 1);
			//		/* Update semua layar */
			//		Display_UpdateScreen();
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
			Display_DrawFilledRectangle(0, 50, 128, 12, 0);
			Display_GotoXY(3, 50);
			Display_Puts("Set to Default !", &Font_7x10, 1);
			/* Update semua layar */
			Display_UpdateScreen();
			/* panggil fungsi default */
			Default_Setting();
			/* Simpan data di EEPROM */
			saveToEeprom();
			/* clear baris 3 */
			HAL_Delay(1000);
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
					myPreset[EQ_preset].gain_L[EQ_band] += 0.5;
					if(myPreset[EQ_preset].gain_L[EQ_band] > 24.0){
						myPreset[EQ_preset].gain_L[EQ_band] = -24.0;
					}
					state_home = display_setting;
					last_state = l_gain;
				}
				if(encoderCCW()){
					myPreset[EQ_preset].gain_L[EQ_band] -= 0.5;
					if(myPreset[EQ_preset].gain_L[EQ_band] < -24.0){
						myPreset[EQ_preset].gain_L[EQ_band] = 24.0;
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
					myPreset[EQ_preset].gain_R[EQ_band] += 0.5;
					if(myPreset[EQ_preset].gain_R[EQ_band] > 24.0){
						myPreset[EQ_preset].gain_R[EQ_band] = -24.0;
					}
					state_home = display_setting;
					last_state = r_gain;
				}
				if(encoderCCW()){
					myPreset[EQ_preset].gain_R[EQ_band] -= 0.5;
					if(myPreset[EQ_preset].gain_R[EQ_band] < -24.0){
						myPreset[EQ_preset].gain_R[EQ_band] = 24.0;
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

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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_i2s2_ext_tx;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

uint32_t L_Buff = 0;
uint32_t R_Buff = 0;

uint16_t rxBuff[BLOCK_SIZE*2];
uint16_t txBuff[BLOCK_SIZE*2];

uint8_t bakso[3];

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

int8_t EQ_preset;
int8_t EQ_channel;
int8_t EQ_band;
float EQ_gain;
int32_t EQ_fc;
float32_t EQ_Q;

char angkaF[] = "-12.349";
float angka_f;

char gain[7];



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2S2_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

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
uint8_t readEnc(void){
	uint8_t dir;
	if(HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin) == 0){
		while(HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin) == 0){
			if(HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin) == 0){
				if(HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin) == 0){
					dir = 1;
					break;
				}
				else{
					dir = 2;
					break;
				}
			}
		}

	}
	else if(HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin) && HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin)){
		dir = 0;
	}

	if(!HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin)){
		dir = 3;
	}
	return dir;
}

void Flash_Write(uint32_t flash_addr, uint32_t flash_data){
	HAL_FLASH_Unlock();
	FLASH_Erase_Sector(FLASH_SECTOR_11, VOLTAGE_RANGE_3);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_addr, flash_data);
	HAL_FLASH_Lock();
}

uint32_t Flash_Read(uint32_t flash_addr){
	uint32_t flash_data;
	flash_data = *(uint32_t*) flash_addr;
	return flash_data;
}

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

	//  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);

	Display_Init();
	Display_GotoXY(0, 0);
	Display_Puts("ANDI", &Font_11x18, 1);
	Display_GotoXY(13, 30);
	Display_Angka3u(123, &Font_11x18, 1);
	Display_GotoXY(13, 50);
	Display_Puts("Prasetyo", &Font_7x10, 1);
	Display_UpdateScreen();

	HAL_Delay(1000);

	/* Hitung koefisien filter Kanan */
	shelv(&R_cS1[0], 0, 0, 100, FREQSAMPLING);
	peak(&R_cS2[0], 0, 300, 50, FREQSAMPLING);
	peak(&R_cS3[0], 0, 1000, 50, FREQSAMPLING);
	peak(&R_cS4[0], 0, 5000, 50, FREQSAMPLING);
	shelv(&R_cS5[0], 1, 0, 12000, FREQSAMPLING);

	/* Hitung koefisien filter kiri */
	shelv(&L_cS1[0], 0, 0, 100, FREQSAMPLING);
	peak(&L_cS2[0], 0, 300, 50, FREQSAMPLING);
	peak(&L_cS3[0], 0, 1000, 50, FREQSAMPLING);
	peak(&L_cS4[0], 0, 5000, 50, FREQSAMPLING);
	shelv(&L_cS5[0], 1, 0, 12000, FREQSAMPLING);

	/* DMA I2S dimulai */
	HAL_I2SEx_TransmitReceive_DMA(&hi2s2, txBuff, rxBuff, BLOCK_SIZE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	bakso[0] = 0xAA;
	bakso[1] = 0x11;
	bakso[2] = 0x01;

	_Bool complete=0;
	//  SSD1306_Clear();
//	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)data_serial, strlen(data_serial));
//	HAL_UART_Receive_DMA(&huart1, (uint8_t*)data_serial_rx, strlen(data_serial_rx));
	HAL_UART_Receive_IT(&huart1, (uint8_t*)data_serial_rx, LEN_SERIAL);

	EQ_preset = atoi(strParamEQ[0]);
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
	cntVal = Flash_Read(0x080E0000);

	while (1)
	{
//		HAL_UART_Receive(&huart1, (uint8_t*)data_serial_rx, strlen(data_serial_rx), 100);

		int i=0;
//		token = strtok(data_serial_rx, "#");
//		while(token != NULL){
//			array[i++]=token;
//			token = strtok(NULL,"#");
//		}
		/* W0C0B0G120F20000B100\n */
//		if(data_serial_rx[0] == 'W'){
//			EQ_preset = data_serial)
//		}

		token = strtok(data_serial_rx, "#");
		while(token != NULL){
			strParamEQ[i++] = token;
			token = strtok(NULL, "#");
		}
		EQ_preset = atoi(strParamEQ[0]);
		EQ_band = atoi(strParamEQ[1]);
		EQ_gain = atof(strParamEQ[2]);
		EQ_fc = atoi(strParamEQ[3]);
		EQ_Q = atof(strParamEQ[4]);

//		HAL_Delay(1000);
//		if((readEnc()==1) && !complete){
//			cntVal ++;
//			Flash_Write(0x080E0000, cntVal);
//			complete=1;
//
//
//		}
//		else if((readEnc()==2) && !complete){
//			cntVal --;
//			complete=1;
//		}
//		else if(readEnc()==0){
//			complete=0;
//		}
//		else if(readEnc()==3){
//			cntVal = 0;
//		}


		//	  cntVal = TIM1->CNT;
		//	  for(int i=0; i<999; i++){
		//		  Display_GotoXY(13, 0);
		//		  Display_Angka3u(i, &Font_7x10, 1);
		//		  Display_GotoXY(13, 9);
		//		  Display_Angka3u(i, &Font_7x10, 1);
		//		  Display_GotoXY(13, 18);
		//		  Display_Angka3u(i, &Font_7x10, 1);
		//		  Display_UpdateScreen();
		//	  }

		//	  Display_DrawFilledRectangle(2, 0, 126, 11, 1);
		//	  Display_GotoXY(38, 2);
		//	  Display_Puts("Preset 1", &Font_7x10, 0);
		//
		//	  Display_DrawRectangle(2, 16, 20, 12, 1);
		//	  Display_GotoXY(4,18);
		//	  Display_Puts("G", &Font_7x10, 1);
		////
		////	  Display_GotoXY(0, 22);
		////	  Display_Puts("G 12.20 -5.00", &Font_7x10, 1);
		////
		////	  Display_GotoXY(0, 32);
		////	  Display_Puts("fc 00.00 -00.00k", &Font_7x10, 1);
		////
		////	  Display_GotoXY(0, 42);
		////	  Display_Puts("Q 0.730 0.239", &Font_7x10, 1);
		//
		//	  Display_UpdateScreen();
		//	  Display_DrawRectangle(0, 0, 45,11, 1);
		//	  Display_GotoXY(2, 2);
		//	  Display_Float(-99.99, &Font_7x10, 1);
		//

		//	  for(float i=0.0; i<99.99; i+=0.01){
		////		  Display_DrawFilledRectangle(0, 14, 45, 11, 1);
		//		  Display_GotoXY(0, 30);
		//		  Display_Float(i, &Font_11x18, 1);
		//		  Display_UpdateScreen();
		//		  HAL_Delay(100);
		//	  }

		//	  Display_GotoXY(0, 9);
		//	  Display_Float(12.53, &Font_7x10, 1);
		//	  Display_GotoXY(0, 18);

		//	  Display_DrawLine(0,18 , 40, 18, 1);
		//	  Display_DrawLine(40, 0, 40, 7, 1);

		//	  SSD1306_GotoXY(0, 0);
		//	  SSD1306_Puts("ANDI", &Font_11x18, 1);
		//	  SSD1306_GotoXY(13, 30);
		//	  SSD1306_Puts("MEI", &Font_11x18, 1);
		//	  SSD1306_UpdateScreen();
		//	  HAL_I2C_Master_Transmit(&hi2c1, 0x3D<<1, bakso, 2, HAL_MAX_DELAY);
		//	  HAL_Delay(1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
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
  hi2s2.Init.Mode = I2S_MODE_MASTER_RX;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_Pin|TEST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, R_SIGN_Pin|L_SIGN_Pin|SW_RIGHT_Pin|SW_LEFT_Pin 
                          |SW_DOWN_Pin|SW_UP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_Pin TEST_Pin */
  GPIO_InitStruct.Pin = LED_Pin|TEST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_DT_Pin ENC_SW_Pin ENC_CLK_Pin */
  GPIO_InitStruct.Pin = ENC_DT_Pin|ENC_SW_Pin|ENC_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : R_SIGN_Pin L_SIGN_Pin SW_RIGHT_Pin SW_LEFT_Pin 
                           SW_DOWN_Pin SW_UP_Pin */
  GPIO_InitStruct.Pin = R_SIGN_Pin|L_SIGN_Pin|SW_RIGHT_Pin|SW_LEFT_Pin 
                          |SW_DOWN_Pin|SW_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_GPIO_WritePin(TEST_GPIO_Port, TEST_Pin, 1);
	L_Samplef = readWord(0,0);
	R_Samplef = readWord(1,0);
//
////	i=10;
////	if(--i < 0){
////		i=10;
////		if(L_Samplef > max) max = L_Samplef;
////		if(L_Samplef < min) min = L_Samplef;
////	}
//
////	if(R_Samplef > max) max = L_Samplef;
////	if(R_Samplef < min) min = L_Samplef;
//
	writeWord(L_Samplef, 0, 0);
	writeWord(L_Samplef, 1, 0);
	HAL_GPIO_WritePin(TEST_GPIO_Port, TEST_Pin, 0);
}

void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s){
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_GPIO_WritePin(TEST_GPIO_Port, TEST_Pin, 1);
	L_Samplef = readWord(0,4);
	R_Samplef = readWord(1,4);
//	i=10;
//	if(--i < 0){
//		i=10;
//		if(L_Samplef > max) max = L_Samplef;
//		if(L_Samplef < min) min = L_Samplef;
//	}

//	writeWord(L_Samplef, 0, 0);
//	writeWord(L_Samplef, 1, 0);
	writeWord(L_Samplef, 0, 4);
	writeWord(L_Samplef, 1, 4);
	HAL_GPIO_WritePin(TEST_GPIO_Port, TEST_Pin, 0);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	for(int i=0; i<strlen(data_serial_rx); i++){
//		data_serial_rx[i] = 0;
//	}

	HAL_UART_Receive_IT(&huart1, (uint8_t*)data_serial_rx, LEN_SERIAL);
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

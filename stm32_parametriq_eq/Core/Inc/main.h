/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define ARM_MATH_CM4
#include "arm_math.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define TEST_Pin GPIO_PIN_2
#define TEST_GPIO_Port GPIOA
#define ENC_DT_Pin GPIO_PIN_9
#define ENC_DT_GPIO_Port GPIOE
#define ENC_SW_Pin GPIO_PIN_10
#define ENC_SW_GPIO_Port GPIOE
#define ENC_CLK_Pin GPIO_PIN_11
#define ENC_CLK_GPIO_Port GPIOE
#define R_SIGN_Pin GPIO_PIN_0
#define R_SIGN_GPIO_Port GPIOD
#define L_SIGN_Pin GPIO_PIN_1
#define L_SIGN_GPIO_Port GPIOD
#define SW_RIGHT_Pin GPIO_PIN_2
#define SW_RIGHT_GPIO_Port GPIOD
#define SW_LEFT_Pin GPIO_PIN_3
#define SW_LEFT_GPIO_Port GPIOD
#define SW_DOWN_Pin GPIO_PIN_4
#define SW_DOWN_GPIO_Port GPIOD
#define SW_UP_Pin GPIO_PIN_5
#define SW_UP_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

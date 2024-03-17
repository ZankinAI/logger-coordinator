/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define RESET_ZIG_Pin GPIO_PIN_7
#define RESET_ZIG_GPIO_Port GPIOA
#define UART_BAUD_RESET_Pin GPIO_PIN_8
#define UART_BAUD_RESET_GPIO_Port GPIOA



/* USER CODE BEGIN Private defines */
#define zigbee_on() HAL_GPIO_WritePin(RESET_ZIG_GPIO_Port, RESET_ZIG_Pin, GPIO_PIN_SET)
#define zigbee_off() HAL_GPIO_WritePin(RESET_ZIG_GPIO_Port, RESET_ZIG_Pin, GPIO_PIN_RESET)

#define zigbee_baud_on() HAL_GPIO_WritePin(UART_BAUD_RESET_GPIO_Port, UART_BAUD_RESET_Pin, GPIO_PIN_SET)
#define zigbee_baud_off() HAL_GPIO_WritePin(UART_BAUD_RESET_GPIO_Port, UART_BAUD_RESET_Pin, GPIO_PIN_RESET)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

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
#include "stm32f4xx_hal.h"

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
#define PARACHUTE_Pin GPIO_PIN_0
#define PARACHUTE_GPIO_Port GPIOC
#define MOTOR_X_Pin GPIO_PIN_1
#define MOTOR_X_GPIO_Port GPIOC
#define MOTOR_Y_Pin GPIO_PIN_2
#define MOTOR_Y_GPIO_Port GPIOC
#define IMU_INT_Pin GPIO_PIN_3
#define IMU_INT_GPIO_Port GPIOC
#define SD_CD_Pin GPIO_PIN_1
#define SD_CD_GPIO_Port GPIOA
#define CS_PRESSURE_Pin GPIO_PIN_2
#define CS_PRESSURE_GPIO_Port GPIOA
#define CS_GYRO_Pin GPIO_PIN_3
#define CS_GYRO_GPIO_Port GPIOA
#define CS_WIRELESS_Pin GPIO_PIN_4
#define CS_WIRELESS_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOA
#define MISO_Pin GPIO_PIN_6
#define MISO_GPIO_Port GPIOA
#define MOSI_Pin GPIO_PIN_7
#define MOSI_GPIO_Port GPIOA
#define ANTENNA_IRQ_Pin GPIO_PIN_5
#define ANTENNA_IRQ_GPIO_Port GPIOC
#define PRESSURE_INT_Pin GPIO_PIN_6
#define PRESSURE_INT_GPIO_Port GPIOC
#define RF_RESET_Pin GPIO_PIN_7
#define RF_RESET_GPIO_Port GPIOC
#define TXD_Pin GPIO_PIN_9
#define TXD_GPIO_Port GPIOA
#define RXD_Pin GPIO_PIN_10
#define RXD_GPIO_Port GPIOA
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

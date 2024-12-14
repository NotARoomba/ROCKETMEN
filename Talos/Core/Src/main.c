/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include <lsm6dsm_reg.h>
#include <stm32f405xx.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
// function prototype
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void platform_delay(uint32_t ms);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
  int16_t temp;
//variable initialization
static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static int16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;
static uint8_t whoamI, rst;
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

   HAL_GPIO_WritePin(CS_GYRO_GPIO_Port, CS_GYRO_Pin, GPIO_PIN_SET);

   // variable initialization for sensor access
  stmdev_ctx_t dev_ctx;
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.handle = &hspi1;

  /* Wait sensor boot time */
  platform_delay(100);
  //set SPI as 3 wire communication
  lsm6dsm_spi_mode_set(&dev_ctx,LSM6DSM_SPI_3_WIRE);
  //set auto increment to read several register at same time
  lsm6dsm_auto_increment_set(&dev_ctx, 1);

  /* Check device ID */
  lsm6dsm_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != LSM6DSM_ID)
    while (1) {
      /* manage here device not found */
    }

  /* Restore default configuration */
  // you should not call this function because it will give uncorrect output register value
  //lsm6dsm_reset_set(&dev_ctx, PROPERTY_ENABLE);

  do {
    lsm6dsm_reset_get(&dev_ctx, &rst);
  } while (rst);

  /*  Enable Block Data Update */
  lsm6dsm_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set Output Data Rate for Acc and Gyro */
  lsm6dsm_xl_data_rate_set(&dev_ctx, LSM6DSM_XL_ODR_12Hz5);
  lsm6dsm_gy_data_rate_set(&dev_ctx, LSM6DSM_GY_ODR_12Hz5);
  /* Set full scale */
  lsm6dsm_xl_full_scale_set(&dev_ctx, LSM6DSM_2g);
  lsm6dsm_gy_full_scale_set(&dev_ctx, LSM6DSM_2000dps);
  /* Configure filtering chain(No aux interface)
   * Accelerometer - analog filter
   */
  lsm6dsm_xl_filter_analog_set(&dev_ctx, LSM6DSM_XL_ANA_BW_400Hz);
  /* Accelerometer - LPF1 path (LPF2 not used) */
  //lsm6dsm_xl_lp1_bandwidth_set(&dev_ctx, LSM6DSM_XL_LP1_ODR_DIV_4);
  /* Accelerometer - LPF1 + LPF2 path */
  lsm6dsm_xl_lp2_bandwidth_set(&dev_ctx,
                               LSM6DSM_XL_LOW_NOISE_LP_ODR_DIV_100);
  /* Accelerometer - High Pass / Slope path */
  //lsm6dsm_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
  //lsm6dsm_xl_hp_bandwidth_set(&dev_ctx, LSM6DSM_XL_HP_ODR_DIV_100);
  /* Gyroscope - filtering chain */
  lsm6dsm_gy_band_pass_set(&dev_ctx, LSM6DSM_HP_260mHz_LP1_STRONG);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // HAL_UART_Transmit(&huart1, (uint8_t*)"Hello World!\r\n", 14, 1000);
    // TEMPERATURE SENSOR INIT

    // if (HAL_I2C_IsDeviceReady(&hi2c1, 0x41 << 1, 2, 1000) == HAL_OK) {
    //   if (HAL_I2C_Master_Transmit(&hi2c1, 0x41 << 1, (uint8_t*)0xBC, 2, 1000) == HAL_OK) {
    //     uint8_t tempData[3];
    //     if (HAL_I2C_Master_Receive(&hi2c1, 0x41 << 1, &tempData, 3, 1000) == HAL_OK) {
    //       temp = (int16_t)((tempData[1] << 8) | tempData[0]);
    //       float tempFloat = ((float)temp/256.0f)+25.0f;
    //       printf("Temperature: %.3f\r\n", tempFloat);
    //     }
    //   }
    // }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
     lsm6dsm_reg_t reg;
	    /* Read output only if new value is available */
	    lsm6dsm_status_reg_get(&dev_ctx, &reg.status_reg);

	    // if (reg.status_reg.xlda) {
	    //   /* Read acceleration field data */
	    //   memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
	    //   lsm6dsm_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
	    //   acceleration_mg[0] =
	    //     lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[0]);
	    //   acceleration_mg[1] =
	    //     lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[1]);
	    //   acceleration_mg[2] =
	    //     lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[2]);
	    //   printf("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
	    //           acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
	    // }

	    if (reg.status_reg.gda) {
	      /* Read angular rate field data */
	      memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
	      lsm6dsm_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
	      angular_rate_mdps[0] =
	        lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[0]);
	      angular_rate_mdps[1] =
	        lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[1]);
	      angular_rate_mdps[2] =
	        lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[2]);
	      // printf("Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n", angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
	      printf("%4.2f,%4.2f,%4.2f\r\n", angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
	    }

	    // if (reg.status_reg.tda) {
	    //   /* Read temperature data */
	    //   memset(&data_raw_temperature, 0x00, sizeof(int16_t));
	    //   lsm6dsm_temperature_raw_get(&dev_ctx, &data_raw_temperature);
	    //   temperature_degC = lsm6dsm_from_lsb_to_celsius(
	    //                        data_raw_temperature);
	    //   printf("Temperature [degC]:%6.2f\r\n",
	    //           temperature_degC);
	    // }
      platform_delay(100);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 130;
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
    if (HAL_I2C_IsDeviceReady(&hi2c1, 0x41 << 1, 2, 1000) != HAL_OK) {
      HAL_UART_Transmit(&huart1, (uint8_t*)"Temperature not ready\r\n", 32, 1000);
    } else {
      HAL_I2C_Mem_Write(&hi2c1, 0x41 << 1, 0x04, 1, (uint8_t*)0x00, 1, 1000);
    }
  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, PARACHUTE_Pin|MOTOR_X_Pin|MOTOR_Y_Pin|RF_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_PRESSURE_Pin|CS_GYRO_Pin|CS_WIRELESS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PARACHUTE_Pin MOTOR_X_Pin MOTOR_Y_Pin RF_RESET_Pin */
  GPIO_InitStruct.Pin = PARACHUTE_Pin|MOTOR_X_Pin|MOTOR_Y_Pin|RF_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : IMU_INT_Pin PRESSURE_INT_Pin */
  GPIO_InitStruct.Pin = IMU_INT_Pin|PRESSURE_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF15_EVENTOUT;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_CD_Pin */
  GPIO_InitStruct.Pin = SD_CD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SD_CD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_PRESSURE_Pin CS_GYRO_Pin CS_WIRELESS_Pin */
  GPIO_InitStruct.Pin = CS_PRESSURE_Pin|CS_GYRO_Pin|CS_WIRELESS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 ANTENNA_IRQ_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_4|ANTENNA_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

int _write(int fd, char* ptr, int len) {
	HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, 0xffffff);
	return len;
}


/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
	  HAL_GPIO_WritePin(GPIOA, CS_GYRO_Pin, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(handle, &reg, 1, 1000);
	  HAL_SPI_Transmit(handle, (uint8_t*) bufp, len, 1000);
	  HAL_GPIO_WritePin(GPIOA, CS_GYRO_Pin, GPIO_PIN_SET);
  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
	  reg |= 0x80;
	  HAL_GPIO_WritePin(GPIOA, CS_GYRO_Pin, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(handle, &reg, 1, 1000);
	  HAL_SPI_Receive(handle, bufp, len, 1000);
	  HAL_GPIO_WritePin(GPIOA, CS_GYRO_Pin, GPIO_PIN_SET);
  return 0;
}

/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
static void platform_delay(uint32_t ms)
{
  HAL_Delay(ms);
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

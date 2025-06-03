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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <lsm6dsm_reg.h>
#include <stm32f405xx.h>
#include <llcc68_hal.h>
#include <llcc68.h>
#include <bmp5.h>
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
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
// function prototype
static int32_t lsm6dsm_platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t lsm6dsm_platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void platform_delay(uint32_t ms);

static int32_t llcc68_platform_write_read(void *handle, const uint8_t *command, uint16_t command_length,
                                          uint8_t *data, uint16_t data_length);
                                          static int32_t llcc68_platform_read(void *handle, const uint8_t *command, uint16_t command_length,
                                          uint8_t *data, uint16_t data_length);
                                          static int32_t llcc68_platform_write(void *handle, const uint8_t *command, uint16_t command_length,
                                          const uint8_t *data, uint16_t data_length);
static int32_t llcc68_platform_reset(void *handle);
static int32_t bmp5_read(uint8_t reg_addr, uint8_t *read_data, uint32_t len, void *intf_ptr);
static int32_t bmp5_write(uint8_t reg_addr,
  const uint8_t * reg_data, uint32_t len, void *intf_ptr);
  void bmp5_error_codes_print_result(const char api_name[], int8_t rslt);
  static int8_t bmp5_set_config(struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev);
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
typedef struct {
  float q; // Process noise covariance
  float r; // Measurement noise covariance
  float x; // Value
  float p; // Estimation error covariance
  float k; // Kalman gain
} KalmanFilter;

void kalman_init(KalmanFilter* kf, float q, float r, float initial_value) {
  kf->q = q;
  kf->r = r;
  kf->x = initial_value;
  kf->p = 1.0f;
  kf->k = 0.0f;
}

float kalman_update(KalmanFilter* kf, float measurement) {
  // Prediction update
  kf->p += kf->q;

  // Measurement update
  kf->k = kf->p / (kf->p + kf->r);
  kf->x += kf->k * (measurement - kf->x);
  kf->p *= (1.0f - kf->k);

  return kf->x;
}
KalmanFilter kalman_acc[3];
KalmanFilter kalman_gyro[3];
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_FATFS_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  //  HAL_GPIO_WritePin(CS_GYRO_GPIO_Port, CS_GYRO_Pin, GPIO_PIN_SET);

   // variable initialization for sensor access
  stmdev_ctx_t dev_ctx;
  dev_ctx.write_reg = lsm6dsm_platform_write;
  dev_ctx.read_reg = lsm6dsm_platform_read;
  dev_ctx.handle = &hspi1;

  llcc68_ctx_t radio_ctx;
  radio_ctx.write_reg = llcc68_platform_write;
  radio_ctx.read_reg = llcc68_platform_read;
  radio_ctx.reset = llcc68_platform_reset;
  radio_ctx.handle = &hspi1;
  radio_ctx.gpio = CS_WIRELESS_GPIO_Port;

  // int8_t rslt;
  // struct bmp5_dev bmp5_ctx;
  // bmp5_ctx.intf = BMP5_SPI_INTF;
  // bmp5_ctx.intf_ptr = &hspi1;
  // bmp5_ctx.read = (bmp5_read_fptr_t) &bmp5_read;
  // bmp5_ctx.write = (bmp5_write_fptr_t) &bmp5_write;
  // bmp5_ctx.delay_us = (bmp5_delay_us_fptr_t) &platform_delay;
  // struct bmp5_osr_odr_press_config osr_odr_press_cfg = { 0 };

  /* Wait sensor boot time */
  platform_delay(1000);

  // PRESSURE SENSOR CONFIGURATION
  // bmp5_read(BMP5_REG_CHIP_ID, &whoamI, 1, &hspi1);
  // printf("Pressure sensor who am I: %d\n", whoamI);
  // rslt = bmp5_init(&bmp5_ctx);
  // bmp5_error_codes_print_result("bmp5_init", rslt);
  // if (rslt == BMP5_OK) {
  //   rslt = bmp5_set_config(&osr_odr_press_cfg, &bmp5_ctx);
  //   bmp5_error_codes_print_result("set_config", rslt);
  // }

  // IMU CONFIGURATION

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
  lsm6dsm_xl_data_rate_set(&dev_ctx, LSM6DSM_XL_ODR_104Hz);
  lsm6dsm_gy_data_rate_set(&dev_ctx, LSM6DSM_XL_ODR_6k66Hz);
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
                               LSM6DSM_XL_LOW_LAT_LP_ODR_DIV_400);
  /* Accelerometer - High Pass / Slope path */
  //lsm6dsm_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
  //lsm6dsm_xl_hp_bandwidth_set(&dev_ctx, LSM6DSM_XL_HP_ODR_DIV_100);
  /* Gyroscope - filtering chain */
  lsm6dsm_gy_band_pass_set(&dev_ctx,  LSM6DSM_HP_DISABLE_LP1_LIGHT     );
  // update the offset bias of acceleration
  platform_delay(2000);

  const int SAMPLE_SIZE = 1000;
  //create a while loop that gets 5 sample of acceleration and calculate the average, then set the offset weight and write it to the sensor taking to account the above
  int16_t acc_bias[3] = {0, 0, 0};
  int i = 0;
  do {
    lsm6dsm_reg_t reg;
      /* Read output only if new value is available */
      lsm6dsm_status_reg_get(&dev_ctx, &reg.status_reg);
      if (reg.status_reg.xlda) {
        /* Read acceleration field data */
        memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
        lsm6dsm_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
        acc_bias[0] += lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[0]);
        acc_bias[1] += lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[1]);
        acc_bias[2] += lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[2]);
        // platform_delay(100);
        i++;
      }
  } while (i < SAMPLE_SIZE);
  acc_bias[0] /= SAMPLE_SIZE;
  acc_bias[1] /= SAMPLE_SIZE;
  acc_bias[2] /= SAMPLE_SIZE;

  acc_bias[0] = 0;
  acc_bias[1] = 0;
  acc_bias[2] = 0;
  // for (int i = 0; i < 10; i++) {
  //   lsm6dsm_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
  //   acc_bias[0] += lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[0]);
  //   acc_bias[1] += lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[1]);
  //   acc_bias[2] += lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[2]);
  //   platform_delay(100);
  // } 
  // acc_bias[0] /= 10;
  // acc_bias[1] /= 10;
  // acc_bias[2] /= 10;

  // lsm6dsm_xl_usr_offset_set(&dev_ctx, (uint8_t*)acc_bias);

  // do the same for gyro
  int16_t gyro_bias[3] = {0, 0, 0};
  i = 0;
  do {
    lsm6dsm_reg_t reg;
	    /* Read output only if new value is available */
	    lsm6dsm_status_reg_get(&dev_ctx, &reg.status_reg);
      if (reg.status_reg.gda) {
        /* Read angular rate field data */
        memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
        lsm6dsm_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
        gyro_bias[0] += lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[0]);
        gyro_bias[1] += lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[1]);
        gyro_bias[2] += lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[2]);
        // platform_delay(100);
        i++;
      }
  } while (i < SAMPLE_SIZE);
  gyro_bias[0] /= SAMPLE_SIZE;
  gyro_bias[1] /= SAMPLE_SIZE;
  gyro_bias[2] /= SAMPLE_SIZE;

  for (int i = 0; i < 3; i++) {
    kalman_init(&kalman_acc[i], 0.1f, 10.0f, 0.0f);
    kalman_init(&kalman_gyro[i], 0.1f, 10.0f, 0.0f);
  }

  gyro_bias[0] = 0;
  gyro_bias[1] = 0;
  gyro_bias[2] = 0;

  // LORA MODULE CONFIGURATION
  // llcc68_reset(&radio_ctx);
  // platform_delay(100);
  //   llcc68_set_standby(&radio_ctx, LLCC68_STANDBY_CFG_RC);
  //   llcc68_set_pkt_type(&radio_ctx, LLCC68_PKT_TYPE_LORA);
  //   llcc68_set_rf_freq(&radio_ctx, 433000000);
  // //   // // 17dBm
  // //   // // {0x03, 0x05, 0x00, 0x01}
  //   const llcc68_pa_cfg_params_t pa_cfg = {0x03,
  //     0x05,
  //    0x00,
  //     0x01
  //   };
  //   const llcc68_pkt_params_lora_t pkt_params = {14, LLCC68_LORA_PKT_EXPLICIT, 14, true, false};
  //   const llcc68_mod_params_lora_t mod_params = {LLCC68_LORA_SF11, LLCC68_LORA_BW_250, LLCC68_LORA_CR_4_5, 0};
  //   llcc68_set_pa_cfg(&radio_ctx, &pa_cfg);
  //   // //The output power is defined as power in dBm in a range of - 9 (0xF7) to +22 (0x16) dBm by step of 1 dB
  //   llcc68_set_tx_params(&radio_ctx, 0x16, LLCC68_RAMP_200_US);
  //   // llcc68_set_lora_mod_params(&radio_ctx, &mod_params);
  //   // llcc68_set_lora_pkt_params(&radio_ctx, &pkt_params);
  //    platform_delay(100);

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
	    if (reg.status_reg.xlda) {
	      /* Read acceleration field data */
	      memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
	      lsm6dsm_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
	      acceleration_mg[0] = kalman_update(&kalman_acc[0], 
          lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[0]) - acc_bias[0]);
acceleration_mg[1] = kalman_update(&kalman_acc[1], 
          lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[1]) - acc_bias[1]);
acceleration_mg[2] = kalman_update(&kalman_acc[2], 
          lsm6dsm_from_fs2g_to_mg(data_raw_acceleration[2]) - acc_bias[2]);
	      // printf("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
	      //         acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
        // printf("ACCEL:%4.2f,%4.2f,%4.2f\r\n", acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
	    } else {
        acceleration_mg[0] = 0;
        acceleration_mg[1] = 0;
        acceleration_mg[2] = 0;
      }

	    if (reg.status_reg.gda) {
	      /* Read angular rate field data */
	      memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
	      lsm6dsm_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
	      angular_rate_mdps[0] = kalman_update(&kalman_gyro[0], 
          lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[0]) - gyro_bias[0]);
angular_rate_mdps[1] = kalman_update(&kalman_gyro[1], 
          lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[1]) - gyro_bias[1]);
angular_rate_mdps[2] = kalman_update(&kalman_gyro[2], 
          lsm6dsm_from_fs2000dps_to_mdps(data_raw_angular_rate[2]) - gyro_bias[2]);
	      // printf("Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n", angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
	      // printf("ANGLE:%4.2f,%4.2f,%4.2f\r\n", angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
	    } 
	    if (reg.status_reg.tda) {
	      /* Read temperature data */
	      memset(&data_raw_temperature, 0x00, sizeof(int16_t));
	      lsm6dsm_temperature_raw_get(&dev_ctx, &data_raw_temperature);
	      temperature_degC = lsm6dsm_from_lsb_to_celsius(
	                           data_raw_temperature);
	      // printf("Temperature [degC]:%6.2f\r\n",
	      //         temperature_degC);
        // printf("TEMP:%.2f,101.325\r\n", temperature_degC);
	    }
      printf("%4.2f,%4.2f,%4.2f,%4.2f,%4.2f,%4.2f,%.2f,101.325\r\n", acceleration_mg[0], acceleration_mg[1], acceleration_mg[2], angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2], temperature_degC);
      // acceleration_mg[0] = 0;
      // acceleration_mg[1] = 0;
      // acceleration_mg[2] = 0;
      // angular_rate_mdps[0] = 0;
      // angular_rate_mdps[1] = 0;
      // angular_rate_mdps[2] = 0;
      // temperature_degC = 0;


      // uint8_t int_status;
      // struct bmp5_sensor_data bmp5_data;
      //   rslt = bmp5_get_interrupt_status(&int_status, &bmp5_ctx);
      //   bmp5_error_codes_print_result("bmp5_get_interrupt_status", rslt);

      //   if (int_status & BMP5_INT_ASSERTED_DRDY)
      //   {
      //       rslt = bmp5_get_sensor_data(&bmp5_data, &osr_odr_press_cfg, &bmp5_ctx);
      //       bmp5_error_codes_print_result("bmp5_get_sensor_data", rslt);

      //       if (rslt == BMP5_OK)
      //       {
      //           printf("%f\r\n", bmp5_data.pressure);
      //       }
      //   }
    //   llcc68_set_buffer_base_address(&radio_ctx, 0x00, 0x00);
    // llcc68_write_buffer(&radio_ctx, 0x00, (uint8_t*)"Hello World!\r\n", 14);
    // llcc68_set_lora_mod_params(&radio_ctx, &mod_params);
    // llcc68_set_lora_pkt_params(&radio_ctx, &pkt_params);
    // llcc68_set_dio_irq_params(&radio_ctx, LLCC68_IRQ_TX_DONE | LLCC68_IRQ_RX_DONE, LLCC68_IRQ_TX_DONE | LLCC68_IRQ_RX_DONE, LLCC68_IRQ_NONE, LLCC68_IRQ_NONE);

    // llcc68_write_register(&radio_ctx, 0x06C0, (uint8_t)0x12, 1);
    // llcc68_write_register(&radio_ctx, 0x06C1, (uint8_t)0x23, 1);
    // llcc68_write_register(&radio_ctx, 0x06C2, (uint8_t)0x34, 1);
    // llcc68_write_register(&radio_ctx, 0x06C3, (uint8_t)0x45, 1);
    // llcc68_write_register(&radio_ctx, 0x06C4, (uint8_t)0x56, 1);
    // llcc68_write_register(&radio_ctx, 0x06C5, (uint8_t)0x67, 1);
    // llcc68_write_register(&radio_ctx, 0x06C6, (uint8_t)0x78, 1);
    // llcc68_write_register(&radio_ctx, 0x06C7, (uint8_t)0x89, 1);

    // llcc68_set_tx(&radio_ctx, 100);
    // platform_delay(100);
    // //  llcc68_set_standby(&radio_ctx, LLCC68_STANDBY_CFG_RC);
    //   llcc68_chip_status_t chip_status;
    //   llcc68_get_status(&radio_ctx, &chip_status);
    //   printf("Chip Mode: %d, Chip Status: %d\r\n", chip_status.chip_mode, chip_status.cmd_status);
    //  // print if busy or not with busy gpio pin
    //  HAL_GPIO_ReadPin(BUSY_GPIO_Port, BUSY_Pin) == GPIO_PIN_SET ? printf("Busy\r\n") : printf("Not Busy\r\n");
    //   platform_delay(100);
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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 128;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
  hspi1.Init.NSS = SPI_NSS_SOFT;
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

  /*Configure GPIO pin : BUSY_Pin */
  GPIO_InitStruct.Pin = BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUSY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ANTENNA_IRQ_Pin */
  GPIO_InitStruct.Pin = ANTENNA_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ANTENNA_IRQ_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

int _write(int fd, char* ptr, int len) {
	HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, 0xffffff);
	return len;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == ANTENNA_IRQ_Pin) {
    printf("IRQ HANDLEDDD FORM THE ANTENAAAAAAA");
  } else {
      __NOP();
  }
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
static int32_t lsm6dsm_platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
	  HAL_GPIO_WritePin(CS_GYRO_GPIO_Port, CS_GYRO_Pin, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(handle, &reg, 1, 1000);
	  HAL_SPI_Transmit(handle, (uint8_t*) bufp, len, 1000);
	  HAL_GPIO_WritePin(CS_GYRO_GPIO_Port, CS_GYRO_Pin, GPIO_PIN_SET);
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
static int32_t lsm6dsm_platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
	  reg |= 0x80;
	  HAL_GPIO_WritePin(CS_GYRO_GPIO_Port, CS_GYRO_Pin, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(handle, &reg, 1, 1000);
	  HAL_SPI_Receive(handle, bufp, len, 1000);
	  HAL_GPIO_WritePin(CS_GYRO_GPIO_Port, CS_GYRO_Pin, GPIO_PIN_SET);
  return 0;
}

static int32_t llcc68_platform_reset(void *handle) {
  HAL_GPIO_WritePin(RF_RESET_GPIO_Port, RF_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(RF_RESET_GPIO_Port, RF_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  return 0;
}

static int32_t bmp5_read(uint8_t reg_addr, uint8_t * reg_data, uint32_t len, void *intf_ptr) {
  HAL_GPIO_WritePin(CS_PRESSURE_GPIO_Port, CS_PRESSURE_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &reg_addr, 1, 1000);
	  HAL_SPI_Receive(&hspi1, reg_data, len, 1000);
	  HAL_GPIO_WritePin(CS_PRESSURE_GPIO_Port, CS_PRESSURE_Pin, GPIO_PIN_SET);
  return 0;
}

static int32_t bmp5_write(uint8_t reg_addr,
  const uint8_t * reg_data, uint32_t len, void *intf_ptr) {
  HAL_GPIO_WritePin(CS_PRESSURE_GPIO_Port, CS_PRESSURE_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &reg_addr, 1, 1000);
	HAL_SPI_Transmit(&hspi1, (uint8_t*) reg_data, len, 1000);
  HAL_GPIO_WritePin(CS_PRESSURE_GPIO_Port, CS_PRESSURE_Pin, GPIO_PIN_SET);
  return 0;
}

/*!
 *  @brief Prints the execution status of the APIs.
 */
void bmp5_error_codes_print_result(const char api_name[], int8_t rslt)
{
    if (rslt != BMP5_OK)
    {
        printf("%s\t", api_name);
        if (rslt == BMP5_E_NULL_PTR)
        {
            printf("Error [%d] : Null pointer\r\n", rslt);
        }
        else if (rslt == BMP5_E_COM_FAIL)
        {
            printf("Error [%d] : Communication failure\r\n", rslt);
        }
        else if (rslt == BMP5_E_DEV_NOT_FOUND)
        {
            printf("Error [%d] : Device not found\r\n", rslt);
        }
        else if (rslt == BMP5_E_INVALID_CHIP_ID)
        {
            printf("Error [%d] : Invalid chip id\r\n", rslt);
        }
        else if (rslt == BMP5_E_POWER_UP)
        {
            printf("Error [%d] : Power up error\r\n", rslt);
        }
        else if (rslt == BMP5_E_POR_SOFTRESET)
        {
            printf("Error [%d] : Power-on reset/softreset failure\r\n", rslt);
        }
        else if (rslt == BMP5_E_INVALID_POWERMODE)
        {
            printf("Error [%d] : Invalid powermode\r\n", rslt);
        }
        else
        {
            /* For more error codes refer "*_defs.h" */
            printf("Error [%d] : Unknown error code\r\n", rslt);
        }
    }
}
static int8_t bmp5_set_config(struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt;
    struct bmp5_iir_config set_iir_cfg;
    struct bmp5_int_source_select int_source_select;

    rslt = bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, dev);
    bmp5_error_codes_print_result("bmp5_set_power_mode1", rslt);

    if (rslt == BMP5_OK)
    {
        /* Get default odr */
        rslt = bmp5_get_osr_odr_press_config(osr_odr_press_cfg, dev);
        bmp5_error_codes_print_result("bmp5_get_osr_odr_press_config", rslt);

        if (rslt == BMP5_OK)
        {
            /* Set ODR as 50Hz */
            osr_odr_press_cfg->odr = BMP5_ODR_50_HZ;

            /* Enable pressure */
            osr_odr_press_cfg->press_en = BMP5_ENABLE;

            /* Set Over-sampling rate with respect to odr */
            osr_odr_press_cfg->osr_t = BMP5_OVERSAMPLING_64X;
            osr_odr_press_cfg->osr_p = BMP5_OVERSAMPLING_4X;

            rslt = bmp5_set_osr_odr_press_config(osr_odr_press_cfg, dev);
            bmp5_error_codes_print_result("bmp5_set_osr_odr_press_config", rslt);
        }

        if (rslt == BMP5_OK)
        {
            set_iir_cfg.set_iir_t = BMP5_IIR_FILTER_COEFF_1;
            set_iir_cfg.set_iir_p = BMP5_IIR_FILTER_COEFF_1;
            set_iir_cfg.shdw_set_iir_t = BMP5_ENABLE;
            set_iir_cfg.shdw_set_iir_p = BMP5_ENABLE;

            rslt = bmp5_set_iir_config(&set_iir_cfg, dev);
            bmp5_error_codes_print_result("bmp5_set_iir_config", rslt);
        }

        if (rslt == BMP5_OK)
        {
            rslt = bmp5_configure_interrupt(BMP5_PULSED, BMP5_ACTIVE_HIGH, BMP5_INTR_PUSH_PULL, BMP5_INTR_ENABLE, dev);
            bmp5_error_codes_print_result("bmp5_configure_interrupt", rslt);

            if (rslt == BMP5_OK)
            {
                /* Note : Select INT_SOURCE after configuring interrupt */
                int_source_select.drdy_en = BMP5_ENABLE;
                rslt = bmp5_int_source_select(&int_source_select, dev);
                bmp5_error_codes_print_result("bmp5_int_source_select", rslt);
            }
        }

        /* Set powermode as normal */
        rslt = bmp5_set_power_mode(BMP5_POWERMODE_NORMAL, dev);
        bmp5_error_codes_print_result("bmp5_set_power_mode", rslt);
    }

    return rslt;
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

static int32_t llcc68_platform_read(void *handle, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ) {
  HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, command, command_length, 1000);
	  HAL_SPI_Receive(&hspi1, data, data_length, 1000);
	  HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_SET);
  return 0;
}

static int32_t llcc68_platform_write(void *handle, const uint8_t* command, const uint16_t command_length,
                                     const uint8_t* data, const uint16_t data_length ) {
  HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, command, command_length, 1000);
  HAL_SPI_Transmit(&hspi1, data, data_length, 1000);
  HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_SET);
  return 0;
}

static int32_t llcc68_platform_write_read(void *handle, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ) {
 uint8_t res;
    
    /* set cs low */
    HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_RESET);
    
    /* if in_len > 0 */
    if (command_length > 0)
    {
        /* transmit the input buffer */
        res = HAL_SPI_Transmit(&hspi1, command, command_length, 1000);
        if (res != HAL_OK)
        {
            /* set cs high */
            HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_SET);
           
            return 1;
        }
    }
    
    /* if out_len > 0 */
    if (data_length > 0)
    {
        /* transmit to the output buffer */
        res = HAL_SPI_Receive(&hspi1, data, data_length, 1000);
        if (res != HAL_OK)
        {
            /* set cs high */
            HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_SET);
           
            return 1;
        }
    }
    
    /* set cs high */
    HAL_GPIO_WritePin(CS_WIRELESS_GPIO_Port, CS_WIRELESS_Pin, GPIO_PIN_SET);
    
    return 0;
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

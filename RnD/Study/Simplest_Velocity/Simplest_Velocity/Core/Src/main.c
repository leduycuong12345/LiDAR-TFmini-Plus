/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stdio.h"
#include "string.h"

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
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void displayOutput_UART2()
{
  char msg2[] = "UART2 OK\r\n";

  HAL_UART_Transmit(&huart2,
                      (uint8_t *)msg2,
                      sizeof(msg2) - 1,
                      1000);
    HAL_Delay(1000);
}
void displayOutput_UART1()
{
  char msg2[] = "UART1 OK\r\n";

  HAL_UART_Transmit(&huart1,
                      (uint8_t *)msg2,
                      sizeof(msg2) - 1,
                      1000);
    HAL_Delay(1000);
}
void displayOutput_UART3()
{
  char msg2[] = "UART3 OK\r\n";

  HAL_UART_Transmit(&huart3,
                      (uint8_t *)msg2,
                      sizeof(msg2) - 1,
                      1000);
    HAL_Delay(1000);
}

//init velocity
static uint8_t  rx_byte;            /* single byte received by IT */
static uint8_t  frame[9];
static uint8_t  idx = 0;
 
static volatile uint8_t  data_ready = 0;
static volatile uint16_t lidar_dist = 0;   /* cm */
static volatile uint16_t lidar_str  = 0;
static volatile float    lidar_temp = 0;   /* deg C */

volatile uint32_t rx_count = 0;
volatile uint32_t err_count = 0;
volatile uint32_t frame_ok = 0;
volatile uint32_t frame_bad = 0;
uint8_t last[9];

static uint16_t previousDistance = 0;
static uint32_t previousTime = 0;
static uint8_t firstRun = 1;

//init velocity
static void tfmini_feed(uint8_t b)
{
    /* sync on two 0x59 header bytes */
    if (idx == 0 && b != 0x59) return;
    if (idx == 1 && b != 0x59) { idx = 0; return; }
 
    frame[idx++] = b;
 
    if (idx == 9) {
        idx = 0;
        uint8_t sum = 0;
        for (int i = 0; i < 8; i++) sum += frame[i];
 
        if (sum == frame[8]) {
            lidar_dist = frame[2] | (frame[3] << 8);
            lidar_str  = frame[4] | (frame[5] << 8);
            lidar_temp = (frame[6] | (frame[7] << 8)) / 8.0f - 256.0f;
            data_ready = 1;
        }
    }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        rx_count++;
        tfmini_feed(rx_byte);
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        err_count++;
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

double calculateVelocity(uint16_t distance)
{
    uint32_t currentTime = HAL_GetTick();

    

    if (firstRun)
    {
        previousDistance = distance;
        previousTime = currentTime;
        firstRun = 0;
        return 0.0f;
    }

    uint32_t deltaTime = currentTime - previousTime;

    if (deltaTime == 0)
        return 0.0f;

    uint16_t deltaDistance =
        (uint16_t)distance - (uint16_t)previousDistance;

    // mm/ms = m/s
    uint16_t velocity = (uint16_t)( deltaDistance * 1000) / (uint16_t)deltaTime;

    uint16_t  oldPreviousDistance=previousDistance;
    previousDistance = distance;
    previousTime = currentTime;

    //logging for debuging the calculate
    char velocityMsg[200];
    int len =snprintf(velocityMsg, sizeof(velocityMsg),
                               "At calculateVelocity previousDistance: %.1d , currenDistance:  %.1d , deltaDistance: %.1d / detalTime: %lu  \r\n = calculated_velocity: %.1d \r\n",
                               oldPreviousDistance,distance,deltaDistance,deltaTime,velocity);

    HAL_UART_Transmit(&huart2,
                                  (uint8_t *)velocityMsg,
                                  len,
                                  1000);
    //ending logging for debugging the calculate

    return velocity;
}
/* ---------- USER CODE BEGIN 0 ---------- */

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();//testing output COM
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
  char msg[100];
  char m[80];
  char velocityMsg[100];
  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);//test code
    //displayOutput_UART1();//test uẩrt1
    //displayOutput_UART2();//test uẩrt2
    //displayOutput_UART3();//test uẩrt3
    
    int n = snprintf(m, sizeof(m), "rx=%lu err=%lu ok=%lu bad=%lu\r\n",
                     rx_count, err_count, frame_ok, frame_bad);
    HAL_UART_Transmit(&huart2, (uint8_t*)m, n, 100);
    HAL_Delay(1000);

    if (data_ready) {
            data_ready = 0;
            int len = snprintf(msg, sizeof(msg),
                               "Dist: %u cm | Str: %u | Temp: %.1f C\r\n",
                               lidar_dist, lidar_str, lidar_temp);
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, 100);
            
            uint16_t simple_velocity=calculateVelocity(lidar_dist);
    
            int len2 =snprintf(velocityMsg, sizeof(velocityMsg),
                               "Simple velocity of static_lidar: %.1d cm/s (chu ky 10ms 100hz 100lan/s) \r\n",
                               simple_velocity);

            HAL_UART_Transmit(&huart2,
                                  (uint8_t *)velocityMsg,
                                  len2,
                                  1000);
    }   
    else
    {
      sprintf(msg,"no new data from lidar yet \r\n");
      HAL_UART_Transmit(&huart2,
                              (uint8_t*)msg,
                              strlen(msg),
                              HAL_MAX_DELAY);
    }
  
    HAL_Delay(1000);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure PC13 as Output */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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

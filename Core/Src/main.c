/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PCA9685.h"
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

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

const uint16_t PCA9685_ID = 0x80; //PCA9685 I2C bus adress
uint8_t delay = 0; //%
uint8_t dutyCycle = 0; //%
uint16_t frequency = 200; //default value of PWM frequency
uint8_t onOffAllChannels = 0; //switch on/off all channels
uint8_t sleepMode = 0; //switch on/off sleep mode

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART3_UART_Init(void);
void UART_Text(uint8_t onOffAllChannels, uint16_t frequency, uint8_t delay, uint8_t dutyCycle, uint8_t sleepMode);
/* USER CODE BEGIN PFP */

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

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	PCA9685_Init(&hi2c1, PCA9685_ID);
	UART_Text(onOffAllChannels, frequency, delay, dutyCycle, sleepMode);

	/*
	 PCA9685_Channel_Set(&hi2c1, PCA9685_ID, LED0, 0, 50);//individually configurable PWM channels
	 PCA9685_Channel_Reset(&hi2c1, PCA9685_ID, LED0);//individually reset PWM channels
	 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		uint8_t rcvBuf[1];
		HAL_StatusTypeDef result;
		result = HAL_UART_Receive(&huart3, rcvBuf, 1, 10);
		if (result == HAL_OK)
		{
			switch (rcvBuf[0])
			{
			case '1':
			{
				if (onOffAllChannels == 1)
				{
					delay = (delay < 100) ? (delay + 1) : delay;
					delay = (delay > (100 - dutyCycle)) ? (100 - dutyCycle) : delay; // dutyCycle + delay <= 100%. If not, the delay variable value will be adjusted.
				}
				else
					delay = 0;
				PCA9685_Channel_Set_All(&hi2c1, PCA9685_ID, delay, dutyCycle);
			}
				break;

			case '2':
			{
				if (onOffAllChannels == 1)
				{
					delay = (delay > 0) ? (delay - 1) : delay;
					delay = (delay > (100 - dutyCycle)) ? (100 - dutyCycle) : delay; // dutyCycle + delay <= 100%. If not, the delay variable value will be adjusted.
				}
				else
					delay = 0;
				PCA9685_Channel_Set_All(&hi2c1, PCA9685_ID, delay, dutyCycle);
			}
				break;

			case '3':
			{
				if (onOffAllChannels == 1)
				{
					dutyCycle = (dutyCycle < 100) ? (dutyCycle + 1) : dutyCycle;
					dutyCycle = (dutyCycle > (100 - delay)) ? (100 - delay) : dutyCycle; // dutyCycle + delay <= 100%. If not, the dutyCycle variable value will be adjusted.
				}
				else
					dutyCycle = 0;
				PCA9685_Channel_Set_All(&hi2c1, PCA9685_ID, delay, dutyCycle);
			}
				break;

			case '4':
			{
				if (onOffAllChannels == 1)
				{
					dutyCycle = (dutyCycle > 0) ? (dutyCycle - 1) : dutyCycle;
					dutyCycle = (dutyCycle > (100 - delay)) ? (100 - delay) : dutyCycle; // dutyCycle + delay <= 100%. If not, the dutyCycle variable value will be adjusted.
				}
				else
					dutyCycle = 0;
				PCA9685_Channel_Set_All(&hi2c1, PCA9685_ID, delay, dutyCycle);
			}
				break;

			case '5':
			{
				frequency = (frequency < 1526) ? (frequency + 1) : frequency;
				PCA9685_Frequency_Set(&hi2c1, PCA9685_ID, frequency);
			}
				break;

			case '6':
			{
				frequency = (frequency > 23) ? (frequency - 1) : frequency;
				PCA9685_Frequency_Set(&hi2c1, PCA9685_ID, frequency);
			}
				break;

			case '7':
			{
				onOffAllChannels = (onOffAllChannels == 1) ? 0 : 1;
				if (onOffAllChannels == 1)
					PCA9685_Channel_Set_All(&hi2c1, PCA9685_ID, delay, dutyCycle);
				else
					PCA9685_Channel_Reset_All(&hi2c1, PCA9685_ID);
			}
				break;

			case '8':
			{
				sleepMode = (sleepMode == 1) ? 0 : 1;
				PCA9685_Sleep_Mode(&hi2c1, PCA9685_ID, sleepMode);
			}
				break;
			}
			UART_Text(onOffAllChannels, frequency, delay, dutyCycle, sleepMode);
		}
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
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

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
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
	hi2c1.Init.ClockSpeed = 50000;
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
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

	/*Configure GPIO pin : PB7 */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void UART_Text(uint8_t onOffAllChannels, uint16_t frequency, uint8_t delay, uint8_t dutyCycle, uint8_t sleepMode)
{
	uint8_t clr_scr[5] =
	{ 0x1B, 0x5B, 0x32, 0x4A };
	HAL_UART_Transmit(&huart3, (uint8_t*) &clr_scr, 4, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "Channels: all, ", 15, 10);
	if (onOffAllChannels == 1)
		HAL_UART_Transmit(&huart3, (uint8_t*) "on ", 4, 10);
	else
		HAL_UART_Transmit(&huart3, (uint8_t*) "off ", 4, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "| frequency:", 12, 10);
	char frequency_str[4];
	itoa(frequency, frequency_str, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) &frequency_str, 4, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "Hz | delay:", 11, 10);
	char delay_str[3];
	itoa(delay, delay_str, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) &delay_str, 3, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "% | duty Cycle:", 15, 10);
	char dutyCycle_str[3];
	itoa(dutyCycle, dutyCycle_str, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) &dutyCycle_str, 3, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "% | sleep mode:", 15, 10);
	if (sleepMode == 1)
		HAL_UART_Transmit(&huart3, (uint8_t*) "on", 2, 10);
	else
		HAL_UART_Transmit(&huart3, (uint8_t*) "off", 3, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "\r\n", 2, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "1/2 - +/- delay\r\n", 15 + 2, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "3/4 - +/- duty cycle\r\n", 20 + 2, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "5/6 - +/- PWM frequency\r\n", 23 + 2, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "7 - enable/disable all channels\r\n", 31 + 2, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "8 - sleep mode on/off\r\n", 21 + 2, 10);
	HAL_UART_Transmit(&huart3, (uint8_t*) "note: duty cycle + delay <=100%\r\n", 31 + 2, 10);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

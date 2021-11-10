/*
 * PCA9685.c
 *
 *  Created on: Nov 7, 2021
 *      Author:  Konstantin Rudenko
 */
/* Private includes ----------------------------------------------------------*/
#include "PCA9685.h"
/* Private variables ---------------------------------------------------------*/
static uint8_t MODE0_reg = 0x00;
static uint8_t LED_ON_L_reg;
static uint8_t LED_ON_H_reg;
static uint8_t LED_OFF_L_reg;
static uint8_t LED_OFF_H_reg;
static uint8_t PRE_SCALE_reg = 0xFE;
static uint32_t LedON;
static uint32_t LedOFF;
/* Private code ---------------------------------------------------------*/

static uint32_t Round_Int_To(uint32_t arg, uint32_t divider)
{
	if (arg % divider < (divider / 2))
		return arg - arg % divider;
	else if (arg % divider >= (divider / 2))
		return arg - arg % divider + divider;
	else
		return arg;
}

static void PCA9685_I2C_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t Reg, uint32_t Data)
{
	uint8_t TxBuffer[2];
	TxBuffer[0] = Reg;
	TxBuffer[1] = Data;
	HAL_I2C_Master_Transmit(hi2c, DevAddress, (uint8_t*) &TxBuffer, 2, 1000);
}

/**
 * @brief Start in Normal Mode, with default frequency 200Hz.
 */
void PCA9685_Init(I2C_HandleTypeDef *hi2c, uint16_t devID)
{
	HAL_Delay(10); //init timeout
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); //OE
	PCA9685_I2C_Transmit(hi2c, devID, MODE0_reg, 0x10); // enable Sleep mode to change PRE_SCAL: set Sleep bit D4 in 1.
	PCA9685_I2C_Transmit(hi2c, devID, PRE_SCALE_reg, 0x1E); //set default frequency 200Hz
	PCA9685_I2C_Transmit(hi2c, devID, MODE0_reg, 0x01); // MODE0 (disable Sleep mode: set Sleep bit D4 in 0, and ALLCALL bit is 1).
}

void PCA9685_Channel_Set(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t channel_numb, uint8_t delay, uint8_t dutyCycle)
{
	if (channel_numb < LED_ALL)
	{
		LED_ON_L_reg = 6 + (channel_numb << 2);
		LED_ON_H_reg = 7 + (channel_numb << 2);
		LED_OFF_L_reg = 8 + (channel_numb << 2);
		LED_OFF_H_reg = 9 + (channel_numb << 2);
	}
	else
		return;
	LedON = Round_Int_To((delay << 12), 100) / 100; // = (delay * 4096) / 100;
	LedOFF = (dutyCycle > 0) ? (LedON + Round_Int_To((dutyCycle << 12), 100) / 100 - 1) : 0; // = LedON + (dutyCycle * 4096) / 100 - 1;
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_L_reg, (LedON & 0xFF));
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_H_reg, (LedON >> 8));
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_L_reg, (LedOFF & 0xFF));
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_H_reg, (LedOFF >> 8));
}


void PCA9685_Channel_Set_All(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t delay, uint8_t dutyCycle)
{
	LED_ON_L_reg = 0xFA;
	LED_ON_H_reg = 0xFB;
	LED_OFF_L_reg = 0xFC;
	LED_OFF_H_reg = 0xFD;
	LedON = Round_Int_To((delay << 12), 100) / 100; // = (delay * 4096) / 100;
	LedOFF = (dutyCycle > 0) ? (LedON + Round_Int_To((dutyCycle << 12), 100) / 100 - 1) : 0; // = LedON + (dutyCycle * 4096) / 100 - 1;
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_L_reg, (LedON & 0xFF));
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_H_reg, (LedON >> 8));
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_L_reg, (LedOFF & 0xFF));
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_H_reg, (LedOFF >> 8));
}

void PCA9685_Channel_Reset(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t channel_numb)
{
	if (channel_numb < LED_ALL)
	{
		LED_ON_L_reg = 6 + (channel_numb << 2);
		LED_ON_H_reg = 7 + (channel_numb << 2);
		LED_OFF_L_reg = 8 + (channel_numb << 2);
		LED_OFF_H_reg = 9 + (channel_numb << 2);
	}
	else
		return;
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_L_reg, 0x00);
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_H_reg, 0x00);
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_L_reg, 0x00);
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_H_reg, 0x00);
}

void PCA9685_Channel_Reset_All(I2C_HandleTypeDef *hi2c, uint16_t devID)
{
	LED_ON_L_reg = 0xFA;
	LED_ON_H_reg = 0xFB;
	LED_OFF_L_reg = 0xFC;
	LED_OFF_H_reg = 0xFD;
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_L_reg, 0x00);
	PCA9685_I2C_Transmit(hi2c, devID, LED_ON_H_reg, 0x00);
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_L_reg, 0x00);
	PCA9685_I2C_Transmit(hi2c, devID, LED_OFF_H_reg, 0x00);
}

/**
 * @brief Enable/Disable Sleep Mode.
 */
void PCA9685_Sleep_Mode(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t flag)
{
	if (flag == 1)
	{
		PCA9685_I2C_Transmit(hi2c, devID, MODE0_reg, 0x10);  // enable Sleep mode: set Sleep bit D4 in 1
	}
	else
	{
		HAL_Delay(10); //init timeout
		PCA9685_I2C_Transmit(hi2c, devID, MODE0_reg, 0x01); // MODE0 (disable Sleep mode: set Sleep bit D4 in 0, and ALLCALL bit is 1).
	}
}

/**
 * @brief 23Hz < frequency < 1526Hz. To set defalut value of frequency (200Hz) - enter frequency = 0.
 */
void PCA9685_Frequency_Set(I2C_HandleTypeDef *hi2c, uint16_t devID, uint16_t frequency)
{
	uint32_t prescale = Round_Int_To((25000000 / (frequency << 12) * 10), 10) / 10 - 1; // = 25MHg /(4096 * frequency) -1, where 2500000 - 25 MHz internal oscillator clock frequency.
	prescale = (frequency == 0) ? 0x1E : prescale; //set default 200Hz if frequancy argument is 0.
	prescale = (prescale > 0xFF) ? 0xFF : prescale;
	prescale = (prescale < 0x03) ? 0x03 : prescale;

	uint8_t TxBuffer[2];
	TxBuffer[0] = MODE0_reg;
	TxBuffer[1] = 0x21; // enable AI
	HAL_I2C_Master_Transmit(hi2c, devID, (uint8_t*) &TxBuffer, 2, 10);

	uint8_t Buffer[70] =
	{ 0 };

	HAL_I2C_Master_Receive(hi2c, devID, (uint8_t*) &Buffer, 70, 100); //read and store all registers from the device into an array
	for (int i = sizeof(Buffer) - 1; i > 0; i--)
		Buffer[i] = Buffer[i - 2];
	Buffer[0] = MODE0_reg;
	Buffer[1] = 0x21;

	TxBuffer[0] = MODE0_reg;
	TxBuffer[1] = 0x10; // enable Sleep mode to change PRE_SCAL: set Sleep bit D4 in 1.
	HAL_I2C_Master_Transmit(hi2c, devID, (uint8_t*) &TxBuffer, 2, 10);

	TxBuffer[0] = PRE_SCALE_reg;
	TxBuffer[1] = prescale; //set frequency
	HAL_I2C_Master_Transmit(hi2c, devID, (uint8_t*) &TxBuffer, 2, 10); //   load all settings from the array..
	HAL_I2C_Master_Transmit(hi2c, devID, (uint8_t*) &Buffer, 70, 100); // ..into the device registers. Disable sleep mode, start with normal mode.
}


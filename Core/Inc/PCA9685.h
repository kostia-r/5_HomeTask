/*
 * PCA9685.h
 *
 *  Created on: Nov 7, 2021
 *      Author: Konstantin Rudenko
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_PCA9685_H_
#define INC_PCA9685_H_
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx_hal.h"
/* Defines -----------------------------------------------------------*/
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4
#define LED5 5
#define LED6 6
#define LED7 7
#define LED8 8
#define LED9 9
#define LED10 10
#define LED11 11
#define LED12 12
#define LED13 13
#define LED14 14
#define LED15 15
#define LED_ALL 16
/* Exported functions prototypes ---------------------------------------------*/
void PCA9685_Init(I2C_HandleTypeDef *hi2c, uint16_t devID);
void PCA9685_Channel_Set(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t channel_numb, uint8_t delay, uint8_t dutyCycle);
void PCA9685_Channel_Set_All(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t delay, uint8_t dutyCycle);
void PCA9685_Channel_Reset(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t channel_numb);
void PCA9685_Channel_Reset_All(I2C_HandleTypeDef *hi2c, uint16_t devID);
void PCA9685_Sleep_Mode(I2C_HandleTypeDef *hi2c, uint16_t devID, uint8_t flag);
void PCA9685_Frequency_Set(I2C_HandleTypeDef *hi2c, uint16_t devID, uint16_t frequency);
#endif /* INC_PCA9685_H_ */

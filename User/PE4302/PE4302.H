#ifndef _PE4302_H_
#define _PE4302_H_

#include "delay.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "sys.h"

// #define PE_LE_0					PCout(9)
// #define PE_CLK					PCout(10)
// #define PE_DAT					PCout(11)

#define PE_LE_0_EN      HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_SET)
#define PE_LE_0_DIS     HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET)

#define PE_CLK_0        HAL_GPIO_WritePin(PE_CLK_GPIO_Port, PE_CLK_Pin, GPIO_PIN_RESET)//PE_CLK = 0
#define PE_CLK_1        HAL_GPIO_WritePin(PE_CLK_GPIO_Port, PE_CLK_Pin, GPIO_PIN_SET)//PE_CLK = 1

#define PE_DAT_0        HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_RESET)//PE_DAT = 0
#define PE_DAT_1        HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_SET)//PE_DAT = 1

// #define PE_LE_0					PCout(1)
// #define PE_CLK					PCout(10)
// #define PE_DAT					PCout(11)

// #define PE_LE_0_EN      PE_LE_0 = 1
// #define PE_LE_0_DIS     PE_LE_0 = 0

// #define PE_CLK_0        PE_CLK = 0
// #define PE_CLK_1        PE_CLK = 1

// #define PE_DAT_0        PE_DAT = 0
// #define PE_DAT_1        PE_DAT = 1

void PE_GPIO_Init(void);
void PE4302_0_Set(unsigned char db);
void PE4302_1_Set(unsigned char db);
#endif

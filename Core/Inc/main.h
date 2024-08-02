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

#define AMPLITUDE_INDEX 0
#define MODULATION_DEGREE_INDEX 1

#define CW_MODE 0
#define AM_MODE 1

extern int ModulationDegree_index;
extern int Amplitude_index;
extern int Frequency_index;
extern int DelayTime_index;
extern int AMwave_index;

extern int CWorAM;

enum BottomType {
  NoWork = 0, 
  ClockwiseRotate,
  AnticlockwiseRotate, 
  PushDown
};

enum BottomMode {
  BottomCWorAMMode = 0,
  BottomAmplitudeMode, 
  BottomModulationDegreeMode, 
  BottomFrequencyMode, 
  BottomDelayMode
};

extern uint32_t DCvoltage_AM[10];
// extern uint32_t DCvoltage[2][10];
extern double Frequency[11];
extern int DelayTime[11][7];
extern uint32_t DCvoltage_CW[11][10];
extern uint32_t DCvoltage_Modulation[7];
extern uint32_t MultiPath_Modulation[7];
extern uint32_t DelayPhase[7];
extern int DelayTimeData[7];
extern int AmplitudeValidValueData[10];

extern int ModulationDegreeData[7];

extern int phase;

extern enum BottomType bottom;
extern enum BottomMode bottom_mode;

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
#define X9C103_UD_2_Pin GPIO_PIN_3
#define X9C103_UD_2_GPIO_Port GPIOE
#define AD9959_UP_Pin GPIO_PIN_4
#define AD9959_UP_GPIO_Port GPIOE
#define PE_LE_Pin GPIO_PIN_1
#define PE_LE_GPIO_Port GPIOC    //  X9C103_INC_2
#define AD9959_P1_Pin GPIO_PIN_3
#define AD9959_P1_GPIO_Port GPIOA
#define X9C103_INC_Pin GPIO_PIN_0
#define X9C103_INC_GPIO_Port GPIOB
#define X9C103_UD_Pin GPIO_PIN_1
#define X9C103_UD_GPIO_Port GPIOB
#define X9C103_INC_2_Pin GPIO_PIN_13
#define X9C103_INC_2_GPIO_Port GPIOB
#define AD9959_CS_Pin GPIO_PIN_7
#define AD9959_CS_GPIO_Port GPIOC
#define RotateBottom_1_Pin GPIO_PIN_8
#define RotateBottom_1_GPIO_Port GPIOC
#define RotateBottom_1_EXTI_IRQn EXTI9_5_IRQn
#define RotateBottom_2_Pin GPIO_PIN_9
#define RotateBottom_2_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_8
#define LCD_RST_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_9
#define LCD_CS_GPIO_Port GPIOA
#define AD9959_SD3_Pin GPIO_PIN_13
#define AD9959_SD3_GPIO_Port GPIOH
#define AD9959_SCK_Pin GPIO_PIN_15
#define AD9959_SCK_GPIO_Port GPIOH
#define AD9959_SD1_Pin GPIO_PIN_0
#define AD9959_SD1_GPIO_Port GPIOI
#define AD9959_SD0_Pin GPIO_PIN_2
#define AD9959_SD0_GPIO_Port GPIOI
#define PE_CLK_Pin GPIO_PIN_10
#define PE_CLK_GPIO_Port GPIOC   // X9C103_UD_2
#define PE_DATA_Pin GPIO_PIN_11  //  X9C103_CS_2
#define PE_DATA_GPIO_Port GPIOC
#define Bottom_Pin GPIO_PIN_12
#define Bottom_GPIO_Port GPIOC
#define Bottom_EXTI_IRQn EXTI15_10_IRQn
#define AD9959_RST_Pin GPIO_PIN_6
#define AD9959_RST_GPIO_Port GPIOD
#define X9C103_CS_2_Pin GPIO_PIN_9
#define X9C103_CS_2_GPIO_Port GPIOG
#define AD9959_SD2_Pin GPIO_PIN_10
#define AD9959_SD2_GPIO_Port GPIOG
#define AD9959_P2_Pin GPIO_PIN_11
#define AD9959_P2_GPIO_Port GPIOG
#define AD9959_P3_Pin GPIO_PIN_12
#define AD9959_P3_GPIO_Port GPIOG
#define X9C103_CS_Pin GPIO_PIN_5
#define X9C103_CS_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_6
#define LCD_DC_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_7
#define LCD_BL_GPIO_Port GPIOB
#define AD9959_P0_Pin GPIO_PIN_9
#define AD9959_P0_GPIO_Port GPIOB
#define AD9959_PWR_Pin GPIO_PIN_4
#define AD9959_PWR_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

#ifndef __X9C103_H
#define __X9C103_H

#include "stm32f4xx_hal.h"
#include "main.h"

/*--------------------------------------------------------------------------
X9C103.H
X9C103 functions.
Copyright (c) 2007 DC. By Delphifx 2007-8-11.
All rights reserved.
--------------------------------------------------------------------------*/

// CS
#define CS_GPIO_PORT    	X9C103_CS_GPIO_Port			              /* GPIO�˿� */
// #define CS_GPIO_CLK 	    RCC_AHB1Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define CS_GPIO_PIN		    X9C103_CS_Pin			        /* ���ӵ�SCLʱ���ߵ�GPIO */

// INC
#define INC_GPIO_PORT    	X9C103_INC_GPIO_Port			              /* GPIO�˿� */
// #define INC_GPIO_CLK 	    RCC_AHB1Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define INC_GPIO_PIN			X9C103_INC_Pin			        /* ���ӵ�SCLʱ���ߵ�GPIO */

// UD
#define UD_GPIO_PORT    	X9C103_UD_GPIO_Port			              /* GPIO�˿� */
// #define UD_GPIO_CLK 	    RCC_AHB1Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define UD_GPIO_PIN		    X9C103_UD_Pin			        /* ���ӵ�SCLʱ���ߵ�GPIO */

/* �йص�λ���ĺ궨��  ʹ�ñ�׼�Ĺ̼������IO*/
#define CS(a)	if (a)	\
					HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_PIN, GPIO_PIN_SET);\
					else		\
					HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_PIN, GPIO_PIN_RESET)

#define INC(a)	if (a)	\
					HAL_GPIO_WritePin(INC_GPIO_PORT, INC_GPIO_PIN, GPIO_PIN_SET);\
					else		\
					HAL_GPIO_WritePin(INC_GPIO_PORT, INC_GPIO_PIN, GPIO_PIN_RESET)

#define UD(a)	if (a)	\
					HAL_GPIO_WritePin(UD_GPIO_PORT, UD_GPIO_PIN, GPIO_PIN_SET);\
					else		\
					HAL_GPIO_WritePin(UD_GPIO_PORT, UD_GPIO_PIN, GPIO_PIN_RESET)



void X9C103_Inc_N_Step(unsigned char N);
void X9C103_Dec_N_Step(unsigned char N);
void X9C103_Inc_N_Step_2 (unsigned char N);
void X9C103_Dec_N_Step_2 (unsigned char N);
void X9C103_Init_2 (void);
void Delay_X9C103(unsigned int t) ;
void X9C103_Init(void);//��ʼ�����м�λ��
void X9C103_Config(void);

#endif

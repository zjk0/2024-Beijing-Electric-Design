#ifndef _PE4302_H__
#define _PE4302_H__
#endif

//PE4302������Ŷ���?
//LEλ��P/Sλ�Ѿ���ģ���ϵĲ�����������ɣ�P/SΪONλʱΪ�̿�
//����ֵ��ע����ǣ�datasheet��˵����LEΪ��ʱ���ݼĴ������Է��ʣ�LEΪ��ʱ���������Ĵ������ݱ�����

//Vital Issues:д������ʱ��LEλ��״̬
//PUP1��PUP2���ſ�����Ϊ���ϵ��ʼ�����Ż����ǹ̶��������ţ����?���ÿ������óɲ�ͬ�Ĺ̶����棬���datasheet Table.6


//һ������µ������?��

#include "main.h"
#include "stm32f4xx_hal.h"

#define PE4302_LE_SET     rt_pin_write(GET_PIN(D,6),PIN_HIGH)       
#define PE4302_LE_RESET   rt_pin_write(GET_PIN(D,6),PIN_LOW)

#define PE4302_CLK_SET    rt_pin_write(GET_PIN(D,3),PIN_HIGH)
#define PE4302_CLK_RESET  rt_pin_write(GET_PIN(D,3),PIN_LOW)

#define PE4302_DATA_SET   rt_pin_write(GET_PIN(D,4),PIN_HIGH)
#define PE4302_DATA_RESET rt_pin_write(GET_PIN(D,4),PIN_LOW)

#define PE4302_PUP1_SET   rt_pin_write(GET_PIN(C,8),PIN_HIGH)
#define PE4302_PUP1_RESET rt_pin_write(GET_PIN(C,8),PIN_LOW)

#define PE4302_PUP2_SET   rt_pin_write(GET_PIN(C,9),PIN_HIGH)
#define PE4302_PUP2_RESET rt_pin_write(GET_PIN(C,9),PIN_LOW)
//���rtt�����ű��?
#define PE4302_LE   GET_PIN(D,6)
#define PE4302_CLK  GET_PIN(D,3)
#define PE4302_DATA GET_PIN(D,4)
#define PE4302_PUP1 GET_PIN(C,8)
#define PE4302_PUP2 GET_PIN(C,9)
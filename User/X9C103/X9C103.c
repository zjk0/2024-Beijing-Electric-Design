#include "X9C103.h"


/*��ʼ��X9C103�ܽ�*/
// void X9C103_Config(void)
// {
//     /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
//   GPIO_InitTypeDef GPIO_InitStructure; 
//     /*ʹ��x9c103ʹ�õ�GPIOʱ��*/ 	
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOFʱ��
	
//   GPIO_InitStructure.GPIO_Pin = CS_GPIO_PIN|INC_GPIO_PIN|UD_GPIO_PIN; //PB0 PB1 PB5
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);  /*���ÿ⺯������ʼ��GPIO*/
//   GPIO_SetBits(CS_GPIO_PORT,CS_GPIO_PIN|INC_GPIO_PIN|UD_GPIO_PIN);/* ��λ����GPIO��	*/
  
// }

//��ʱus�ӳ��� 
void Delay_X9C103(unsigned int t) 
{ 
unsigned int i; 
for(i=0;i<t;i++) ;

} 

//************************************************************************
// ���ֵ�λ�����ϵ�һ��
// ���ֵ�λ��100����ͷ���൱��99��
//************************************************************************
void X9C103_Inc_N_Step(unsigned char N)
{
   unsigned char i=0; 
          
        CS(0);             // CS  ����
        UD(1);             // U/D ����   �������INC���أ�ִ��UP����  
        Delay_X9C103(3);           // ��ʱ��Լ2us
        for(i = N; i>0; i--)
        {
					INC(1);         // ����INC //����ģʽ
          Delay_X9C103(200); // ��ʱ2us����
					INC(0);         // ��ΪGPIO��ʼ��ʱ��INC�Ѿ������ˡ�INC ����;  // ����һ������                            
					Delay_X9C103(600);            // ��ʱ��Լ500us, Rw�ȴ�����ȶ�
				}
				INC(0);//����ģʽ
        CS(1);              // ���CS���� //store

}
//************************************************************************
// ���ֵ�λ�����µ�һ��
// ���ֵ�λ��100����ͷ���൱��99��
//************************************************************************
void X9C103_Dec_N_Step(unsigned char N)
{
   unsigned char i=0; 
    CS(0);  
		UD(0);               //CLRB_X9C103_UD;   // U/D ��0��  �������INC���أ�ִ��Down����  
    Delay_X9C103(3);            // ��ʱ��Լ2us
//    CS(0);  
 
   for(i=N;i>0;i--) 
  { 
     INC(1);               // ����INC    ��ΪINC��������Ч
     Delay_X9C103(200);              // ��ʱ2us����
     INC(0);                // INC ����;  // ����һ������
     Delay_X9C103(600);            // ��ʱ��Լ500us, Rw�ȴ�����ȶ�
  }
     INC(0);//����ģʽ
     CS(1);                 // ���CS���� // store 
 

}

void X9C103_Init(void)//��ʼ�����м�λ��
{
  X9C103_Dec_N_Step(99); 
  X9C103_Inc_N_Step(49); 
}

void X9C103_Inc_N_Step_2 (unsigned char N) {
    unsigned char i=0; 
          
        HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_RESET);//CS(0);             // CS  ����
        HAL_GPIO_WritePin(PE_CLK_GPIO_Port, PE_CLK_Pin, GPIO_PIN_SET);//UD(1);             // U/D ����   �������INC���أ�ִ��UP����  
        Delay_X9C103(3);           // ��ʱ��Լ2us
        for(i = N; i>0; i--)
        {
					HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_SET);//INC(1);         // ����INC //����ģʽ
          Delay_X9C103(200); // ��ʱ2us����
					HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);         // ��ΪGPIO��ʼ��ʱ��INC�Ѿ������ˡ�INC ����;  // ����һ������                            
					Delay_X9C103(600);            // ��ʱ��Լ500us, Rw�ȴ�����ȶ�
				}
				HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);//����ģʽ
        HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_SET);//CS(1);              // ���CS���� //store
}

void X9C103_Dec_N_Step_2 (unsigned char N) {
    unsigned char i=0; 
    HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_RESET);//CS(0);  
		HAL_GPIO_WritePin(PE_CLK_GPIO_Port, PE_CLK_Pin, GPIO_PIN_RESET);//UD(0);               //CLRB_X9C103_UD;   // U/D ��0��  �������INC���أ�ִ��Down����  
    Delay_X9C103(3);            // ��ʱ��Լ2us
//    CS(0);  
 
   for(i=N;i>0;i--) 
  { 
     HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_SET);//INC(1);               // ����INC    ��ΪINC��������Ч
     Delay_X9C103(200);              // ��ʱ2us����
     HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);                // INC ����;  // ����һ������
     Delay_X9C103(600);            // ��ʱ��Լ500us, Rw�ȴ�����ȶ�
  }
     HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);//����ģʽ
     HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_SET);//CS(1);           
}

void X9C103_Init_2 (void) {
    X9C103_Dec_N_Step_2(99); 
  X9C103_Inc_N_Step_2(49); 
}

#include "X9C103.h"


/*初始化X9C103管脚*/
// void X9C103_Config(void)
// {
//     /*定义一个GPIO_InitTypeDef类型的结构体*/
//   GPIO_InitTypeDef GPIO_InitStructure; 
//     /*使能x9c103使用的GPIO时钟*/ 	
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOF时钟
	
//   GPIO_InitStructure.GPIO_Pin = CS_GPIO_PIN|INC_GPIO_PIN|UD_GPIO_PIN; //PB0 PB1 PB5
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);  /*调用库函数，初始化GPIO*/
//   GPIO_SetBits(CS_GPIO_PORT,CS_GPIO_PIN|INC_GPIO_PIN|UD_GPIO_PIN);/* 置位所有GPIO灯	*/
  
// }

//延时us子程序 
void Delay_X9C103(unsigned int t) 
{ 
unsigned int i; 
for(i=0;i<t;i++) ;

} 

//************************************************************************
// 数字电位器向上调一步
// 数字电位器100个抽头，相当于99步
//************************************************************************
void X9C103_Inc_N_Step(unsigned char N)
{
   unsigned char i=0; 
          
        CS(0);             // CS  拉低
        UD(1);             // U/D 拉高   则下面的INC下沿，执行UP操作  
        Delay_X9C103(3);           // 延时大约2us
        for(i = N; i>0; i--)
        {
					INC(1);         // 拉高INC //保存模式
          Delay_X9C103(200); // 延时2us左右
					INC(0);         // 因为GPIO初始化时，INC已经拉高了。INC 拉低;  // 产生一个下沿                            
					Delay_X9C103(600);            // 延时大约500us, Rw等待输出稳定
				}
				INC(0);//保存模式
        CS(1);              // 完毕CS拉高 //store

}
//************************************************************************
// 数字电位器向下调一步
// 数字电位器100个抽头，相当于99步
//************************************************************************
void X9C103_Dec_N_Step(unsigned char N)
{
   unsigned char i=0; 
    CS(0);  
		UD(0);               //CLRB_X9C103_UD;   // U/D 清0，  则下面的INC下沿，执行Down操作  
    Delay_X9C103(3);            // 延时大约2us
//    CS(0);  
 
   for(i=N;i>0;i--) 
  { 
     INC(1);               // 拉高INC    因为INC的下沿有效
     Delay_X9C103(200);              // 延时2us左右
     INC(0);                // INC 拉低;  // 产生一个下沿
     Delay_X9C103(600);            // 延时大约500us, Rw等待输出稳定
  }
     INC(0);//保存模式
     CS(1);                 // 完毕CS拉高 // store 
 

}

void X9C103_Init(void)//初始化至中间位置
{
  X9C103_Dec_N_Step(99); 
  X9C103_Inc_N_Step(49); 
}

void X9C103_Inc_N_Step_2 (unsigned char N) {
    unsigned char i=0; 
          
        HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_RESET);//CS(0);             // CS  拉低
        HAL_GPIO_WritePin(PE_CLK_GPIO_Port, PE_CLK_Pin, GPIO_PIN_SET);//UD(1);             // U/D 拉高   则下面的INC下沿，执行UP操作  
        Delay_X9C103(3);           // 延时大约2us
        for(i = N; i>0; i--)
        {
					HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_SET);//INC(1);         // 拉高INC //保存模式
          Delay_X9C103(200); // 延时2us左右
					HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);         // 因为GPIO初始化时，INC已经拉高了。INC 拉低;  // 产生一个下沿                            
					Delay_X9C103(600);            // 延时大约500us, Rw等待输出稳定
				}
				HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);//保存模式
        HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_SET);//CS(1);              // 完毕CS拉高 //store
}

void X9C103_Dec_N_Step_2 (unsigned char N) {
    unsigned char i=0; 
    HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_RESET);//CS(0);  
		HAL_GPIO_WritePin(PE_CLK_GPIO_Port, PE_CLK_Pin, GPIO_PIN_RESET);//UD(0);               //CLRB_X9C103_UD;   // U/D 清0，  则下面的INC下沿，执行Down操作  
    Delay_X9C103(3);            // 延时大约2us
//    CS(0);  
 
   for(i=N;i>0;i--) 
  { 
     HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_SET);//INC(1);               // 拉高INC    因为INC的下沿有效
     Delay_X9C103(200);              // 延时2us左右
     HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);                // INC 拉低;  // 产生一个下沿
     Delay_X9C103(600);            // 延时大约500us, Rw等待输出稳定
  }
     HAL_GPIO_WritePin(PE_LE_GPIO_Port, PE_LE_Pin, GPIO_PIN_RESET);//INC(0);//保存模式
     HAL_GPIO_WritePin(PE_DATA_GPIO_Port, PE_DATA_Pin, GPIO_PIN_SET);//CS(1);           
}

void X9C103_Init_2 (void) {
    X9C103_Dec_N_Step_2(99); 
  X9C103_Inc_N_Step_2(49); 
}

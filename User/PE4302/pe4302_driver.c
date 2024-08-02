#include "pe4302_driver.h"
void pe4302_WriteDataToRegister(double atten_word);

void pe4302_init(void)
{
    rt_pin_mode(PE4302_LE,PIN_MODE_OUTPUT);
    rt_pin_mode(PE4302_DATA,PIN_MODE_OUTPUT);
    rt_pin_mode(PE4302_CLK,PIN_MODE_OUTPUT);
}

INIT_DEVICE_EXPORT(pe4302_init);
/*-----------------------
八位转六位最方便的操作：左移
-----------------------*/
    void pe4302_WriteDataToRegister(double atten_word) //传入参数为衰减dB
{

    rt_uint8_t i = 0;
    rt_uint8_t atten_byte;
    atten_word *= 2;
    atten_byte = (rt_uint8_t)atten_word;
    atten_byte <<= 2;

    PE4302_LE_RESET;
    PE4302_CLK_RESET;

    for ( i = 0; i < 6; i++)
    {
        if (0x80 == (atten_byte & 0x80))
        {
            PE4302_DATA_SET;
        }
        else
        {
            PE4302_DATA_RESET;
        }
        rt_hw_us_delay(1);
        PE4302_CLK_SET;
            rt_hw_us_delay(1);
        
        atten_byte <<= 1;
        PE4302_CLK_RESET;
        rt_hw_us_delay(1);
        //缺延时
    }
    PE4302_LE_SET;
    rt_hw_us_delay(1);
    PE4302_LE_RESET;

}


//INIT_DEVICE_EXPORT(pe4302_WriteDataToRegister);
/*-----------------
function:利用pup1 pup2实现固定衰减,主要是为了确定上电时的衰减度 详见Table.6
notes:想要实现该功能需要手动将拨码器上的P/S拨至OFF
其实这一部分也间接表明了拨码器的实现原理......
----------------*/
void pe4302_solid_atten(rt_uint8_t level)
{
    PE4302_LE_RESET;
    if (level == 0)
    {
        //0dB
        PE4302_PUP1_RESET;
        PE4302_PUP2_RESET;
    }
    else if (level == 1)
    {
        //8dB
        PE4302_PUP1_RESET;
        PE4302_PUP2_SET;
    }
    else if (level == 2)
    {
        //16dB
        PE4302_PUP1_SET;
        PE4302_PUP2_RESET;
    }
    else
    {
        //32dB
        PE4302_PUP1_SET;
        PE4302_PUP2_SET;
    }
}


int input(int argc, char **argv)
{
    if (argc == 2)
    {
        rt_kprintf("This is true parameter and function name!\r\n");
        pe4302_WriteDataToRegister(atof(argv[1]));
    }
}
MSH_CMD_EXPORT(input, write in the atten word);

int print_hello(int argc, char **argv)
{
    if ((argc == 2) && (!rt_strcmp(argv[1], "print!")))
    {
        rt_kprintf("hello!\n");
    }
}
MSH_CMD_EXPORT(print_hello, used to print hello);
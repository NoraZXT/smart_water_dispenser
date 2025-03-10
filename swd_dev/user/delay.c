#include "stm32f10x.h"
#include "delay.h"

//延时系数
unsigned char UsCount = 0;
unsigned short MsCount = 0;

void delay_init(void)
{
    SysTick->CTRL &= ~(1 << 2);		//选择时钟为HCLK(72MHz)/8        103--9MHz
    
    UsCount = 9;
    
    MsCount = UsCount * 1000;
    
}

void delay_us(unsigned short us)
{
    unsigned int ctrResult = 0;
    us &= 0x00FFFFFF;
    
    SysTick->LOAD = us * UsCount;  //装载数据
    SysTick->VAL = 0;              //清零
    SysTick->CTRL = 1;             //使能计数器
    
    do
    {
        ctrResult = SysTick->CTRL;
    }while((ctrResult & 0x01) && !(ctrResult & (1<<16)));   //保证在运行、检查是否倒计数到0
    
    SysTick->CTRL = 0;
    SysTick->VAL =0;
}

void delay_ms(unsigned short ms)
{
    while(ms>0)
    {
        delay_us(1000);
        ms--;
    }
}

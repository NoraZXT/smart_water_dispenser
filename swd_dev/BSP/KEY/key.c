#include "stm32f10x.h"
#include "key.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "usart.h"
#include "delay.h"
#include "LCD1602.h"

unsigned int g_tim2_cnt;


key_val_typedef key1;
key_val_typedef key2;
key_val_typedef key3;

void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef tim2_init_struct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    tim2_init_struct.TIM_Prescaler = 72 - 1;
    tim2_init_struct.TIM_CounterMode = TIM_CounterMode_Up;
    tim2_init_struct.TIM_Period = 10000 - 1;
    tim2_init_struct.TIM_ClockDivision = TIM_CKD_DIV1;    
    tim2_init_struct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM2, &tim2_init_struct);
    TIM_InternalClockConfig(TIM2);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    

    
    NVIC_InitTypeDef nvic_initstruct;    
    nvic_initstruct.NVIC_IRQChannel = TIM2_IRQn;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_initstruct);

    TIM_Cmd(TIM2, DISABLE);
}

void key_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    NVIC_InitTypeDef nvic_initstruct;
    EXTI_InitTypeDef exti_initstruct;

/*----------------------------------------------------------------------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);       //打开GPIOB的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IPU;                  //设置为输出
    gpio_initstruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_9;                      //将初始化的Pin脚
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;              //可承载的最大频率
    GPIO_Init(GPIOB, &gpio_initstruct);                         //初始化GPIO
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

    exti_initstruct.EXTI_Line = EXTI_Line0;
    exti_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_initstruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exti_initstruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_initstruct);

    nvic_initstruct.NVIC_IRQChannel = EXTI0_IRQn;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_initstruct);
/*-----------------------------------------------------------------------------------------------*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

    exti_initstruct.EXTI_Line = EXTI_Line1;
    exti_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_initstruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exti_initstruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_initstruct); 
    
    nvic_initstruct.NVIC_IRQChannel = EXTI1_IRQn;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_initstruct);
/*-----------------------------------------------------------------------------------------------*/

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);

    exti_initstruct.EXTI_Line = EXTI_Line9;
    exti_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_initstruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exti_initstruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_initstruct); 
    
    nvic_initstruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_initstruct);

}

void EXTI0_IRQHandler(void)
{
    static unsigned char key_status = 0;
    
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        if(!key_status && KEY1_STATUS)
        {
            TIM_Cmd(TIM2, DISABLE);
            key_status = 1;
            g_tim2_cnt = 0;
//            UsartPrintf(USART3,"KEY1_press \r\n");
            TIM_Cmd(TIM2, ENABLE);        
        }
        else if(key_status && !KEY1_STATUS)
        {
            key_status = 0;
            TIM_Cmd(TIM2, DISABLE);
            if(g_tim2_cnt >= 5 && g_tim2_cnt < 100)
            {
                key1.short_press = 1;
//                UsartPrintf(USART3,"KEY1_S \r\n");
                
            }
            else if(g_tim2_cnt >=100)
            {
                key1.long_press = 1;
//                UsartPrintf(USART3,"KEY1_L \r\n");
            }
        }
    }
     EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void)
{
    static unsigned char key_status = 0;
    
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        
        if( KEY2_STATUS && !key_status)         //按下
        {
            TIM_Cmd(TIM2, DISABLE);
            key_status = 1;
            g_tim2_cnt = 0;
            TIM_Cmd(TIM2, ENABLE);
//            UsartPrintf(USART3,"KEY2_PRESS \r\n");
        }
        else if(key_status && !KEY2_STATUS)     //松开
        {
            key_status = 0;
            TIM_Cmd(TIM2, DISABLE);
            if(g_tim2_cnt >= 5 && g_tim2_cnt < 100)
            {
                key2.short_press = 1;
                g_tim2_cnt = 0;
//                UsartPrintf(USART3,"KEY2_S \r\n");
            }
            else if(g_tim2_cnt >=100)
            {
                key2.long_press = 1;
                g_tim2_cnt = 0;
//                UsartPrintf(USART3,"KEY2_L  %d\r\n", g_tim2_cnt);
            }
        }
        
    }
 EXTI_ClearITPendingBit(EXTI_Line1);    
}

void EXTI9_5_IRQHandler(void)
{
    static unsigned char key_status = 0;
    
    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        if(!key_status && KEY3_STATUS)
        {
            key_status = 1;
            g_tim2_cnt = 0;
            TIM_Cmd(TIM2, ENABLE);
        }
        else if(key_status && !KEY3_STATUS)
        {
            key_status = 0;
            TIM_Cmd(TIM2, DISABLE);
            if(g_tim2_cnt >= 5 && g_tim2_cnt < 100)
            {
                key3.short_press = 1;
//                UsartPrintf(USART3,"KEY3_S \r\n");
            }
            else if(g_tim2_cnt >=100)
            {
                key3.long_press = 1;
//                UsartPrintf(USART3,"KEY3_L  %d\r\n", g_tim2_cnt);
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update))
    {

        g_tim2_cnt ++;
        if(g_tim2_cnt >= 100 && KEY1_STATUS)
        {
            TIM_Cmd(TIM2, DISABLE);
        }
        if(g_tim2_cnt >= 100 && KEY2_STATUS)
        {
            TIM_Cmd(TIM2, DISABLE);           
        }
        else if(g_tim2_cnt >= 100 && KEY3_STATUS)
        {
            TIM_Cmd(TIM2, DISABLE);

        }
        
        
    }
    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}

unsigned char scan_key(void)
{
    if(key1.short_press)
    {
        key1.short_press = 0;
        return KEY1_PRESS;
    }
    else if(key1.long_press)
    {
        key1.long_press = 0;
        return KEY1_LONG_PRESS;
    }
    else if(key2.short_press)
    {
        key2.short_press = 0;
        return KEY2_PRESS;
    }
    else if(key2.long_press)
    {
        key2.long_press = 0;
        return KEY2_LONG_PRESS;
    }
    else if(key3.short_press)
    {
        key3.short_press = 0;
        return KEY3_PRESS;
    }
    else if(key3.long_press)
    {
        key3.long_press = 0;
        return KEY3_LONG_PRESS;
    }
//    return 0;
}

void PUMP_init(void)    //水泵继电器IO初始化
{
    GPIO_InitTypeDef gpio_init_struct;
    
    PUMP_GPIO_CLK_ENABLE();
    
    gpio_init_struct.GPIO_Pin = PUMP_GPIO_PIN;
    gpio_init_struct.GPIO_Mode =GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PUMP_GPIO_PORT, &gpio_init_struct);
    PUMP(0);
}

void HEATER_init(void)      //加热模块继电器IO初始化
{
    GPIO_InitTypeDef gpio_init_struct;
    
    HEATER_GPIO_CLK_ENABLE();
    
    gpio_init_struct.GPIO_Pin = HEATER_GPIO_PIN;
    gpio_init_struct.GPIO_Mode =GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HEATER_GPIO_PORT, &gpio_init_struct);
    HEATER(0);    
}

void LEVEL_init(void)          //水位模块IO初始化
{
    GPIO_InitTypeDef gpio_init_struct;
    
    LEVEL_GPIO_CLK_ENABLE();
    
    gpio_init_struct.GPIO_Pin = LEVEL_GPIO_PIN;
    gpio_init_struct.GPIO_Mode =GPIO_Mode_IPU;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HEATER_GPIO_PORT, &gpio_init_struct);

}

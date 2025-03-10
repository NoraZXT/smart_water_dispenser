#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#define KEY2_GPIO_PORT          GPIOB
#define KEY2_GPIO_PIN           GPIO_Pin_1
#define KEY2_IRQHandler         EXTI1_IRQHandler               
#define KEY2_STATUS             !GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY2_GPIO_PIN)


#define KEY1_GPIO_PORT          GPIOB
#define KEY1_GPIO_PIN           GPIO_Pin_0
#define KEY1_IRQHandler         EXTI0_IRQHandler
#define KEY1_STATUS             !GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY1_GPIO_PIN)

#define KEY3_GPIO_PORT          GPIOB
#define KEY3_GPIO_PIN           GPIO_Pin_9
#define KEY3_IRQHandler         EXTI9_IRQHandler
#define KEY3_STATUS             !GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN)

#define KEY1_PRESS                  1
#define KEY1_LONG_PRESS             2
#define KEY2_PRESS                  3
#define KEY2_LONG_PRESS             4
#define KEY3_PRESS                  5
#define KEY3_LONG_PRESS             6 




#define PUMP_GPIO_PORT          GPIOA
#define PUMP_GPIO_PIN           GPIO_Pin_6
#define PUMP_GPIO_CLK_ENABLE()       do{RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }while(0)
#define PUMP(x)  do{ x ? \
                        GPIO_WriteBit(PUMP_GPIO_PORT, PUMP_GPIO_PIN, Bit_SET): \
                        GPIO_WriteBit(PUMP_GPIO_PORT, PUMP_GPIO_PIN, Bit_RESET); \
                    }while(0)


                    
                    
#define HEATER_GPIO_PORT          GPIOA
#define HEATER_GPIO_PIN           GPIO_Pin_7
#define HEATER_GPIO_CLK_ENABLE()       do{RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }while(0)
#define HEATER(x)  do{ x ? \
                        GPIO_WriteBit(HEATER_GPIO_PORT, HEATER_GPIO_PIN, Bit_SET): \
                        GPIO_WriteBit(HEATER_GPIO_PORT, HEATER_GPIO_PIN, Bit_RESET); \
                    }while(0)

                    
#define LEVEL_GPIO_PORT          GPIOA
#define LEVEL_GPIO_PIN           GPIO_Pin_0
#define LEVEL_GPIO_CLK_ENABLE()       do{RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }while(0)
#define LEVEL_STATUS             !GPIO_ReadInputDataBit(LEVEL_GPIO_PORT, LEVEL_GPIO_PIN)                    
                    

typedef struct
{
    unsigned char short_press;
    unsigned char long_press;
}key_val_typedef;

void TIM2_Init(void);
void key_init(void);
unsigned char scan_key(void);

void HEATER_init(void);
void PUMP_init(void);
void LEVEL_init(void);
#endif

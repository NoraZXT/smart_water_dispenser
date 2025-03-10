#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x_gpio.h"

#define DS18B20_GPIO_PORT               GPIOA
#define DS18B20_GPIO_PIN                GPIO_Pin_5
#define DS18B20_GPIO_CLK_ENABLE()       do{RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }while(0)

#define DS18B20_OUT(x)  do{ x ? \
                        GPIO_WriteBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, Bit_SET): \
                        GPIO_WriteBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN, Bit_RESET); \
                    }while(0)

#define DS18B20_IN      GPIO_ReadInputDataBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)

unsigned char DS18B20_init(void);
unsigned char DS18B20_check(void);
short DS18B20_get_temperature(void);
                    
#endif

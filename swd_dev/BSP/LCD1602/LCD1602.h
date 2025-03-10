#ifndef _LCD1602_H_
#define _LCD1602_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#define LCD_RS_PIN          GPIO_Pin_12
#define LCD_RW_PIN          GPIO_Pin_13
#define LCD_E_PIN           GPIO_Pin_14
#define LCD_DB0_PIN         GPIO_Pin_15

#define LCD_DB1_PIN         GPIO_Pin_8
#define LCD_DB2_PIN         GPIO_Pin_9
#define LCD_DB3_PIN         GPIO_Pin_10
#define LCD_DB4_PIN         GPIO_Pin_11
#define LCD_DB5_PIN         GPIO_Pin_12

#define LCD_DB6_PIN         GPIO_Pin_5
#define LCD_DB7_PIN         GPIO_Pin_6

#define LCD_RS(x)   do{ x?\
                        GPIO_WriteBit(GPIOB, LCD_RS_PIN, Bit_SET): \
                        GPIO_WriteBit(GPIOB, LCD_RS_PIN, Bit_RESET); \
                    }while(0)

#define LCD_RW(x)   do{ x?\
                        GPIO_WriteBit(GPIOB, LCD_RW_PIN, Bit_SET): \
                        GPIO_WriteBit(GPIOB, LCD_RW_PIN, Bit_RESET); \
                    }while(0)

#define LCD_E(x)   do{ x?\
                        GPIO_WriteBit(GPIOB, LCD_E_PIN, Bit_SET): \
                        GPIO_WriteBit(GPIOB, LCD_E_PIN, Bit_RESET); \
                    }while(0)


void LCD1602_init(void);
void WriteByteData(char c);
void WriteByteCmd(char c);                    
void LCD_ShowString(unsigned char Line,unsigned char Column,char *String);
void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length); 
void LCD_ShowChar(unsigned char Line,unsigned char Column,char Char);  
void LCD1602_gpio_init(void);                    
#endif

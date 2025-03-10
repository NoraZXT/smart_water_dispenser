#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"

#define USART_DEBUG USART3

void ESP8266_USERT_INIT(unsigned int baud);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
void DEBUG_USERT_INIT(unsigned int baud);

#endif

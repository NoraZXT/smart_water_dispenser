/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>

#include "LCD1602.h"

//�û��Լ���WIFI���ƺ�����
//����ʹ���ֻ��ȵ㣬�����ȵ�Ƶ������Ϊ2.4G

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"ONEPLUS\",\"mbgv4977\"\r\n"

//#define ESP8266_WIFI_INFO		"AT+CWJAP=\"WIFI����\",\"WIFI����\"\r\n"

unsigned char esp8266_buf[512];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;


//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;
//    UsartPrintf(USART_DEBUG,"SEND %s\r\n",cmd);
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
//            UsartPrintf(USART_DEBUG,"REV %s\r\n",esp8266_buf); 
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
//                
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART2, data, len);		//�����豸������������..........................
//        UsartPrintf(USART_DEBUG,"SEND %s\r\n",data);
	}

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
//            UsartPrintf(USART_DEBUG,"REV %s\r\n",esp8266_buf);
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{
	
	ESP8266_Clear();
    WriteByteCmd(0x01);
	LCD_ShowString(0,0,"ESP8266 Init...");
	UsartPrintf(USART_DEBUG, " 1. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "4. CWJAP\r\n"); 
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "5. ESP8266 Init OK\r\n");
}

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_buf[esp8266_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}


void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef tim3_init_struct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    tim3_init_struct.TIM_Prescaler = 7200 - 1;
    tim3_init_struct.TIM_CounterMode = TIM_CounterMode_Up;
    tim3_init_struct.TIM_Period = 55000 - 1;
    tim3_init_struct.TIM_ClockDivision = TIM_CKD_DIV1;    
    tim3_init_struct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM3, &tim3_init_struct);
    TIM_InternalClockConfig(TIM3);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);    
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    

    
    NVIC_InitTypeDef nvic_initstruct;    
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_initstruct.NVIC_IRQChannel = TIM3_IRQn;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_initstruct);

    TIM_Cmd(TIM3, ENABLE);
}
extern unsigned char timeCount;
void TIM3_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update))
    {
        timeCount = 1;
    }
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
}


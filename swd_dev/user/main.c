#include "stm32f10x.h"
#include <string.h>
#include "DS18B20.h"
#include "delay.h"
#include "usart.h"
#include "LCD1602.h"
#include "key.h"
#include "esp8266.h"
#include "onenet.h"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
/*-------------------------ȫ�ֱ���------------------------------------------------*/
unsigned int Temp,SET_TEMP_LOW, SET_TEMP_HIGH;  //ʵʱ�¶ȣ��¶����ޣ��¶�����
unsigned char MODE, pre_MODE;     //0Ϊ���£�1Ϊ����
unsigned char SET_TEMP, pre_SET_TEMP;     //1Ϊ�������ޣ�2Ϊ��������
unsigned char LACK_WATER;   //1Ϊȱˮ 0Ϊ��ȱˮ
unsigned char PUMP_status, HEATER_status;     //ˮ��״̬������ģ��״̬
unsigned char timeCount;   //��ʱ�ϴ���־λ

unsigned char SendWaitForAsk=0;  //�Զ�������־λ
/*----------------------------------------------------------------------------------*/
void Handware_init(void);
void Show_lcd(char mode, unsigned int set_temp_high, unsigned int set_temp_low, char pump_status);

int main(void)
{
    short TEMP = 0;
    timeCount = 0;
    unsigned char *dataPtr = NULL;
    MODE = pre_MODE = 0;       //Ĭ�ϳ���ģʽ
    pre_SET_TEMP = SET_TEMP = 1;
    PUMP_status = 0;
    HEATER_status = 0;
    SET_TEMP_LOW = 10;
    SET_TEMP_HIGH = 15;
    
    delay_init();
    delay_ms(1000);
    Handware_init();
    while(1)      //�����Զ�����
    {
        ESP8266_Init();
        WriteByteCmd(0x01);
        LCD_ShowString(0,0,"CONNECT MQTTs");
        UsartPrintf(USART3,"CONNECT MQTTs Server...\r\n");
        while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
            delay_ms(500);
        UsartPrintf(USART3,"CONNECT MQTTs SUCCESS\r\n");
        WriteByteCmd(0x01);
        LCD_ShowString(0,0,"OneNET_DevLink");
        while(OneNet_DevLink())
        {
            delay_ms(500);
            ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
        }
        OneNET_Subscribe();
        
    //    delay_ms(1000);
        
        
        while(1)
        {
            LACK_WATER = LEVEL_STATUS;
//            TEMP = 20;
            TEMP = DS18B20_get_temperature(); 
            Temp = TEMP / 10;         

                
 /*----------------------------------����ɨ��-------------------------------------------------------------------------------------------
KEY1��ѡ�񣩶̰����л�ģʽ
          Ĭ�ϳ���ģʽ���̰��л�Ϊ����ģʽ���ٴζ̰��л�Ϊ����ģʽ
KEY1��ѡ�񣩳������л�KEY3���¶ȣ��޸Ķ���
          Ĭ��KEY3�����¶����ޣ�����1s���ɿ���KEY3�л�Ϊ�����¶����ޣ��ٴγ���1s���ɿ���KEY3�л�Ϊ�����¶�����

KEY2(��/��)�̰����ı�ˮ�ÿ���
                Ĭ��ˮ��Ϊ�ر�״̬���̰��л�Ϊˮ�ÿ����ٴζ̰��л�Ϊˮ�ù�

KEY3(�¶�)�̰����¶���ֵ+5��
KEY3(�¶�)�������¶���ֵ-5��
--------------------------------------------------------------------------------------------------------------------------------------*/
            switch(scan_key())
            {
                case KEY1_PRESS:     //KEY1�̰�
                    if(pre_MODE == 0)
                    {
                        MODE = 1;
                    }
                    else if(pre_MODE == 1)
                    {
                        MODE = 0;
                    }
                    
                    break;
                case KEY1_LONG_PRESS:   //KEY1����
                    if(pre_SET_TEMP == 1)
                    {
                        SET_TEMP = 2;
                    }
                    else if(pre_SET_TEMP == 2)
                    {
                        SET_TEMP = 1;
                    }
                    break;
                case KEY2_PRESS:        //KEY2�̰�
                    if(!LACK_WATER)
                    {
                        if((MODE == 0 && PUMP_status == 0) || (MODE == 1 && PUMP_status == 0 && Temp >= SET_TEMP_LOW))
                        {
                            PUMP(1);
                            PUMP_status = 1;
                        }
                        else if((MODE == 0 && PUMP_status == 1) || (MODE == 1 && PUMP_status == 1 && Temp >= SET_TEMP_LOW))
                        {
                            PUMP(0);
                            PUMP_status = 0;
                        }                
                    }
                    break;                
                case KEY2_LONG_PRESS:
                    break;
                case KEY3_PRESS:    //KEY3�̰�
                    if(MODE == 1)
                    {
                        if(SET_TEMP == 1)   //��������
                        {
                            SET_TEMP_LOW = SET_TEMP_LOW + 5;
                            if((SET_TEMP_LOW + 5) >= SET_TEMP_HIGH)     //�̶��������ٱ�����С5����ֹ�������ޱ����޸�����
                            {
                                SET_TEMP_LOW = SET_TEMP_HIGH - 5;
                            }
                        }
                        else if(SET_TEMP == 2)    //��������
                        {
                            SET_TEMP_HIGH = SET_TEMP_HIGH + 5;
                            if(SET_TEMP_HIGH >= 80)      //�̶��������Ϊ80
                            {
                                SET_TEMP_HIGH = 80;
                            }
                        }                
                    }

                    break;
                case KEY3_LONG_PRESS:     //LEY3����
                    if(MODE == 1)
                    {
                        if(SET_TEMP == 1)      //��������
                        {   
                            SET_TEMP_LOW = SET_TEMP_LOW - 5;
                            if(SET_TEMP_LOW <= 0)   //�̶�������СΪ0
                            {
                                SET_TEMP_LOW = 0;
                            }
                        }
                        else if(SET_TEMP == 2)     //��������
                        {
                            SET_TEMP_HIGH = SET_TEMP_HIGH - 5;
                            if(SET_TEMP_HIGH<= (SET_TEMP_LOW + 5))  //�̶��������ٱ�����С5����ֹ�������ޱ����޸�����
                            {
                                SET_TEMP_HIGH = SET_TEMP_LOW + 5;
                            }
                        }                    
                    }

                    break;

            }
 
/*--------------------ESP8266�����ϴ�/��������------------------------------------------------------------------------------*/            
            if(timeCount == 1)  //��ʱ��ʱ�䵽����ƽ̨��������
            {
                if(SendWaitForAsk == 2)    //����2�η�������ƽ̨δ�ظ�������ѭ������ESP8266��ʼ������
                {
                    SendWaitForAsk = 0;
                    break;
                }
                OneNet_SendData(); //��ƽ̨��������
                timeCount = 0;
                ESP8266_Clear();
    //            UsartPrintf(USART3, "TEMP %d��\r\n",Temp);
            }

            dataPtr = ESP8266_GetIPD(0);  //����ƽ̨���͵�����
            if(dataPtr != NULL)
            {
                OneNet_RevPro(dataPtr);   //��������
            }
/*----------------------------------------------------------------------------------------------------------------*/
            
           //����״̬ 
            pre_MODE = MODE;
            pre_SET_TEMP = SET_TEMP;
            
            
            if(LACK_WATER)  //�ж�Ϊȱˮ��ֹͣˮ�úͺͼ�����������LCD��ʾȱˮ
            {
                PUMP(0);
                PUMP_status = 0;
                HEATER(0);
                HEATER_status = 0;
                WriteByteCmd(0x01);
                LCD_ShowString(0,0,"LACK WATER");            
            }
            else    //���ݰ����ͱ�־λ���ж�������д���
            {
                Show_lcd(MODE, SET_TEMP_HIGH, SET_TEMP_LOW, PUMP_status);
                if(MODE == 1 && Temp < SET_TEMP_LOW)
                {
                    HEATER(1);
                    HEATER_status = 1;
                    PUMP(0);
                    PUMP_status = 0;
                }
                else if(MODE == 1 && Temp < SET_TEMP_HIGH && Temp > SET_TEMP_LOW && HEATER_status == 1)
                {
                    HEATER(1);
                    HEATER_status = 1;
    //                PUMP(0);
    //                PUMP_status = 0;
                }
                else if(MODE == 1 && Temp >= SET_TEMP_HIGH && HEATER_status == 1)
                {
                    HEATER(0);
                    HEATER_status = 0;
                }
                else if(MODE == 1 && Temp <= SET_TEMP_HIGH && Temp >= SET_TEMP_LOW && HEATER_status == 0)
                {
                    HEATER(0);
                    HEATER_status = 0;            
                }
                else if(MODE == 0)
                {
                    HEATER(0);
                    HEATER_status = 0;            
                }            
                
            }            
        }        
    }

}
void Show_lcd(char mode, unsigned int set_temp_high, unsigned int set_temp_low, char pump_status)
{
    WriteByteCmd(0x01);
    LCD_ShowString(0,0,"T:");
    LCD_ShowNum(0,2,Temp, 2);
    if(pump_status == 1)
    {
        LCD_ShowString(0,12,"ON");             
    }
    else if(pump_status == 0)
    {
        LCD_ShowString(0,12,"OFF");            
    }
  
    if(mode == 0)
    {
        LCD_ShowString(1, 0, "Normal");
    }
    else if(mode == 1)
    {
        LCD_ShowString(1, 0, "Heat");
        LCD_ShowString(1, 5, "TH:") ;
        LCD_ShowNum(1,8,set_temp_high, 2);
        LCD_ShowString(1, 11, "TL:") ;
        LCD_ShowNum(1,14,set_temp_low, 2);
    }
}
void Handware_init(void)
{

    LCD1602_init();
    WriteByteCmd(0x01);
    LCD_ShowString(0,0,"Handware init");    
    HEATER_init();
    PUMP_init();
    LEVEL_init();
    DEBUG_USERT_INIT(115200);
    ESP8266_USERT_INIT(115200);
    while(DS18B20_init())
    {
        UsartPrintf(USART3,"DS18B20 FALSE\r\n");
        delay_ms(500);
    }
    UsartPrintf(USART3,"DS18B20 OK\r\n");
    DS18B20_get_temperature();
    key_init();
    TIM2_Init();    //��ʱ��2���ڰ����жϳ����̰�
    TIM3_Init();    //��ʱ��3���ڶ�ʱ�ϱ�����
}

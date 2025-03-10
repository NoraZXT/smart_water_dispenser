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
/*-------------------------全局变量------------------------------------------------*/
unsigned int Temp,SET_TEMP_LOW, SET_TEMP_HIGH;  //实时温度，温度下限，温度上限
unsigned char MODE, pre_MODE;     //0为常温，1为加热
unsigned char SET_TEMP, pre_SET_TEMP;     //1为调整下限，2为调整上限
unsigned char LACK_WATER;   //1为缺水 0为不缺水
unsigned char PUMP_status, HEATER_status;     //水泵状态，加热模块状态
unsigned char timeCount;   //定时上传标志位

unsigned char SendWaitForAsk=0;  //自动重连标志位
/*----------------------------------------------------------------------------------*/
void Handware_init(void);
void Show_lcd(char mode, unsigned int set_temp_high, unsigned int set_temp_low, char pump_status);

int main(void)
{
    short TEMP = 0;
    timeCount = 0;
    unsigned char *dataPtr = NULL;
    MODE = pre_MODE = 0;       //默认常温模式
    pre_SET_TEMP = SET_TEMP = 1;
    PUMP_status = 0;
    HEATER_status = 0;
    SET_TEMP_LOW = 10;
    SET_TEMP_HIGH = 15;
    
    delay_init();
    delay_ms(1000);
    Handware_init();
    while(1)      //用于自动重连
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

                
 /*----------------------------------按键扫描-------------------------------------------------------------------------------------------
KEY1（选择）短按：切换模式
          默认常温模式，短按切换为加热模式，再次短按切换为常温模式
KEY1（选择）长按：切换KEY3（温度）修改对象
          默认KEY3调整温度下限，长按1s后松开，KEY3切换为调整温度上限，再次长按1s后松开，KEY3切换为调整温度下限

KEY2(开/关)短按：改变水泵开关
                默认水泵为关闭状态，短按切换为水泵开，再次短按切换为水泵关

KEY3(温度)短按：温度阈值+5℃
KEY3(温度)长按：温度阈值-5℃
--------------------------------------------------------------------------------------------------------------------------------------*/
            switch(scan_key())
            {
                case KEY1_PRESS:     //KEY1短按
                    if(pre_MODE == 0)
                    {
                        MODE = 1;
                    }
                    else if(pre_MODE == 1)
                    {
                        MODE = 0;
                    }
                    
                    break;
                case KEY1_LONG_PRESS:   //KEY1长按
                    if(pre_SET_TEMP == 1)
                    {
                        SET_TEMP = 2;
                    }
                    else if(pre_SET_TEMP == 2)
                    {
                        SET_TEMP = 1;
                    }
                    break;
                case KEY2_PRESS:        //KEY2短按
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
                case KEY3_PRESS:    //KEY3短按
                    if(MODE == 1)
                    {
                        if(SET_TEMP == 1)   //调整下限
                        {
                            SET_TEMP_LOW = SET_TEMP_LOW + 5;
                            if((SET_TEMP_LOW + 5) >= SET_TEMP_HIGH)     //固定下限至少比上限小5，防止出现下限比上限搞的情况
                            {
                                SET_TEMP_LOW = SET_TEMP_HIGH - 5;
                            }
                        }
                        else if(SET_TEMP == 2)    //调整上限
                        {
                            SET_TEMP_HIGH = SET_TEMP_HIGH + 5;
                            if(SET_TEMP_HIGH >= 80)      //固定上限最大为80
                            {
                                SET_TEMP_HIGH = 80;
                            }
                        }                
                    }

                    break;
                case KEY3_LONG_PRESS:     //LEY3长按
                    if(MODE == 1)
                    {
                        if(SET_TEMP == 1)      //调整下限
                        {   
                            SET_TEMP_LOW = SET_TEMP_LOW - 5;
                            if(SET_TEMP_LOW <= 0)   //固定下限最小为0
                            {
                                SET_TEMP_LOW = 0;
                            }
                        }
                        else if(SET_TEMP == 2)     //调整上限
                        {
                            SET_TEMP_HIGH = SET_TEMP_HIGH - 5;
                            if(SET_TEMP_HIGH<= (SET_TEMP_LOW + 5))  //固定下限至少比上限小5，防止出现下限比上限搞的情况
                            {
                                SET_TEMP_HIGH = SET_TEMP_LOW + 5;
                            }
                        }                    
                    }

                    break;

            }
 
/*--------------------ESP8266部分上传/解析数据------------------------------------------------------------------------------*/            
            if(timeCount == 1)  //定时器时间到，向平台发送数据
            {
                if(SendWaitForAsk == 2)    //连续2次发送数据平台未回复，跳出循环进行ESP8266初始化重连
                {
                    SendWaitForAsk = 0;
                    break;
                }
                OneNet_SendData(); //向平台发送数据
                timeCount = 0;
                ESP8266_Clear();
    //            UsartPrintf(USART3, "TEMP %d℃\r\n",Temp);
            }

            dataPtr = ESP8266_GetIPD(0);  //接收平台发送的数据
            if(dataPtr != NULL)
            {
                OneNet_RevPro(dataPtr);   //解析数据
            }
/*----------------------------------------------------------------------------------------------------------------*/
            
           //更新状态 
            pre_MODE = MODE;
            pre_SET_TEMP = SET_TEMP;
            
            
            if(LACK_WATER)  //判断为缺水，停止水泵和和加热器工作，LCD显示缺水
            {
                PUMP(0);
                PUMP_status = 0;
                HEATER(0);
                HEATER_status = 0;
                WriteByteCmd(0x01);
                LCD_ShowString(0,0,"LACK WATER");            
            }
            else    //根据按键和标志位进行对外设进行处理
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
    TIM2_Init();    //定时器2用于按键判断长按短按
    TIM3_Init();    //定时器3用于定时上报数据
}

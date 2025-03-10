#include "DS18B20.h"
#include "delay.h"

static void DS18B20_reset(void)
{
    DS18B20_OUT(0);
    delay_us(750);
    DS18B20_OUT(1);
    delay_us(15);
}

unsigned char DS18B20_check(void)
{
    unsigned char retry = 0;
    unsigned char rval = 0;
    
    while(DS18B20_IN && retry < 200)
    {
        retry++;
        delay_us(1);
    }
    
    if(retry >= 200)
    {
        rval = 1;
    }
    else
    {
        retry = 0;
        while(!DS18B20_IN && retry < 240)
        {
            retry++;
            delay_us(1);
        }
        
        if(retry >= 240)
        {
            rval = 1;
        }
    }
    return rval;
}

static unsigned char DS18B20_read_bit(void)
{
    uint8_t data = 0;
    DS18B20_OUT(0);
    delay_us(2);
    DS18B20_OUT(1);
    delay_us(12);
    
    if(DS18B20_IN)
    {
        data = 1;
    }
    delay_us(50);
    return data;
}

static unsigned char DS18B20_read_byte(void)
{
    unsigned char i,b,data = 0;
    for(i = 0; i < 8; i++)
    {
        b = DS18B20_read_bit();
        data |= b << i;
    }
    return data;
}

static void DS18B20_write_byte(unsigned char data)
{
    unsigned char j;
    for(j = 1;j <= 8; j++)
    {
        if(data & 0x01)
        {
            DS18B20_OUT(0);
            delay_us(2);
            DS18B20_OUT(1);
            delay_us(60);
        }
        else
        {
            DS18B20_OUT(0);
            delay_us(60);
            DS18B20_OUT(1);
            delay_us(2);
        }
        data >>= 1;
    }
}

static void DS18B20_start(void)
{
    DS18B20_reset();
    DS18B20_check();
    DS18B20_write_byte(0XCC);
    DS18B20_write_byte(0X44);
}

unsigned char DS18B20_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    DS18B20_GPIO_CLK_ENABLE();
    
    gpio_init_struct.GPIO_Pin = DS18B20_GPIO_PIN;
    gpio_init_struct.GPIO_Mode =GPIO_Mode_Out_OD;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_GPIO_PORT, &gpio_init_struct);
    
    DS18B20_reset();
    return DS18B20_check();
}

short DS18B20_get_temperature(void)
{
    uint8_t flag = 1;//,a = 0;
    uint8_t TL = 0,TH = 0;
    short temp = 0;
    
    DS18B20_start();
    DS18B20_reset();
    DS18B20_check();
    DS18B20_write_byte(0XCC);
    DS18B20_write_byte(0XBE);
    TL = DS18B20_read_byte();
    TH = DS18B20_read_byte();
    
    if(TH > 7)
    {
        TH = TH;
        TL = TL;
        flag = 0;
    }
    temp = TH;
    temp <<= 8;
    temp += TL;
    
    if(flag == 0)
    {
        temp = (double)(temp + 1) * 0.625;
        temp = -temp;
    }
    else
    {
        temp = (double)temp * 0.625;
    }
    
//    if(a == 1)  temp = 99;
    return temp;
}

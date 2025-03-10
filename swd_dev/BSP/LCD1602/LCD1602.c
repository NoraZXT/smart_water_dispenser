#include "LCD1602.h"
#include "stm32f10x_gpio.h"
#include "delay.h"

void LCD1602_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pin = LCD_RS_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &gpio_init_struct);    
    
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pin = LCD_RW_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &gpio_init_struct);

    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pin = LCD_E_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &gpio_init_struct);

    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pin = LCD_DB0_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &gpio_init_struct);

    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pin = LCD_DB6_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &gpio_init_struct);
    
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pin = LCD_DB7_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &gpio_init_struct);    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pin = LCD_DB1_PIN | LCD_DB2_PIN | LCD_DB3_PIN | LCD_DB4_PIN | LCD_DB5_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &gpio_init_struct);    
    
}

void WriteByteData(char c)
{
    LCD_RS(1);
    LCD_RW(0);
    LCD_E(0);
    
    GPIO_WriteBit(GPIOB, LCD_DB0_PIN, (BitAction)(c & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB1_PIN, (BitAction)((c >> 1) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB2_PIN, (BitAction)((c >> 2) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB3_PIN, (BitAction)((c >> 3) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB4_PIN, (BitAction)((c >> 4) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB5_PIN, (BitAction)((c >> 5) & 0x01));
    GPIO_WriteBit(GPIOB, LCD_DB6_PIN, (BitAction)((c >> 6) & 0x01));
    GPIO_WriteBit(GPIOB, LCD_DB7_PIN, (BitAction)((c >> 7) & 0x01));
    
    LCD_E(1);
    delay_ms(1);
    LCD_E(0);
    delay_ms(1);
//    delay_ms(1);
}

void WriteByteCmd(char c)
{
    LCD_RS(0);
    LCD_RW(0);
//    LCD_E(0);
    
    GPIO_WriteBit(GPIOB, LCD_DB0_PIN, (BitAction)(c & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB1_PIN, (BitAction)((c >> 1) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB2_PIN, (BitAction)((c >> 2) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB3_PIN, (BitAction)((c >> 3) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB4_PIN, (BitAction)((c >> 4) & 0x01));
    GPIO_WriteBit(GPIOA, LCD_DB5_PIN, (BitAction)((c >> 5) & 0x01));
    GPIO_WriteBit(GPIOB, LCD_DB6_PIN, (BitAction)((c >> 6) & 0x01));
    GPIO_WriteBit(GPIOB, LCD_DB7_PIN, (BitAction)((c >> 7) & 0x01));

    LCD_E(1);
    delay_ms(1);
    LCD_E(0);
    delay_ms(1);
}

void LCD1602_init(void)
{
    LCD1602_gpio_init();
    
    WriteByteCmd(0x38);
    WriteByteCmd(0x0C);
    WriteByteCmd(0x06);
    WriteByteCmd(0x01);
}

static int setAddr(int row, int col)
{
    char addr;
    if(row == 0)
        addr = 0x80 + col;
    else if(row == 1)
        addr = 0x80 + 0x40 + col;
    else
        return -1;
    WriteByteCmd(addr);
    return 0;
}


void LCD_ShowChar(unsigned char Line,unsigned char Column,char Char)
{
    setAddr(Line,Column);
    WriteByteData(Char);
}


void LCD_ShowString(unsigned char Line,unsigned char Column,char *String)
{
    unsigned char i;
    setAddr(Line,Column);
    for(i=0;String[i]!='\0';i++)
    {
        WriteByteData(String[i]);
    }
}

int LCD_Pow(int X,int Y)
{
    unsigned char i;
    int Result=1;
    for(i=0;i<Y;i++)
    {
        Result*=X;
    }
    return Result;
}


void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length)
{
    unsigned char i;
    setAddr(Line,Column);
    for(i=Length;i>0;i--)
    {
        WriteByteData(Number/LCD_Pow(10,i-1)%10+'0');
    }
}


void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,int Number,unsigned char Length)
{
    unsigned char i;
    unsigned int Number1;
    setAddr(Line,Column);
    if(Number>=0)
    {
        WriteByteData('+');
        Number1=Number;
    }
    else
    {
        WriteByteData('-');
        Number1=-Number;
    }
    for(i=Length;i>0;i--)
    {
        WriteByteData(Number1/LCD_Pow(10,i-1)%10+'0');
    }
}


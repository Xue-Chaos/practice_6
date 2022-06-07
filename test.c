/* 包含头文件 */
#include<iocc2530.h>
#include<string.h>

/*宏定义*/
#define LED1 P1_0
#define LED2 P1_1
#define uint16 unsigned short
/*定义变量*/
int count=0;//统计定时器溢出次数
char output[8];//存放转换成字符形式的传感器数据
uint16 flamgas_val;//ADC采集结果

/*声明函数*/
void InitCLK(void);//系统时钟初始化函数，为32MHz
void InitUART0( );//串口0初始化
void InitT1( );//定时器1初始化
void Delay(int delaytime);//延时函数
unsigned short Get_adc( );//ADC采集
void Uart_tx_string(char *data_tx,int len);//往串口发送指定长度的数据
void InitLED(void);//灯的初始化

/*定义函数*/
void InitCLK(void)
{
  CLKCONCMD &= 0x80;
  while(CLKCONSTA & 0x40);
}

void InitLED()
{
  P1SEL &= ~0x03;//设置P1_0、P1_1为GPIO口
  P1DIR |= 0x03;//设置P1_0和P1_1为输出
  LED1 = LED2 = 0;//设置LED1和LED2的初始状态
}

void Delay(int delaytime)
{
    int i=0,j=0;
    for(i=0; i<240; i++)
        for(j=0; j<delaytime; j++);
}

void InitT1( )
{
    /*.......：定时器设置....................*/
    T1CTL |= 0x09;
    T1IE = 1;
     EA = 1; 
}

uint16 Get_adc( )
{
    /*.......答题区3开始：ADC参数设置....................*/
    
  /*.......答题区3结束.......................................*/
     
    while(!ADCIF);
    ADCIF=0;
    unsigned long value;
    value = ADCH;
    value = value<<8;
    value |=ADCL;
    value = value*330;
    value = value>>15;
    return (uint16)value;
}
void InitUART0( )
{
   U0CSR |=0X80;//串口模式
   P0SEL |= 0x0c;
   U0UCR |= 0x80;

   U0BAUD = 216;
   U0GCR = 8;
  
    UTX0IF = 0;    
    EA = 1;   
}



void Uart_tx_string(char *data_tx,int len)  
{   unsigned int j;  
    for(j=0;j<len;j++)  
    {   U0DBUF = *data_tx++;  
        while(UTX0IF == 0);  
        UTX0IF = 0;   
    }
} 



#pragma vector = T1_VECTOR
__interrupt void t1( )
{
    T1IF = 0;//清除定时器1中断标志
    count++; //累加中断次数
  
        
    value = Get_adc();
    sprintf(str, "%d", value);

    Uart_tx_string(str, sizeof(str));

}
void main( )
{
    InitCLK();//系统时钟32M
    InitLED();//灯的初始化   
    InitUART0();//串口初始化
    InitT1();//定时器初始化
    P1DIR |= 0x03;
    LED1 = 1;
    LED2 = 0;
    while(1)
    {
    }
  }
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
    /*.......答题区2开始：定时器设置....................*/
    T1CTL = 0X09;//32分频，自由运行模式 （1001）
    IEN1 |=0X02;//定时器1中断使能 或 T1IE=1
   
    /*.......答题区2结束.......................................*/
     EA = 1; 
}

uint16 Get_adc( )
{
    /*.......答题区3开始：ADC参数设置....................*/
    APCFG |= 1;//设置P0_0为模拟端口
    P0SEL |= 0x01;//设置P0_0为外设
    P0DIR &= ~0x01;//设置P0_0为输入方向
    ADCCON3 = 0x80|0x10|0x00;//或0x90;设置参考电压3.3V 128分频 使用AIN0通道
  /*.......答题区3结束.......................................*/
     
    while(!ADCIF);
    ADCIF=0;
    unsigned long value;
    value = ADCH;
    value = value<<8;
    value |=ADCL;
    value = value*330;//VALUE * 3.3v / 32768
    value = value>>15;
    return (uint16)value;
}
void InitUART0( )
{
   U0CSR |=0X80;//串口模式
    /*.......答题区4开始：串口设置....................*/
   
    PERCFG|= 0x00;//USART0使用备用位置1 P0_2 P0_3
    P0SEL |=0X0C;//设置P0_2 P0_3为外设
    U0UCR |= 0X80;//流控无 8位数据位 无奇偶校验 1位停止位

    U0GCR = 11; //设置波特率为115200 （见书上对应表）
    U0BAUD = 216;
    /*.......答题区4结束.......................................*/
  
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
    
    /*.......答题区5开始：定时2秒后翻转LED2灯，用来指示有在持续通信，并清零计数值....................*/
    if(count>=31)// 定时2秒到
    {
        LED2 = ~LED2; //LED2灯翻转，用来指示有在持续通信
        count = 0; //计数值清零
    /*.......答题区5结束.......................................*/  
        
    /*.......答题区6开始：处理把可燃气体传感数据发送到串口....................*/
        flamgas_val = Get_adc();
        output[0] = flamgas_val/100+'0';
        output[1] = '.';
        output[2] = flamgas_val/10%10+'0';
        output[3] = flamgas_val%10+'0';
        output[4] = 'V';
        output[5] = '\r';
        output[6] = '\n';
        output[7] ='\0';
        
        Uart_tx_string("可燃气体传感器值：",sizeof("可燃气体传感器值："));
        Uart_tx_string(output,8);//发送传感数据到串口
    }
     /*.......答题区6结束.......................................*/

}
void main( )
{
    InitCLK();//系统时钟32M
    InitLED();//灯的初始化   
    InitUART0();//串口初始化
    InitT1();//定时器初始化
    /*.......答题区1开始：灯起始状态....................*/
    LED1 = 1;
    LED2 = 0;  
    /*.......答题区1结束.......................................*/
    while(1)
    {
    }
  }
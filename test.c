/* ����ͷ�ļ� */
#include<iocc2530.h>
#include<string.h>

/*�궨��*/
#define LED1 P1_0
#define LED2 P1_1
#define uint16 unsigned short
/*�������*/
int count=0;//ͳ�ƶ�ʱ���������
char output[8];//���ת�����ַ���ʽ�Ĵ���������
uint16 flamgas_val;//ADC�ɼ����

/*��������*/
void InitCLK(void);//ϵͳʱ�ӳ�ʼ��������Ϊ32MHz
void InitUART0( );//����0��ʼ��
void InitT1( );//��ʱ��1��ʼ��
void Delay(int delaytime);//��ʱ����
unsigned short Get_adc( );//ADC�ɼ�
void Uart_tx_string(char *data_tx,int len);//�����ڷ���ָ�����ȵ�����
void InitLED(void);//�Ƶĳ�ʼ��

/*���庯��*/
void InitCLK(void)
{
  CLKCONCMD &= 0x80;
  while(CLKCONSTA & 0x40);
}

void InitLED()
{
  P1SEL &= ~0x03;//����P1_0��P1_1ΪGPIO��
  P1DIR |= 0x03;//����P1_0��P1_1Ϊ���
  LED1 = LED2 = 0;//����LED1��LED2�ĳ�ʼ״̬
}

void Delay(int delaytime)
{
    int i=0,j=0;
    for(i=0; i<240; i++)
        for(j=0; j<delaytime; j++);
}

void InitT1( )
{
    /*.......����ʱ������....................*/
    T1CTL |= 0x09;
    T1IE = 1;
     EA = 1; 
}

uint16 Get_adc( )
{
    /*.......������3��ʼ��ADC��������....................*/
    
  /*.......������3����.......................................*/
     
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
   U0CSR |=0X80;//����ģʽ
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
    T1IF = 0;//�����ʱ��1�жϱ�־
    count++; //�ۼ��жϴ���
  
        
    value = Get_adc();
    sprintf(str, "%d", value);

    Uart_tx_string(str, sizeof(str));

}
void main( )
{
    InitCLK();//ϵͳʱ��32M
    InitLED();//�Ƶĳ�ʼ��   
    InitUART0();//���ڳ�ʼ��
    InitT1();//��ʱ����ʼ��
    P1DIR |= 0x03;
    LED1 = 1;
    LED2 = 0;
    while(1)
    {
    }
  }
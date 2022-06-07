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
    /*.......������2��ʼ����ʱ������....................*/
    T1CTL = 0X09;//32��Ƶ����������ģʽ ��1001��
    IEN1 |=0X02;//��ʱ��1�ж�ʹ�� �� T1IE=1
   
    /*.......������2����.......................................*/
     EA = 1; 
}

uint16 Get_adc( )
{
    /*.......������3��ʼ��ADC��������....................*/
    APCFG |= 1;//����P0_0Ϊģ��˿�
    P0SEL |= 0x01;//����P0_0Ϊ����
    P0DIR &= ~0x01;//����P0_0Ϊ���뷽��
    ADCCON3 = 0x80|0x10|0x00;//��0x90;���òο���ѹ3.3V 128��Ƶ ʹ��AIN0ͨ��
  /*.......������3����.......................................*/
     
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
   U0CSR |=0X80;//����ģʽ
    /*.......������4��ʼ����������....................*/
   
    PERCFG|= 0x00;//USART0ʹ�ñ���λ��1 P0_2 P0_3
    P0SEL |=0X0C;//����P0_2 P0_3Ϊ����
    U0UCR |= 0X80;//������ 8λ����λ ����żУ�� 1λֹͣλ

    U0GCR = 11; //���ò�����Ϊ115200 �������϶�Ӧ��
    U0BAUD = 216;
    /*.......������4����.......................................*/
  
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
    
    /*.......������5��ʼ����ʱ2���תLED2�ƣ�����ָʾ���ڳ���ͨ�ţ����������ֵ....................*/
    if(count>=31)// ��ʱ2�뵽
    {
        LED2 = ~LED2; //LED2�Ʒ�ת������ָʾ���ڳ���ͨ��
        count = 0; //����ֵ����
    /*.......������5����.......................................*/  
        
    /*.......������6��ʼ������ѿ�ȼ���崫�����ݷ��͵�����....................*/
        flamgas_val = Get_adc();
        output[0] = flamgas_val/100+'0';
        output[1] = '.';
        output[2] = flamgas_val/10%10+'0';
        output[3] = flamgas_val%10+'0';
        output[4] = 'V';
        output[5] = '\r';
        output[6] = '\n';
        output[7] ='\0';
        
        Uart_tx_string("��ȼ���崫����ֵ��",sizeof("��ȼ���崫����ֵ��"));
        Uart_tx_string(output,8);//���ʹ������ݵ�����
    }
     /*.......������6����.......................................*/

}
void main( )
{
    InitCLK();//ϵͳʱ��32M
    InitLED();//�Ƶĳ�ʼ��   
    InitUART0();//���ڳ�ʼ��
    InitT1();//��ʱ����ʼ��
    /*.......������1��ʼ������ʼ״̬....................*/
    LED1 = 1;
    LED2 = 0;  
    /*.......������1����.......................................*/
    while(1)
    {
    }
  }
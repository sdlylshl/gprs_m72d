#include "msp430x54x.h"
#include "GPS.h"

#define GPS_RX_Buffer_Size 100
char  GPS_RX_Buffer[GPS_RX_Buffer_Size]                      ;//GPS���ݻ���100
unsigned int GPS_RX_Buffer_Cnt                               ;//GPS���ݻ������
unsigned char Want_GPS_Data_Type=0x01                        ;//��ҪGPS��������00==����Ӧ��0x11==GPRMC��0x22==GPGGA�� 
unsigned char GPS_R_OK_End_Flag                              ;//GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
//GPS
extern void GPS_UCA1_Init(void)                              ;//GPS�жϳ�ʼ��
extern void SHUT_GPS_UCA1(void)                             ;//�ر�GPS�ж�
//***************************************************************************//
//  Init_UART(void): ��ʼ��UCA1   GPS                                        //
//***************************************************************************//
void GPS_UCA1_Init(void)
{
 //UCA1TX P5.6  UCA1RX P5.7
 /* P5SEL   |= BIT6 + BIT7                                           ; // ѡ�����Ź���
  P5DIR   |= BIT6                                                  ; // TX����Ϊ���
    //��żУ�� UCPEN      1����
    //У��ѡ�� UCPAR      0 ��У��  1żУ��
    //���ͼĴ��� UCMST    0��λ�ȷ� 1 ��λ�ȷ�
    //�ַ����� UC7BIT     0  8λ    1 7λ
    //ֹͣλѡ�� UCSPB    0 1λֹͣλ 1 2λֹͣλ
    //UCSIģʽѡ�� UCMODE 00 UARTģʽ 01 ���ģʽ 10��ַλ���ģʽ 11 �Զ������ʼ��
    //ͬ��ģʽ UCSYNC     0�첽ģʽ 1 ͬ��ģʽ
    //1ֹͣλ 8����λ ��У�� �첽 ��λ�ȷ�
    UCA1CTL0 = 0;
    //ʱ��Դѡ�� UCSSELx 00 �ⲿʱ�� UCLK 01 ACLK 10 SMCLK 11 SMCLK
    //���ճ����ж����� UCRXEIE 0 ������ 1 �����ж�
    //���մ���ַ��ж����� UCBRKIE 0 ������UCRXIFG 1����UCRXIFG
    //˯��״̬ UCDORM 0��˯�� 1 ˯��
    //���͵�ַλ UCTXADDR 0���͵���һ֡������ 1 ���͵���һ֡�ǵ�ַ �����ģʽ��
    //���ʹ�� 0���͵���һ֡���Ǵ�� ���Զ������ʼ�⣩
    //�����λ���� 0 ��ֹ 1 ���� (��λ״̬�� USCI�����߼���ƽ)
    UCA1CTL1 = UCSWRST; // ״̬����λ
    UCA1CTL1 |= UCSSEL__SMCLK; // CLK =
    //1.048M  115200 BR=  9 BRS=1 BRF=0
    //1.048M  9600 BR=109 BRS=2 BRF=0
    //20M 115200 BR=173 BRS=5 BRF=0
    //16M 115200 BR=0x8A BRS=7 BRF=0
    //1M  115200 BR=8 BRS=6 BRF=0
    //16M 9600   BR=1666 BRS=6 BRF=0
    //1M  9600   BR=104 BRS=1 BRF=0
    //32K 9600   BR=3   BRS=3 BRF=0
    //������ 115200
    UCA1BR0 = 0x8A; // 32kHz/9600=3.41
    UCA1BR1 = 0x00;
    UCA1MCTL = UCBRS_7 + UCBRF_0; // UCBRSx=3, UCBRFx=0

    UCA1CTL1 &= ~UCSWRST; // ����״̬��
    UCA1IE |= UCRXIE; // ��������ж�  */
  
 //UCA1TX P5.6  UCA1RX P5.7
  P5SEL   |= BIT6 + BIT7                                           ; // ѡ�����Ź���
  P5DIR   |= BIT6                                                  ; // TX����Ϊ���
  UCA1CTL1        = UCSWRST                                        ; // ״̬����λ
  UCA1CTL1       |= UCSSEL_1                                       ; // CLK = ACLK
  UCA1BR0         = 0x03                                           ; // 32kHz/9600=3.41 
  UCA1BR1         = 0x00                                           ; 
  UCA1MCTL        = UCBRS_3 + UCBRF_0                              ; // UCBRSx=3, UCBRFx=0
  UCA1CTL1       &= ~UCSWRST                                       ; // ����״̬��
  UCA1IE         |= UCRXIE                                         ; // ��������ж�
  
  GPS_ANT_IO_Init();
}

void SHUT_GPS_UCA1(void)                                             //�ر�GPS��ʼ��  
{
    //UCA1TX P5.6  UCA1RX P5.7
    P5SEL   &= ~(BIT6 + BIT7)                                        ; // ����Ϊ��ͨIO
    P5DIR   &= ~(BIT6 + BIT7)                                        ; // TX RX ����Ϊ����
    //�ر�UCA1
    UCA1CTL1        = UCSWRST                                        ; // ״̬����λ
}

//***************************************************************************//
//  GPS�����жϷ������                                                      //
//***************************************************************************//
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    
    GPS_RX_Buffer[GPS_RX_Buffer_Cnt] = UCA1RXBUF             ;
    if(GPS_RX_Buffer[GPS_RX_Buffer_Cnt]=='P'&&GPS_RX_Buffer[GPS_RX_Buffer_Cnt-1]=='G')
    {
      if(GPS_RX_Buffer[GPS_RX_Buffer_Cnt-2]=='$')
          GPS_RX_Buffer_Cnt = 0                               ;//�жϽ���ͷ�ļ�
    }
    
    if((GPS_RX_Buffer[GPS_RX_Buffer_Cnt-1]==0x0D)&&(GPS_RX_Buffer[GPS_RX_Buffer_Cnt]==0x0A))
    {
      if(Want_GPS_Data_Type==0x11)
      {
          if((GPS_RX_Buffer[1]=='R')&&(GPS_RX_Buffer[2]=='M')&&(GPS_RX_Buffer[3]=='C'))
          {
             GPS_R_OK_End_Flag=0x11                               ;//GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
             SHUT_GPS_UCA1()                                     ;
          }
      }
      if(Want_GPS_Data_Type==0x22)
      { 
          if((GPS_RX_Buffer[1]=='G')&&(GPS_RX_Buffer[2]=='G')&&(GPS_RX_Buffer[3]=='A'))
           {
             GPS_R_OK_End_Flag=0x22                               ;//GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
             SHUT_GPS_UCA1()                                     ;
           }
      }
    }
    
    GPS_RX_Buffer_Cnt++                                       ;
    if (GPS_RX_Buffer_Cnt > GPS_RX_Buffer_Size) 
    {GPS_RX_Buffer_Cnt = 0;}                                   
}

#include "msp430x54x.h"
#include "PortIO.h"

unsigned long int One_Mint_Warn_Cnt;//һ���Ӽ���

extern unsigned int  OVER_TIMES_CNT;//3.5�������ʱ�����

extern unsigned long int Two_JI_Warn_Cnt;//������������
extern unsigned long int One_JI_Warn_Cnt;//һ����������
extern unsigned long int ExitLMP_Time_Cnt;//�˳��͹���ʱ����
extern unsigned long GSM_GPS_AT_Cmd_Cnt;//����GSM��ȡ������Ϣ����
extern unsigned long int No_SIM_CARD_Cnt;//��ⲻ��SIM����ʼ������
extern unsigned long int GPS_ANT_IO_CNT;//GPS���ߴ���
extern unsigned long int SPI_Iint_Time_Cnt;//430������ʱ
extern unsigned long int SIM_Card_Work_Cnt;//�������ֻ�����ʱ���ʱ
extern unsigned char Soft_VER_OK                                        ;
extern unsigned long int MSP_430_Start_Time             ;//430������ʱ
extern unsigned long LOW_POW_TIME_CNT                                         ;
extern unsigned long int GSM_INIT_TIME_CNT                                    ;//GSM��ʼ����ʱ��
extern unsigned long int GSM_STAT_TIME_CNT                                    ;//����ֻ��� 5���Ӽ��һ��
extern unsigned long int UP_SPI_NUM_CNT                                       ;//Զ����������
extern unsigned long int CALL_MCU_TIME_CNT                                    ;//ʱ�����
extern unsigned long int RD_Data_1_Min_Cnt                                    ;//���� MUC Ӧ��ʱ�����
extern unsigned int GSM_TimeOut_1ms                                           ;
extern unsigned long int Soft_Vers_Cnt                                        ;//��������汾��ʱ���ʱ
extern unsigned long int GSM_Start_Count                                      ;//������ʱ
extern unsigned long int GPS_LOCATE_Time                                      ;//GPS����ʱ��10S
extern unsigned long Write_Flag_time                                          ;
extern unsigned long int THR_Mint_Time_Cnt                                    ;//3.5����ʱ����������ڲ��ϴ����ݼ��
extern unsigned char Want_GPS_Data_Type                                       ;//��ҪGPS��������00==����Ӧ��0x11==GPRMC��0x22==GPGGA�� 
//Timer
//***************************************************************************//
//                                                                           //
//  Init_TimerA0(void): ����TimerA0                                          //
//                                                                           //
//***************************************************************************//
void TIME_A0_Init(void)
{
  //��ʱ�� �����ڱȽ�ģʽ
  
    //TA���ƼĴ���
    //ʱ��ѡ��TASSEL1:TASSEL0 00 TACLK 01 ACLK 10 SMCLK 11 INCLK
    //ʱ�ӷ�Ƶ ID1:ID0      00 1/1 01 1/2 10 1/4 11 1/8
    //ģʽ���� MC1:MC0      00 ֹͣ 01 ������ 10�������� 11 ��������
    //��ʱ������ TACLR      1 ����
    //�ж�����λ TAIE       1 �ж�����
    //�жϱ�־λ TAIFG      1 �ж�����
    //TA0CTL   =TASSEL1+TACLR;//����ʱ����ΪSMCLK,��Ƶϵ������Ϊ1,��������0,��λTimer0_A5
    
    TA0CTL   =TASSEL__ACLK+ID__8+TACLR;//����ʱ����ΪACLK,��Ƶϵ������Ϊ8,��������0,��λTimer0_A5

    //������   0~TA0CCR0-0-TA0CCR0.....
    //�������� 0~0xFFFF~0~0xFFFF......
    //�������� 0~TA0CCR0~0~TA0CCR0....
    //TA0CCR0  =  16000  - 1                                   ; // SMCK=EX2=16MHz�����ü��������ʱ��Ϊ1ms
    
    TA0CCR0  =  4  - 1                                   ; // ACLK=EX1=32.768K/8=4096�����ü��������ʱ��Ϊ1ms

    //����/�ȽϿ��ƼĴ���
    //����ʽ CM1:CM0  00��ֹ���� 01 ������ 10�½��� 11���¶�����
    //����Դѡ�� CCIS1:CCIS0  00 CCIxA 01 CCIxB 10 GND 11 VCC
    //ͬ����ʽ SCS  0 �첽 1 ͬ��
    //����ģʽѡ�� CAP  0�Ƚ� 1����
    //���ģʽ OUTMx 
    //����ź�  0 ����͵�ƽ 1 ����ߵ�ƽ
    //������� COV  1�������
    //�жϱ�־ CCIFG  1 �ж�����
    TA0CCTL0 = CCIE;//���ò��� ʹ�ܱȽ��ж�

    TA0CTL  |= (1 << 4)                                      ; // ���ü�����Ϊ�Ӽ���������
}





//������
extern unsigned long int Heart_Beat_Count;                     //1msʱ�����
//���ݰ�
extern unsigned long int MCU_Data_Count;                     //1msʱ�����
extern unsigned long int MCU_ACK_Count;                              //Ӧ�������
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
   GSM_Start_Count ++                                     ;//������ʱ
   GSM_TimeOut_1ms++                                      ;//ָ�ʱʱ�����
   Heart_Beat_Count++                                     ;//����������ʱ�����
   MCU_Data_Count++                                       ;//���ݰ�����ʱ�����
   MCU_ACK_Count++                                        ;//���ݰ���Ӧ��ʱ
   GPS_LOCATE_Time++                                      ;//GPS����ʱ��10S
   Soft_Vers_Cnt  ++                                      ;//��������汾��ʱ���ʱ
   THR_Mint_Time_Cnt    ++                                ;//3.5����ʱ����������ڲ��ϴ����ݼ��
   Write_Flag_time++                                      ;//дflash���
   RD_Data_1_Min_Cnt    ++                                ;//���� MUC Ӧ��ʱ�����
   CALL_MCU_TIME_CNT    ++                                ;//ʱ�����
   UP_SPI_NUM_CNT       ++                                ;//Զ����������
   GSM_STAT_TIME_CNT    ++                                ;//����ֻ��� 5���Ӽ��һ��
   GSM_INIT_TIME_CNT    ++                                ;//GSM��ʼ����ʱ��
   LOW_POW_TIME_CNT     ++                                ;
   SIM_Card_Work_Cnt++;
   SPI_Iint_Time_Cnt++;//430������ʱ
   GPS_ANT_IO_CNT++;//GPS���ߴ���
   GSM_GPS_AT_Cmd_Cnt++;//����GSM��ȡ������Ϣ����
   ExitLMP_Time_Cnt++;//�˳��͹���ʱ����
   One_JI_Warn_Cnt++;//һ����������
   Two_JI_Warn_Cnt++;//������������
   One_Mint_Warn_Cnt++;//һ���Ӽ���
   
   
   if(!(P2IN&PWRON))
   {
	   MSP_430_Start_Time   ++          ;//430������ʱ
   }
   
//    TA0CCR0  = (16000) - 1                                  ;
}





/*******************************************************************\
*	      ��������Do_GPS_Data             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���ա�����GPS����  
*	      ������  
           1��ͨ������GPS�������ͣ�00=����Ӧ��11=GPRMC��22=GPGGA�����Ʋ���
           2��Do_GPS_Mesg_OK_Flag��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
           3��Open_UCA1_UART_Init(void)==�ý���������ݲ�����ʱ��GPS�ж�
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void Time_Count_Value_Init(void)                          //ʱ���������
{
  
   OVER_TIMES_CNT        =0                               ;
   GSM_TimeOut_1ms       =0                               ;//ָ�ʱʱ�����
   Heart_Beat_Count      =0                               ;//����������ʱ�����
   MCU_Data_Count        =0                               ;//���ݰ�����ʱ�����
   MCU_ACK_Count         =0                               ;//���ݰ���Ӧ��ʱ
   GPS_LOCATE_Time       =10241                           ;//GPS����ʱ��10S
   Soft_Vers_Cnt         =0                               ;//��������汾��ʱ���ʱ
   THR_Mint_Time_Cnt     =0                               ;//3.5����ʱ����������ڲ��ϴ����ݼ��
   Write_Flag_time       =0                               ;//дflash���
   RD_Data_1_Min_Cnt     =0                               ;//���� MUC Ӧ��ʱ�����
   CALL_MCU_TIME_CNT     =0                               ;//ʱ�����
   UP_SPI_NUM_CNT        =0                               ;//Զ����������
   GSM_STAT_TIME_CNT     =0                               ;//����ֻ��� 5���Ӽ��һ��
   GSM_INIT_TIME_CNT     =0                               ;//GSM��ʼ����ʱ��
   LOW_POW_TIME_CNT      =0                               ;
   Want_GPS_Data_Type    =0x01                            ;//��ҪGPS��������00==����Ӧ��0x11==GPRMC��0x22==GPGGA�� 
   Soft_VER_OK=0x11;
   SIM_Card_Work_Cnt=0;
   GPS_ANT_IO_CNT=0;//GPS���ߴ���
   No_SIM_CARD_Cnt=0;
   GSM_GPS_AT_Cmd_Cnt=0;//����GSM��ȡ������Ϣ����
   One_JI_Warn_Cnt=0;//һ����������
   Two_JI_Warn_Cnt=0;//������������
   One_Mint_Warn_Cnt=0;//һ���Ӽ���
}

#include "msp430x54x.h"
#include "GSM.h"

//GSM ״̬
#define TCPIP_ERRO 0x20
#define GPRS_ERRO 0x10
#define GSM_ERRO 0x08
#define SIM_ERRO 0x04
#define SLEEP_ERRO 0x02
#define POWER_ERRO 0x01

//0 ���� 0x01�ػ� 0x02���� 0x04 SIM�� 0x08 GSM�������� 0x10GPRS�������� 0x20 TCPIP����
unsigned char GSM_STATUS=0x3D                                     ;
unsigned char UDP_Built_flag                                      ;//11==���罨����00==����Ͽ�����û�н���
unsigned long int GSM_Start_Count=0                               ;//������ʱ

char MSP430_POWER_ON_FLAG;//11=�ϵ磻55=�ϵ�
#define RING BIT0
unsigned char POWER_ON_FLAG                                        ;//11=�ػ���55=����
/*******************************************************************\
*	      ��������GSM_NOM_POW_ON          
*	      �������ⲿ�ļ�����
*	      ���ܣ�  Ӳ������ 
*	      ������  V2
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������GSM_NORMAL_POWER_ON
\*******************************************************************/
void GSM_NOM_POW_ON(void)
{
    P1DIR   &= ~BIT0                                                 ;//P1.0 inputs
    P4DIR   &= ~BIT5                                                 ;//P4.5 inputs stu

    P4DIR   |=  BIT0                                                 ;//P4.1 outputs
    P4OUT   &=  ~  BIT0                                                 ;//GSMPWR L
    Delayms(10)                                                      ;//XX*1Ms��ʱ
    
    //on  4.2
    P4DIR   |=  BIT2                                                 ;//P4.1 outputs
    P4OUT   &= ~BIT2                                                 ;//GSMPWR L
    Delayms(200)                                                     ;//XX*1Ms��ʱ
    P4OUT   |=  BIT2                                                 ;//GSMPWR H
    Delayms(200)                                                     ;//XX*1Ms��ʱ
    GSM_Start_Count=0;//������ʱ
    while((!(P4IN&BIT5))&&(GSM_Start_Count<SECD_10));
    Delayms(50)                                                      ;//XX*1Ms��ʱ
    P4OUT   &= ~BIT2                                                 ;//  GSMPWR L     P4.1 reset
    
    GSM_STATUS = 0x3C                                                ;
    GSM_ECHO   = 0x87                                                ;
    POWER_ON_FLAG   =   0x55                                         ;//55=����
    SIM_CARD_TIME_OUT=500                                            ;//SIM����ʼ����ʱ��
    Delayms(2000)                                                    ;//XX*1Ms��ʱ
}


/*******************************************************************\
*	      ��������GSM_NOM_POW_ON          
*	      �������ⲿ�ļ�����
*	      ���ܣ�  Ӳ������ 
*	      ������  V1
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������GSM_NORMAL_POWER_ON
\*******************************************************************/
/*
void GSM_NOM_POW_ON(void)
{
    P1DIR   &= ~BIT0                                                 ;//P1.0 inputs
    P4DIR   &= ~BIT5                                                 ;//P4.5 inputs stu

    P4DIR   |=  BIT0                                                 ;//P4.1 outputs
    P4OUT   |=  BIT0                                                 ;//GSMPWR L
    Delayms(10)                                                      ;//XX*1Ms��ʱ
    
    //on  4.2
    P4DIR   |=  BIT2                                                 ;//P4.1 outputs
    P4OUT   &= ~BIT2                                                 ;//GSMPWR L
    Delayms(200)                                                     ;//XX*1Ms��ʱ
    P4OUT   |=  BIT2                                                 ;//GSMPWR H
    Delayms(200)                                                     ;//XX*1Ms��ʱ
    GSM_Start_Count=0;//������ʱ
    while((!(P4IN&BIT5))&&(GSM_Start_Count<SECD_30));
    Delayms(50)                                                      ;//XX*1Ms��ʱ
    P4OUT   &= ~BIT2                                                 ;//  GSMPWR L     P4.1 reset
    
    GSM_STATUS = 0x3C                                                ;
    GSM_ECHO   = 0x87                                                ;
    SIM_CARD_TIME_OUT=500                                            ;//SIM����ʼ����ʱ��
}

*/


char * POWER_DOWN = "AT+QPOWD=1"                                     ;//�ػ��ַ���
/*******************************************************************\
*	      ��������NORMAL_POWER_DOWN             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����ػ� 
*	      ������  ��ʱ15S
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void GSM_NOM_POW_OFF(void)
{
  
    if(UDP_Built_flag==0x11)
    {
        UDP_Built_flag      =0x00                                   ;//00==����Ͽ�����û�н���
        GSM_SendCMD(PACKET_Query,"AT+QICLOSE",0, 1000)              ;   
        Delayms(200)                                                ;
        GSM_SendCMD(PACKET_Query,"AT+QIDEACT",0, 1000)              ;
    }
    
    if(GSM_SendCMD(PACKET_CONFIG,POWER_DOWN, 0,100) == 1) 
        POWER_ON_FLAG   =   0x11                                     ;//11=�ػ�
    Delayms(15000)                                                    ;//XX*1Ms��ʱ
    P4DIR   |=  BIT0                                                 ;//P4.1 outputs
    P4OUT   &= ~BIT0                                                 ;//GSMPWR L
    
}
/*******************************************************************\
*	      ��������RE_START_GSM             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����ϵͳ 
*	      ������  
          ��ʽ��  
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void RE_START_GSM(void)                                              //����ϵͳ 
{
    GSM_NOM_POW_OFF()                                                ;
    Delayms(15000)                                                    ;//XX*1Ms��ʱ
    GSM_NOM_POW_ON()                                                 ;
}





void GSM_Sleep()
{

}


//GSM ����״̬���
void GSM_Power_state()
{
if(P4IN&BIT5)
    GSM_STATUS &=~POWER_ERRO;
  else
    GSM_STATUS =0x3C;
}

unsigned long int GSM_STAT_TIME_CNT=60000            ;//����ֻ��� 5���Ӽ��һ��
char * SIM_AT_CPIN = "AT+CPIN?"                       ;//AT+CPIN  ���� PIN
void GSM_SIM_state()
{
    static unsigned char SIM_CPIN                                       ;
    SIM_CPIN   =GSM_SendCMD(PACKET_Query, SIM_AT_CPIN,0, 10)            ;
    if (SIM_CPIN==0xAA) 
    {
        Module_Status[2] &= ~0xC0;
        Write_No_Chg_Card_Flash();//д�����޿�Flash 
    }
    else
    {
        if(SIM_CPIN==0xA0) 
        {
            Module_Status[2] |= 0xC0;
            Write_No_Chg_Card_Flash();//д�����޿�Flash 
        }
    }
}
void GSM_GSM_state()
{
  
  
}


char * GSM_GPRS_AT_State = "AT+QISTAT"                       ;//AT+CPIN  ���� PIN
void GSM_GPRS_state()
{
    if(GSM_SendCMD(PACKET_Query, GSM_GPRS_AT_State,0, 10))
    {
        if(((M72D_Brust_RX_Buf[0]==0x0D)&&(M72D_Brust_RX_Buf[1]==0x0A)&&(M72D_Brust_RX_Buf[2]==0x0D)&&(M72D_Brust_RX_Buf[3]==0x0A))
           &&((M72D_Brust_RX_Buf[4]=='S')&&(M72D_Brust_RX_Buf[5]=='T')&&(M72D_Brust_RX_Buf[6]=='A')&&(M72D_Brust_RX_Buf[7]=='T')&&(M72D_Brust_RX_Buf[8]=='E')))
        {
        
            if(((M72D_Brust_RX_Buf[11]=='P')&&(M72D_Brust_RX_Buf[12]=='D')&&(M72D_Brust_RX_Buf[13]=='P'))
               &&((M72D_Brust_RX_Buf[15]=='D')&&(M72D_Brust_RX_Buf[17]=='A')&&(M72D_Brust_RX_Buf[19]=='T')))
            {   //+PDP DEACT [GPRS��·(TCPIP)�Ͽ� ]&&(M72D_Brust_RX_Buf[16]=='E')&&(M72D_Brust_RX_Buf[18]=='C')
                CONNECT_FAIL_Flag     =0x11                   ;//CONNECT FAIL����ʹ��
                CONNECT_FAIL_RESET()                          ;//����Conect_Fail����
            }
        }
    }
}
void GSM_TCPIP_state()
{
  
  
}

void GSM_State()
{
    if(GSM_STAT_TIME_CNT>SECD_30)
    {
        GSM_STAT_TIME_CNT  =0                ;//����ֻ��� 1���Ӽ��һ��
        GSM_Power_state()                    ; 
        GSM_SIM_state()                      ;
        GSM_GSM_state()                      ;
        GSM_GPRS_state()                     ;
        GSM_TCPIP_state()                    ;
    }
}
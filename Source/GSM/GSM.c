#include "msp430x54x.h"
#include "GSM.h"

//0 ���� 0x01�ػ� 0x02���� 0x04 SIM�� 0x08 GSM�������� 0x10GPRS�������� 0x20 TCPIP����
extern unsigned char GSM_STATUS;
//GSM ״̬
#define TCPIP_ERRO 0x20
#define GPRS_ERRO 0x10
#define GSM_ERRO 0x08
#define SIM_ERRO 0x04
#define SLEEP_ERRO 0x02
#define POWER_ERRO 0x01

//*****************************************************************************//
//M72D���ջ�����
unsigned int M72D_RX_Buf_Cnt                                    ;//���ջ��������
unsigned int UP_Data_Cnt                                        ;//SIM����ʼ����ʱ��
unsigned char packet_type                                       ;//M72D  ������
//M72D ͻ������ָ����ջ����� (δ����ָ�� �����յ����� �� RING +QRD ��)
#define PACKET_BRUST 0x00                                        //PACKET_TYPE=0 (ͻ�����հ�)
#define M72D_Brust_RX_BUF_SIZE 80                                
unsigned char M72D_Brust_RX_Buf[M72D_Brust_RX_BUF_SIZE]          ;
unsigned char M72D_Brust_STATUS                                  ;//ͻ����������� 0 ����Ӧ 0x01 OK 0x02 ERRO 0x04 �������
unsigned char M72D_Brust_Cnt                                     ;


#define PACKET_Query 0x01                                         //PACKET_TYPE=1 (ģ���ѯ��)
#define M72D_Query_RX_BUF_SIZE 50                                 //���ݴ�������ֽ���
unsigned char M72D_Query_RX_Buf[M72D_Query_RX_BUF_SIZE];
unsigned char M72D_Query_STATUS;     //���ð�������� 0 ����Ӧ 0x01 OK 0x02 ERRO 0x04 �������

//PACKET_TYPE=2 (ģ�����ð�)
#define PACKET_CONFIG 0x02
#define M72D_SetConfig_RX_BUF_SIZE 50                         //���ݴ�������ֽ���
unsigned char M72D_SetConfig_RX_Buf[M72D_SetConfig_RX_BUF_SIZE];
unsigned char M72D_SetConfig_STATUS; //���ð�������� 0 ����Ӧ 0x01 OK 0x02 ERRO 0x04 �������

//M72D ���ݰ����ջ��� ����������
//PACKET_TYPE=4 (���ݽ��հ�)
#define PAKET_DATA 0x04
#define M72D_ServerData_RX_BUF_SIZE 1200                         //���ݴ�������ֽ���
unsigned char M72D_ServerData_RX_Buf[M72D_ServerData_RX_BUF_SIZE];
unsigned char M72D_ServerData_STATUS; //���ݰ�������� 0 ����Ӧ 0x01 OK 0x02 ERRO 0x04 �������

//M72D ��������  ��Ҫ�������ʱ�䳤�İ���������
//PACKET_TYPE=8 (��ʱ���)
#define PACKET_NETCONFIG 0x08
#define M72D_NetConfig_RX_BUF_SIZE 100                         //���ݴ�������ֽ���
unsigned char M72D_NetConfig_RX_Buf[M72D_NetConfig_RX_BUF_SIZE];
unsigned char M72D_NetConfig_STATUS; //������������ 0 ����Ӧ 0x01 OK 0x02 ERRO 0x04 �������

//M72D δ֪��
//PACKET_TYPE=0x10 (δ֪��)
#define PCKET_OTHER 0x10
#define M72D_Other_RX_BUF_SIZE 100                         //���ݴ�������ֽ���
unsigned char M72D_Other_RX_Buf[M72D_Other_RX_BUF_SIZE];
unsigned char M72D_Other_STATUS;              //δ֪��������� 0 ����Ӧzx02 ERRO 0x04 �������
//*****************************************************************************//
//GSM
extern void GPS_UCA1_Init(void);
extern void SHUT_GPS_UCA1(void);
extern void GSM();
extern char Conect_Fail_Reset(void)                                         ;//����Conect_Fail����
extern char GSM_Brust()                                                     ;//ͻ�����ݴ���
//***************************************************************************//
//  Init_UART(void): ��ʼ��UCA0   GSM                                        //
//***************************************************************************//
void GSM_UCA0_Init(void)
{
    //UCA0TX P3.4 UCA0RX P3.5
    P3SEL |= BIT4 + BIT5; // ѡ�����Ź���
    P3DIR |= BIT4; // TX����Ϊ���
    //��żУ�� UCPEN      1����
    //У��ѡ�� UCPAR      0 ��У��  1żУ��
    //���ͼĴ��� UCMST    0��λ�ȷ� 1 ��λ�ȷ�
    //�ַ����� UC7BIT     0  8λ    1 7λ
    //ֹͣλѡ�� UCSPB    0 1λֹͣλ 1 2λֹͣλ
    //UCSIģʽѡ�� UCMODE 00 UARTģʽ 01 ���ģʽ 10��ַλ���ģʽ 11 �Զ������ʼ��
    //ͬ��ģʽ UCSYNC     0�첽ģʽ 1 ͬ��ģʽ
    //1ֹͣλ 8����λ ��У�� �첽 ��λ�ȷ�
    UCA0CTL0 = 0;
    //ʱ��Դѡ�� UCSSELx 00 �ⲿʱ�� UCLK 01 ACLK 10 SMCLK 11 SMCLK
    //���ճ����ж����� UCRXEIE 0 ������ 1 �����ж�
    //���մ���ַ��ж����� UCBRKIE 0 ������UCRXIFG 1����UCRXIFG
    //˯��״̬ UCDORM 0��˯�� 1 ˯��
    //���͵�ַλ UCTXADDR 0���͵���һ֡������ 1 ���͵���һ֡�ǵ�ַ �����ģʽ��
    //���ʹ�� 0���͵���һ֡���Ǵ�� ���Զ������ʼ�⣩
    //�����λ���� 0 ��ֹ 1 ���� (��λ״̬�� USCI�����߼���ƽ)
    UCA0CTL1 = UCSWRST; // ״̬����λ
    UCA0CTL1 |= UCSSEL__SMCLK; // CLK =
    //1.048M  115200 BR=  9 BRS=1 BRF=0
    //1.048M  9600 BR=109 BRS=2 BRF=0
    //20M 115200 BR=173 BRS=5 BRF=0
    //16M 115200 BR=0x8A BRS=7 BRF=0
    //1M  115200 BR=8 BRS=6 BRF=0
    //16M 9600   BR=1666 BRS=6 BRF=0
    //1M  9600   BR=104 BRS=1 BRF=0
    //32K 9600   BR=3   BRS=3 BRF=0
    //������ 115200
    UCA0BR0 = 0x8A; // 32kHz/9600=3.41
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS_7 + UCBRF_0; // UCBRSx=3, UCBRFx=0

    UCA0CTL1 &= ~UCSWRST; // ����״̬��
    UCA0IE |= UCRXIE; // ��������ж�

}

void SHUT_GSM_UCA0(void)
{
    //UCA1TX P5.6  UCA1RX P5.7
    P3SEL &= ~(BIT4 + BIT5); // ����Ϊ��ͨIO
    P3DIR &= ~(BIT4 + BIT5); // TX RX ����Ϊ����
    //�ر�UCA1
    UCA0CTL1 = UCSWRST; // ״̬����λ
}


void GSM()
{
    //0 ���� 0x01�ػ� 0x02���� 0x04 SIM�� 0x08 GSM�������� 0x10GPRS�������� 0x20 TCPIP����
    //GSM_SIM_Signal();//�����ź�ǿ��
    //Delayms(1000);
    GSM_State();
    CONNECT_FAIL_RESET();//����Conect_Fail����
    GSM_Brust();//ͻ�����ݴ���
    //GSM����������
    if (GSM_STATUS == 0) 
    {//��ʼ����� ��һ��״̬����
            GSM_Heart_Beat();
            GSM_Send_MCU(); 
            RecData();
            Send_Soft_Version();//��������汾��  
            Call_MCU_Data()    ;//����GPS�ն��ϱ���λ��������
            DO_ROOT_UP()       ;//����Զ����������
            Send_SIM_Card_NUM();//���������ֻ�����
            CHECK_MCU_WARN();//���MCU����
    } 
    else 
    {//Ӳ�����
        if ((GSM_STATUS & 0x03)) 
        {//GPS�ػ�������//��������GSM ���ϼ��
            GSM_NOM_POW_ON();
        } 
        else 
        {//GSM ����״̬���//�������� (ģ�鿪����������)
            GSM_State()                                         ;
            if(GSM_ECHO & 0x80)
            {
              TCPIP_BASIC_SET();
            }
            else
            {
                if (GSM_STATUS & SIM_ERRO) 
                {//SIM�����ʧ��
                    if (SIM_NUM_CHECK())
                    {
                        GSM_STATUS &= ~SIM_ERRO;
                    }
                } 
                else 
                {//��ʼ������
                    if (GSM_STATUS & 0x08) 
                    {
                        if (GSM_CREG_Init())
                            GSM_STATUS &= ~GSM_ERRO;
                    } 
                    else 
                    {
                        //2.GPRS
                        if (GSM_STATUS & 0x10) 
                        {
                            if(GSM_GPRS_Init())
                              GSM_STATUS &= ~GPRS_ERRO;
                        } 
                        else 
                        {//3.TCPIP
                            if (GSM_STATUS & 0x20) 
                            {//TCPIP δ���ӻ�����ʧ��
                                if(GSM_TCPIP_Init())
                                {
                                    UDP_Built_flag        =0x11;
                                    GSM_STATUS &= ~TCPIP_ERRO;
                                    Module_Status[1]  &= ~ 0x30;//GPRS�����쳣
                                    GPS_GSM_System_Stu[1] &= ~0x30;
                                    Heart_Beat_Count =   30720;
                                    READ_UP_SPI_FLASH();//ת������汾�ż���������
                                }
                            }
                            else 
                            {
                                //TCPIP ���ӳɹ�
                                GSM_STATUS = 0;
    
                            }                                        //TCPIP
                        }                                        //GPRS
                    }                                        //GSM
                }                                        //SIM
            }                                       //��������
        }                                        //POWER
    }                                        //STATUS

}                                        //end while

//***************************************************************************//
//  GSM�����жϷ������                                                      //
//***************************************************************************//
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{

    switch (packet_type) {
    case PACKET_BRUST:
      {//ͻ������
          M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
    
          if (M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == '+')
          {
                M72D_RX_Buf_Cnt=0;
                break;
          }
          
          if (M72D_RX_Buf_Cnt > M72D_Brust_RX_BUF_SIZE) 
          {
                M72D_RX_Buf_Cnt = 0;
                M72D_Brust_STATUS = 0xFF; //���
                break;
          } 
             
          if (M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt] == ':')
          {
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'I' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'R' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'D' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'I' )
                    {
                          M72D_Brust_STATUS = Receive_Gprs_Data ; //+QIRDI: 0,1,0 [GPRS ���ݵ��� AT+QINDI=1 ]
                          break;
                    }
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'C' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'M' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'T' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'I' )
                    {
                          M72D_Brust_STATUS = 0x02; //+CMTI:<mem>,<index> [����Ϣ���� AT+CNMI=2,1]
                          break;
                    }
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt -12] == 'N' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt -11] == 'O' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt -10] == 'T' &&
                    
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 8] == 'I' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 7] == 'N' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'R' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'D' )
                    {
                      M72D_Brust_STATUS = 0x04; //+CPIN: READY [SIM ״̬��� AT+CPIN?]
                      break;
                    }
                
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'C' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'R' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'E' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'G' )
                    {
                      M72D_Brust_STATUS = 0x08; //+CREG: 1    [GSM ����״̬ 0 ������ 1���� 2��������]
                      break;
                    }
                
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'C' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'F' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'U' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'N' )
                    {    
                      M72D_Brust_STATUS = 0x40; //+CFUN: 1 [ȫ���ܹ���]
                      break;
                    }    
          }
          
          
        if ( M72D_Brust_RX_Buf[6] == 'C' && M72D_Brust_RX_Buf[4] == 'L' && 
             M72D_Brust_RX_Buf[2] == 'S' && M72D_Brust_RX_Buf[0] == 'Q')
            {
                if(M72D_RX_Buf_Cnt>58)
                {
                  M72D_Brust_STATUS = GSM_Get_GPS ; //
                  for(M72D_Brust_Cnt=0;M72D_Brust_Cnt<40;M72D_Brust_Cnt++)                         //�豸IDת��
                   {
                      GSM_GSP_BUF[M72D_Brust_Cnt]=M72D_Brust_RX_Buf[11+M72D_Brust_Cnt]        ;
                   }
                   M72D_Brust_RX_Buf[0]   =0x00     ;
                   M72D_Brust_RX_Buf[1]   =0x00     ;
                }
                break;
            }
          
        if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 7] == 'P' && 
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 5] == 'D' && 
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'A' && 
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'C' &&
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'T' )
            {
                 M72D_Brust_STATUS = 0x20; //+PDP DEACT [GPRS��·(TCPIP)�Ͽ� ]
                 break;
            } 
        break;
      }
    case PACKET_Query:
      {//��ѯ����
            M72D_Query_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_Query_STATUS = 0x01; //OK or SEND OK
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'E'
                    && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_Query_STATUS = 0xA0; //ERROR
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'Y')
                if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'D'
                    && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'A')
                    if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'E'
                        && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 5] == 'R')
                         M72D_Query_STATUS = 0xAA; //ready
            
            
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'G')
                if ((M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 8] == 'N')
                    && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 7] == 'N')
                    M72D_Query_STATUS = 0xB0; //UDP Connectting
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'L')
                if ((M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'F')
                    &&(M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 6] == 'D')
                    &&(M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 9] == 'S'))
                {
                    M72D_Query_STATUS = 0xC0                         ;//SEND FAIL
                    GSM_STATUS |=  TCPIP_ERRO                        ;
                }
           
            
            if (M72D_RX_Buf_Cnt > M72D_Query_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_Query_STATUS = 0xFF; //���
            }
            break;
      }
    case PACKET_CONFIG:
      {//���ý���
            M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
            if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_SetConfig_STATUS = 0x01; //OK
            
            if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'E'
                    && M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_SetConfig_STATUS = 0xA0; //ERROR
            
            if (M72D_RX_Buf_Cnt > M72D_SetConfig_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_SetConfig_STATUS = 0xFF; //���
            }
            break;
      }
        
    case PAKET_DATA:
      {  //���ݽ���
            M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
            
            if(Sys_NOW_UP_Flag==0x11)
                UP_Data_Cnt=1051;
            else
                UP_Data_Cnt=0;
            
            if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                  if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 3] == 0x0A)
                    if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 4] == 0x0D)
                    {
                      if(M72D_RX_Buf_Cnt>30+UP_Data_Cnt)
                        M72D_ServerData_STATUS = 0x01; //������
                      else
                        M72D_ServerData_STATUS = 0x02; //������
                    }
            if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'R' &&
                    M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O' )
                          if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'R' &&
                             M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 5] == 'E' )
                             if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 6] == 0x0A &&
                             M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 7] ==0x0D )
                               if(M72D_RX_Buf_Cnt>30+UP_Data_Cnt)
                                  M72D_ServerData_STATUS = 0xA0; //Error
            
            if (M72D_RX_Buf_Cnt > M72D_ServerData_RX_BUF_SIZE) 
            {
                M72D_RX_Buf_Cnt = 0;
                M72D_ServerData_STATUS = 0xFF; //���
            }
            break;
      }

    case PACKET_NETCONFIG:
      {  //�������
            M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
    
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
              if(M72D_RX_Buf_Cnt>4)
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'T' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_NetConfig_STATUS = 0x01; //CONNECT OK
            
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'L')
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'F' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'I')
                    M72D_NetConfig_STATUS = 0x02; //CONNECT FAIL
    
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'Y')
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'A' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'D')
                    M72D_NetConfig_STATUS = 0x03; //ALREADY CONNECT
            
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == '>')
                    M72D_NetConfig_STATUS = '>'; //�ȴ����ݷ���
    
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'E' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_NetConfig_STATUS = 0xA0; //ERROR
           
            if (M72D_RX_Buf_Cnt > M72D_NetConfig_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_NetConfig_STATUS = 0xFF; //���
            }
    
            break;
      }
    case PCKET_OTHER:
      { //δ֪������
            M72D_Other_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
    
            if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_Other_STATUS = 0x01; //OK
    
            if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'E'
                    && M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_Other_STATUS = 0xA0; //ERROR
    
            if (M72D_RX_Buf_Cnt > M72D_Other_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_Other_STATUS = 0xFF; //���
            }
    
            break;
      }

    default:
        break;
    }
}


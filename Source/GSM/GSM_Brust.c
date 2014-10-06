

#include "GSM.h"

//#include "E:\������\2013\GPSһ���\��һ�� -�޶�2\2013.12.20--00\Source\Global.h"


//0 ���� 0x01�ػ� 0x02���� 0x04 SIM�� 0x08 GSM�������� 0x10GPRS�������� 0x20 TCPIP����   
char GSM_Brust()//ͻ�����ݴ���
{
      char Brst_Num                                           ;
      switch (M72D_Brust_STATUS)
      {
          case Receive_Gprs_Data:
            {//M72D_Brust_STATUS = 0x01; //+QIRDI: 0,1,0 [GPRS ���ݵ��� AT+QINDI=1]
                Delayms(5)                                    ;//XX*1Ms��ʱ
                RD_Data_1_Min_Cnt=RD_Data_Time+1              ;//���� MUC Ӧ��ʱ�����
                RD_UCA0_Time=10240;
                RecData()                                     ;//������������ת��
                break                                         ;
            }
            
          case 0x02:
            {//+CMTI:<mem>,<index> [����Ϣ���� AT+CNMI=2,1]
                break                                         ;
            }
            
          case GSM_Get_GPS:
            {//+CMTI:<mem>,<index> [����Ϣ���� AT+CNMI=2,1]
                Do_GSM_GPS_Data();//����GSM����GPS����
                break;
            } 
            
          case 0x04:
            {//+CPIN: READY [SIM ״̬��� AT+CPIN?]
                Module_Status[2] |= 0XC0;//ȡ������
                GPS_GSM_System_Stu[2] |= 0XC0; 
                Write_No_Chg_Card_Flash();//д�����޿�Flash 
                CONNECT_FAIL_Flag=0x11;//CONNECT FAIL����ʹ��
                CONNECT_FAIL_RESET();//����Conect_Fail����
                break                                         ;
            }
            
          case 0x08:
            {//M72D_Brust_STATUS = 0x02; //+CREG: 1
                CONNECT_FAIL_Flag     =0x11                   ;//CONNECT FAIL����ʹ��
                CONNECT_FAIL_RESET()                          ;//����Conect_Fail����
                break                                         ;
            }

          case 0x20:
            {//+PDP DEACT [GPRS��·(TCPIP)�Ͽ� ]
                CONNECT_FAIL_Flag     =0x11                   ;//CONNECT FAIL����ʹ��
                CONNECT_FAIL_RESET()                          ;//����Conect_Fail����
                break                                         ;
            }
            
          case 0x40:
            {//+CFUN: 1 [ȫ���ܹ���]

                break                                         ;
            }
            
          case 0x80:
            {//NORMAL_POWER_DOWN
                GSM_NOM_POW_ON()                              ;
                UDP_Built_flag      =0x00                     ;//00==����Ͽ�����û�н���
                Heart_Beat_Count    =0                        ;//��������1msʱ�����
                if(MCU_Data_Count>(MCU_TimeOut>>2))
                    MCU_Data_Count =   MCU_TimeOut >>  2      ;//1ms ���ݰ�����
                break                                         ;
            }
            
            
          default:
            {
                break                                         ;
            }
      }
    
     if(M72D_Brust_STATUS!=0)
     {//�建����
           for(Brst_Num=0;Brst_Num<M72D_Brust_RX_BUF_SIZE;Brst_Num++)
        {
            M72D_Brust_RX_Buf[Brst_Num]=0x00                  ;
        }
        M72D_Brust_STATUS=0                                   ;
    }  
    return 0                                                  ;
}



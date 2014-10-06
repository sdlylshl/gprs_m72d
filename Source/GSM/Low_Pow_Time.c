
#include "msp430x54x.h"
#include "GSM.h"

char  Low_Pow_Time_Send_ERR_Flag;//�͹���ʱ��������ָ���ʧ�ܱ�־ �ط�
char* Low_Pow_Time_Addr    =  (char *)0x184A;//�͹���ʱ�������ñ�־��ַ

#define  Low_Pow_Time_BUF_SIZE 20
char REC_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE];//���յ͹���ʱ��������
char ANS_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE];//�ظ��͹���ʱ��������

extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//ת��ID�ź��������                              
extern void Delayms(unsigned long int ms);//��ʱXX*1Ms
/*******************************************************************\
*	      ��������Set_LowPower_ModeTime             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  �͹���ʱ��������
*	      ������ 
          ��ʽ�� ID(4)+�������0x69(2)+����ָ��(2)
*	      ����ֵ��������������������   

          00184A        74      �͹���ʱ�������ñ�־
          00184B~001856 ==75\86  �͹���ʱ��������
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char Low_Power_Mode_Time(void)//�͹���ʱ��������
{
    unsigned char   LP_CNT;
    unsigned int CAL_Low_Pow_Time_CRC,NET_Low_Pow_Time_CRC;//У���
      
    NET_Low_Pow_Time_CRC =   REC_Low_Pow_Time_BUF[9]                                ;
    NET_Low_Pow_Time_CRC =   NET_Low_Pow_Time_CRC  <<8                              ;
    NET_Low_Pow_Time_CRC =   NET_Low_Pow_Time_CRC +   REC_Low_Pow_Time_BUF[8]             ;
    CAL_Low_Pow_Time_CRC =crc_modbus2((unsigned char *)REC_Low_Pow_Time_BUF,
                                Low_Pow_Time_BUF_SIZE-2)                  ;//8
    if(CAL_Low_Pow_Time_CRC==NET_Low_Pow_Time_CRC)
    {
        Tran_ID_CmdNum(ANS_Low_Pow_Time_BUF,0x64)                            ;//ת��ID�ź��������

        Flash_WriteChar(74,0xAA)                                        ;//д��־λ
        for(LP_CNT=0;LP_CNT<12;LP_CNT++)                                 //дIP���˿ں�
        {  
           _NOP();_NOP();_NOP();_NOP();_NOP();          
           Flash_WriteChar(75+LP_CNT,REC_Low_Pow_Time_BUF[6+LP_CNT])    ;      
        }  
        _NOP();_NOP();_NOP();_NOP();_NOP();
        
        
        for(LP_CNT=0;LP_CNT<12;LP_CNT++)                                 //дIP���˿ں�
        {  
           ANS_Low_Pow_Time_BUF[6+LP_CNT] = * (Low_Pow_Time_Addr+1+LP_CNT);//6-17  
           _NOP();_NOP();
        }  
    
        CAL_Low_Pow_Time_CRC=crc_modbus2((unsigned char *)ANS_Low_Pow_Time_BUF,
                                    (Low_Pow_Time_BUF_SIZE-2))          ;//18
        ANS_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE-2]=(char)(CAL_Low_Pow_Time_CRC&0xFF);
        ANS_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE-1]=(char)((CAL_Low_Pow_Time_CRC>>8)&0xFF);//19
    
        Low_Pow_Time_Send_ERR_Flag   =    1                                 ;
        if(GSM_SendData(ANS_Low_Pow_Time_BUF,Low_Pow_Time_BUF_SIZE))             //���ݷ���
        {
           Low_Pow_Time_Send_ERR_Flag    =   0                              ;
           return 1                                                     ;
        }
    
        if(Low_Pow_Time_Send_ERR_Flag)                                           //ʧ���ط� 
        {
           Delayms(200);//XX*1Ms��ʱ
           Low_Pow_Time_Send_ERR_Flag     =   0;
           if(GSM_SendData(ANS_Low_Pow_Time_BUF,Low_Pow_Time_BUF_SIZE))
           {          
              Low_Pow_Time_Send_ERR_Flag     =   0                          ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;
}





/**********************************************************************\
*	      ��������RD_Up_Speed_FLASH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ȡ���������� �޿�����FLASH
*	      ������  
          ��ʽ�� 
*	      ����ֵ��
*
*	      �޸���ʷ��
\**********************************************************************/
void RD_Low_Pow_Time_FLASH(void)//���������� �޿�����
{
    if((* Low_Pow_Time_Addr==0xAA))
    {

    }
}


/**********************************************************************\
*	      ��������RD_Up_Speed_FLASH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ȡ���͹���ʱ��������FLASH
*	      ������  
          ��ʽ�� 
*	      ����ֵ��
*
*	      �޸���ʷ��
\**********************************************************************/
void Ceshi_Low_Pow_Time(void)//���Ե͹���ʱ��������
{
    unsigned char   LK_CNT;
    unsigned int CAL_Up_Speed_CRC;//У���
    
    
    for(LK_CNT=0;LK_CNT<4;LK_CNT++)                         //�豸IDת��
    {
       REC_Low_Pow_Time_BUF[LK_CNT]=GPS_GSM_ID_Memory[LK_CNT]        ;
    }

    REC_Low_Pow_Time_BUF[4]=0x54;
    REC_Low_Pow_Time_BUF[5]=0x54;
    

    CAL_Up_Speed_CRC=crc_modbus2((unsigned char *)REC_Low_Pow_Time_BUF,
                                (Low_Pow_Time_BUF_SIZE-2))          ;//8
    REC_Low_Pow_Time_BUF[8]    =   (char)(CAL_Up_Speed_CRC&0xFF)        ;
    REC_Low_Pow_Time_BUF[9]    =   (char)((CAL_Up_Speed_CRC>>8)&0xFF)   ;
}













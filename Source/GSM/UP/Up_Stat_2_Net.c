#include "msp430x54x.h"
#include "Root_Up.h"

#define ANS_Up_Stat_Buf_Size 20

char ANS_Up_Stat_EROR_Flag                                          ;//����ָ���ʧ�ܱ�־ �ط�
char ANS_Up_Stat_Buf[20]  ;//�ظ�ƽ̨��������ָ������

/*******************************************************************\
*	      ��������ANS_UP_STAT_2_NET             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  �ظ���������ָ��ƽ̨����  
*	      ������ 0x01==������0x00==������ 
          ��ʽ�� ID(4)+�������0x69(2)+����ָ��(2)
*	      ����ֵ��������������ָ��״̬ 
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char ANS_UP_STAT_2_NET(void)                                         //�ظ���������ָ��ƽ̨����
{
    unsigned int ANS_UP_CRC                                         ;
      
    ANS_Up_Stat_EROR_Flag    =1                                     ;
 
    Tran_ID_CmdNum(ANS_Up_Stat_Buf,0x1A)                   ;//ת��ID�ź��������  
    
    ANS_Up_Stat_Buf[6]= 0x00  ;
    ANS_Up_Stat_Buf[7]= 0x14  ;
    
    ANS_Up_Stat_Buf[9]  =   0x01                                    ;//������־00==������,01==����
    
    ANS_UP_CRC=crc_modbus2((unsigned char *)ANS_Up_Stat_Buf,(18))   ;
    ANS_Up_Stat_Buf[18]    =   (char)(ANS_UP_CRC&0xFF)              ;
    ANS_Up_Stat_Buf[19]    =   (char)((ANS_UP_CRC>>8)&0xFF)         ;
   
    if(GSM_SendData(ANS_Up_Stat_Buf,ANS_Up_Stat_Buf_Size))           //���ݷ���
    {
       ANS_Up_Stat_EROR_Flag    =0                                  ;//�޸ı�־λ
       return 1                                                     ;
    }
    
    if(ANS_Up_Stat_EROR_Flag)                                        //ʧ���ط�
    {
       Delayms(50)                                                  ;//XX*1Ms��ʱ
       if(GSM_SendData(ANS_Up_Stat_Buf,ANS_Up_Stat_Buf_Size))
       {          
          ANS_Up_Stat_EROR_Flag    =    0                           ;
          return 1                                                  ;
       }
       ANS_Up_Stat_EROR_Flag    =    0                              ;
    }
    return 0                                                        ;
}















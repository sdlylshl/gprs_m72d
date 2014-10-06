#include "msp430x54x.h"
#include "GSM.h"

char LOOK_M_SD_ER_Flag                                              ;//����ָ���ʧ�ܱ�־ �ط�

#define  NET_LOOK_M_BUF_SIZE 10
char REC_LOOK_M_BUF[NET_LOOK_M_BUF_SIZE]                              ;//���� GPS�ն�����/��������
char ANS_LOOK_M_BUF[NET_LOOK_M_BUF_SIZE];//GPS�ն�����/�������� �ظ�

extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//ת��ID�ź��������                              
extern void Delayms(unsigned long int ms)                           ;//XX*1Ms��ʱ
/*******************************************************************\
*	      ��������GSM_Lock_Monitor             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���ա�����ƽ̨����  
*	      ������ 0x03==������0x00==���� 
          ��ʽ�� ID(4)+�������0x69(2)+����ָ��(2)
*	      ����ֵ��������������������    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char GSM_Lock_Monitor()
{
    unsigned char   LK_CNT                                              ;
    
    unsigned int CAL_LOOK_M_CRC,NET_LOOK_M_CRC                          ;//У���
      
    NET_LOOK_M_CRC =   REC_LOOK_M_BUF[9]                                ;
    NET_LOOK_M_CRC =   NET_LOOK_M_CRC  <<8                              ;
    NET_LOOK_M_CRC =   NET_LOOK_M_CRC +   REC_LOOK_M_BUF[8]             ;
    CAL_LOOK_M_CRC =crc_modbus2((unsigned char *)REC_LOOK_M_BUF,
                                NET_LOOK_M_BUF_SIZE-2)                  ;//8
    if(CAL_LOOK_M_CRC==NET_LOOK_M_CRC)
    {
        Tran_ID_CmdNum(ANS_LOOK_M_BUF,0x96)                            ;//ת��ID�ź��������
        if((REC_LOOK_M_BUF[6]&REC_LOOK_M_BUF[7])==0x03)
        {
            Module_Status[2] |=0x03                                     ;//1����������������������
            GPS_GSM_System_Stu[2]|=0x03                                 ;
            ANS_LOOK_M_BUF[6]=0x03                                      ;
            ANS_LOOK_M_BUF[7]=0x03                                      ;
            
            for(LK_CNT=0;LK_CNT<2;LK_CNT++)
            { 
                Flash_WriteChar(5+LK_CNT,0x03)                          ;
                _NOP();_NOP();_NOP();_NOP();_NOP();
            }
            
        }
        
        if((REC_LOOK_M_BUF[6]&REC_LOOK_M_BUF[7])==0x0C)
        {
            Module_Status[3] |=0x0C                                     ;//2�����������ƹ��ʣ�����
            GPS_GSM_System_Stu[3]|=0x0C                                 ;
            ANS_LOOK_M_BUF[6]=0x0C                                      ;
            ANS_LOOK_M_BUF[7]=0x0C                                      ;
            
            for(LK_CNT=0;LK_CNT<2;LK_CNT++)
            { 
                Flash_WriteChar(5+LK_CNT,0x0C)                          ;
                _NOP();_NOP();_NOP();_NOP();_NOP();
            }
        }
        
        if((REC_LOOK_M_BUF[6]&REC_LOOK_M_BUF[7])==0x00)
        {
            Module_Status[2] &= ~0x03                                   ;//��������\���ƹ��ʽ���
            GPS_GSM_System_Stu[2]&= ~0x03                               ; 
            Module_Status[3] &= ~0x0C                                   ;
            GPS_GSM_System_Stu[3]&= ~0x0C                               ;
            ANS_LOOK_M_BUF[6]=0x00                                      ;
            ANS_LOOK_M_BUF[7]=0x00                                      ;
            
            for(LK_CNT=0;LK_CNT<2;LK_CNT++)
            { 
                Flash_WriteChar(5+LK_CNT,0x00)                          ;
                _NOP();_NOP();_NOP();_NOP();_NOP();
            }
        }    

        CAL_LOOK_M_CRC=crc_modbus2((unsigned char *)ANS_LOOK_M_BUF,
                                    (NET_LOOK_M_BUF_SIZE-2))            ;//8
        ANS_LOOK_M_BUF[8]    =   (char)(CAL_LOOK_M_CRC&0xFF)            ;
        ANS_LOOK_M_BUF[9]    =   (char)((CAL_LOOK_M_CRC>>8)&0xFF)       ;
    
        LOOK_M_SD_ER_Flag   =    1                                      ;
        if(GSM_SendData(ANS_LOOK_M_BUF,NET_LOOK_M_BUF_SIZE))             //���ݷ���
        {
           LOOK_M_SD_ER_Flag    =   0                                   ;
           return 1                                                     ;
        }
    
        if(LOOK_M_SD_ER_Flag)                                           //ʧ���ط� 
        {
           Delayms(200);//XX*1Ms��ʱ
           LOOK_M_SD_ER_Flag     =   0;
           if(GSM_SendData(ANS_LOOK_M_BUF,NET_LOOK_M_BUF_SIZE))
           {          
              LOOK_M_SD_ER_Flag     =   0                               ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;
}



/**********************************************************************\
*	      ��������RD_LOCK_BUS_FLASH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ȡ����״̬
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+����״̬(2)+CRC(2)
*	      ����ֵ��
*
*	      �޸���ʷ��2013.10.22 -������
\**********************************************************************/
void RD_LOCK_BUS_FLASH(void)
{
    char* LK_BS_Addr    =  (char *)0x1805                           ;//�����׵�ַ
    
    
    if((*(LK_BS_Addr) & *(LK_BS_Addr+1))==0x03)
    {
        Module_Status[2]   |=    0x03                               ;//����
        GPS_GSM_System_Stu[2]  |=    0x03                           ;
    }
    
    if((*(LK_BS_Addr) & *(LK_BS_Addr+1))==0x0C)
    {
        Module_Status[3] |=0x0C                                     ;//2�����������ƹ��ʣ�����
        GPS_GSM_System_Stu[3]|=0x0C                                 ;
    }
    
    if((*(LK_BS_Addr) & *(LK_BS_Addr+1))==0x00)
    {
        Module_Status[2] &= ~0x03                                   ;//��������\���ƹ��ʽ���
        GPS_GSM_System_Stu[2]&= ~0x03                               ; 
        Module_Status[3] &= ~0x0C                                   ;
        GPS_GSM_System_Stu[3]&= ~0x0C                               ;
    }

}

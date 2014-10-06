
#include "msp430x54x.h"
#include "GSM.h"

unsigned char CALL_MCU_TIME_FLAG=0x00                                ;//����GPS�ն��ϱ���λ���������־λ 11==����MCU
unsigned long int CALL_MCU_TIME_CNT                                  ;//ʱ�����

#define  CALL_MCU_BUF_SIZE 12
char  REC_CALL_MCU_BUF[CALL_MCU_BUF_SIZE]                            ;//����MCU�����ϴ�ʱ��������
char CALL_MCU_CRC(void)                                              ;//�ظ��ϱ���λ��������
/*******************************************************************\
*	      ��������Call_MCU_Data             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����GPS�ն��ϱ���λ��������
*	      ������  
          ��ʽ��  
*	      ����ֵ��   
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char Call_MCU_Data(void)                                             //����GPS�ն��ϱ���λ��������
{
    if((CALL_MCU_TIME_FLAG==0x11)&&(CALL_MCU_TIME_CNT>SECD_2))
    {   
        CALL_MCU_TIME_CNT   =   0                                   ;//ʱ�����
        MCU_Data_Count  =   MCU_Data_TimeOut    +   1               ;
        Heart_Beat_Count=   10240                                   ;
        if(GSM_Send_MCU())                                             
        {
            CALL_MCU_TIME_FLAG   =  0x00                            ;
        }
        return 1                                                    ;
    }
    return 0                                                        ;
}



char SD_CALL_MCU_FLAG                                               ;//ָ���ʧ�ܱ�־ �ط�
char  ANS_CALL_MCU_BUF[CALL_MCU_BUF_SIZE];//MCU�����ϴ�ʱ�������ûظ� Sd_MCU_Time
/*******************************************************************\
*	      ��������ANSW_CALL_MCU             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����GPS�ն��ϱ���λ��������
*	      ������  
          ��ʽ��  
*	      ����ֵ��   
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char ANSW_CALL_MCU(void)                                            //�ظ��ϱ���λ��������
{
    unsigned char CAL_Num                                               ;//������
    unsigned int CAL_MCU_CRC                                            ;//����GPS�ն��ϱ���λ�������� У���
    if(CALL_MCU_CRC()==1)  
    {
        Tran_ID_CmdNum(ANS_CALL_MCU_BUF,0x9A)                   ;//ת��ID�ź�������� 
        
        for(CAL_Num=0;CAL_Num<4;CAL_Num++)                               //�ظ����м��������ʱ��
        {            
            ANS_CALL_MCU_BUF[6+CAL_Num]=REC_CALL_MCU_BUF[6+CAL_Num]     ;      
        } 
        
        CAL_MCU_CRC=crc_modbus2((unsigned char *)ANS_CALL_MCU_BUF,
                                    (CALL_MCU_BUF_SIZE-2))              ;//10
        ANS_CALL_MCU_BUF[10]    =   (char)(CAL_MCU_CRC&0xFF)            ;
        ANS_CALL_MCU_BUF[11]    =   (char)((CAL_MCU_CRC>>8)&0xFF)       ;
        
        SD_CALL_MCU_FLAG  = 1                                           ;
        if(GSM_SendData(ANS_CALL_MCU_BUF,CALL_MCU_BUF_SIZE))             //���ݷ���   
        {
           SD_CALL_MCU_FLAG  =0                                         ;
           return 1                                                     ;
        }
        
        if(SD_CALL_MCU_FLAG)                                             //ʧ���ط�
        {
           Delayms(200);//XX*1Ms��ʱ
           SD_CALL_MCU_FLAG  = 0;
           if(GSM_SendData(ANS_CALL_MCU_BUF,CALL_MCU_BUF_SIZE))         
           {          
              SD_CALL_MCU_FLAG  = 0                                     ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;  
}






/*******************************************************************\
*	      ��������CALL_MCU_CRC             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����GPS�ն��ϱ���λ��������
*	      ������  
          ��ʽ��  
*	      ����ֵ��   
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char CALL_MCU_CRC(void)                                            //�ظ��ϱ���λ��������
{
  
    unsigned int CAL_CALL_MCU_CRC,NET_CALL_MCU_CRC                        ;//����GPS�ն��ϱ���λ�������� У���
    
    NET_CALL_MCU_CRC=   REC_CALL_MCU_BUF[11]                              ;
    NET_CALL_MCU_CRC =   NET_CALL_MCU_CRC  <<8                            ;
    NET_CALL_MCU_CRC =   NET_CALL_MCU_CRC +   REC_CALL_MCU_BUF[10]        ;
    CAL_CALL_MCU_CRC=crc_modbus2((unsigned char *)REC_CALL_MCU_BUF,
                            CALL_MCU_BUF_SIZE-2)                          ;//10
    if(CAL_CALL_MCU_CRC==NET_CALL_MCU_CRC)
    {
        CALL_MCU_TIME_CNT   =   0                                         ;
        CALL_MCU_TIME_FLAG  =   0x11                                      ;// 11==����MCU
        return 1                                                          ;
    }
    return 0                                                              ;  
}






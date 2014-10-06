#include "msp430x54x.h"
#include "GSM.h"

char SD_SOFT_V_ER_Flag                                              ;//��������汾��ʧ�ܱ�־ �ط�

#define  SOFT_VER_BUF_SIZE 40
char  SOFT_VER_BUF[SOFT_VER_BUF_SIZE];//��������汾��
unsigned long int Soft_Vers_Cnt                                     ;//��������汾��ʱ���ʱ
extern unsigned char Soft_VER_OK                                    ;//����汾��(��ʾ���������������ŵ������汾��)
extern unsigned char Soft_VER_memory[32]                                     ;//�洢����汾��

/*******************************************************************\
*	      ��������Send_Soft_Version             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��������汾�� 
*	      ������  ��ʾ���������������ŵ����һ�������汾�� 
          ��ʽ��  ��������������5���Ӻ���һ��
*	      ����ֵ��   
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
char Send_Soft_Version()
{
    unsigned char Vers_Num                                          ;
    unsigned int CAL_SOFT_VER_CRC                                   ;//У���
    
    if((MSP430_POWER_ON_FLAG==0x11)&&((Soft_VER_OK==0x11)||(SD_SOFT_V_ER_Flag==1))
       &&(Soft_Vers_Cnt>MSP_A0_Min_5)&&(UDP_Built_flag==0x11))
    {
       Soft_Vers_Cnt    =0                                          ;
       Soft_VER_OK      =0xAA                                       ;

       Tran_ID_CmdNum(SOFT_VER_BUF,0x89)                            ;//ת��ID�ź��������
       
       for(Vers_Num=0;Vers_Num<32;Vers_Num++)                        //�汾��ת��
       {
          SOFT_VER_BUF[Vers_Num+6]=Soft_VER_memory[Vers_Num]        ;
       }
       
       
       CAL_SOFT_VER_CRC=crc_modbus2((unsigned char *)SOFT_VER_BUF,
                                    (SOFT_VER_BUF_SIZE-2))          ;//38
       SOFT_VER_BUF[38]    =   (char)(CAL_SOFT_VER_CRC&0xFF)        ;
       SOFT_VER_BUF[39]    =   (char)((CAL_SOFT_VER_CRC>>8)&0xFF)   ;
       
       SD_SOFT_V_ER_Flag    =   1                                   ;
       if(GSM_SendData(SOFT_VER_BUF,SOFT_VER_BUF_SIZE))
        {                                                            //��������汾��
           SD_SOFT_V_ER_Flag   =0                                   ;
           return 1                                                 ;
        }
    }
    if(SD_SOFT_V_ER_Flag)                                    
    {
        Delayms(200);
        SD_SOFT_V_ER_Flag   =0                                    ;
       if(GSM_SendData(SOFT_VER_BUF,SOFT_VER_BUF_SIZE))
       {          
          SD_SOFT_V_ER_Flag   =0                                    ;
          return 1                                                  ;
       }
    }
    return 0                                                        ;
}




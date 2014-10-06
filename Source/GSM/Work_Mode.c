
#include "msp430x54x.h"
#include "GSM.h"

char HML_Mode_BuF[21]                                                   ;//��Ź���ģʽ��������(HMLFB+����)

#define  WORK_MODE_BUF_SZ 29
char REC_WORK_MODE_BUF[WORK_MODE_BUF_SZ]                                ;//����MCU�����ϴ�ʱ��������
char ANS_WORK_MODE_BUF[8]              ;//����ģʽ���� �ظ�
char SD_WORK_MD_ER_FG                                                   ;//����ģʽ���� ����ʧ�ܱ�־ �ط�
char Work_Mode_SPI_OK_Flag                                              ;//����ģʽ���� SPI���Է��ͱ�־ 11==���Է��ͣ�00==���ܷ���

/*******************************************************************\
*	      ��������Work_Mode_Set             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����ϵͳ���й���ģʽ���� 
*	      ������  
          ��ʽ�� ID(4)+�������0x38(2)+HMLFB(20)+΢������(1)+CRC(2)
*	      ����ֵ������IP��ַ\Port��   
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char Work_Mode_Set()
{
    unsigned int CAL_WK_MD_CRC,NET_WK_MD_CRC                            ;//У���
    unsigned int WK_CNT                                                 ;//������
      
    NET_WK_MD_CRC =   REC_WORK_MODE_BUF[28]                             ;
    NET_WK_MD_CRC =   NET_WK_MD_CRC  <<8                                ;
    NET_WK_MD_CRC =   NET_WK_MD_CRC +   REC_WORK_MODE_BUF[27]           ;
    CAL_WK_MD_CRC =crc_modbus2((unsigned char *)REC_WORK_MODE_BUF,
                            WORK_MODE_BUF_SZ-2)                         ;//27
    if(CAL_WK_MD_CRC==NET_WK_MD_CRC)
    {
        Tran_ID_CmdNum(ANS_WORK_MODE_BUF,0x83)                            ;//ת��ID�ź��������

        for(WK_CNT=0;WK_CNT<21;WK_CNT++)                                 //��������ת��SPI
        {            
            HML_Mode_BuF[WK_CNT]=REC_WORK_MODE_BUF[6+WK_CNT]            ;
        }
        Work_Mode_SPI_OK_Flag   =0x11                                   ;//SPI���Է��͹���ģʽ����
        
        Module_Status[5]  |= 0x03                                       ;
        ANS_WORK_MODE_BUF[6]=0xFF                                       ;//�޸Ĺ���ģʽ�ɹ�
        ANS_WORK_MODE_BUF[7]=0xFF                                       ;
    }
    else
    {
        Work_Mode_SPI_OK_Flag   =0x00                                   ;
        Module_Status[5]  &= ~0x03                                      ;
        ANS_WORK_MODE_BUF[6]=0x00                                       ;//�޸Ĺ���ģʽʧ��
        ANS_WORK_MODE_BUF[7]=0x00                                       ;
    }
    
    SD_WORK_MD_ER_FG    =   1                                           ;

    if(GSM_SendData(ANS_WORK_MODE_BUF,sizeof(ANS_WORK_MODE_BUF)))       //���ݷ���
    {
       SD_WORK_MD_ER_FG  =  0;
       return 1;
    }
    
    
    if(SD_WORK_MD_ER_FG)                                                //�ط� 
    {
       Delayms(200);//XX*1Ms��ʱ
       SD_WORK_MD_ER_FG  = 0;
       if(GSM_SendData(ANS_WORK_MODE_BUF,sizeof(ANS_WORK_MODE_BUF)))         
       {          
          SD_WORK_MD_ER_FG    =    0                                    ;
          return 1                                                      ;
       }
    }
    return 0                                                            ;  
}





















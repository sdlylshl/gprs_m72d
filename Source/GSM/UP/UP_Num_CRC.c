

#include "Root_Up.h"

#define UP_NET_Bit_Sum 1051

unsigned long int NET_UP_BIT_SUM                                    ;//�ۼ��ѽ����ļ��ֽ���
unsigned int      SPI_UP_BIT_SUM                                    ;//�ۼ��ѽ����ļ��ֽ���
unsigned char UP_Data_OK_Flag                               ;//
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
char UP_A2_NUM_CRC(void)                                            //��������CRCУ��
{
    unsigned int UP_A2_CRC,UP_ROOT_A2_CRC                           ;//����������ʼУ���
    unsigned int UP_A2_CNT                                          ;    
    static unsigned int UP_A2_CMD_LEN                               ;
    
    UP_A2_CMD_LEN   =   UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-2]                         ;//�����������ݳ���
    UP_A2_CMD_LEN   =   UP_A2_CMD_LEN  <<8                          ;
    UP_A2_CMD_LEN   =   UP_A2_CMD_LEN +UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-1]       ;    
    
    UP_A2_CRC=UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-1]                     ;//������������CRCУ���
    UP_A2_CRC=UP_A2_CRC  <<8                                        ;
    UP_A2_CRC=UP_A2_CRC+UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-2]           ;
    
    UP_ROOT_A2_CRC=crc_modbus2(UP_SYSTEM_C_LANG,UP_NET_Bit_Sum-2)    ;//��������CRCУ���
    
    if((UP_ROOT_A2_CRC==UP_A2_CRC)
          &&(UP_SYSTEM_C_LANG[0]==GPS_GSM_ID_Memory[0])
         &&(UP_SYSTEM_C_LANG[2]==GPS_GSM_ID_Memory[2]))
    {
        
        NET_UP_BIT_SUM=UP_SYSTEM_C_LANG[17];
        NET_UP_BIT_SUM=NET_UP_BIT_SUM<<8;
        NET_UP_BIT_SUM=NET_UP_BIT_SUM+UP_SYSTEM_C_LANG[18];
        NET_UP_BIT_SUM=NET_UP_BIT_SUM<<8;        
        NET_UP_BIT_SUM=NET_UP_BIT_SUM+UP_SYSTEM_C_LANG[19];
        NET_UP_BIT_SUM=NET_UP_BIT_SUM<<8;           
        NET_UP_BIT_SUM=NET_UP_BIT_SUM+UP_SYSTEM_C_LANG[20];
        
        SPI_UP_BIT_SUM=NET_UP_BIT_SUM>>10;
        
        for(UP_A2_CNT=23;UP_A2_CNT<UP_NET_Bit_Sum;UP_A2_CNT++)
        { 
           UP_SYSTEM_C_LANG[UP_A2_CNT-23]=UP_SYSTEM_C_LANG[UP_A2_CNT];
        }
        
        for(UP_A2_CNT=0;UP_A2_CNT<(UP_SYSTEM_C_SIZE-UP_NET_Bit_Sum);UP_A2_CNT++)
        { 
           UP_SYSTEM_C_LANG[UP_NET_Bit_Sum+UP_A2_CNT]=0x00;
        }
        
        Module_Status[5]|=0x0c;//��ÿ��������ɵı�־  
        UP_Data_OK_Flag=0xAA;
        Sys_NOW_UP_Flag=0x55;
        
        return 1                                                    ;
    }
    else
    { 
        return 0                                                    ;
    }
}

































#include "msp430x54x.h"
#include "GSM.h"

unsigned long int MCU_Time_Set_Flash                                    ;//MCU�����ϴ�ʱ��

#define  MCU_TIME_BUF_SZ 12
char  ANS_MCU_TIME_BUF[MCU_TIME_BUF_SZ];//MCU�����ϴ�ʱ�������ûظ� Sd_MCU_Time
char  REC_MCU_TIME_BUF[MCU_TIME_BUF_SZ]                                 ;//����MCU�����ϴ�ʱ��������

char  MCU_Time_Set_EROR_Flag                                            ;//ָ���ʧ�ܱ�־ �ط�
/**********************************************************************\
*	      ��������MCU_Time_Set             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  MCU�����ϴ�ʱ��������  
*	      ������  
          ��ʽ��  ID(4)+�������0x87(2)+ʱ��(4)+CRC(2)
*	      ����ֵ��MCU_Time   
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
char MCU_Time_Set()                                                     //MCU�����ϴ�ʱ�������� 
{
    unsigned char TM_SET                                       ;
    unsigned int CAL_MCU_CRC_SUM,NET_MCU_CRC_SUM                        ;//Խ�籨���������� У���
    char* MCU_Time                                                      ;
    
    NET_MCU_CRC_SUM =   REC_MCU_TIME_BUF[11]                            ;
    NET_MCU_CRC_SUM =   NET_MCU_CRC_SUM  <<8                            ;
    NET_MCU_CRC_SUM =   NET_MCU_CRC_SUM +   REC_MCU_TIME_BUF[10]        ;
    CAL_MCU_CRC_SUM=crc_modbus2((unsigned char *)REC_MCU_TIME_BUF,
                            MCU_TIME_BUF_SZ-2)                          ;//10
    if(CAL_MCU_CRC_SUM==NET_MCU_CRC_SUM)
    {
        Tran_ID_CmdNum(ANS_MCU_TIME_BUF,0x32)                            ;//ת��ID�ź��������

        MCU_Time_Set_Flash    =      REC_MCU_TIME_BUF[6]                ;
        for(TM_SET=0;TM_SET<3;TM_SET++)
        {
            MCU_Time_Set_Flash=MCU_Time_Set_Flash<<8                    ;
            MCU_Time_Set_Flash=MCU_Time_Set_Flash+REC_MCU_TIME_BUF[7+TM_SET];
        }
        MCU_Time_Set_Flash=(MCU_Time_Set_Flash/1000)<<10 ;
        
        if((MCU_Time_Set_Flash>MSP_A0_Min_5)&&(MCU_Time_Set_Flash<MSP_Hour_30))//����ʱ������Ϊ2Min--30H
        {
            MCU_TimeOut =   MCU_Time_Set_Flash                          ;//�޸ĵ�ǰMCU�ϴ�ʱ��
            MCU_Data_TimeOut      =   MCU_TimeOut;//�������ݰ�����ʱ��
            Flash_WriteChar(7,0x11)                                     ;//д��־λ
            _NOP();_NOP();_NOP();_NOP();_NOP();
            Flash_WriteChar(11,MCU_Time_Set_Flash)                      ;
            for(TM_SET=0;TM_SET<3;TM_SET++)                        
            {   
                _NOP();_NOP();_NOP();_NOP();_NOP();
                MCU_Time_Set_Flash=MCU_Time_Set_Flash   >>  8           ;
                Flash_WriteChar((10-TM_SET),MCU_Time_Set_Flash)         ;
            }        
            _NOP();_NOP();_NOP();_NOP();_NOP();            
            MCU_Time    = (char *)0x1808                                ;//��ȡ�ϴ�MCUʱ��
            for(TM_SET=0;TM_SET<4;TM_SET++)                        
            {            
                ANS_MCU_TIME_BUF[6+TM_SET]=*(MCU_Time+TM_SET)           ;
            }
        }
        else
        {
            MCU_Data_TimeOut      =   MCU_TimeOut;//�������ݰ�����ʱ��
            MCU_Time_Set_Flash    =      MCU_TimeOut                    ;//�ϴ�MCUʱ��ʧ��
            ANS_MCU_TIME_BUF[9]   =   MCU_Time_Set_Flash                ;
            for(TM_SET=0;TM_SET<3;TM_SET++)
            {
                MCU_Time_Set_Flash          =MCU_Time_Set_Flash>>8      ;
                ANS_MCU_TIME_BUF[8-TM_SET]  =MCU_Time_Set_Flash;
            }
        }
        
        CAL_MCU_CRC_SUM=crc_modbus2((unsigned char *)ANS_MCU_TIME_BUF,
                                    (MCU_TIME_BUF_SZ-2))                ;//10
        ANS_MCU_TIME_BUF[10]    =   (char)(CAL_MCU_CRC_SUM&0xFF)        ;
        ANS_MCU_TIME_BUF[11]    =   (char)((CAL_MCU_CRC_SUM>>8)&0xFF)   ;
        
        MCU_Time_Set_EROR_Flag=1                                        ;
        if(GSM_SendData(ANS_MCU_TIME_BUF,MCU_TIME_BUF_SZ))                   //���ݷ���
        {
           MCU_Time_Set_EROR_Flag=0                                     ;
           return 1                                                     ;
        }
        if(MCU_Time_Set_EROR_Flag)                                       //ʧ���ط� 
        {
           Delayms(200);
           MCU_Time_Set_EROR_Flag=0;
           if(GSM_SendData(ANS_MCU_TIME_BUF,MCU_TIME_BUF_SZ))         
           {          
              MCU_Time_Set_EROR_Flag=0                                  ;
              return 1                                                  ;
           }
           MCU_Time_Set_EROR_Flag=0                                     ;
        }
    }
    return 0                                                            ;  
}





/**********************************************************************\
*	      ��������RD_MCU_TIME_FLASH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ȡʱ������FLASH
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+ʱ��(4)+CRC(2)
*	      ����ֵ��
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
void RD_MCU_TIME_FLASH(void)
{
    unsigned int RD_MCU_NUM                                             ;
    char* TM_ST_AD_FG   =  (char *)0x1807                               ;//ʱ�����ñ�־
    char* TM_DD_Addr    =  (char *)0x1808                               ;//�����׵�ַ
    
    if(*(TM_ST_AD_FG)==0x11)
    {
        MCU_TimeOut =   *(TM_DD_Addr   )                                ;//���ݰ�����ʱ�� 
        for(RD_MCU_NUM=0;RD_MCU_NUM<4;RD_MCU_NUM++)                            
        {   
            MCU_TimeOut = MCU_TimeOut <<  8                             ;
            MCU_TimeOut = MCU_TimeOut+*(TM_DD_Addr+RD_MCU_NUM)        ; 
        }
        
        MCU_Data_TimeOut    =   MCU_TimeOut;//�������ݰ�����ʱ��
    }

}
















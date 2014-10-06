
#include "msp430x54x.h"
#include "GSM.h"

#define  NET_IP_PORT_BUF_SIZE 14
char REC_IP_PORT_BUF[NET_IP_PORT_BUF_SIZE]                              ;//����IP�˿ں�
char ANS_IP_PORT_BUF[NET_IP_PORT_BUF_SIZE];//IP�˿ں� �ظ�

char IP_PORT_EROR_Flag                                                  ;//IP�˿ں� �ط�
void ReaD_IP_PORT_FLASH(void)                                           ;//��ȡIP��ַ\Port�� 
void IP_2_A(unsigned char NUM,unsigned char ADD)                        ;
void PORT_2_A(unsigned int P_NUM)                                       ;
/**********************************************************************\
*	      ��������IP_Port_Reset             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����IP��ַ\Port��  
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+IP��ַ(4)+Port��(2)
*	      ����ֵ������IP��ַ\Port��   
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
char IP_Port_Reset()
{
    unsigned char PT_Num,IP_Num                                         ;//������
    char* IP_PORT                                                       ;
    unsigned int CAL_IP_PT_CRC,NET_IP_PT_CRC                            ;//У���
      
    NET_IP_PT_CRC =   REC_IP_PORT_BUF[13]                               ;
    NET_IP_PT_CRC =   NET_IP_PT_CRC  <<8                                ;
    NET_IP_PT_CRC =   NET_IP_PT_CRC +   REC_IP_PORT_BUF[12]             ;
    CAL_IP_PT_CRC =crc_modbus2((unsigned char *)REC_IP_PORT_BUF,
                                NET_IP_PORT_BUF_SIZE-2)                 ;//12
    if(CAL_IP_PT_CRC==NET_IP_PT_CRC)
    {
        Tran_ID_CmdNum(ANS_IP_PORT_BUF,0x87)                        ;//ת��ID�ź��������
    
        Flash_WriteChar(12,0x11)                                        ;//д��־λ
        for(IP_Num=0;IP_Num<6;IP_Num++)                                 //дIP���˿ں�
        {  
           _NOP();_NOP();_NOP();_NOP();_NOP();          
           Flash_WriteChar(13+IP_Num,REC_IP_PORT_BUF[6+IP_Num])         ;      
        }  
        
        _NOP();_NOP();_NOP();_NOP();_NOP();
        
        IP_PORT     = (char *)0x180D                                    ;//IP���˿ں��׵�ַ
        for(PT_Num=0;PT_Num<6;PT_Num++)                                 
        {            
            ANS_IP_PORT_BUF[6+PT_Num] =  *(IP_PORT+PT_Num)              ;//�ظ�IP���˿ں�      
        }  
        
        ReaD_IP_PORT_FLASH()                                            ;//��ȡIP��ַ\Port�� 

        CONNECT_FAIL_Flag     =0x11                                     ;//�޸ı�־λ�������޸�IP������ִ������
        
        CAL_IP_PT_CRC=crc_modbus2((unsigned char *)ANS_IP_PORT_BUF,
                                    (NET_IP_PORT_BUF_SIZE-2))           ;//12
        ANS_IP_PORT_BUF[12]    =   (char)(CAL_IP_PT_CRC&0xFF)           ;
        ANS_IP_PORT_BUF[13]    =   (char)((CAL_IP_PT_CRC>>8)&0xFF)      ;
        
        IP_PORT_EROR_Flag   = 1                                         ;
        if(GSM_SendData(ANS_IP_PORT_BUF,NET_IP_PORT_BUF_SIZE))           //���ݷ���   
        {
           IP_PORT_EROR_Flag  =  0                                      ;
           return 1                                                     ;
        }
        if(IP_PORT_EROR_Flag)                                            //ʧ���ط�
        {
            Delayms(200);
            IP_PORT_EROR_Flag  =  0;
           if(GSM_SendData(ANS_IP_PORT_BUF,NET_IP_PORT_BUF_SIZE))         
           {          
              IP_PORT_EROR_Flag  =  0                                   ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;  
}


/*******************************************************************\
*	      ��������ReaD_IP_PORT             
*	      �������ڲ��ļ�����
*	      ���ܣ�  ��ȡIP��ַ\Port��  
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+IP��ַ(8)+Port��(4)
*	      ����ֵ��
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
void ReaD_IP_PORT_FLASH(void)
{
  
    unsigned char RD_NUM,FL_NUM,FL_CNT                                            ;
    unsigned int  PORT_NUM                                          ;
    char* IP_PORT_AD                                                ;//IP���˿ں��׵�ַ
    //char* IP_PT_AD_FG   =  (char *)0x180C                           ;//IP���˿ں�

    IP_PORT_AD     = (char *)0x180D                                 ;

    if(*(IP_PORT_AD-1)==0x11)
    {
        for(FL_NUM=0,FL_CNT=0;FL_NUM<4;FL_NUM++)                                 
        {
            RD_NUM         =   *(IP_PORT_AD + FL_NUM)                   ;
            IP_2_A(RD_NUM,17+FL_CNT)                                    ;
            FL_CNT  =   FL_CNT  +   4                                   ;  
        } 
        
        PORT_NUM    =   *(IP_PORT_AD + 4)                               ;
        PORT_NUM    =   PORT_NUM    <<  8                               ;
        PORT_NUM    =   PORT_NUM    +    *(IP_PORT_AD + 5)              ;
        if(PORT_NUM<10000)
        {
            PORT_2_A(PORT_NUM)                                          ;
        }
    }
}



/*******************************************************************\
*	      ��������ReaD_IP_PORT             
*	      �������ڲ��ļ�����
*	      ���ܣ�  ��ȡIP��ַ\Port��  
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+IP��ַ(8)+Port��(4)
*	      ����ֵ��
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
void IP_2_A(unsigned char I_NUM,unsigned char ADD)
{
    unsigned char RD_IP_NUM,RD_IP_BAI,RD_IP_SHI,RD_IP_GE            ;
    
    RD_IP_NUM   =   I_NUM                                           ;

    RD_IP_SHI   =   RD_IP_NUM   %   100                             ;
    RD_IP_BAI   =   RD_IP_NUM   /   100                             ;
    UDP_Built_STR[ADD+0]   =  HEX_TO_ASCII(RD_IP_BAI)               ;//16�����ַ�ת��ASCII
    RD_IP_GE    =   RD_IP_SHI   %   10                              ;
    RD_IP_SHI   =   RD_IP_SHI   /   10                              ;
    UDP_Built_STR[ADD+1]   =  HEX_TO_ASCII(RD_IP_SHI)               ;
    UDP_Built_STR[ADD+2]   =  HEX_TO_ASCII(RD_IP_GE )               ;
}





/*******************************************************************\
*	      ��������ReaD_IP_PORT             
*	      �������ڲ��ļ�����
*	      ���ܣ�  ��ȡIP��ַ\Port��  
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+IP��ַ(8)+Port��(4)
*	      ����ֵ��
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
void PORT_2_A(unsigned int P_NUM)
{
    unsigned char RD_PT_QIAN,RD_PT_BAI,RD_PT_SHI,RD_PT_GE           ;
    unsigned int RD_IP_NUM                                          ;
    
    RD_IP_NUM   =   P_NUM                                           ;

    RD_PT_BAI    =   RD_IP_NUM   %   1000                           ;
    RD_PT_QIAN   =   RD_IP_NUM   /   1000                           ;
    UDP_Built_STR[35]   =  HEX_TO_ASCII(RD_PT_QIAN)                 ;//16�����ַ�ת��ASCII
    
    RD_PT_SHI    =   RD_PT_BAI   %   100                            ;  
    RD_PT_BAI    =   RD_PT_BAI   /   100                            ;  
    UDP_Built_STR[36]   =  HEX_TO_ASCII(RD_PT_BAI)                  ;
    
    RD_PT_GE     =   RD_PT_SHI   %   10                             ;  
    RD_PT_SHI    =   RD_PT_SHI   /   10                             ;  
    UDP_Built_STR[37]   =  HEX_TO_ASCII(RD_PT_SHI)                  ;  
    UDP_Built_STR[38]   =  HEX_TO_ASCII(RD_PT_GE)                   ;      
}



/**********************************************************************\
*	      ��������Test_IP_Port             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����IP��ַ\Port��  
*	      ������  
          ��ʽ�� 
*	      ����ֵ 
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
void Test_IP_Port(void)
{
    unsigned char Test;
    
    Flash_WriteChar(12,0x11);//д��־λ
    for(Test=0;Test<6;Test++)//дIP���˿ں�
    {  
       _NOP();_NOP();_NOP();_NOP();_NOP();          
       Flash_WriteChar(13+Test,Test);      
    } 
    ReaD_IP_PORT_FLASH();//��ȡIP��ַ\Port�� 
}


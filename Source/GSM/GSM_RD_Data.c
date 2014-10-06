
#include "GSM.h"

unsigned long int RD_Data_Time                                             ;//���� MUC Ӧ��ʱ�����
unsigned int RD_UCA0_Time                                             ;
unsigned long int RD_Data_1_Min_Cnt                                        ;//���� MUC Ӧ��ʱ�����
void RecData()//���� ��������ת��
{
    if((RD_Data_1_Min_Cnt>RD_Data_Time)&&(UDP_Built_flag==0x11))
    {  
        RD_Data_1_Min_Cnt    = 0                                           ;//���� MUC Ӧ��ʱ�����
        if(GSM_SendCMD(PAKET_DATA,"AT+QIRD=0,1,0,1500",0,RD_UCA0_Time)==1)           //��50--200���512���ݶ�ȡ����
        {
            char *str=(char *) M72D_ServerData_RX_Buf                      ;
            long len                                                       ;
            unsigned char IP_Cnt,UP_Cnt,TM_Cnt,OUT_Cnt                     ;
            unsigned int  A2_Cnt,RD_CNT                                    ;
            

            if (GSM_ECHO & 0x01)                                           
            {//�����Թ���//ָ���+3�ֽ�(0x0D 0x0D 0x0A)+����
                str += 30+sizeof("AT+QIRD=0,1,0,1024")                     ;
            } 
            else 
            {//2�ֽ�(0x0D 0x0A)+���� +QIRD: 58.57.53.58:2013,UDP,10\r\n(28+i+2)+\r\n+OK
               str +=30                                                    ; 
            }           
            len=GSM_atol(str)                                              ;
            while(*str++ != '\n')
            {
                if(*str==0x00)
                    break                                                  ;
            }
    

            switch(*(str+4)&*(str+5))  
            {    //���� ��������ת��
                case 0x18:
                {
                     if((*(str+6)==0xFF)&&(*(str+7)==0xFF))                 
                        MCU_Data_ACK=0                                     ;
					 LOW_POW_GSM_Flag         =      0x11                  ;
                     break                                                 ;
                } 
                
                
                case 0xA5:
                {
                     if((*(str+6)==0x33)&&(*(str+7)==0x33))                 
                        SIM_Card_ER_Flag=0                                     ;
                     break                                                 ;
                } 
                
                case 0x19:
                {//��ʽ����ѯ/����(1)+��γ��(16)  OUT_EDGE_BUF_SZ==25
                     if(len==OUT_EDGE_BUF_SZ)
                     {
                         for(OUT_Cnt=0;OUT_Cnt<OUT_EDGE_BUF_SZ;OUT_Cnt++,str++)
                         { 
                             NET_OUT_EDGE_BUF[OUT_Cnt]=*(str)              ;
                         }
                         OUT_EDGE_SET()                                    ;//Խ�籨����������
                     }
                     break                                                 ;
                }
                case 0x23:
                {   //MCU�����ϴ�ʱ��������   MCU_TIME_BUF_SZ==12
                    if(len==MCU_TIME_BUF_SZ)
                    {
                         for(TM_Cnt=0;TM_Cnt<MCU_TIME_BUF_SZ;TM_Cnt++,str++)
                         {
                             REC_MCU_TIME_BUF[TM_Cnt]=*(str)               ;
                         }
                         MCU_Time_Set()                                    ;
                    }
                     break                                                 ;
                }
                
                
                case 0x38:
                { //������������ WORK_MODE_BUF_SZ=29                                                          
                    if(len==WORK_MODE_BUF_SZ)
                    {
                         for(RD_CNT=0;RD_CNT<WORK_MODE_BUF_SZ;RD_CNT++,str++)
                         { //��ʽ��HMLFB(20)+΢������(1)
                            REC_WORK_MODE_BUF[RD_CNT]=*(str)               ;
                         }
                         Work_Mode_Set()                                   ;
                    }
                    break                                                  ;
                }
                
                case 0x54:
                { //������������ WORK_MODE_BUF_SZ=29                                                          
                    if(len==NET_Up_Speed_BUF_SIZE)
                    {
                         for(RD_CNT=0;RD_CNT<NET_Up_Speed_BUF_SIZE;RD_CNT++,str++)
                         { //��ʽ��HMLFB(20)+΢������(1)
                            REC_Up_Speed_BUF[RD_CNT]=*(str)               ;
                         }
                         Allow_Up_Speed()                                  ;
                    }
                    break                                                  ;
                }
                
                
                case 0x69:                                     
                {                                                           //�⳵����������
                     if(len==NET_LOOK_M_BUF_SIZE)
                     {
                         for(IP_Cnt=0;IP_Cnt<NET_LOOK_M_BUF_SIZE;IP_Cnt++,str++)
                         {  
                            REC_LOOK_M_BUF[IP_Cnt]=*(str)                  ;
                         }
                        GSM_Lock_Monitor()                                 ;
                     }
                     break                                                 ;
                } 
                case 0x78:                                             
                {    //��ʽ��IP��ַ(4)+Port��(2)    NET_IP_PORT_BUF_SIZE 14
                     if(len==NET_IP_PORT_BUF_SIZE)
                     {
                         for(IP_Cnt=0;IP_Cnt<NET_IP_PORT_BUF_SIZE;IP_Cnt++,str++)
                         {  
                            REC_IP_PORT_BUF[IP_Cnt]=*(str)                 ;
                         }
                         IP_Port_Reset()                                   ;
                     }
                     break                                                 ;
                } 
                case 0x98:                                      
                {   //����汾���ϴ��ظ�����    
                    if(len==8)
                    {
                         if(*(str+6)==0xFF&&*(str+7)==0xFF)
                           SD_SOFT_V_ER_Flag   =0                          ;
                         else
                           SD_SOFT_V_ER_Flag   =1                          ; 
                    }
                     break                                                 ;
                }
                
                case 0xA1:                                      
                {   //���ط�����������ָ��                                     
                    if(len==29)
                    {
                        for(UP_Cnt=0;UP_Cnt<29;UP_Cnt++,str++)
                        { 
                            UP_Root_Stat_BuF[UP_Cnt]=*(str)                ;
                        }
                        Root_UP_Net_Begin()                                ;
                        RD_Data_1_Min_Cnt   =   256                        ;
                    }
                    break                                                  ;
                }
                
                case 0x2A:                                      
                {                                                           //�������ݴ���                                     
                     for(A2_Cnt=0;A2_Cnt<len;A2_Cnt++,str++)
                     { 
                        UP_SYSTEM_C_LANG[A2_Cnt]=*(str)                    ;
                     }
                     UP_A2_NUM_CRC()                                       ;//��������CRCУ��
                     RD_Data_1_Min_Cnt   =   256                           ;
                     break                                                 ;
                }
                
                
                case 0xA9:                                      
                {   //����GPS�ն��ϱ���λ��������     12                                
                    if(len==CALL_MCU_BUF_SIZE)
                    {
                        for(TM_Cnt=0;TM_Cnt<CALL_MCU_BUF_SIZE;TM_Cnt++,str++)
                        {
                            REC_CALL_MCU_BUF[TM_Cnt]=*(str)                ;
                        }
                        ANSW_CALL_MCU()                                    ;//�ظ��ϱ���λ��������
                    }
                    break                                                  ;
                }
                case 0xB2:                                                  // ���ݰ�Ӧ����
                {
                      if(crc_modbus2((unsigned char *)str,len))      
                      break                                                ;
                }
                  
                default:
                     break                                     ;
            }
        }
    }
}

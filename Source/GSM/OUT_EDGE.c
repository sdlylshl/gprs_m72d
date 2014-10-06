
#include "msp430x54x.h"
#include "GSM.h"


extern unsigned long int GPS_J_Data                                          ;//���������ϴ���Ϣ
extern unsigned long int GPS_W_Data                                          ;//γ�������ϴ���Ϣ

unsigned int OUT_EDGE_CRC_Sum                                       ;//Խ�籨���������� У���

long int NET_SET_J_SMA  =0                                          ;//������Сֵ
long int NET_SET_J_BIG  =0                                          ;//�������ֵ
long int NET_SET_W_SMA   =0                                         ;//γ����Сֵ
long int NET_SET_W_BIG   =0                                         ;//γ�����ֵ


#define OUT_EDGE_BUF_SZ 25
char ANS_OUT_EDGE_BUF[OUT_EDGE_BUF_SZ];//Խ�籨���������ûظ�
char NET_OUT_EDGE_BUF[OUT_EDGE_BUF_SZ]                              ;//ƽ̨�·�Խ�籨������
char SD_OUT_EDGE_EROR_Flag                                          ;//Խ�籨������ʧ�ܱ�־ �ط�
char JG_OUT_EDGE_Flag  =0x00                                        ;//Խ�籨���жϱ�־λ 11=�����ж�
char* EDGE_OUT  = (char *)0x1814                                    ;//Խ�籨��ֵ�׵�ַ




char RD_OUT_EDGE_FH(void)                                           ;//��ȡԽ�籨�� 
void JG_OUT_EDGE(void)                                              ;//�ж�Խ�籨�� 
/*******************************************************************\
*	      ��������OUT_EDGE_SET             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  Խ�籨���������ã��ظ�ƽ̨���ж��Ƿ�Խ��  
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char OUT_EDGE_SET(void)                                              //Խ�籨����������
{
    unsigned char OUT_Num                                           ;
    unsigned int OUT_CRC_SUM                                        ;//Խ�籨���������� У���

    OUT_EDGE_CRC_Sum =   NET_OUT_EDGE_BUF[24]                       ;
    OUT_EDGE_CRC_Sum =   OUT_EDGE_CRC_Sum  <<8                      ;
    OUT_EDGE_CRC_Sum =   OUT_EDGE_CRC_Sum +   NET_OUT_EDGE_BUF[23]  ;
    OUT_CRC_SUM=crc_modbus2((unsigned char *)NET_OUT_EDGE_BUF,
                            OUT_EDGE_BUF_SZ-2)                      ;//23
    
    if(OUT_CRC_SUM==OUT_EDGE_CRC_Sum)
    {
        SD_OUT_EDGE_EROR_Flag=1                                     ;
        Tran_ID_CmdNum(ANS_OUT_EDGE_BUF,0x91)                            ;//ת��ID�ź��������

        ANS_OUT_EDGE_BUF[6]   =    NET_OUT_EDGE_BUF[6]              ;//SetType ��0=��ѯԽ�羭γ�ȣ�1=����Խ�羭γ�ȡ�

        if(NET_OUT_EDGE_BUF[6]==0x01)
        {   //����Խ�羭γ��
            Flash_WriteChar(19,0x11)                                ;//д��־λ
            for(OUT_Num=0;OUT_Num<16;OUT_Num++)                      //дԽ�籨��ֵ
            {      
                _NOP();_NOP();_NOP();_NOP();_NOP();
                Flash_WriteChar(20+OUT_Num,NET_OUT_EDGE_BUF[7+OUT_Num]);      
            }
            _NOP();_NOP();_NOP();_NOP();_NOP();         
            for(OUT_Num=0;OUT_Num<16;OUT_Num++)                                 
            {            
                ANS_OUT_EDGE_BUF[7+OUT_Num] =  *(EDGE_OUT+OUT_Num)  ;//�ظ�flashԽ�羭γ��      
            }
            
            RD_OUT_EDGE_FH()                                        ;//��ȡԽ�籨�� 
        }
        
        if(NET_OUT_EDGE_BUF[6]==0x00)
        {   //��ѯԽ�羭γ��
            if((*(EDGE_OUT-1)==0x11))
            {
                for(OUT_Num=0;OUT_Num<16;OUT_Num++)                                 
                {            
                    ANS_OUT_EDGE_BUF[7+OUT_Num]=*(EDGE_OUT+OUT_Num) ;//�ظ�flashԽ�羭γ��      
                }
            }
            else
            {           
                for(OUT_Num=0;OUT_Num<16;OUT_Num++)                                 
                {            
                    ANS_OUT_EDGE_BUF[7+OUT_Num] =  0x00             ;//�ظ�flashԽ�羭γ��      
                }
            }
            
        }
        
        if((NET_OUT_EDGE_BUF[6]==0x01)||(NET_OUT_EDGE_BUF[6]==0x00))
        {
        
            OUT_CRC_SUM=crc_modbus2((unsigned char *)ANS_OUT_EDGE_BUF,
                                    (OUT_EDGE_BUF_SZ-2))                ;//23
            ANS_OUT_EDGE_BUF[23]    =   (char)(OUT_CRC_SUM&0xFF)        ;
            ANS_OUT_EDGE_BUF[24]    =   (char)((OUT_CRC_SUM>>8)&0xFF)   ;
            
            if(GSM_SendData(ANS_OUT_EDGE_BUF,OUT_EDGE_BUF_SZ))           //���ݷ���   
            {
                SD_OUT_EDGE_EROR_Flag=0                                 ;
                return 1                                                ;
            }
            if(SD_OUT_EDGE_EROR_Flag)                                    //ʧ���ط�
            {
                Delayms(200);
                SD_OUT_EDGE_EROR_Flag=0                             ;
                if(GSM_SendData(ANS_OUT_EDGE_BUF,OUT_EDGE_BUF_SZ))         
                {          
                    SD_OUT_EDGE_EROR_Flag=0                             ;
                    return 1                                            ;
                }
            }
        }
    }
    return 0                                                        ;  
    

}


/*******************************************************************\
*	      ��������RD_OUT_EDGE_FH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ȡԽ�籨��FLASH��γ������ֵ  
*	      ������  
          ��ʽ�� 
*	      ����ֵ��
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char RD_OUT_EDGE_FH(void)                                           //��ȡԽ�籨�� 
{
    unsigned char EDG_Num                                           ;
    unsigned long GPS_Jing_Wei                                        ;//���������ϴ���Ϣ

    if((*( EDGE_OUT-1)==0x11))
    {
        NET_SET_J_SMA   =  *(EDGE_OUT)                              ;//������Сֵ
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_J_SMA  =NET_SET_J_SMA    <<  8                   ;
           NET_SET_J_SMA  =NET_SET_J_SMA  +  *(EDGE_OUT+1+EDG_Num)  ;      
        }                                                           //204093300
        GPS_Jing_Wei    =   (NET_SET_J_SMA  %  100000)  *  6   /10  ;//55980
        NET_SET_J_SMA   =    NET_SET_J_SMA  /  1000000              ;//204
        NET_SET_J_SMA   =    NET_SET_J_SMA*100000+GPS_Jing_Wei      ;//20455980
        
        
        NET_SET_J_BIG   =  *(EDGE_OUT+4)                            ;//�������ֵ
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_J_BIG  =NET_SET_J_BIG    <<  8                   ;
           NET_SET_J_BIG  =NET_SET_J_BIG  +  *(EDGE_OUT+5+EDG_Num)  ;//204093596     
        }
        GPS_Jing_Wei    =   (NET_SET_J_BIG  %  100000)  *  6   /10  ;//56157
        NET_SET_J_BIG   =    NET_SET_J_BIG  /  1000000              ;//204
        NET_SET_J_BIG   =    NET_SET_J_BIG*100000+GPS_Jing_Wei      ;//20456157
        
        
        NET_SET_W_SMA   =  *(EDGE_OUT+8)                            ;//γ����Сֵ
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_W_SMA  =NET_SET_W_SMA    <<  8                   ;
           NET_SET_W_SMA  =NET_SET_W_SMA  +  *(EDGE_OUT+9+EDG_Num)  ;//70804390      
        }
        GPS_Jing_Wei    =   (NET_SET_W_SMA  %  100000)  *  6         ;//26340
        NET_SET_W_SMA   =    NET_SET_W_SMA  /  100000                ;//7080
        NET_SET_W_SMA   =    NET_SET_W_SMA*100000+GPS_Jing_Wei       ;//70802634
        
        
        NET_SET_W_BIG   =  *(EDGE_OUT+12)                           ;//�������ֵ
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_W_BIG  =NET_SET_W_BIG    <<  8                    ;
           NET_SET_W_BIG  =NET_SET_W_BIG  +  *(EDGE_OUT+13+EDG_Num)  ;      
        }
        GPS_Jing_Wei    =   (NET_SET_W_BIG  %  100000)  *  6         ;//26340
        NET_SET_W_BIG   =    NET_SET_W_BIG  /  100000                ;//7080
        NET_SET_W_BIG   =    NET_SET_W_BIG*100000+GPS_Jing_Wei       ;//70802634
        
        JG_OUT_EDGE_Flag  =  0x11                                   ;//Խ�籨���жϱ�־λ 11=�����ж�
        JG_OUT_EDGE()                                               ;//�ж�Խ�籨�� 
    }
    return 0                                                        ;  
    
    
}




/*******************************************************************\
*	      ��������JG_OUT_EDGE             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  �ж�Խ�籨��  
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void JG_OUT_EDGE(void)                                               //�ж�Խ�籨�� 
{
    if(JG_OUT_EDGE_Flag  ==  0x11)                                   //Խ�籨���жϱ�־λ 11=�����ж�
    {
        if(((GPS_W_Data<NET_SET_J_SMA)||(GPS_W_Data>NET_SET_J_BIG)
           ||(GPS_J_Data<NET_SET_W_SMA)||(GPS_J_Data>NET_SET_W_BIG))
           &&((NET_SET_J_SMA!=0)&&(NET_SET_J_BIG!=0)&&(NET_SET_W_SMA!=0)&&(NET_SET_W_BIG!=0))
           &&((GPS_GSM_System_Stu[0]&GPS_Position)))
        {
            Module_Status[1] |=  0xC0                               ;//�������ֵԽ�籨���ж�
            GPS_GSM_System_Stu[1]|=0xC0                             ;
        }
        else
        {
            Module_Status[1] &=~ 0xC0                               ;//�������ֵԽ�籨���ж�
            GPS_GSM_System_Stu[1]&=~0xC0                            ;
        }
    }
}









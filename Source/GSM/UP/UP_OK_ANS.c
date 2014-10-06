
#include "Root_Up.h"

char UP_OK_EROR_Flag                                                  ;//IP�˿ں� �ط�
#define UP_Result_Buf_SZ 20
char ANS_UP_OK_BUF[UP_Result_Buf_SZ]                                  ;//�ϱ�������������ظ�
char REC_UP_OK_BUF[UP_Result_Buf_SZ]                                  ;//ƽ̨�·��ϱ������������
char ROOT_UP_OK_Flag                                                  ;//����������־λ11==OK��00==Fail
//unsigned char UP_STR_Flag                                           ;//������־11==OK��00==Fail


/*******************************************************************\
*	      ��������ANS_UP_Result             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  GPSһ����ϱ������������
*	      ������  
          ��ʽ�� 
                GPS�ն�ID��	    	4
                �������	0xA3	2
                ���ݰ�����	    	2
                ������������		1
                �������ļ��汾��	ASCII��	5+ BCD��	3
                �����������	0x00���ɹ���0x01��ʧ��	1
                У��		        2
*	      ����ֵ��    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char NET_ANS_UP_Result(void)                                        //���ط�����������ָ��
{
    unsigned int CAL_UP_OK_CRC,NET_UP_OK_CRC                            ;//У���
      
    NET_UP_OK_CRC =   REC_UP_OK_BUF[19]                                 ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC  <<8                                ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC +   REC_UP_OK_BUF[18]               ;
    CAL_UP_OK_CRC =crc_modbus2((unsigned char *)REC_UP_OK_BUF,
                                UP_Result_Buf_SZ-2)                     ;//18
    if(CAL_UP_OK_CRC==NET_UP_OK_CRC)
    {
        if(COMP_Peice_STR(REC_UP_OK_BUF,ANS_UP_OK_BUF,6,6,12)==1)
        {
            ROOT_UP_OK_Flag  =0x11                                      ;//����������־λ11==OK��00==Fail
            return 1                                                    ;
        }
    }
    return 0                                                            ;  
}



/*******************************************************************\
*	      ��������ANS_UP_Result             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  GPSһ����ϱ������������
*	      ������  
          ��ʽ�� 
                GPS�ն�ID��	    	4
                �������	0xA3	2
                ���ݰ�����	    	2
                ������������		1
                �������ļ��汾��	ASCII��	5+ BCD��	3
                �����������	0x00���ɹ���0x01��ʧ��	1
                У��		        2
*	      ����ֵ��    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char GPS_ASK_UP_Result(void)                                        //���ط�����������ָ��
{

    unsigned char MV_Num                                                ;//������
    unsigned int CAL_UP_OK_CRC,NET_UP_OK_CRC                            ;//У���
    unsigned int UP_OK_BUF_Len                                          ;//��ָ����ֽ��ܳ��� 
      
    NET_UP_OK_CRC =   REC_UP_OK_BUF[19]                                 ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC  <<8                                ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC +   REC_UP_OK_BUF[18]               ;
    CAL_UP_OK_CRC =crc_modbus2((unsigned char *)REC_UP_OK_BUF,
                                UP_Result_Buf_SZ-2)                     ;//18
    if(CAL_UP_OK_CRC==NET_UP_OK_CRC)
    {
        Tran_ID_CmdNum(ANS_UP_OK_BUF,0xA3)                   ;//ת��ID�ź��������   

        UP_OK_BUF_Len     =  UP_Result_Buf_SZ                           ;//��ָ����ֽ��ܳ���
        ANS_UP_OK_BUF[7]  =  UP_OK_BUF_Len                              ;
        UP_OK_BUF_Len     =  UP_OK_BUF_Len   >>8                        ;
        ANS_UP_OK_BUF[6]  =  UP_OK_BUF_Len                              ;
        
        READ_UP_SPI_FLASH()                                             ;//ת������汾�ż���������
        ANS_UP_OK_BUF[8]  =  UP_STR_A10_Type                            ;//��������ϵͳ���� 
        for(MV_Num=0;MV_Num<8;MV_Num++)                                  //�����ļ��汾��
        {            
            ANS_UP_OK_BUF[9+MV_Num]=ANS_UP_SPI_Num_BuF[14+MV_Num]       ;
        }
        ANS_UP_OK_BUF[17]  =  0x01                                      ;
        
        CAL_UP_OK_CRC=crc_modbus2((unsigned char *)ANS_UP_OK_BUF,
                                    (UP_Result_Buf_SZ-2))               ;//18
        ANS_UP_OK_BUF[18]    =   (char)(CAL_UP_OK_CRC&0xFF)             ;
        ANS_UP_OK_BUF[19]    =   (char)((CAL_UP_OK_CRC>>8)&0xFF)        ;
        
        UP_OK_EROR_Flag   =   1                                         ;
        if(GSM_SendData(ANS_UP_OK_BUF,UP_Result_Buf_SZ))                 //���ݷ���   
        {
           UP_OK_EROR_Flag  =  0                                        ;
           return 1                                                     ;
        }
        if(UP_OK_EROR_Flag)                                              //ʧ���ط�
        {
           if(GSM_SendData(ANS_UP_OK_BUF,UP_Result_Buf_SZ))         
           {          
              UP_OK_EROR_Flag  =  0                                     ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;  
}



























































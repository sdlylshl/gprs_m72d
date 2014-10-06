
#include "Root_Up.h"

unsigned char UP_STR_A10_Type                                       ;//��������ϵͳ���� 
unsigned char UP_STR_Flag                                           ;//������־11==OK��00==Fail

char UP_Root_Stat_BuF[29]                                           ;//������ط�����������ָ��
unsigned char UP_Stat_SPI_BuF[18]                                   ;//�����������SPIָ��
void TRAN_UP_SPI_BUF(char Move_Num)                                 ;//ת������汾��
void READ_UP_SPI_FLASH(void)                                        ;//ת������汾�ż���������

/*******************************************************************\
*	      ��������Root_UP_Net_Begin             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���ط�����������ָ��
*	      ������  
          ��ʽ��  ID(4)+Cmd(2)+ָ����ˮ��(2)+���ݰ�����(2)
                  +������������(1)+���������ļ����ֽ���(2)
                  +�����ļ��汾��(8)+CRCУ��(2))
*	      ����ֵ��    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char Root_UP_Net_Begin(void)                                        //���ط�����������ָ��
{
    unsigned int NET_CNT                                            ;//������
    unsigned int UP_CRC_B_Sum                                       ;//����������ʼУ���
    unsigned int UP_Packet_Len                                      ;//���ط�����������ָ����ֽ��ܳ���
    unsigned int UP_Root_CRC_Sum                                    ;//Զ��������ʼУ���
    
    UP_Packet_Len  =   UP_Root_Stat_BuF[6]                          ;//��������ָ����ֽڳ���
    UP_Packet_Len  =   UP_Packet_Len  <<  8                         ;
    UP_Packet_Len  =   UP_Packet_Len +UP_Root_Stat_BuF[7]           ;
    
    UP_Root_CRC_Sum =   UP_Root_Stat_BuF[28]                        ;
    UP_Root_CRC_Sum =   UP_Root_CRC_Sum  <<8                        ;
    UP_Root_CRC_Sum =   UP_Root_CRC_Sum +   UP_Root_Stat_BuF[27]    ;
    UP_CRC_B_Sum=crc_modbus2((unsigned char *)UP_Root_Stat_BuF,27)  ;
    
    if(UP_CRC_B_Sum==UP_Root_CRC_Sum)
    {
        Flash_WriteChar(57,'V')                                     ;//д��־λ
        for(NET_CNT=0;NET_CNT<9;NET_CNT++)                           //д������������1+�����ļ��汾��8
        {   
           _NOP();_NOP();_NOP();_NOP();_NOP();
           Flash_WriteChar(58+NET_CNT,UP_Root_Stat_BuF[8+NET_CNT])  ;      
        } 
        
        for(NET_CNT=0;NET_CNT<10;NET_CNT++)                           //д������������1+�����ļ��汾��8
        {   
           _NOP();_NOP();_NOP();_NOP();_NOP();
           Flash_WriteChar(87+NET_CNT,UP_Root_Stat_BuF[17+NET_CNT])  ;      
        }
        
        _NOP();_NOP();_NOP();_NOP();_NOP();
        READ_UP_SPI_FLASH()                                         ;//ת������汾�ż���������
        
        UP_STR_A10_Type  =   UP_Root_Stat_BuF[8]                    ;//��������ϵͳ����
        switch(UP_STR_A10_Type)                                      //�����ļ��汾��
        {   
            case 0x12:
            {
                TRAN_UP_SPI_BUF(8)                                  ;//ת������汾�ż�CRCУ��
                Module_Status[4]   |=    0x03                       ;//������־
                ANS_Up_Stat_Buf[8] |=    0x12                       ;//�ظ�ƽ̨��������ָ������
                ANS_UP_STAT_2_NET()                                 ;//�ظ���������ָ��ƽ̨����
                return 1                                            ;
            } //0x03==������ʾ��;
            
            case 0x11:
            {
                Module_Status[4]   |=    0x0C                       ;
                TRAN_UP_SPI_BUF(16)                                 ;
                break                                               ;
            } //0x0C==����������;
            
            case 0x10:
            {
                Module_Status[4]   |=    0x30                       ; 
                TRAN_UP_SPI_BUF(24)                                 ;
                break                                               ;
            } //0x30==�������ſ�����ϵͳ����
            
            case 0x13:
            {
                TRAN_UP_SPI_BUF(32)                                 ;
                break                                               ;
            } //0xC0==����GPS,GSMϵͳ����
            
           default:
                break                                               ;
        }

    }
    return 0                                                        ;
}


/*******************************************************************\
*	      ��������Root_UP_Net_Begin             
*	      �������ڲ��ļ�����
*	      ���ܣ�  ת������汾��
*	      ������  Move_Num==ת������汾����
          ��ʽ��  ��
*	      ����ֵ����    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void TRAN_UP_SPI_BUF(char Move_Num)                                  //ת������汾��
{
    unsigned char UP_Num,RT_Num                                     ;//ת�Ƶ�ǰ����汾�ż�����
    
    Move_Num    =   Move_Num-8                                      ;
    for(UP_Num=0;UP_Num<8;UP_Num++)                                 
    {
        ANS_Up_Stat_Buf[10+UP_Num]=Soft_VER_memory[UP_Num+Move_Num] ;//ת�Ƶ�ǰ����汾��
    }
    
    for(RT_Num=0;RT_Num<18;RT_Num++)                                 //�������������ļ���SPI
    {
        UP_Stat_SPI_BuF[RT_Num]=UP_Root_Stat_BuF[8+1+RT_Num]       ;//������������,�����ļ��汾��    
    }
}



/*******************************************************************\
*	      ��������READ_UP_SPI_FLASH             
*	      �������ڲ��ļ�����
*	      ���ܣ�  ת������汾��
*	      ������  Move_Num==ת������汾����
          ��ʽ��  ��
*	      ����ֵ����    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void READ_UP_SPI_FLASH(void)                                         //ת������汾�ż���������
{
    unsigned char ST_Num                                            ;//ת�Ƶ�ǰ����汾�ż�����
    char* RTUP_ADD   = (char *)0x1839                               ;//��������汾�� ��־λ�׵�ַ001839
    
    if(* (RTUP_ADD)=='V')
    {
        UP_STR_A10_Type  =   * (RTUP_ADD+1)                         ;//��������ϵͳ����
        for(ST_Num=0;ST_Num<8;ST_Num++)                              //ת�������汾�Ÿ����������� 
        {
            ANS_UP_SPI_Num_BuF[14+ST_Num]=* (RTUP_ADD+2+ST_Num);  
            UP_Stat_SPI_BuF[ST_Num]      =* (RTUP_ADD+2+ST_Num);//������������,�����ļ��汾��
        }
        
        for(ST_Num=0;ST_Num<10;ST_Num++)                              //ת�������汾�Ÿ����������� 
        {
            UP_Stat_SPI_BuF[8+ST_Num]      =* (RTUP_ADD+30+ST_Num);//������������,�����ļ��汾��
        }
        UP_SPI_Num_Flag=0x11;
        UP_Send_Times=0;
        //Module_Status[4]|=0x03;//������־  
    }
}
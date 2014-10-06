


#include "msp430x54x.h"
#include "GSM.h"

char Up_Speed_Send_ERR_Flag;//������ �޿�����ָ���ʧ�ܱ�־ �ط�
char* Up_Speed_Addr    =  (char *)0x1848;//�������޿��׵�ַ

#define  Allow_NO_Card  0xC0         //
#define  Allow_Chg_Card 0x30         //

#define  NET_Up_Speed_BUF_SIZE 10
char REC_Up_Speed_BUF[NET_Up_Speed_BUF_SIZE];//���������� �޿�����
char ANS_Up_Speed_BUF[NET_Up_Speed_BUF_SIZE];//�ظ������� �޿����� 

extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//ת��ID�ź��������                              
extern void Delayms(unsigned long int ms);//��ʱXX*1Ms
/*******************************************************************\
*	      ��������GSM_Lock_Monitor             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���ա�����ƽ̨����  
*	      ������ 0x03==�������٣�0x00==���� 
          ��ʽ�� ID(4)+�������0x69(2)+����ָ��(2)
*	      ����ֵ��������������������   

          001848~001849 ==72\73= �޿�����==0xC0�� ��������===0x30

*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char Allow_Up_Speed(void)//���������� �޿�����
{
    unsigned int CAL_Up_Speed_CRC,NET_Up_Speed_CRC;//У���
      
    NET_Up_Speed_CRC =   REC_Up_Speed_BUF[9]                                ;
    NET_Up_Speed_CRC =   NET_Up_Speed_CRC  <<8                              ;
    NET_Up_Speed_CRC =   NET_Up_Speed_CRC +   REC_Up_Speed_BUF[8]             ;
    CAL_Up_Speed_CRC =crc_modbus2((unsigned char *)REC_Up_Speed_BUF,
                                NET_Up_Speed_BUF_SIZE-2)                  ;//8
    if(CAL_Up_Speed_CRC==NET_Up_Speed_CRC)
    {
        Tran_ID_CmdNum(ANS_Up_Speed_BUF,0x45)                            ;//ת��ID�ź��������

        if((REC_Up_Speed_BUF[6]==Allow_NO_Card)&&(REC_Up_Speed_BUF[7]==Allow_Chg_Card))
        {
            Module_Status[2]      &=~ Allow_NO_Card;//ȡ������
            //GPS_GSM_System_Stu[2] &=~ Allow_NO_Card; 
            Module_Status[2]      &=~Allow_Chg_Card;//�޿�����
            //GPS_GSM_System_Stu[2] &=~Allow_Chg_Card;     

            ANS_Up_Speed_BUF[6]=Allow_NO_Card;
            ANS_Up_Speed_BUF[7]=Allow_Chg_Card;
            
            Flash_WriteChar(72,Allow_NO_Card|Allow_Chg_Card);
            _NOP();_NOP();_NOP();_NOP();_NOP();
            Flash_WriteChar(73,Allow_NO_Card|Allow_Chg_Card);
            
            Write_No_Chg_Card_Flash();//д�����޿�Flash 
        }
        
        CAL_Up_Speed_CRC=crc_modbus2((unsigned char *)ANS_Up_Speed_BUF,
                                    (NET_Up_Speed_BUF_SIZE-2))          ;//8
        ANS_Up_Speed_BUF[8]    =   (char)(CAL_Up_Speed_CRC&0xFF)        ;
        ANS_Up_Speed_BUF[9]    =   (char)((CAL_Up_Speed_CRC>>8)&0xFF)   ;
    
        Up_Speed_Send_ERR_Flag   =    1                                 ;
        if(GSM_SendData(ANS_Up_Speed_BUF,NET_Up_Speed_BUF_SIZE))             //���ݷ���
        {
           Up_Speed_Send_ERR_Flag    =   0                              ;
           return 1                                                     ;
        }
    
        if(Up_Speed_Send_ERR_Flag)                                           //ʧ���ط� 
        {
           Delayms(200);//XX*1Ms��ʱ
           Up_Speed_Send_ERR_Flag     =   0;
           if(GSM_SendData(ANS_Up_Speed_BUF,NET_Up_Speed_BUF_SIZE))
           {          
              Up_Speed_Send_ERR_Flag     =   0                          ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;
}



/**********************************************************************\
*	      ��������RD_Up_Speed_FLASH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ȡ���������� �޿�����FLASH
*	      ������  
          ��ʽ�� 
*	      ����ֵ��
*
*	      �޸���ʷ��
\**********************************************************************/
void RD_Up_Speed_FLASH(void)//���������� �޿�����
{
    if((((GPS_GSM_System_Stu[2]&Allow_NO_Card)==Allow_NO_Card)
         ||((GPS_GSM_System_Stu[2]&Allow_Chg_Card)==Allow_Chg_Card))&&
       (((*Up_Speed_Addr)==(Allow_NO_Card|Allow_Chg_Card))&&
         (*(Up_Speed_Addr+1)==(Allow_NO_Card|Allow_Chg_Card))))
    {
        Module_Status[2]       &=~ Allow_NO_Card;//ȡ������
        //GPS_GSM_System_Stu[2]  &=~ Allow_NO_Card; 
        
        Module_Status[2]       &=~Allow_Chg_Card;
        //GPS_GSM_System_Stu[2]  &=~Allow_Chg_Card;//�޿����� 
        
        //if(MSP430_POWER_ON_FLAG==0x11)
        //{
            //Write_No_Chg_Card_Flash();//д�����޿�Flash 
            //Flash_WriteChar(72,Allow_NO_Card|Allow_Chg_Card);
            //_NOP();_NOP();_NOP();_NOP();_NOP();
            //Flash_WriteChar(73,Allow_NO_Card|Allow_Chg_Card);
        //}
    }
}



/**********************************************************************\
*	      ��������RD_Up_Speed_FLASH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ȡ���������� �޿�����FLASH
*	      ������  
          ��ʽ�� 
*	      ����ֵ��
*
*	      �޸���ʷ��
\**********************************************************************/
void Ceshi_Allow_Up_Speed(void)//���Դ��������� �޿�����
{
    unsigned char   LK_CNT;
    unsigned int CAL_Up_Speed_CRC;//У���
    
    
    for(LK_CNT=0;LK_CNT<4;LK_CNT++)                         //�豸IDת��
    {
       REC_Up_Speed_BUF[LK_CNT]=GPS_GSM_ID_Memory[LK_CNT]        ;
    }

    REC_Up_Speed_BUF[4]=0x54;
    REC_Up_Speed_BUF[5]=0x54;
    
    REC_Up_Speed_BUF[6]=Allow_NO_Card;
    REC_Up_Speed_BUF[7]=Allow_Chg_Card;    
    
    CAL_Up_Speed_CRC=crc_modbus2((unsigned char *)REC_Up_Speed_BUF,
                                (NET_Up_Speed_BUF_SIZE-2))          ;//8
    REC_Up_Speed_BUF[8]    =   (char)(CAL_Up_Speed_CRC&0xFF)        ;
    REC_Up_Speed_BUF[9]    =   (char)((CAL_Up_Speed_CRC>>8)&0xFF)   ;
}








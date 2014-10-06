
#include "msp430x54x.h"
#include "GSM.h"


char HAVE_SIM_NUM_FLASH    =0x00                                    ;//�洢�ֻ�����FLASH 11=�Ѿ�д�룬������д��55==����д��
char WR_SIM_NUM_FH_OK_FG   =0x00                                    ;//Խ�籨���жϱ�־λ 11=�����ж�
char FLASH_SIM_NUM[20]                                              ;//FLASHת�Ƶ�����
char M72_SIM_NUM[20]                                                ;//FLASHת�Ƶ�����
#define  Send_SIM_Card_BUF_SIZE 49
char Send_SIM_Card_BUF[Send_SIM_Card_BUF_SIZE];//GPS�ն�����/�������� �ظ�



/*******************************************************************\
*	      ��������READ_SIM_NUM_FH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  �洢�ֻ�����FLASH���ж��Ƿ�����ֻ���  
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void READ_SIM_NUM_FH(void)                                           //�洢�ֻ����� 
{
    unsigned char RED_Num                                           ;
    char* SIM_NUM   = (char *)0x1824                                ;//SIM���ű�־λ�׵�ַ
    
    if(* (SIM_NUM)==0x11)
    {
        for(RED_Num=0;RED_Num<20;RED_Num++)                          //дSIM����
        {            
            FLASH_SIM_NUM[RED_Num]  =   *(1 + RED_Num + SIM_NUM)    ;
            Send_SIM_Card_BUF[6+RED_Num]=*(1 + RED_Num + SIM_NUM);
        }
        HAVE_SIM_NUM_FLASH  =0x11                                   ;//�洢�ֻ�����FLASH 11=�Ѿ�д�룬������д
    }
    else
    {
        HAVE_SIM_NUM_FLASH  =0x55                                   ;//�洢�ֻ�����FLASH 55==����д��
    }
}


/*******************************************************************\
*	      ��������SAVE_SIM_NUM_FH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  �洢�ֻ�����FLASH���ж��Ƿ�����ֻ���  
*	      ������  D_NUM==����λ����
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char SAVE_SIM_NUM_FH(unsigned char CMD_LEN)                           //�洢�ֻ����� 
{
    unsigned char SIM_Num                                           ;
    
    if((M72D_Query_RX_Buf[0]==0x0D&&M72D_Query_RX_Buf[1]==0x0A)
       &&(M72D_Query_RX_Buf[22]==0x0D&&M72D_Query_RX_Buf[23]==0x0A)
         &&(M72D_Query_RX_Buf[26]=='O'&&M72D_Query_RX_Buf[27]=='K'))     //�洢�ֻ�����FLASH 55==����д��
    {
        if(HAVE_SIM_NUM_FLASH==0x55)
        {
            Flash_WriteChar(36,0x11)                                ;//д��־λ
            for(SIM_Num=0;SIM_Num<20;SIM_Num++)                          //дSIM����
            {    
               _NOP();_NOP();_NOP();_NOP();_NOP();
               Flash_WriteChar(37+SIM_Num,
                               M72D_Query_RX_Buf[CMD_LEN+SIM_Num])  ; 
            } 
            _NOP();_NOP();_NOP();_NOP();_NOP();
        }
        
        READ_SIM_NUM_FH();
        for(SIM_Num=0;SIM_Num<20;SIM_Num++)                          //дSIM����
         {            
               M72_SIM_NUM[SIM_Num]=M72D_Query_RX_Buf[CMD_LEN+SIM_Num]  ; 
         }
        return 1;
    }
    return 0;
}







/*******************************************************************\
*	      ��������READ_SIM_NUM_FH             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  �洢�ֻ�����FLASH���ж��Ƿ�����ֻ���  
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void SIM_CHG_WARN(void)                            //�����ֻ������� 
{
    if(HAVE_SIM_NUM_FLASH==0x11)      
    {                                         //�洢�ֻ�����
        if(Compare_String(M72_SIM_NUM,FLASH_SIM_NUM,20)==1)          //�Ƚ������ַ����Ƿ���ͬ
        {
             Module_Status[2] &= ~0x30;//ģ��״̬��Ϣ
             GPS_GSM_System_Stu[2]&= ~0x30;
             Write_No_Chg_Card_Flash();//д�����޿�Flash 
        }
        else
        {
             Module_Status[2] |=0x30;
             GPS_GSM_System_Stu[2]|=0x30;//GPS״̬�洢 
             Write_No_Chg_Card_Flash();//д�����޿�Flash 
        }     
    }
}


unsigned long int SIM_Card_Work_Cnt;//�������ֻ�����ʱ���ʱ
char SIM_Card_Work_Flag;//�ϵ��ֻ����ű�־
char SIM_Card_ER_Flag;//�����ֻ�����ʧ�ܱ�־ �ط�

/*******************************************************************\
*	      ��������Send_SIM_Card_NUM             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���������ֻ�����  
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ��2013.12.13 ����
\*******************************************************************/
void Send_SIM_Card_NUM(void)                            //���������ֻ�����
{

    unsigned char   SIM_CNT                                              ;
    unsigned int CAL_SIM_CRC;//У���
      
   if((MSP430_POWER_ON_FLAG==0x11)&&((SIM_Card_Work_Flag==0xAA)||(SIM_Card_ER_Flag==1))&&(UDP_Built_flag==0x11)
      &&(SIM_Card_Work_Cnt>MSP_A0_Min_3)&&(Heart_Beat_Count>SECD_10)&&(Heart_Beat_Count<SECD_20))
    {
       SIM_Card_Work_Cnt=0;
       SIM_Card_Work_Flag=0x55;
       
       Tran_ID_CmdNum(Send_SIM_Card_BUF,0x5A)                            ;//ת��ID�ź��������

       READ_SIM_NUM_FH();//�洢�ֻ����� 
       SIM_CHG_WARN()  ;//����ֻ�������
       
       if(GPS_GSM_System_Stu[2] & 0x30)
       {
           Send_SIM_Card_BUF[26]=0x22;
           for(SIM_CNT=0;SIM_CNT<20;SIM_CNT++) //��������ת��
           {
              Send_SIM_Card_BUF[27+SIM_CNT]=M72_SIM_NUM[SIM_CNT];
           }
       }
       
       if(!(GPS_GSM_System_Stu[2] & 0x30))
       {
           Send_SIM_Card_BUF[26]=0x11;
           for(SIM_CNT=0;SIM_CNT<20;SIM_CNT++)//�豸IDת��
           {
              Send_SIM_Card_BUF[27+SIM_CNT]=0x00;
           }
       }
       
       CAL_SIM_CRC=crc_modbus2((unsigned char *)Send_SIM_Card_BUF,
                                    (Send_SIM_Card_BUF_SIZE-2));//47
       Send_SIM_Card_BUF[47]    =   (char)(CAL_SIM_CRC&0xFF);
       Send_SIM_Card_BUF[48]    =   (char)((CAL_SIM_CRC>>8)&0xFF);
       
       SIM_Card_ER_Flag    =   1                                   ;
       if(GSM_SendData(Send_SIM_Card_BUF,Send_SIM_Card_BUF_SIZE))
        {                                                            //��������汾��
           SIM_Card_ER_Flag   =0                                   ;
        }
    }
    if(SIM_Card_ER_Flag)                                    
    {
       Delayms(200);
       SIM_Card_ER_Flag   =0                                    ;
       if(GSM_SendData(Send_SIM_Card_BUF,Send_SIM_Card_BUF_SIZE))
       {          
          SIM_Card_ER_Flag   =0                                    ;
       }
    }
}



char Change_SIM_Card_Flag;//�ϵ�����ֻ����ű�־
/*******************************************************************\
*	      ��������Change_SIM_Card_NUM             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��������ֻ�����  
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ��2013.12.13 ����
\*******************************************************************/
 void Change_SIM_Card_NUM(void)                            //��������ֻ�����
{
    unsigned char   Change_CNT;
    if(((ARM_DATA_STU&0x01)==0x01)&&(Change_SIM_Card_Flag==0xAA)&&((GSM_MCU[13]&0x03)==0x03))
    {
        GSM_MCU[13]&=~0x03;
        Change_SIM_Card_Flag=0x55;
        Flash_WriteChar(36,0x00)                                ;//д��־λ
        for(Change_CNT=0;Change_CNT<20;Change_CNT++)                         
        {            
           _NOP();_NOP();_NOP();_NOP();_NOP();
           Flash_WriteChar(37+Change_CNT,0x00); 
        }
        _NOP();_NOP();_NOP();_NOP();_NOP();
        READ_SIM_NUM_FH();
    }
}


unsigned int No_Card_Warning_Times=0;//�����޿���������
char* No_Card_Head_Flash=(char *)0x1846;//���б����׵�ַ001844 
char* No_Card_Warning_Times_Head_Flash=(char *)0x1858;//�����޿����������׵�ַ88��89
/*******************************************************************\
*	      ��������Write_No_Chg_Card_Flash             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  д���� �޿� Flash 
*	      ������  70 71
          001846~001847 ==70\71= �޿�==0xC0�� ����===0x30
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ��2014.1.11 ����
\*******************************************************************/

 void Write_No_Chg_Card_Flash(void)//д�����޿�Flash 
{
   unsigned int No_Card_Times=0;//�����޿���������
   
   
   if(((GPS_GSM_System_Stu[2]&0xC0)==0x00)&&(!(*No_Card_Head_Flash==0x00))&&((*(No_Card_Head_Flash+1)!=0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0));//ȡ������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x00)&&((*No_Card_Head_Flash!=0x00))&&((*(No_Card_Head_Flash+1)!=0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30));//11 ��������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30));
       _NOP();_NOP();_NOP();_NOP();_NOP();

   }

   
   if(((GPS_GSM_System_Stu[2]&0xC0)==0xC0)&&((*No_Card_Head_Flash!=0xC0))&&((*(No_Card_Head_Flash+1)!=0xC0)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0)|(*No_Card_Head_Flash));//ȡ������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       
       
       No_Card_Times=No_Card_Warning_Times++;
       Flash_WriteChar(89,(No_Card_Times));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(88,(No_Card_Times>>8));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x30)&&((*No_Card_Head_Flash!=0x30))&&((*(No_Card_Head_Flash+1)!=0x30)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30)|(*No_Card_Head_Flash));//11 ��������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       
       
       No_Card_Times=No_Card_Warning_Times++;
       Flash_WriteChar(89,(No_Card_Times));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(88,(No_Card_Times>>8));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
   
   Read_No_Chg_Card_Flash();
   
  /* if(((GPS_GSM_System_Stu[2]&0xC0)==0xC0)&&((*No_Card_Head_Flash!=0xC0))&&(!(*(No_Card_Head_Flash+1)==0xC0)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0));//ȡ������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x30)&&(!(*No_Card_Head_Flash==0x30))&&(!(*(No_Card_Head_Flash+1)==0x30)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30));//11 ��������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
   */
}


/* void Write_No_Chg_Card_Flash(void)//д�����޿�Flash 
{
  
   if(((GPS_GSM_System_Stu[2]&0xC0)==0x00)&&(!(*No_Card_Head_Flash==0x00))&&(!(*(No_Card_Head_Flash+1)==0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0)|(*No_Card_Head_Flash));//ȡ������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x00)&&(!(*No_Card_Head_Flash==0x00))&&(!(*(No_Card_Head_Flash+1)==0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30)|(*No_Card_Head_Flash));//11 ��������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
  
   if(((GPS_GSM_System_Stu[2]&0xC0)==0xC0)&&(!(*No_Card_Head_Flash==0xC0))&&(!(*(No_Card_Head_Flash+1)==0xC0)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0)|(*No_Card_Head_Flash));//ȡ������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x30)&&(!(*No_Card_Head_Flash==0x30))&&(!(*(No_Card_Head_Flash+1)==0x30)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30)|(*No_Card_Head_Flash));//11 ��������
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
   

   Read_No_Chg_Card_Flash();
}

*/
/*******************************************************************\
*	      ��������Read_No_Chg_Card_Flash             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  д���� �޿� Flash 
*	      ������  70 71
          001846~001847 ==70\71= �޿�==0xC0�� ����===0x30
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ��2014.1.11 ����
\*******************************************************************/
 void Read_No_Chg_Card_Flash(void)//�������޿�Flash 
{  
   if(((*No_Card_Head_Flash&0xC0)==0x00)&&((*(No_Card_Head_Flash+1)&0xC0)==0x00))
   {
        Module_Status[2] &=~ 0XC0;//ȡ������
        GPS_GSM_System_Stu[2] &=~ 0XC0; 
   }

   if(((*No_Card_Head_Flash&0x30)==0x00)&&((*(No_Card_Head_Flash+1)&0x30)==0x00))
   {
        Module_Status[2]&=~0x30;
        GPS_GSM_System_Stu[2]&=~0x30;//�޿�����    
   }
  
   if(((*No_Card_Head_Flash&0xC0)==0xC0)&&((*(No_Card_Head_Flash+1)&0xC0)==0xC0))
   {
        Module_Status[2] |= 0XC0;//ȡ������
        GPS_GSM_System_Stu[2] |= 0XC0; 
        GSM_Send_MCU_Buf[99]=* No_Card_Warning_Times_Head_Flash;
        GSM_Send_MCU_Buf[100]=*(No_Card_Warning_Times_Head_Flash+1);
   }

   if(((*No_Card_Head_Flash&0x30)==0x30)&&((*(No_Card_Head_Flash+1)&0x30)==0x30))
   {
        Module_Status[2] |=0x30;
        GPS_GSM_System_Stu[2]|=0x30;//GPS״̬�洢     
        GSM_Send_MCU_Buf[99]=* No_Card_Warning_Times_Head_Flash;
        GSM_Send_MCU_Buf[100]=*(No_Card_Warning_Times_Head_Flash+1);
   }
   
}
























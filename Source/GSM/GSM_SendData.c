
#include "msp430x54x.h"
#include "GSM.h"

char Module_Status[6]={0X00,0X00,0X00,0x03}                       ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
char GPS_GSM_System_Stu[4]                                        ;//GSM\GPS״̬
char GPS_GSM_ID_Memory[4]                                         ;//һ����豸ID�ն˺�  {0X38,0X1C,0X06,0X00}={0X02,0XB4,0XC4,0x04}  

unsigned long int MCU_TimeOut   =   MSP_A0_Min_30                 ;//���ݰ�����ʱ�� 1843000
unsigned long int MCU_Data_TimeOut=   MSP_A0_Min_1                ;//�������ݰ�����ʱ��      
unsigned long int MCU_Data_Count                                  ;//1ms ���ݰ�����
unsigned long int MCU_ACK_TimeOut                                 ;//1min ��ѯӦ��� ��Ӧ�����ط�
unsigned long int MCU_ACK_Count=0                                 ;//Ӧ�������

unsigned char Pow3_Send_Times                                     ;//���ʹ���

char LOW_POW_GSM_Flag =0x55                                       ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
char Send_MCU_ERR                                                 ;//����ʧ�ܱ�־ �ط�
char MCU_Data_ACK                                                 ;//���������ݱ�־λ
char GSM_Send_MCU_Buf[106]                                        ;//�ϱ�MCU��������(�ظ�)

/*******************************************************************\
*	      ��������GSM_Send_MCU             GPS_ANT_STATUS
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����MCU���� 
*	      ������  
ID(4)+������(2)+GPS��Ϣ(4+28)+MCU��־(1)+[MCU����(62)]+ModbusУ��(2)+[������(2)]
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
           1//GSM_MCU[MCU_CNT]=0;ȥ���˴� 2013.12.13 ��ֹ����0����
\*******************************************************************/
char GSM_Send_MCU(void)
{
    unsigned int tmp;
    unsigned char MCU_CNT;  
    static   unsigned int len;
 
    if((MCU_Data_Count>MCU_Data_TimeOut)&&(UDP_Built_flag==0x11)
       &&(Heart_Beat_Count>10200)&&(Heart_Beat_Count<30715))
    {
        
        MCU_Data_Count=0;
        
        if((MCU_Data_TimeOut<MSP_A0_Min_5)&&(P2IN&PWRON))
        {
            MCU_Data_TimeOut    =   MCU_TimeOut;//�������ݰ�����ʱ��
            MCU_ACK_TimeOut     =   MSP_A0_Min_1;
        }
        RD_MCU_TIME_FLASH();//��ȡʱ������FLASH     
  
        if(LPM3_Open_Box_Gsm_Flag==0xAA)
        {
            GPS_GSM_System_Stu[2]|=0x0C;
            if(LPM3_Open_Send_Time==Pow3_Send_Times++)
            {
                Pow3_Send_Times=0;
                LPM3_Open_Box_Gsm_Flag=0x55;
                GPS_GSM_System_Stu[2]&=~0x0C;
            }
        }
        
        for(MCU_CNT=0;MCU_CNT<4;MCU_CNT++)//GPS ״̬ת��  
        {
           GSM_Send_MCU_Buf[6+MCU_CNT]=GPS_GSM_System_Stu[MCU_CNT] ; 
        }
    
        /*ARM_DATA_STU &= ~0x04;//GPS������Ч  
        for(MCU_CNT=0;MCU_CNT<28;MCU_CNT++)//GPS����ת��
        {
           GSM_Send_MCU_Buf[6+4+MCU_CNT]=GPS_Inf[MCU_CNT];
        }*///ֱ�ӷ���GPS����
        
        
        if((ARM_DATA_STU&0x04)==0x04)
        {//GPS������Ч      
            ARM_DATA_STU &= ~0x04;
            for(MCU_CNT=0;MCU_CNT<28;MCU_CNT++)//GPS����ת��
            {
               GSM_Send_MCU_Buf[6+4+MCU_CNT]=GPS_Inf[MCU_CNT];
            }
        } 
        else
        {//û��GPS��λ����
            for(MCU_CNT=0;MCU_CNT<28;MCU_CNT++)                     
            {
               GSM_Send_MCU_Buf[6+4+MCU_CNT]=0x00                  ;
            } 
        }
        
        
        if(((ARM_DATA_STU&0x01)==0x01)&&(LPM==0x00))  
        {   //MCU ������Ч
            ARM_DATA_STU &= ~0x01;
            GSM_Send_MCU_Buf[38]=0x13; 
            for(MCU_CNT=0;MCU_CNT<62;MCU_CNT++)
            {
               GSM_Send_MCU_Buf[11+28+MCU_CNT]=GSM_MCU[MCU_CNT]; 
            }
            len = 105;             
            tmp=crc_modbus2((unsigned char *)GSM_Send_MCU_Buf,(len-2));
            GSM_Send_MCU_Buf[103]=(char)(tmp&0xFF);
            GSM_Send_MCU_Buf[104]=(char)((tmp>>8)&0xFF);
        }
        else
        {//MCU ������Ч 
            GSM_Send_MCU_Buf[38]=0x00;
            len = 43;            
            tmp=crc_modbus2((unsigned char *)GSM_Send_MCU_Buf,(len-2));
            GSM_Send_MCU_Buf[41]=(char)(tmp&0xFF);
            GSM_Send_MCU_Buf[42]=(char)((tmp>>8)&0xFF);
        }
        GSM_Send_MCU_Buf[105]=0;
        Send_MCU_ERR=1;
        MCU_Data_ACK=1;
        if(GSM_SendData(GSM_Send_MCU_Buf,len))
        {//���ݷ���
            MCU_ACK_Count  =  0;//������Ӧ��ʱ
            Send_MCU_ERR   =  0; 
			//LOW_POW_GSM_Flag=0x11;
            return 1;
        }
    }        
 
    if(MCU_Data_ACK && (MCU_ACK_Count>MCU_ACK_TimeOut)
       &&(Heart_Beat_Count<10199)&&(Heart_Beat_Count>7679)
       &&(UDP_Built_flag==0x11))                                   
    { //7.5--10S��ʱ����Ӧ�ط�
        Send_MCU_ERR=1;
        MCU_ACK_Count  = 0;
        if(GSM_SendData(GSM_Send_MCU_Buf,len))
        { 
            MCU_Data_Count = 0;
            Send_MCU_ERR   = 0;
            MCU_Data_ACK   = 0;
            //LOW_POW_GSM_Flag = 0x11;
            return 1;
        }
        MCU_Data_ACK=0;
    }
    if(Send_MCU_ERR&&(UDP_Built_flag==0x11)
       &&(Heart_Beat_Count<7678)&&(Heart_Beat_Count>5120))
    { //5--7.5Sʧ���ط� 
        if(GSM_SendData(GSM_Send_MCU_Buf,len))
        { 
            MCU_ACK_Count  =  0;
            Send_MCU_ERR   =  0;
            //LOW_POW_GSM_Flag=0x11;
            return 1;
        } 
        Send_MCU_ERR       =  0;
    }
    return 0;
}



unsigned long int One_JI_Warn_Cnt=0;//һ����������
unsigned long int Two_JI_Warn_Cnt=0;//������������


unsigned long int One_Degree_Alert_Time=0;//1����������
/**********************************************************************\
*	      ��������CHECK_MCU_WARN             
*	      �������ڲ��ļ�����
*	      ���ܣ�  ���MCU����  
*	      ������  GSM_MCU[1] ˮ��101��
          ��ʽ�� 
*	      ����ֵ��AA==�б�����00==�ޱ���   
*
*	      �޸���ʷ����ÿ��������MSP430_POWER_ON_FLAG=0x11;//
\**********************************************************************/
char CHECK_MCU_WARN(void)//���MCU���� 
{
    if((MSP430_POWER_ON_FLAG==0x11)//11=�ϵ磻55=�ϵ�
      &&(UDP_Built_flag==0x11)&&((ARM_DATA_STU&0x01)==0x01))
    {
        if((One_Degree_Alert_Time!=0)&&(GSM_MCU[10]==0x00)&&(GSM_MCU[11]==0x00)&&(GSM_MCU[12]==0x00)&&((GSM_MCU[13]&0x0F)==0x00)
           &&(One_JI_Warn_Cnt>MSP_A0_Min_1))
           One_Degree_Alert_Time=0;
      
        if((((GSM_MCU[1]>142)||((GSM_MCU[10]&0x03)==0x03))      //ˮ�¸߱���
            ||(((GSM_MCU[10]&0x0C)==0x0C))                      //����ѹ����
            //||(((GSM_MCU[11]&0x0C)==0x0C))                    //�����������¶ȸ߱���
              ||((GSM_MCU[12]&0x30)==0x30)||((GSM_MCU[12]&0xC0)==0xC0))//����ѹ��P1 P2  2014.02.12 �޸�
              //&&(UDP_B||((GSM_MCU[6]>195)||(GSM_MCU[7]>195))) //����ѹ��P1 P2uilt_flag==0x11)&&((ARM_DATA_STU&0x01)==0x01)                      
              &&(One_JI_Warn_Cnt>One_Degree_Alert_Time))                 //һ����������
        {
            One_JI_Warn_Cnt=0;
            
            if(One_Degree_Alert_Time>MSP_A0_Min_10)
              One_Degree_Alert_Time   =  0;
                One_Degree_Alert_Time   +=  MSP_A0_Min_1;
              
            MCU_Data_Count=MCU_Data_TimeOut+1;
            Heart_Beat_Count=10200+1; 
            if(GSM_Send_MCU())
              return 1;
            GSM_Send_MCU(); 
        }

          if((((GSM_MCU[11]&0x30)==0x30)            //��ȴҺҺλ�ͱ���
                  ||((GSM_MCU[11]&0xC0)==0xC0)      //���˶�������
                   ||((GSM_MCU[12]&0x03)==0x03)     //Һѹ�����˶�������
                    ||((GSM_MCU[12]&0x0C)==0x0C))
                    &&(Two_JI_Warn_Cnt>MSP_A0_Min_10)
             )  // ��ˮ���뱨��  
        {
            Two_JI_Warn_Cnt=0; //������������
            
            MCU_Data_Count=MCU_Data_TimeOut+1;
            Heart_Beat_Count=10200+1; 
            if(GSM_Send_MCU())
              return 1;
            GSM_Send_MCU();
        }

    }
    
    return 0;
}

unsigned long int ExitLMP_Time_Cnt=0;//�˳��͹���ʱ����
/**********************************************************************\
*	      ��������ExitLMP_SendMCU             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���˳��͹���ʱ������һ��MCU����  
*	      ������  1 P2IN&PWRON 
                  2 UDP_Built_flag=0x11
                  3 ExitLMP_Time_Cnt�͹��ļ�ʱ����3Min
          ��ʽ�� 
*	      ����ֵ��AA==�б�����00==�ޱ���   
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
void ExitLMP_SendMCU(void)
{
    if(((P2IN&PWRON)==0)&&(MSP430_POWER_ON_FLAG==0x55)&&(UDP_Built_flag==0x11)
       &&((ARM_DATA_STU&0x01)==0x01)&&(ExitLMP_Time_Cnt>MSP_A0_Min_5))//�͹��ķ���MCU����  
    {
        ExitLMP_Time_Cnt=0;
        MCU_Data_Count=MCU_Data_TimeOut+1;
        Heart_Beat_Count=10200+1; 
        
        GPS_Inf[14]=LPM;
        GSM_Send_MCU();
    }
}














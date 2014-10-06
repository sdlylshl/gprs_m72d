
#include "msp430x54x.h"
#include "GSM.h"

#define  GSM_GSP_BUF_SIZE 40
char  GSM_GSP_BUF[GSM_GSP_BUF_SIZE];//GSM���վ�γ��ʱ����Ϣ����
char * Get_GSM_GSP_AT = "AT+QGSMLOC";
unsigned long GSM_GPS_AT_Cmd_Cnt=0;//����GSM��ȡ������Ϣ����

//AT+QGSMLOC ��ȡ��γ�Ⱥ�ʱ�� 6s
//+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:47
//AT+QGSMLOC=1
//+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:55
//OK

/*******************************************************************\
*	      ��������Send_GSM_GPS_AT_Cmd             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ѯ��γ������ 
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  GPS_GSM_System_Stu[0]|=0x0C;//GPS���ź�
      Module_Status[0]|=0x0C;
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void Send_GSM_GPS_AT_Cmd(void)
{
    if((MSP430_POWER_ON_FLAG==0x11)&&(UDP_Built_flag==0x11)
	   &&((GPS_GSM_System_Stu[0]&GPS_Model_Bug)  //GPS��ģ����ϡ�
          ||(GPS_GSM_System_Stu[0]&GPS_No_Signal)//GPS���źţ�
          ||(GPS_GSM_System_Stu[0]&GPS_Ant_Open) //GPS�����߼���/δ�ӡ���
          ||((GPS_GSM_System_Stu[0]&GPS_Position)==0x00)//GPS��λ��־��
            )&&(GSM_GPS_AT_Cmd_Cnt>SECD_30))
    {
        GSM_GPS_AT_Cmd_Cnt=0;//����GSM��ȡ������Ϣ����
        GSM_SendCMD(PACKET_BRUST, Get_GSM_GSP_AT,0, 10);
    }
}

struct Gsmloc_Time
{
  unsigned char GSM_YEAR;
  unsigned char GSM_Moth;
  unsigned char GSM_dayy;
  unsigned char GSM_Hour;
  unsigned char GSM_Mint;
  unsigned char GSM_Secd;
}LOC;

/*******************************************************************\
*	      ��������Do_GPS_Data             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���ա�����GPS����  
*	      ������  
           1��ͨ������GPS�������ͣ�00=����Ӧ��11=GPRMC��22=GPGGA�����Ʋ���
           2��Do_GPS_Mesg_OK_Flag��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
           3��Open_UCA1_UART_Init(void)==�ý���������ݲ�����ʱ��GPS�ж�
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void Do_GSM_GPS_Data(void)//����GSM����GPS����
{

  
    //unsigned char GPS_COUNT;
    unsigned long int GSM_LOC_Weidu=0;//γ����ʱ���
    unsigned long int GSM_LOC_Jingdu=0;//������ʱ���
    
    unsigned  long int LOC_Wei_Miao=0;//γ�� ��
    unsigned  long int LOC_Jig_Miao=0;//���� �� 
    
    unsigned  int LOC_Wei_DU=0;//γ�� ��  
    unsigned  int LOC_Jig_DU=0;//���� �� 
    


    LOC_Jig_DU=ASCIITOHEX('0',GSM_GSP_BUF[0]);//����  7084417
    LOC_Jig_DU=LOC_Jig_DU*10+ASCIITOHEX('0',GSM_GSP_BUF[1]);  
    LOC_Jig_DU=LOC_Jig_DU*10+ASCIITOHEX('0',GSM_GSP_BUF[2]);  
    LOC_Jig_DU=LOC_Jig_DU*6;//*6}
    
    LOC_Jig_Miao=ASCIITOHEX('0',GSM_GSP_BUF[4]);
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[5]);  
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[6]); 
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[7]); 
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[8]); 
    LOC_Jig_Miao=LOC_Jig_Miao*6/10;
    
    GSM_LOC_Jingdu=LOC_Jig_DU*100000+LOC_Jig_Miao;
    
    
    LOC_Wei_DU=ASCIITOHEX('0',GSM_GSP_BUF[11]);//γ�� 204 955439
    LOC_Wei_DU=LOC_Wei_DU*10+ASCIITOHEX('0',GSM_GSP_BUF[12]); 
    LOC_Wei_DU=LOC_Wei_DU*6;
    
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[14]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[15]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[16]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[17]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[18]);  
    LOC_Wei_Miao=LOC_Wei_Miao*6/10;
    
    GSM_LOC_Weidu=LOC_Wei_DU*100000+LOC_Wei_Miao;

    
    
    if(GSM_GSP_BUF[21]=='2'&&GSM_GSP_BUF[22]=='0')
    {
        LOC.GSM_YEAR=ASCIITOHEX('0',GSM_GSP_BUF[23]);//�� 
        LOC.GSM_YEAR=LOC.GSM_YEAR*10+ASCIITOHEX('0',GSM_GSP_BUF[24]);  
        
        LOC.GSM_Moth=ASCIITOHEX('0',GSM_GSP_BUF[26]);//�� 
        LOC.GSM_Moth=LOC.GSM_Moth*10+ASCIITOHEX('0',GSM_GSP_BUF[27]);  
        
        LOC.GSM_dayy=ASCIITOHEX('0',GSM_GSP_BUF[29]);//�� 
        LOC.GSM_dayy=LOC.GSM_dayy*10+ASCIITOHEX('0',GSM_GSP_BUF[30]);  
        
        LOC.GSM_Hour=ASCIITOHEX('0',GSM_GSP_BUF[32]);//ʱ 
        LOC.GSM_Hour=LOC.GSM_Hour*10+ASCIITOHEX('0',GSM_GSP_BUF[33]);  
        
        LOC.GSM_Mint=ASCIITOHEX('0',GSM_GSP_BUF[35]);//�� 
        LOC.GSM_Mint=LOC.GSM_Mint*10+ASCIITOHEX('0',GSM_GSP_BUF[36]);  
        
        LOC.GSM_Secd=ASCIITOHEX('0',GSM_GSP_BUF[38]);//�� 
        LOC.GSM_Secd=LOC.GSM_Secd*10+ASCIITOHEX('0',GSM_GSP_BUF[39]);  
    }
    
    VmainMon();//����ƿ��﮵�ص�ѹ
    GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
    
    GPS_Inf[0]=LOC.GSM_YEAR/10*16+LOC.GSM_YEAR%10              ;//UTCʱ����
    GPS_Inf[1]=LOC.GSM_Moth/10*16+LOC.GSM_Moth%10              ;//UTCʱ����
    GPS_Inf[2]=LOC.GSM_dayy/10*16+LOC.GSM_dayy%10              ;//UTCʱ����  
    GPS_Inf[3]=LOC.GSM_Hour/10*16+LOC.GSM_Hour%10              ;//UTCʱ��Сʱ  
    GPS_Inf[4]=LOC.GSM_Mint/10*16+LOC.GSM_Mint%10              ;//UTCʱ�����  
    //LOC.GSM_Secd=LOC.GSM_Secd+10;
    GPS_Inf[5]=LOC.GSM_Secd/10*16+LOC.GSM_Secd%10              ;//UTCʱ����     
    
    
      GPS_Inf[9]=GSM_LOC_Weidu ;//γ�������ϴ���Ϣ
      GSM_LOC_Weidu= GSM_LOC_Weidu >>8;
      GPS_Inf[8]=GSM_LOC_Weidu;
      GSM_LOC_Weidu= GSM_LOC_Weidu >>8;  
      GPS_Inf[7]=GSM_LOC_Weidu;
      GSM_LOC_Weidu= GSM_LOC_Weidu >>8;
      GPS_Inf[6]=GSM_LOC_Weidu;
    
      GPS_Inf[13]=GSM_LOC_Jingdu;//���������ϴ���Ϣ
      GSM_LOC_Jingdu= GSM_LOC_Jingdu >>8;
      GPS_Inf[12]=GSM_LOC_Jingdu;
      GSM_LOC_Jingdu= GSM_LOC_Jingdu >>8;
      GPS_Inf[11]=GSM_LOC_Jingdu;
      GSM_LOC_Jingdu= GSM_LOC_Jingdu >>8; 
      GPS_Inf[10]=GSM_LOC_Jingdu;
      GPS_Inf[14]=LPM;
      GPS_Inf[15]=0   ;//��������

      GPS_Inf[17]=0;//���θ߶�
      GPS_Inf[16]=0;

      GPS_Inf[19]=0                                     ;//�ƶ��ٶ�
      GPS_Inf[18]=0                                     ;
  
      GPS_Inf[21]=0                             ;//�����
      GPS_Inf[20]=0                             ;
      
      
      GPS_Inf[22]=0x00 ;//Ԥ�� δ��
      GPS_Inf[23]=GSM_SIM_Iint_Sig_Num;//�ź�����ǿ��
      
      GPS_Inf[25]=0;
      GPS_Inf[24]=BAT_V_main;//��ص�ѹ
      
      GPS_Inf[27]=0;
      GPS_Inf[26]=LI_BAT_VOL;//﮵�ص�ѹ
      
  
     GPS_Data_OK_Flag =0x11;
     ARM_DATA_STU |=0x04   ;//GPS��Ϣ��λ
     Module_Status[0]=0xC0;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
     GPS_GSM_System_Stu[0]=0xC0;//GPS״̬�洢    
}
















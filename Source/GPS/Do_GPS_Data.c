

#include "GPS.h"

char GPS_Inf[28]                                                    ;//��ʾ��ƽ̨GPS���ݻ���

unsigned long int GPS_LOCATE_Time  = 0                              ;//GPS����ʱ��10S

char LOW_POW_GPS_Flag =0x55                                         ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
unsigned char GPS_Data_OK_Flag                                     ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
char *GPS_Buffer_RMC                                                ;//GPS_RX_Buffer���������׵�ַ
char *GPS_Buffer_GGA                                                ;//GPS_RX_Buffer���������׵�ַ
char *GPS_Head_STR                                                  ;//��GPSͷ�ļ�ָ��
char *GPGGA_Save_STR                                                ;//GPGGA����ָ��
char *UTC_Time                                                      ;//UTCʱ���ַ�ָ��  
char *GPS_GPRMC_Data_Use                                            ;//GPRMC������Ч��־λָ��
char *GPS_GPGGA_Data_Use                                            ;//GPGGA������Ч��־λָ��
char *GPS_Latitude                                                  ;//γ������ָ��
char *GPS_North_South                                               ;//γ�ȱ�γ��γ����
char *GPS_Longitude                                                 ;//��������ָ��
char *GPS_West_East                                                 ;//���ȱ�γ��γ����
char *GPS_Move_Speed                                                ;//�ƶ��ٶ����ݵ�λm/s
char *GPS_Course_Degree                                             ;//�����0-360
char *UTC_Time_YMD                                                  ;//UTCʱ�������� 
char *GPS_Magnetic_Degree                                           ;//�ش�ƫ��0-360
char *GPS_Magnetic_Degree_Dir                                       ;//�ش�ƫ�Ƿ���
char *Position_Output_Mode                                          ;//��λģʽ��� 
char *GPRMC_Check_Sum                                               ;//GPRMC����У���
char *GGA_Sates_Used                                                ;//��������
char *GGA_MSL_Altitude                                              ;//���θ߶�


unsigned long int GPS_J_Data=0                                      ;//���������ϴ���Ϣ
unsigned long int GPS_W_Data=0                                      ;//γ�������ϴ���Ϣ
int GPS_Speed_Data=0                                                ;//�����ٶ������ϴ���Ϣ
unsigned int GPS_Course_Degree_Data=0                               ;//�����0-360
unsigned char GPRMC_Check_Sum_Data=0                                ;//GPRMCУ���
unsigned char GGA_Sates_Used_Data=0                                 ;//��������
int  GGA_MSL_Altitude_Data=0                                        ;//���θ߶�
unsigned char UTC_Time_Year=0                                       ;//UTCʱ���� 
unsigned char UTC_Time_Moth=0                                       ;//UTCʱ����
unsigned char UTC_Time_dayy=0                                       ;//UTCʱ���� 
unsigned char UTC_Time_Hour                                         ;//UTCʱ��Сʱ 
unsigned char UTC_Time_Mint                                         ;//UTCʱ�����  
unsigned char UTC_Time_Secd                                         ;//UTCʱ���� 
extern char ARM_DATA_STU                                            ;
extern unsigned char LPM                                            ;//�͹���ģʽ
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
void Do_GPS_Data(void)                                            //���ա�����GPS����
{
  
  if(GPS_LOCATE_Time>MSP_A0_Min_10)
  {
      GPS_LOCATE_Time=0;
      GPS_GSM_System_Stu[0]|=0x0C;//GPS���ź�
      Module_Status[0]|=0x0C;
  }
  
  GPS_ANT_STATUS();//GPS���߼��
  Send_GSM_GPS_AT_Cmd();
  
  switch (Want_GPS_Data_Type) 
  {
     case 0x01:
      {
         if(GPS_LOCATE_Time>10240)                                   //GPS����ʱ��10S
         {   
             GPS_LOCATE_Time=0                                      ;
             GPS_UCA1_Init()                                        ;//��GPS���ճ�ʼ��
             Want_GPS_Data_Type=0x22                                ;
         }
         break;
      }
    
    case 0x11:
      {
        if(GPS_R_OK_End_Flag==0x11)                                  //GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
          {
            GPS_R_OK_End_Flag  = 0x00                               ;//GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
            Do_GPS_RMC_Data()                                       ;//����GPRMC���ݰ�UTCʱ�����ݴ���
          }
         break                                                      ;
      }
    case 0x22:
      {
         if(GPS_R_OK_End_Flag==0x22)                                 //GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
         {
            GPS_R_OK_End_Flag  = 0x00                               ;//GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
            Do_GPS_GGA_Data()                                       ;//����GPRMC���ݰ�UTCʱ�����ݴ���
         }       
         break;
      }
    case 0x33:
      {   
         GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
         if((GSM_SIM_Iint_Sig_Num<32))                               //(GSM_SIM_Iint_Sig_Num>0)&&
         {
            VmainMon();//�������Դ��ѹ��﮵�ص�ѹ   
            Want_GPS_Data_Type=0x11; 
         }
         Delayms(200);//XX*1Ms��ʱ
         break;
      }
    case 0x44:
    { 
         GSM_GPS_AT_Cmd_Cnt=0;//����GSM��ȡ������Ϣ����
         GPS_Data_OK_Flag =0x11                                     ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
         ARM_DATA_STU |=0x04                                        ;//GPS��Ϣ��λ
         Module_Status[0] =0xC0                                     ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
         GPS_GSM_System_Stu[0]=0xC0                                 ;//GPS״̬�洢        
         Want_GPS_Data_Type=0x01                                    ;
         LOW_POW_GPS_Flag =0x11                                     ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
         break                                                      ;
    }
    default:
        break;
  }    
}

/*******************************************************************\
*	      ��������Do_GPS_RMC_Data             
*	      �����򣺱����ļ�����
*	      ���ܣ�  ����GPRMC���ݰ�����
*	      ������ 
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
void Do_GPS_RMC_Data(void)                                         //����GPRMC���ݰ�UTCʱ�����ݴ���
{
    GPS_Buffer_RMC =GPS_RX_Buffer                                   ;//GPS_RX_Buffer���������׵�ַ
    GPS_Head_STR=GSM_strsep (&GPS_Buffer_RMC, ",")                  ;//1 ��ȡ�����ݰ�ͷ�ļ�ָ��
    UTC_Time=GSM_strsep (&GPS_Buffer_RMC, ",")                      ;//2 ��ȡUTCʱ������ָ��
    GPS_GPRMC_Data_Use=GSM_strsep (&GPS_Buffer_RMC, ",")            ;//3 GPRMC���ݰ���Ч��־λ

      if(*GPS_GPRMC_Data_Use=='A')
      {
         Want_GPS_Data_Type = 0x44                                  ;//��ҪGPS��������00==����Ӧ��
         GPS_Latitude=GSM_strsep (&GPS_Buffer_RMC, ",")             ;//γ������ָ��
         GPS_North_South=GSM_strsep (&GPS_Buffer_RMC, ",")          ;//γ�ȱ�γ��γ����
         GPS_Longitude=GSM_strsep (&GPS_Buffer_RMC, ",")            ;//��������ָ��
         GPS_West_East=GSM_strsep (&GPS_Buffer_RMC, ",")            ;//���ȱ�γ��γ����
         GPS_Move_Speed=GSM_strsep (&GPS_Buffer_RMC, ",")           ;//�ƶ��ٶ����ݵ�λm/s
         GPS_Course_Degree=GSM_strsep (&GPS_Buffer_RMC, ",")        ;//�����0-360
         UTC_Time_YMD=GSM_strsep (&GPS_Buffer_RMC, ",")             ;//UTCʱ�������� 
         GPS_Magnetic_Degree=GSM_strsep (&GPS_Buffer_RMC, ",")      ;//�ش�ƫ��0-360
         GPS_Magnetic_Degree_Dir=GSM_strsep (&GPS_Buffer_RMC, ",")  ;//�ش�ƫ�Ƿ���
         Position_Output_Mode=GSM_strsep (&GPS_Buffer_RMC, "*")     ;//��λģʽ���
         GPRMC_Check_Sum =GPS_Buffer_RMC                            ;//GPRMC����У���
         GPS_Post_RMC_Mesg()                                        ;//����GPRMC���ݰ���λ��Ϣ���ݴ���
      }
      else
      {
         Open_UCA0_UART_Init()                                      ;//��GPS�ж�
      }	
}

/*******************************************************************\
*	      ��������Do_GPS_GGA_Data             
*	      �����򣺱����ļ�����
*	      ���ܣ�  ����GPGGA���ݰ�����
*	      ������ 
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
void Do_GPS_GGA_Data(void)                                          //����GPGGA���ݰ����ݴ���
{
    unsigned int GGA_Numr=0                                         ;//GPGGA���ݰ����ݼ���
    GPS_Buffer_GGA  =  GPS_RX_Buffer                                ;//GPS_RX_Buffer���������׵�ַ
    for(GGA_Numr=0;GGA_Numr<6;GGA_Numr++)        
    {
       GPGGA_Save_STR=GSM_strsep (&GPS_Buffer_GGA, ",")             ;//GPGGA����ָ��
    } 
    GPS_GPGGA_Data_Use=GSM_strsep (&GPS_Buffer_GGA, ",")            ;//GPGGA������Ч��־λָ��
    if(*GPS_GPGGA_Data_Use=='1'||*GPS_GPGGA_Data_Use=='6')
      {
        GGA_Sates_Used=GSM_strsep (&GPS_Buffer_GGA, ",")            ;//��������
        GPGGA_Save_STR=GSM_strsep (&GPS_Buffer_GGA, ",")            ;//GPGGA����ָ��
        GGA_MSL_Altitude=GSM_strsep (&GPS_Buffer_GGA, ",")          ;//���θ߶�
        Want_GPS_Data_Type = 0x33                                   ;//ִ����һ��
        GPS_Post_GGA_Mesg()                                         ;//����GPGGA���ݰ���λ��Ϣ���ݴ���
        Open_UCA0_UART_Init()                                       ;//��GPS�ж�
      }
    else
    {
      Open_UCA0_UART_Init()                                         ;//��GPS�ж�
    }
}


/*******************************************************************\
*	      ��������GPS_Post_GGA_Mesg             
*	      �����򣺱����ļ�����
*	      ���ܣ�  
*	      ������ 
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
void GPS_Post_GGA_Mesg(void)                                       //����GPGGA���ݰ���λ��Ϣ���ݴ���
{
  unsigned int GGA_Num=0                                           ;//���ھ�γ�ȴ������
  int GPS_Position_OK_MSL_Altitude=0                               ;//���θ߶�
 
  GGA_Sates_Used_Data=ASCIITOHEX('0',*(GGA_Sates_Used++))          ;//��������
  GGA_Sates_Used_Data=GGA_Sates_Used_Data*10+ASCIITOHEX('0',*(GGA_Sates_Used++));//��������
  for(GGA_Num=0;GGA_Num<4;GGA_Num++)        
  {
     if(*(GGA_MSL_Altitude)!='.')
      GPS_Position_OK_MSL_Altitude=GPS_Position_OK_MSL_Altitude*10+ASCIITOHEX('0',*(GGA_MSL_Altitude++));//��������
      else {break;}  
  } 
  GGA_MSL_Altitude_Data=GPS_Position_OK_MSL_Altitude               ;//���θ߶�
}


/*******************************************************************\
*	      ����������GPS�ж�             
*	      �����򣺱����ļ�����
*	      ���ܣ�  ��GPS���ݴ���ƽ̨���ݻ��棬����ʾ����
*	      ������  ��
*	      ����ֵ����   
*
*	      �޸���ʷ����ÿ��������
\*****************************************************************/
void Open_UCA0_UART_Init(void)                                     //��GPS�ж�
{
    unsigned char Open_UCA1_Num=0                                 ;//���ھ�γ�ȴ������
    for(Open_UCA1_Num=0;Open_UCA1_Num<10;Open_UCA1_Num++)        
    {
      GPS_RX_Buffer[Open_UCA1_Num]   =  '@'                       ;//��������
    }
   GPS_RX_Buffer_Cnt=0                                            ;//GPS���ݻ������
   GPS_UCA1_Init()                                                ;//GPS�жϳ�ʼ��   
}
/*******************************************************************\
*	      ��������Do_GPS_RMC_Data             
*	      �����򣺱����ļ�����
*	      ���ܣ�  
*	      ������ 
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������

\*****************************************************************/
void GPS_Post_RMC_Mesg(void)                                      //����GPRMC���ݰ���λ��Ϣ���ݴ���
{
    unsigned int Nermber=0                                          ;//���ھ�γ�ȴ������
    unsigned long int GPS_Position_OK_gLat=0                        ;//γ�ȴ�����ʱ���
    unsigned long int GPS_Position_OK_gLong=0                       ;//���ȴ�����ʱ���
    unsigned int GPS_Position_OK_Speed=0                            ;//�����ٶȴ�����ʱ���
    unsigned int GPS_Position_OK_Degree=0                           ;//����Ǵ�����ʱ���
    
    UTC_Time_Hour =ASCIITOHEX('0',*(UTC_Time++))*10                 ;//UTCʱ��Сʱ 
    UTC_Time_Hour =UTC_Time_Hour+ASCIITOHEX('0',*(UTC_Time++))      ; 
    //UTC_Time_TO_Secd=UTC_Time_Hour*3600                             ;
    UTC_Time_Mint =ASCIITOHEX('0',*(UTC_Time++))*10                 ;//UTCʱ�����   
    UTC_Time_Mint =UTC_Time_Mint+ASCIITOHEX('0',*(UTC_Time++))      ;
    //UTC_Time_TO_Secd=UTC_Time_TO_Secd+UTC_Time_Mint*60              ;
    UTC_Time_Secd =ASCIITOHEX('0',*(UTC_Time++))*10                 ;//UTCʱ����   
    UTC_Time_Secd =UTC_Time_Secd+ASCIITOHEX('0',*(UTC_Time++))      ; 
    //UTC_Time_TO_Secd=UTC_Time_TO_Secd+UTC_Time_Secd                 ;
    
    UTC_Time_dayy =ASCIITOHEX('0',*(UTC_Time_YMD++))*10             ;//���ڶ�ȡ
    UTC_Time_dayy =UTC_Time_dayy+ASCIITOHEX('0',*(UTC_Time_YMD++))  ; 
    UTC_Time_Moth =ASCIITOHEX('0',*(UTC_Time_YMD++))*10             ;//�·ݶ�ȡ
    UTC_Time_Moth =UTC_Time_Moth+ASCIITOHEX('0',*(UTC_Time_YMD++))  ;  
    UTC_Time_Year =ASCIITOHEX('0',*(UTC_Time_YMD++))*10             ;//��ݶ�ȡ
    UTC_Time_Year =UTC_Time_Year+ASCIITOHEX('0',*(UTC_Time_YMD++))  ; 
    
    
    
    GPS_Position_OK_gLat=GPS_Position_OK_gLat*10+ASCIITOHEX('0',*(GPS_Latitude++));//γ��
    GPS_Position_OK_gLat=GPS_Position_OK_gLat*10+ASCIITOHEX('0',*(GPS_Latitude++));//γ��
    GPS_Position_OK_gLat=GPS_Position_OK_gLat*6                                   ;
    for(Nermber=0;Nermber<5;Nermber++)        
    {
     GPS_Position_OK_gLat=GPS_Position_OK_gLat*10+ASCIITOHEX('0',*(GPS_Latitude++));//γ��
     if(Nermber == 1){GPS_Latitude++;}
    } 
    //if(*GPS_North_South=='S')
    //{
    //     GPS_Position_OK_gLat=0-GPS_Position_OK_gLat;//�з���32λ���ͱ�����>0 ��ʾ��γ��0< ��ʾ��γ
    //}  
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//���� 
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//���� 
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//���� 
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*6                                    ;//���� 
    for(Nermber=0;Nermber<5;Nermber++)        
    {
     GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//���� 
     if(Nermber == 1){GPS_Longitude++;}
    } 
    //if(*GPS_West_East=='W')
    //{
    //    GPS_Position_OK_gLong=0-GPS_Position_OK_gLong                  ;//�з���32λ���ͱ�����>0 ��ʾ������0< ��ʾ����
    //}
    for(Nermber=0;Nermber<4;Nermber++)        
    {
     if(*(GPS_Move_Speed)!='.')
      GPS_Position_OK_Speed=GPS_Position_OK_Speed*10+ASCIITOHEX('0',*(GPS_Move_Speed++));//�����ٶ�
      else {break;}  
    } 
  
    for(Nermber=0;Nermber<4;Nermber++)        
    {
     if(*(GPS_Course_Degree)!='.')
      GPS_Position_OK_Degree=GPS_Position_OK_Degree*10+ASCIITOHEX('0',*(GPS_Course_Degree++));//�����0-360
      else {break;}  
    } 
    
    GPRMC_Check_Sum_Data=ASCIITOHEX('0',*(GPRMC_Check_Sum++))       ;//GPRMCУ���
    GPRMC_Check_Sum_Data=GPRMC_Check_Sum_Data*10+ASCIITOHEX('0',*(GPRMC_Check_Sum++)); 
    
    
    if(*GPS_North_South=='S')                       //2013.09.05 ������ �޸ľ�γ�ȴ���Ϊ��γ,�������λΪ1 8000
    {   
        GPS_W_Data=1;
        GPS_W_Data=GPS_W_Data<<31                   ;//�з���32λ���ͱ�����>0 ��ʾ��γ��0< ��ʾ��γ
    }
    else
    {
        GPS_W_Data=0;
    }
    
    if(*GPS_West_East=='W')
    {
        GPS_J_Data=1;
        GPS_J_Data=GPS_J_Data<<31                  ;//�з���32λ���ͱ�����>0 ��ʾ������0< ��ʾ����
    }
    else
    {
        GPS_J_Data=0;
    }

    GPS_W_Data=GPS_W_Data+GPS_Position_OK_gLat                      ;//γ�������ϴ���Ϣ
    GPS_J_Data=GPS_J_Data+GPS_Position_OK_gLong                     ;//���������ϴ���Ϣ
    GPS_Speed_Data=GPS_Position_OK_Speed                            ;//�����ٶ������ϴ���Ϣ
    GPS_Course_Degree_Data=GPS_Position_OK_Degree                   ;//�����0-360

    VmainMon()                                                      ;//�������Դ��ѹ��﮵�ص�ѹ
    JG_OUT_EDGE()                                                   ;//�ж�Խ�籨�� 
    Save_GPS_Inf_Buffer()                                           ;//��GPS���ݴ��뻺��
}


/*******************************************************************\
*	      ��������Save_Net_Disp_Buffer             
*	      �����򣺱����ļ�����
*	      ���ܣ�  ��GPS���ݴ���ƽ̨���ݻ��棬����ʾ����
*	      ������  ��
*	      ����ֵ����   
*
*	      �޸���ʷ��
                   1.������ �޸�GPS����ģʽ �͹��� ���� ȫ��ģʽ
\*****************************************************************/
void Save_GPS_Inf_Buffer(void)                                   //��GPS���ݴ��뻺��
{
  char UTC_TIME_BCD                                              ;
  
  UTC_TIME_BCD=UTC_Time_Year/10*16+UTC_Time_Year%10              ;
  GPS_Inf[0]=UTC_TIME_BCD                                        ;//UTCʱ���� 
  UTC_TIME_BCD=UTC_Time_Moth/10*16+UTC_Time_Moth%10              ;  
  GPS_Inf[1]=UTC_TIME_BCD                                        ;//UTCʱ����
  UTC_TIME_BCD=UTC_Time_dayy/10*16+UTC_Time_dayy%10              ;  
  GPS_Inf[2]=UTC_TIME_BCD                                        ;//UTCʱ���� 
  
  UTC_TIME_BCD=UTC_Time_Hour/10*16+UTC_Time_Hour%10              ;  
  GPS_Inf[3]=UTC_TIME_BCD                                        ;//UTCʱ��Сʱ
  UTC_TIME_BCD=UTC_Time_Mint/10*16+UTC_Time_Mint%10              ;  
  GPS_Inf[4]=UTC_TIME_BCD                                        ;//UTCʱ�����  
  UTC_TIME_BCD=UTC_Time_Secd/10*16+UTC_Time_Secd%10              ; 
  GPS_Inf[5]=UTC_TIME_BCD                                        ;//UTCʱ����

  GPS_Inf[9]=GPS_W_Data                                          ;//γ�������ϴ���Ϣ
  GPS_W_Data= GPS_W_Data >>8                                     ;
  GPS_Inf[8]=GPS_W_Data                                          ;
  GPS_W_Data= GPS_W_Data >>8                                     ;  
  GPS_Inf[7]=GPS_W_Data                                          ;
  GPS_W_Data= GPS_W_Data >>8                                     ;
  GPS_Inf[6]=GPS_W_Data                                          ;

  GPS_Inf[13]=GPS_J_Data                                         ;//���������ϴ���Ϣ
  GPS_J_Data= GPS_J_Data >>8                                     ;
  GPS_Inf[12]=GPS_J_Data                                         ;
  GPS_J_Data= GPS_J_Data >>8                                     ;
  GPS_Inf[11]=GPS_J_Data                                         ;
  GPS_J_Data= GPS_J_Data >>8                                     ; 
  GPS_Inf[10]=GPS_J_Data                                         ;

  if(GPS_Speed_Data>40)
      GPS_Inf[14]=0x01                                          ;//GPSģʽ 01==����ģʽ
  else
  {
	   GPS_Inf[14]=LPM;
  }
  
  
  GPS_Inf[15]=GGA_Sates_Used_Data                                ;//��������
  
  //GGA_MSL_Altitude_Data=GGA_MSL_Altitude_Data+5000               ;
  if(GGA_MSL_Altitude_Data>8000)
     GGA_MSL_Altitude_Data=0;
  GPS_Inf[17]=GGA_MSL_Altitude_Data                              ;//���θ߶�
  GGA_MSL_Altitude_Data= GGA_MSL_Altitude_Data >>8               ;    
  GPS_Inf[16]=GGA_MSL_Altitude_Data                              ; 
 
 
  GPS_Inf[19]=GPS_Speed_Data                                     ;//�ƶ��ٶ�
  GPS_Speed_Data= GPS_Speed_Data >>8                             ;  
  GPS_Inf[18]=GPS_Speed_Data                                     ;
  
  GPS_Inf[21]=GPS_Course_Degree_Data                             ;//�����
  GPS_Course_Degree_Data= GPS_Course_Degree_Data >>8             ;  
  GPS_Inf[20]=GPS_Course_Degree_Data                             ;
  
  GPS_Inf[22]=0x00                                               ;//Ԥ�� δ��
  GPS_Inf[23]=GSM_SIM_Signal_Num                                 ;//�ź�����ǿ��
  
  
  GPS_Inf[25]=0                                         ;
  GPS_Inf[24]=BAT_V_main                                         ;//��ص�ѹ
  
  GPS_Inf[27]=0                                         ;
  GPS_Inf[26]=LI_BAT_VOL                                         ;//﮵�ص�ѹ
}















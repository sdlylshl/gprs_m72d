#include "msp430x54x.h"
#include "GSM.h"

unsigned int  OVER_TIMES_CNT                                        ;//3.5�������ʱ�����
unsigned long int THR_Mint_Time_Cnt                                 ;//3.5����ʱ����������ڲ��ϴ����ݼ��
unsigned long int GSM_INIT_TIME_CNT                                 ;//GSM��ʼ����ʱ��
unsigned char GSM_ECHO=0x87                                         ;
unsigned char Thre_Num                                              ;//3�μ�¼
unsigned char GSM_SIM_Signal()                                               ;
unsigned char CONNECT_FAIL_Flag                                     ;//CONNECT FAIL����ʹ��

extern unsigned char ASCIITOHEX( unsigned char Input_H,
                                unsigned char Input_L );//��ASCII�ַ�ת��Ϊ16���Ƶ���
extern char StrSearch(char *a, char *b,
                      unsigned char aLength,
                      unsigned char bLength)           ;//�ַ���Ѱ��
char CONNECT_FAIL_RESET(void)                           ;//����Conect_Fail����
//****************************************************************************//

unsigned long int  TCPIP_BASIC_CNT                                  ;//M72ģ��TCPIP�������ü���
unsigned char TCPIP_BASIC_Step                                      ;//M72ģ��TCPIP��������
char * ATE_0_Disp = "ATE0"                                          ;//ATE<value>  �������ģʽ 0������
char * AT_NO_DIS_DATA ="AT+QISDE=0"                                 ;//0=AT+QISEND���ݲ����� 
char * AT_OK = "AT"                                                 ;
char * AT_FGCNT_0 = "AT+QIFGCNT=0"                                  ;//����ǰ�ó����ܿ���0ͨ��
char * AT_CSD_GPRS = "AT+QICSGP=1,\"IP\",\"CMNET\""                 ;//����CSD��GPRS����ģʽ
char * AT_SEND_OK = "AT+QIPROMPT=1"                                 ;//1==���ͳɹ�ʱ��ʾ��>���ҷ��ء�SEND OK��
char * AT_GPRS_CLASS_B = "AT+CGCLASS=\"B\""                         ;//GPRS�ƶ�̨���
char * AT_GPRS_ATTACH = "AT+CGATT=1"                                ;//GPRS���總��
char * GPRS_ATTACH_L  = "AT+CGATT=0"                                ;//GPRS���總��
char * Set_Bond_Rate = "AT&W"                                       ;//�̶�������
char *AT_CREG_1  =    "AT+CREG=1"                                   ;//Ѱ������ AT+CGREG
char *AT_CGREG_1  =   "AT+CGREG=1"                                  ;//Ѱ������ 

/*******************************************************************\
*	      ��������TCPIP_BASIC_SET             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  TCPIP��������
                  1 ����Ӧ������==115200      2  ����ǰ�ó����ܿ���0ͨ��
                  3 ����CSD��GPRS����ģʽ     4 �ر����ݻ���
                  5 ���ͳɹ�ʱ��ʾ��>���ҷ��ء�SEND OK��
                  6 GPRS�ƶ�̨���B           7 
                  8 GPRS���總��
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ��1 ������ 2013.12.04 ����̶����������� AT&W
\*******************************************************************/
char TCPIP_BASIC_SET()
{
    switch(TCPIP_BASIC_Step)
    {
        case 0:
          { 
              TCPIP_BASIC_Step=1;
              TCPIP_BASIC_CNT=0;
              No_SIM_CARD_Cnt=0;
              break;
          } 
           
        case 1:
          { 
              if(GSM_SendCMD(PACKET_CONFIG, AT_OK,0, 20) == 1)
              {
                  TCPIP_BASIC_Step      =   2                        ;
                  TCPIP_BASIC_CNT       =   0                        ;
                  Delayms(200);
              }
              else
              {
                  TCPIP_BASIC_CNT++;  
                  if(TCPIP_BASIC_CNT>20)
                  {
                    TCPIP_BASIC_CNT=0;
                    TCPIP_BASIC_Step=0;
                    OVER_TIMES_CNT=5;//����ϵͳ
                    CONNECT_FAIL_Flag=0x11;
                    CONNECT_FAIL_RESET();
                  }
                  Delayms(1000);
              }
              break                                                  ;
          }   //
          
        case 2:
           { 
              if(GSM_SendCMD(PACKET_CONFIG,ATE_0_Disp,0,20)==1)
              {
                  TCPIP_BASIC_Step  =   3                            ;
                  GSM_ECHO &= ~0x01                                  ;
              }
              Delayms(200)                                           ;
              break                                                  ;
          }   //�ر����ݻ���   
          
         case 3:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_GPRS_CLASS_B,0,20)==1)
              TCPIP_BASIC_Step      =   4                            ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //����CSD��GPRS����ģʽ     
          
         case 4:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_FGCNT_0,0,20)==1)
              TCPIP_BASIC_Step      =   5                            ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //����ǰ�ó����ܿ���0ͨ��
         case 5:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_SEND_OK,0,20)==1)
              {
                  TCPIP_BASIC_Step  =   6                            ;
                  GSM_ECHO &=~0x02                                   ;
              }
              Delayms(200)                                           ;
              break                                                  ;
          }   //���ͳɹ�ʱ��ʾ��>���ҷ��ء�SEND OK��            
          
         case 6:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,Set_Bond_Rate,0,20)==1)
                  TCPIP_BASIC_Step  =   7                            ;
              Delayms(200)                                           ;
              break                                                  ;                                              ;
          }   //GPRS�ƶ�̨���B
          
         case 7:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_CGREG_1,0,20)==1)
              TCPIP_BASIC_Step      =   8                            ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //���÷�͸��ģʽ0
          
         case 8:
          { 
              TCPIP_BASIC_Step  =   9                                ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //GPRS���總��
          
         case 9:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_CREG_1,0,20)==1)
              TCPIP_BASIC_Step      =   10                           ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //AT_CREG_1 
          
         case 10:
          { 
              //if(GSM_SendCMD(PACKET_CONFIG,AT_SAVE_SET,0,20)==1)
              TCPIP_BASIC_Step      =   11                           ;
              break                                                  ;
          }   //AT_SAVE_SET
          
         case 11:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_NO_DIS_DATA,0,20)==1)
                {
                    GSM_ECHO    &=  ~0x04                            ;
                    GSM_ECHO    &=  ~0x80                            ;//���óɹ�
                    TCPIP_BASIC_Step      =   0                      ;
                    return GSM_ECHO                                  ;
                }
              Delayms(200)                                            ;
              break                                                  ;
          }   //���ͳɹ�ʱ��ʾ��>���ҷ��ء�SEND OK��      
          
        default:
          {
              TCPIP_BASIC_Step      =   0                            ;
              break                                                  ;
          }
    }
    
  return 0;
}


//****************************************************************************//


char * SIM_AT_QCCID_Query = "AT+QCCID";

//AT+CIMI  ��������ƶ�̨�豸��ʶ��IMSI��
//AT+CIMI?
//+CIMI: "460023209180635"
//OK
//AT+CIMI
//460023209180635
//OK
char * SIM_AT_CIMI_Query = "AT+CIMI";



//1. �������
//2. SIM �����

// AT+CSNS0  ��������/����ģʽ
// 1  ����/���潻��ģʽ����������
// 2  ����
// 3  ����/���ݽ���ģʽ����������
// 4  ����
// 5  ����/���潻��ģʽ����������
// 6  ����/���ݽ���ģʽ����������
// 7  ���ݸ�������ģʽ

// AT+CCLK? ʵʱʱ��
//+CCLK: "13/06/19,02:34:02+00"

// AT+QNITZ=1  ����/���� GSM����ʱ��ͬ��



char SIM_CARD_TYPE_FLAG;
unsigned int SIM_CARD_TIME_OUT;//SIM����ʼ����ʱ��
unsigned long int No_SIM_CARD_Cnt;//��ⲻ��SIM����ʼ������
/*******************************************************************\
*	      ��������SIM_NUM_CHECK             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ѯSIM��CCID�� 
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������SIM_Check
\*******************************************************************/
char SIM_NUM_CHECK()
{
    static int SIM_CARD_NUM                                           ;
    if(GSM_INIT_TIME_CNT>SIM_CARD_TIME_OUT)
    {
        GSM_INIT_TIME_CNT=0;//GSM��ʼ����ʱ��            
        SIM_CARD_TIME_OUT=SECD_2;//SIM����ʼ����ʱ��
        SIM_CARD_TYPE_FLAG=GSM_SendCMD(PACKET_Query, SIM_AT_QCCID_Query,0, 10);
        if (SIM_CARD_TYPE_FLAG==1) 
        {
            if (GSM_ECHO & 0x01) 
            {//�����Թ���ָ���+3�ֽ�(0x0D 0x0D 0x0A)+����  0x01
                SIM_CARD_NUM=GSM_strlen(SIM_AT_QCCID_Query)           ;
                if(SAVE_SIM_NUM_FH(SIM_CARD_NUM+3))
                {
                    SIM_CHG_WARN();//�����ֻ������� 
                    Module_Status[2] &= ~0XC0;
                    GPS_GSM_System_Stu[2] &= ~ 0XC0; 
                    Write_No_Chg_Card_Flash();//д�����޿�Flash 
                    No_SIM_CARD_Cnt=0;//��⵽SIM��
                    return 1;
                }
                
            } else 
            {//2�ֽ�(0x0D 0x0A)+����
                if(SAVE_SIM_NUM_FH(2))//�洢�ֻ�����
                {
                    SIM_CHG_WARN(); 
                    Module_Status[2] &= ~0XC0;
                    GPS_GSM_System_Stu[2] &= ~ 0XC0;
                    Write_No_Chg_Card_Flash();//д�����޿�Flash 
                    No_SIM_CARD_Cnt=0;//��⵽SIM��
                    return 1;
                }
            }
        }
        else
        {
            if((No_SIM_CARD_Cnt++>20)&&(SIM_CARD_TYPE_FLAG==0xA0) )
            {
                No_SIM_CARD_Cnt=0;
                OVER_TIMES_CNT=5;
                CONNECT_FAIL_Flag=0x11;
                CONNECT_FAIL_RESET();
                Module_Status[2] |= 0XC0;//ȡ������
                GPS_GSM_System_Stu[2] |= 0XC0; 
                Write_No_Chg_Card_Flash();//д�����޿�Flash 
            }
            
            SIM_CARD_TIME_OUT=SECD_5;//SIM����ʼ����ʱ��
        }
    }
    return 0                                                          ;
}

//AT+CREG ����ע����Ϣ<stat>  0   δע�᣻ME��ǰû��������Ҫע��ҵ�����Ӫ����
// 1   ��ע�ᣬ������
// 2   δע�ᣬ�� ME ��������Ҫע��ҵ�����Ӫ����
// 3   ע�ᱻ�ܾ�
// 4   δ֪
// 5   ��ע�ᣬ����
//AT+CREG?
//+CREG: 0,1
//OK
char * GSM_AT_CREG_Query = "AT+CREG?";
/*******************************************************************\
*	      ��������GSM_CREG_Init             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ѯSIM��CCID�� 
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������SIM_Check
\*******************************************************************/
unsigned long int GSM_CREG_TIME_OUT =500;//SIM����ʼ����ʱ��
unsigned int CREG_Zhu_Ce_Cnt ;//ע�������ѯע�����
char GSM_CREG_Init()
{
    if(GSM_INIT_TIME_CNT>GSM_CREG_TIME_OUT)
    {
        GSM_INIT_TIME_CNT=0;//GSM��ʼ����ʱ��
        GSM_CREG_TIME_OUT=500;
        GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
        if((GSM_SIM_Iint_Sig_Num>5)&&(GSM_SIM_Iint_Sig_Num<32))
        {
            GSM_SIM_Iint_Sig_Num=0;  
            if (GSM_SendCMD(PACKET_Query, GSM_AT_CREG_Query,0, 10) == 1) 
            {
                if (GSM_ECHO & 0x01) 
                {//ָ���+3�ֽ�(0x0D 0x0D 0x0A)+����
                    if ((M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10 - 1] == '1')||
                        (M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10 - 1] == '5')||
                        (CREG_Zhu_Ce_Cnt++ >20)||
                        ((M72D_Query_RX_Buf[0]==0x0D&&M72D_Query_RX_Buf[1]==0x0A)&&
                        (M72D_Query_RX_Buf[2+10 - 1] == '1'||M72D_Query_RX_Buf[2+10 - 1] == '5'))
                        )
                    {
                      
                        Module_Status[1]  &= ~ 0xC0;//GPRS�ź�ä��
                        GPS_GSM_System_Stu[1] &= ~0xC0;
                        Delayms(200);//XX*1Ms��ʱ
                        if(GSM_SendCMD(PACKET_CONFIG,AT_GPRS_ATTACH,0,20)==1)
                        {
                            CREG_Zhu_Ce_Cnt=0;
                            Delayms(200);
                            if(GSM_SendCMD(PACKET_CONFIG,AT_CSD_GPRS,0,20)==1)
                            return 1;
                        }
                    }
        
                } else {
                    //2�ֽ�(0x0D 0x0A)+����
                    if (((M72D_Query_RX_Buf[0]==0x0D&&M72D_Query_RX_Buf[1]==0x0A)&&(M72D_Query_RX_Buf[2+10 - 1] == '1'||M72D_Query_RX_Buf[2+10 - 1] == '5'))
                        ||(CREG_Zhu_Ce_Cnt++ >20))
                    {
                        Module_Status[1]  &= ~ 0xC0;//GPRS�ź�ä��
                        GPS_GSM_System_Stu[1] &= ~0xC0;
                        Delayms(200);//XX*1Ms��ʱ
                        if(GSM_SendCMD(PACKET_CONFIG,AT_GPRS_ATTACH,0,20)==1)
                        {
                            CREG_Zhu_Ce_Cnt=0;
                            Delayms(200);
                            //CREG_Zhu_Ce_Cnt=0;
                            if(GSM_SendCMD(PACKET_CONFIG,AT_CSD_GPRS,0,20)==1)
                            return 1;
                        }
                        else
                        {
                            Delayms(200);
                            GSM_SendCMD(PACKET_CONFIG,GPRS_ATTACH_L,0,20);
                        }
                        
                    }
                }
                if(CREG_Zhu_Ce_Cnt++>20)
                {
                     CREG_Zhu_Ce_Cnt=0;
                     OVER_TIMES_CNT=5;
                     CONNECT_FAIL_Flag=0x11;
                     CONNECT_FAIL_RESET(); 
                     Module_Status[1]  |= 0xC0;//GPRS�ź�ä��
                     GPS_GSM_System_Stu[1]|=0xC0;  
                }
                
                GSM_CREG_TIME_OUT=SECD_5;//SIM����ʼ����ʱ��
            }
        }
    }
    return 0;
}


/*******************************************************************\
*	      ��������GSM_GPRS_Init             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ��ѯSIM��CCID�� 
*	      ������  
          ��ʽ�� 
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������SIM_Check
\*******************************************************************/
char * GSM_AT_CGREG = "AT+CGREG?";
char GSM_SIM_Iint_Sig_Num;
char GSM_GPRS_Init()
{
    if(GSM_INIT_TIME_CNT>GSM_CREG_TIME_OUT)
    {
        GSM_INIT_TIME_CNT=0 ;//GSM��ʼ����ʱ��
        GSM_CREG_TIME_OUT=500;//SIM����ʼ����ʱ��
        GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
        //Delayms(200);//XX*1Ms��ʱ
        if((GSM_SIM_Iint_Sig_Num>5)&&(GSM_SIM_Iint_Sig_Num<32))
        {
            GSM_SIM_Iint_Sig_Num=0;  
            if (GSM_SendCMD(PACKET_Query, GSM_AT_CGREG,0, 10) == 1) 
            {   
                Delayms(200);//XX*1Ms��ʱ
                if (GSM_ECHO & 0x01) 
                {//ָ���+3�ֽ�(0x0D 0x0D 0x0A)+����
                    if (M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10] == '1'||
                        M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10] == '5')
                    {
                        CREG_Zhu_Ce_Cnt=0;
                        return 1;
                    }
        
                } else 
                {//2�ֽ�(0x0D 0x0A)+����
                    if (M72D_Query_RX_Buf[2+10 ] == '1' || M72D_Query_RX_Buf[2+10] == '5')
                    {
                        CREG_Zhu_Ce_Cnt=0;
                        return 1;
                    }
                    if((M72D_Query_RX_Buf[0]==0x0D)&&(M72D_Query_RX_Buf[1]==0x0A)
                       &&(M72D_Query_RX_Buf[2]==0x0D)&&(M72D_Query_RX_Buf[3]==0x0A))
                    {
                        if (M72D_Query_RX_Buf[2+10+2] == '1'|| M72D_Query_RX_Buf[2+10+2] == '5')
                        {
                            CREG_Zhu_Ce_Cnt=0;
                            return 1;
                        }
                    }
                }
                
            }
        }
        
      CREG_Zhu_Ce_Cnt++;
      if(CREG_Zhu_Ce_Cnt>20)
        {
            CREG_Zhu_Ce_Cnt=0;
            GSM_SendCMD(PACKET_Query,GSM_AT_CGREG,0,10);
            return 1;
        }
        
        
        GSM_CREG_TIME_OUT=SECD_5;//SIM����ʼ����ʱ��
    }
    return 0;
}

// AT+QIFGCNT  ����ǰ�ó���


// AT+QNTP  ͨ��TCP/IP����ʱ�������ͬ������ʱ��
char * TCPIP_AT_QNTP = "AT+QNTP";

//AT+QIOPEN="UDP","58.57.53.58","1090" ɽ�ؽ������޹�˾GPS����ƽ̨�˿�
//char * TCPIP_AT_QIOPEN = "AT+QIOPEN=\"UDP\",\"58.57.53.58\",\"2013\"";


char UDP_Built_STR[41]={'A','T','+','Q','I','O','P','E','N','=',                     //  AT+QIOPEN=
                        '"','U','D','P','"',',',                                     //"UDP",
                        '"','0','5','8','.','0','5','7','.','0','5','3','.','0','5','8','"',',',     //"58.57.53.58",
                        '"','2','0','1','3','"'};                                    //"1090"    '"','1','0','9','0','"',0X0D }  '"','2','0','1','3','"',0X0D };  
//AT+QIOPEN="UDP","58.57.53.58","2013" ɽ�ؽ������޹�˾GPS����ƽ̨�˿�

char GSM_TCPIP_Init()
{
/*      �������󷵻صķ��������� 
һ��  "ERROR"�����ﷵ��ERROR����������ԭ�� 
1�� �����ʽ���ԣ�����������ݸ�ʽ���ԣ���Ҫ����QIMUX�ǲ���Ϊ0��ͨ������
"AT+QIMUX?"��ѯ�������ѯ���Ϊ1������Ҫͨ������AT+QIMUX=0��������
��Ϊ0���� 
2�� ��ǰ��TCPIP�����״̬����IP INITIAL��IP STATUS��IP CLOSE��ͨ������
AT+QISTAT��ѯ���������ǰ״̬ΪTCP CONNECTING������Ҫִ��
AT+QICLOSE�رյ�ǰʧ�ܵ�TCP���ӡ����������״̬������Ҫִ������
AT+QIDEACT�Ͽ���ǰʧ�ܵ�GPRS������ 
����  "ALREADY CONNECT"��������Ѿ�����һ��TCP���ӻ�UDP���ӡ����ȷ����Ҫ��
���µ����ӣ�����Ҫ����AT+QICLOSE�رյ�ǰ���ӡ� 
  
  AT+QIOPEN="TCP","116.226.39.202","7007"
OK
CONNECT FAIL
  
AT+QIOPEN="UDP","58.57.53.58","1090" 
OK
CONNECT OK
  
AT+QIOPEN="UDP","58.57.53.58","1090" 
ALREADY CONNECT
ERROR
  
  */
    char stat;
    if(GSM_INIT_TIME_CNT>GSM_CREG_TIME_OUT)
    {
        GSM_INIT_TIME_CNT=0;//GSM��ʼ����ʱ��
        GSM_CREG_TIME_OUT=SECD_5;//SIM����ʼ����ʱ��
                
        if(GSM_SendCMD(PACKET_Query,"AT+QINDI=1", 0,10)==1)                            ;//��TCPIP����
        stat=GSM_SendCMD(PACKET_NETCONFIG,UDP_Built_STR, 0,10)                         ;//OK TCPIP_AT_QIOPEN
        switch (stat) 
        {
        case 1:
          {// CONNECT OK
              Thre_Num=0;
              return 1;
          }
        case 2:
          {// CONNECT FAIL
               Thre_Num++;
               if(Thre_Num>3)
               {   
                   Thre_Num   =0;
                   CONNECT_FAIL_Flag =0x11;//CONNECT FAIL����ʹ��
                   CONNECT_FAIL_RESET();//����Conect_Fail����
               }
               return 0;
          }
        case 3:
          {//ALREADY CONNECT
               Thre_Num=0;
               return 1;
          }
        case 0xA0:
          {//ERROR
               if(GSM_SendCMD(PACKET_CONFIG,"AT+QIMUX]=0",0, 10));
               //CONNECT_FAIL_Flag      =0x11                                          ;//CONNECT FAIL����ʹ��
               //CONNECT_FAIL_RESET()                                                  ;//����Conect_Fail����
               return 0 ;
          }
        default:
          {
               //Delayms(60000)                                                        ;//XX*1Ms��ʱ
               //CONNECT_FAIL_Flag      =0x11                                          ;//CONNECT FAIL����ʹ��
               //CONNECT_FAIL_RESET()                                                  ;//����Conect_Fail����
               return 0;
          }   
        }
        
    }
    return 0;
}



/********************************************************\
*	��������GSM_SIM_Signal
   1    �������ⲿ�ļ�����
*	���ܣ�  ��ʼ����ʱ��
*	������  
             AT+CSQ?   �ź�����
             +CSQ:28,0
             OK
*	����ֵ: ��
*	�޸���ʷ����ÿ��������
\********************************************************/
unsigned char GSM_SIM_Signal_Num                                ;//�ź�����ǿ��
char * GSM_SIM_Signal_AT = "AT+CSQ"                             ;
char * CSQ_String = "+CSQ:"                                     ;
char   SIM_Signal                                               ;//GPS_RX_Buffer���������׵�ַ
unsigned char GSM_SIM_Signal()
{    
    //Delayms(1000);
    char SIM_Num                                                ;//�ڲ�������
    GSM_SIM_Signal_Num=0                                        ;
    if (GSM_SendCMD(PACKET_Query, GSM_SIM_Signal_AT,0, 10) == 1) //����OK
    {
        //���� +CSQ:28,0 //�����Թ��� //0x0D 0x0A+����(6)
        
        SIM_Signal=StrSearch(M72D_Query_RX_Buf, CSQ_String,
                      M72D_Query_RX_BUF_SIZE,4)                 ;//�ַ���Ѱ��
        for(SIM_Num=0;SIM_Num<3;SIM_Num++)        
        {
             if((M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]!=',')
                &&((M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]>0x2F)&&(M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]<0x3A)))
             {
                GSM_SIM_Signal_Num=GSM_SIM_Signal_Num*10+
                                   ASCIITOHEX('0',M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]);//�������� 
             }
             else {break;}  
        } 
        return GSM_SIM_Signal_Num                                ;
    }
    return 0;
}
 

/*******************************************************************\
*	      ��������Conect_Fail_Reset             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����Conect_Fail����  
*	      ������  
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/           
char CONNECT_FAIL_RESET(void)                                         //����Conect_Fail����
{
    if((THR_Mint_Time_Cnt>MSP_A0_Min_3)||(CONNECT_FAIL_Flag==0x11)) 
    {
        CONNECT_FAIL_Flag   =0x00                                   ;//��־λ����
        THR_Mint_Time_Cnt   =0                                      ;//3.5���Ӽ�������
        GSM_STATUS          = 0x3D                                  ;
        GSM_ECHO            =0x87                                   ;
        UDP_Built_STR[40]  =0                                       ;
        
        Module_Status[1] |=  0x30                                   ;//GPRS�����쳣
        GPS_GSM_System_Stu[1]|=0x30                                 ;
        CREG_Zhu_Ce_Cnt =0;//ע�������ѯע�����
        
        if(UDP_Built_flag==0x11)
        {
            UDP_Built_flag      =0x00                                   ;//00==����Ͽ�����û�н���
            GSM_SendCMD(PACKET_Query,"AT+QICLOSE",0, 1000)              ;   
            Delayms(200)                                                ;
            GSM_SendCMD(PACKET_Query,"AT+QIDEACT",0, 1000)              ;
            Delayms(200)                                                ;
            GSM_SendCMD(PACKET_CONFIG,GPRS_ATTACH_L,0,20)               ;
        }
        
        OVER_TIMES_CNT  ++                                          ;  
        if(OVER_TIMES_CNT>3)
        {
            RE_START_GSM()                                          ;//����ϵͳ
            OVER_TIMES_CNT  =0                                      ;
        }
        
        No_SIM_CARD_Cnt=0;
        Heart_Beat_Count    =0                                      ;//��������1msʱ�����
        if(MCU_Data_Count>(MCU_TimeOut>>2))
            MCU_Data_Count =   MCU_TimeOut >>  2                    ;//1ms ���ݰ�����
        return 1                                                    ; 
    }
    return 0                                                        ;
}
           
           
           
           
           
           
           
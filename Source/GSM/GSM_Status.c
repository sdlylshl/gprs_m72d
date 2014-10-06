
#include "msp430x54x.h"
#include "GSM.h"

//GSM 状态
#define TCPIP_ERRO 0x20
#define GPRS_ERRO 0x10
#define GSM_ERRO 0x08
#define SIM_ERRO 0x04
#define SLEEP_ERRO 0x02
#define POWER_ERRO 0x01

//0 正常 0x01关机 0x02休眠 0x04 SIM卡 0x08 GSM网络正常 0x10GPRS网络正常 0x20 TCPIP正常
unsigned char GSM_STATUS=0x3D                                     ;
unsigned char UDP_Built_flag                                      ;//11==网络建立，00==网络断开或者没有建立
unsigned long int GSM_Start_Count=0                               ;//开机延时

char MSP430_POWER_ON_FLAG;//11=上电；55=断电
#define RING BIT0
unsigned char POWER_ON_FLAG                                        ;//11=关机；55=开机
/*******************************************************************\
*	      函数名：GSM_NOM_POW_ON          
*	      作用域：外部文件调用
*	      功能：  硬件开机 
*	      参数：  V2
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）GSM_NORMAL_POWER_ON
\*******************************************************************/
void GSM_NOM_POW_ON(void)
{
    P1DIR   &= ~BIT0                                                 ;//P1.0 inputs
    P4DIR   &= ~BIT5                                                 ;//P4.5 inputs stu

    P4DIR   |=  BIT0                                                 ;//P4.1 outputs
    P4OUT   &=  ~  BIT0                                                 ;//GSMPWR L
    Delayms(10)                                                      ;//XX*1Ms延时
    
    //on  4.2
    P4DIR   |=  BIT2                                                 ;//P4.1 outputs
    P4OUT   &= ~BIT2                                                 ;//GSMPWR L
    Delayms(200)                                                     ;//XX*1Ms延时
    P4OUT   |=  BIT2                                                 ;//GSMPWR H
    Delayms(200)                                                     ;//XX*1Ms延时
    GSM_Start_Count=0;//开机延时
    while((!(P4IN&BIT5))&&(GSM_Start_Count<SECD_10));
    Delayms(50)                                                      ;//XX*1Ms延时
    P4OUT   &= ~BIT2                                                 ;//  GSMPWR L     P4.1 reset
    
    GSM_STATUS = 0x3C                                                ;
    GSM_ECHO   = 0x87                                                ;
    POWER_ON_FLAG   =   0x55                                         ;//55=开机
    SIM_CARD_TIME_OUT=500                                            ;//SIM卡初始化计时用
    Delayms(2000)                                                    ;//XX*1Ms延时
}


/*******************************************************************\
*	      函数名：GSM_NOM_POW_ON          
*	      作用域：外部文件调用
*	      功能：  硬件开机 
*	      参数：  V1
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）GSM_NORMAL_POWER_ON
\*******************************************************************/
/*
void GSM_NOM_POW_ON(void)
{
    P1DIR   &= ~BIT0                                                 ;//P1.0 inputs
    P4DIR   &= ~BIT5                                                 ;//P4.5 inputs stu

    P4DIR   |=  BIT0                                                 ;//P4.1 outputs
    P4OUT   |=  BIT0                                                 ;//GSMPWR L
    Delayms(10)                                                      ;//XX*1Ms延时
    
    //on  4.2
    P4DIR   |=  BIT2                                                 ;//P4.1 outputs
    P4OUT   &= ~BIT2                                                 ;//GSMPWR L
    Delayms(200)                                                     ;//XX*1Ms延时
    P4OUT   |=  BIT2                                                 ;//GSMPWR H
    Delayms(200)                                                     ;//XX*1Ms延时
    GSM_Start_Count=0;//开机延时
    while((!(P4IN&BIT5))&&(GSM_Start_Count<SECD_30));
    Delayms(50)                                                      ;//XX*1Ms延时
    P4OUT   &= ~BIT2                                                 ;//  GSMPWR L     P4.1 reset
    
    GSM_STATUS = 0x3C                                                ;
    GSM_ECHO   = 0x87                                                ;
    SIM_CARD_TIME_OUT=500                                            ;//SIM卡初始化计时用
}

*/


char * POWER_DOWN = "AT+QPOWD=1"                                     ;//关机字符串
/*******************************************************************\
*	      函数名：NORMAL_POWER_DOWN             
*	      作用域：外部文件调用
*	      功能：  软件关机 
*	      参数：  延时15S
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）
\*******************************************************************/
void GSM_NOM_POW_OFF(void)
{
  
    if(UDP_Built_flag==0x11)
    {
        UDP_Built_flag      =0x00                                   ;//00==网络断开或者没有建立
        GSM_SendCMD(PACKET_Query,"AT+QICLOSE",0, 1000)              ;   
        Delayms(200)                                                ;
        GSM_SendCMD(PACKET_Query,"AT+QIDEACT",0, 1000)              ;
    }
    
    if(GSM_SendCMD(PACKET_CONFIG,POWER_DOWN, 0,100) == 1) 
        POWER_ON_FLAG   =   0x11                                     ;//11=关机
    Delayms(15000)                                                    ;//XX*1Ms延时
    P4DIR   |=  BIT0                                                 ;//P4.1 outputs
    P4OUT   &= ~BIT0                                                 ;//GSMPWR L
    
}
/*******************************************************************\
*	      函数名：RE_START_GSM             
*	      作用域：外部文件调用
*	      功能：  重启系统 
*	      参数：  
          格式：  
*	      返回值：  
*
*	      修改历史：（每条详述）
\*******************************************************************/
void RE_START_GSM(void)                                              //重启系统 
{
    GSM_NOM_POW_OFF()                                                ;
    Delayms(15000)                                                    ;//XX*1Ms延时
    GSM_NOM_POW_ON()                                                 ;
}





void GSM_Sleep()
{

}


//GSM 工作状态检测
void GSM_Power_state()
{
if(P4IN&BIT5)
    GSM_STATUS &=~POWER_ERRO;
  else
    GSM_STATUS =0x3C;
}

unsigned long int GSM_STAT_TIME_CNT=60000            ;//检测手机卡 5分钟检测一次
char * SIM_AT_CPIN = "AT+CPIN?"                       ;//AT+CPIN  输入 PIN
void GSM_SIM_state()
{
    static unsigned char SIM_CPIN                                       ;
    SIM_CPIN   =GSM_SendCMD(PACKET_Query, SIM_AT_CPIN,0, 10)            ;
    if (SIM_CPIN==0xAA) 
    {
        Module_Status[2] &= ~0xC0;
        Write_No_Chg_Card_Flash();//写换卡无卡Flash 
    }
    else
    {
        if(SIM_CPIN==0xA0) 
        {
            Module_Status[2] |= 0xC0;
            Write_No_Chg_Card_Flash();//写换卡无卡Flash 
        }
    }
}
void GSM_GSM_state()
{
  
  
}


char * GSM_GPRS_AT_State = "AT+QISTAT"                       ;//AT+CPIN  输入 PIN
void GSM_GPRS_state()
{
    if(GSM_SendCMD(PACKET_Query, GSM_GPRS_AT_State,0, 10))
    {
        if(((M72D_Brust_RX_Buf[0]==0x0D)&&(M72D_Brust_RX_Buf[1]==0x0A)&&(M72D_Brust_RX_Buf[2]==0x0D)&&(M72D_Brust_RX_Buf[3]==0x0A))
           &&((M72D_Brust_RX_Buf[4]=='S')&&(M72D_Brust_RX_Buf[5]=='T')&&(M72D_Brust_RX_Buf[6]=='A')&&(M72D_Brust_RX_Buf[7]=='T')&&(M72D_Brust_RX_Buf[8]=='E')))
        {
        
            if(((M72D_Brust_RX_Buf[11]=='P')&&(M72D_Brust_RX_Buf[12]=='D')&&(M72D_Brust_RX_Buf[13]=='P'))
               &&((M72D_Brust_RX_Buf[15]=='D')&&(M72D_Brust_RX_Buf[17]=='A')&&(M72D_Brust_RX_Buf[19]=='T')))
            {   //+PDP DEACT [GPRS链路(TCPIP)断开 ]&&(M72D_Brust_RX_Buf[16]=='E')&&(M72D_Brust_RX_Buf[18]=='C')
                CONNECT_FAIL_Flag     =0x11                   ;//CONNECT FAIL允许使能
                CONNECT_FAIL_RESET()                          ;//处理Conect_Fail链接
            }
        }
    }
}
void GSM_TCPIP_state()
{
  
  
}

void GSM_State()
{
    if(GSM_STAT_TIME_CNT>SECD_30)
    {
        GSM_STAT_TIME_CNT  =0                ;//检测手机卡 1分钟检测一次
        GSM_Power_state()                    ; 
        GSM_SIM_state()                      ;
        GSM_GSM_state()                      ;
        GSM_GPRS_state()                     ;
        GSM_TCPIP_state()                    ;
    }
}
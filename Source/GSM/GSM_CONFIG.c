#include "msp430x54x.h"
#include "GSM.h"

unsigned int  OVER_TIMES_CNT                                        ;//3.5分钟溢出时间计数
unsigned long int THR_Mint_Time_Cnt                                 ;//3.5分钟时间计数，用于不上传数据检测
unsigned long int GSM_INIT_TIME_CNT                                 ;//GSM初始化计时用
unsigned char GSM_ECHO=0x87                                         ;
unsigned char Thre_Num                                              ;//3次记录
unsigned char GSM_SIM_Signal()                                               ;
unsigned char CONNECT_FAIL_Flag                                     ;//CONNECT FAIL允许使能

extern unsigned char ASCIITOHEX( unsigned char Input_H,
                                unsigned char Input_L );//将ASCII字符转换为16进制的数
extern char StrSearch(char *a, char *b,
                      unsigned char aLength,
                      unsigned char bLength)           ;//字符串寻找
char CONNECT_FAIL_RESET(void)                           ;//处理Conect_Fail链接
//****************************************************************************//

unsigned long int  TCPIP_BASIC_CNT                                  ;//M72模块TCPIP基础设置计数
unsigned char TCPIP_BASIC_Step                                      ;//M72模块TCPIP基础设置
char * ATE_0_Disp = "ATE0"                                          ;//ATE<value>  命令回显模式 0不回显
char * AT_NO_DIS_DATA ="AT+QISDE=0"                                 ;//0=AT+QISEND数据不回显 
char * AT_OK = "AT"                                                 ;
char * AT_FGCNT_0 = "AT+QIFGCNT=0"                                  ;//配置前置场景受控于0通道
char * AT_CSD_GPRS = "AT+QICSGP=1,\"IP\",\"CMNET\""                 ;//设置CSD或GPRS连接模式
char * AT_SEND_OK = "AT+QIPROMPT=1"                                 ;//1==发送成功时显示“>”且返回“SEND OK”
char * AT_GPRS_CLASS_B = "AT+CGCLASS=\"B\""                         ;//GPRS移动台类别
char * AT_GPRS_ATTACH = "AT+CGATT=1"                                ;//GPRS网络附着
char * GPRS_ATTACH_L  = "AT+CGATT=0"                                ;//GPRS网络附着
char * Set_Bond_Rate = "AT&W"                                       ;//固定波特率
char *AT_CREG_1  =    "AT+CREG=1"                                   ;//寻找网络 AT+CGREG
char *AT_CGREG_1  =   "AT+CGREG=1"                                  ;//寻找网络 

/*******************************************************************\
*	      函数名：TCPIP_BASIC_SET             
*	      作用域：外部文件调用
*	      功能：  TCPIP基础设置
                  1 自适应波特率==115200      2  配置前置场景受控于0通道
                  3 设置CSD或GPRS连接模式     4 关闭数据回显
                  5 发送成功时显示“>”且返回“SEND OK”
                  6 GPRS移动台类别B           7 
                  8 GPRS网络附着
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：1 蒋本庆 2013.12.04 加入固定波特率设置 AT&W
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
                    OVER_TIMES_CNT=5;//重启系统
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
          }   //关闭数据回显   
          
         case 3:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_GPRS_CLASS_B,0,20)==1)
              TCPIP_BASIC_Step      =   4                            ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //设置CSD或GPRS连接模式     
          
         case 4:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_FGCNT_0,0,20)==1)
              TCPIP_BASIC_Step      =   5                            ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //配置前置场景受控于0通道
         case 5:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_SEND_OK,0,20)==1)
              {
                  TCPIP_BASIC_Step  =   6                            ;
                  GSM_ECHO &=~0x02                                   ;
              }
              Delayms(200)                                           ;
              break                                                  ;
          }   //发送成功时显示“>”且返回“SEND OK”            
          
         case 6:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,Set_Bond_Rate,0,20)==1)
                  TCPIP_BASIC_Step  =   7                            ;
              Delayms(200)                                           ;
              break                                                  ;                                              ;
          }   //GPRS移动台类别B
          
         case 7:
          { 
              if(GSM_SendCMD(PACKET_CONFIG,AT_CGREG_1,0,20)==1)
              TCPIP_BASIC_Step      =   8                            ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //配置非透传模式0
          
         case 8:
          { 
              TCPIP_BASIC_Step  =   9                                ;
              Delayms(200)                                           ;
              break                                                  ;
          }   //GPRS网络附着
          
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
                    GSM_ECHO    &=  ~0x80                            ;//配置成功
                    TCPIP_BASIC_Step      =   0                      ;
                    return GSM_ECHO                                  ;
                }
              Delayms(200)                                            ;
              break                                                  ;
          }   //发送成功时显示“>”且返回“SEND OK”      
          
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

//AT+CIMI  请求国际移动台设备标识（IMSI）
//AT+CIMI?
//+CIMI: "460023209180635"
//OK
//AT+CIMI
//460023209180635
//OK
char * SIM_AT_CIMI_Query = "AT+CIMI";



//1. 开机检测
//2. SIM 卡检测

// AT+CSNS0  配置语音/数据模式
// 1  语音/传真交替模式，语音优先
// 2  传真
// 3  语音/数据交替模式，语音优先
// 4  数据
// 5  语音/传真交替模式，传真优先
// 6  语音/数据交替模式，数据优先
// 7  数据跟随语音模式

// AT+CCLK? 实时时钟
//+CCLK: "13/06/19,02:34:02+00"

// AT+QNITZ=1  启用/禁用 GSM网络时间同步



char SIM_CARD_TYPE_FLAG;
unsigned int SIM_CARD_TIME_OUT;//SIM卡初始化计时用
unsigned long int No_SIM_CARD_Cnt;//检测不到SIM卡初始化计数
/*******************************************************************\
*	      函数名：SIM_NUM_CHECK             
*	      作用域：外部文件调用
*	      功能：  查询SIM卡CCID号 
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）SIM_Check
\*******************************************************************/
char SIM_NUM_CHECK()
{
    static int SIM_CARD_NUM                                           ;
    if(GSM_INIT_TIME_CNT>SIM_CARD_TIME_OUT)
    {
        GSM_INIT_TIME_CNT=0;//GSM初始化计时用            
        SIM_CARD_TIME_OUT=SECD_2;//SIM卡初始化计时用
        SIM_CARD_TYPE_FLAG=GSM_SendCMD(PACKET_Query, SIM_AT_QCCID_Query,0, 10);
        if (SIM_CARD_TYPE_FLAG==1) 
        {
            if (GSM_ECHO & 0x01) 
            {//带回显功能指令长度+3字节(0x0D 0x0D 0x0A)+内容  0x01
                SIM_CARD_NUM=GSM_strlen(SIM_AT_QCCID_Query)           ;
                if(SAVE_SIM_NUM_FH(SIM_CARD_NUM+3))
                {
                    SIM_CHG_WARN();//更换手机卡报警 
                    Module_Status[2] &= ~0XC0;
                    GPS_GSM_System_Stu[2] &= ~ 0XC0; 
                    Write_No_Chg_Card_Flash();//写换卡无卡Flash 
                    No_SIM_CARD_Cnt=0;//检测到SIM卡
                    return 1;
                }
                
            } else 
            {//2字节(0x0D 0x0A)+内容
                if(SAVE_SIM_NUM_FH(2))//存储手机卡号
                {
                    SIM_CHG_WARN(); 
                    Module_Status[2] &= ~0XC0;
                    GPS_GSM_System_Stu[2] &= ~ 0XC0;
                    Write_No_Chg_Card_Flash();//写换卡无卡Flash 
                    No_SIM_CARD_Cnt=0;//检测到SIM卡
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
                Module_Status[2] |= 0XC0;//取卡报警
                GPS_GSM_System_Stu[2] |= 0XC0; 
                Write_No_Chg_Card_Flash();//写换卡无卡Flash 
            }
            
            SIM_CARD_TIME_OUT=SECD_5;//SIM卡初始化计时用
        }
    }
    return 0                                                          ;
}

//AT+CREG 网络注册信息<stat>  0   未注册；ME当前没有搜索到要注册业务的新营运商
// 1   已注册，本地网
// 2   未注册，但 ME 正在搜索要注册业务的新营运商
// 3   注册被拒绝
// 4   未知
// 5   已注册，漫游
//AT+CREG?
//+CREG: 0,1
//OK
char * GSM_AT_CREG_Query = "AT+CREG?";
/*******************************************************************\
*	      函数名：GSM_CREG_Init             
*	      作用域：外部文件调用
*	      功能：  查询SIM卡CCID号 
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）SIM_Check
\*******************************************************************/
unsigned long int GSM_CREG_TIME_OUT =500;//SIM卡初始化计时用
unsigned int CREG_Zhu_Ce_Cnt ;//注册网络查询注册次数
char GSM_CREG_Init()
{
    if(GSM_INIT_TIME_CNT>GSM_CREG_TIME_OUT)
    {
        GSM_INIT_TIME_CNT=0;//GSM初始化计时用
        GSM_CREG_TIME_OUT=500;
        GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
        if((GSM_SIM_Iint_Sig_Num>5)&&(GSM_SIM_Iint_Sig_Num<32))
        {
            GSM_SIM_Iint_Sig_Num=0;  
            if (GSM_SendCMD(PACKET_Query, GSM_AT_CREG_Query,0, 10) == 1) 
            {
                if (GSM_ECHO & 0x01) 
                {//指令长度+3字节(0x0D 0x0D 0x0A)+内容
                    if ((M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10 - 1] == '1')||
                        (M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10 - 1] == '5')||
                        (CREG_Zhu_Ce_Cnt++ >20)||
                        ((M72D_Query_RX_Buf[0]==0x0D&&M72D_Query_RX_Buf[1]==0x0A)&&
                        (M72D_Query_RX_Buf[2+10 - 1] == '1'||M72D_Query_RX_Buf[2+10 - 1] == '5'))
                        )
                    {
                      
                        Module_Status[1]  &= ~ 0xC0;//GPRS信号盲区
                        GPS_GSM_System_Stu[1] &= ~0xC0;
                        Delayms(200);//XX*1Ms延时
                        if(GSM_SendCMD(PACKET_CONFIG,AT_GPRS_ATTACH,0,20)==1)
                        {
                            CREG_Zhu_Ce_Cnt=0;
                            Delayms(200);
                            if(GSM_SendCMD(PACKET_CONFIG,AT_CSD_GPRS,0,20)==1)
                            return 1;
                        }
                    }
        
                } else {
                    //2字节(0x0D 0x0A)+内容
                    if (((M72D_Query_RX_Buf[0]==0x0D&&M72D_Query_RX_Buf[1]==0x0A)&&(M72D_Query_RX_Buf[2+10 - 1] == '1'||M72D_Query_RX_Buf[2+10 - 1] == '5'))
                        ||(CREG_Zhu_Ce_Cnt++ >20))
                    {
                        Module_Status[1]  &= ~ 0xC0;//GPRS信号盲区
                        GPS_GSM_System_Stu[1] &= ~0xC0;
                        Delayms(200);//XX*1Ms延时
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
                     Module_Status[1]  |= 0xC0;//GPRS信号盲区
                     GPS_GSM_System_Stu[1]|=0xC0;  
                }
                
                GSM_CREG_TIME_OUT=SECD_5;//SIM卡初始化计时用
            }
        }
    }
    return 0;
}


/*******************************************************************\
*	      函数名：GSM_GPRS_Init             
*	      作用域：外部文件调用
*	      功能：  查询SIM卡CCID号 
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）SIM_Check
\*******************************************************************/
char * GSM_AT_CGREG = "AT+CGREG?";
char GSM_SIM_Iint_Sig_Num;
char GSM_GPRS_Init()
{
    if(GSM_INIT_TIME_CNT>GSM_CREG_TIME_OUT)
    {
        GSM_INIT_TIME_CNT=0 ;//GSM初始化计时用
        GSM_CREG_TIME_OUT=500;//SIM卡初始化计时用
        GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
        //Delayms(200);//XX*1Ms延时
        if((GSM_SIM_Iint_Sig_Num>5)&&(GSM_SIM_Iint_Sig_Num<32))
        {
            GSM_SIM_Iint_Sig_Num=0;  
            if (GSM_SendCMD(PACKET_Query, GSM_AT_CGREG,0, 10) == 1) 
            {   
                Delayms(200);//XX*1Ms延时
                if (GSM_ECHO & 0x01) 
                {//指令长度+3字节(0x0D 0x0D 0x0A)+内容
                    if (M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10] == '1'||
                        M72D_Query_RX_Buf[GSM_strlen(GSM_AT_CREG_Query) + 3 + 10] == '5')
                    {
                        CREG_Zhu_Ce_Cnt=0;
                        return 1;
                    }
        
                } else 
                {//2字节(0x0D 0x0A)+内容
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
        
        
        GSM_CREG_TIME_OUT=SECD_5;//SIM卡初始化计时用
    }
    return 0;
}

// AT+QIFGCNT  配置前置场景


// AT+QNTP  通过TCP/IP网络时间服务器同步本地时间
char * TCPIP_AT_QNTP = "AT+QNTP";

//AT+QIOPEN="UDP","58.57.53.58","1090" 山重建机有限公司GPS管理平台端口
//char * TCPIP_AT_QIOPEN = "AT+QIOPEN=\"UDP\",\"58.57.53.58\",\"2013\"";


char UDP_Built_STR[41]={'A','T','+','Q','I','O','P','E','N','=',                     //  AT+QIOPEN=
                        '"','U','D','P','"',',',                                     //"UDP",
                        '"','0','5','8','.','0','5','7','.','0','5','3','.','0','5','8','"',',',     //"58.57.53.58",
                        '"','2','0','1','3','"'};                                    //"1090"    '"','1','0','9','0','"',0X0D }  '"','2','0','1','3','"',0X0D };  
//AT+QIOPEN="UDP","58.57.53.58","2013" 山重建机有限公司GPS管理平台端口

char GSM_TCPIP_Init()
{
/*      其他错误返回的分析及处理： 
一、  "ERROR"，这里返回ERROR可能有两个原因： 
1、 命令格式不对，如果所有数据格式都对，需要看看QIMUX是不是为0（通过命令
"AT+QIMUX?"查询，如果查询结果为1，则需要通过命令AT+QIMUX=0将其重新
设为0）。 
2、 当前的TCPIP服务的状态不是IP INITIAL或IP STATUS或IP CLOSE（通过命令
AT+QISTAT查询）。如果当前状态为TCP CONNECTING，则需要执行
AT+QICLOSE关闭当前失败的TCP连接。如果是其他状态，则需要执行命令
AT+QIDEACT断开当前失败的GPRS场景。 
二、  "ALREADY CONNECT"，这表明已经存在一个TCP连接或UDP连接。如果确认需要建
立新的连接，则需要命令AT+QICLOSE关闭当前连接。 
  
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
        GSM_INIT_TIME_CNT=0;//GSM初始化计时用
        GSM_CREG_TIME_OUT=SECD_5;//SIM卡初始化计时用
                
        if(GSM_SendCMD(PACKET_Query,"AT+QINDI=1", 0,10)==1)                            ;//打开TCPIP缓存
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
                   CONNECT_FAIL_Flag =0x11;//CONNECT FAIL允许使能
                   CONNECT_FAIL_RESET();//处理Conect_Fail链接
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
               //CONNECT_FAIL_Flag      =0x11                                          ;//CONNECT FAIL允许使能
               //CONNECT_FAIL_RESET()                                                  ;//处理Conect_Fail链接
               return 0 ;
          }
        default:
          {
               //Delayms(60000)                                                        ;//XX*1Ms延时
               //CONNECT_FAIL_Flag      =0x11                                          ;//CONNECT FAIL允许使能
               //CONNECT_FAIL_RESET()                                                  ;//处理Conect_Fail链接
               return 0;
          }   
        }
        
    }
    return 0;
}



/********************************************************\
*	函数名：GSM_SIM_Signal
   1    作用域：外部文件调用
*	功能：  初始化主时钟
*	参数：  
             AT+CSQ?   信号质量
             +CSQ:28,0
             OK
*	返回值: 无
*	修改历史：（每条详述）
\********************************************************/
unsigned char GSM_SIM_Signal_Num                                ;//信号质量强度
char * GSM_SIM_Signal_AT = "AT+CSQ"                             ;
char * CSQ_String = "+CSQ:"                                     ;
char   SIM_Signal                                               ;//GPS_RX_Buffer接收数组首地址
unsigned char GSM_SIM_Signal()
{    
    //Delayms(1000);
    char SIM_Num                                                ;//内部计数用
    GSM_SIM_Signal_Num=0                                        ;
    if (GSM_SendCMD(PACKET_Query, GSM_SIM_Signal_AT,0, 10) == 1) //返回OK
    {
        //解析 +CSQ:28,0 //带回显功能 //0x0D 0x0A+内容(6)
        
        SIM_Signal=StrSearch(M72D_Query_RX_Buf, CSQ_String,
                      M72D_Query_RX_BUF_SIZE,4)                 ;//字符串寻找
        for(SIM_Num=0;SIM_Num<3;SIM_Num++)        
        {
             if((M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]!=',')
                &&((M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]>0x2F)&&(M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]<0x3A)))
             {
                GSM_SIM_Signal_Num=GSM_SIM_Signal_Num*10+
                                   ASCIITOHEX('0',M72D_Query_RX_Buf[SIM_Signal+2+SIM_Num]);//可用星数 
             }
             else {break;}  
        } 
        return GSM_SIM_Signal_Num                                ;
    }
    return 0;
}
 

/*******************************************************************\
*	      函数名：Conect_Fail_Reset             
*	      作用域：外部文件调用
*	      功能：  处理Conect_Fail链接  
*	      参数：  
*	      返回值：无     
*
*	      修改历史：（每条详述）
\*******************************************************************/           
char CONNECT_FAIL_RESET(void)                                         //处理Conect_Fail链接
{
    if((THR_Mint_Time_Cnt>MSP_A0_Min_3)||(CONNECT_FAIL_Flag==0x11)) 
    {
        CONNECT_FAIL_Flag   =0x00                                   ;//标志位清零
        THR_Mint_Time_Cnt   =0                                      ;//3.5分钟计数清零
        GSM_STATUS          = 0x3D                                  ;
        GSM_ECHO            =0x87                                   ;
        UDP_Built_STR[40]  =0                                       ;
        
        Module_Status[1] |=  0x30                                   ;//GPRS网络异常
        GPS_GSM_System_Stu[1]|=0x30                                 ;
        CREG_Zhu_Ce_Cnt =0;//注册网络查询注册次数
        
        if(UDP_Built_flag==0x11)
        {
            UDP_Built_flag      =0x00                                   ;//00==网络断开或者没有建立
            GSM_SendCMD(PACKET_Query,"AT+QICLOSE",0, 1000)              ;   
            Delayms(200)                                                ;
            GSM_SendCMD(PACKET_Query,"AT+QIDEACT",0, 1000)              ;
            Delayms(200)                                                ;
            GSM_SendCMD(PACKET_CONFIG,GPRS_ATTACH_L,0,20)               ;
        }
        
        OVER_TIMES_CNT  ++                                          ;  
        if(OVER_TIMES_CNT>3)
        {
            RE_START_GSM()                                          ;//重启系统
            OVER_TIMES_CNT  =0                                      ;
        }
        
        No_SIM_CARD_Cnt=0;
        Heart_Beat_Count    =0                                      ;//发送心跳1ms时间计数
        if(MCU_Data_Count>(MCU_TimeOut>>2))
            MCU_Data_Count =   MCU_TimeOut >>  2                    ;//1ms 数据包计数
        return 1                                                    ; 
    }
    return 0                                                        ;
}
           
           
           
           
           
           
           
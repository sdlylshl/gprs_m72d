#include "msp430x54x.h"
#include "PortIO.h"

unsigned long int One_Mint_Warn_Cnt;//一分钟计数

extern unsigned int  OVER_TIMES_CNT;//3.5分钟溢出时间计数

extern unsigned long int Two_JI_Warn_Cnt;//二级报警计数
extern unsigned long int One_JI_Warn_Cnt;//一级报警计数
extern unsigned long int ExitLMP_Time_Cnt;//退出低功耗时计数
extern unsigned long GSM_GPS_AT_Cmd_Cnt;//发送GSM获取地理信息计数
extern unsigned long int No_SIM_CARD_Cnt;//检测不到SIM卡初始化计数
extern unsigned long int GPS_ANT_IO_CNT;//GPS天线次数
extern unsigned long int SPI_Iint_Time_Cnt;//430开机计时
extern unsigned long int SIM_Card_Work_Cnt;//发发送手机卡号时间计时
extern unsigned char Soft_VER_OK                                        ;
extern unsigned long int MSP_430_Start_Time             ;//430开机计时
extern unsigned long LOW_POW_TIME_CNT                                         ;
extern unsigned long int GSM_INIT_TIME_CNT                                    ;//GSM初始化计时用
extern unsigned long int GSM_STAT_TIME_CNT                                    ;//检测手机卡 5分钟检测一次
extern unsigned long int UP_SPI_NUM_CNT                                       ;//远程升级计数
extern unsigned long int CALL_MCU_TIME_CNT                                    ;//时间计数
extern unsigned long int RD_Data_1_Min_Cnt                                    ;//解析 MUC 应答时间计数
extern unsigned int GSM_TimeOut_1ms                                           ;
extern unsigned long int Soft_Vers_Cnt                                        ;//发送软件版本号时间计时
extern unsigned long int GSM_Start_Count                                      ;//开机延时
extern unsigned long int GPS_LOCATE_Time                                      ;//GPS接收时间10S
extern unsigned long Write_Flag_time                                          ;
extern unsigned long int THR_Mint_Time_Cnt                                    ;//3.5分钟时间计数，用于不上传数据检测
extern unsigned char Want_GPS_Data_Type                                       ;//需要GPS数据类型00==无响应；0x11==GPRMC；0x22==GPGGA； 
//Timer
//***************************************************************************//
//                                                                           //
//  Init_TimerA0(void): 设置TimerA0                                          //
//                                                                           //
//***************************************************************************//
void TIME_A0_Init(void)
{
  //定时器 工作在比较模式
  
    //TA控制寄存器
    //时钟选择TASSEL1:TASSEL0 00 TACLK 01 ACLK 10 SMCLK 11 INCLK
    //时钟分频 ID1:ID0      00 1/1 01 1/2 10 1/4 11 1/8
    //模式控制 MC1:MC0      00 停止 01 增计数 10连续计数 11 增减计数
    //定时器清零 TACLR      1 清零
    //中断允许位 TAIE       1 中断允许
    //中断标志位 TAIFG      1 中断请求
    //TA0CTL   =TASSEL1+TACLR;//计数时钟设为SMCLK,分频系数设置为1,计数器清0,复位Timer0_A5
    
    TA0CTL   =TASSEL__ACLK+ID__8+TACLR;//计数时钟设为ACLK,分频系数设置为8,计数器清0,复位Timer0_A5

    //增计数   0~TA0CCR0-0-TA0CCR0.....
    //连续计数 0~0xFFFF~0~0xFFFF......
    //增减计数 0~TA0CCR0~0~TA0CCR0....
    //TA0CCR0  =  16000  - 1                                   ; // SMCK=EX2=16MHz，设置计数器溢出时间为1ms
    
    TA0CCR0  =  4  - 1                                   ; // ACLK=EX1=32.768K/8=4096，设置计数器溢出时间为1ms

    //捕获/比较控制寄存器
    //捕获方式 CM1:CM0  00禁止捕获 01 上升沿 10下降沿 11上下都捕获
    //捕获源选择 CCIS1:CCIS0  00 CCIxA 01 CCIxB 10 GND 11 VCC
    //同步方式 SCS  0 异步 1 同步
    //工作模式选择 CAP  0比较 1捕获
    //输出模式 OUTMx 
    //输出信号  0 输出低电平 1 输出高电平
    //捕获溢出 COV  1捕获溢出
    //中断标志 CCIFG  1 中断请求
    TA0CCTL0 = CCIE;//禁用捕获 使能比较中断

    TA0CTL  |= (1 << 4)                                      ; // 设置计数器为加计数，启动
}





//心跳包
extern unsigned long int Heart_Beat_Count;                     //1ms时间计数
//数据包
extern unsigned long int MCU_Data_Count;                     //1ms时间计数
extern unsigned long int MCU_ACK_Count;                              //应答包计数
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
   GSM_Start_Count ++                                     ;//开机延时
   GSM_TimeOut_1ms++                                      ;//指令超时时间计数
   Heart_Beat_Count++                                     ;//心跳包发送时间计数
   MCU_Data_Count++                                       ;//数据包发送时间计数
   MCU_ACK_Count++                                        ;//数据包响应超时
   GPS_LOCATE_Time++                                      ;//GPS接收时间10S
   Soft_Vers_Cnt  ++                                      ;//发送软件版本号时间计时
   THR_Mint_Time_Cnt    ++                                ;//3.5分钟时间计数，用于不上传数据检测
   Write_Flag_time++                                      ;//写flash间隔
   RD_Data_1_Min_Cnt    ++                                ;//解析 MUC 应答时间计数
   CALL_MCU_TIME_CNT    ++                                ;//时间计数
   UP_SPI_NUM_CNT       ++                                ;//远程升级计数
   GSM_STAT_TIME_CNT    ++                                ;//检测手机卡 5分钟检测一次
   GSM_INIT_TIME_CNT    ++                                ;//GSM初始化计时用
   LOW_POW_TIME_CNT     ++                                ;
   SIM_Card_Work_Cnt++;
   SPI_Iint_Time_Cnt++;//430开机计时
   GPS_ANT_IO_CNT++;//GPS天线次数
   GSM_GPS_AT_Cmd_Cnt++;//发送GSM获取地理信息计数
   ExitLMP_Time_Cnt++;//退出低功耗时计数
   One_JI_Warn_Cnt++;//一级报警计数
   Two_JI_Warn_Cnt++;//二级报警计数
   One_Mint_Warn_Cnt++;//一分钟计数
   
   
   if(!(P2IN&PWRON))
   {
	   MSP_430_Start_Time   ++          ;//430开机计时
   }
   
//    TA0CCR0  = (16000) - 1                                  ;
}





/*******************************************************************\
*	      函数名：Do_GPS_Data             
*	      作用域：外部文件调用
*	      功能：  接收、处理GPS数据  
*	      参数：  
           1、通过控制GPS数据类型（00=无响应；11=GPRMC；22=GPGGA）控制步骤
           2、Do_GPS_Mesg_OK_Flag：GPS数据处理完成标志 00==未完成；11==完成
           3、Open_UCA1_UART_Init(void)==用接收完成数据不可用时打开GPS中断
*	      返回值：无     
*
*	      修改历史：（每条详述）
\*******************************************************************/
void Time_Count_Value_Init(void)                          //时间计数清零
{
  
   OVER_TIMES_CNT        =0                               ;
   GSM_TimeOut_1ms       =0                               ;//指令超时时间计数
   Heart_Beat_Count      =0                               ;//心跳包发送时间计数
   MCU_Data_Count        =0                               ;//数据包发送时间计数
   MCU_ACK_Count         =0                               ;//数据包响应超时
   GPS_LOCATE_Time       =10241                           ;//GPS接收时间10S
   Soft_Vers_Cnt         =0                               ;//发送软件版本号时间计时
   THR_Mint_Time_Cnt     =0                               ;//3.5分钟时间计数，用于不上传数据检测
   Write_Flag_time       =0                               ;//写flash间隔
   RD_Data_1_Min_Cnt     =0                               ;//解析 MUC 应答时间计数
   CALL_MCU_TIME_CNT     =0                               ;//时间计数
   UP_SPI_NUM_CNT        =0                               ;//远程升级计数
   GSM_STAT_TIME_CNT     =0                               ;//检测手机卡 5分钟检测一次
   GSM_INIT_TIME_CNT     =0                               ;//GSM初始化计时用
   LOW_POW_TIME_CNT      =0                               ;
   Want_GPS_Data_Type    =0x01                            ;//需要GPS数据类型00==无响应；0x11==GPRMC；0x22==GPGGA； 
   Soft_VER_OK=0x11;
   SIM_Card_Work_Cnt=0;
   GPS_ANT_IO_CNT=0;//GPS天线次数
   No_SIM_CARD_Cnt=0;
   GSM_GPS_AT_Cmd_Cnt=0;//发送GSM获取地理信息计数
   One_JI_Warn_Cnt=0;//一级报警计数
   Two_JI_Warn_Cnt=0;//二级报警计数
   One_Mint_Warn_Cnt=0;//一分钟计数
}
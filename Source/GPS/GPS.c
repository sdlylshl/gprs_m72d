#include "msp430x54x.h"
#include "GPS.h"

#define GPS_RX_Buffer_Size 100
char  GPS_RX_Buffer[GPS_RX_Buffer_Size]                      ;//GPS数据缓存100
unsigned int GPS_RX_Buffer_Cnt                               ;//GPS数据缓存计数
unsigned char Want_GPS_Data_Type=0x01                        ;//需要GPS数据类型00==无响应；0x11==GPRMC；0x22==GPGGA； 
unsigned char GPS_R_OK_End_Flag                              ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
//GPS
extern void GPS_UCA1_Init(void)                              ;//GPS中断初始化
extern void SHUT_GPS_UCA1(void)                             ;//关闭GPS中断
//***************************************************************************//
//  Init_UART(void): 初始化UCA1   GPS                                        //
//***************************************************************************//
void GPS_UCA1_Init(void)
{
 //UCA1TX P5.6  UCA1RX P5.7
 /* P5SEL   |= BIT6 + BIT7                                           ; // 选择引脚功能
  P5DIR   |= BIT6                                                  ; // TX设置为输出
    //奇偶校验 UCPEN      1允许
    //校验选择 UCPAR      0 奇校验  1偶校验
    //发送寄存器 UCMST    0低位先发 1 高位先发
    //字符长度 UC7BIT     0  8位    1 7位
    //停止位选择 UCSPB    0 1位停止位 1 2位停止位
    //UCSI模式选择 UCMODE 00 UART模式 01 多机模式 10地址位多机模式 11 自动波特率检测
    //同步模式 UCSYNC     0异步模式 1 同步模式
    //1停止位 8数据位 无校验 异步 低位先发
    UCA1CTL0 = 0;
    //时钟源选择 UCSSELx 00 外部时钟 UCLK 01 ACLK 10 SMCLK 11 SMCLK
    //接收出错中断允许 UCRXEIE 0 不允许 1 允许中断
    //接收打断字符中断允许 UCBRKIE 0 不设置UCRXIFG 1设置UCRXIFG
    //睡眠状态 UCDORM 0不睡眠 1 睡眠
    //发送地址位 UCTXADDR 0发送的下一帧是数据 1 发送的下一帧是地址 （多机模式）
    //发送打断 0发送的下一帧不是打断 （自动波特率检测）
    //软件复位允许 0 禁止 1 允许 (复位状态下 USCI保持逻辑电平)
    UCA1CTL1 = UCSWRST; // 状态机复位
    UCA1CTL1 |= UCSSEL__SMCLK; // CLK =
    //1.048M  115200 BR=  9 BRS=1 BRF=0
    //1.048M  9600 BR=109 BRS=2 BRF=0
    //20M 115200 BR=173 BRS=5 BRF=0
    //16M 115200 BR=0x8A BRS=7 BRF=0
    //1M  115200 BR=8 BRS=6 BRF=0
    //16M 9600   BR=1666 BRS=6 BRF=0
    //1M  9600   BR=104 BRS=1 BRF=0
    //32K 9600   BR=3   BRS=3 BRF=0
    //波特率 115200
    UCA1BR0 = 0x8A; // 32kHz/9600=3.41
    UCA1BR1 = 0x00;
    UCA1MCTL = UCBRS_7 + UCBRF_0; // UCBRSx=3, UCBRFx=0

    UCA1CTL1 &= ~UCSWRST; // 启动状态机
    UCA1IE |= UCRXIE; // 允许接收中断  */
  
 //UCA1TX P5.6  UCA1RX P5.7
  P5SEL   |= BIT6 + BIT7                                           ; // 选择引脚功能
  P5DIR   |= BIT6                                                  ; // TX设置为输出
  UCA1CTL1        = UCSWRST                                        ; // 状态机复位
  UCA1CTL1       |= UCSSEL_1                                       ; // CLK = ACLK
  UCA1BR0         = 0x03                                           ; // 32kHz/9600=3.41 
  UCA1BR1         = 0x00                                           ; 
  UCA1MCTL        = UCBRS_3 + UCBRF_0                              ; // UCBRSx=3, UCBRFx=0
  UCA1CTL1       &= ~UCSWRST                                       ; // 启动状态机
  UCA1IE         |= UCRXIE                                         ; // 允许接收中断
  
  GPS_ANT_IO_Init();
}

void SHUT_GPS_UCA1(void)                                             //关闭GPS初始化  
{
    //UCA1TX P5.6  UCA1RX P5.7
    P5SEL   &= ~(BIT6 + BIT7)                                        ; // 设置为普通IO
    P5DIR   &= ~(BIT6 + BIT7)                                        ; // TX RX 设置为输入
    //关闭UCA1
    UCA1CTL1        = UCSWRST                                        ; // 状态机复位
}

//***************************************************************************//
//  GPS接收中断服务程序                                                      //
//***************************************************************************//
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    
    GPS_RX_Buffer[GPS_RX_Buffer_Cnt] = UCA1RXBUF             ;
    if(GPS_RX_Buffer[GPS_RX_Buffer_Cnt]=='P'&&GPS_RX_Buffer[GPS_RX_Buffer_Cnt-1]=='G')
    {
      if(GPS_RX_Buffer[GPS_RX_Buffer_Cnt-2]=='$')
          GPS_RX_Buffer_Cnt = 0                               ;//判断接收头文件
    }
    
    if((GPS_RX_Buffer[GPS_RX_Buffer_Cnt-1]==0x0D)&&(GPS_RX_Buffer[GPS_RX_Buffer_Cnt]==0x0A))
    {
      if(Want_GPS_Data_Type==0x11)
      {
          if((GPS_RX_Buffer[1]=='R')&&(GPS_RX_Buffer[2]=='M')&&(GPS_RX_Buffer[3]=='C'))
          {
             GPS_R_OK_End_Flag=0x11                               ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
             SHUT_GPS_UCA1()                                     ;
          }
      }
      if(Want_GPS_Data_Type==0x22)
      { 
          if((GPS_RX_Buffer[1]=='G')&&(GPS_RX_Buffer[2]=='G')&&(GPS_RX_Buffer[3]=='A'))
           {
             GPS_R_OK_End_Flag=0x22                               ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
             SHUT_GPS_UCA1()                                     ;
           }
      }
    }
    
    GPS_RX_Buffer_Cnt++                                       ;
    if (GPS_RX_Buffer_Cnt > GPS_RX_Buffer_Size) 
    {GPS_RX_Buffer_Cnt = 0;}                                   
}

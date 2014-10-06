#include "msp430x54x.h"
#include "GSM.h"

//0 正常 0x01关机 0x02休眠 0x04 SIM卡 0x08 GSM网络正常 0x10GPRS网络正常 0x20 TCPIP正常
extern unsigned char GSM_STATUS;
//GSM 状态
#define TCPIP_ERRO 0x20
#define GPRS_ERRO 0x10
#define GSM_ERRO 0x08
#define SIM_ERRO 0x04
#define SLEEP_ERRO 0x02
#define POWER_ERRO 0x01

//*****************************************************************************//
//M72D接收缓冲器
unsigned int M72D_RX_Buf_Cnt                                    ;//接收缓存计数器
unsigned int UP_Data_Cnt                                        ;//SIM卡初始化计时用
unsigned char packet_type                                       ;//M72D  包类型
//M72D 突发传输指令接收缓存器 (未发送指令 即可收到数据 如 RING +QRD 等)
#define PACKET_BRUST 0x00                                        //PACKET_TYPE=0 (突发接收包)
#define M72D_Brust_RX_BUF_SIZE 80                                
unsigned char M72D_Brust_RX_Buf[M72D_Brust_RX_BUF_SIZE]          ;
unsigned char M72D_Brust_STATUS                                  ;//突发包接收完成 0 无响应 0x01 OK 0x02 ERRO 0x04 数据溢出
unsigned char M72D_Brust_Cnt                                     ;


#define PACKET_Query 0x01                                         //PACKET_TYPE=1 (模块查询包)
#define M72D_Query_RX_BUF_SIZE 50                                 //数据传输最大字节数
unsigned char M72D_Query_RX_Buf[M72D_Query_RX_BUF_SIZE];
unsigned char M72D_Query_STATUS;     //配置包接收完成 0 无响应 0x01 OK 0x02 ERRO 0x04 数据溢出

//PACKET_TYPE=2 (模块配置包)
#define PACKET_CONFIG 0x02
#define M72D_SetConfig_RX_BUF_SIZE 50                         //数据传输最大字节数
unsigned char M72D_SetConfig_RX_Buf[M72D_SetConfig_RX_BUF_SIZE];
unsigned char M72D_SetConfig_STATUS; //配置包接收完成 0 无响应 0x01 OK 0x02 ERRO 0x04 数据溢出

//M72D 数据包接收缓存 包含升级包
//PACKET_TYPE=4 (数据接收包)
#define PAKET_DATA 0x04
#define M72D_ServerData_RX_BUF_SIZE 1200                         //数据传输最大字节数
unsigned char M72D_ServerData_RX_Buf[M72D_ServerData_RX_BUF_SIZE];
unsigned char M72D_ServerData_STATUS; //数据包接收完成 0 无响应 0x01 OK 0x02 ERRO 0x04 数据溢出

//M72D 网络配置  主要针对配置时间长的包进行设置
//PACKET_TYPE=8 (长时间包)
#define PACKET_NETCONFIG 0x08
#define M72D_NetConfig_RX_BUF_SIZE 100                         //数据传输最大字节数
unsigned char M72D_NetConfig_RX_Buf[M72D_NetConfig_RX_BUF_SIZE];
unsigned char M72D_NetConfig_STATUS; //网络包接收完成 0 无响应 0x01 OK 0x02 ERRO 0x04 数据溢出

//M72D 未知包
//PACKET_TYPE=0x10 (未知包)
#define PCKET_OTHER 0x10
#define M72D_Other_RX_BUF_SIZE 100                         //数据传输最大字节数
unsigned char M72D_Other_RX_Buf[M72D_Other_RX_BUF_SIZE];
unsigned char M72D_Other_STATUS;              //未知包接收完成 0 无响应zx02 ERRO 0x04 数据溢出
//*****************************************************************************//
//GSM
extern void GPS_UCA1_Init(void);
extern void SHUT_GPS_UCA1(void);
extern void GSM();
extern char Conect_Fail_Reset(void)                                         ;//处理Conect_Fail链接
extern char GSM_Brust()                                                     ;//突发数据处理
//***************************************************************************//
//  Init_UART(void): 初始化UCA0   GSM                                        //
//***************************************************************************//
void GSM_UCA0_Init(void)
{
    //UCA0TX P3.4 UCA0RX P3.5
    P3SEL |= BIT4 + BIT5; // 选择引脚功能
    P3DIR |= BIT4; // TX设置为输出
    //奇偶校验 UCPEN      1允许
    //校验选择 UCPAR      0 奇校验  1偶校验
    //发送寄存器 UCMST    0低位先发 1 高位先发
    //字符长度 UC7BIT     0  8位    1 7位
    //停止位选择 UCSPB    0 1位停止位 1 2位停止位
    //UCSI模式选择 UCMODE 00 UART模式 01 多机模式 10地址位多机模式 11 自动波特率检测
    //同步模式 UCSYNC     0异步模式 1 同步模式
    //1停止位 8数据位 无校验 异步 低位先发
    UCA0CTL0 = 0;
    //时钟源选择 UCSSELx 00 外部时钟 UCLK 01 ACLK 10 SMCLK 11 SMCLK
    //接收出错中断允许 UCRXEIE 0 不允许 1 允许中断
    //接收打断字符中断允许 UCBRKIE 0 不设置UCRXIFG 1设置UCRXIFG
    //睡眠状态 UCDORM 0不睡眠 1 睡眠
    //发送地址位 UCTXADDR 0发送的下一帧是数据 1 发送的下一帧是地址 （多机模式）
    //发送打断 0发送的下一帧不是打断 （自动波特率检测）
    //软件复位允许 0 禁止 1 允许 (复位状态下 USCI保持逻辑电平)
    UCA0CTL1 = UCSWRST; // 状态机复位
    UCA0CTL1 |= UCSSEL__SMCLK; // CLK =
    //1.048M  115200 BR=  9 BRS=1 BRF=0
    //1.048M  9600 BR=109 BRS=2 BRF=0
    //20M 115200 BR=173 BRS=5 BRF=0
    //16M 115200 BR=0x8A BRS=7 BRF=0
    //1M  115200 BR=8 BRS=6 BRF=0
    //16M 9600   BR=1666 BRS=6 BRF=0
    //1M  9600   BR=104 BRS=1 BRF=0
    //32K 9600   BR=3   BRS=3 BRF=0
    //波特率 115200
    UCA0BR0 = 0x8A; // 32kHz/9600=3.41
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS_7 + UCBRF_0; // UCBRSx=3, UCBRFx=0

    UCA0CTL1 &= ~UCSWRST; // 启动状态机
    UCA0IE |= UCRXIE; // 允许接收中断

}

void SHUT_GSM_UCA0(void)
{
    //UCA1TX P5.6  UCA1RX P5.7
    P3SEL &= ~(BIT4 + BIT5); // 设置为普通IO
    P3DIR &= ~(BIT4 + BIT5); // TX RX 设置为输入
    //关闭UCA1
    UCA0CTL1 = UCSWRST; // 状态机复位
}


void GSM()
{
    //0 正常 0x01关机 0x02休眠 0x04 SIM卡 0x08 GSM网络正常 0x10GPRS网络正常 0x20 TCPIP正常
    //GSM_SIM_Signal();//测试信号强度
    //Delayms(1000);
    GSM_State();
    CONNECT_FAIL_RESET();//处理Conect_Fail链接
    GSM_Brust();//突发数据处理
    //GSM正常工作下
    if (GSM_STATUS == 0) 
    {//初始化完成 且一切状态正常
            GSM_Heart_Beat();
            GSM_Send_MCU(); 
            RecData();
            Send_Soft_Version();//发送软件版本号  
            Call_MCU_Data()    ;//呼叫GPS终端上报定位数据命令
            DO_ROOT_UP()       ;//处理远程升级程序
            Send_SIM_Card_NUM();//开机发送手机卡号
            CHECK_MCU_WARN();//检测MCU报警
    } 
    else 
    {//硬件检测
        if ((GSM_STATUS & 0x03)) 
        {//GPS关机或休眠//重新启动GSM 故障检测
            GSM_NOM_POW_ON();
        } 
        else 
        {//GSM 工作状态检测//回显配置 (模块开机才能配置)
            GSM_State()                                         ;
            if(GSM_ECHO & 0x80)
            {
              TCPIP_BASIC_SET();
            }
            else
            {
                if (GSM_STATUS & SIM_ERRO) 
                {//SIM卡检测失败
                    if (SIM_NUM_CHECK())
                    {
                        GSM_STATUS &= ~SIM_ERRO;
                    }
                } 
                else 
                {//初始化网络
                    if (GSM_STATUS & 0x08) 
                    {
                        if (GSM_CREG_Init())
                            GSM_STATUS &= ~GSM_ERRO;
                    } 
                    else 
                    {
                        //2.GPRS
                        if (GSM_STATUS & 0x10) 
                        {
                            if(GSM_GPRS_Init())
                              GSM_STATUS &= ~GPRS_ERRO;
                        } 
                        else 
                        {//3.TCPIP
                            if (GSM_STATUS & 0x20) 
                            {//TCPIP 未连接或连接失败
                                if(GSM_TCPIP_Init())
                                {
                                    UDP_Built_flag        =0x11;
                                    GSM_STATUS &= ~TCPIP_ERRO;
                                    Module_Status[1]  &= ~ 0x30;//GPRS网络异常
                                    GPS_GSM_System_Stu[1] &= ~0x30;
                                    Heart_Beat_Count =   30720;
                                    READ_UP_SPI_FLASH();//转移软件版本号及升级类型
                                }
                            }
                            else 
                            {
                                //TCPIP 连接成功
                                GSM_STATUS = 0;
    
                            }                                        //TCPIP
                        }                                        //GPRS
                    }                                        //GSM
                }                                        //SIM
            }                                       //基础配置
        }                                        //POWER
    }                                        //STATUS

}                                        //end while

//***************************************************************************//
//  GSM接收中断服务程序                                                      //
//***************************************************************************//
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{

    switch (packet_type) {
    case PACKET_BRUST:
      {//突发接收
          M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
    
          if (M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == '+')
          {
                M72D_RX_Buf_Cnt=0;
                break;
          }
          
          if (M72D_RX_Buf_Cnt > M72D_Brust_RX_BUF_SIZE) 
          {
                M72D_RX_Buf_Cnt = 0;
                M72D_Brust_STATUS = 0xFF; //溢出
                break;
          } 
             
          if (M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt] == ':')
          {
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'I' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'R' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'D' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'I' )
                    {
                          M72D_Brust_STATUS = Receive_Gprs_Data ; //+QIRDI: 0,1,0 [GPRS 数据到达 AT+QINDI=1 ]
                          break;
                    }
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'C' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'M' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'T' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'I' )
                    {
                          M72D_Brust_STATUS = 0x02; //+CMTI:<mem>,<index> [短消息到达 AT+CNMI=2,1]
                          break;
                    }
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt -12] == 'N' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt -11] == 'O' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt -10] == 'T' &&
                    
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 8] == 'I' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 7] == 'N' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'R' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'D' )
                    {
                      M72D_Brust_STATUS = 0x04; //+CPIN: READY [SIM 状态检测 AT+CPIN?]
                      break;
                    }
                
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'C' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'R' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'E' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'G' )
                    {
                      M72D_Brust_STATUS = 0x08; //+CREG: 1    [GSM 网络状态 0 无网络 1正常 2正在搜索]
                      break;
                    }
                
                if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'C' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'F' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'U' && 
                     M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'N' )
                    {    
                      M72D_Brust_STATUS = 0x40; //+CFUN: 1 [全功能工作]
                      break;
                    }    
          }
          
          
        if ( M72D_Brust_RX_Buf[6] == 'C' && M72D_Brust_RX_Buf[4] == 'L' && 
             M72D_Brust_RX_Buf[2] == 'S' && M72D_Brust_RX_Buf[0] == 'Q')
            {
                if(M72D_RX_Buf_Cnt>58)
                {
                  M72D_Brust_STATUS = GSM_Get_GPS ; //
                  for(M72D_Brust_Cnt=0;M72D_Brust_Cnt<40;M72D_Brust_Cnt++)                         //设备ID转存
                   {
                      GSM_GSP_BUF[M72D_Brust_Cnt]=M72D_Brust_RX_Buf[11+M72D_Brust_Cnt]        ;
                   }
                   M72D_Brust_RX_Buf[0]   =0x00     ;
                   M72D_Brust_RX_Buf[1]   =0x00     ;
                }
                break;
            }
          
        if ( M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 7] == 'P' && 
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 5] == 'D' && 
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'A' && 
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'C' &&
             M72D_Brust_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'T' )
            {
                 M72D_Brust_STATUS = 0x20; //+PDP DEACT [GPRS链路(TCPIP)断开 ]
                 break;
            } 
        break;
      }
    case PACKET_Query:
      {//查询接收
            M72D_Query_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_Query_STATUS = 0x01; //OK or SEND OK
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'E'
                    && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_Query_STATUS = 0xA0; //ERROR
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'Y')
                if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'D'
                    && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'A')
                    if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'E'
                        && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 5] == 'R')
                         M72D_Query_STATUS = 0xAA; //ready
            
            
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'G')
                if ((M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 8] == 'N')
                    && M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 7] == 'N')
                    M72D_Query_STATUS = 0xB0; //UDP Connectting
            
            if (M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'L')
                if ((M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'F')
                    &&(M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 6] == 'D')
                    &&(M72D_Query_RX_Buf[M72D_RX_Buf_Cnt - 9] == 'S'))
                {
                    M72D_Query_STATUS = 0xC0                         ;//SEND FAIL
                    GSM_STATUS |=  TCPIP_ERRO                        ;
                }
           
            
            if (M72D_RX_Buf_Cnt > M72D_Query_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_Query_STATUS = 0xFF; //溢出
            }
            break;
      }
    case PACKET_CONFIG:
      {//配置接收
            M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
            if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_SetConfig_STATUS = 0x01; //OK
            
            if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'E'
                    && M72D_SetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_SetConfig_STATUS = 0xA0; //ERROR
            
            if (M72D_RX_Buf_Cnt > M72D_SetConfig_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_SetConfig_STATUS = 0xFF; //溢出
            }
            break;
      }
        
    case PAKET_DATA:
      {  //数据接收
            M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
            
            if(Sys_NOW_UP_Flag==0x11)
                UP_Data_Cnt=1051;
            else
                UP_Data_Cnt=0;
            
            if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                  if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 3] == 0x0A)
                    if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 4] == 0x0D)
                    {
                      if(M72D_RX_Buf_Cnt>30+UP_Data_Cnt)
                        M72D_ServerData_STATUS = 0x01; //有数据
                      else
                        M72D_ServerData_STATUS = 0x02; //空数据
                    }
            if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'R' &&
                    M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O' )
                          if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'R' &&
                             M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 5] == 'E' )
                             if (M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 6] == 0x0A &&
                             M72D_ServerData_RX_Buf[M72D_RX_Buf_Cnt - 7] ==0x0D )
                               if(M72D_RX_Buf_Cnt>30+UP_Data_Cnt)
                                  M72D_ServerData_STATUS = 0xA0; //Error
            
            if (M72D_RX_Buf_Cnt > M72D_ServerData_RX_BUF_SIZE) 
            {
                M72D_RX_Buf_Cnt = 0;
                M72D_ServerData_STATUS = 0xFF; //溢出
            }
            break;
      }

    case PACKET_NETCONFIG:
      {  //网络接收
            M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
    
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
              if(M72D_RX_Buf_Cnt>4)
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'T' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_NetConfig_STATUS = 0x01; //CONNECT OK
            
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'L')
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'F' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'I')
                    M72D_NetConfig_STATUS = 0x02; //CONNECT FAIL
    
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'Y')
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'A' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'D')
                    M72D_NetConfig_STATUS = 0x03; //ALREADY CONNECT
            
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == '>')
                    M72D_NetConfig_STATUS = '>'; //等待数据发送
    
            if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 3] == 'E' && M72D_NetConfig_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_NetConfig_STATUS = 0xA0; //ERROR
           
            if (M72D_RX_Buf_Cnt > M72D_NetConfig_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_NetConfig_STATUS = 0xFF; //溢出
            }
    
            break;
      }
    case PCKET_OTHER:
      { //未知包接收
            M72D_Other_RX_Buf[M72D_RX_Buf_Cnt++] = UCA0RXBUF;
    
            if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'K')
                if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'O')
                    M72D_Other_STATUS = 0x01; //OK
    
            if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 1] == 'R')
                if (M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 4] == 'E'
                    && M72D_Other_RX_Buf[M72D_RX_Buf_Cnt - 2] == 'R')
                    M72D_Other_STATUS = 0xA0; //ERROR
    
            if (M72D_RX_Buf_Cnt > M72D_Other_RX_BUF_SIZE) {
                M72D_RX_Buf_Cnt = 0;
                M72D_Other_STATUS = 0xFF; //溢出
            }
    
            break;
      }

    default:
        break;
    }
}


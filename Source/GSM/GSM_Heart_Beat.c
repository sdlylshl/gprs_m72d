
#include "msp430x54x.h"
#include "GSM.h"

unsigned long int Heart_Beat_Send_Cnt                          ;//心跳发送次数计数
unsigned long int HeartBeat_TimeOut=MSP_A0_Min_1               ;//配置心跳包发送时间  61425
unsigned long int Heart_Beat_Count=0                           ;//发送心跳1ms时间计数
unsigned long int Heart_Beat_Num=0                             ;//心跳包计数  
char Heart_Beat_SendERROR                                      ;//发送失败标志
char Heart_Beat[10]                                            ;//心跳数据包

/*******************************************************************\
*	      函数名：GSM_Heart_Beat             
*	      作用域：外部文件调用
*	      功能：   PING命令：
*	      参数：  该命令的说明： GPS终端登录网络后，移动网络分配给
          GPS终端的IP地址和端口号是动态的，GPS终端与服务器之间采用
          UDP传输方式，如果GPS超过一分钟不向服务器发送数据，移动网
          络将会收回GPS终端当前的IP和端口号，等GPS终端下一次再发送
          数据时，会获取新的IP和端口号。所以为了维持网络链路，GPS终
          端登录网络后，每一分钟发送一次PING命令至服务器。
          格式： 
*	      返回值：  该命令不需要服务器回复
*
*	      修改历史：（每条详述）
\*******************************************************************/

char GSM_Heart_Beat()
{
    unsigned long int Heart_Beat_Cnt                           ;//1ms时间计数    
    if((Heart_Beat_Count>HeartBeat_TimeOut)&&(UDP_Built_flag==0x11))
    {
        Heart_Beat_Count=0                                     ;//发送心跳1ms时间计数清零
        Heart_Beat_Num++                                       ;//心跳累加计数
        Heart_Beat_Send_Cnt++;
        Heart_Beat_SendERROR=1                                 ;//置位发送失败标志
       
        //UP_SPI_Num_Flag=0x11;
        Heart_Beat_Cnt=Heart_Beat_Num                          ;//心跳计数赋值
        Heart_Beat[9]=Heart_Beat_Cnt                           ;
        Heart_Beat_Cnt= Heart_Beat_Cnt >>8                     ;
        Heart_Beat[8]=Heart_Beat_Cnt                           ;
        Heart_Beat_Cnt= Heart_Beat_Cnt >>8                     ;
        Heart_Beat[7]=Heart_Beat_Cnt                           ;
        Heart_Beat_Cnt= Heart_Beat_Cnt >>8                     ;
        Heart_Beat[6]=Heart_Beat_Cnt                           ;
            
        if(GSM_SendData(Heart_Beat,sizeof(Heart_Beat)))         //发送心跳数据
        {
           THR_Mint_Time_Cnt    =0                             ;//3.5分钟时间计数，用于不上传数据检测
           Heart_Beat_SendERROR=0                              ;
           return 1                                            ;
        }  
    }
    
    if((Heart_Beat_SendERROR)&&(UDP_Built_flag==0x11)
       &&(Heart_Beat_Count<5119)&&(Heart_Beat_Count>1023))     //1-5S失败重发
    {
        Delayms(200);//XX*1Ms延时
        if(GSM_SendData(Heart_Beat,sizeof(Heart_Beat)))
        {
            THR_Mint_Time_Cnt   =0                             ;//3.5分钟时间计数，用于不上传数据检测
            Heart_Beat_SendERROR=0                             ;
            return 1                                           ;
        }
        Heart_Beat_SendERROR=0                                 ;
    }
    
  return 0                                                     ;
}


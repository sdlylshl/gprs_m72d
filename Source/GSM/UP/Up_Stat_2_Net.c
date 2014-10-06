#include "msp430x54x.h"
#include "Root_Up.h"

#define ANS_Up_Stat_Buf_Size 20

char ANS_Up_Stat_EROR_Flag                                          ;//锁车指令发送失败标志 重发
char ANS_Up_Stat_Buf[20]  ;//回复平台升级启动指令数组

/*******************************************************************\
*	      函数名：ANS_UP_STAT_2_NET             
*	      作用域：外部文件调用
*	      功能：  回复升级启动指令平台数据  
*	      参数： 0x01==升级；0x00==不升级 
          格式： ID(4)+命令编码0x69(2)+锁车指令(2)
*	      返回值：返回升级启动指令状态 
*
*	      修改历史：（每条详述）
\*******************************************************************/
char ANS_UP_STAT_2_NET(void)                                         //回复升级启动指令平台数据
{
    unsigned int ANS_UP_CRC                                         ;
      
    ANS_Up_Stat_EROR_Flag    =1                                     ;
 
    Tran_ID_CmdNum(ANS_Up_Stat_Buf,0x1A)                   ;//转存ID号和命令编码  
    
    ANS_Up_Stat_Buf[6]= 0x00  ;
    ANS_Up_Stat_Buf[7]= 0x14  ;
    
    ANS_Up_Stat_Buf[9]  =   0x01                                    ;//升级标志00==不升级,01==升级
    
    ANS_UP_CRC=crc_modbus2((unsigned char *)ANS_Up_Stat_Buf,(18))   ;
    ANS_Up_Stat_Buf[18]    =   (char)(ANS_UP_CRC&0xFF)              ;
    ANS_Up_Stat_Buf[19]    =   (char)((ANS_UP_CRC>>8)&0xFF)         ;
   
    if(GSM_SendData(ANS_Up_Stat_Buf,ANS_Up_Stat_Buf_Size))           //数据发送
    {
       ANS_Up_Stat_EROR_Flag    =0                                  ;//修改标志位
       return 1                                                     ;
    }
    
    if(ANS_Up_Stat_EROR_Flag)                                        //失败重发
    {
       Delayms(50)                                                  ;//XX*1Ms延时
       if(GSM_SendData(ANS_Up_Stat_Buf,ANS_Up_Stat_Buf_Size))
       {          
          ANS_Up_Stat_EROR_Flag    =    0                           ;
          return 1                                                  ;
       }
       ANS_Up_Stat_EROR_Flag    =    0                              ;
    }
    return 0                                                        ;
}















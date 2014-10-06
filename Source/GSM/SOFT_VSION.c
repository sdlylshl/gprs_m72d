#include "msp430x54x.h"
#include "GSM.h"

char SD_SOFT_V_ER_Flag                                              ;//发送软件版本号失败标志 重发

#define  SOFT_VER_BUF_SIZE 40
char  SOFT_VER_BUF[SOFT_VER_BUF_SIZE];//发送软件版本号
unsigned long int Soft_Vers_Cnt                                     ;//发送软件版本号时间计时
extern unsigned char Soft_VER_OK                                    ;//软件版本号(显示器，控制器，油门电机软件版本号)
extern unsigned char Soft_VER_memory[32]                                     ;//存储软件版本号

/*******************************************************************\
*	      函数名：Send_Soft_Version             
*	      作用域：外部文件调用
*	      功能：  发送软件版本号 
*	      参数：  显示器，控制器，油门电机，一体机软件版本号 
          格式：  开机发送心跳，5分钟后发送一次
*	      返回值：   
*
*	      修改历史：（每条详述）
\*****************************************************************/
char Send_Soft_Version()
{
    unsigned char Vers_Num                                          ;
    unsigned int CAL_SOFT_VER_CRC                                   ;//校验和
    
    if((MSP430_POWER_ON_FLAG==0x11)&&((Soft_VER_OK==0x11)||(SD_SOFT_V_ER_Flag==1))
       &&(Soft_Vers_Cnt>MSP_A0_Min_5)&&(UDP_Built_flag==0x11))
    {
       Soft_Vers_Cnt    =0                                          ;
       Soft_VER_OK      =0xAA                                       ;

       Tran_ID_CmdNum(SOFT_VER_BUF,0x89)                            ;//转存ID号和命令编码
       
       for(Vers_Num=0;Vers_Num<32;Vers_Num++)                        //版本号转存
       {
          SOFT_VER_BUF[Vers_Num+6]=Soft_VER_memory[Vers_Num]        ;
       }
       
       
       CAL_SOFT_VER_CRC=crc_modbus2((unsigned char *)SOFT_VER_BUF,
                                    (SOFT_VER_BUF_SIZE-2))          ;//38
       SOFT_VER_BUF[38]    =   (char)(CAL_SOFT_VER_CRC&0xFF)        ;
       SOFT_VER_BUF[39]    =   (char)((CAL_SOFT_VER_CRC>>8)&0xFF)   ;
       
       SD_SOFT_V_ER_Flag    =   1                                   ;
       if(GSM_SendData(SOFT_VER_BUF,SOFT_VER_BUF_SIZE))
        {                                                            //发送软件版本号
           SD_SOFT_V_ER_Flag   =0                                   ;
           return 1                                                 ;
        }
    }
    if(SD_SOFT_V_ER_Flag)                                    
    {
        Delayms(200);
        SD_SOFT_V_ER_Flag   =0                                    ;
       if(GSM_SendData(SOFT_VER_BUF,SOFT_VER_BUF_SIZE))
       {          
          SD_SOFT_V_ER_Flag   =0                                    ;
          return 1                                                  ;
       }
    }
    return 0                                                        ;
}




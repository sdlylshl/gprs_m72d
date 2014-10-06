#include "msp430x54x.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"
#include ".\GSM\UP\Root_Up.h"

char Open_Box_Flag;
extern void Time_Count_Value_Init(void)                          ;//时间计数清零
/**********************************************************************\
*	      函数名：LPM_GSM_Globl_Init             
*	      作用域：外部文件调用
*	      功能：  处理IP地址\Port号  
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+IP地址(4)+Port号(2)
*	      返回值：返回IP地址\Port号   
*
*	      修改历史：（每条详述）
\**********************************************************************/
void LPM_GSM_Globl_IN(void)
{
    MCU_Data_TimeOut    =   SECD_30;
    MCU_Data_Count      =   0;
	
    HeartBeat_TimeOut   =   20480 ;//20S
    
    THR_Mint_Time_Cnt   =   0;
    CONNECT_FAIL_Flag   =   0;
    RD_Data_1_Min_Cnt   =   0;//解析 MUC 应答时间计数
    Soft_Vers_Cnt       =   0;//发送软件版本号时间计时
    Heart_Beat_Send_Cnt =   0;//心跳发送次数计数
    Time_Count_Value_Init();
    Sys_NOW_UP_Flag=0x66;//低功耗模式
    CREG_Zhu_Ce_Cnt=0 ;//注册网络查询注册次数
    One_JI_Warn_Cnt=0;//一级报警计数
    Two_JI_Warn_Cnt=0;//二级报警计数
    GSM_INIT_TIME_CNT=SECD_10;//GSM初始化计时用
}






/**********************************************************************\
*	      函数名：LPM_GSM_Globl_Init             
*	      作用域：外部文件调用
*	      功能：  处理IP地址\Port号  
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+IP地址(4)+Port号(2)
*	      返回值：返回IP地址\Port号   
*
*	      修改历史：（每条详述）
\**********************************************************************/
void LPM_GSM_Globl_OUT(void)
{
    RD_MCU_TIME_FLASH();        
    MCU_Data_TimeOut    =   MSP_A0_Min_2            ;
    MCU_ACK_TimeOut     =   MSP_A0_Min_1          ;
    HeartBeat_TimeOut   =   MSP_A0_Min_1            ;
    UDP_Built_flag      =   0x00                    ;//11==网络建立，00==网络断开或者没有建立
    Heart_Beat_Send_Cnt =0                          ;//心跳发送次数计数
    Time_Count_Value_Init();
    Sys_NOW_UP_Flag=0x55;//未升级
    One_JI_Warn_Cnt=0;//一级报警计数
    Two_JI_Warn_Cnt=0;//二级报警计数
    CREG_Zhu_Ce_Cnt=0 ;//注册网络查询注册次数
    Module_Status[0] &=~0xC0;
    GPS_GSM_System_Stu[0]&=~0xC0;
    GSM_INIT_TIME_CNT=0;//GSM初始化计时用
}







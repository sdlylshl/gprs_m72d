
#include "msp430x54x.h"
#include "GSM.h"

unsigned char CALL_MCU_TIME_FLAG=0x00                                ;//呼叫GPS终端上报定位数据命令标志位 11==发送MCU
unsigned long int CALL_MCU_TIME_CNT                                  ;//时间计数

#define  CALL_MCU_BUF_SIZE 12
char  REC_CALL_MCU_BUF[CALL_MCU_BUF_SIZE]                            ;//接收MCU数据上传时间间隔设置
char CALL_MCU_CRC(void)                                              ;//回复上报定位数据命令
/*******************************************************************\
*	      函数名：Call_MCU_Data             
*	      作用域：外部文件调用
*	      功能：  呼叫GPS终端上报定位数据命令
*	      参数：  
          格式：  
*	      返回值：   
*
*	      修改历史：（每条详述）
\*******************************************************************/
char Call_MCU_Data(void)                                             //呼叫GPS终端上报定位数据命令
{
    if((CALL_MCU_TIME_FLAG==0x11)&&(CALL_MCU_TIME_CNT>SECD_2))
    {   
        CALL_MCU_TIME_CNT   =   0                                   ;//时间计数
        MCU_Data_Count  =   MCU_Data_TimeOut    +   1               ;
        Heart_Beat_Count=   10240                                   ;
        if(GSM_Send_MCU())                                             
        {
            CALL_MCU_TIME_FLAG   =  0x00                            ;
        }
        return 1                                                    ;
    }
    return 0                                                        ;
}



char SD_CALL_MCU_FLAG                                               ;//指令发送失败标志 重发
char  ANS_CALL_MCU_BUF[CALL_MCU_BUF_SIZE];//MCU数据上传时间间隔设置回复 Sd_MCU_Time
/*******************************************************************\
*	      函数名：ANSW_CALL_MCU             
*	      作用域：外部文件调用
*	      功能：  呼叫GPS终端上报定位数据命令
*	      参数：  
          格式：  
*	      返回值：   
*
*	      修改历史：（每条详述）
\*******************************************************************/
char ANSW_CALL_MCU(void)                                            //回复上报定位数据命令
{
    unsigned char CAL_Num                                               ;//计数用
    unsigned int CAL_MCU_CRC                                            ;//呼叫GPS终端上报定位数据命令 校验和
    if(CALL_MCU_CRC()==1)  
    {
        Tran_ID_CmdNum(ANS_CALL_MCU_BUF,0x9A)                   ;//转存ID号和命令编码 
        
        for(CAL_Num=0;CAL_Num<4;CAL_Num++)                               //回复呼叫间隔、呼叫时长
        {            
            ANS_CALL_MCU_BUF[6+CAL_Num]=REC_CALL_MCU_BUF[6+CAL_Num]     ;      
        } 
        
        CAL_MCU_CRC=crc_modbus2((unsigned char *)ANS_CALL_MCU_BUF,
                                    (CALL_MCU_BUF_SIZE-2))              ;//10
        ANS_CALL_MCU_BUF[10]    =   (char)(CAL_MCU_CRC&0xFF)            ;
        ANS_CALL_MCU_BUF[11]    =   (char)((CAL_MCU_CRC>>8)&0xFF)       ;
        
        SD_CALL_MCU_FLAG  = 1                                           ;
        if(GSM_SendData(ANS_CALL_MCU_BUF,CALL_MCU_BUF_SIZE))             //数据发送   
        {
           SD_CALL_MCU_FLAG  =0                                         ;
           return 1                                                     ;
        }
        
        if(SD_CALL_MCU_FLAG)                                             //失败重发
        {
           Delayms(200);//XX*1Ms延时
           SD_CALL_MCU_FLAG  = 0;
           if(GSM_SendData(ANS_CALL_MCU_BUF,CALL_MCU_BUF_SIZE))         
           {          
              SD_CALL_MCU_FLAG  = 0                                     ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;  
}






/*******************************************************************\
*	      函数名：CALL_MCU_CRC             
*	      作用域：外部文件调用
*	      功能：  呼叫GPS终端上报定位数据命令
*	      参数：  
          格式：  
*	      返回值：   
*
*	      修改历史：（每条详述）
\*******************************************************************/
char CALL_MCU_CRC(void)                                            //回复上报定位数据命令
{
  
    unsigned int CAL_CALL_MCU_CRC,NET_CALL_MCU_CRC                        ;//呼叫GPS终端上报定位数据命令 校验和
    
    NET_CALL_MCU_CRC=   REC_CALL_MCU_BUF[11]                              ;
    NET_CALL_MCU_CRC =   NET_CALL_MCU_CRC  <<8                            ;
    NET_CALL_MCU_CRC =   NET_CALL_MCU_CRC +   REC_CALL_MCU_BUF[10]        ;
    CAL_CALL_MCU_CRC=crc_modbus2((unsigned char *)REC_CALL_MCU_BUF,
                            CALL_MCU_BUF_SIZE-2)                          ;//10
    if(CAL_CALL_MCU_CRC==NET_CALL_MCU_CRC)
    {
        CALL_MCU_TIME_CNT   =   0                                         ;
        CALL_MCU_TIME_FLAG  =   0x11                                      ;// 11==发送MCU
        return 1                                                          ;
    }
    return 0                                                              ;  
}






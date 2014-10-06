
#include "msp430x54x.h"
#include "GSM.h"

char  Low_Pow_Time_Send_ERR_Flag;//低功耗时间间隔设置指令发送失败标志 重发
char* Low_Pow_Time_Addr    =  (char *)0x184A;//低功耗时间间隔设置标志地址

#define  Low_Pow_Time_BUF_SIZE 20
char REC_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE];//接收低功耗时间间隔设置
char ANS_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE];//回复低功耗时间间隔设置

extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//转存ID号和命令编码                              
extern void Delayms(unsigned long int ms);//延时XX*1Ms
/*******************************************************************\
*	      函数名：Set_LowPower_ModeTime             
*	      作用域：外部文件调用
*	      功能：  低功耗时间间隔设置
*	      参数： 
          格式： ID(4)+命令编码0x69(2)+锁车指令(2)
*	      返回值：返回锁车、解锁命令   

          00184A        74      低功耗时间间隔设置标志
          00184B~001856 ==75\86  低功耗时间间隔设置
*
*	      修改历史：（每条详述）
\*******************************************************************/
char Low_Power_Mode_Time(void)//低功耗时间间隔设置
{
    unsigned char   LP_CNT;
    unsigned int CAL_Low_Pow_Time_CRC,NET_Low_Pow_Time_CRC;//校验和
      
    NET_Low_Pow_Time_CRC =   REC_Low_Pow_Time_BUF[9]                                ;
    NET_Low_Pow_Time_CRC =   NET_Low_Pow_Time_CRC  <<8                              ;
    NET_Low_Pow_Time_CRC =   NET_Low_Pow_Time_CRC +   REC_Low_Pow_Time_BUF[8]             ;
    CAL_Low_Pow_Time_CRC =crc_modbus2((unsigned char *)REC_Low_Pow_Time_BUF,
                                Low_Pow_Time_BUF_SIZE-2)                  ;//8
    if(CAL_Low_Pow_Time_CRC==NET_Low_Pow_Time_CRC)
    {
        Tran_ID_CmdNum(ANS_Low_Pow_Time_BUF,0x64)                            ;//转存ID号和命令编码

        Flash_WriteChar(74,0xAA)                                        ;//写标志位
        for(LP_CNT=0;LP_CNT<12;LP_CNT++)                                 //写IP及端口号
        {  
           _NOP();_NOP();_NOP();_NOP();_NOP();          
           Flash_WriteChar(75+LP_CNT,REC_Low_Pow_Time_BUF[6+LP_CNT])    ;      
        }  
        _NOP();_NOP();_NOP();_NOP();_NOP();
        
        
        for(LP_CNT=0;LP_CNT<12;LP_CNT++)                                 //写IP及端口号
        {  
           ANS_Low_Pow_Time_BUF[6+LP_CNT] = * (Low_Pow_Time_Addr+1+LP_CNT);//6-17  
           _NOP();_NOP();
        }  
    
        CAL_Low_Pow_Time_CRC=crc_modbus2((unsigned char *)ANS_Low_Pow_Time_BUF,
                                    (Low_Pow_Time_BUF_SIZE-2))          ;//18
        ANS_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE-2]=(char)(CAL_Low_Pow_Time_CRC&0xFF);
        ANS_Low_Pow_Time_BUF[Low_Pow_Time_BUF_SIZE-1]=(char)((CAL_Low_Pow_Time_CRC>>8)&0xFF);//19
    
        Low_Pow_Time_Send_ERR_Flag   =    1                                 ;
        if(GSM_SendData(ANS_Low_Pow_Time_BUF,Low_Pow_Time_BUF_SIZE))             //数据发送
        {
           Low_Pow_Time_Send_ERR_Flag    =   0                              ;
           return 1                                                     ;
        }
    
        if(Low_Pow_Time_Send_ERR_Flag)                                           //失败重发 
        {
           Delayms(200);//XX*1Ms延时
           Low_Pow_Time_Send_ERR_Flag     =   0;
           if(GSM_SendData(ANS_Low_Pow_Time_BUF,Low_Pow_Time_BUF_SIZE))
           {          
              Low_Pow_Time_Send_ERR_Flag     =   0                          ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;
}





/**********************************************************************\
*	      函数名：RD_Up_Speed_FLASH             
*	      作用域：外部文件调用
*	      功能：  读取处理允许换卡 无卡升速FLASH
*	      参数：  
          格式： 
*	      返回值：
*
*	      修改历史：
\**********************************************************************/
void RD_Low_Pow_Time_FLASH(void)//处理允许换卡 无卡升速
{
    if((* Low_Pow_Time_Addr==0xAA))
    {

    }
}


/**********************************************************************\
*	      函数名：RD_Up_Speed_FLASH             
*	      作用域：外部文件调用
*	      功能：  读取处低功耗时间间隔设置FLASH
*	      参数：  
          格式： 
*	      返回值：
*
*	      修改历史：
\**********************************************************************/
void Ceshi_Low_Pow_Time(void)//测试低功耗时间间隔设置
{
    unsigned char   LK_CNT;
    unsigned int CAL_Up_Speed_CRC;//校验和
    
    
    for(LK_CNT=0;LK_CNT<4;LK_CNT++)                         //设备ID转存
    {
       REC_Low_Pow_Time_BUF[LK_CNT]=GPS_GSM_ID_Memory[LK_CNT]        ;
    }

    REC_Low_Pow_Time_BUF[4]=0x54;
    REC_Low_Pow_Time_BUF[5]=0x54;
    

    CAL_Up_Speed_CRC=crc_modbus2((unsigned char *)REC_Low_Pow_Time_BUF,
                                (Low_Pow_Time_BUF_SIZE-2))          ;//8
    REC_Low_Pow_Time_BUF[8]    =   (char)(CAL_Up_Speed_CRC&0xFF)        ;
    REC_Low_Pow_Time_BUF[9]    =   (char)((CAL_Up_Speed_CRC>>8)&0xFF)   ;
}













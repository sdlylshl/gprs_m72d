#include "msp430x54x.h"
#include "GSM.h"

char LOOK_M_SD_ER_Flag                                              ;//锁车指令发送失败标志 重发

#define  NET_LOOK_M_BUF_SIZE 10
char REC_LOOK_M_BUF[NET_LOOK_M_BUF_SIZE]                              ;//接收 GPS终端锁车/解锁命令
char ANS_LOOK_M_BUF[NET_LOOK_M_BUF_SIZE];//GPS终端锁车/解锁命令 回复

extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//转存ID号和命令编码                              
extern void Delayms(unsigned long int ms)                           ;//XX*1Ms延时
/*******************************************************************\
*	      函数名：GSM_Lock_Monitor             
*	      作用域：外部文件调用
*	      功能：  接收、处理平台数据  
*	      参数： 0x03==锁车；0x00==解锁 
          格式： ID(4)+命令编码0x69(2)+锁车指令(2)
*	      返回值：返回锁车、解锁命令    
*
*	      修改历史：（每条详述）
\*******************************************************************/
char GSM_Lock_Monitor()
{
    unsigned char   LK_CNT                                              ;
    
    unsigned int CAL_LOOK_M_CRC,NET_LOOK_M_CRC                          ;//校验和
      
    NET_LOOK_M_CRC =   REC_LOOK_M_BUF[9]                                ;
    NET_LOOK_M_CRC =   NET_LOOK_M_CRC  <<8                              ;
    NET_LOOK_M_CRC =   NET_LOOK_M_CRC +   REC_LOOK_M_BUF[8]             ;
    CAL_LOOK_M_CRC =crc_modbus2((unsigned char *)REC_LOOK_M_BUF,
                                NET_LOOK_M_BUF_SIZE-2)                  ;//8
    if(CAL_LOOK_M_CRC==NET_LOOK_M_CRC)
    {
        Tran_ID_CmdNum(ANS_LOOK_M_BUF,0x96)                            ;//转存ID号和命令编码
        if((REC_LOOK_M_BUF[6]&REC_LOOK_M_BUF[7])==0x03)
        {
            Module_Status[2] |=0x03                                     ;//1级锁车（限制启动）锁车
            GPS_GSM_System_Stu[2]|=0x03                                 ;
            ANS_LOOK_M_BUF[6]=0x03                                      ;
            ANS_LOOK_M_BUF[7]=0x03                                      ;
            
            for(LK_CNT=0;LK_CNT<2;LK_CNT++)
            { 
                Flash_WriteChar(5+LK_CNT,0x03)                          ;
                _NOP();_NOP();_NOP();_NOP();_NOP();
            }
            
        }
        
        if((REC_LOOK_M_BUF[6]&REC_LOOK_M_BUF[7])==0x0C)
        {
            Module_Status[3] |=0x0C                                     ;//2级锁车（限制功率）锁车
            GPS_GSM_System_Stu[3]|=0x0C                                 ;
            ANS_LOOK_M_BUF[6]=0x0C                                      ;
            ANS_LOOK_M_BUF[7]=0x0C                                      ;
            
            for(LK_CNT=0;LK_CNT<2;LK_CNT++)
            { 
                Flash_WriteChar(5+LK_CNT,0x0C)                          ;
                _NOP();_NOP();_NOP();_NOP();_NOP();
            }
        }
        
        if((REC_LOOK_M_BUF[6]&REC_LOOK_M_BUF[7])==0x00)
        {
            Module_Status[2] &= ~0x03                                   ;//限制启动\限制功率解锁
            GPS_GSM_System_Stu[2]&= ~0x03                               ; 
            Module_Status[3] &= ~0x0C                                   ;
            GPS_GSM_System_Stu[3]&= ~0x0C                               ;
            ANS_LOOK_M_BUF[6]=0x00                                      ;
            ANS_LOOK_M_BUF[7]=0x00                                      ;
            
            for(LK_CNT=0;LK_CNT<2;LK_CNT++)
            { 
                Flash_WriteChar(5+LK_CNT,0x00)                          ;
                _NOP();_NOP();_NOP();_NOP();_NOP();
            }
        }    

        CAL_LOOK_M_CRC=crc_modbus2((unsigned char *)ANS_LOOK_M_BUF,
                                    (NET_LOOK_M_BUF_SIZE-2))            ;//8
        ANS_LOOK_M_BUF[8]    =   (char)(CAL_LOOK_M_CRC&0xFF)            ;
        ANS_LOOK_M_BUF[9]    =   (char)((CAL_LOOK_M_CRC>>8)&0xFF)       ;
    
        LOOK_M_SD_ER_Flag   =    1                                      ;
        if(GSM_SendData(ANS_LOOK_M_BUF,NET_LOOK_M_BUF_SIZE))             //数据发送
        {
           LOOK_M_SD_ER_Flag    =   0                                   ;
           return 1                                                     ;
        }
    
        if(LOOK_M_SD_ER_Flag)                                           //失败重发 
        {
           Delayms(200);//XX*1Ms延时
           LOOK_M_SD_ER_Flag     =   0;
           if(GSM_SendData(ANS_LOOK_M_BUF,NET_LOOK_M_BUF_SIZE))
           {          
              LOOK_M_SD_ER_Flag     =   0                               ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;
}



/**********************************************************************\
*	      函数名：RD_LOCK_BUS_FLASH             
*	      作用域：外部文件调用
*	      功能：  读取锁车状态
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+锁车状态(2)+CRC(2)
*	      返回值：
*
*	      修改历史：2013.10.22 -蒋本庆
\**********************************************************************/
void RD_LOCK_BUS_FLASH(void)
{
    char* LK_BS_Addr    =  (char *)0x1805                           ;//锁车首地址
    
    
    if((*(LK_BS_Addr) & *(LK_BS_Addr+1))==0x03)
    {
        Module_Status[2]   |=    0x03                               ;//锁车
        GPS_GSM_System_Stu[2]  |=    0x03                           ;
    }
    
    if((*(LK_BS_Addr) & *(LK_BS_Addr+1))==0x0C)
    {
        Module_Status[3] |=0x0C                                     ;//2级锁车（限制功率）锁车
        GPS_GSM_System_Stu[3]|=0x0C                                 ;
    }
    
    if((*(LK_BS_Addr) & *(LK_BS_Addr+1))==0x00)
    {
        Module_Status[2] &= ~0x03                                   ;//限制启动\限制功率解锁
        GPS_GSM_System_Stu[2]&= ~0x03                               ; 
        Module_Status[3] &= ~0x0C                                   ;
        GPS_GSM_System_Stu[3]&= ~0x0C                               ;
    }

}

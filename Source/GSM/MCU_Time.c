
#include "msp430x54x.h"
#include "GSM.h"

unsigned long int MCU_Time_Set_Flash                                    ;//MCU数据上传时间

#define  MCU_TIME_BUF_SZ 12
char  ANS_MCU_TIME_BUF[MCU_TIME_BUF_SZ];//MCU数据上传时间间隔设置回复 Sd_MCU_Time
char  REC_MCU_TIME_BUF[MCU_TIME_BUF_SZ]                                 ;//接收MCU数据上传时间间隔设置

char  MCU_Time_Set_EROR_Flag                                            ;//指令发送失败标志 重发
/**********************************************************************\
*	      函数名：MCU_Time_Set             
*	      作用域：外部文件调用
*	      功能：  MCU数据上传时间间隔设置  
*	      参数：  
          格式：  ID(4)+命令编码0x87(2)+时间(4)+CRC(2)
*	      返回值：MCU_Time   
*
*	      修改历史：（每条详述）
\**********************************************************************/
char MCU_Time_Set()                                                     //MCU数据上传时间间隔设置 
{
    unsigned char TM_SET                                       ;
    unsigned int CAL_MCU_CRC_SUM,NET_MCU_CRC_SUM                        ;//越界报警命令设置 校验和
    char* MCU_Time                                                      ;
    
    NET_MCU_CRC_SUM =   REC_MCU_TIME_BUF[11]                            ;
    NET_MCU_CRC_SUM =   NET_MCU_CRC_SUM  <<8                            ;
    NET_MCU_CRC_SUM =   NET_MCU_CRC_SUM +   REC_MCU_TIME_BUF[10]        ;
    CAL_MCU_CRC_SUM=crc_modbus2((unsigned char *)REC_MCU_TIME_BUF,
                            MCU_TIME_BUF_SZ-2)                          ;//10
    if(CAL_MCU_CRC_SUM==NET_MCU_CRC_SUM)
    {
        Tran_ID_CmdNum(ANS_MCU_TIME_BUF,0x32)                            ;//转存ID号和命令编码

        MCU_Time_Set_Flash    =      REC_MCU_TIME_BUF[6]                ;
        for(TM_SET=0;TM_SET<3;TM_SET++)
        {
            MCU_Time_Set_Flash=MCU_Time_Set_Flash<<8                    ;
            MCU_Time_Set_Flash=MCU_Time_Set_Flash+REC_MCU_TIME_BUF[7+TM_SET];
        }
        MCU_Time_Set_Flash=(MCU_Time_Set_Flash/1000)<<10 ;
        
        if((MCU_Time_Set_Flash>MSP_A0_Min_5)&&(MCU_Time_Set_Flash<MSP_Hour_30))//设置时间允许为2Min--30H
        {
            MCU_TimeOut =   MCU_Time_Set_Flash                          ;//修改当前MCU上传时间
            MCU_Data_TimeOut      =   MCU_TimeOut;//配置数据包发送时间
            Flash_WriteChar(7,0x11)                                     ;//写标志位
            _NOP();_NOP();_NOP();_NOP();_NOP();
            Flash_WriteChar(11,MCU_Time_Set_Flash)                      ;
            for(TM_SET=0;TM_SET<3;TM_SET++)                        
            {   
                _NOP();_NOP();_NOP();_NOP();_NOP();
                MCU_Time_Set_Flash=MCU_Time_Set_Flash   >>  8           ;
                Flash_WriteChar((10-TM_SET),MCU_Time_Set_Flash)         ;
            }        
            _NOP();_NOP();_NOP();_NOP();_NOP();            
            MCU_Time    = (char *)0x1808                                ;//读取上传MCU时间
            for(TM_SET=0;TM_SET<4;TM_SET++)                        
            {            
                ANS_MCU_TIME_BUF[6+TM_SET]=*(MCU_Time+TM_SET)           ;
            }
        }
        else
        {
            MCU_Data_TimeOut      =   MCU_TimeOut;//配置数据包发送时间
            MCU_Time_Set_Flash    =      MCU_TimeOut                    ;//上传MCU时间失败
            ANS_MCU_TIME_BUF[9]   =   MCU_Time_Set_Flash                ;
            for(TM_SET=0;TM_SET<3;TM_SET++)
            {
                MCU_Time_Set_Flash          =MCU_Time_Set_Flash>>8      ;
                ANS_MCU_TIME_BUF[8-TM_SET]  =MCU_Time_Set_Flash;
            }
        }
        
        CAL_MCU_CRC_SUM=crc_modbus2((unsigned char *)ANS_MCU_TIME_BUF,
                                    (MCU_TIME_BUF_SZ-2))                ;//10
        ANS_MCU_TIME_BUF[10]    =   (char)(CAL_MCU_CRC_SUM&0xFF)        ;
        ANS_MCU_TIME_BUF[11]    =   (char)((CAL_MCU_CRC_SUM>>8)&0xFF)   ;
        
        MCU_Time_Set_EROR_Flag=1                                        ;
        if(GSM_SendData(ANS_MCU_TIME_BUF,MCU_TIME_BUF_SZ))                   //数据发送
        {
           MCU_Time_Set_EROR_Flag=0                                     ;
           return 1                                                     ;
        }
        if(MCU_Time_Set_EROR_Flag)                                       //失败重发 
        {
           Delayms(200);
           MCU_Time_Set_EROR_Flag=0;
           if(GSM_SendData(ANS_MCU_TIME_BUF,MCU_TIME_BUF_SZ))         
           {          
              MCU_Time_Set_EROR_Flag=0                                  ;
              return 1                                                  ;
           }
           MCU_Time_Set_EROR_Flag=0                                     ;
        }
    }
    return 0                                                            ;  
}





/**********************************************************************\
*	      函数名：RD_MCU_TIME_FLASH             
*	      作用域：外部文件调用
*	      功能：  读取时间设置FLASH
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+时间(4)+CRC(2)
*	      返回值：
*
*	      修改历史：（每条详述）
\**********************************************************************/
void RD_MCU_TIME_FLASH(void)
{
    unsigned int RD_MCU_NUM                                             ;
    char* TM_ST_AD_FG   =  (char *)0x1807                               ;//时间设置标志
    char* TM_DD_Addr    =  (char *)0x1808                               ;//锁车首地址
    
    if(*(TM_ST_AD_FG)==0x11)
    {
        MCU_TimeOut =   *(TM_DD_Addr   )                                ;//数据包发送时间 
        for(RD_MCU_NUM=0;RD_MCU_NUM<4;RD_MCU_NUM++)                            
        {   
            MCU_TimeOut = MCU_TimeOut <<  8                             ;
            MCU_TimeOut = MCU_TimeOut+*(TM_DD_Addr+RD_MCU_NUM)        ; 
        }
        
        MCU_Data_TimeOut    =   MCU_TimeOut;//配置数据包发送时间
    }

}
















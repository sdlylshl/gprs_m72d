

#include "Root_Up.h"

#define UP_NET_Bit_Sum 1051

unsigned long int NET_UP_BIT_SUM                                    ;//累计已接收文件字节数
unsigned int      SPI_UP_BIT_SUM                                    ;//累计已接收文件字节数
unsigned char UP_Data_OK_Flag                               ;//
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
char UP_A2_NUM_CRC(void)                                            //升级程序CRC校验
{
    unsigned int UP_A2_CRC,UP_ROOT_A2_CRC                           ;//计算升级开始校验和
    unsigned int UP_A2_CNT                                          ;    
    static unsigned int UP_A2_CMD_LEN                               ;
    
    UP_A2_CMD_LEN   =   UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-2]                         ;//解析本包数据长度
    UP_A2_CMD_LEN   =   UP_A2_CMD_LEN  <<8                          ;
    UP_A2_CMD_LEN   =   UP_A2_CMD_LEN +UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-1]       ;    
    
    UP_A2_CRC=UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-1]                     ;//解析本包数据CRC校验和
    UP_A2_CRC=UP_A2_CRC  <<8                                        ;
    UP_A2_CRC=UP_A2_CRC+UP_SYSTEM_C_LANG[UP_NET_Bit_Sum-2]           ;
    
    UP_ROOT_A2_CRC=crc_modbus2(UP_SYSTEM_C_LANG,UP_NET_Bit_Sum-2)    ;//计算数据CRC校验和
    
    if((UP_ROOT_A2_CRC==UP_A2_CRC)
          &&(UP_SYSTEM_C_LANG[0]==GPS_GSM_ID_Memory[0])
         &&(UP_SYSTEM_C_LANG[2]==GPS_GSM_ID_Memory[2]))
    {
        
        NET_UP_BIT_SUM=UP_SYSTEM_C_LANG[17];
        NET_UP_BIT_SUM=NET_UP_BIT_SUM<<8;
        NET_UP_BIT_SUM=NET_UP_BIT_SUM+UP_SYSTEM_C_LANG[18];
        NET_UP_BIT_SUM=NET_UP_BIT_SUM<<8;        
        NET_UP_BIT_SUM=NET_UP_BIT_SUM+UP_SYSTEM_C_LANG[19];
        NET_UP_BIT_SUM=NET_UP_BIT_SUM<<8;           
        NET_UP_BIT_SUM=NET_UP_BIT_SUM+UP_SYSTEM_C_LANG[20];
        
        SPI_UP_BIT_SUM=NET_UP_BIT_SUM>>10;
        
        for(UP_A2_CNT=23;UP_A2_CNT<UP_NET_Bit_Sum;UP_A2_CNT++)
        { 
           UP_SYSTEM_C_LANG[UP_A2_CNT-23]=UP_SYSTEM_C_LANG[UP_A2_CNT];
        }
        
        for(UP_A2_CNT=0;UP_A2_CNT<(UP_SYSTEM_C_SIZE-UP_NET_Bit_Sum);UP_A2_CNT++)
        { 
           UP_SYSTEM_C_LANG[UP_NET_Bit_Sum+UP_A2_CNT]=0x00;
        }
        
        Module_Status[5]|=0x0c;//清每包数据完成的标志  
        UP_Data_OK_Flag=0xAA;
        Sys_NOW_UP_Flag=0x55;
        
        return 1                                                    ;
    }
    else
    { 
        return 0                                                    ;
    }
}
































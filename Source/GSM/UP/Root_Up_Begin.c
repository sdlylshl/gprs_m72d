
#include "Root_Up.h"

unsigned char UP_STR_A10_Type                                       ;//升级控制系统类型 
unsigned char UP_STR_Flag                                           ;//升级标志11==OK；00==Fail

char UP_Root_Stat_BuF[29]                                           ;//存放网关发送升级启动指令
unsigned char UP_Stat_SPI_BuF[18]                                   ;//存放升级启动SPI指令
void TRAN_UP_SPI_BUF(char Move_Num)                                 ;//转移软件版本号
void READ_UP_SPI_FLASH(void)                                        ;//转移软件版本号及升级类型

/*******************************************************************\
*	      函数名：Root_UP_Net_Begin             
*	      作用域：外部文件调用
*	      功能：  网关发送升级启动指令
*	      参数：  
          格式：  ID(4)+Cmd(2)+指令流水号(2)+数据包长度(2)
                  +升级程序种类(1)+升级程序文件总字节数(2)
                  +升级文件版本号(8)+CRC校验(2))
*	      返回值：    
*
*	      修改历史：（每条详述）
\*******************************************************************/
char Root_UP_Net_Begin(void)                                        //网关发送升级启动指令
{
    unsigned int NET_CNT                                            ;//计数用
    unsigned int UP_CRC_B_Sum                                       ;//计算升级开始校验和
    unsigned int UP_Packet_Len                                      ;//网关发送升级启动指令包字节总长度
    unsigned int UP_Root_CRC_Sum                                    ;//远程升级开始校验和
    
    UP_Packet_Len  =   UP_Root_Stat_BuF[6]                          ;//升级启动指令包字节长度
    UP_Packet_Len  =   UP_Packet_Len  <<  8                         ;
    UP_Packet_Len  =   UP_Packet_Len +UP_Root_Stat_BuF[7]           ;
    
    UP_Root_CRC_Sum =   UP_Root_Stat_BuF[28]                        ;
    UP_Root_CRC_Sum =   UP_Root_CRC_Sum  <<8                        ;
    UP_Root_CRC_Sum =   UP_Root_CRC_Sum +   UP_Root_Stat_BuF[27]    ;
    UP_CRC_B_Sum=crc_modbus2((unsigned char *)UP_Root_Stat_BuF,27)  ;
    
    if(UP_CRC_B_Sum==UP_Root_CRC_Sum)
    {
        Flash_WriteChar(57,'V')                                     ;//写标志位
        for(NET_CNT=0;NET_CNT<9;NET_CNT++)                           //写升级程序种类1+升级文件版本号8
        {   
           _NOP();_NOP();_NOP();_NOP();_NOP();
           Flash_WriteChar(58+NET_CNT,UP_Root_Stat_BuF[8+NET_CNT])  ;      
        } 
        
        for(NET_CNT=0;NET_CNT<10;NET_CNT++)                           //写升级程序种类1+升级文件版本号8
        {   
           _NOP();_NOP();_NOP();_NOP();_NOP();
           Flash_WriteChar(87+NET_CNT,UP_Root_Stat_BuF[17+NET_CNT])  ;      
        }
        
        _NOP();_NOP();_NOP();_NOP();_NOP();
        READ_UP_SPI_FLASH()                                         ;//转移软件版本号及升级类型
        
        UP_STR_A10_Type  =   UP_Root_Stat_BuF[8]                    ;//升级控制系统类型
        switch(UP_STR_A10_Type)                                      //升级文件版本号
        {   
            case 0x12:
            {
                TRAN_UP_SPI_BUF(8)                                  ;//转移软件版本号及CRC校验
                Module_Status[4]   |=    0x03                       ;//升级标志
                ANS_Up_Stat_Buf[8] |=    0x12                       ;//回复平台升级启动指令数组
                ANS_UP_STAT_2_NET()                                 ;//回复升级启动指令平台数据
                return 1                                            ;
            } //0x03==升级显示器;
            
            case 0x11:
            {
                Module_Status[4]   |=    0x0C                       ;
                TRAN_UP_SPI_BUF(16)                                 ;
                break                                               ;
            } //0x0C==升级控制器;
            
            case 0x10:
            {
                Module_Status[4]   |=    0x30                       ; 
                TRAN_UP_SPI_BUF(24)                                 ;
                break                                               ;
            } //0x30==升级油门控制器系统类型
            
            case 0x13:
            {
                TRAN_UP_SPI_BUF(32)                                 ;
                break                                               ;
            } //0xC0==升级GPS,GSM系统类型
            
           default:
                break                                               ;
        }

    }
    return 0                                                        ;
}


/*******************************************************************\
*	      函数名：Root_UP_Net_Begin             
*	      作用域：内部文件调用
*	      功能：  转移软件版本号
*	      参数：  Move_Num==转移软件版本类型
          格式：  无
*	      返回值：无    
*
*	      修改历史：（每条详述）
\*******************************************************************/
void TRAN_UP_SPI_BUF(char Move_Num)                                  //转移软件版本号
{
    unsigned char UP_Num,RT_Num                                     ;//转移当前软件版本号计数用
    
    Move_Num    =   Move_Num-8                                      ;
    for(UP_Num=0;UP_Num<8;UP_Num++)                                 
    {
        ANS_Up_Stat_Buf[10+UP_Num]=Soft_VER_memory[UP_Num+Move_Num] ;//转移当前软件版本号
    }
    
    for(RT_Num=0;RT_Num<18;RT_Num++)                                 //启动升级程序文件给SPI
    {
        UP_Stat_SPI_BuF[RT_Num]=UP_Root_Stat_BuF[8+1+RT_Num]       ;//升级程序种类,升级文件版本号    
    }
}



/*******************************************************************\
*	      函数名：READ_UP_SPI_FLASH             
*	      作用域：内部文件调用
*	      功能：  转移软件版本号
*	      参数：  Move_Num==转移软件版本类型
          格式：  无
*	      返回值：无    
*
*	      修改历史：（每条详述）
\*******************************************************************/
void READ_UP_SPI_FLASH(void)                                         //转移软件版本号及升级类型
{
    unsigned char ST_Num                                            ;//转移当前软件版本号计数用
    char* RTUP_ADD   = (char *)0x1839                               ;//升级软件版本号 标志位首地址001839
    
    if(* (RTUP_ADD)=='V')
    {
        UP_STR_A10_Type  =   * (RTUP_ADD+1)                         ;//升级控制系统类型
        for(ST_Num=0;ST_Num<8;ST_Num++)                              //转移升级版本号给索引升级包 
        {
            ANS_UP_SPI_Num_BuF[14+ST_Num]=* (RTUP_ADD+2+ST_Num);  
            UP_Stat_SPI_BuF[ST_Num]      =* (RTUP_ADD+2+ST_Num);//升级程序种类,升级文件版本号
        }
        
        for(ST_Num=0;ST_Num<10;ST_Num++)                              //转移升级版本号给索引升级包 
        {
            UP_Stat_SPI_BuF[8+ST_Num]      =* (RTUP_ADD+30+ST_Num);//升级程序种类,升级文件版本号
        }
        UP_SPI_Num_Flag=0x11;
        UP_Send_Times=0;
        //Module_Status[4]|=0x03;//升级标志  
    }
}
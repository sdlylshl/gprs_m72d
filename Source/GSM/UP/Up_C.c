

#include "msp430x54x.h"
#include "Root_Up.h"

char Sys_NOW_UP_Flag;//系统正在升级标志 11==在升级 55 未升级  升级完成时需要置位 88  低功耗模式时为66
#define UP_SYSTEM_C_SIZE 1200
unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//升级包数据
extern unsigned char Data_Bag_TX_Finish;
extern unsigned int      SPI_UP_BIT_SUM                                    ;//累计已接收文件字节数

/*******************************************************************\
*	      函数名：DO_ROOT_UP             
*	      作用域：外部文件调用
*	      功能：  处理远程升级程序
*	      参数：  
          格式：  ID(4)+Cmd(2)+指令流水号(2)+数据包长度(2)
                  +升级程序种类(1)+升级程序文件总字节数(2)
                  +升级文件版本号(8)+CRC校验(2))
*	      返回值：    
*
*	      修改历史：（每条详述）
\*******************************************************************/
void DO_ROOT_UP(void)                                               //处理远程升级程序
{
  
    static unsigned long int ROOT_NUM                             ;//累计已接收文件字节数

    if(Ask_index==1)
    {
        Ask_index=0;
        ROOT_NUM=Index;
        ROOT_NUM=ROOT_NUM<<10;
        if(UP_SPI_Sum_Len!=ROOT_NUM)//已接收的数据包的索引号
        {
             UP_SPI_Num_Flag=0x11;
             UP_SPI_Sum_Len=Index;
             UP_Send_Times=0;
             Sys_NOW_UP_Flag=0x11;
             
            //向平台要数据包;
            //标志置1;		
        }
        
        
        if((UP_SPI_Sum_Len==SPI_UP_BIT_SUM)&&(!Data_Bag_TX_Finish))
        //else
        {
             //UP_SPI_Num_Flag=0x00;
             //Module_Status[4]       = 0x00                                ;//无升级要求
             //标志置1;
             Module_Status[5]|=0x0c;//清每包数据完成的标志  
        }
        
        if((UP_SPI_Sum_Len==ROOT_NUM)&&(UP_SPI_NUM_CNT>SECD_5*2))//if((SPI_UP_BIT_SUM!=ROOT_NUM)&&(UP_SPI_NUM_CNT>SECD_5*2))
        {
            if(UP_Send_Times>5)
            {
                UP_SPI_Num_Flag=0x00;
                Sys_NOW_UP_Flag=0x55;
            }
            else
            {
                UP_SPI_Num_Flag=0x11;
				UP_SPI_Sum_Len=Index;
                Sys_NOW_UP_Flag=0x11;
            }
        }
    }
    
    if(Sys_NOW_UP_Flag==0x11)//加快升级速度
    {
        RD_Data_Time=128;
        RD_UCA0_Time=10240;
    }
    else
    {
        RD_Data_Time=5120;
        RD_UCA0_Time=512;
    }
    
    UP_SPI_Num_Net()                                               ;//SPI包数索引号,发送给平台索引包
    //Ce_Shi_SPI_NUM()                                               ;//回复升级启动指令平台数据
}




/**********************************************************************\
*	      函数名：UP_VAR_Init             
*	      作用域：外部文件调用
*	      功能：  处理IP地址\Port号  
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+IP地址(4)+Port号(2)
*	      返回值：返回IP地址\Port号   
*
*	      修改历史：（每条详述）
\**********************************************************************/
void UP_VAR_Init(void)
{

}

















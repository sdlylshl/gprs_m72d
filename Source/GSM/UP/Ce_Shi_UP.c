
#include "msp430x54x.h"
#include "Root_Up.h"



//unsigned int  UP_SPI_Ce_SHI                                       ;

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
/*
void Ce_Shi_SPI_NUM(void)                                          //回复升级启动指令平台数据
{
    unsigned int  UP_SPI_Ce_CNT                                   ; 
    
    if(UP_SPI_Num_Flag == 0x00)
    {
        UP_SPI_Num_Flag  =0x11                                    ;//11==ARM索要数据(包数索引号,每包字节数)
        UP_SPI_Ce_CNT   =   UP_SPI_Ce_SHI                         ;
        
        UP_SPI_Num_BuF[1]   =   UP_SPI_Ce_CNT                     ;
        UP_SPI_Ce_CNT       =   UP_SPI_Ce_CNT   >>  8             ;
        UP_SPI_Num_BuF[0]   =   UP_SPI_Ce_CNT                     ;
    
        UP_SPI_Num_BuF[2]=0x02                                    ;
        UP_SPI_Num_BuF[3]=0x00                                    ;
        
        UP_SPI_Ce_SHI++                                           ;
    }
}


*/
























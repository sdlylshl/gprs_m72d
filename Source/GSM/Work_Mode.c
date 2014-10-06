
#include "msp430x54x.h"
#include "GSM.h"

char HML_Mode_BuF[21]                                                   ;//存放工作模式参数设置(HMLFB+上限)

#define  WORK_MODE_BUF_SZ 29
char REC_WORK_MODE_BUF[WORK_MODE_BUF_SZ]                                ;//接收MCU数据上传时间间隔设置
char ANS_WORK_MODE_BUF[8]              ;//工作模式参数 回复
char SD_WORK_MD_ER_FG                                                   ;//工作模式参数 发送失败标志 重发
char Work_Mode_SPI_OK_Flag                                              ;//工作模式参数 SPI可以发送标志 11==可以发送；00==不能发送

/*******************************************************************\
*	      函数名：Work_Mode_Set             
*	      作用域：外部文件调用
*	      功能：  处理系统运行工作模式参数 
*	      参数：  
          格式： ID(4)+命令编码0x38(2)+HMLFB(20)+微调上限(1)+CRC(2)
*	      返回值：返回IP地址\Port号   
*
*	      修改历史：（每条详述）
\*******************************************************************/
char Work_Mode_Set()
{
    unsigned int CAL_WK_MD_CRC,NET_WK_MD_CRC                            ;//校验和
    unsigned int WK_CNT                                                 ;//计数用
      
    NET_WK_MD_CRC =   REC_WORK_MODE_BUF[28]                             ;
    NET_WK_MD_CRC =   NET_WK_MD_CRC  <<8                                ;
    NET_WK_MD_CRC =   NET_WK_MD_CRC +   REC_WORK_MODE_BUF[27]           ;
    CAL_WK_MD_CRC =crc_modbus2((unsigned char *)REC_WORK_MODE_BUF,
                            WORK_MODE_BUF_SZ-2)                         ;//27
    if(CAL_WK_MD_CRC==NET_WK_MD_CRC)
    {
        Tran_ID_CmdNum(ANS_WORK_MODE_BUF,0x83)                            ;//转存ID号和命令编码

        for(WK_CNT=0;WK_CNT<21;WK_CNT++)                                 //工作参数转存SPI
        {            
            HML_Mode_BuF[WK_CNT]=REC_WORK_MODE_BUF[6+WK_CNT]            ;
        }
        Work_Mode_SPI_OK_Flag   =0x11                                   ;//SPI可以发送工作模式参数
        
        Module_Status[5]  |= 0x03                                       ;
        ANS_WORK_MODE_BUF[6]=0xFF                                       ;//修改工作模式成功
        ANS_WORK_MODE_BUF[7]=0xFF                                       ;
    }
    else
    {
        Work_Mode_SPI_OK_Flag   =0x00                                   ;
        Module_Status[5]  &= ~0x03                                      ;
        ANS_WORK_MODE_BUF[6]=0x00                                       ;//修改工作模式失败
        ANS_WORK_MODE_BUF[7]=0x00                                       ;
    }
    
    SD_WORK_MD_ER_FG    =   1                                           ;

    if(GSM_SendData(ANS_WORK_MODE_BUF,sizeof(ANS_WORK_MODE_BUF)))       //数据发送
    {
       SD_WORK_MD_ER_FG  =  0;
       return 1;
    }
    
    
    if(SD_WORK_MD_ER_FG)                                                //重发 
    {
       Delayms(200);//XX*1Ms延时
       SD_WORK_MD_ER_FG  = 0;
       if(GSM_SendData(ANS_WORK_MODE_BUF,sizeof(ANS_WORK_MODE_BUF)))         
       {          
          SD_WORK_MD_ER_FG    =    0                                    ;
          return 1                                                      ;
       }
    }
    return 0                                                            ;  
}





















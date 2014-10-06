
#include "Root_Up.h"

char UP_OK_EROR_Flag                                                  ;//IP端口号 重发
#define UP_Result_Buf_SZ 20
char ANS_UP_OK_BUF[UP_Result_Buf_SZ]                                  ;//上报程序升级结果回复
char REC_UP_OK_BUF[UP_Result_Buf_SZ]                                  ;//平台下发上报程序升级结果
char ROOT_UP_OK_Flag                                                  ;//程序升级标志位11==OK；00==Fail
//unsigned char UP_STR_Flag                                           ;//升级标志11==OK；00==Fail


/*******************************************************************\
*	      函数名：ANS_UP_Result             
*	      作用域：外部文件调用
*	      功能：  GPS一体机上报程序升级结果
*	      参数：  
          格式： 
                GPS终端ID号	    	4
                命令编码	0xA3	2
                数据包长度	    	2
                升级程序种类		1
                升级后文件版本号	ASCII码	5+ BCD码	3
                程序升级结果	0x00：成功；0x01：失败	1
                校验		        2
*	      返回值：    
*
*	      修改历史：（每条详述）
\*******************************************************************/
char NET_ANS_UP_Result(void)                                        //网关发送升级启动指令
{
    unsigned int CAL_UP_OK_CRC,NET_UP_OK_CRC                            ;//校验和
      
    NET_UP_OK_CRC =   REC_UP_OK_BUF[19]                                 ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC  <<8                                ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC +   REC_UP_OK_BUF[18]               ;
    CAL_UP_OK_CRC =crc_modbus2((unsigned char *)REC_UP_OK_BUF,
                                UP_Result_Buf_SZ-2)                     ;//18
    if(CAL_UP_OK_CRC==NET_UP_OK_CRC)
    {
        if(COMP_Peice_STR(REC_UP_OK_BUF,ANS_UP_OK_BUF,6,6,12)==1)
        {
            ROOT_UP_OK_Flag  =0x11                                      ;//程序升级标志位11==OK；00==Fail
            return 1                                                    ;
        }
    }
    return 0                                                            ;  
}



/*******************************************************************\
*	      函数名：ANS_UP_Result             
*	      作用域：外部文件调用
*	      功能：  GPS一体机上报程序升级结果
*	      参数：  
          格式： 
                GPS终端ID号	    	4
                命令编码	0xA3	2
                数据包长度	    	2
                升级程序种类		1
                升级后文件版本号	ASCII码	5+ BCD码	3
                程序升级结果	0x00：成功；0x01：失败	1
                校验		        2
*	      返回值：    
*
*	      修改历史：（每条详述）
\*******************************************************************/
char GPS_ASK_UP_Result(void)                                        //网关发送升级启动指令
{

    unsigned char MV_Num                                                ;//计数用
    unsigned int CAL_UP_OK_CRC,NET_UP_OK_CRC                            ;//校验和
    unsigned int UP_OK_BUF_Len                                          ;//本指令包字节总长度 
      
    NET_UP_OK_CRC =   REC_UP_OK_BUF[19]                                 ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC  <<8                                ;
    NET_UP_OK_CRC =   NET_UP_OK_CRC +   REC_UP_OK_BUF[18]               ;
    CAL_UP_OK_CRC =crc_modbus2((unsigned char *)REC_UP_OK_BUF,
                                UP_Result_Buf_SZ-2)                     ;//18
    if(CAL_UP_OK_CRC==NET_UP_OK_CRC)
    {
        Tran_ID_CmdNum(ANS_UP_OK_BUF,0xA3)                   ;//转存ID号和命令编码   

        UP_OK_BUF_Len     =  UP_Result_Buf_SZ                           ;//本指令包字节总长度
        ANS_UP_OK_BUF[7]  =  UP_OK_BUF_Len                              ;
        UP_OK_BUF_Len     =  UP_OK_BUF_Len   >>8                        ;
        ANS_UP_OK_BUF[6]  =  UP_OK_BUF_Len                              ;
        
        READ_UP_SPI_FLASH()                                             ;//转移软件版本号及升级类型
        ANS_UP_OK_BUF[8]  =  UP_STR_A10_Type                            ;//升级控制系统类型 
        for(MV_Num=0;MV_Num<8;MV_Num++)                                  //升级文件版本号
        {            
            ANS_UP_OK_BUF[9+MV_Num]=ANS_UP_SPI_Num_BuF[14+MV_Num]       ;
        }
        ANS_UP_OK_BUF[17]  =  0x01                                      ;
        
        CAL_UP_OK_CRC=crc_modbus2((unsigned char *)ANS_UP_OK_BUF,
                                    (UP_Result_Buf_SZ-2))               ;//18
        ANS_UP_OK_BUF[18]    =   (char)(CAL_UP_OK_CRC&0xFF)             ;
        ANS_UP_OK_BUF[19]    =   (char)((CAL_UP_OK_CRC>>8)&0xFF)        ;
        
        UP_OK_EROR_Flag   =   1                                         ;
        if(GSM_SendData(ANS_UP_OK_BUF,UP_Result_Buf_SZ))                 //数据发送   
        {
           UP_OK_EROR_Flag  =  0                                        ;
           return 1                                                     ;
        }
        if(UP_OK_EROR_Flag)                                              //失败重发
        {
           if(GSM_SendData(ANS_UP_OK_BUF,UP_Result_Buf_SZ))         
           {          
              UP_OK_EROR_Flag  =  0                                     ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;  
}



























































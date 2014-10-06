
#include "msp430x54x.h"
#include "GSM.h"

#define  NET_IP_PORT_BUF_SIZE 14
char REC_IP_PORT_BUF[NET_IP_PORT_BUF_SIZE]                              ;//接收IP端口号
char ANS_IP_PORT_BUF[NET_IP_PORT_BUF_SIZE];//IP端口号 回复

char IP_PORT_EROR_Flag                                                  ;//IP端口号 重发
void ReaD_IP_PORT_FLASH(void)                                           ;//读取IP地址\Port号 
void IP_2_A(unsigned char NUM,unsigned char ADD)                        ;
void PORT_2_A(unsigned int P_NUM)                                       ;
/**********************************************************************\
*	      函数名：IP_Port_Reset             
*	      作用域：外部文件调用
*	      功能：  处理IP地址\Port号  
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+IP地址(4)+Port号(2)
*	      返回值：返回IP地址\Port号   
*
*	      修改历史：（每条详述）
\**********************************************************************/
char IP_Port_Reset()
{
    unsigned char PT_Num,IP_Num                                         ;//计数用
    char* IP_PORT                                                       ;
    unsigned int CAL_IP_PT_CRC,NET_IP_PT_CRC                            ;//校验和
      
    NET_IP_PT_CRC =   REC_IP_PORT_BUF[13]                               ;
    NET_IP_PT_CRC =   NET_IP_PT_CRC  <<8                                ;
    NET_IP_PT_CRC =   NET_IP_PT_CRC +   REC_IP_PORT_BUF[12]             ;
    CAL_IP_PT_CRC =crc_modbus2((unsigned char *)REC_IP_PORT_BUF,
                                NET_IP_PORT_BUF_SIZE-2)                 ;//12
    if(CAL_IP_PT_CRC==NET_IP_PT_CRC)
    {
        Tran_ID_CmdNum(ANS_IP_PORT_BUF,0x87)                        ;//转存ID号和命令编码
    
        Flash_WriteChar(12,0x11)                                        ;//写标志位
        for(IP_Num=0;IP_Num<6;IP_Num++)                                 //写IP及端口号
        {  
           _NOP();_NOP();_NOP();_NOP();_NOP();          
           Flash_WriteChar(13+IP_Num,REC_IP_PORT_BUF[6+IP_Num])         ;      
        }  
        
        _NOP();_NOP();_NOP();_NOP();_NOP();
        
        IP_PORT     = (char *)0x180D                                    ;//IP及端口号首地址
        for(PT_Num=0;PT_Num<6;PT_Num++)                                 
        {            
            ANS_IP_PORT_BUF[6+PT_Num] =  *(IP_PORT+PT_Num)              ;//回复IP及端口号      
        }  
        
        ReaD_IP_PORT_FLASH()                                            ;//读取IP地址\Port号 

        CONNECT_FAIL_Flag     =0x11                                     ;//修改标志位，用于修改IP后立即执行重启
        
        CAL_IP_PT_CRC=crc_modbus2((unsigned char *)ANS_IP_PORT_BUF,
                                    (NET_IP_PORT_BUF_SIZE-2))           ;//12
        ANS_IP_PORT_BUF[12]    =   (char)(CAL_IP_PT_CRC&0xFF)           ;
        ANS_IP_PORT_BUF[13]    =   (char)((CAL_IP_PT_CRC>>8)&0xFF)      ;
        
        IP_PORT_EROR_Flag   = 1                                         ;
        if(GSM_SendData(ANS_IP_PORT_BUF,NET_IP_PORT_BUF_SIZE))           //数据发送   
        {
           IP_PORT_EROR_Flag  =  0                                      ;
           return 1                                                     ;
        }
        if(IP_PORT_EROR_Flag)                                            //失败重发
        {
            Delayms(200);
            IP_PORT_EROR_Flag  =  0;
           if(GSM_SendData(ANS_IP_PORT_BUF,NET_IP_PORT_BUF_SIZE))         
           {          
              IP_PORT_EROR_Flag  =  0                                   ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;  
}


/*******************************************************************\
*	      函数名：ReaD_IP_PORT             
*	      作用域：内部文件调用
*	      功能：  读取IP地址\Port号  
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+IP地址(8)+Port号(4)
*	      返回值：
*
*	      修改历史：（每条详述）
\*****************************************************************/
void ReaD_IP_PORT_FLASH(void)
{
  
    unsigned char RD_NUM,FL_NUM,FL_CNT                                            ;
    unsigned int  PORT_NUM                                          ;
    char* IP_PORT_AD                                                ;//IP及端口号首地址
    //char* IP_PT_AD_FG   =  (char *)0x180C                           ;//IP及端口号

    IP_PORT_AD     = (char *)0x180D                                 ;

    if(*(IP_PORT_AD-1)==0x11)
    {
        for(FL_NUM=0,FL_CNT=0;FL_NUM<4;FL_NUM++)                                 
        {
            RD_NUM         =   *(IP_PORT_AD + FL_NUM)                   ;
            IP_2_A(RD_NUM,17+FL_CNT)                                    ;
            FL_CNT  =   FL_CNT  +   4                                   ;  
        } 
        
        PORT_NUM    =   *(IP_PORT_AD + 4)                               ;
        PORT_NUM    =   PORT_NUM    <<  8                               ;
        PORT_NUM    =   PORT_NUM    +    *(IP_PORT_AD + 5)              ;
        if(PORT_NUM<10000)
        {
            PORT_2_A(PORT_NUM)                                          ;
        }
    }
}



/*******************************************************************\
*	      函数名：ReaD_IP_PORT             
*	      作用域：内部文件调用
*	      功能：  读取IP地址\Port号  
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+IP地址(8)+Port号(4)
*	      返回值：
*
*	      修改历史：（每条详述）
\*****************************************************************/
void IP_2_A(unsigned char I_NUM,unsigned char ADD)
{
    unsigned char RD_IP_NUM,RD_IP_BAI,RD_IP_SHI,RD_IP_GE            ;
    
    RD_IP_NUM   =   I_NUM                                           ;

    RD_IP_SHI   =   RD_IP_NUM   %   100                             ;
    RD_IP_BAI   =   RD_IP_NUM   /   100                             ;
    UDP_Built_STR[ADD+0]   =  HEX_TO_ASCII(RD_IP_BAI)               ;//16进制字符转换ASCII
    RD_IP_GE    =   RD_IP_SHI   %   10                              ;
    RD_IP_SHI   =   RD_IP_SHI   /   10                              ;
    UDP_Built_STR[ADD+1]   =  HEX_TO_ASCII(RD_IP_SHI)               ;
    UDP_Built_STR[ADD+2]   =  HEX_TO_ASCII(RD_IP_GE )               ;
}





/*******************************************************************\
*	      函数名：ReaD_IP_PORT             
*	      作用域：内部文件调用
*	      功能：  读取IP地址\Port号  
*	      参数：  
          格式： ID(4)+命令编码0x87(2)+IP地址(8)+Port号(4)
*	      返回值：
*
*	      修改历史：（每条详述）
\*****************************************************************/
void PORT_2_A(unsigned int P_NUM)
{
    unsigned char RD_PT_QIAN,RD_PT_BAI,RD_PT_SHI,RD_PT_GE           ;
    unsigned int RD_IP_NUM                                          ;
    
    RD_IP_NUM   =   P_NUM                                           ;

    RD_PT_BAI    =   RD_IP_NUM   %   1000                           ;
    RD_PT_QIAN   =   RD_IP_NUM   /   1000                           ;
    UDP_Built_STR[35]   =  HEX_TO_ASCII(RD_PT_QIAN)                 ;//16进制字符转换ASCII
    
    RD_PT_SHI    =   RD_PT_BAI   %   100                            ;  
    RD_PT_BAI    =   RD_PT_BAI   /   100                            ;  
    UDP_Built_STR[36]   =  HEX_TO_ASCII(RD_PT_BAI)                  ;
    
    RD_PT_GE     =   RD_PT_SHI   %   10                             ;  
    RD_PT_SHI    =   RD_PT_SHI   /   10                             ;  
    UDP_Built_STR[37]   =  HEX_TO_ASCII(RD_PT_SHI)                  ;  
    UDP_Built_STR[38]   =  HEX_TO_ASCII(RD_PT_GE)                   ;      
}



/**********************************************************************\
*	      函数名：Test_IP_Port             
*	      作用域：外部文件调用
*	      功能：  测试IP地址\Port号  
*	      参数：  
          格式： 
*	      返回值 
*
*	      修改历史：（每条详述）
\**********************************************************************/
void Test_IP_Port(void)
{
    unsigned char Test;
    
    Flash_WriteChar(12,0x11);//写标志位
    for(Test=0;Test<6;Test++)//写IP及端口号
    {  
       _NOP();_NOP();_NOP();_NOP();_NOP();          
       Flash_WriteChar(13+Test,Test);      
    } 
    ReaD_IP_PORT_FLASH();//读取IP地址\Port号 
}


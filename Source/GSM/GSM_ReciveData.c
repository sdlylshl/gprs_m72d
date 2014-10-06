
#include "GSM.h"

extern unsigned char Monitor_Update_Data[600]                              ;//显示器升级包数据
unsigned long int RD_Data_1_Min_Cnt                                        ;//解析 MUC 应答时间计数
    
   

void RecData()//解析 并将数据转存
{
    if((RD_Data_1_Min_Cnt>1024)&&(UDP_Built_flag==0x11))
    {  
        RD_Data_1_Min_Cnt    = 0                                           ;//解析 MUC 应答时间计数
        if(GSM_SendCMD(PAKET_DATA,"AT+QIRD=0,1,0,1024",0,200)==1)           //将50--200解决512数据读取问题
        {
            char *str=(char *) M72D_ServerData_RX_Buf                      ;
            long len                                                       ;
            unsigned char IP_Cnt,UP_Cnt,TM_Cnt,OUT_Cnt                     ;
            unsigned int  A2_Cnt,RD_CNT                                    ;
            

            if (GSM_ECHO & 0x01)                                           
            {//带回显功能//指令长度+3字节(0x0D 0x0D 0x0A)+内容
                str += 30+sizeof("AT+QIRD=0,1,0,1024")                     ;
            } 
            else 
            {//2字节(0x0D 0x0A)+内容 +QIRD: 58.57.53.58:2013,UDP,10\r\n(28+i+2)+\r\n+OK
               str +=30                                                    ; 
            }           
            len=GSM_atol(str)                                              ;
            while(*str++ != '\n')
            {
                if(*str==0x00)
                    break                                                  ;
            }
    

            switch(*(str+4)&*(str+5))  
            {    //解析 并将数据转存
                case 0x18:
                {
                     if((*(str+6)==0xFF)&&(*(str+7)==0xFF))                 
                        MCU_Data_ACK=0                                     ;
                     break                                                 ;
                } 
                
                case 0x19:
                {//格式：查询/设置(1)+经纬度(16)  OUT_EDGE_BUF_SZ==25
                     if(len==OUT_EDGE_BUF_SZ)
                     {
                         for(OUT_Cnt=0;OUT_Cnt<OUT_EDGE_BUF_SZ;OUT_Cnt++,str++)
                         { 
                             NET_OUT_EDGE_BUF[OUT_Cnt]=*(str)              ;
                         }
                         OUT_EDGE_SET()                                    ;//越界报警命令设置
                     }
                     break                                                 ;
                }
                case 0x23:
                {   //MCU数据上传时间间隔设置   MCU_TIME_BUF_SZ==12
                    if(len==MCU_TIME_BUF_SZ)
                    {
                         for(TM_Cnt=0;TM_Cnt<MCU_TIME_BUF_SZ;TM_Cnt++,str++)
                         {
                             REC_MCU_TIME_BUF[TM_Cnt]=*(str)               ;
                         }
                         MCU_Time_Set()                                    ;
                    }
                     break                                                 ;
                }
                
                
                case 0x38:
                { //工作参数处理 WORK_MODE_BUF_SZ=29                                                          
                    if(len==WORK_MODE_BUF_SZ)
                    {
                         for(RD_CNT=0;RD_CNT<WORK_MODE_BUF_SZ;RD_CNT++,str++)
                         { //格式：HMLFB(20)+微调上限(1)
                            REC_WORK_MODE_BUF[RD_CNT]=*(str)               ;
                         }
                         Work_Mode_Set()                                   ;
                    }
                    break                                                  ;
                }
                case 0x69:                                     
                {                                                           //解车、锁车处理
                     if(*(str+6)==0x03&&*(str+7)==0x03)       
                        Lock_Open_Bus_Flag  =0x11                          ; 
                     if(*(str+6)==0x00&&*(str+7)==0x00) 
                        Lock_Open_Bus_Flag  =0x00                          ;
                     GSM_Lock_Monitor()                                    ;
                     break                                                 ;
                } 
                case 0x78:                                             
                {    //格式：IP地址(4)+Port号(2)    NET_IP_PORT_BUF_SIZE 14
                     if(len==NET_IP_PORT_BUF_SIZE)
                     {
                         for(IP_Cnt=0;IP_Cnt<NET_IP_PORT_BUF_SIZE;IP_Cnt++,str++)
                         {  
                            REC_IP_PORT_BUF[IP_Cnt]=*(str)                 ;
                         }
                         IP_Port_Reset()                                   ;
                     }
                     break                                                 ;
                } 
                case 0x98:                                      
                {                                                           //软件版本号上传回复处理    
                    if(len==8)
                    {
                         if(*(str+6)==0xFF&&*(str+7)==0xFF)
                           SD_SOFT_V_ER_Flag   =0                          ;
                         else
                           SD_SOFT_V_ER_Flag   =1                          ; 
                    }
                     break                                                 ;
                }
                
                case 0xA1:                                      
                {   //网关发送升级启动指令                                     
                    if(len==29)
                    {
                        for(UP_Cnt=0;UP_Cnt<29;UP_Cnt++,str++)
                        { 
                            UP_Root_Stat_BuF[UP_Cnt]=*(str)                ;
                        }
                        Root_UP_Net_Begin()                                ;
                        RD_Data_1_Min_Cnt   =   512                        ;
                    }
                    break                                                  ;
                }
                
                case 0x2A:                                      
                {                                                           //升级数据处理                                     
                     for(A2_Cnt=0;A2_Cnt<len;A2_Cnt++,str++)
                     { 
                        Monitor_Update_Data[A2_Cnt]=*(str)                 ;
                     }
                     UP_A2_NUM_CRC()                                       ;//升级程序CRC校验
                     RD_Data_1_Min_Cnt   =   512                           ;
                     break                                                 ;
                }
                
                
                case 0xA9:                                      
                {   //呼叫GPS终端上报定位数据命令                                     
                    if(len==10)
                    {
                        CALL_MCU_TIME_JIAN  =   *(str+7)                   ;                                         
                        CALL_MCU_TIME_JIAN  =   CALL_MCU_TIME_JIAN  <<  8  ;
                        CALL_MCU_TIME_JIAN  =   CALL_MCU_TIME_JIAN+*(str+6);
                        
                        CALL_MCU_TIME_LONG  =   *(str+9)                   ;
                        CALL_MCU_TIME_LONG  =   CALL_MCU_TIME_LONG  <<  8  ;
                        CALL_MCU_TIME_LONG  =   CALL_MCU_TIME_LONG+*(str+8);
                        
                        CALL_MCU_TIMES=CALL_MCU_TIME_LONG/CALL_MCU_TIME_JIAN;
                        
                        CALL_MCU_TIME_JIAN  =   CALL_MCU_TIME_JIAN  *  1024;
                        CALL_MCU_TIME_CNT   =   0                          ;
                        CALL_MCU_TIME_FLAG  =   0x11                       ;// 11==发送MCU
                        
                        for(TM_Cnt=0;TM_Cnt<4;TM_Cnt++,str++)
                        {
                           ANS_CAL_MCU_BUF[6+TM_Cnt]=*(str+6)              ;
                        }
                        ANSW_CALL_MCU()                                    ;//回复上报定位数据命令
                    }
                    break                                                  ;
                }
                case 0xB2:                                                  // 数据包应答处理
                {
                      if(crc_modbus2((unsigned char *)str,len))      
                      break                                                ;
                }
                  
                default:
                     break                                     ;
            }
        }
    }
}

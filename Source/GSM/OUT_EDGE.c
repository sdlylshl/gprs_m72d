
#include "msp430x54x.h"
#include "GSM.h"


extern unsigned long int GPS_J_Data                                          ;//经度最终上传信息
extern unsigned long int GPS_W_Data                                          ;//纬度最终上传信息

unsigned int OUT_EDGE_CRC_Sum                                       ;//越界报警命令设置 校验和

long int NET_SET_J_SMA  =0                                          ;//经度最小值
long int NET_SET_J_BIG  =0                                          ;//经度最大值
long int NET_SET_W_SMA   =0                                         ;//纬度最小值
long int NET_SET_W_BIG   =0                                         ;//纬度最大值


#define OUT_EDGE_BUF_SZ 25
char ANS_OUT_EDGE_BUF[OUT_EDGE_BUF_SZ];//越界报警命令设置回复
char NET_OUT_EDGE_BUF[OUT_EDGE_BUF_SZ]                              ;//平台下发越界报警命令
char SD_OUT_EDGE_EROR_Flag                                          ;//越界报警发送失败标志 重发
char JG_OUT_EDGE_Flag  =0x00                                        ;//越界报警判断标志位 11=可以判断
char* EDGE_OUT  = (char *)0x1814                                    ;//越界报警值首地址




char RD_OUT_EDGE_FH(void)                                           ;//读取越界报警 
void JG_OUT_EDGE(void)                                              ;//判断越界报警 
/*******************************************************************\
*	      函数名：OUT_EDGE_SET             
*	      作用域：外部文件调用
*	      功能：  越界报警命令设置；回复平台；判断是否越界  
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）
\*******************************************************************/
char OUT_EDGE_SET(void)                                              //越界报警命令设置
{
    unsigned char OUT_Num                                           ;
    unsigned int OUT_CRC_SUM                                        ;//越界报警命令设置 校验和

    OUT_EDGE_CRC_Sum =   NET_OUT_EDGE_BUF[24]                       ;
    OUT_EDGE_CRC_Sum =   OUT_EDGE_CRC_Sum  <<8                      ;
    OUT_EDGE_CRC_Sum =   OUT_EDGE_CRC_Sum +   NET_OUT_EDGE_BUF[23]  ;
    OUT_CRC_SUM=crc_modbus2((unsigned char *)NET_OUT_EDGE_BUF,
                            OUT_EDGE_BUF_SZ-2)                      ;//23
    
    if(OUT_CRC_SUM==OUT_EDGE_CRC_Sum)
    {
        SD_OUT_EDGE_EROR_Flag=1                                     ;
        Tran_ID_CmdNum(ANS_OUT_EDGE_BUF,0x91)                            ;//转存ID号和命令编码

        ANS_OUT_EDGE_BUF[6]   =    NET_OUT_EDGE_BUF[6]              ;//SetType ：0=查询越界经纬度；1=设置越界经纬度。

        if(NET_OUT_EDGE_BUF[6]==0x01)
        {   //设置越界经纬度
            Flash_WriteChar(19,0x11)                                ;//写标志位
            for(OUT_Num=0;OUT_Num<16;OUT_Num++)                      //写越界报警值
            {      
                _NOP();_NOP();_NOP();_NOP();_NOP();
                Flash_WriteChar(20+OUT_Num,NET_OUT_EDGE_BUF[7+OUT_Num]);      
            }
            _NOP();_NOP();_NOP();_NOP();_NOP();         
            for(OUT_Num=0;OUT_Num<16;OUT_Num++)                                 
            {            
                ANS_OUT_EDGE_BUF[7+OUT_Num] =  *(EDGE_OUT+OUT_Num)  ;//回复flash越界经纬度      
            }
            
            RD_OUT_EDGE_FH()                                        ;//读取越界报警 
        }
        
        if(NET_OUT_EDGE_BUF[6]==0x00)
        {   //查询越界经纬度
            if((*(EDGE_OUT-1)==0x11))
            {
                for(OUT_Num=0;OUT_Num<16;OUT_Num++)                                 
                {            
                    ANS_OUT_EDGE_BUF[7+OUT_Num]=*(EDGE_OUT+OUT_Num) ;//回复flash越界经纬度      
                }
            }
            else
            {           
                for(OUT_Num=0;OUT_Num<16;OUT_Num++)                                 
                {            
                    ANS_OUT_EDGE_BUF[7+OUT_Num] =  0x00             ;//回复flash越界经纬度      
                }
            }
            
        }
        
        if((NET_OUT_EDGE_BUF[6]==0x01)||(NET_OUT_EDGE_BUF[6]==0x00))
        {
        
            OUT_CRC_SUM=crc_modbus2((unsigned char *)ANS_OUT_EDGE_BUF,
                                    (OUT_EDGE_BUF_SZ-2))                ;//23
            ANS_OUT_EDGE_BUF[23]    =   (char)(OUT_CRC_SUM&0xFF)        ;
            ANS_OUT_EDGE_BUF[24]    =   (char)((OUT_CRC_SUM>>8)&0xFF)   ;
            
            if(GSM_SendData(ANS_OUT_EDGE_BUF,OUT_EDGE_BUF_SZ))           //数据发送   
            {
                SD_OUT_EDGE_EROR_Flag=0                                 ;
                return 1                                                ;
            }
            if(SD_OUT_EDGE_EROR_Flag)                                    //失败重发
            {
                Delayms(200);
                SD_OUT_EDGE_EROR_Flag=0                             ;
                if(GSM_SendData(ANS_OUT_EDGE_BUF,OUT_EDGE_BUF_SZ))         
                {          
                    SD_OUT_EDGE_EROR_Flag=0                             ;
                    return 1                                            ;
                }
            }
        }
    }
    return 0                                                        ;  
    

}


/*******************************************************************\
*	      函数名：RD_OUT_EDGE_FH             
*	      作用域：外部文件调用
*	      功能：  读取越界报警FLASH经纬度设置值  
*	      参数：  
          格式： 
*	      返回值：
*
*	      修改历史：（每条详述）
\*******************************************************************/
char RD_OUT_EDGE_FH(void)                                           //读取越界报警 
{
    unsigned char EDG_Num                                           ;
    unsigned long GPS_Jing_Wei                                        ;//经度最终上传信息

    if((*( EDGE_OUT-1)==0x11))
    {
        NET_SET_J_SMA   =  *(EDGE_OUT)                              ;//经度最小值
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_J_SMA  =NET_SET_J_SMA    <<  8                   ;
           NET_SET_J_SMA  =NET_SET_J_SMA  +  *(EDGE_OUT+1+EDG_Num)  ;      
        }                                                           //204093300
        GPS_Jing_Wei    =   (NET_SET_J_SMA  %  100000)  *  6   /10  ;//55980
        NET_SET_J_SMA   =    NET_SET_J_SMA  /  1000000              ;//204
        NET_SET_J_SMA   =    NET_SET_J_SMA*100000+GPS_Jing_Wei      ;//20455980
        
        
        NET_SET_J_BIG   =  *(EDGE_OUT+4)                            ;//经度最大值
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_J_BIG  =NET_SET_J_BIG    <<  8                   ;
           NET_SET_J_BIG  =NET_SET_J_BIG  +  *(EDGE_OUT+5+EDG_Num)  ;//204093596     
        }
        GPS_Jing_Wei    =   (NET_SET_J_BIG  %  100000)  *  6   /10  ;//56157
        NET_SET_J_BIG   =    NET_SET_J_BIG  /  1000000              ;//204
        NET_SET_J_BIG   =    NET_SET_J_BIG*100000+GPS_Jing_Wei      ;//20456157
        
        
        NET_SET_W_SMA   =  *(EDGE_OUT+8)                            ;//纬度最小值
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_W_SMA  =NET_SET_W_SMA    <<  8                   ;
           NET_SET_W_SMA  =NET_SET_W_SMA  +  *(EDGE_OUT+9+EDG_Num)  ;//70804390      
        }
        GPS_Jing_Wei    =   (NET_SET_W_SMA  %  100000)  *  6         ;//26340
        NET_SET_W_SMA   =    NET_SET_W_SMA  /  100000                ;//7080
        NET_SET_W_SMA   =    NET_SET_W_SMA*100000+GPS_Jing_Wei       ;//70802634
        
        
        NET_SET_W_BIG   =  *(EDGE_OUT+12)                           ;//经度最大值
        for(EDG_Num=0;EDG_Num<3;EDG_Num++)                                 
        {   
           NET_SET_W_BIG  =NET_SET_W_BIG    <<  8                    ;
           NET_SET_W_BIG  =NET_SET_W_BIG  +  *(EDGE_OUT+13+EDG_Num)  ;      
        }
        GPS_Jing_Wei    =   (NET_SET_W_BIG  %  100000)  *  6         ;//26340
        NET_SET_W_BIG   =    NET_SET_W_BIG  /  100000                ;//7080
        NET_SET_W_BIG   =    NET_SET_W_BIG*100000+GPS_Jing_Wei       ;//70802634
        
        JG_OUT_EDGE_Flag  =  0x11                                   ;//越界报警判断标志位 11=可以判断
        JG_OUT_EDGE()                                               ;//判断越界报警 
    }
    return 0                                                        ;  
    
    
}




/*******************************************************************\
*	      函数名：JG_OUT_EDGE             
*	      作用域：外部文件调用
*	      功能：  判断越界报警  
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）
\*******************************************************************/
void JG_OUT_EDGE(void)                                               //判断越界报警 
{
    if(JG_OUT_EDGE_Flag  ==  0x11)                                   //越界报警判断标志位 11=可以判断
    {
        if(((GPS_W_Data<NET_SET_J_SMA)||(GPS_W_Data>NET_SET_J_BIG)
           ||(GPS_J_Data<NET_SET_W_SMA)||(GPS_J_Data>NET_SET_W_BIG))
           &&((NET_SET_J_SMA!=0)&&(NET_SET_J_BIG!=0)&&(NET_SET_W_SMA!=0)&&(NET_SET_W_BIG!=0))
           &&((GPS_GSM_System_Stu[0]&GPS_Position)))
        {
            Module_Status[1] |=  0xC0                               ;//经度最大值越界报警判断
            GPS_GSM_System_Stu[1]|=0xC0                             ;
        }
        else
        {
            Module_Status[1] &=~ 0xC0                               ;//经度最大值越界报警判断
            GPS_GSM_System_Stu[1]&=~0xC0                            ;
        }
    }
}











#include "Root_Up.h"

#define ROOT_UP_OUT_TIME 61440

//unsigned int  UP_SPI_NUM_ADD_1   =0                                 ;//远程发送次数计数
unsigned char SD_EROR_UP_NUM                                        ;//11==发送失败，
unsigned char UP_SPI_Num_Flag    =0x55                              ;//11==ARM索要数据(包数索引号,每包字节数)
unsigned long int UP_SPI_Sum_Len =0xffffffff                                ;//累计已接收文件字节数
unsigned long int UP_SPI_NUM_CNT                                    ;//远程升级计数
unsigned long int UP_Send_Times                                        ;//远程升级发送次数计数


#define AnsUp_SpiNum_BfSz 24
//char   UP_SPI_Num_BuF[4]                                            ;//存放包数索引号,每包字节数
char   ANS_UP_SPI_Num_BuF[AnsUp_SpiNum_BfSz];//向平台索要升级数据包
/*******************************************************************\
*	      函数名：UP_SPI_Num_Net             
*	      作用域：外部文件调用
*	      功能：  SPI包数索引号,发送给平台索引包
*	      参数：  
          格式：  ID(4)+Cmd(2)+指令流水号(2)+数据包长度(2)
                  +升级程序种类(1)+升级程序文件总字节数(2)
                  +升级文件版本号(8)+CRC校验(2))
*	      返回值：    
*
*	      修改历史：（每条详述）
\*******************************************************************/
char UP_SPI_Num_Net(void)                                            //SPI包数索引号,发送给平台索引包
{
    unsigned int  UP_SPI_CRC                                        ;
    static   unsigned long int UP_SPI_Sum                                    ;//累计已接收文件字节数

    if((UDP_Built_flag==0x11)&&(UP_SPI_Num_Flag==0x11)
       &&(UP_SPI_NUM_CNT>128) &&(Heart_Beat_Send_Cnt>1))     //         
    {
        UP_Send_Times++;
        UP_SPI_NUM_CNT      =   0                                   ;//远程升级计数
        UP_SPI_Num_Flag=0x00;
     

        Tran_ID_CmdNum(ANS_UP_SPI_Num_BuF,0xA2)                   ;//转存ID号和命令编码                              
        
        ANS_UP_SPI_Num_BuF[6]   =   0x00                            ;//本指令包字节总长度 
        ANS_UP_SPI_Num_BuF[7]   =   0x18                            ; 
        
        ANS_UP_SPI_Num_BuF[8]   =   UP_STR_A10_Type                 ;//升级控制系统类型 
        
        ANS_UP_SPI_Num_BuF[9]   =   0x01                            ;//升级标志:00==不升级，0x01==升级 
        

        UP_SPI_Sum_Len  =   UP_SPI_Sum_Len  <<  10                   ;//1024   *512  //修改成256
        
        UP_SPI_Sum      =UP_SPI_Sum_Len;
        ANS_UP_SPI_Num_BuF[13]  =   UP_SPI_Sum                  ;
        UP_SPI_Sum  =   UP_SPI_Sum  >>  8                   ;
        ANS_UP_SPI_Num_BuF[12]  =   UP_SPI_Sum                  ;
        UP_SPI_Sum  =   UP_SPI_Sum  >>  8                   ;
        ANS_UP_SPI_Num_BuF[11]  =   UP_SPI_Sum                  ;
        UP_SPI_Sum  =   UP_SPI_Sum  >>  8                   ;
        ANS_UP_SPI_Num_BuF[10]  =   UP_SPI_Sum                  ;
        
        
        UP_SPI_CRC=crc_modbus2((unsigned char *)ANS_UP_SPI_Num_BuF,
                               (AnsUp_SpiNum_BfSz-2))               ;
        ANS_UP_SPI_Num_BuF[22]    =   (char)(UP_SPI_CRC&0xFF)       ;
        ANS_UP_SPI_Num_BuF[23]    =   (char)((UP_SPI_CRC>>8)&0xFF)  ;
        
        SD_EROR_UP_NUM    =   0x11                                  ;

        if(GSM_SendData(ANS_UP_SPI_Num_BuF,AnsUp_SpiNum_BfSz))       //数据发送
        {
            SD_EROR_UP_NUM    =0                                    ;//修改标志位
            return 1                                                ;
        }
        if(SD_EROR_UP_NUM == 0x11)                                   //失败重发
        {
            Delayms(200)                                            ;//XX*1Ms延时
            if(GSM_SendData(ANS_UP_SPI_Num_BuF,AnsUp_SpiNum_BfSz))
            {          
               SD_EROR_UP_NUM    =    0                             ;
               return 1                                             ;
            }
            SD_EROR_UP_NUM    =    0                                ;
        }
    }
    return 0                                                        ;
}



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
void ROOT_UP_TIME_FLASH(void)                                        //网关发送升级启动指令
{

    if((UP_SPI_Num_Flag==0x11)&&(UP_SPI_NUM_CNT>ROOT_UP_OUT_TIME))     //         
    {
      
      
    }
}







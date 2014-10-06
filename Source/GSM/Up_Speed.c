


#include "msp430x54x.h"
#include "GSM.h"

char Up_Speed_Send_ERR_Flag;//允许换卡 无卡升速指令发送失败标志 重发
char* Up_Speed_Addr    =  (char *)0x1848;//允许换卡无卡首地址

#define  Allow_NO_Card  0xC0         //
#define  Allow_Chg_Card 0x30         //

#define  NET_Up_Speed_BUF_SIZE 10
char REC_Up_Speed_BUF[NET_Up_Speed_BUF_SIZE];//接收允许换卡 无卡升速
char ANS_Up_Speed_BUF[NET_Up_Speed_BUF_SIZE];//回复允许换卡 无卡升速 

extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//转存ID号和命令编码                              
extern void Delayms(unsigned long int ms);//延时XX*1Ms
/*******************************************************************\
*	      函数名：GSM_Lock_Monitor             
*	      作用域：外部文件调用
*	      功能：  接收、处理平台数据  
*	      参数： 0x03==允许升速；0x00==解锁 
          格式： ID(4)+命令编码0x69(2)+锁车指令(2)
*	      返回值：返回锁车、解锁命令   

          001848~001849 ==72\73= 无卡允许==0xC0； 换卡允许===0x30

*
*	      修改历史：（每条详述）
\*******************************************************************/
char Allow_Up_Speed(void)//处理允许换卡 无卡升速
{
    unsigned int CAL_Up_Speed_CRC,NET_Up_Speed_CRC;//校验和
      
    NET_Up_Speed_CRC =   REC_Up_Speed_BUF[9]                                ;
    NET_Up_Speed_CRC =   NET_Up_Speed_CRC  <<8                              ;
    NET_Up_Speed_CRC =   NET_Up_Speed_CRC +   REC_Up_Speed_BUF[8]             ;
    CAL_Up_Speed_CRC =crc_modbus2((unsigned char *)REC_Up_Speed_BUF,
                                NET_Up_Speed_BUF_SIZE-2)                  ;//8
    if(CAL_Up_Speed_CRC==NET_Up_Speed_CRC)
    {
        Tran_ID_CmdNum(ANS_Up_Speed_BUF,0x45)                            ;//转存ID号和命令编码

        if((REC_Up_Speed_BUF[6]==Allow_NO_Card)&&(REC_Up_Speed_BUF[7]==Allow_Chg_Card))
        {
            Module_Status[2]      &=~ Allow_NO_Card;//取卡报警
            //GPS_GSM_System_Stu[2] &=~ Allow_NO_Card; 
            Module_Status[2]      &=~Allow_Chg_Card;//无卡报警
            //GPS_GSM_System_Stu[2] &=~Allow_Chg_Card;     

            ANS_Up_Speed_BUF[6]=Allow_NO_Card;
            ANS_Up_Speed_BUF[7]=Allow_Chg_Card;
            
            Flash_WriteChar(72,Allow_NO_Card|Allow_Chg_Card);
            _NOP();_NOP();_NOP();_NOP();_NOP();
            Flash_WriteChar(73,Allow_NO_Card|Allow_Chg_Card);
            
            Write_No_Chg_Card_Flash();//写换卡无卡Flash 
        }
        
        CAL_Up_Speed_CRC=crc_modbus2((unsigned char *)ANS_Up_Speed_BUF,
                                    (NET_Up_Speed_BUF_SIZE-2))          ;//8
        ANS_Up_Speed_BUF[8]    =   (char)(CAL_Up_Speed_CRC&0xFF)        ;
        ANS_Up_Speed_BUF[9]    =   (char)((CAL_Up_Speed_CRC>>8)&0xFF)   ;
    
        Up_Speed_Send_ERR_Flag   =    1                                 ;
        if(GSM_SendData(ANS_Up_Speed_BUF,NET_Up_Speed_BUF_SIZE))             //数据发送
        {
           Up_Speed_Send_ERR_Flag    =   0                              ;
           return 1                                                     ;
        }
    
        if(Up_Speed_Send_ERR_Flag)                                           //失败重发 
        {
           Delayms(200);//XX*1Ms延时
           Up_Speed_Send_ERR_Flag     =   0;
           if(GSM_SendData(ANS_Up_Speed_BUF,NET_Up_Speed_BUF_SIZE))
           {          
              Up_Speed_Send_ERR_Flag     =   0                          ;
              return 1                                                  ;
           }
        }
    }
    return 0                                                            ;
}



/**********************************************************************\
*	      函数名：RD_Up_Speed_FLASH             
*	      作用域：外部文件调用
*	      功能：  读取处理允许换卡 无卡升速FLASH
*	      参数：  
          格式： 
*	      返回值：
*
*	      修改历史：
\**********************************************************************/
void RD_Up_Speed_FLASH(void)//处理允许换卡 无卡升速
{
    if((((GPS_GSM_System_Stu[2]&Allow_NO_Card)==Allow_NO_Card)
         ||((GPS_GSM_System_Stu[2]&Allow_Chg_Card)==Allow_Chg_Card))&&
       (((*Up_Speed_Addr)==(Allow_NO_Card|Allow_Chg_Card))&&
         (*(Up_Speed_Addr+1)==(Allow_NO_Card|Allow_Chg_Card))))
    {
        Module_Status[2]       &=~ Allow_NO_Card;//取卡报警
        //GPS_GSM_System_Stu[2]  &=~ Allow_NO_Card; 
        
        Module_Status[2]       &=~Allow_Chg_Card;
        //GPS_GSM_System_Stu[2]  &=~Allow_Chg_Card;//无卡报警 
        
        //if(MSP430_POWER_ON_FLAG==0x11)
        //{
            //Write_No_Chg_Card_Flash();//写换卡无卡Flash 
            //Flash_WriteChar(72,Allow_NO_Card|Allow_Chg_Card);
            //_NOP();_NOP();_NOP();_NOP();_NOP();
            //Flash_WriteChar(73,Allow_NO_Card|Allow_Chg_Card);
        //}
    }
}



/**********************************************************************\
*	      函数名：RD_Up_Speed_FLASH             
*	      作用域：外部文件调用
*	      功能：  读取处理允许换卡 无卡升速FLASH
*	      参数：  
          格式： 
*	      返回值：
*
*	      修改历史：
\**********************************************************************/
void Ceshi_Allow_Up_Speed(void)//测试处理允许换卡 无卡升速
{
    unsigned char   LK_CNT;
    unsigned int CAL_Up_Speed_CRC;//校验和
    
    
    for(LK_CNT=0;LK_CNT<4;LK_CNT++)                         //设备ID转存
    {
       REC_Up_Speed_BUF[LK_CNT]=GPS_GSM_ID_Memory[LK_CNT]        ;
    }

    REC_Up_Speed_BUF[4]=0x54;
    REC_Up_Speed_BUF[5]=0x54;
    
    REC_Up_Speed_BUF[6]=Allow_NO_Card;
    REC_Up_Speed_BUF[7]=Allow_Chg_Card;    
    
    CAL_Up_Speed_CRC=crc_modbus2((unsigned char *)REC_Up_Speed_BUF,
                                (NET_Up_Speed_BUF_SIZE-2))          ;//8
    REC_Up_Speed_BUF[8]    =   (char)(CAL_Up_Speed_CRC&0xFF)        ;
    REC_Up_Speed_BUF[9]    =   (char)((CAL_Up_Speed_CRC>>8)&0xFF)   ;
}








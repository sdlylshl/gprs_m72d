
#include "msp430x54x.h"
#include "GSM.h"


char HAVE_SIM_NUM_FLASH    =0x00                                    ;//存储手机卡号FLASH 11=已经写入，无需再写；55==可以写入
char WR_SIM_NUM_FH_OK_FG   =0x00                                    ;//越界报警判断标志位 11=可以判断
char FLASH_SIM_NUM[20]                                              ;//FLASH转移到数组
char M72_SIM_NUM[20]                                                ;//FLASH转移到数组
#define  Send_SIM_Card_BUF_SIZE 49
char Send_SIM_Card_BUF[Send_SIM_Card_BUF_SIZE];//GPS终端锁车/解锁命令 回复



/*******************************************************************\
*	      函数名：READ_SIM_NUM_FH             
*	      作用域：外部文件调用
*	      功能：  存储手机卡号FLASH；判断是否更换手机卡  
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）
\*******************************************************************/
void READ_SIM_NUM_FH(void)                                           //存储手机卡号 
{
    unsigned char RED_Num                                           ;
    char* SIM_NUM   = (char *)0x1824                                ;//SIM卡号标志位首地址
    
    if(* (SIM_NUM)==0x11)
    {
        for(RED_Num=0;RED_Num<20;RED_Num++)                          //写SIM卡号
        {            
            FLASH_SIM_NUM[RED_Num]  =   *(1 + RED_Num + SIM_NUM)    ;
            Send_SIM_Card_BUF[6+RED_Num]=*(1 + RED_Num + SIM_NUM);
        }
        HAVE_SIM_NUM_FLASH  =0x11                                   ;//存储手机卡号FLASH 11=已经写入，无需再写
    }
    else
    {
        HAVE_SIM_NUM_FLASH  =0x55                                   ;//存储手机卡号FLASH 55==可以写入
    }
}


/*******************************************************************\
*	      函数名：SAVE_SIM_NUM_FH             
*	      作用域：外部文件调用
*	      功能：  存储手机卡号FLASH；判断是否更换手机卡  
*	      参数：  D_NUM==回显位置数
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）
\*******************************************************************/
char SAVE_SIM_NUM_FH(unsigned char CMD_LEN)                           //存储手机卡号 
{
    unsigned char SIM_Num                                           ;
    
    if((M72D_Query_RX_Buf[0]==0x0D&&M72D_Query_RX_Buf[1]==0x0A)
       &&(M72D_Query_RX_Buf[22]==0x0D&&M72D_Query_RX_Buf[23]==0x0A)
         &&(M72D_Query_RX_Buf[26]=='O'&&M72D_Query_RX_Buf[27]=='K'))     //存储手机卡号FLASH 55==可以写入
    {
        if(HAVE_SIM_NUM_FLASH==0x55)
        {
            Flash_WriteChar(36,0x11)                                ;//写标志位
            for(SIM_Num=0;SIM_Num<20;SIM_Num++)                          //写SIM卡号
            {    
               _NOP();_NOP();_NOP();_NOP();_NOP();
               Flash_WriteChar(37+SIM_Num,
                               M72D_Query_RX_Buf[CMD_LEN+SIM_Num])  ; 
            } 
            _NOP();_NOP();_NOP();_NOP();_NOP();
        }
        
        READ_SIM_NUM_FH();
        for(SIM_Num=0;SIM_Num<20;SIM_Num++)                          //写SIM卡号
         {            
               M72_SIM_NUM[SIM_Num]=M72D_Query_RX_Buf[CMD_LEN+SIM_Num]  ; 
         }
        return 1;
    }
    return 0;
}







/*******************************************************************\
*	      函数名：READ_SIM_NUM_FH             
*	      作用域：外部文件调用
*	      功能：  存储手机卡号FLASH；判断是否更换手机卡  
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：（每条详述）
\*******************************************************************/
void SIM_CHG_WARN(void)                            //更换手机卡报警 
{
    if(HAVE_SIM_NUM_FLASH==0x11)      
    {                                         //存储手机卡号
        if(Compare_String(M72_SIM_NUM,FLASH_SIM_NUM,20)==1)          //比较两个字符串是否相同
        {
             Module_Status[2] &= ~0x30;//模块状态信息
             GPS_GSM_System_Stu[2]&= ~0x30;
             Write_No_Chg_Card_Flash();//写换卡无卡Flash 
        }
        else
        {
             Module_Status[2] |=0x30;
             GPS_GSM_System_Stu[2]|=0x30;//GPS状态存储 
             Write_No_Chg_Card_Flash();//写换卡无卡Flash 
        }     
    }
}


unsigned long int SIM_Card_Work_Cnt;//发发送手机卡号时间计时
char SIM_Card_Work_Flag;//上电手机卡号标志
char SIM_Card_ER_Flag;//发送手机卡号失败标志 重发

/*******************************************************************\
*	      函数名：Send_SIM_Card_NUM             
*	      作用域：外部文件调用
*	      功能：  开机发送手机卡号  
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：2013.12.13 创建
\*******************************************************************/
void Send_SIM_Card_NUM(void)                            //开机发送手机卡号
{

    unsigned char   SIM_CNT                                              ;
    unsigned int CAL_SIM_CRC;//校验和
      
   if((MSP430_POWER_ON_FLAG==0x11)&&((SIM_Card_Work_Flag==0xAA)||(SIM_Card_ER_Flag==1))&&(UDP_Built_flag==0x11)
      &&(SIM_Card_Work_Cnt>MSP_A0_Min_3)&&(Heart_Beat_Count>SECD_10)&&(Heart_Beat_Count<SECD_20))
    {
       SIM_Card_Work_Cnt=0;
       SIM_Card_Work_Flag=0x55;
       
       Tran_ID_CmdNum(Send_SIM_Card_BUF,0x5A)                            ;//转存ID号和命令编码

       READ_SIM_NUM_FH();//存储手机卡号 
       SIM_CHG_WARN()  ;//检测手机卡报警
       
       if(GPS_GSM_System_Stu[2] & 0x30)
       {
           Send_SIM_Card_BUF[26]=0x22;
           for(SIM_CNT=0;SIM_CNT<20;SIM_CNT++) //换卡卡号转存
           {
              Send_SIM_Card_BUF[27+SIM_CNT]=M72_SIM_NUM[SIM_CNT];
           }
       }
       
       if(!(GPS_GSM_System_Stu[2] & 0x30))
       {
           Send_SIM_Card_BUF[26]=0x11;
           for(SIM_CNT=0;SIM_CNT<20;SIM_CNT++)//设备ID转存
           {
              Send_SIM_Card_BUF[27+SIM_CNT]=0x00;
           }
       }
       
       CAL_SIM_CRC=crc_modbus2((unsigned char *)Send_SIM_Card_BUF,
                                    (Send_SIM_Card_BUF_SIZE-2));//47
       Send_SIM_Card_BUF[47]    =   (char)(CAL_SIM_CRC&0xFF);
       Send_SIM_Card_BUF[48]    =   (char)((CAL_SIM_CRC>>8)&0xFF);
       
       SIM_Card_ER_Flag    =   1                                   ;
       if(GSM_SendData(Send_SIM_Card_BUF,Send_SIM_Card_BUF_SIZE))
        {                                                            //发送软件版本号
           SIM_Card_ER_Flag   =0                                   ;
        }
    }
    if(SIM_Card_ER_Flag)                                    
    {
       Delayms(200);
       SIM_Card_ER_Flag   =0                                    ;
       if(GSM_SendData(Send_SIM_Card_BUF,Send_SIM_Card_BUF_SIZE))
       {          
          SIM_Card_ER_Flag   =0                                    ;
       }
    }
}



char Change_SIM_Card_Flag;//上电更换手机卡号标志
/*******************************************************************\
*	      函数名：Change_SIM_Card_NUM             
*	      作用域：外部文件调用
*	      功能：  允许更换手机卡号  
*	      参数：  
          格式： 
*	      返回值：  
*
*	      修改历史：2013.12.13 创建
\*******************************************************************/
 void Change_SIM_Card_NUM(void)                            //允许更换手机卡号
{
    unsigned char   Change_CNT;
    if(((ARM_DATA_STU&0x01)==0x01)&&(Change_SIM_Card_Flag==0xAA)&&((GSM_MCU[13]&0x03)==0x03))
    {
        GSM_MCU[13]&=~0x03;
        Change_SIM_Card_Flag=0x55;
        Flash_WriteChar(36,0x00)                                ;//写标志位
        for(Change_CNT=0;Change_CNT<20;Change_CNT++)                         
        {            
           _NOP();_NOP();_NOP();_NOP();_NOP();
           Flash_WriteChar(37+Change_CNT,0x00); 
        }
        _NOP();_NOP();_NOP();_NOP();_NOP();
        READ_SIM_NUM_FH();
    }
}


unsigned int No_Card_Warning_Times=0;//换卡无卡报警次数
char* No_Card_Head_Flash=(char *)0x1846;//开盒报警首地址001844 
char* No_Card_Warning_Times_Head_Flash=(char *)0x1858;//换卡无卡报警次数首地址88、89
/*******************************************************************\
*	      函数名：Write_No_Chg_Card_Flash             
*	      作用域：外部文件调用
*	      功能：  写换卡 无卡 Flash 
*	      参数：  70 71
          001846~001847 ==70\71= 无卡==0xC0； 换卡===0x30
          格式： 
*	      返回值：  
*
*	      修改历史：2014.1.11 创建
\*******************************************************************/

 void Write_No_Chg_Card_Flash(void)//写换卡无卡Flash 
{
   unsigned int No_Card_Times=0;//换卡无卡报警次数
   
   
   if(((GPS_GSM_System_Stu[2]&0xC0)==0x00)&&(!(*No_Card_Head_Flash==0x00))&&((*(No_Card_Head_Flash+1)!=0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0));//取卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x00)&&((*No_Card_Head_Flash!=0x00))&&((*(No_Card_Head_Flash+1)!=0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30));//11 换卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30));
       _NOP();_NOP();_NOP();_NOP();_NOP();

   }

   
   if(((GPS_GSM_System_Stu[2]&0xC0)==0xC0)&&((*No_Card_Head_Flash!=0xC0))&&((*(No_Card_Head_Flash+1)!=0xC0)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0)|(*No_Card_Head_Flash));//取卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       
       
       No_Card_Times=No_Card_Warning_Times++;
       Flash_WriteChar(89,(No_Card_Times));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(88,(No_Card_Times>>8));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x30)&&((*No_Card_Head_Flash!=0x30))&&((*(No_Card_Head_Flash+1)!=0x30)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30)|(*No_Card_Head_Flash));//11 换卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       
       
       No_Card_Times=No_Card_Warning_Times++;
       Flash_WriteChar(89,(No_Card_Times));
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(88,(No_Card_Times>>8));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
   
   Read_No_Chg_Card_Flash();
   
  /* if(((GPS_GSM_System_Stu[2]&0xC0)==0xC0)&&((*No_Card_Head_Flash!=0xC0))&&(!(*(No_Card_Head_Flash+1)==0xC0)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0));//取卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x30)&&(!(*No_Card_Head_Flash==0x30))&&(!(*(No_Card_Head_Flash+1)==0x30)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30));//11 换卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
   */
}


/* void Write_No_Chg_Card_Flash(void)//写换卡无卡Flash 
{
  
   if(((GPS_GSM_System_Stu[2]&0xC0)==0x00)&&(!(*No_Card_Head_Flash==0x00))&&(!(*(No_Card_Head_Flash+1)==0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0)|(*No_Card_Head_Flash));//取卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x00)&&(!(*No_Card_Head_Flash==0x00))&&(!(*(No_Card_Head_Flash+1)==0x00)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30)|(*No_Card_Head_Flash));//11 换卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
  
   if(((GPS_GSM_System_Stu[2]&0xC0)==0xC0)&&(!(*No_Card_Head_Flash==0xC0))&&(!(*(No_Card_Head_Flash+1)==0xC0)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0xC0)|(*No_Card_Head_Flash));//取卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0xC0)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if(((GPS_GSM_System_Stu[2]&0x30)==0x30)&&(!(*No_Card_Head_Flash==0x30))&&(!(*(No_Card_Head_Flash+1)==0x30)))
   {
       Flash_WriteChar(70,(GPS_GSM_System_Stu[2]&0x30)|(*No_Card_Head_Flash));//11 换卡报警
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(71,(GPS_GSM_System_Stu[2]&0x30)|(*(No_Card_Head_Flash+1)));
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
   

   Read_No_Chg_Card_Flash();
}

*/
/*******************************************************************\
*	      函数名：Read_No_Chg_Card_Flash             
*	      作用域：外部文件调用
*	      功能：  写换卡 无卡 Flash 
*	      参数：  70 71
          001846~001847 ==70\71= 无卡==0xC0； 换卡===0x30
          格式： 
*	      返回值：  
*
*	      修改历史：2014.1.11 创建
\*******************************************************************/
 void Read_No_Chg_Card_Flash(void)//读换卡无卡Flash 
{  
   if(((*No_Card_Head_Flash&0xC0)==0x00)&&((*(No_Card_Head_Flash+1)&0xC0)==0x00))
   {
        Module_Status[2] &=~ 0XC0;//取卡报警
        GPS_GSM_System_Stu[2] &=~ 0XC0; 
   }

   if(((*No_Card_Head_Flash&0x30)==0x00)&&((*(No_Card_Head_Flash+1)&0x30)==0x00))
   {
        Module_Status[2]&=~0x30;
        GPS_GSM_System_Stu[2]&=~0x30;//无卡报警    
   }
  
   if(((*No_Card_Head_Flash&0xC0)==0xC0)&&((*(No_Card_Head_Flash+1)&0xC0)==0xC0))
   {
        Module_Status[2] |= 0XC0;//取卡报警
        GPS_GSM_System_Stu[2] |= 0XC0; 
        GSM_Send_MCU_Buf[99]=* No_Card_Warning_Times_Head_Flash;
        GSM_Send_MCU_Buf[100]=*(No_Card_Warning_Times_Head_Flash+1);
   }

   if(((*No_Card_Head_Flash&0x30)==0x30)&&((*(No_Card_Head_Flash+1)&0x30)==0x30))
   {
        Module_Status[2] |=0x30;
        GPS_GSM_System_Stu[2]|=0x30;//GPS状态存储     
        GSM_Send_MCU_Buf[99]=* No_Card_Warning_Times_Head_Flash;
        GSM_Send_MCU_Buf[100]=*(No_Card_Warning_Times_Head_Flash+1);
   }
   
}























